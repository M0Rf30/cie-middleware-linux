#include "ASN1/ASN1Integer.h"

const BYTE CASN1Integer::TAG = 0x02;

CASN1Integer::CASN1Integer(const CASN1Object& obj) : CASN1Object(obj) {
  setTag(TAG);
}

CASN1Integer::~CASN1Integer() {}

CASN1Integer::CASN1Integer(unsigned long nVal) {
  UUCByteArray val;

  // first digit
  BYTE btDigit = (BYTE)(nVal & 0x000000FF);
  if (btDigit == 0x80) {
    val.append((BYTE)0x00);
    val.append((BYTE)0x80);
  } else {
    val.append(btDigit);
  }

  nVal = nVal / 256;

  int i = 1;
  while (nVal > 0) {
    btDigit = (BYTE)(nVal >> (256 * i));
    val.append(btDigit);
    nVal = nVal / 256;
  }

  val.reverse();
  setValue(val);
  setTag(TAG);
}

CASN1Integer::CASN1Integer(const BYTE* pbtVal, unsigned int nLen) {
  setValue(UUCByteArray(pbtVal, nLen));
  setTag(TAG);
}

int CASN1Integer::getIntValue() const { return getLongValue(); }

unsigned long CASN1Integer::getLongValue() const {
  long nValue = 0;
  const BYTE* btBuffer = getValue()->getContent();
  unsigned long nLen = getLength();

  if (nLen == 1) {
    nValue = btBuffer[0];
  } else {
    for (unsigned int i = 0; i < nLen; i++) {
      BYTE btVal = (BYTE)(btBuffer[nLen - i - 1]);

      nValue += btVal << (8 * i);
    }
  }
  return nValue;
}
