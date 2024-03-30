#pragma once
#include "ASN1/ASN1Integer.h"
#include "ASN1/ASN1Sequence.h"
#include "ASN1Object.h"

class CRSAPublicKey : public CASN1Sequence {
 public:
  // Costruttori
  CRSAPublicKey(UUCBufferedReader& reader);

  CRSAPublicKey(const CASN1Object& obj);

  CRSAPublicKey(const CASN1Integer& modulus, const CASN1Integer& exponent);

  virtual ~CRSAPublicKey();

  CASN1Integer getModulus();

  CASN1Integer getExponent();

 private:
};
