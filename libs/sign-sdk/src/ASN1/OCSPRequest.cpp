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
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "ASN1/AlgorithmIdentifier.h"

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

  // Assuming baIssuerName is a buffer with the issuer name data
  unsigned char hash[SHA_DIGEST_LENGTH];
  EVP_MD_CTX* sha1_ctx = EVP_MD_CTX_new();
  EVP_DigestInit(sha1_ctx, EVP_sha1());
  EVP_DigestUpdate(sha1_ctx, baIssuerName.getContent(),
                   baIssuerName.getLength());
  EVP_DigestFinal(sha1_ctx, hash, NULL);
  EVP_MD_CTX_free(sha1_ctx);

  // Reinterpret the hash as five unsigned 32-bit words.
  unsigned* word = reinterpret_cast<unsigned*>(hash);
  char szAux[100];

  sprintf(szAux, "%08X%08X%08X%08X%08X ", __builtin_bswap32(word[0]),
          __builtin_bswap32(word[1]), __builtin_bswap32(word[2]),
          __builtin_bswap32(word[3]), __builtin_bswap32(word[4]));

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
