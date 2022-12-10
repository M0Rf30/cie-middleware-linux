/*
 *  OCSPRequest.cpp
 *  iDigitalSApp
 *
 *  Created by svp on 14/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

/*
 OCSPRequest     ::=     SEQUENCE {
 tbsRequest                  TBSRequest,
 optionalSignature   [0]     EXPLICIT Signature OPTIONAL }

 TBSRequest      ::=     SEQUENCE {
 version             [0] EXPLICIT Version DEFAULT v1,
 requestorName       [1] EXPLICIT GeneralName OPTIONAL,
 requestList             SEQUENCE OF Request,
 requestExtensions   [2] EXPLICIT Extensions OPTIONAL }

 Signature       ::=     SEQUENCE {
 signatureAlgorithm   AlgorithmIdentifier,
 signature            BIT STRING,
 certs                [0] EXPLICIT SEQUENCE OF Certificate OPTIONAL }

 Version  ::=  INTEGER  {  v1(0) }

 Request ::=     SEQUENCE {
 reqCert                    CertID,
 singleRequestExtensions    [0] EXPLICIT Extensions OPTIONAL }

 CertID ::= SEQUENCE {
 hashAlgorithm            AlgorithmIdentifier,
 issuerNameHash     OCTET STRING, -- Hash of Issuer's DN
 issuerKeyHash      OCTET STRING, -- Hash of Issuers public key
 serialNumber       CertificateSerialNumber }

 issuerNameHash is the hash of the Issuer's distinguished name. The
 hash shall be calculated over the DER encoding of the issuer's name
 field in the certificate being checked. issuerKeyHash is the hash of
 the Issuer's public key. The hash shall be calculated over the value
 (excluding tag and length) of the subject public key field in the
 issuer's certificate. The hash algorithm used for both these hashes,
 is identified in hashAlgorithm. serialNumber is the serial number of
 the certificate for which status is being requested.
 */

#include "OCSPRequest.h"

#include <CertStore.h>

#include "ASN1OptionalField.h"
#include "ASN1/AlgorithmIdentifier.h"
#include "RSA/sha1.h"

COCSPRequest::COCSPRequest(UUCBufferedReader& reader) : CASN1Sequence(reader) {}

COCSPRequest::COCSPRequest(const CASN1Object& ocspRequest)
    : CASN1Sequence(ocspRequest) {}

COCSPRequest::COCSPRequest(CCertificate& certificate) {
  CASN1Sequence tbsRequest;

  CASN1Sequence requestList;

  CASN1Sequence request;

  CASN1Sequence certId;

  CASN1Integer serialNumber(certificate.getSerialNumber());

  CAlgorithmIdentifier hashAlgorithm(szSHA1OID);

  CName issuerName(certificate.getIssuer());

  UUCByteArray baIssuerName;
  issuerName.toByteArray(baIssuerName);

  // calcola l'hash SHA1
  SHA1Context sha;

  // hash issuername
  SHA1Reset(&sha);

  SHA1Input(&sha, baIssuerName.getContent(), baIssuerName.getLength());

  SHA1Result(&sha);

  char szAux[100];
  sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0],
          sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3],
          sha.Message_Digest[4]);

  UUCByteArray baIssuerNameHash(szAux);
  UUCByteArray baIssuerKeyHash;

  // hash public key
  UUCByteArray baPubKey;

  {
    CASN1Sequence authorityKeyIdentifier(
        certificate.getAuthorithyKeyIdentifier());
    CASN1OctetString keyIdentifier(authorityKeyIdentifier.elementAt(0));
    keyIdentifier.setTag(0x04);  // set the correct tag

    baIssuerKeyHash.append(*keyIdentifier.getValue());
  }

  certId.addElement(hashAlgorithm);
  certId.addElement(CASN1OctetString(baIssuerNameHash));
  certId.addElement(CASN1OctetString(baIssuerKeyHash));
  certId.addElement(serialNumber);

  request.addElement(certId);

  requestList.addElement(request);

  tbsRequest.addElement(requestList);

  addElement(tbsRequest);
}
