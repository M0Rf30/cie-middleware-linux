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

#include <podofo/podofo.h>
#if PODOFO_VERSION_MAJOR < 1
#if PODOFO_VERSION_MINOR < 10
#include <podofo/doc/PdfSignOutputDevice.h>
#include <podofo/doc/PdfSignatureField.h>
#endif
#else
#error PoDoFo version not supported (yet)
#endif
#include "Util/UUCByteArray.h"

#define SIGNATURE_SIZE 10000

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

#if PODOFO_VERSION_MINOR < 10
  void GetBufferForSignature(UUCByteArray& toSign);

  void SetSignature(const char* signature, int len);
#endif

  void GetSignedPdf(UUCByteArray& signature);

  const double getWidth(int pageIndex);

  const double getHeight(int pageIndex);

#if PODOFO_VERSION_MINOR < 10
 private:
#endif

  PdfMemDocument *m_pPdfDocument;

#if PODOFO_VERSION_MINOR < 10
 private:
  PdfSignatureField *m_pSignatureField;

  PdfSignOutputDevice *m_pSignOutputDevice;

  PdfOutputDevice *m_pFinalOutDevice;

  char *m_pSignDocbuffer;

  int m_actualLen;

#else
  PdfSignature *m_pSignatureField;

  BufferStreamDevice *m_pSignOutputDevice;

 private:
  charbuff m_pOutputBuffer;
#endif
};

#endif  // _PDFSIGNATUREGENERATOR_H_
