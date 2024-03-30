//
//  VerificaConCIE.h
//  cie-pkcs11
//
//  Copyright © 2021 IPZS. All rights reserved.
//

#ifndef LIBS_PKCS11_SRC_CSP_VERIFICACONCIE_H_
#define LIBS_PKCS11_SRC_CSP_VERIFICACONCIE_H_

#include <stdio.h>
#include <sys/types.h>

#include "CSP/AbilitaCIE.h"
#include "Sign/CIEVerify.h"

typedef CK_RV (*verificaConCIEfn)(const char* inFilePath);
typedef CK_RV (*getNumberOfSignfn)(void);
typedef CK_RV (*getVerifyInfofn)(u_int64_t index, struct verifyInfo_t* vInfos);
typedef CK_RV (*estraiP7mfn)(const char* inFilePath, const char* outFilePath);
#endif  // LIBS_PKCS11_SRC_CSP_VERIFICACONCIE_H_
