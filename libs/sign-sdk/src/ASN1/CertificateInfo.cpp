// CertificateInfo.cpp: implementation of the CCertificateInfo class.
#include "CertificateInfo.h"

// Construction/Destruction

CCertificateInfo::CCertificateInfo(UUCBufferedReader& reader)
    : CASN1Sequence(reader) {}

CCertificateInfo::CCertificateInfo(const CASN1Object& certInfo)
    : CASN1Sequence(certInfo) {}

CCertificateInfo::~CCertificateInfo() {}

CASN1Integer CCertificateInfo::getSerialNumber() {
  return (CASN1Integer)elementAt(1);
}

CAlgorithmIdentifier CCertificateInfo::getSignatureAlgo() {
  return (CAlgorithmIdentifier)elementAt(2);
}

CName CCertificateInfo::getIssuer() { return (CName)elementAt(3); }

CASN1UTCTime CCertificateInfo::getExpiration() {
  return CASN1Sequence(elementAt(4)).elementAt(1);
}

CASN1UTCTime CCertificateInfo::getFrom() {
  return CASN1Sequence(elementAt(4)).elementAt(0);
}

CASN1Integer CCertificateInfo::getVersion() {
  return CASN1Sequence(elementAt(0)).elementAt(0);
}

CName CCertificateInfo::getSubject() { return (CName)elementAt(5); }

CSubjectPublicKeyInfo CCertificateInfo::getSubjectPublicKeyInfo() {
  return elementAt(6);
}

CASN1Sequence CCertificateInfo::getExtensions() {
  int count = size();
  return elementAt(count - 1);
}
