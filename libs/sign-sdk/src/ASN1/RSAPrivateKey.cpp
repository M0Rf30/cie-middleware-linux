#include "ASN1/RSAPrivateKey.h"

// RSAPrivateKey :: = SEQUENCE{
//	version           Version,
//	modulus           INTEGER, --n
//	publicExponent    INTEGER, --e
//	privateExponent   INTEGER, --d
//	prime1            INTEGER, --p
//	prime2            INTEGER, --q
//	exponent1         INTEGER, --d mod(p - 1)
//	exponent2         INTEGER, --d mod(q - 1)
//	coefficient       INTEGER, --(inverse of q) mod p
//	otherPrimeInfos   OtherPrimeInfos OPTIONAL
// }

// Macro per trasformazione da little-endian a big-endian e viceversa
#define REVERSE(in, out, len)       \
  {                                 \
    for (int i = 0; i < len; i++) { \
      out[i] = in[len - i - 1];     \
    }                               \
  }

CRSAPrivateKey::CRSAPrivateKey(UUCBufferedReader& reader)
    : CASN1Sequence(reader) {}

CRSAPrivateKey::CRSAPrivateKey(const CASN1Object& obj) : CASN1Sequence(obj) {}

CRSAPrivateKey::CRSAPrivateKey(const UUCByteArray& content)
    : CASN1Sequence(content) {}

CRSAPrivateKey::CRSAPrivateKey(const CASN1Integer& modulus,
                               const CASN1Integer& pubExp,
                               const CASN1Integer& priExp) {
  addElement(modulus);
  addElement(pubExp);
  addElement(priExp);
}

CRSAPrivateKey::~CRSAPrivateKey(void) {}

CASN1Integer CRSAPrivateKey::getModulus() { return CASN1Integer(elementAt(1)); }

CASN1Integer CRSAPrivateKey::getPublicExponent() {
  return CASN1Integer(elementAt(2));
}

CASN1Integer CRSAPrivateKey::getPrivateExponent() {
  return CASN1Integer(elementAt(3));
}
