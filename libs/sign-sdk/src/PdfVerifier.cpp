/*
 *  PdfVerifier.cpp
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstddef>
#ifndef HP_UX

#include <string>

#include "ASN1/SignedData.h"
#include "ASN1/SignerInfo.h"
#include "PdfVerifier.h"
#include "SignedDocument.h"

#ifdef GetObject
#undef GetObject
#endif

extern logFunc pfnCrashliticsLog;

PDFVerifier::PDFVerifier() : m_pPdfMemDocument(NULL), m_actualLen(0) {}

PDFVerifier::~PDFVerifier() {
  if (m_pPdfMemDocument) delete m_pPdfMemDocument;
}

int PDFVerifier::Load(const char *pdf, int len) {
  if (m_pPdfMemDocument) delete m_pPdfMemDocument;

  try {
    m_pPdfMemDocument = new PdfMemDocument();
    m_pPdfMemDocument->LoadFromBuffer(pdf, len, true);
    m_actualLen = len;
    m_szDocBuffer = (char *)pdf;

    return 0;
  } catch (::PoDoFo::PdfError &err) {
    return -2;
  } catch (...) {
    return -1;
  }
}

int PDFVerifier::Load(const char *szFilePath) {
  if (m_pPdfMemDocument) delete m_pPdfMemDocument;

  try {
    m_pPdfMemDocument = new PdfMemDocument();
    m_pPdfMemDocument->Load(szFilePath);

    BYTE buffer[BUFFERSIZE];
    int nRead = 0;

    FILE *f = fopen(szFilePath, "rb");
    if (!f) {
      return DISIGON_ERROR_FILE_NOT_FOUND;
    }

    m_data.removeAll();

    while (((nRead = fread(buffer, 1, BUFFERSIZE, f)) > 0)) {
      m_data.append(buffer, nRead);
    }

    fclose(f);
    m_actualLen = m_data.getLength();
    m_szDocBuffer = (char *)m_data.getContent();

    return 0;
  } catch (::PoDoFo::PdfError &err) {
    return -2;
  } catch (...) {
    return -1;
  }
}

int PDFVerifier::GetNumberOfSignatures(const char *szFilePath) {
  pfnCrashliticsLog("PDFVerifier::GetNumberOfSignatures");
  pfnCrashliticsLog(szFilePath);

  PdfMemDocument doc;

  try {
    doc.Load(szFilePath);

    pfnCrashliticsLog("file loaded");

    return GetNumberOfSignatures(&doc);
  } catch (::PoDoFo::PdfError &err) {
    return -2;
  } catch (...) {
    return -1;
  }
}

int PDFVerifier::GetNumberOfSignatures(PdfMemDocument *pPdfDocument) {
  printf("GetNumberOfSignatures");

  /// Find the document catalog dictionary
  const PdfObject *const trailer = pPdfDocument->GetTrailer();
  if (!trailer->IsDictionary()) return -1;

  printf("trailer ok");

  const PdfObject *const catalogRef =
      trailer->GetDictionary().GetKey(PdfName("Root"));
  if (catalogRef == 0 || !catalogRef->IsReference()) return -2;

  printf("Catalogref ok");

  const PdfObject *const catalog =
      pPdfDocument->GetObjects().GetObject(catalogRef->GetReference());
  if (catalog == 0 || !catalog->IsDictionary()) return -3;

  printf("catalog ok");

  /// Find the Fields array in catalog dictionary
  const PdfObject *acroFormValue =
      catalog->GetDictionary().GetKey(PdfName("AcroForm"));
  if (acroFormValue == 0) return 0;

  printf("acroform ok 1");

  if (acroFormValue->IsReference())
    acroFormValue =
        pPdfDocument->GetObjects().GetObject(acroFormValue->GetReference());

  printf("acroform ok 2");

  if (!acroFormValue->IsDictionary()) return 0;

  printf("acroform ok 3");

  const PdfObject *fieldsValue =
      acroFormValue->GetDictionary().GetKey(PdfName("Fields"));
  if (fieldsValue == 0) return 0;

  printf("fieldsValue ok");

  try {
    if (fieldsValue->IsReference()) {
      fieldsValue =
          pPdfDocument->GetObjects().GetObject(fieldsValue->GetReference());
    }
  } catch (...) {
    printf("First chance Exception\n");
  }

  printf("fieldsValue ok 2");

  if (!fieldsValue->IsArray()) return 0;

  printf("fieldsValue is array");

  /// Verify if each object of the array is a signature field
  int n = 0;
  const PdfArray &array = fieldsValue->GetArray();
  for (unsigned int i = 0; i < array.size(); i++) {
    const PdfObject *const obj =
        pPdfDocument->GetObjects().GetObject(array[i].GetReference());
    if (IsSignatureField(pPdfDocument, obj)) {
      n++;
    }
  }

  return n;
}

int PDFVerifier::GetNumberOfSignatures() {
  if (!m_pPdfMemDocument) return -1;

  return GetNumberOfSignatures(m_pPdfMemDocument);
}

int PDFVerifier::VerifySignature(size_t index, const char *szDate,
                                 char *signatureType,
                                 REVOCATION_INFO *pRevocationInfo) {
  if (!m_pPdfMemDocument) return -1;

  /// Find the document catalog dictionary
  const PdfObject *const trailer = m_pPdfMemDocument->GetTrailer();
  if (!trailer->IsDictionary()) return -1;

  const PdfObject *const catalogRef =
      trailer->GetDictionary().GetKey(PdfName("Root"));
  if (catalogRef == 0 || !catalogRef->IsReference()) return -2;

  const PdfObject *const catalog =
      m_pPdfMemDocument->GetObjects().GetObject(catalogRef->GetReference());
  if (catalog == 0 || !catalog->IsDictionary()) return -3;

  /// Find the Fields array in catalog dictionary
  const PdfObject *acroFormValue =
      catalog->GetDictionary().GetKey(PdfName("AcroForm"));
  if (acroFormValue == 0) return 0;

  if (acroFormValue->IsReference())
    acroFormValue = m_pPdfMemDocument->GetObjects().GetObject(
        acroFormValue->GetReference());

  if (!acroFormValue->IsDictionary()) return 0;

  const PdfObject *fieldsValue =
      acroFormValue->GetDictionary().GetKey(PdfName("Fields"));
  if (fieldsValue == 0) return 0;

  if (fieldsValue->IsReference())
    fieldsValue =
        m_pPdfMemDocument->GetObjects().GetObject(fieldsValue->GetReference());

  if (!fieldsValue->IsArray()) return 0;

  vector<const PdfObject *> signatureVector;

  /// Verify if each object of the array is a signature field
  const PdfArray &array = fieldsValue->GetArray();
  for (size_t i = 0; i < array.size(); i++) {
    const PdfObject *pObj =
        m_pPdfMemDocument->GetObjects().GetObject(array[i].GetReference());
    if (IsSignatureField(m_pPdfMemDocument, pObj)) {
      signatureVector.push_back(pObj);
    }
  }

  if (index >= signatureVector.size()) return 0;

  const PdfObject *pSignatureObject = signatureVector[index];

  return VerifySignature(m_pPdfMemDocument, pSignatureObject, szDate,
                         signatureType, pRevocationInfo);
}

int PDFVerifier::VerifySignature(const PdfMemDocument *pDoc,
                                 const PdfObject *const pObj,
                                 const char *szDate, char *signatureType,
                                 REVOCATION_INFO *pRevocationInfo) {
  if (pObj == 0) return false;

  if (!pObj->IsDictionary()) return -1;

  const PdfObject *const keyFTValue =
      pObj->GetDictionary().GetKey(PdfName("FT"));
  if (keyFTValue == 0) return -2;

  string value;
  keyFTValue->ToString(value);
  if (value != "/Sig") return -3;

  const PdfObject *const keyVValue = pObj->GetDictionary().GetKey(PdfName("V"));
  if (keyVValue == 0) return -4;

  const PdfObject *const signature =
      pDoc->GetObjects().GetObject(keyVValue->GetReference());
  if (signature->IsDictionary()) {
    string byteRange;
    string signdData;
    string subfilter;

    const PdfObject *const keyByteRange =
        signature->GetDictionary().GetKey(PdfName("ByteRange"));
    keyByteRange->ToString(byteRange);

    const PdfObject *const keyContents =
        signature->GetDictionary().GetKey(PdfName("Contents"));
    keyContents->ToString(signdData);

    const PdfObject *const keySubFilter =
        signature->GetDictionary().GetKey(PdfName("SubFilter"));
    keySubFilter->ToString(subfilter);

    strtok((char *)byteRange.c_str(), " []");

    int len = atoi(strtok(NULL, " []"));
    int start1 = atoi(strtok(NULL, " []"));
    int len1 = atoi(strtok(NULL, " []"));

    const char *szSignedData = strtok((char *)signdData.c_str(), "<>");

    UUCByteArray baSignedData(szSignedData);
    CSignedDocument signedDocument(baSignedData.getContent(),
                                   baSignedData.getLength());

    CSignedData signedData(signedDocument.getSignedData());

    strcpy(signatureType, subfilter.c_str());

    if (subfilter == "/adbe.pkcs7.detached" ||
        subfilter == "/ETSI.CAdES.detached") {
      // extract the actual content
      UUCByteArray baContent;
      baContent.append((BYTE *)m_szDocBuffer, len);
      baContent.append(((BYTE *)m_szDocBuffer + start1), len1);
      CASN1SetOf signerInfos = signedData.getSignerInfos();
      CSignerInfo signerInfo(signerInfos.elementAt(0));
      CASN1SetOf certificates = signedData.getCertificates();

      CASN1OctetString actualContent(baContent);

      return CSignerInfo::verifySignature(
          actualContent, signerInfo, certificates, szDate, pRevocationInfo);
    } else if (subfilter == "/adbe.pkcs7.sha1") {
      return signedData.verify(0, szDate, pRevocationInfo);

    } else {
      return -5;
    }
  } else
    return -6;
}

bool PDFVerifier::IsSignatureField(const PdfMemDocument *pDoc,
                                   const PdfObject *const pObj) {
  if (pObj == 0) return false;
  if (!pObj->IsDictionary()) return false;

  const PdfObject *const keyFTValue =
      pObj->GetDictionary().GetKey(PdfName("FT"));
  if (keyFTValue == 0) return false;

  string value;
  keyFTValue->ToString(value);
  if (value != "/Sig") return false;

  const PdfObject *const keyVValue = pObj->GetDictionary().GetKey(PdfName("V"));
  if (keyVValue == 0) return false;

  const PdfObject *const signature =
      pDoc->GetObjects().GetObject(keyVValue->GetReference());
  if (signature->IsDictionary())
    return true;
  else
    return false;
}

int PDFVerifier::GetSignature(size_t index, UUCByteArray &signedDocument,
                              SignatureAppearanceInfo &signatureInfo) {
  if (!m_pPdfMemDocument) return -1;

  /// Find the document catalog dictionary
  const PdfObject *const trailer = m_pPdfMemDocument->GetTrailer();
  if (!trailer->IsDictionary()) return -1;

  const PdfObject *const catalogRef =
      trailer->GetDictionary().GetKey(PdfName("Root"));
  if (catalogRef == 0 || !catalogRef->IsReference())
    return -2;  // throw std::invalid_argument("Invalid /Root entry");

  const PdfObject *const catalog =
      m_pPdfMemDocument->GetObjects().GetObject(catalogRef->GetReference());
  if (catalog == 0 || !catalog->IsDictionary())
    return -3;  // throw std::invalid_argument("Invalid or non-dictionary
  // referenced by /Root entry");

  /// Find the Fields array in catalog dictionary
  const PdfObject *acroFormValue =
      catalog->GetDictionary().GetKey(PdfName("AcroForm"));
  if (acroFormValue == 0) return -4;

  if (acroFormValue->IsReference())
    acroFormValue = m_pPdfMemDocument->GetObjects().GetObject(
        acroFormValue->GetReference());

  if (!acroFormValue->IsDictionary()) return -5;

  const PdfObject *fieldsValue =
      acroFormValue->GetDictionary().GetKey(PdfName("Fields"));
  if (fieldsValue == 0) return -6;

  if (fieldsValue->IsReference())
    fieldsValue =
        m_pPdfMemDocument->GetObjects().GetObject(fieldsValue->GetReference());

  if (!fieldsValue->IsArray()) return -7;

  vector<const PdfObject *> signatureVector;

  /// Verify if each object of the array is a signature field
  const PdfArray &array = fieldsValue->GetArray();
  for (size_t i = 0; i < array.size(); i++) {
    const PdfObject *pObj =
        m_pPdfMemDocument->GetObjects().GetObject(array[i].GetReference());
    if (IsSignatureField(m_pPdfMemDocument, pObj)) {
      signatureVector.push_back(pObj);
    }
  }

  if (index >= signatureVector.size()) return -8;

  const PdfObject *pSignatureObject = signatureVector[index];

  return GetSignature(m_pPdfMemDocument, pSignatureObject, signedDocument,
                      signatureInfo);
}

int PDFVerifier::GetSignature(const PdfMemDocument *pDoc,
                              const PdfObject *const pObj,
                              UUCByteArray &signedDocument,
                              SignatureAppearanceInfo &appearanceInfo) {
  if (pObj == 0) return -1;

  if (!pObj->IsDictionary()) return -1;

  const PdfObject *const keyFTValue =
      pObj->GetDictionary().GetKey(PdfName("FT"));
  if (keyFTValue == 0) return -2;

  string value;
  keyFTValue->ToString(value);
  if (value != "/Sig") return -3;

  const PdfObject *const keyVValue = pObj->GetDictionary().GetKey(PdfName("V"));
  if (keyVValue == 0) return -4;

  const PdfObject *const keyRect =
      pObj->GetDictionary().GetKey(PdfName("Rect"));
  if (keyRect == 0) {
    return -4;
  }

  PdfArray rectArray = keyRect->GetArray();
  PdfRect rect;
  rect.FromArray(rectArray);

  appearanceInfo.left = rect.GetLeft();
  appearanceInfo.bottom = rect.GetBottom();
  appearanceInfo.width = rect.GetWidth();
  appearanceInfo.heigth = rect.GetHeight();

  const PdfObject *const signature =
      pDoc->GetObjects().GetObject(keyVValue->GetReference());
  if (signature->IsDictionary()) {
    string signdData;

    const PdfObject *const keyContents =
        signature->GetDictionary().GetKey(PdfName("Contents"));
    keyContents->ToString(signdData);

    const char *szSignedData = strtok((char *)signdData.c_str(), "<>");

    signedDocument.append(szSignedData);
    return 0;
  } else
    return -6;
}

#endif
