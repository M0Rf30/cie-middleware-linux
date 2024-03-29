// CertificateInfo.h: interface for the CCertificateInfo class.

#if !defined( \
    AFX_CERTIFICATEINFO_H__BC0C3782_D000_44C9_B3CE_532326CA26F6__INCLUDED_)
#define AFX_CERTIFICATEINFO_H__BC0C3782_D000_44C9_B3CE_532326CA26F6__INCLUDED_

#include "ASN1/ASN1Integer.h"
#include "ASN1/ASN1Sequence.h"
#include "ASN1/ASN1UTCTime.h"
#include "ASN1/AlgorithmIdentifier.h"
#include "Name.h"
#include "SubjectPublicKeyInfo.h"

class CCertificateInfo : public CASN1Sequence {
 public:
  CCertificateInfo(UUCBufferedReader& reader);

  CCertificateInfo(const CASN1Object& cert);

  virtual ~CCertificateInfo();

  CASN1Integer getVersion();

  CASN1Integer getSerialNumber();

  CAlgorithmIdentifier getSignatureAlgo();

  CName getIssuer();

  CASN1UTCTime getExpiration();
  CASN1UTCTime getFrom();

  CName getSubject();

  CSubjectPublicKeyInfo getSubjectPublicKeyInfo();

  CASN1Sequence getExtensions();
};

#endif  // !defined(AFX_CERTIFICATEINFO_H__BC0C3782_D000_44C9_B3CE_532326CA26F6__INCLUDED_)
