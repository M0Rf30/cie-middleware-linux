/*
 *  CCrl.h
 *  iDigitalSApp
 *
 *  Created by svp on 15/07/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _CRL_H
#define _CRL_H

#include "ASN1/ASN1Integer.h"
#include "ASN1/ASN1Sequence.h"
#include "Sign/disigonsdk.h"

class CCrl : public CASN1Sequence {
 public:
  CCrl(UUCBufferedReader& reader);

  CCrl(const CASN1Object& contentInfo);

  bool isRevoked(const CASN1Integer& serialNumber, const char* szDateTime,
                 int* pReason, REVOCATION_INFO* pRevocationInfo);
};

#endif  //_CRL_H
