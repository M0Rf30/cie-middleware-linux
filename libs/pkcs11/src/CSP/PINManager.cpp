//
//  PINManager.cpp
//  cie-pkcs11
//
//  Created by ugo chirico on 06/10/18. http://www.ugochirico.com
//  Copyright © 2018 IPZS. All rights reserved.
//

#include "CSP/PINManager.h"

#include <cryptopp/misc.h>

#include <string>

#include "CSP/AbilitaCIE.h"
#include "CSP/IAS.h"
#include "LOGGER/Logger.h"
#include "PCSC/PCSC.h"
#include "PKCS11/PKCS11Functions.h"
#include "Util/ModuleInfo.h"

using namespace CieIDLogger;

extern "C" {
CK_RV CK_ENTRY CambioPIN(const char* szCurrentPIN, const char* szNuovoPIN,
                         int* pAttempts, PROGRESS_CALLBACK progressCallBack);
CK_RV CK_ENTRY SbloccoPIN(const char* szPUK, const char* szNuovoPIN,
                          int* pAttempts, PROGRESS_CALLBACK progressCallBack);
}

int TokenTransmitCallback(safeConnection* data, uint8_t* apdu, DWORD apduSize,
                          uint8_t* resp, DWORD* respSize);

CK_RV CK_ENTRY CambioPIN(const char* szCurrentPIN, const char* szNewPIN,
                         int* pAttempts, PROGRESS_CALLBACK progressCallBack) {
  char* readers = NULL;
  char* ATR = NULL;

  LOG_INFO("******** Starting PINManager::ChangePIN ********");
  // verifica bontà PIN
  if (szCurrentPIN == NULL || strnlen(szCurrentPIN, 9) != 8)
    return CKR_PIN_LEN_RANGE;

  if (szNewPIN == NULL || strnlen(szNewPIN, 9) != 8) return CKR_PIN_LEN_RANGE;
  try {
    DWORD len = 0;

    SCARDCONTEXT hSC;

    progressCallBack(10, "Connessione alla CIE");

    LOG_DEBUG("PINManager::ChangePIN - SCardEstablishContext");
    long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
    if (nRet != SCARD_S_SUCCESS) {
      LOG_ERROR("PINManager::ChangePIN - res: %d", nRet);
      return CKR_DEVICE_ERROR;
    }

    LOG_DEBUG("PINManager::ChangePIN - SCardListReaders");

    nRet = SCardListReaders(hSC, nullptr, NULL, &len);
    if (nRet != SCARD_S_SUCCESS) {
      LOG_ERROR("PINManager::ChangePIN - res: %d", nRet);
      return CKR_TOKEN_NOT_PRESENT;
    }

    if (len == 1) {
      LOG_ERROR("PINManager::ChangePIN - No readers");
      return CKR_TOKEN_NOT_PRESENT;
    }

    readers = (char*)malloc(len);

    nRet = SCardListReaders(hSC, nullptr, (char*)readers, &len);
    if (nRet != SCARD_S_SUCCESS) {
      free(readers);
      return CKR_TOKEN_NOT_PRESENT;
    }

    LOG_INFO("PINManager::ChangePIN - CIE connected");
    progressCallBack(10, "CIE Connessa");

    char* curreader = readers;
    bool foundCIE = false;

    for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1) {
      safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
      if (!conn.hCard) continue;

      DWORD atrLen = 40;
      nRet = SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR,
                            &atrLen);
      if (nRet != SCARD_S_SUCCESS) {
        LOG_ERROR("PINManager::ChangePIN - SCardGetAttrib err: %d", nRet);
        free(readers);
        return CKR_DEVICE_ERROR;
      }

      ATR = (char*)malloc(atrLen);

      nRet = SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR,
                            &atrLen);
      if (nRet != SCARD_S_SUCCESS) {
        LOG_ERROR("PINManager::ChangePIN - SCardGetAttrib err: %d", nRet);
        free(readers);
        free(ATR);
        return CKR_DEVICE_ERROR;
      }

      ByteArray atrBa((BYTE*)ATR, atrLen);

      IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
      ias.SetCardContext(&conn);
      ias.attemptsRemaining = -1;

      ias.token.Reset();
      // Continue looking for CIE if the token is unrecognised
      try {
        ias.SelectAID_IAS();
      } catch (logged_error& err) {
        free(ATR);
        ATR = NULL;
        continue;
      }
      ias.ReadPAN();

      progressCallBack(20, "Lettura dati dalla CIE");

      ByteDynArray resp;
      ias.SelectAID_CIE();

      ias.InitEncKey();
      ias.ReadDappPubKey(resp);

      foundCIE = true;

      // leggo i parametri di dominio DH e della chiave di extauth
      ias.InitDHParam();

      ias.InitExtAuthKeyParam();

      progressCallBack(40, "Autenticazione...");

      ias.DHKeyExchange();

      // DAPP
      ias.DAPP();

      progressCallBack(80, "Cambio PIN...");

      ByteArray oldPINBa((BYTE*)szCurrentPIN, strlen(szCurrentPIN));

      StatusWord sw = ias.VerifyPIN(oldPINBa);
      LOG_INFO("PINManager::VerifyPIN verify PIN status: %02X", sw);

      if (sw == 0x6983) {
        free(readers);
        free(ATR);
        return CKR_PIN_LOCKED;
      }
      if (sw >= 0x63C0 && sw <= 0x63CF) {
        if (pAttempts != nullptr) *pAttempts = sw - 0x63C0;

        free(readers);
        free(ATR);
        return CKR_PIN_INCORRECT;
      }

      if (sw == 0x6700) {
        free(readers);
        free(ATR);
        return CKR_PIN_INCORRECT;
      }
      if (sw == 0x6300) {
        free(readers);
        free(ATR);
        return CKR_PIN_INCORRECT;
      }
      if (sw != 0x9000) {
        throw scard_error(sw);
      }

      ByteDynArray cert;
      bool isEnrolled = ias.IsEnrolled();

      if (isEnrolled) ias.GetCertificate(cert);

      ByteArray newPINBa((BYTE*)szNewPIN, strlen(szNewPIN));

      sw = ias.ChangePIN(oldPINBa, newPINBa);
      LOG_INFO("PINManager::ChangePIN change PIN status: %02X", sw);
      if (sw != 0x9000) {
        throw scard_error(sw);
      }

      if (isEnrolled) {
        std::string strPAN;
        dumpHexData(ias.PAN.mid(5, 6), strPAN, false);
        ByteArray leftPINBa = newPINBa.left(4);
        ias.SetCache(strPAN.c_str(), cert, leftPINBa);
      }

      progressCallBack(100, "Cambio PIN eseguito");
      LOG_INFO("******** PINManager::ChangePIN Completed ********");

      // A this point a CIE has been found, stop looking for it
      break;
    }

    if (!foundCIE) {
      free(readers);
      free(ATR);
      return CKR_TOKEN_NOT_RECOGNIZED;
    }
  } catch (...) {
    if (readers) free(readers);
    if (ATR) free(ATR);
    return CKR_GENERAL_ERROR;
  }

  if (readers) free(readers);
  if (ATR) free(ATR);

  return CKR_OK;
}

CK_RV CK_ENTRY SbloccoPIN(const char* szPUK, const char* szNewPIN,
                          int* pAttempts, PROGRESS_CALLBACK progressCallBack) {
  char* readers = NULL;
  char* ATR = NULL;
  LOG_INFO("******** Starting PINManager::SbloccoPIN ********");
  // verifica bontà PIN
  if (szPUK == NULL || strnlen(szPUK, 9) != 8) return CKR_PIN_LEN_RANGE;

  if (szNewPIN == NULL || strnlen(szNewPIN, 9) != 8) return CKR_PIN_LEN_RANGE;
  try {
    DWORD len = 0;

    SCARDCONTEXT hSC;

    progressCallBack(10, "Connessione alla CIE");

    LOG_DEBUG("PINManager::UnlockPIN - SCardEstablishContext");
    long nRet = SCardEstablishContext(SCARD_SCOPE_USER, nullptr, nullptr, &hSC);
    if (nRet != SCARD_S_SUCCESS) {
      LOG_ERROR("PINManager::UnlockPIN - SCardEstablishContext err: %d", nRet);
      return CKR_DEVICE_ERROR;
    }

    if (SCardListReaders(hSC, nullptr, NULL, &len) != SCARD_S_SUCCESS) {
      LOG_ERROR("PINManager::UnlockPIN - SCardEstablishContext err: %d", nRet);
      return CKR_TOKEN_NOT_PRESENT;
    }

    if (len == 1) {
      LOG_ERROR("PINManager::UnlockPIN - No readers");
      return CKR_TOKEN_NOT_PRESENT;
    }

    readers = (char*)malloc(len);

    if (SCardListReaders(hSC, nullptr, (char*)readers, &len) !=
        SCARD_S_SUCCESS) {
      free(readers);
      return CKR_TOKEN_NOT_PRESENT;
    }

    LOG_INFO("PINManager::UnlockPIN - CIE connected");
    progressCallBack(20, "CIE Connessa");

    char* curreader = readers;
    bool foundCIE = false;

    for (; curreader[0] != 0; curreader += strnlen(curreader, len) + 1) {
      safeConnection conn(hSC, curreader, SCARD_SHARE_SHARED);
      if (!conn.hCard) continue;

      DWORD atrLen = 40;
      if (SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR,
                         &atrLen) != SCARD_S_SUCCESS) {
        LOG_ERROR("PINManager::UnlockPIN - SCardGetAttrib err: nRet");
        free(readers);
        return CKR_DEVICE_ERROR;
      }

      ATR = (char*)malloc(atrLen);

      if (SCardGetAttrib(conn.hCard, SCARD_ATTR_ATR_STRING, (uint8_t*)ATR,
                         &atrLen) != SCARD_S_SUCCESS) {
        free(readers);
        free(ATR);
        return CKR_DEVICE_ERROR;
      }

      ByteArray atrBa((BYTE*)ATR, atrLen);

      IAS ias((CToken::TokenTransmitCallback)TokenTransmitCallback, atrBa);
      ias.SetCardContext(&conn);
      ias.attemptsRemaining = -1;

      ias.token.Reset();
      // Continue looking for CIE if the token is unrecognised
      try {
        ias.SelectAID_IAS();
      } catch (logged_error& err) {
        free(ATR);
        ATR = NULL;
        continue;
      }
      ias.ReadPAN();

      progressCallBack(30, "Lettura dati dalla CIE");

      ByteDynArray resp;
      ias.SelectAID_CIE();

      ias.InitEncKey();
      ias.ReadDappPubKey(resp);

      foundCIE = true;

      // leggo i parametri di dominio DH e della chiave di extauth
      ias.InitDHParam();

      ias.InitExtAuthKeyParam();

      progressCallBack(50, "Autenticazione...");

      ias.DHKeyExchange();

      // DAPP
      ias.DAPP();

      progressCallBack(80, "Sblocco carta...");

      ByteArray pukBa((BYTE*)szPUK, strlen(szPUK));

      StatusWord sw = ias.VerifyPUK(pukBa);
      LOG_INFO("PINManager::UnlockPIN VerifyPUK status: %02X", sw);

      if (sw == 0x6983) {
        free(ATR);
        free(readers);
        return CKR_PIN_LOCKED;
      }
      if (sw >= 0x63C0 && sw <= 0x63CF) {
        free(ATR);
        free(readers);
        if (pAttempts != nullptr) *pAttempts = sw - 0x63C0;

        return CKR_PIN_INCORRECT;
      }

      if (sw == 0x6700) {
        free(ATR);
        free(readers);
        return CKR_PIN_INCORRECT;
      }
      if (sw == 0x6300) {
        free(ATR);
        free(readers);
        return CKR_PIN_INCORRECT;
      }
      if (sw != 0x9000) {
        throw scard_error(sw);
      }

      ByteDynArray cert;
      bool isEnrolled = ias.IsEnrolled();

      if (isEnrolled) ias.GetCertificate(cert);

      ByteArray newPINBa((BYTE*)szNewPIN, strlen(szNewPIN));

      sw = ias.ChangePIN(newPINBa);
      LOG_INFO("PINManager::UnlockPIN ChangePIN status: %02X", sw);
      if (sw != 0x9000) {
        throw scard_error(sw);
      }

      if (isEnrolled) {
        std::string strPAN;
        dumpHexData(ias.PAN.mid(5, 6), strPAN, false);
        ByteArray leftPINBa = newPINBa.left(4);
        ias.SetCache(strPAN.c_str(), cert, leftPINBa);
      }

      progressCallBack(100, "Sblocco carta eseguito");
      LOG_INFO("******** PINManager::UnlockPIN Completed ********");

      // A this point a CIE has been found, stop looking for it
      break;
    }

    if (!foundCIE) {
      free(ATR);
      free(readers);
      return CKR_TOKEN_NOT_RECOGNIZED;
    }

    LOG_INFO("******** PINManager::SbloccoPIN Completed ********");
  } catch (...) {
    if (ATR) free(ATR);

    if (readers) free(readers);

    return CKR_GENERAL_ERROR;
  }

  if (ATR) free(ATR);

  if (readers) free(readers);

  return CKR_OK;
}
