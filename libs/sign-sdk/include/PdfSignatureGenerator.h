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

#include <podofo/auxiliary/StreamDevice.h>
#include <podofo/podofo.h>

#include "Sign/definitions.h"
#include "Util/UUCByteArray.h"
#include "auxiliary/OutputDevice.h"
#include "auxiliary/basetypes.h"

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

  void SetSignature(const string_view& signature, int len);

  void GetSignedPdf(UUCByteArray& signature);

  void AddFont(const char* szFontName, const char* szFontPath);

  const double getWidth(int pageIndex);

  const double getHeight(int pageIndex);

 private:
  PdfMemDocument* m_pPdfDocument;

  PdfSignature* m_pSignatureField;

  StreamDevice* m_pSignOutputDevice;

  StreamDevice* m_pFinalOutDevice;

  charbuff m_pMainDocbuffer;

  charbuff m_pSignDocbuffer;

  const double lastSignatureY(int left, int bottom);

  int m_actualLen;

  static bool IsSignatureField(const PdfMemDocument* pDoc,
                               const PdfObject* const pObj);
};

#endif  // _PDFSIGNATUREGENERATOR_H_
