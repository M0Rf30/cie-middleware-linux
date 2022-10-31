// SignedData.cpp: implementation of the CSignedData class.
#include "SignedData.h"
#include "ASN1OptionalField.h"
#include "ASN1Exception.h"
#include "SignerInfo.h"
#include "RSAPublicKey.h"
#include "RSA/rsaeuro.h"
#include "RSA/rsa.h"
#include "RSA/sha2.h"
#include "DigestInfo.h"
#include <sys/types.h>
#include "ASN1/Certificate.h"
#include "ASN1/Crl.h"
#include <map>
#include "RSA/sha1.h"

// Construction/Destruction
CSignedData::CSignedData(UUCBufferedReader& reader) : CASN1Sequence(reader) {}

CSignedData::CSignedData(const CASN1Object& signedData)
    : CASN1Sequence(signedData) {}

CSignedData::CSignedData(const CASN1SetOf& algos,
                         const CContentInfo& contentInfo,
                         const CASN1SetOf& signerInfos,
                         const CASN1SetOf& certificates) {
  addElement(CASN1Integer(1));
  addElement(algos);
  addElement(contentInfo);

  // Optional fields
  addElement(CASN1OptionalField(certificates,
                                0));  // ExtendedCertificateAndCertificates

  // Signer Infos
  addElement(signerInfos);
}

CSignedData::~CSignedData() {}

CASN1SetOf CSignedData::getDigestAlgorithmIdentifiers() {
  return (CASN1SetOf)elementAt(1);
}

CContentInfo CSignedData::getContentInfo() {
  return (CContentInfo)elementAt(2);
}

CASN1SetOf CSignedData::getSignerInfos() { return (CASN1SetOf)elementAt(4); }

CCertificate CSignedData::getSignerCertificate(int index) {
  CASN1SetOf signerInfos = getSignerInfos();
  CASN1SetOf certificates = getCertificates();

  CSignerInfo sinfo = signerInfos.elementAt(index);

  CIssuerAndSerialNumber issuerAndSerialNumber =
      sinfo.getIssuerAndSerialNumber();

  for (unsigned int i = 0; i < certificates.size(); i++) {
    CCertificate cert = certificates.elementAt(i);
    CName issuer = cert.getIssuer();
    CName serialNumber = cert.getSerialNumber();

    CIssuerAndSerialNumber issuerAndSerial(issuer, serialNumber, false);

    if (issuerAndSerial == issuerAndSerialNumber) {
      return cert;
    }
  }

  throw -1;
}

int CSignedData::verify(int i) { return verify(i, NULL, NULL); }

int CSignedData::verify(int i, const char* date,
                        REVOCATION_INFO* pRevocationInfo) {
  if (getContentInfo().size() < 2)  // detached
    return -2;

  CASN1SetOf signerInfos = getSignerInfos();
  CSignerInfo signerInfo(signerInfos.elementAt(i));
  CASN1SetOf certificates = getCertificates();
  CASN1OctetString content = getContentInfo().getContent();
  return CSignerInfo::verifySignature(content, signerInfo, certificates, date,
                                      pRevocationInfo);
}

void CSignedData::makeDetached() {
  if (getContentInfo().size() == 2) getContentInfo().removeElementAt(1);
}

void CSignedData::setContent(UUCByteArray& content) {
  CASN1ObjectIdentifier dataOID(szDataOID);
  CASN1OctetString data(content);
  CContentInfo ci(dataOID, data);
  setElementAt(ci, 2);
}

CASN1SetOf CSignedData::getCertificates() { return elementAt(3); }
