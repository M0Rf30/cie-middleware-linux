#include "CSP/IAS.h"

#include <cryptopp/asn.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/misc.h>
#include <cryptopp/queue.h>

#include "Crypto/AES.h"
#include "Crypto/ASNParser.h"
#include "Crypto/DES3.h"
#include "Crypto/MAC.h"
#include "Crypto/RSA.h"
#include "Crypto/SHA1.h"
#include "Crypto/sha256.h"
#include "Crypto/sha512.h"
#include "LOGGER/Logger.h"
#include "Util/CacheLib.h"
#include "Util/ModuleInfo.h"

using namespace CieIDLogger;

#define CIE_KEY_DH_ID 0x81
#define CIE_KEY_ExtAuth_ID 0x84
#define CIE_PIN_ID 0x81
#define CIE_PUK_ID 0x82
#define CIE_KEY_Sign_ID 0x81

#include <stdlib.h>
#include <unistd.h>

extern CLog Log;

extern CModuleInfo moduleInfo;
extern ByteArray SkipZero(ByteArray &ba);

void GetPublicKeyFromCert(CryptoPP::BufferedTransformation &certin,
                          CryptoPP::BufferedTransformation &keyout,
                          CryptoPP::BufferedTransformation &issuer,
                          CryptoPP::Integer &serial);

void showUI(const char *szPAN);
void notifyCardNotRegistered(const char *szPAN);

IAS::IAS(CToken::TokenTransmitCallback transmit, ByteArray ATR) {
  init_func

      Callback = nullptr;
  this->ATR = ATR;
  uint8_t gemaltoAID[] = {0xA0, 0x00, 0x00, 0x00, 0x30, 0x80, 0x00,
                          0x00, 0x00, 0x09, 0x81, 0x60, 0x01};
  IAS_AID = VarToByteArray(gemaltoAID);

  uint8_t AID[] = {0xA0, 0x00, 0x00, 0x00, 0x00, 0x39};
  CIE_AID = VarToByteArray(AID);

  ActiveSM = false;
  ActiveDF = DF_Root;

  token.setTransmitCallback(transmit, nullptr);
}

IAS::~IAS() {}

uint8_t defModule[] = {
    0xba, 0x28, 0x37, 0xab, 0x4c, 0x6b, 0xb8, 0x27, 0x57, 0x7b, 0xff, 0x4e,
    0xb7, 0xb1, 0xe4, 0x9c, 0xdd, 0xe0, 0xf1, 0x66, 0x14, 0xd1, 0xef, 0x24,
    0xc1, 0xb7, 0x5c, 0xf7, 0x0f, 0xb1, 0x2c, 0xd1, 0x8f, 0x4d, 0x14, 0xe2,
    0x81, 0x4b, 0xa4, 0x87, 0x7e, 0xa8, 0x00, 0xe1, 0x75, 0x90, 0x60, 0x76,
    0xb5, 0x62, 0xba, 0x53, 0x59, 0x73, 0xc5, 0xd8, 0xb3, 0x78, 0x05, 0x1d,
    0x8a, 0xfc, 0x74, 0x07, 0xa1, 0xd9, 0x19, 0x52, 0x9e, 0x03, 0xc1, 0x06,
    0xcd, 0xa1, 0x8d, 0x69, 0x9a, 0xfb, 0x0d, 0x8a, 0xb4, 0xfd, 0xdd, 0x9d,
    0xc7, 0x19, 0x15, 0x9a, 0x50, 0xde, 0x94, 0x68, 0xf0, 0x2a, 0xb1, 0x03,
    0xe2, 0x82, 0xa5, 0x0e, 0x71, 0x6e, 0xc2, 0x3c, 0xda, 0x5b, 0xfc, 0x4a,
    0x23, 0x2b, 0x09, 0xa4, 0xb2, 0xc7, 0x07, 0x45, 0x93, 0x95, 0x49, 0x09,
    0x9b, 0x44, 0x83, 0xcb, 0xae, 0x62, 0xd0, 0x09, 0x96, 0x74, 0xdb, 0xf6,
    0xf3, 0x9b, 0x72, 0x23, 0xa9, 0x9d, 0x88, 0xe3, 0x3f, 0x1a, 0x0c, 0xde,
    0xde, 0xeb, 0xbd, 0xc3, 0x55, 0x17, 0xab, 0xe9, 0x88, 0x0a, 0xab, 0x24,
    0x0e, 0x1e, 0xa1, 0x66, 0x28, 0x3a, 0x27, 0x4a, 0x9a, 0xd9, 0x3b, 0x4b,
    0x1d, 0x19, 0xf3, 0x67, 0x9f, 0x3e, 0x8b, 0x5f, 0xf6, 0xa1, 0xe0, 0xed,
    0x73, 0x6e, 0x84, 0xd5, 0xab, 0xe0, 0x3c, 0x59, 0xe7, 0x34, 0x6b, 0x42,
    0x18, 0x75, 0x5d, 0x75, 0x36, 0x6c, 0xbf, 0x41, 0x36, 0xf0, 0xa2, 0x6c,
    0x3d, 0xc7, 0x0a, 0x69, 0xab, 0xaa, 0xf6, 0x6e, 0x13, 0xa1, 0xb2, 0xfa,
    0xad, 0x05, 0x2c, 0xa6, 0xec, 0x9c, 0x51, 0xe2, 0xae, 0xd1, 0x4d, 0x16,
    0xe0, 0x90, 0x25, 0x4d, 0xc3, 0xf6, 0x4e, 0xa2, 0xbd, 0x8a, 0x83, 0x6b,
    0xba, 0x99, 0xde, 0xfa, 0xcb, 0xa3, 0xa6, 0x13, 0xae, 0xed, 0xd9, 0x3a,
    0x96, 0x15, 0x27, 0x3d};
uint8_t defPrivExp[] = {
    0x47, 0x16, 0xc2, 0xa3, 0x8c, 0xcc, 0x7a, 0x07, 0xb4, 0x15, 0xeb, 0x1a,
    0x61, 0x75, 0xf2, 0xaa, 0xa0, 0xe4, 0x9c, 0xea, 0xf1, 0xba, 0x75, 0xcb,
    0xa0, 0x9a, 0x68, 0x4b, 0x04, 0xd8, 0x11, 0x18, 0x79, 0xd3, 0xe2, 0xcc,
    0xd8, 0xb9, 0x4d, 0x3c, 0x5c, 0xf6, 0xc5, 0x57, 0x53, 0xf0, 0xed, 0x95,
    0x87, 0x91, 0x0b, 0x3c, 0x77, 0x25, 0x8a, 0x01, 0x46, 0x0f, 0xe8, 0x4c,
    0x2e, 0xde, 0x57, 0x64, 0xee, 0xbe, 0x9c, 0x37, 0xfb, 0x95, 0xcd, 0x69,
    0xce, 0xaf, 0x09, 0xf4, 0xb1, 0x35, 0x7c, 0x27, 0x63, 0x14, 0xab, 0x43,
    0xec, 0x5b, 0x3c, 0xef, 0xb0, 0x40, 0x3f, 0x86, 0x8f, 0x68, 0x8e, 0x2e,
    0xc0, 0x9a, 0x49, 0x73, 0xe9, 0x87, 0x75, 0x6f, 0x8d, 0xa7, 0xa1, 0x01,
    0xa2, 0xca, 0x75, 0xa5, 0x4a, 0x8c, 0x4c, 0xcf, 0x9a, 0x1b, 0x61, 0x47,
    0xe4, 0xde, 0x56, 0x42, 0x3a, 0xf7, 0x0b, 0x20, 0x67, 0x17, 0x9c, 0x5e,
    0xeb, 0x64, 0x68, 0x67, 0x86, 0x34, 0x78, 0xd7, 0x52, 0xc7, 0xf4, 0x12,
    0xdb, 0x27, 0x75, 0x41, 0x57, 0x5a, 0xa0, 0x61, 0x9d, 0x30, 0xbc, 0xcc,
    0x8d, 0x87, 0xe6, 0x17, 0x0b, 0x33, 0x43, 0x9a, 0x2c, 0x93, 0xf2, 0xd9,
    0x7e, 0x18, 0xc0, 0xa8, 0x23, 0x43, 0xa6, 0x01, 0x2a, 0x5b, 0xb1, 0x82,
    0x28, 0x08, 0xf0, 0x1b, 0x5c, 0xfd, 0x85, 0x67, 0x3a, 0xc0, 0x96, 0x4c,
    0x5f, 0x3c, 0xfd, 0x2d, 0xaf, 0x81, 0x42, 0x35, 0x97, 0x64, 0xa9, 0xad,
    0xb9, 0xe3, 0xf7, 0x6d, 0xb6, 0x13, 0x46, 0x1c, 0x1b, 0xc9, 0x13, 0xdc,
    0x9a, 0xc0, 0xab, 0x50, 0xd3, 0x65, 0xf7, 0x7c, 0xb9, 0x31, 0x94, 0xc9,
    0x8a, 0xa9, 0x66, 0xd8, 0x9c, 0xdd, 0x55, 0x51, 0x25, 0xa5, 0xe5, 0x9e,
    0xcf, 0x4f, 0xa3, 0xf0, 0xc3, 0xfd, 0x61, 0x0c, 0xd3, 0xd0, 0x56, 0x43,
    0x93, 0x38, 0xfd, 0x81};
uint8_t defPubExp[] = {0x00, 0x01, 0x00, 0x01};

void IAS::ReadSOD(ByteDynArray &data) {
  init_func readfile(0x1006, data);
  LOG_DEBUG("SOD:");
  LOG_BUFFER(data.data(), data.size());
  exit_func
}
void IAS::ReadDH(ByteDynArray &data) {
  init_func readfile(0xd004, data);

  LOG_DEBUG("ReadDH - Data:");
  LOG_BUFFER(data.data(), data.size());
  exit_func
}
void IAS::ReadCertCIE(ByteDynArray &data) {
  init_func readfile(0x1003, data);

  LOG_DEBUG("ReadCertCIE - Cert CIE:");
  LOG_BUFFER(data.data(), data.size());
  exit_func
}
void IAS::ReadServiziPubKey(ByteDynArray &data) {
  init_func readfile(0x1005, data);

  LOG_DEBUG("ReadServiziPubKey - Data:");
  LOG_BUFFER(data.data(), data.size());
  exit_func
}
void IAS::ReadSerialeCIE(ByteDynArray &data) {
  init_func readfile(0x1002, data);

  LOG_DEBUG("ReadSerialeCIE - CIE serial number:");
  LOG_BUFFER(data.data(), data.size());
  exit_func
}
void IAS::ReadIdServizi(ByteDynArray &data) {
  init_func readfile(0x1001, data);

  LOG_DEBUG("ReadIdServizi - CIE Id Servizi:");
  LOG_BUFFER(data.data(), data.size());
  exit_func
}

void IAS::Sign(ByteArray &data, ByteDynArray &signedData) {
  init_func ByteDynArray resp;
  uint8_t SetKey[] = {0x00, 0x22, 0x41, 0xA4};
  uint8_t val02 = 2;
  uint8_t keyId = CIE_KEY_Sign_ID;

  LOG_DEBUG("IAS::Sign - Buffer to sign:");
  LOG_BUFFER(data.data(), data.size());

  StatusWord sw;
  ByteArray val02Ba = VarToByteArray(val02);
  ByteArray keyIdBa = VarToByteArray(keyId);
  if ((sw = SendAPDU_SM(VarToByteArray(SetKey),
                        ASN1Tag(0x80, val02Ba).append(ASN1Tag(0x84, keyIdBa)),
                        resp)) != 0x9000) {
    LOG_ERROR("IAS::Sign - Smart card response error");
    throw scard_error(sw);
  }

  uint8_t Sign[] = {0x00, 0x88, 0x00, 0x00};
  if ((sw = SendAPDU_SM(VarToByteArray(Sign), data, signedData)) != 0x9000) {
    LOG_ERROR("IAS::Sign error!");
    throw scard_error(sw);
  }
}

StatusWord IAS::VerifyPUK(ByteArray &PIN) {
  init_func ByteDynArray resp;

  LOG_DEBUG("IAS::VerifyPUK");
  uint8_t verifyPIN[] = {0x00, 0x20, 0x00, CIE_PUK_ID};
  return SendAPDU_SM(VarToByteArray(verifyPIN), PIN, resp);
}

StatusWord IAS::VerifyPIN(ByteArray &PIN) {
  init_func ByteDynArray resp;

  LOG_DEBUG("IAS::VerifyPIN");
  uint8_t verifyPIN[] = {0x00, 0x20, 0x00, CIE_PIN_ID};
  return SendAPDU_SM(VarToByteArray(verifyPIN), PIN, resp);
  exit_func
}

StatusWord IAS::UnblockPIN() {
  init_func ByteDynArray resp;

  LOG_DEBUG("IAS::UnblockPIN");
  uint8_t UnblockPIN[] = {0x00, 0x2C, 0x03, CIE_PIN_ID};
  return SendAPDU_SM(VarToByteArray(UnblockPIN), ByteArray(), resp);
  exit_func
}

StatusWord IAS::ChangePIN(ByteArray &oldPIN, ByteArray &newPIN) {
  init_func ByteDynArray resp;
  ByteDynArray data = oldPIN;
  data.append(newPIN);
  uint8_t ChangePIN[] = {0x00, 0x24, 0x00, CIE_PIN_ID};
  return SendAPDU_SM(VarToByteArray(ChangePIN), data, resp);
  exit_func
}

StatusWord IAS::ChangePIN(ByteArray &newPIN) {
  init_func ByteDynArray resp;

  LOG_DEBUG("IAS::ChangePIN");
  uint8_t ChangePIN[] = {0x00, 0x2C, 0x02, CIE_PIN_ID};
  return SendAPDU_SM(VarToByteArray(ChangePIN), newPIN, resp);
  exit_func
}

void IAS::readfile(uint16_t id, ByteDynArray &content) {
  init_func

      LOG_DEBUG("IAS::readfile - id: %d", id);
  LOG_DEBUG("IAS::readfile - is SM Active: %d", ActiveSM);

  if (ActiveSM) return readfile_SM(id, content);

  ByteDynArray resp;
  uint8_t selectFile[] = {0x00, 0xa4, 0x02, 0x04};
  uint8_t fileId[] = {static_cast<uint8_t>(HIBYTE(id)),
                      static_cast<uint8_t>(LOBYTE(id))};
  StatusWord sw;
  if ((sw = SendAPDU(VarToByteArray(selectFile), VarToByteArray(fileId),
                     resp)) != 0x9000)
    throw scard_error(sw);

  WORD cnt = 0;
  uint8_t chunk = 128;
  while (true) {
    ByteDynArray chn;
    uint8_t readFile[] = {0x00, 0xb0, static_cast<uint8_t>(HIBYTE(cnt)),
                          static_cast<uint8_t>(LOBYTE(cnt))};
    sw = SendAPDU(VarToByteArray(readFile), ByteArray(), chn, &chunk);
    if ((sw >> 8) == 0x6c) {
      uint8_t le = sw & 0xff;
      sw = SendAPDU(VarToByteArray(readFile), ByteArray(), chn, &le);
    }
    if (sw == 0x9000) {
      content.append(chn);
      cnt = content.size();
      chunk = 128;
    } else {
      if (sw == 0x6282)
        content.append(chn);
      else if (sw != 0x6b00)
        throw scard_error(sw);
      break;
    }
  }
  exit_func
}

void IAS::readfile_SM(uint16_t id, ByteDynArray &content) {
  init_func

      ByteDynArray resp;
  uint8_t selectFile[] = {0x00, 0xa4, 0x02, 0x04};
  uint8_t fileId[] = {static_cast<uint8_t>(HIBYTE(id)),
                      static_cast<uint8_t>(LOBYTE(id))};
  StatusWord sw;
  if ((sw = SendAPDU_SM(VarToByteArray(selectFile), VarToByteArray(fileId),
                        resp)) != 0x9000)
    throw scard_error(sw);

  WORD cnt = 0;
  uint8_t chunk = 128;
  while (true) {
    ByteDynArray chn;
    uint8_t readFile[] = {0x00, 0xb0, HIBYTE(cnt), LOBYTE(cnt)};
    sw = SendAPDU_SM(VarToByteArray(readFile), ByteArray(), chn, &chunk);
    if ((sw >> 8) == 0x6c) {
      uint8_t le = sw & 0xff;
      sw = SendAPDU_SM(VarToByteArray(readFile), ByteArray(), chn, &le);
    }
    if (sw == 0x9000) {
      content.append(chn);
      cnt = content.size();
      chunk = 128;
    } else {
      if (sw == 0x6282)
        content.append(chn);
      else if (sw != 0x6b00)
        throw scard_error(sw);
      break;
    }
  }
  exit_func
}

void IAS::SelectAID_CIE(bool SM) {
  init_func ByteDynArray resp;
  uint8_t selectCIE[] = {0x00, 0xa4, 0x04, 0x0c};
  ByteDynArray selectCIEapdu;
  StatusWord sw;

  LOG_DEBUG("IAS::SelectAID_CIE");
  if (SM) {
    if ((sw = SendAPDU_SM(VarToByteArray(selectCIE), CIE_AID, resp)) != 0x9000)
      throw scard_error(sw);
  } else {
    if ((sw = SendAPDU(VarToByteArray(selectCIE), CIE_AID, resp)) != 0x9000)
      throw scard_error(sw);
  }
  ActiveDF = DF_CIE;
  ActiveSM = false;
  exit_func
}

void IAS::ReadCIEType() {
  init_func

      std::vector<uint8_t>
          atr_vector((ATR.data()), ATR.data() + ATR.size());

  type = get_type(atr_vector);
  if (type == CIE_Type::CIE_Unknown) {
    throw logged_error("ReadCIEType - CIE not recognized");
  }

#if 0
	if (ATR.indexOf(baNXP_ATR, position)) {
		type = CIE_Type::CIE_NXP;
		LOG_INFO("ReadCIEType - CIE NXP detected");
	}
	else if (ATR.indexOf(baGemalto_ATR, position)) {
		type = CIE_Type::CIE_Gemalto;
		LOG_INFO("ReadCIEType - CIE Gemalto detected\n");
	}
	else if (ATR.indexOf(baGemalto2_ATR, position)) {
		type = CIE_Type::CIE_Gemalto;
		LOG_INFO("ReadCIEType - CIE Gemalto2 detected\n");
	}
	else if (ATR.indexOf(baSTM_ATR, position)) {
		type = CIE_Type::CIE_STM;
		LOG_INFO("ReadCIEType - CIE STM detected\n");
	}
	else if (ATR.indexOf(baSTM2_ATR, position)) {
		type = CIE_Type::CIE_STM2;
		LOG_INFO("ReadCIEType - CIE STM2 detected\n");
	}
	else if (ATR.indexOf(baSTM3_ATR, position))
	{
		type = CIE_Type::CIE_STM3;
		LOG_INFO("ReadCIEType - CIE STM3 detected\n");
	}
	else
		throw logged_error("ReadCIEType - CIE not recognized");
#endif
}

void IAS::SelectAID_IAS(bool SM) {
  init_func if (type == CIE_Type::CIE_Unknown) { ReadCIEType(); }
  ByteDynArray resp;
  StatusWord sw;
  if ((type >= CIE_Type::CIE_NXP)) {
    uint8_t selectMF[] = {0x00, 0xa4, 0x00, 0x00};

    if (SM) {
      if ((sw = SendAPDU_SM(VarToByteArray(selectMF), ByteArray(), resp)) !=
          0x9000)
        throw scard_error(sw);
    } else {
      if ((sw = SendAPDU(VarToByteArray(selectMF), ByteArray(), resp)) !=
          0x9000)
        throw scard_error(sw);
    }
  } else if ((type < CIE_Type::CIE_NXP) && (type != CIE_Type::CIE_Unknown)) {
    uint8_t selectIAS[] = {0x00, 0xa4, 0x04, 0x0c};
    if (SM) {
      if ((sw = SendAPDU_SM(VarToByteArray(selectIAS), IAS_AID, resp)) !=
          0x9000)
        throw scard_error(sw);
    } else {
      if ((sw = SendAPDU(VarToByteArray(selectIAS), IAS_AID, resp)) != 0x9000)
        throw scard_error(sw);
    }
  } else {
    throw logged_error("SelectAID_IAS - CIE not recognized");
  }

  SM = false;
  ActiveDF = DF_IAS;
  ActiveSM = false;
  exit_func
}

void IAS::ReadDappPubKey(ByteDynArray &DappKey) {
  init_func

      LOG_DEBUG("**** Starting ReadDappPubKey *****");

  ByteDynArray resp;
  readfile(0x1004, DappKey);

  CASNParser parser;
  parser.Parse(DappKey);
  LOG_DEBUG("ReadDappPubKey - Parsing ok");
  ByteArray module = parser.tags[0]->tags[0]->content;
  while (module[0] == 0) module = module.mid(1);
  DappModule = module;
  ByteArray pubKey = parser.tags[0]->tags[1]->content;
  while (pubKey[0] == 0) pubKey = pubKey.mid(1);
  DappPubKey = pubKey;

  LOG_DEBUG("ReadDappPubKey - Pub Key:");
  LOG_BUFFER(DappPubKey.data(), DappPubKey.size());

  LOG_DEBUG("**** ReadDappPubKey Completed *****");
  exit_func
}

void IAS::ReadPAN() {
  init_func readfile(0xd003, PAN);

  LOG_DEBUG("ReadPAN - PAN:");
  LOG_BUFFER(PAN.data(), PAN.size());
  exit_func
}

void IAS::DAPP() {
  init_func

      ByteDynArray resp;
  uint8_t psoVerifyAlgo = 0x41;
  uint8_t PKdScheme = 0x9B;
  uint8_t shaOID = 0x04;
  DWORD shaSize = 32;
  CSHA256 sha256;

  if (DappPubKey.isEmpty()) {
    throw logged_error("DAPP - DAPP key not available");
  }

  ByteDynArray module = VarToByteArray(defModule);
  ByteDynArray pubexp = VarToByteArray(defPubExp);
  ByteDynArray privexp = VarToByteArray(defPrivExp);

  ByteDynArray cert;
  ByteDynArray CHR, CHA, OID;

  uint8_t snIFD[] = {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
  uint8_t CPI = 0x8A;
  uint8_t baseCHR[] = {0x00, 0x00, 0x00, 0x00};

  ByteArray baseCHRBa = VarToByteArray(baseCHR);
  ByteArray snIFDBa = VarToByteArray(snIFD);

  CHR.set(&baseCHRBa, &snIFDBa);
  CHA.set(&CA_AID, 01);
  uint8_t baseOID[] = {0x2A, 0x81, 0x22, 0xF4, 0x2A, 0x02, 0x04, 0x01};

  ByteArray baseOIDBa = VarToByteArray(baseOID);
  OID.set(&baseOIDBa, shaOID);

  ByteDynArray endEntityCert;
  endEntityCert.set(CPI, &CA_CAR, &CHR, &CHA, &OID, &module, &pubexp);

  ByteDynArray certSign, toSign;
  ByteArray endEntityCertBa =
      endEntityCert.left(CA_module.size() - shaSize - 2);

  ByteDynArray endEntityCertDigestBa = sha256.Digest(endEntityCert);

  toSign.set(0x6A, &endEntityCertBa, &endEntityCertDigestBa, 0xbc);
  CRSA caKey(CA_module, CA_privexp);

  certSign = caKey.RSA_PURE(toSign);

  ByteDynArray certVerif;
  CRSA caPubKey(CA_module, CA_pubexp);

  certVerif = caPubKey.RSA_PURE(certSign);
  ER_ASSERT(certVerif == toSign, "Errore in verifica firma!")

  ByteDynArray PkRem;
  PkRem = endEntityCert.mid(CA_module.size() - shaSize - 2);

  cert.setASN1Tag(0x7F21, ASN1Tag(0x5F37, certSign)
                              .append(ASN1Tag(0x5F38, PkRem))
                              .append(ASN1Tag(0x42, CA_CAR)));

  uint8_t SelectKey[] = {0x00, 0x22, 0x81, 0xb6};
  uint8_t id = CIE_KEY_ExtAuth_ID;
  StatusWord sw;

  uint8_t le = 0;
  ByteArray psoVerifyAlgoBa = VarToByteArray(psoVerifyAlgo);
  ByteArray idBa = VarToByteArray(id);
  if ((sw = SendAPDU_SM(
           VarToByteArray(SelectKey),
           ASN1Tag(0x80, psoVerifyAlgoBa).append(ASN1Tag(0x83, idBa)), resp,
           &le)) != 0x9000)
    throw scard_error(sw);

  uint8_t VerifyCert[] = {0x00, 0x2A, 0x00, 0xAE};
  if ((sw = SendAPDU_SM(VarToByteArray(VerifyCert), cert, resp)) != 0x9000)
    throw scard_error(sw);

  uint8_t SetCHR[] = {0x00, 0x22, 0x81, 0xA4};

  if ((sw = SendAPDU_SM(VarToByteArray(SetCHR), ASN1Tag(0x83, CHR), resp)) !=
      0x9000)
    throw scard_error(sw);

  ByteDynArray challenge;
  uint8_t GetChallenge[] = {0x00, 0x84, 0x00, 0x00};
  uint8_t chLen = 8;

  if ((sw = SendAPDU_SM(VarToByteArray(GetChallenge), ByteArray(), challenge,
                        &chLen)) != 0x9000)
    throw scard_error(sw);

  ByteDynArray toHash;
  size_t padSize = module.size() - shaSize - 2;
  ByteDynArray PRND(padSize);
  PRND.random();
  toHash.set(&PRND, &dh_pubKey, &snIFDBa, &challenge, &dh_ICCpubKey, &dh_g,
             &dh_p, &dh_q);
  ByteDynArray toHashBa = sha256.Digest(toHash);
  toSign.set(0x6a, &PRND, &toHashBa, 0xBC);

  CRSA certKey(module, privexp);

  ByteDynArray signResp = certKey.RSA_PURE(toSign);
  ByteDynArray chResponse;
  chResponse.set(&snIFDBa, &signResp);

  uint8_t ExtAuth[] = {0x00, 0x82, 0x00, 0x00};
  if ((sw = SendAPDU_SM(VarToByteArray(ExtAuth), chResponse, resp)) != 0x9000)
    throw scard_error(sw);

  uint8_t IntAuth[] = {0x00, 0x22, 0x41, 0xa4};
  uint8_t Val82 = 0x82;
  ByteArray IntAuthBa = VarToByteArray(IntAuth);
  ByteArray Val82Ba = VarToByteArray(Val82);
  ByteArray PKdSchemeBa = VarToByteArray(PKdScheme);

  if ((sw = SendAPDU_SM(
           VarToByteArray(IntAuth),
           ASN1Tag(0x84, Val82Ba).append(ASN1Tag(0x80, PKdSchemeBa)), resp)) !=
      0x9000)
    throw scard_error(sw);

  ByteDynArray rndIFD(8);
  rndIFD.random();
  uint8_t GiveRandom[] = {0x00, 0x88, 0x00, 0x00};
  if ((sw = SendAPDU_SM(VarToByteArray(GiveRandom), rndIFD, resp, 0)) != 0x9000)
    throw scard_error(sw);

  ByteDynArray SN_ICC = resp.mid(0, 8);

  CRSA intAuthKey(DappModule, DappPubKey);
  ByteArray respBa = resp.mid(8);

  ByteDynArray intAuthResp = intAuthKey.RSA_PURE(respBa);
  ER_ASSERT(intAuthResp[0] == 0x6a, "Errore nell'autenticazione del chip");
  ByteArray PRND2 = intAuthResp.mid(1, intAuthResp.size() - 32 - 2);
  ByteArray hashICC = intAuthResp.mid(PRND2.size() + 1, 32);

  ByteDynArray toHashIFD;
  toHashIFD.set(&PRND2, &dh_ICCpubKey, &SN_ICC, &rndIFD, &dh_pubKey, &dh_g,
                &dh_p, &dh_q);

  ByteDynArray calcHashIFD = sha256.Digest(toHashIFD);

  ER_ASSERT(calcHashIFD == hashICC, "Errore nell'autenticazione del chip")
  ER_ASSERT(intAuthResp.right(1)[0] == 0xbc,
            "Errore nell'autenticazione del chip");

  ByteArray challengeBa = challenge.right(4);
  ByteArray rndIFDBa = rndIFD.right(4);
  sessSSC.set(&challengeBa, &rndIFDBa);
  ActiveSM = true;
  exit_func
}

void IAS::DHKeyExchange() {
  init_func CASNParser asn1;

  ByteDynArray dh_prKey, secret, resp, d1;
  do {
    dh_prKey.resize(dh_q.size());
    dh_prKey.random();
  } while (dh_q[0] < dh_prKey[0]);

  // dh_prKey deve essere dispari
  dh_prKey.right(1)[0] |= 1;

  ByteDynArray dhg(dh_g.size());
  dhg.fill(0);
  dhg.rightcopy(dh_g);
  CRSA rsa(dh_p, dh_prKey);

  dh_pubKey = rsa.RSA_PURE(dhg);

  uint8_t algo = 0x9b;
  uint8_t keyId = CIE_KEY_DH_ID;
  ByteArray algoBa = VarToByteArray(algo);
  ByteArray keyIdBa = VarToByteArray(keyId);

  d1.setASN1Tag(0x80, algoBa)
      .append(ASN1Tag(0x83, keyIdBa))
      .append(ASN1Tag(0x91, dh_pubKey));

  uint8_t MSE_SET[] = {0x00, 0x22, 0x41, 0xa6};
  StatusWord sw;
  if ((sw = SendAPDU(VarToByteArray(MSE_SET), d1, resp)) != 0x9000)
    throw scard_error(sw);
  uint8_t GET_DATA[] = {0x00, 0xcb, 0x3f, 0xff};
  uint8_t GET_DATA_Data[] = {0x4d, 0x04, 0xa6, 0x02, 0x91, 0x00};
  if ((sw = SendAPDU(VarToByteArray(GET_DATA), VarToByteArray(GET_DATA_Data),
                     resp)) != 0x9000)
    throw scard_error(sw);

  asn1.Parse(resp);
  dh_ICCpubKey = asn1.tags[0]->tags[0]->content;

  secret = rsa.RSA_PURE(dh_ICCpubKey);

  CSHA256 sha256;

  uint8_t diffENC[] = {0x00, 0x00, 0x00, 0x01};
  uint8_t diffMAC[] = {0x00, 0x00, 0x00, 0x02};

  sessENC = sha256.Digest(ByteDynArray(secret).append(VarToByteArray(diffENC)))
                .left(16);
  sessMAC = sha256.Digest(ByteDynArray(secret).append(VarToByteArray(diffMAC)))
                .left(16);

  sessSSC.resize(8);
  sessSSC.fill(0);
  sessSSC[7] = 1;

  ActiveSM = true;
  exit_func
}

void IAS::increment(ByteArray &seq) {
  for (size_t i = seq.size() - 1; i >= 0; i--) {
    if (seq[i] < 255) {
      seq[i]++;
      for (size_t j = i + 1; j < seq.size(); j++) seq[j] = 0;
      return;
    }
  }
}

ByteDynArray IAS::SM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &apdu,
                     ByteArray &seq) {
  init_func

      std::string dmp;

  increment(seq);

  ByteDynArray smHead;
  smHead = apdu.left(4);
  smHead[0] |= 0x0C;
  auto calcMac = ISOPad(ByteDynArray(seq).append(smHead));

  ByteDynArray iv(8);
  iv.fill(0);  // IV for APDU encryption and signature should be 0.

  CDES3 encDes(keyEnc, iv);
  CMAC sigMac(keySig, iv);

  uint8_t Val01 = 1;

  ByteDynArray datafield, doob;
  size_t apduSize = apdu.size();

  if (apdu[4] != 0 && apduSize > 5) {
    ByteDynArray enc = encDes.RawEncode(ISOPad(apdu.mid(5, apdu[4])));

    if ((apdu[1] & 1) == 0)
      doob.setASN1Tag(0x87, VarToByteDynArray(Val01).append(enc));
    else
      doob.setASN1Tag(0x85, enc);

    calcMac.append(doob);
    datafield.append(doob);
  }
  if (apdu[4] == 0 && apduSize > 7) {
    ByteDynArray enc =
        encDes.RawEncode(ISOPad(apdu.mid(7, (apdu[5] << 8) | apdu[6])));
    if ((apdu[1] & 1) == 0)
      doob.setASN1Tag(0x87, VarToByteDynArray(Val01).append(enc));
    else
      doob.setASN1Tag(0x85, enc);

    calcMac.append(doob);
    datafield.append(doob);
  }
  if (apduSize == 5 || apduSize == static_cast<size_t>(apdu[4] + 6)) {
    uint8_t le = apdu[apduSize - 1];
    ByteArray leBa = VarToByteArray(le);
    doob.setASN1Tag(0x97, leBa);
    calcMac.append(doob);
    datafield.append(doob);
  }

  ByteDynArray macBa = sigMac.Mac(ISOPad(calcMac));

  ByteDynArray tagMacBa = ASN1Tag(0x8e, macBa);
  datafield.append(tagMacBa);

  ByteDynArray elabResp;
  if (datafield.size() < 0x100)
    elabResp.set(&smHead, (uint8_t)datafield.size(), &datafield, (uint8_t)0x00);
  else {
    auto len = datafield.size();
    auto lenBA = VarToByteArray(len);
    ByteArray lenBa = lenBA.reverse().right(3);

    elabResp.set(&smHead, &lenBa, &datafield, (uint8_t)0x00, (uint8_t)0x00);
  }

  return elabResp;
}

StatusWord IAS::respSM(ByteArray &keyEnc, ByteArray &keySig, ByteArray &resp,
                       ByteArray &seq, ByteDynArray &elabResp) {
  init_func

      increment(seq);
  DWORD index, llen, lgn;
  StatusWord sw = 0xffff;
  ByteDynArray encData;
  ByteDynArray respMac, calcMac;
  ByteDynArray iv(8);
  iv.fill(
      0);  // IV for APDU encryption should be 0. Please refer to IAS
           // specification §7.1.9 Secure messaging – Command APDU protection
  CDES3 encDes(keyEnc, iv);
  CMAC sigMac(keySig, iv);

  calcMac = seq;
  index = 0;
  do {
    if (resp[index] == 0x99) {
      calcMac.append(resp.mid(index, resp[index + 1] + 2));
      sw = resp[index + 2] << 8 | resp[index + 3];
      index += 4;
    } else if (resp[index] == 0x8e) {
      if (resp[index + 1] != 0x08)
        throw logged_error("Lunghezza del MAC non valida");
      respMac = resp.mid(index + 2, 8);
      index += 10;
    } else if (resp[index] == 0x85) {
      if (resp[index + 1] > 0x80) {
        llen = resp[index + 1] - 0x80;
        if (llen == 1)
          lgn = resp[index + 2];
        else if (llen == 2)
          lgn = (resp[index + 2] << 8) | resp[index + 3];
        else
          throw logged_error(stdPrintf("Lunghezza ASN1 non valida: %i", llen));
        encData = resp.mid(index + llen + 2, lgn);
        calcMac.append(resp.mid(index, lgn + llen + 2));
        index += llen + lgn + 2;
      } else {
        encData = resp.mid(index + 2, resp[index + 1]);
        calcMac.append(resp.mid(index, resp[index + 1] + 2));
        index += resp[index + 1] + 2;
      }
    } else if (resp[index] == 0x87) {
      if (resp[index + 1] > 0x80) {
        llen = resp[index + 1] - 0x80;
        if (llen == 1)
          lgn = resp[index + 2];
        else if (llen == 2)
          lgn = (resp[index + 2] << 8) | resp[index + 3];
        else
          throw logged_error(stdPrintf("Lunghezza ASN1 non valida: %i", llen));
        encData = resp.mid(index + llen + 3, lgn - 1);
        calcMac.append(resp.mid(index, lgn + llen + 2));
        index += llen + lgn + 2;
      } else {
        encData = resp.mid(index + 3, resp[index + 1] - 1);
        calcMac.append(resp.mid(index, resp[index + 1] + 2));
        index += resp[index + 1] + 2;
      }
    } else
      throw logged_error(
          "Tag non riconosciuto nella risposta in Secure Messaging");
  } while (index < resp.size());

  auto smMac = sigMac.Mac(ISOPad(calcMac));
  ER_ASSERT(smMac == respMac, "Errore nel checksum della risposta del chip")

  if (!encData.isEmpty()) {
    elabResp = encDes.RawDecode(encData);
    elabResp.resize(RemoveISOPad(elabResp), true);
  } else
    elabResp.clear();

  return sw;
}

StatusWord IAS::getResp(ByteDynArray &resp, StatusWord sw,
                        ByteDynArray &elabresp) {
  init_func elabresp.clear();
  if (resp.size() != 0) elabresp.append(resp);

  ByteDynArray curresp;
  while (true) {
    if ((sw >> 8) == 0x61) {
      uint8_t ln = sw & 0xff;
      if (ln != 0) {
        uint8_t apdu[] = {0x00, 0xc0, 0x00, 0x00, ln};
        sw = token.Transmit(VarToByteArray(apdu), &curresp);
        elabresp.append(curresp);
        return sw;
      } else {
        uint8_t apdu[] = {0x00, 0xc0, 0x00, 0x00, 0x00};
        sw = token.Transmit(VarToByteArray(apdu), &curresp);
        elabresp.append(curresp);
      }
    } else {
      return sw;
    }
  }
  exit_func
}

StatusWord IAS::getResp_SM(ByteArray &resp, StatusWord sw,
                           ByteDynArray &elabresp) {
  init_func

      ByteDynArray s,
      ap;
  CASNParser p;
  elabresp.clear();
  if (resp.size() != 0) elabresp.append(resp);

  ByteDynArray curresp;
  while (true) {
    if ((sw >> 8) == 0x61) {
      uint8_t ln = sw & 0xff;
      if (ln != 0) {
        uint8_t apdu[] = {0x00, 0xc0, 0x00, 0x00, ln};
        sw = token.Transmit(VarToByteArray(apdu), &curresp);
        elabresp.append(curresp);
        if (sw == 0x9000) break;
        if ((sw >> 8) != 0x61) throw scard_error(sw);
      } else {
        uint8_t apdu[] = {0x0c, 0xc0, 0x00, 0x00, 0x00};
        sw = token.Transmit(VarToByteArray(apdu), &curresp);
        elabresp.append(curresp);
      }
    } else if (sw == 0x9000 || sw == 0x6b00 || sw == 0x6282)
      break;
    else
      return sw;
  }
  return respSM(sessENC, sessMAC, elabresp, sessSSC, elabresp);
  exit_func
}

StatusWord IAS::SendAPDU_SM(ByteArray head, ByteArray data, ByteDynArray &resp,
                            uint8_t *le) {
  init_func ByteDynArray smApdu;
  ByteDynArray s, curresp;
  ByteArray emptyBa;
  ByteArray leBa = VarToByteArray(*le);
  std::string str;

  StatusWord sw;
  if (data.size() < 0xE7) {
    smApdu.set(&head, (uint8_t)data.size(), &data,
               (le == nullptr) ? &emptyBa : &leBa);

    smApdu = SM(sessENC, sessMAC, smApdu, sessSSC);
    sw = token.Transmit(smApdu, &curresp);
    sw = getResp_SM(curresp, sw, resp);
    return sw;
  } else {
    // attenzione:
    // in alcuni casi la carta ritorna 61xx fra un comando e l'altro in
    // chaining. Questo è un grosso problema, perchè la get response sembra che
    // faccia saltare il chaining. Forse è una questione di driver del lettore?
    // Per daesso l'ho osservato solo su una virtual machine Win7 con il lettore
    // in sharing con l'host

    size_t i = 0;
    uint8_t cla = head[0];
    while (true) {
      s = data.mid(i, min1(0xE7, data.size() - i));
      i += s.size();
      if (i != data.size())
        head[0] = cla | 0x10;
      else
        head[0] = cla;
      if (s.size() != 0)
        smApdu.set(&head, (BYTE)s.size(), &s,
                   (le == nullptr || i < data.size()) ? &emptyBa : &leBa);
      else
        smApdu.set(&head,
                   (le == nullptr || i < data.size()) ? &emptyBa : &leBa);

      smApdu = SM(sessENC, sessMAC, smApdu, sessSSC);
      sw = token.Transmit(smApdu, &curresp);
      sw = getResp_SM(curresp, sw, resp);
      if (i == data.size()) return sw;
    }
  }
  exit_func
}

StatusWord IAS::SendAPDU(ByteArray head, ByteArray data, ByteDynArray &resp,
                         uint8_t *le) {
  init_func

      ByteArray emptyBa;
  ByteArray leBa = VarToByteArray(*le);
  std::string str;

  ByteDynArray apdu, curresp;
  auto ds = data.size();

  if (ds > 255) {
    size_t i = 0;
    uint8_t cla = head[0];
    while (true) {
      auto s = data.mid(i, min1(data.size() - i, 255));
      i += s.size();
      if (i != data.size())
        head[0] = (cla | 0x10);
      else
        head[0] = cla;

      apdu.set(&head, (BYTE)s.size(), &s, le == nullptr ? &emptyBa : &leBa);

      StatusWord sw = token.Transmit(apdu, &curresp);
      if (i == data.size()) {
        sw = getResp(curresp, sw, resp);

        return sw;
      }
    }
  } else {
    if (data.size() != 0)
      apdu.set(&head, (BYTE)data.size(), &data,
               le == nullptr ? &emptyBa : &leBa);
    else
      apdu.set(&head, le == nullptr ? &emptyBa : &leBa);

    StatusWord sw = token.Transmit(apdu, &curresp);

    sw = getResp(curresp, sw, resp);

    return sw;
  }
  exit_func
}

void IAS::InitDHParam() {
  init_func ByteDynArray resp;

  CASNParser parser;
  StatusWord sw;

  if (type == CIE_Type::CIE_Gemalto) {
    uint8_t getDHDoup[] = {00, 0xcb, 0x3f, 0xff};
    uint8_t getDHDuopData[] = {0x4d, 0x08, 0x70, 0x06, 0xBF,
                               0xA1, 0x01, 0x02, 0xA3, 0x80};

    if ((sw = SendAPDU(VarToByteArray(getDHDoup), VarToByteArray(getDHDuopData),
                       resp)) != 0x9000)
      throw scard_error(sw);

    parser.Parse(resp);

    dh_g = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;
    dh_p = parser.tags[0]->tags[0]->tags[0]->tags[1]->content;
    dh_q = parser.tags[0]->tags[0]->tags[0]->tags[2]->content;
  } else if ((type > CIE_Type::CIE_Gemalto)) {
    uint8_t getDHDoup[] = {00, 0xcb, 0x3f, 0xff};
    uint8_t getDHDuopData_g[] = {0x4D, 0x0A, 0x70, 0x08, 0xBF, 0xA1,
                                 0x01, 0x04, 0xA3, 0x02, 0x97, 0x00};

    if ((sw = SendAPDU(VarToByteArray(getDHDoup),
                       VarToByteArray(getDHDuopData_g), resp)) != 0x9000)
      throw scard_error(sw);
    parser.Parse(resp);
    dh_g = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;

    uint8_t getDHDuopData_p[] = {0x4D, 0x0A, 0x70, 0x08, 0xBF, 0xA1,
                                 0x01, 0x04, 0xA3, 0x02, 0x98, 0x00};
    if ((sw = SendAPDU(VarToByteArray(getDHDoup),
                       VarToByteArray(getDHDuopData_p), resp)) != 0x9000)
      throw scard_error(sw);
    parser.Parse(resp);
    dh_p = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;

    uint8_t getDHDuopData_q[] = {0x4D, 0x0A, 0x70, 0x08, 0xBF, 0xA1,
                                 0x01, 0x04, 0xA3, 0x02, 0x99, 0x00};
    if ((sw = SendAPDU(VarToByteArray(getDHDoup),
                       VarToByteArray(getDHDuopData_q), resp)) != 0x9000)
      throw scard_error(sw);
    parser.Parse(resp);
    dh_q = parser.tags[0]->tags[0]->tags[0]->tags[0]->content;
  } else
    throw logged_error("InitDHParam - CIE type not recognizes");

  exit_func
}

CASNTag *GetTag(CASNTagArray &tags, DWORD id) {
  for (std::size_t i = 0; i < tags.size(); i++) {
    if (tags[i]->tagInt() == id) return tags[i].get();
  }
  return nullptr;
}

void IAS::InitExtAuthKeyParam() {
  init_func ByteDynArray resp;

  uint8_t getKeyDoup[] = {00, 0xcb, 0x3f, 0xff};
  uint8_t getKeyDuopData[] = {
      0x4d, 0x09, 0x70, 0x07, 0xBF, 0xA0, CIE_KEY_ExtAuth_ID & 0x7f,
      0x03, 0x7F, 0x49, 0x80};
  StatusWord sw;
  if ((sw = SendAPDU(VarToByteArray(getKeyDoup), VarToByteArray(getKeyDuopData),
                     resp)) != 0x9000)
    throw scard_error(sw);

  CASNParser parser;
  parser.Parse(resp);

  CA_module = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x81)->content;
  CA_pubexp = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x82)->content;
  CA_privexp = baExtAuth_PrivExp;
  CA_CHR = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x5F20)->content;
  CA_CHA = GetTag(parser.tags[0]->tags[0]->tags[0]->tags, 0x5F4C)->content;
  CA_CAR = CA_CHR.mid(4);
  CA_AID = CA_CHA.left(6);
}

void IAS::SetCardContext(void *pCardData) {
  token.setTransmitCallbackData(pCardData);
}

void IAS::Deauthenticate() { init_func token.Reset(true); }

extern uint8_t encMod[];
extern uint8_t encPriv[];
extern uint8_t encPub[];

void IAS::InitEncKey() {
  // uso la chiave di intAuth per i servizi per decifrare il certificato
  init_func std::string strPAN;
  dumpHexData(PAN.mid(5, 6), strPAN, false);

  uint8_t mseSet[] = {0x00, 0x22, 0x41, 0xa4};
  uint8_t mseSetData[] = {0x80, 0x01, 0x02, 0x84, 0x01, 0x83};
  ByteDynArray resp;
  StatusWord sw;
  if (sessSSC.isEmpty()) {
    if ((sw = SendAPDU(VarToByteArray(mseSet), VarToByteArray(mseSetData),
                       resp)) != 0x9000) {
      LOG_DEBUG("IAS::InitEncKey - sendapdu1 error: %x", sw);
      throw scard_error(sw);
    }
    uint8_t intAuth[] = {0x00, 0x88, 0x00, 0x00};
    if ((sw = SendAPDU(VarToByteArray(intAuth),
                       ByteArray((BYTE *)strPAN.c_str(), strPAN.length()),
                       resp)) != 0x9000) {
      LOG_DEBUG("IAS::InitEncKey - sendapdu2 error: %x", sw);
      throw scard_error(sw);
    }
  } else {
    if ((sw = SendAPDU_SM(VarToByteArray(mseSet), VarToByteArray(mseSetData),
                          resp)) != 0x9000) {
      LOG_DEBUG("IAS::InitEncKey - sendapdu3 error: %x", sw);
      throw scard_error(sw);
    }
    uint8_t intAuth[] = {0x00, 0x88, 0x00, 0x00};
    if ((sw = SendAPDU_SM(VarToByteArray(intAuth),
                          ByteArray((BYTE *)strPAN.c_str(), strPAN.length()),
                          resp)) != 0x9000) {
      LOG_DEBUG("IAS::InitEncKey - sendapdu4 error: %x", sw);
      throw scard_error(sw);
    }
  }

  CSHA512 sha512;
  auto cardSeed = sha512.Digest(resp);
  CardEncKey = cardSeed.left(32);
  CardEncIv = cardSeed.mid(32).left(16);
}

void IAS::SetCache(const char *PAN, ByteArray &certificate,
                   ByteArray &FirstPIN) {
  init_func ByteDynArray encCert, encPIN;
  CAES enc(CardEncKey, CardEncIv);

  encCert = enc.Encode(certificate);
  encPIN = enc.Encode(FirstPIN);
  CacheSetData(PAN, encCert.data(), (int)encCert.size(), encPIN.data(),
               (int)encPIN.size());
}

int integrity = 0;
bool IsLowIntegrity() { return integrity == 1; }

void IAS::IconaSbloccoPIN() { init_func }

void IAS::GetFirstPIN(ByteDynArray &PIN) {
  init_func std::string PANStr;
  dumpHexData(PAN.mid(5, 6), PANStr, false);
  std::vector<BYTE> EncPINBuf;
  CacheGetPIN(PANStr.c_str(), EncPINBuf);

  CAES enc(CardEncKey, CardEncIv);
  PIN = enc.Decode(ByteArray(EncPINBuf.data(), EncPINBuf.size()));
}

bool IAS::IsEnrolled() {
  init_func std::string PANStr;
  dumpHexData(PAN.mid(5, 6), PANStr, false);
  return CacheExists(PANStr.c_str());
}

bool IAS::IsEnrolled(const char *szPAN) { init_func return CacheExists(szPAN); }

bool IAS::Unenroll(const char *szPAN) { init_func return CacheRemove(szPAN); }

bool IAS::Unenroll() {
  init_func std::string PANStr;
  dumpHexData(PAN.mid(5, 6), PANStr, false);
  return CacheRemove(PANStr.c_str());
}

void IAS::GetCertificate(ByteDynArray &certificate, bool askEnable) {
  init_func if (!Certificate.isEmpty()) {
    certificate = Certificate;
    return;
  }

  std::string PANStr;
  dumpHexData(PAN.mid(5, 6), PANStr, false);
  if (!CacheExists(PANStr.c_str())) {
    if (askEnable) {
      notifyCardNotRegistered(PANStr.c_str());
      return;
    } else {
      certificate.clear();
      return;
    }
  }

  std::vector<BYTE> certEncBuf;
  CacheGetCertificate(PANStr.c_str(), certEncBuf);

  CAES enc(CardEncKey, CardEncIv);
  certificate = enc.Decode(ByteArray(certEncBuf.data(), certEncBuf.size()));
  Certificate = certificate;
}

uint8_t IAS::GetSODDigestAlg(ByteArray &SOD) {
  CASNParser parser;
  uint8_t OID_SHA512[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03};
  uint8_t OID_SHA256[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01};

  parser.Parse(SOD);

  std::string dump;
  dumpHexData(SOD, dump);

  CASNTag &SODTag = *parser.tags[0];

  CASNTag &temp = SODTag.Child(0, 0x30);

  CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);

  auto &digestAlgo = temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).content;

  if (digestAlgo == VarToByteArray(OID_SHA256)) {
    LOG_DEBUG("GetSODDigestAlg - SOD Digest Algo: SHA256");
    return 1;
  } else if (digestAlgo == VarToByteArray(OID_SHA512)) {
    LOG_DEBUG("GetSODDigestAlg - SOD Digest Algo: SHA512");
    return 2;
  } else
    throw logged_error("GetSODDigestAlg - Digest algorithm not supported");
}

void IAS::VerificaSODPSS(ByteArray &SOD,
                         std::map<uint8_t, ByteDynArray> &hashSet) {
  init_func CASNParser parser;
  parser.Parse(SOD);

  std::string dump;
  dumpHexData(SOD, dump);

  CASNTag &SODTag = *parser.tags[0];

  CASNTag &temp = SODTag.Child(0, 0x30);

  /* Verifica OID contentInfo */
  uint8_t OID[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02};
  temp.Child(0, 06).Verify(VarToByteArray(OID));

  uint8_t val3 = 3;
  CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);
  temp2.Child(0, 2).Verify(VarToByteArray(val3));

  uint8_t OID_SHA512[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03};
  temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).Verify(
      VarToByteArray(OID_SHA512));

  uint8_t OID3[] = {0x67, 0x81, 0x08, 0x01, 0x01, 0x01};
  temp2.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID3));

  /* Prendo gli sha512 dei DG letti */
  ByteArray ttData = temp2.Child(2, 0x30).Child(1, 0xA0).Child(0, 04).content;

  CASNParser ttParser;
  ttParser.Parse(ttData);
  CASNTag &signedData = *ttParser.tags[0];
  signedData.CheckTag(0x30);

  /* Prendo il certificato del signer */
  CASNTag &signerCert = temp2.Child(3, 0xA0).Child(0, 0x30);

  CASNTag &temp3 = temp2.Child(4, 0x31).Child(0, 0x30);
  uint8_t val1 = 1;
  temp3.Child(0, 02).Verify(VarToByteArray(val1));

  CASNTag &issuerName = temp3.Child(1, 0x30).Child(0, 0x30);

  temp3.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SHA512));

  CASNTag &signerInfo = temp3.Child(3, 0xA0);

  uint8_t OID4[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x03};
  signerInfo.Child(0, 0x30).Child(0, 06).Verify(VarToByteArray(OID4));

  uint8_t OID5[] = {0x67, 0x81, 0x08, 0x01, 0x01, 0x01};
  signerInfo.Child(0, 0x30).Child(1, 0x31).Child(0, 06).Verify(
      VarToByteArray(OID5));

  uint8_t OID6[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x04};
  signerInfo.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID6));

  CASNTag &digest =
      temp3.Child(3, 0xA0).Child(1, 0x30).Child(1, 0x31).Child(0, 04);
  ByteArray digest_ =
      SOD.mid((int)digest.startPos, (int)(digest.endPos - digest.startPos));

  uint8_t OID_RSAPSS[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A};
  auto &signAlgo = temp3.Child(4, 0x30).Child(0, 06).content;
  auto &digestAlgo = temp3.Child(4, 0x30)
                         .Child(1, 0x30)
                         .Child(0, 0xA0)
                         .Child(0, 0x30)
                         .Child(0, 06)
                         .content;

  if (digestAlgo != VarToByteArray(OID_SHA512))
    throw logged_error("VerificaSODPSS - Digest algorithm not valid");
  if (signAlgo != VarToByteArray(OID_RSAPSS))
    throw logged_error("VerificaSODPSS - Digest algorithm not valid");

  CSHA512 sha512;
  ByteArray toHash = ttData.mid((int)signedData.startPos,
                                (int)(signedData.endPos - signedData.startPos));
  ByteDynArray calcDigest_ = sha512.Digest(toHash);

  if (calcDigest_ != digest.content)
    throw logged_error(
        "VerificaSODPSS - SOD Digest doesn't correspond to data");

  CASNTag &signature = temp3.Child(5, 04);

  ByteArray certRaw = SOD.mid((int)signerCert.startPos,
                              (int)(signerCert.endPos - signerCert.startPos));

  CryptoPP::ByteQueue certin;
  certin.Put(certRaw.data(), certRaw.size());
  CryptoPP::ByteQueue pbKey;
  CryptoPP::ByteQueue issuer;
  CryptoPP::Integer serial;

  GetPublicKeyFromCert(certin, pbKey, issuer, serial);

  ByteDynArray pubKeyData(pbKey.CurrentSize());
  pbKey.Get(pubKeyData.data(), pubKeyData.size());

  CASNParser pubKeyParser;
  pubKeyParser.Parse(pubKeyData);
  CASNTag &pubKey = *pubKeyParser.tags[0]->tags[1];

  ByteArray content = pubKey.content;

  if (content.data()[0] == 0)  // unsigned bits
    content = content.mid(1);

  CASNParser pubKeyParser2;
  pubKeyParser2.Parse(content);
  CASNTag &pubKey2 = *pubKeyParser2.tags[0];

  CASNTag &modTag = pubKey2.Child(0, 02);
  ByteArray mod = modTag.content;
  while (mod[0] == 0) mod = mod.mid(1);
  CASNTag &expTag = pubKey2.Child(1, 02);
  ByteArray exp = expTag.content;
  while (exp[0] == 0) exp = exp.mid(1);

  ByteArray signatureData = signature.content;

  CRSA rsa(mod, exp);

  ByteArray toSign =
      SOD.mid((int)signerInfo.tags[0]->startPos,
              (int)(signerInfo.tags[signerInfo.tags.size() - 1]->endPos -
                    signerInfo.tags[0]->startPos));

  ByteDynArray toSignBa = toSign.getASN1Tag(0x31);
  ByteArray ba(toSignBa.data(), toSignBa.size());

  bool result = rsa.RSA_PSS(signatureData, ba);

  if (!result) {
    throw logged_error("VerificaSODPSS - SOD Sign not valid");
  }

  issuerName.Reparse();
  CASNParser issuerParser;

  ByteDynArray issuerBa(issuer.CurrentSize());
  issuer.Get(issuerBa.data(), issuerBa.size());

  issuerParser.Parse(issuerBa);

  CASNTag &CertIssuer = *issuerParser.tags[0];
  if (issuerName.tags.size() != CertIssuer.tags.size())
    printf("Issuer name non corrispondente");

  uint8_t val = 1;
  signedData.Child(0, 02).Verify(VarToByteArray(val));
  signedData.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SHA512));

  CASNTag &hashTag = signedData.Child(2, 0x30);
  for (std::size_t i = 0; i < hashTag.tags.size(); i++) {
    CASNTag &hashDG = *(hashTag.tags[i]);
    CASNTag &dgNum = hashDG.CheckTag(0x30).Child(0, 02);
    CASNTag &dgHash = hashDG.Child(1, 04);
    uint8_t num = ByteArrayToVar(dgNum.content, BYTE);

    if (hashSet.find(num) == hashSet.end() || hashSet[num].size() == 0)
      throw logged_error(stdPrintf("Digest non trovato per il DG %02X", num));

    if (hashSet[num] != dgHash.content)
      throw logged_error(
          stdPrintf("VerificaSODPSS - Digest for DG %02X not found", num));
  }

  exit_func
}

void IAS::VerificaSOD(ByteArray &SOD, std::map<BYTE, ByteDynArray> &hashSet) {
  init_func CASNParser parser;
  parser.Parse(SOD);

  std::string dump;
  dumpHexData(SOD, dump);

  CASNTag &SODTag = *parser.tags[0];

  CASNTag &temp = SODTag.Child(0, 0x30);
  uint8_t OID[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02};

  temp.Child(0, 06).Verify(VarToByteArray(OID));
  uint8_t val3 = 3;
  CASNTag &temp2 = temp.Child(1, 0xA0).Child(0, 0x30);
  temp2.Child(0, 2).Verify(VarToByteArray(val3));

  uint8_t OID_SH256[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01};
  temp2.Child(1, 0x31).Child(0, 0x30).Child(0, 6).Verify(
      VarToByteArray(OID_SH256));

  uint8_t OID3[] = {0x67, 0x81, 0x08, 0x01, 0x01, 0x01};
  temp2.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID3));
  ByteArray ttData = temp2.Child(2, 0x30).Child(1, 0xA0).Child(0, 04).content;
  CASNParser ttParser;
  ttParser.Parse(ttData);
  CASNTag &signedData = *ttParser.tags[0];
  signedData.CheckTag(0x30);

  CASNTag &signerCert = temp2.Child(3, 0xA0).Child(0, 0x30);

  CASNTag &temp3 = temp2.Child(4, 0x31).Child(0, 0x30);
  uint8_t val1 = 1;
  temp3.Child(0, 02).Verify(VarToByteArray(val1));
  CASNTag &issuerName = temp3.Child(1, 0x30).Child(0, 0x30);
  temp3.Child(2, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SH256));

  CASNTag &signerInfo = temp3.Child(3, 0xA0);

  uint8_t OID4[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x03};
  signerInfo.Child(0, 0x30).Child(0, 06).Verify(VarToByteArray(OID4));
  uint8_t OID5[] = {0x67, 0x81, 0x08, 0x01, 0x01, 0x01};
  signerInfo.Child(0, 0x30).Child(1, 0x31).Child(0, 06).Verify(
      VarToByteArray(OID5));
  uint8_t OID6[] = {0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x04};
  signerInfo.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID6));
  CASNTag &digest =
      temp3.Child(3, 0xA0).Child(1, 0x30).Child(1, 0x31).Child(0, 04);

  uint8_t OID_RSAwithSHA256[] = {0x2A, 0x86, 0x48, 0x86, 0xF7,
                                 0x0D, 0x01, 0x01, 0x0b};
  uint8_t OID_RSAwithSHA1[] = {0x2a, 0x86, 0x48, 0x86, 0xf7,
                               0x0d, 0x01, 0x01, 0x05};
  auto &digestAlgo = temp3.Child(4, 0x30).Child(0, 06).content;
  bool isSHA1 = false;
  bool isSHA256 = false;
  if (digestAlgo == VarToByteArray(OID_RSAwithSHA1))
    isSHA1 = true;
  else if (digestAlgo == VarToByteArray(OID_RSAwithSHA256))
    isSHA256 = true;
  else
    throw logged_error("VerificaSOD - Digest algorithm not valid");

  CASNTag &signature = temp3.Child(5, 04);

  ByteArray toHash = ttData.mid((int)signedData.startPos,
                                (int)(signedData.endPos - signedData.startPos));
  CSHA256 sha256;
  ByteDynArray calcDigest = sha256.Digest(toHash);
  if (calcDigest != digest.content)
    throw logged_error("VerificaSOD - SOD digest does not match with data");

  ByteArray certRaw = SOD.mid((int)signerCert.startPos,
                              (int)(signerCert.endPos - signerCert.startPos));

  CryptoPP::ByteQueue certin;
  certin.Put(certRaw.data(), certRaw.size());
  CryptoPP::ByteQueue pbKey;
  CryptoPP::ByteQueue issuer;
  CryptoPP::Integer serial;

  GetPublicKeyFromCert(certin, pbKey, issuer, serial);

  ByteDynArray pubKeyData(pbKey.CurrentSize());
  pbKey.Get(pubKeyData.data(), pubKeyData.size());

  CASNParser pubKeyParser;
  pubKeyParser.Parse(pubKeyData);
  CASNTag &pubKey = *pubKeyParser.tags[0]->tags[1];

  ByteArray content = pubKey.content;

  if (content.data()[0] == 0)  // unsigned bits
    content = content.mid(1);

  CASNParser pubKeyParser2;
  pubKeyParser2.Parse(content);
  CASNTag &pubKey2 = *pubKeyParser2.tags[0];

  CASNTag &modTag = pubKey2.Child(0, 02);
  ByteArray mod = modTag.content;
  while (mod[0] == 0) mod = mod.mid(1);
  CASNTag &expTag = pubKey2.Child(1, 02);
  ByteArray exp = expTag.content;
  while (exp[0] == 0) exp = exp.mid(1);

  ByteArray signatureData = signature.content;

  CRSA rsa(mod, exp);

  ByteDynArray decryptedSignature = rsa.RSA_PURE(signatureData);
  decryptedSignature =
      decryptedSignature.mid(RemovePaddingBT1(decryptedSignature));

  ByteArray toSign =
      SOD.mid((int)signerInfo.tags[0]->startPos,
              (int)(signerInfo.tags[signerInfo.tags.size() - 1]->endPos -
                    signerInfo.tags[0]->startPos));

  ByteDynArray digestSignature;
  if (isSHA1) {
    CSHA1 sha1;
    decryptedSignature = decryptedSignature.mid(RemoveSha1(decryptedSignature));
    digestSignature = sha1.Digest(toSign.getASN1Tag(0x31));
  }
  if (isSHA256) {
    CSHA256 sha256;
    decryptedSignature =
        decryptedSignature.mid(RemoveSha256(decryptedSignature));

    ByteDynArray toSignBa = toSign.getASN1Tag(0x31);
    ByteArray ba(toSignBa.data(), toSignBa.size());

    digestSignature = sha256.Digest(ba);
  }
  if (digestSignature != decryptedSignature)
    throw logged_error("VerificaSOD - SOD sign not valid");

  issuerName.Reparse();
  CASNParser issuerParser;

  ByteDynArray issuerBa(issuer.CurrentSize());
  issuer.Get(issuerBa.data(), issuerBa.size());

  issuerParser.Parse(issuerBa);

  CASNTag &CertIssuer = *issuerParser.tags[0];
  if (issuerName.tags.size() != CertIssuer.tags.size())
    printf("Issuer name non corrispondente");

  uint8_t val0 = 0;
  signedData.Child(0, 02).Verify(VarToByteArray(val0));
  signedData.Child(1, 0x30).Child(0, 06).Verify(VarToByteArray(OID_SH256));

  CASNTag &hashTag = signedData.Child(2, 0x30);
  for (std::size_t i = 0; i < hashTag.tags.size(); i++) {
    CASNTag &hashDG = *(hashTag.tags[i]);
    CASNTag &dgNum = hashDG.CheckTag(0x30).Child(0, 02);
    CASNTag &dgHash = hashDG.Child(1, 04);
    uint8_t num = ByteArrayToVar(dgNum.content, BYTE);

    if (hashSet.find(num) == hashSet.end() || hashSet[num].size() == 0)
      printf("%s", stdPrintf("Digest non trovato per il DG %02X", num).c_str());

    if (hashSet[num] != dgHash.content)
      printf(
          "%s",
          stdPrintf("Digest non corrispondente per il DG %02X", num).c_str());
  }

  exit_func
}

#define DWL_MSGRESULT 0