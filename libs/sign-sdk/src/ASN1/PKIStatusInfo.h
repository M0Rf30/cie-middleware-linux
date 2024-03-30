/*
 *  PKIStatusInfo.h
 *  iDigitalSApp
 *
 *  Created by svp on 19/03/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PKISTATUSINFO_H
#define _PKISTATUSINFO_H

#include "ASN1/ASN1Integer.h"
#include "ASN1/ASN1Object.h"
#include "ASN1/ASN1Sequence.h"
#include "ASN1/UUCBufferedReader.h"
class CPKIStatusInfo : public CASN1Sequence {
 public:
  CPKIStatusInfo(UUCBufferedReader& reader);

  CPKIStatusInfo(const CASN1Object& PKIStatusInfo);

  virtual ~CPKIStatusInfo();

  CASN1Integer getStatus();
};

#endif
