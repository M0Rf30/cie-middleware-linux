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

#include <podofo/auxiliary/InputStream.h>
#include <podofo/auxiliary/OutputDevice.h>
#include <podofo/auxiliary/OutputStream.h>
#include <podofo/auxiliary/Rect.h>
#include <podofo/main/PdfFontManager.h>
#include <podofo/main/PdfSignature.h>
#include <podofo/podofo.h>

#include "Util/UUCByteArray.h"

using namespace PoDoFo;
using namespace std;

class PdfSignatureGenerator {
 public:
  PdfSignatureGenerator();

  virtual ~PdfSignatureGenerator();

  int Load(const char* pdf, int len);

  void AdjustByteRange(OutputStreamDevice& device, size_t byteRangeOffset,
                       size_t conentsBeaconOffset, size_t conentsBeaconSize,
                       charbuff& buffer);

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

  size_t ReadForSignature(StreamDevice& device, size_t conentsBeaconOffset,
                          size_t conentsBeaconSize, char* buffer, size_t size);

  void SetGraphometricData(PdfSignature* pSignatureField,
                           nullable<const PdfString&> vendor,
                           nullable<const PdfString&> szGraphometricData,
                           nullable<const PdfString&> szVersion);

  void SetSignature(const char* signature, int len);

  void GetSignedPdf(UUCByteArray& signature);

  void AddFont(const char* szFontName, const char* szFontPath);

  const double getWidth(int pageIndex);

  const double getHeight(int pageIndex);

 private:
  PdfSignatureBeacons* m_beacons;

  PdfDocument* m_pPdfDocument;

  PdfMemDocument* m_pPdfMemDocument;

  PdfWriter* m_pPdfWriter;

  PdfSignature* m_pSignatureField;

  StreamDevice* m_pSignOutputDevice;

  OutputStreamDevice* m_pFinalOutDevice;

  char* m_pMainDocbuffer;

  char* m_pSignDocbuffer;

  const double lastSignatureY(int left, int bottom);

  int m_actualLen;

  static bool IsSignatureField(const PdfMemDocument* pDoc,
                               const PdfObject* const pObj);
};

#endif  // _PDFSIGNATUREGENERATOR_H_
