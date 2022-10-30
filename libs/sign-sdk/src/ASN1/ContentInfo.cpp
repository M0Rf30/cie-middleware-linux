#include "ContentInfo.h"

#include "ASN1OptionalField.h"

CContentInfo::~CContentInfo() {}

CContentInfo::CContentInfo(const CASN1Object& contentInfo)
    : CASN1Sequence(contentInfo) {}

CContentInfo::CContentInfo(UUCBufferedReader& reader) : CASN1Sequence(reader) {}

CContentInfo::CContentInfo(const CContentType& contentType) {
  addElement(contentType);
}

CContentInfo::CContentInfo(const CContentType& contentType,
                           const CASN1Object& content) {
  addElement(contentType);

  CASN1Sequence innerContent;
  innerContent.addElement(content);
  addElement(CASN1OptionalField(innerContent, 0));
}

void CContentInfo::setContent(const CASN1Object& content) {
  CASN1Sequence innerContent;
  innerContent.addElement(content);
  if (size() < 2)
    addElement(CASN1OptionalField(innerContent, 0));
  else {
    setElementAt(CASN1OptionalField(innerContent, 0), 1);
  }
}

CContentType CContentInfo::getContentType() {
  return CContentType(elementAt(0));
}

CASN1Object CContentInfo::getContent() {
  // il getValue è necessario perchè il content è dichiarato explicit [0]
  return CASN1Object(*elementAt(1).getValue());
}
