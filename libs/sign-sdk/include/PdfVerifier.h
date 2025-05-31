/*
 *  PdfVerifier.h
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PDFVERIFIER_H_
#define _PDFVERIFIER_H_

#include <podofo/podofo.h>
#if PODOFO_VERSION_MAJOR < 2
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
#include <podofo/doc/PdfSignOutputDevice.h>
#include <podofo/doc/PdfSignatureField.h>
#endif
#else
#error PoDoFo version not supported (yet)
#endif
#include "Sign/disigonsdk.h"
#include "Util/UUCByteArray.h"

using namespace PoDoFo;
using namespace std;

typedef struct _SignatureAppearanceInfo {
  int left;
  int bottom;
  int width;
  int heigth;
} SignatureAppearanceInfo;

class PDFVerifier {
 public:
  PDFVerifier();

  virtual ~PDFVerifier();

  int Load(const char* pdf, int len);
  int Load(const char* szFilePath);

  int GetNumberOfSignatures();

  int VerifySignature(size_t index, const char* szDate, char* signatureType,
                      REVOCATION_INFO* pRevocationInfo);

  int GetSignature(size_t index, UUCByteArray& signedDocument,
                   SignatureAppearanceInfo& appearanceInfo);

  static int GetNumberOfSignatures(PdfMemDocument* pPdfDocument);
  static int GetNumberOfSignatures(const char* szFilePath);

 private:
  UUCByteArray m_data;
  static bool IsSignatureField(const PdfMemDocument* pDoc,
                               const PdfObject* const pObj);

  int VerifySignature(const PdfMemDocument* pDoc, const PdfObject* const pObj,
                      const char* szDate, char* signatureType,
                      REVOCATION_INFO* pRevocationInfo);

  int GetSignature(const PdfMemDocument* pDoc, const PdfObject* const pObj,
                   UUCByteArray& signedDocument,
                   SignatureAppearanceInfo& appearanceInfo);

  PdfMemDocument* m_pPdfMemDocument;

  int m_actualLen;

  char* m_szDocBuffer;
};

#endif  //_PDFVERIFIER_H
