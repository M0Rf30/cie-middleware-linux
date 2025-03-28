#include "PKCS11/CIEP11Template.h"

#include <cryptopp/asn.h>
#include <cryptopp/cryptlib.h>
#include <stdio.h>

#include <memory>

#include "CSP/IAS.h"
#include "Crypto/AES.h"
#include "Crypto/ASNParser.h"
#include "PCSC/CardLocker.h"
#include "PCSC/PCSC.h"
#include "Util/CryptoppUtils.h"

extern CLog Log;

#include "LOGGER/Logger.h"

using namespace CryptoPP;
using namespace lcp;
using namespace CieIDLogger;

void notifyPINLocked();
void notifyPINWrong(int trials);

void GetCertInfo(CryptoPP::BufferedTransformation &certin, std::string &serial,
                 CryptoPP::BufferedTransformation &issuer,
                 CryptoPP::BufferedTransformation &subject,
                 std::string &notBefore, std::string &notAfter,
                 CryptoPP::Integer &mod, CryptoPP::Integer &pubExp);

int TokenTransmitCallback(CSlot *data, BYTE *apdu, DWORD apduSize, BYTE *resp,
                          DWORD *respSize) {
  if (apduSize == 2) {
    WORD code = *(WORD *)apdu;
    if (code == 0xfffd) {
      long bufLen = *respSize;
      *respSize = sizeof(data->hCard) + 2;
      CryptoPP::memcpy_s(resp, bufLen, &data->hCard, sizeof(data->hCard));
      resp[sizeof(data->hCard)] = 0;
      resp[sizeof(data->hCard) + 1] = 0;

      return SCARD_S_SUCCESS;
    } else if (code == 0xfffe) {
      DWORD protocol = 0;
      auto ris =
          SCardReconnect(data->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx,
                         SCARD_UNPOWER_CARD, &protocol);

      if (ris == SCARD_S_SUCCESS) {
        SCardBeginTransaction(data->hCard);
        *respSize = 2;
        resp[0] = 0x90;
        resp[1] = 0x00;
      }
      return ris;
    } else if (code == 0xffff) {
      DWORD protocol = 0;
      auto ris = SCardReconnect(data->hCard, SCARD_SHARE_SHARED,
                                SCARD_PROTOCOL_Tx, SCARD_RESET_CARD, &protocol);
      if (ris == SCARD_S_SUCCESS) {
        SCardBeginTransaction(data->hCard);
        *respSize = 2;
        resp[0] = 0x90;
        resp[1] = 0x00;
      }
      return ris;
    }
  }

  auto ris = SCardTransmit(data->hCard, SCARD_PCI_T1, apdu, apduSize, NULL,
                           resp, respSize);
  if (ris == SCARD_W_RESET_CARD || ris == SCARD_W_UNPOWERED_CARD) {
    LOG_ERROR("TokenTransmitCallback - Card reset error: %x", ris);

    DWORD protocol = 0;
    ris = SCardReconnect(data->hCard, SCARD_SHARE_SHARED, SCARD_PROTOCOL_Tx,
                         SCARD_LEAVE_CARD, &protocol);
    if (ris != SCARD_S_SUCCESS) {
      LOG_ERROR("TokenTransmitCallback - Errore reconnect %d", ris);
    } else {
      ris = SCardTransmit(data->hCard, SCARD_PCI_T1, apdu, apduSize, NULL, resp,
                          respSize);
    }
  }

  if (ris != SCARD_S_SUCCESS) {
    LOG_ERROR("TokenTransmitCallback - APDU transmission error: %x", ris);
  }

  return ris;
}

class CIEData {
 public:
  CK_USER_TYPE userType;
  CAES aesKey;
  CToken token;
  bool init;
  CIEData(CSlot *slot, ByteArray atr)
      : slot(*slot),
        ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atr) {
    ByteDynArray key(32);
    ByteDynArray iv(16);
    aesKey.Init(key.random(), iv.random());
    token.setTransmitCallbackData(slot);
    userType = -1;
    init = false;
  }
  CSlot &slot;
  IAS ias;
  std::shared_ptr<CP11PublicKey> pubKey;
  std::shared_ptr<CP11PrivateKey> privKey;
  std::shared_ptr<CP11Certificate> cert;
  ByteDynArray SessionPIN;
};

void CIEtemplateInitLibrary(class CCardTemplate &Template, void *templateData) {
  return;
}
void CIEtemplateInitCard(void *&pTemplateData, CSlot &pSlot) {
  init_func ByteArray ATR;
  pSlot.GetATR(ATR);

  pTemplateData = new CIEData(&pSlot, ATR);
}
void CIEtemplateFinalCard(void *pTemplateData) {
  if (pTemplateData) delete (CIEData *)pTemplateData;
}

ByteArray SkipZero(ByteArray &ba) {
  for (DWORD i = 0; i < ba.size(); i++) {
    if (ba[i] != 0) return ba.mid(i);
  }
  return ByteArray();
}

void CIEtemplateInitSession(void *pTemplateData) {
  CIEData *cie = (CIEData *)pTemplateData;

  if (!cie->init) {
    ByteDynArray certRaw;
    cie->slot.Connect();
    {
      safeConnection faseConn(cie->slot.hCard);
      CCardLocker lockCard(cie->slot.hCard);
      cie->ias.SetCardContext(&cie->slot);
      cie->ias.SelectAID_IAS();
      cie->ias.ReadPAN();

      ByteDynArray resp;
      cie->ias.SelectAID_CIE();
      cie->ias.ReadDappPubKey(resp);
      cie->ias.InitEncKey();
      cie->ias.GetCertificate(certRaw, true);
    }

    CK_BBOOL vtrue = TRUE;
    CK_BBOOL vfalse = FALSE;
    BYTE labelCert[] = "CIE Certificate";
    BYTE labelPriv[] = "CIE Private Key";
    BYTE labelPub[] = "CIE Public Key";
    CK_BYTE objId = 0x01;  // For simplicity we only need one (numbered '1')

    cie->pubKey = std::make_shared<CP11PublicKey>(cie);
    cie->privKey = std::make_shared<CP11PrivateKey>(cie);
    cie->cert = std::make_shared<CP11Certificate>(cie);

    cie->pubKey->addAttribute(CKA_LABEL, VarToByteArray(labelPub));
    cie->pubKey->addAttribute(CKA_ID, VarToByteArray(objId));
    cie->pubKey->addAttribute(CKA_PRIVATE, VarToByteArray(vfalse));
    cie->pubKey->addAttribute(CKA_TOKEN, VarToByteArray(vtrue));
    cie->pubKey->addAttribute(CKA_VERIFY, VarToByteArray(vtrue));
    CK_KEY_TYPE keyrsa = CKK_RSA;
    cie->pubKey->addAttribute(CKA_KEY_TYPE, VarToByteArray(keyrsa));

    cie->privKey->addAttribute(CKA_LABEL, VarToByteArray(labelPriv));
    cie->privKey->addAttribute(CKA_ID, VarToByteArray(objId));
    cie->privKey->addAttribute(CKA_PRIVATE, VarToByteArray(vtrue));
    cie->privKey->addAttribute(CKA_TOKEN, VarToByteArray(vtrue));
    cie->privKey->addAttribute(CKA_KEY_TYPE, VarToByteArray(keyrsa));

    cie->privKey->addAttribute(CKA_SIGN, VarToByteArray(vtrue));

    cie->cert->addAttribute(CKA_LABEL, VarToByteArray(labelCert));
    cie->cert->addAttribute(CKA_ID, VarToByteArray(objId));
    cie->cert->addAttribute(CKA_PRIVATE, VarToByteArray(vfalse));
    cie->cert->addAttribute(CKA_TOKEN, VarToByteArray(vtrue));

    CK_CERTIFICATE_TYPE certx509 = CKC_X_509;
    cie->cert->addAttribute(CKA_CERTIFICATE_TYPE, VarToByteArray(certx509));

    CryptoPP::ByteQueue certin;
    certin.Put(certRaw.data(), certRaw.size());

    std::string serial;
    CryptoPP::ByteQueue issuer;
    CryptoPP::ByteQueue subject;
    std::string notBefore;
    std::string notAfter;
    CryptoPP::Integer mod;
    CryptoPP::Integer pubExp;

    GetCertInfo(certin, serial, issuer, subject, notBefore, notAfter, mod,
                pubExp);

    ByteDynArray modulus(mod.ByteCount());
    mod.Encode(modulus.data(), modulus.size());

    ByteDynArray publicExponent(pubExp.ByteCount());
    pubExp.Encode(publicExponent.data(), publicExponent.size());

    CK_LONG keySizeBits = (CK_LONG)modulus.size() * 8;

    cie->pubKey->addAttribute(CKA_MODULUS, modulus);
    cie->pubKey->addAttribute(CKA_PUBLIC_EXPONENT, publicExponent);
    cie->pubKey->addAttribute(CKA_MODULUS_BITS, VarToByteArray(keySizeBits));

    cie->privKey->addAttribute(CKA_MODULUS, modulus);
    cie->privKey->addAttribute(CKA_PUBLIC_EXPONENT, publicExponent);

    ByteDynArray issuerBa(issuer.CurrentSize());
    issuer.Get(issuerBa.data(), issuerBa.size());

    ByteDynArray subjectBa(subject.CurrentSize());
    subject.Get(subjectBa.data(), subjectBa.size());

    cie->cert->addAttribute(CKA_ISSUER, issuerBa);
    cie->cert->addAttribute(CKA_SERIAL_NUMBER,
                            ByteArray((BYTE *)serial.c_str(), serial.size()));
    cie->cert->addAttribute(CKA_SUBJECT, subjectBa);

    CK_DATE start, end;

    char sFrom[8], sTo[8];
    memcpy(sFrom, notBefore.c_str(), 8);
    memcpy(sTo, notAfter.c_str(), 8);

    VarToByteArray(start.year).copy(ByteArray((BYTE *)sFrom, 4));
    VarToByteArray(start.month).copy(ByteArray((BYTE *)&sFrom[4], 2));
    VarToByteArray(start.day).copy(ByteArray((BYTE *)&sFrom[6], 2));

    VarToByteArray(end.year).copy(ByteArray((BYTE *)sTo, 4));
    VarToByteArray(end.month).copy(ByteArray((BYTE *)&sTo[4], 2));
    VarToByteArray(end.day).copy(ByteArray((BYTE *)&sTo[6], 2));

    cie->cert->addAttribute(CKA_START_DATE, VarToByteArray(start));
    cie->cert->addAttribute(CKA_END_DATE, VarToByteArray(end));

    // add to the object
    size_t len = GetASN1DataLenght(certRaw);
    cie->cert->addAttribute(CKA_VALUE, certRaw.left(len));

    cie->slot.AddP11Object(cie->pubKey);
    cie->slot.AddP11Object(cie->privKey);
    cie->slot.AddP11Object(cie->cert);

    cie->init = true;
  }
}
void CIEtemplateFinalSession(void *pTemplateData) {}

bool CIEtemplateMatchCard(CSlot &pSlot) {
  init_func CToken token;

  pSlot.Connect();
  {
    safeConnection faseConn(pSlot.hCard);
    ByteArray ATR;
    pSlot.GetATR(ATR);
    token.setTransmitCallback(
        (CToken::TokenTransmitCallback)TokenTransmitCallback, &pSlot);
    IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, ATR);
    ias.SetCardContext(&pSlot);
    {
      safeTransaction trans(faseConn, SCARD_LEAVE_CARD);
      ias.SelectAID_IAS();
      ias.ReadPAN();
    }
    return true;
  }
}

ByteDynArray CIEtemplateGetSerial(CSlot &pSlot) {
  init_func CToken token;

  pSlot.Connect();
  {
    safeConnection faseConn(pSlot.hCard);
    CCardLocker lockCard(pSlot.hCard);
    ByteArray ATR;
    pSlot.GetATR(ATR);
    IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, ATR);
    ias.SetCardContext(&pSlot);
    ias.SelectAID_IAS();
    ias.ReadPAN();
    std::string numSerial;
    dumpHexData(ias.PAN.mid(5, 6), numSerial, false);
    return ByteArray((BYTE *)numSerial.c_str(), numSerial.length());
  }
}
void CIEtemplateGetModel(CSlot &pSlot, std::string &szModel) {
  szModel = "CIE 3.0";
}
void CIEtemplateGetTokenFlags(CSlot &pSlot, CK_FLAGS &dwFlags) {
  dwFlags = CKF_LOGIN_REQUIRED | CKF_USER_PIN_INITIALIZED |
            CKF_TOKEN_INITIALIZED | CKF_REMOVABLE_DEVICE;
}

void CIEtemplateLogin(void *pTemplateData, CK_USER_TYPE userType,
                      ByteArray &Pin) {
  init_func CToken token;
  CIEData *cie = (CIEData *)pTemplateData;

  cie->SessionPIN.clear();
  cie->userType = -1;

  cie->slot.Connect();
  cie->ias.SetCardContext(&cie->slot);
  cie->ias.token.Reset();
  {
    safeConnection safeConn(cie->slot.hCard);
    CCardLocker lockCard(cie->slot.hCard);

    cie->ias.SelectAID_IAS();
    cie->ias.SelectAID_CIE();
    cie->ias.InitDHParam();

    if (cie->ias.DappPubKey.isEmpty()) {
      ByteDynArray DappKey;
      cie->ias.ReadDappPubKey(DappKey);
    }

    cie->ias.InitExtAuthKeyParam();
    // faccio lo scambio di chiavi DH
    if (cie->ias.Callback != nullptr)
      cie->ias.Callback(1, "DiffieHellman", cie->ias.CallbackData);
    cie->ias.DHKeyExchange();
    // DAPP
    if (cie->ias.Callback != nullptr)
      cie->ias.Callback(2, "DAPP", cie->ias.CallbackData);
    cie->ias.DAPP();
    // verifica PIN
    StatusWord sw;
    if (cie->ias.Callback != nullptr)
      cie->ias.Callback(3, "Verify PIN", cie->ias.CallbackData);
    if (userType == CKU_USER) {
      ByteDynArray FullPIN;
      cie->ias.GetFirstPIN(FullPIN);
      FullPIN.append(Pin);
      sw = cie->ias.VerifyPIN(FullPIN);
    } else if (userType == CKU_SO) {
      sw = cie->ias.VerifyPUK(Pin);
    } else {
      throw p11_error(CKR_ARGUMENTS_BAD);
    }

    if (sw == 0x6983) {
      if (userType == CKU_USER) {
        notifyPINLocked();
        throw p11_error(CKR_PIN_LOCKED);
      }
    }
    if (sw >= 0x63C0 && sw <= 0x63CF) {
      int attemptsRemaining = sw - 0x63C0;
      notifyPINWrong(attemptsRemaining);
      throw p11_error(CKR_PIN_INCORRECT);
    }
    if (sw == 0x6700) {
      notifyPINWrong(-1);
      throw p11_error(CKR_PIN_INCORRECT);
    }
    if (sw == 0x6300) {
      notifyPINWrong(-1);
      throw p11_error(CKR_PIN_INCORRECT);
    }
    if (sw != 0x9000) {
      throw scard_error(sw);
    }

    cie->SessionPIN = cie->aesKey.Encode(Pin);
    cie->userType = userType;
  }
}
void CIEtemplateLogout(void *pTemplateData, CK_USER_TYPE userType) {
  CIEData *cie = (CIEData *)pTemplateData;
  cie->userType = -1;
  cie->SessionPIN.clear();
}
void CIEtemplateReadObjectAttributes(void *pCardTemplateData,
                                     CP11Object *pObject) {}
void CIEtemplateSign(void *pCardTemplateData, CP11PrivateKey *pPrivKey,
                     ByteArray &baSignBuffer, ByteDynArray &baSignature,
                     CK_MECHANISM_TYPE mechanism, bool bSilent) {
  init_func CToken token;
  CIEData *cie = (CIEData *)pCardTemplateData;
  if (cie->userType == CKU_USER) {
    ByteDynArray Pin;
    cie->slot.Connect();
    cie->ias.SetCardContext(&cie->slot);
    cie->ias.token.Reset();
    {
      safeConnection safeConn(cie->slot.hCard);
      CCardLocker lockCard(cie->slot.hCard);

      Pin = cie->aesKey.Decode(cie->SessionPIN);
      cie->ias.SelectAID_IAS();
      cie->ias.SelectAID_CIE();
      cie->ias.DHKeyExchange();
      cie->ias.DAPP();

      ByteDynArray FullPIN;
      cie->ias.GetFirstPIN(FullPIN);
      FullPIN.append(Pin);
      if (cie->ias.VerifyPIN(FullPIN) != 0x9000)
        throw p11_error(CKR_PIN_INCORRECT);
      cie->ias.Sign(baSignBuffer, baSignature);
    }
  }
}

void CIEtemplateInitPIN(void *pCardTemplateData, ByteArray &baPin) {
  init_func CToken token;
  CIEData *cie = (CIEData *)pCardTemplateData;
  if (cie->userType == CKU_SO) {
    // posso usarla solo se sono loggato come so
    ByteDynArray Pin;
    cie->slot.Connect();
    cie->ias.SetCardContext(&cie->slot);
    cie->ias.token.Reset();
    {
      safeConnection safeConn(cie->slot.hCard);
      CCardLocker lockCard(cie->slot.hCard);

      Pin = cie->aesKey.Decode(cie->SessionPIN);
      cie->ias.SelectAID_IAS();
      cie->ias.SelectAID_CIE();

      cie->ias.DHKeyExchange();
      cie->ias.DAPP();
      if (cie->ias.VerifyPUK(Pin) != 0x9000) throw p11_error(CKR_PIN_INCORRECT);

      if (cie->ias.UnblockPIN() != 0x9000) throw p11_error(CKR_GENERAL_ERROR);

      ByteDynArray changePIN;
      cie->ias.GetFirstPIN(changePIN);
      changePIN.append(baPin);

      if (cie->ias.ChangePIN(changePIN) != 0x9000)
        throw p11_error(CKR_GENERAL_ERROR);
    }
  } else {
    throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
  }
}

void CIEtemplateSetPIN(void *pCardTemplateData, ByteArray &baOldPin,
                       ByteArray &baNewPin, CK_USER_TYPE User) {
  init_func CToken token;
  CIEData *cie = (CIEData *)pCardTemplateData;
  if (cie->userType != CKU_SO) {
    // posso usarla sia se sono loggato come user sia se non sono loggato
    ByteDynArray Pin;
    cie->slot.Connect();
    cie->ias.SetCardContext(&cie->slot);
    cie->ias.token.Reset();
    {
      safeConnection safeConn(cie->slot.hCard);
      CCardLocker lockCard(cie->slot.hCard);
      cie->ias.SelectAID_IAS();
      if (cie->userType != CKU_USER) cie->ias.InitDHParam();
      cie->ias.SelectAID_CIE();

      if (cie->userType != CKU_USER) {
        cie->ias.ReadPAN();
        ByteDynArray resp;
        cie->ias.ReadDappPubKey(resp);
      }

      cie->ias.DHKeyExchange();
      cie->ias.DAPP();
      ByteDynArray oldPIN, newPIN;
      cie->ias.GetFirstPIN(oldPIN);
      newPIN = oldPIN;
      oldPIN.append(baOldPin);
      newPIN.append(baNewPin);

      if (cie->ias.VerifyPIN(oldPIN) != 0x9000)
        throw p11_error(CKR_PIN_INCORRECT);
      if (cie->ias.ChangePIN(oldPIN, newPIN) != 0x9000)
        throw p11_error(CKR_GENERAL_ERROR);
    }
  } else {
    throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
  }
}

void CIEtemplateSignRecover(void *pCardTemplateData, CP11PrivateKey *pPrivKey,
                            ByteArray &baSignBuffer, ByteDynArray &baSignature,
                            CK_MECHANISM_TYPE mechanism, bool bSilent) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
void CIEtemplateDecrypt(void *pCardTemplateData, CP11PrivateKey *pPrivKey,
                        ByteArray &baEncryptedData, ByteDynArray &baData,
                        CK_MECHANISM_TYPE mechanism, bool bSilent) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
void CIEtemplateGenerateRandom(void *pCardTemplateData,
                               ByteArray &baRandomData) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
CK_ULONG CIEtemplateGetObjectSize(void *pCardTemplateData,
                                  CP11Object *pObject) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
void CIEtemplateSetKeyPIN(void *pTemplateData, CP11Object *pObject,
                          ByteArray &Pin) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
void CIEtemplateSetAttribute(void *pTemplateData, CP11Object *pObject,
                             CK_ATTRIBUTE_PTR pTemplate, CK_ULONG ulCount) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
std::shared_ptr<CP11Object> CIEtemplateCreateObject(void *pTemplateData,
                                                    CK_ATTRIBUTE_PTR pTemplate,
                                                    CK_ULONG ulCount) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
void CIEtemplateDestroyObject(void *pTemplateData, CP11Object &Object) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
std::shared_ptr<CP11Object> CIEtemplateGenerateKey(void *pCardTemplateData,
                                                   CK_MECHANISM_PTR pMechanism,
                                                   CK_ATTRIBUTE_PTR pTemplate,
                                                   CK_ULONG ulCount) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}
void CIEtemplateGenerateKeyPair(void *pCardTemplateData,
                                CK_MECHANISM_PTR pMechanism,
                                CK_ATTRIBUTE_PTR pPublicKeyTemplate,
                                CK_ULONG ulPublicKeyAttributeCount,
                                CK_ATTRIBUTE_PTR pPrivateKeyTemplate,
                                CK_ULONG ulPrivateKeyAttributeCount,
                                std::shared_ptr<CP11Object> &pPublicKey,
                                std::shared_ptr<CP11Object> &pPrivateKey) {
  throw p11_error(CKR_FUNCTION_NOT_SUPPORTED);
}

/**
 * Reads an X.509 v3 certificate from certin, extracts the subjectPublicKeyInfo
 * structure (which is one way PK_Verifiers can get their key material) and
 * writes it to keyout
 *
 * @throws CryptoPP::BERDecodeError
 */

void GetPublicKeyFromCert(CryptoPP::BufferedTransformation &certin,
                          CryptoPP::BufferedTransformation &keyout,
                          CryptoPP::BufferedTransformation &issuer,

                          Integer &serial) {
  BERSequenceDecoder x509Cert(certin);
  BERSequenceDecoder tbsCert(x509Cert);

  // consume the context tag on the version
  BERGeneralDecoder context(tbsCert, 0xa0);
  word32 ver;

  // only want a v3 cert
  BERDecodeUnsigned<word32>(context, ver, INTEGER, 2, 2);

  // serialNumber         CertificateSerialNumber,
  serial.BERDecode(tbsCert);

  // signature            AlgorithmIdentifier,
  BERSequenceDecoder signature(tbsCert);
  signature.SkipAll();

  // issuer               Name,
  BERSequenceDecoder issuerName(tbsCert);
  issuerName.CopyTo(issuer);
  issuerName.SkipAll();

  // validity             Validity,
  BERSequenceDecoder validity(tbsCert);
  validity.SkipAll();

  // subject              Name,
  BERSequenceDecoder subjectName(tbsCert);
  subjectName.SkipAll();

  // subjectPublicKeyInfo SubjectPublicKeyInfo,
  BERSequenceDecoder spki(tbsCert);
  DERSequenceEncoder spkiEncoder(keyout);

  spki.CopyTo(spkiEncoder);
  spkiEncoder.MessageEnd();

  spki.SkipAll();
  tbsCert.SkipAll();
  x509Cert.SkipAll();
}

void GetCertInfo(CryptoPP::BufferedTransformation &certin, std::string &serial,
                 CryptoPP::BufferedTransformation &issuer,
                 CryptoPP::BufferedTransformation &subject,
                 std::string &notBefore, std::string &notAfter,
                 CryptoPP::Integer &mod, CryptoPP::Integer &pubExp) {
  BERSequenceDecoder cert(certin);

  BERSequenceDecoder toBeSignedCert(cert);

  // consume the context tag on the version
  BERGeneralDecoder context(toBeSignedCert, 0xa0);
  word32 ver;

  // only want a v3 cert
  BERDecodeUnsigned<word32>(context, ver, INTEGER, 2, 2);

  serial = CryptoppUtils::Cert::ReadIntegerAsString(toBeSignedCert);

  // algorithmId
  CryptoppUtils::Cert::SkipNextSequence(toBeSignedCert);

  // issuer               Name,
  BERSequenceDecoder issuerName(toBeSignedCert);
  DERSequenceEncoder issuerEncoder(issuer);
  issuerName.CopyTo(issuerEncoder);
  issuerEncoder.MessageEnd();
  issuerName.SkipAll();

  // validity
  CryptoppUtils::Cert::ReadDateTimeSequence(toBeSignedCert, notBefore,
                                            notAfter);

  // subject
  BERSequenceDecoder subjectName(toBeSignedCert);
  DERSequenceEncoder subjectEncoder(subject);
  subjectName.CopyTo(subjectEncoder);
  subjectEncoder.MessageEnd();

  subjectName.SkipAll();

  // Public key
  CryptoPP::BERSequenceDecoder publicKey(toBeSignedCert);
  {
    CryptoPP::BERSequenceDecoder ident(publicKey);
    ident.SkipAll();
    CryptoPP::BERGeneralDecoder key(publicKey, CryptoPP::BIT_STRING);
    key.Skip(1);  // Must skip (possibly a bug in Crypto++)
    CryptoPP::BERSequenceDecoder keyPair(key);

    mod.BERDecode(keyPair);
    pubExp.BERDecode(keyPair);
  }

  publicKey.SkipAll();
  toBeSignedCert.SkipAll();
  cert.SkipAll();
}
