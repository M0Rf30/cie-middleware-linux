#pragma once

#include <PCSC/wintypes.h>
#include <PCSC/winscard.h>

#include "PKCS11/cryptoki.h"

#define MAXVAL 0xffffff
#define MAXSESSIONS MAXVAL

#define CK_ENTRY
#define LIBRARY_VERSION_MAJOR 2
#define LIBRARY_VERSION_MINOR 0

#define PIN_LEN 8
#define USER_PIN_ID 0x10

#include "LOGGER/Logger.h"

using namespace CieIDLogger;

#define init_p11_func                    \
  LOG_INFO("[PKCS11] %s", __FUNCTION__); \
  try {
#define exit_p11_func                                             \
  }                                                               \
  catch (p11_error & p11Err) {                                    \
    LOG_ERROR("[PKCS11] EXC: %s", p11Err.what());                 \
    LOG_ERROR("[PKCS11] P11Error: %x", p11Err.getP11ErrorCode()); \
    return p11Err.getP11ErrorCode();                              \
  }                                                               \
  catch (std::exception & err) {                                  \
    LOG_ERROR("EXCLOG->");                                        \
    LOG_ERROR("EXC: %s", err.what());                             \
    LOG_ERROR("<-EXCLOG");                                        \
    return CKR_GENERAL_ERROR;                                     \
  }                                                               \
  catch (...) {                                                   \
    LOG_ERROR("%s, CKR_GENERAL_ERROR", __FUNCTION__);             \
    return CKR_GENERAL_ERROR;                                     \
  }

extern "C" {
CK_RV CK_ENTRY C_UpdateSlotList();
}
