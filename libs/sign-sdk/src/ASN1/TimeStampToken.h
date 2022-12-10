// TimeStampToken.h: interface for the CTimeStampToken class.
//
#ifndef _TIMESTAMPTOKEN_H
#define _TIMESTAMPTOKEN_H

#if !defined( \
    AFX_TIMESTAMPTOKEN_H__2D568B92_6258_4EBE_B1CA_17F3746F3801__INCLUDED_)
#define AFX_TIMESTAMPTOKEN_H__2D568B92_6258_4EBE_B1CA_17F3746F3801__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif  // _MSC_VER > 1000

#include "ASN1/ContentInfo.h"
#include "SignedData.h"
#include "TSTInfo.h"
#include "ASN1/TimeStampToken.h"

class CTimeStampToken : public CContentInfo {
  public:
    CTimeStampToken(UUCBufferedReader& reader);

    CTimeStampToken(const CASN1Object& timeStampToken);

    CTSTInfo getTSTInfo();

    virtual ~CTimeStampToken();

    int verify(REVOCATION_INFO* pRevocationInfo);
    int verify(const char* szDateTime, REVOCATION_INFO* pRevocationInfo);

    CASN1SetOf getCertificates();
};

#endif  // !defined(AFX_TIMESTAMPTOKEN_H__2D568B92_6258_4EBE_B1CA_17F3746F3801__INCLUDED_)
#endif  // TIMESTAMPTOKEN
