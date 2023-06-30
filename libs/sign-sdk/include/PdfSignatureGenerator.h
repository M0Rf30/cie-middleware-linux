/*
 *  PdfSignatureGenerator.h
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PDFSIGNATUREGENERATOR_H_
#define _PDFSIGNATUREGENERATOR_H_

#include <string_view>
#include <podofo/auxiliary/OutputDevice.h>
#include <podofo/podofo.h>

#include "../../shared/src/Util/UUCByteArray.h"

using namespace PoDoFo;
using namespace std;

class PdfSignatureGenerator {
 public:
  PdfSignatureGenerator();

  virtual ~PdfSignatureGenerator();

  int Load(const char* pdf, int len);

  void InitSignature(int pageIndex, const char* szReason,
                     const char* szReasonLabel, const char* szName,
                     const char* szNameLabel, const char* szLocation,
                     const char* szLocationLabel, const char* szFieldName,
                     const char* szSubFilter);

  void InitSignature(int pageIndex, float left, float bottom, float width,
                     float height, const char* szReason,
                     const char* szReasonLabel, const char* szName,
                     const char* szNameLabel, const char* szLocation,
                     const char* szLocationLabel, const char* szFieldName,
                     const char* szSubFilter);

  void InitSignature(int pageIndex, float left, float bottom, float width,
                     float height, const char* szReason,
                     const char* szReasonLabel, const char* szName,
                     const char* szNameLabel, const char* szLocation,
                     const char* szLocationLabel, const char* szFieldName,
                     const char* szSubFilter, const char* szImagePath,
                     const char* szDescription, const char* szGraphometricData,
                     const char* szVersion);

  void GetBufferForSignature(UUCByteArray& toSign);

  void SetSignature(const char* signature, size_t len);

  void GetSignedPdf(UUCByteArray& signature);

  void AddFont(const std::string_view& szFontName, const char* szFontPath);

  const double getWidth(int pageIndex);

  const double getHeight(int pageIndex);

 private:
  PdfDocument* m_pPdfDocument;

  PdfMemDocument* m_pPdfMemDocument;

  PdfWriter* m_pPdfWriter;

  PdfField* m_pSignatureField;

  OutputStreamDevice* m_pSignOutputDevice;

  OutputStreamDevice* m_pFinalOutDevice;

  const char* m_pMainDocbuffer;

  const char* m_pSignDocbuffer;

  const double lastSignatureY(int left, int bottom);

  int m_actualLen;

  static bool IsSignatureField(const PdfDocument* pDoc,
                               const PdfObject* const pObj);
};

#endif  // _PDFSIGNATUREGENERATOR_H_
