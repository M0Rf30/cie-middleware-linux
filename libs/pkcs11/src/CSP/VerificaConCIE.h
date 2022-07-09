//
//  VerificaConCIE.h
//  cie-pkcs11
//
//  Copyright © 2021 IPZS. All rights reserved.
//

#ifndef  CSP_VERIFICACONCIE_H_
#define  CSP_VERIFICACONCIE_H_

#include <stdio.h>

#include "../Sign/CIEVerify.h"
#include "AbilitaCIE.h"

typedef CK_RV (*verificaConCIEfn)(const char* inFilePath);
typedef CK_RV (*getNumberOfSignfn)(void);
typedef CK_RV (*getVerifyInfofn)(int index, struct verifyInfo_t* vInfos);
typedef CK_RV (*estraiP7mfn)(const char* inFilePath, const char* outFilePath);
#endif  // CSP_VERIFICACONCIE_H_
