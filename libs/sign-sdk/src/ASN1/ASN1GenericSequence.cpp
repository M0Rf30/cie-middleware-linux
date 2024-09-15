#include "ASN1GenericSequence.h"

#include <stdlib.h>

#include "ASN1Exception.h"

CASN1GenericSequence::CASN1GenericSequence(BYTE btTag)
    : m_pnOffsets(NULL), m_nOffsetsMax(MAXSIZE), m_nSize(0) {
  m_pnOffsets =
      (unsigned int*)calloc(m_nOffsetsMax + 2, sizeof(m_pnOffsets[0]));
  setTag(btTag);
}

CASN1GenericSequence::CASN1GenericSequence(UUCBufferedReader& reader)
    : CASN1Object(reader),
      m_pnOffsets(NULL),
      m_nOffsetsMax(MAXSIZE),
      m_nSize(0) {
  m_pnOffsets =
      (unsigned int*)calloc(m_nOffsetsMax + 2, sizeof(m_pnOffsets[0]));
  m_nSize = makeOffset();
}

CASN1GenericSequence::CASN1GenericSequence(const UUCByteArray& content)
    : CASN1Object(content),
      m_pnOffsets(NULL),
      m_nOffsetsMax(MAXSIZE),
      m_nSize(0) {
  m_pnOffsets =
      (unsigned int*)calloc(m_nOffsetsMax + 2, sizeof(m_pnOffsets[0]));
  m_nSize = makeOffset();
}

CASN1GenericSequence::CASN1GenericSequence(const CASN1Object& obj)
    : CASN1Object(obj), m_pnOffsets(NULL), m_nOffsetsMax(MAXSIZE), m_nSize(0) {
  m_pnOffsets =
      (unsigned int*)calloc(m_nOffsetsMax + 2, sizeof(m_pnOffsets[0]));
  m_nSize = makeOffset();
}

CASN1GenericSequence::CASN1GenericSequence(const CASN1GenericSequence& obj)
    : CASN1Object(obj), m_pnOffsets(NULL), m_nOffsetsMax(MAXSIZE), m_nSize(0) {
  m_pnOffsets =
      (unsigned int*)calloc(m_nOffsetsMax + 2, sizeof(m_pnOffsets[0]));
  m_nSize = makeOffset();
}

CASN1GenericSequence::CASN1GenericSequence(const BYTE* value, long len)
    : CASN1Object(value, len),
      m_pnOffsets(NULL),
      m_nOffsetsMax(MAXSIZE),
      m_nSize(0) {
  m_pnOffsets =
      (unsigned int*)calloc(m_nOffsetsMax + 2, sizeof(m_pnOffsets[0]));
  m_nSize = makeOffset();
}

CASN1GenericSequence::~CASN1GenericSequence() {
  if (m_pnOffsets) free(m_pnOffsets);
  // NSLog(@"~CASN1GenericSequence()");
}

CASN1GenericSequence& CASN1GenericSequence::operator=(
    const CASN1GenericSequence& obj) {
  setValue(*obj.getValue());
  setTag(obj.getTag());
  m_nSize = makeOffset();
  return *this;
}

void CASN1GenericSequence::fromByteArray(const UUCByteArray& content) {
  UUCBufferedReader reader(content);

  fromReader(reader);
  m_nSize = makeOffset();
}

void CASN1GenericSequence::addElement(const CASN1Object& obj) {
  UUCByteArray serObj;
  obj.toByteArray(serObj);

  const UUCByteArray* pOldVal = getValue();

  if (pOldVal->getLength() == 0) {
    setValue(serObj);
  } else {
    UUCByteArray newVal;
    // copy old val
    newVal.append(*pOldVal);
    // copy val to add
    newVal.append(serObj);
    // set new val
    setValue(newVal);
  }

  m_nSize = makeOffset();
}

void CASN1GenericSequence::addElementAt(const CASN1Object& obj, int nPos) {
  if (nPos < 0 || (unsigned int)nPos > size())
    throw -1;  // new IllegalArgumentException("Invalid position:" + nPos);

  UUCByteArray serObj;
  obj.toByteArray(serObj);

  const UUCByteArray* pOldVal = getValue();

  UUCByteArray newVal;

  if (pOldVal->getLength() == 0) {
    newVal.append(serObj);
  } else if (nPos == 0) {
    // copy val to add
    newVal.append(serObj);
    // copy old val
    newVal.append(*pOldVal);
  } else {
    int offset = m_pnOffsets[nPos];

    // copy old val fino all'offset
    newVal.append(pOldVal->getContent(), offset);

    // copy val to add
    newVal.append(serObj);

    // copy the rest of the old val
    newVal.append(pOldVal->getContent() + offset,
                  pOldVal->getLength() - offset);
  }

  // set new val
  setValue(newVal);

  m_nSize = makeOffset();
}

CASN1Object CASN1GenericSequence::elementAt(int nPos) {
  if (this->size() > static_cast<unsigned int>(nPos)) {
    int offset = m_pnOffsets[nPos];
    UUCByteArray curObj(getValue()->getContent() + offset,
                        getLength() - offset + 1);
    CASN1Object curAsn1Obj(curObj);

    m_nextOffset = offset + curAsn1Obj.getSerializedLength();

    return curAsn1Obj;
  }
  return CASN1Object();
}

CASN1Object CASN1GenericSequence::nextElement() {
  UUCByteArray curObj(getValue()->getContent() + m_nextOffset,
                      getLength() - m_nextOffset + 1);

  CASN1Object curAsn1Obj(curObj);

  m_nextOffset += curAsn1Obj.getSerializedLength();

  return curAsn1Obj;
}

CASN1Object CASN1GenericSequence::elementAtOpt(int nPos) {
  if (this->size() > (unsigned int)nPos) {
    int offset = m_pnOffsets[nPos];
    CASN1Object curAsn1Obj(getValue()->getContent() + offset,
                           getLength() - offset + 1);

    m_nextOffset = offset + curAsn1Obj.getSerializedLength();

    return curAsn1Obj;
  }
  return CASN1Object();
}

CASN1Object CASN1GenericSequence::nextElementOpt() {
  CASN1Object curAsn1Obj(getValue()->getContent() + m_nextOffset,
                         getLength() - m_nextOffset + 1);

  m_nextOffset += curAsn1Obj.getSerializedLength();

  return curAsn1Obj;
}

void CASN1GenericSequence::setElementAt(const CASN1Object& obj, int nPos) {
  removeElementAt(nPos);
  addElementAt(obj, nPos);
}

void CASN1GenericSequence::removeElementAt(int nPos) {
  if (nPos < 0 || (unsigned int)nPos > size()) throw -1;

  UUCByteArray oldVal(*(getValue()));

  UUCByteArray newVal;

  if (oldVal.getLength() == 0) {
    // do nothing
  } else if (nPos == 0) {
    // elimina la prima
    int offset = m_pnOffsets[1];

    // copy the rest of the old val
    newVal.append(oldVal.getContent() + offset, oldVal.getLength() - offset);
  } else {
    int offset = m_pnOffsets[nPos];
    int offset1 = m_pnOffsets[nPos + 1];

    // copy old val fino all'offset
    newVal.append(oldVal.getContent(), offset);

    // copy the rest of the old val
    newVal.append(oldVal.getContent() + offset1, oldVal.getLength() - offset1);
  }

  // set new val
  setValue(newVal);

  m_nSize = makeOffset();
}

void CASN1GenericSequence::removeAll() {
  UUCByteArray newVal;

  // set new val
  setValue(newVal);

  m_nSize = 0;
}

bool CASN1GenericSequence::isPresent(int nPos) const {
  if (nPos < 0) throw -1;

  return (unsigned int)nPos < size();
}

unsigned int CASN1GenericSequence::size() const { return m_nSize; }

int CASN1GenericSequence::makeOffset() {
  const UUCByteArray* pContent = getValue();
  unsigned long len = pContent->getLength();

  unsigned int offset = 0;
  UUCByteArray objVal;
  unsigned int i = 0;
  while (offset < len) {
    if (i == m_nOffsetsMax) {
      m_nOffsetsMax += 1000;
      m_pnOffsets = (unsigned int*)realloc(
          m_pnOffsets, sizeof(m_pnOffsets[0]) * m_nOffsetsMax + 2);
    }
    m_pnOffsets[i] = offset;

    // oggetto corrente
    try {
      CASN1Object currentObj(pContent->getContent() + offset,
                             pContent->getLength() - offset + 1);
      int iLen = currentObj.getOrigLenLen() + currentObj.getLength() + 2;
      offset += iLen;
      i++;
    } catch (const CASN1ParsingException& e) {
      break;
    }
  }

  return i;
}
