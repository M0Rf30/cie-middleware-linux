#ifndef _ASN1SETOF_H
#define _ASN1SETOF_H

#include "ASN1GenericSequence.h"
#include "ASN1Object.h"
#define MAX_OBJ 10

class CASN1SetOf : public CASN1GenericSequence {
 public:
  ~CASN1SetOf();

  CASN1SetOf();

  CASN1SetOf(const CASN1Object&);

 private:
  static const BYTE TAG;
};

#endif  // _ASN1SETOF_H
