#ifndef _ASN1OBJECTIDENTIFIER_H
#define _ASN1OBJECTIDENTIFIER_H

#include <string>

#include "ASN1Object.h"

using namespace std;

class CASN1ObjectIdentifier : public CASN1Object {
 private:
  const static BYTE TAG;

 public:
  CASN1ObjectIdentifier(UUCBufferedReader& reader);

  CASN1ObjectIdentifier(const CASN1Object&);

  CASN1ObjectIdentifier(const char* szObjId);

  // distruttore
  ~CASN1ObjectIdentifier();

  void ToOidString(UUCByteArray& objId);

  bool equals(const CASN1ObjectIdentifier& objid);
};

#endif  // _ASN1OBJECTIDENTIFIER_H
