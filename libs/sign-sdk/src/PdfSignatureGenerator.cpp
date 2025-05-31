/*
 *  PdfSignatureGenerator.cpp
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#include "PdfSignatureGenerator.h"

#include <fstream>
#include <iostream>

#include "PdfVerifier.h"
#include "UUCLogger.h"

#define MAX_TMP 1000
#define FONT_NAME "DejaVu Sans"
#define FONT_SIZE 5.0
#define TXT_PAD 5.5

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef GetObject
#undef GetObject
#endif

int GetNumberOfSignatures(PdfMemDocument* pPdfDocument);

USE_LOG;

PdfSignatureGenerator::PdfSignatureGenerator()
    : m_pSignatureField(NULL),
      m_pSignOutputDevice(NULL),
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
      m_pFinalOutDevice(NULL),
      m_pSignDocbuffer(NULL),
#endif
      m_pPdfDocument(NULL) {
}

PdfSignatureGenerator::~PdfSignatureGenerator() {
  if (m_pPdfDocument) delete m_pPdfDocument;

#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
  if (m_pSignatureField) delete m_pSignatureField;
#endif

  if (m_pSignOutputDevice) delete m_pSignOutputDevice;

#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
  if (m_pFinalOutDevice) delete m_pFinalOutDevice;

  if (m_pSignDocbuffer) delete[] m_pSignDocbuffer;
#endif
}

int PdfSignatureGenerator::Load(const char* pdf, int len) {
  if (m_pPdfDocument) delete m_pPdfDocument;

  try {
    m_pPdfDocument = new PdfMemDocument();
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
    m_pPdfDocument->LoadFromBuffer(pdf, len, true);

    m_actualLen = len;

#else
    // Copy pdf buffer for later use
    auto input = std::make_shared<SpanStreamDevice>(bufferview(pdf, len));
    m_pSignOutputDevice = new BufferStreamDevice(m_pOutputBuffer);
    input->CopyTo(*m_pSignOutputDevice);

    m_pPdfDocument->LoadFromBuffer(bufferview(pdf, len));
#endif
    return PDFVerifier::GetNumberOfSignatures(m_pPdfDocument);
  } catch (::PoDoFo::PdfError& err) {
    return -2;
  } catch (...) {
    return -1;
  }
}

void PdfSignatureGenerator::InitSignature(
    int pageIndex, const char* szReason, const char* szReasonLabel,
    const char* szName, const char* szNameLabel, const char* szLocation,
    const char* szLocationLabel, const char* szFieldName,
    const char* szSubFilter) {
  LOG_DBG((0, "quella con tutti 0\n", ""));
  InitSignature(pageIndex, 0, 0, 0, 0, szReason, szReasonLabel, szName,
                szNameLabel, szLocation, szLocationLabel, szFieldName,
                szSubFilter);
}

void PdfSignatureGenerator::InitSignature(
    int pageIndex, float left, float bottom, float width, float height,
    const char* szReason, const char* szReasonLabel, const char* szName,
    const char* szNameLabel, const char* szLocation,
    const char* szLocationLabel, const char* szFieldName,
    const char* szSubFilter) {
  LOG_DBG((0, "quella senza tutti 0\n", ""));
  InitSignature(pageIndex, left, bottom, width, height, szReason, szReasonLabel,
                szName, szNameLabel, szLocation, szLocationLabel, szFieldName,
                szSubFilter, NULL, NULL, NULL, NULL);
}

void PdfSignatureGenerator::InitSignature(
    int pageIndex, float left, float bottom, float width, float height,
    const char* szReason, const char* szReasonLabel, const char* szName,
    const char* szNameLabel, const char* szLocation,
    const char* szLocationLabel, const char* szFieldName,
    const char* szSubFilter, const char* szImagePath, const char* szDescription,
    const char* szGraphometricData, const char* szVersion) {
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
  int fulllen = m_actualLen * 2 + SIGNATURE_SIZE * 2;

  if (m_pSignatureField) delete m_pSignatureField;

  PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
  PdfRect cropBox = pPage->GetCropBox();

  float cropBoxWidth = cropBox.GetWidth();
  float cropBoxHeight = cropBox.GetHeight();
#else
  PdfPage* pPage = &m_pPdfDocument->GetPages().GetPageAt(pageIndex);
  Rect cropBox = pPage->GetCropBox();

  float cropBoxWidth = cropBox.Width;
  float cropBoxHeight = cropBox.Height;
#endif

  float left0 = left * cropBoxWidth;
  float bottom0 = cropBoxHeight - (bottom * cropBoxHeight);

  float width0 = width * cropBoxWidth;
  float height0 = height * cropBoxHeight;

#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
  PdfRect rect(left0, bottom0, width0, height0);

  PdfAcroForm* acroForm = m_pPdfDocument->GetAcroForm();

  // Add /SigFlags to acroform
  pdf_int64 flags = 3;
  acroForm->GetObject()->GetDictionary().AddKey(PdfName("SigFlags"),
                                                PdfObject(flags));

  // Create annotation
  PdfAnnotation* pAnnot = pPage->CreateAnnotation(ePdfAnnotation_Widget, rect);
  pAnnot->SetFlags(static_cast<EPdfAnnotationFlags>(0x84));

  m_pSignatureField = new PdfSignatureField(pAnnot, acroForm, m_pPdfDocument);
#else
  Rect rect(left0, bottom0, width0, height0);

  m_pSignatureField =
      &pPage->CreateField<PdfSignature>(PdfString(szFieldName), rect);
  m_pSignatureField->EnsureValueObject();

  m_pSignatureField->MustGetWidget().SetFlags(
      static_cast<PdfAnnotationFlags>(0x84));
#endif

  LOG_DBG((0, "InitSignature", "PdfSignatureField OK"));

  // This is the card holder's name
  // Shouldn't this go in /Name? Goes in /Reason
  if (szReason && szReason[0]) {
    PdfString name(szReason);
    m_pSignatureField->SetSignatureReason(name);
  }

  LOG_DBG((0, "InitSignature", "szReason OK"));

#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
  // /T: SignatureN
  if (szFieldName && szFieldName[0]) {
    // This corresponds to /T
    PdfString fieldName = PdfString(szFieldName);
    m_pSignatureField->SetFieldName(fieldName);
  }

  LOG_DBG((0, "InitSignature", "szFieldName OK"));
#endif

  if (szLocation && szLocation[0]) {
    PdfString location(szLocation);
    m_pSignatureField->SetSignatureLocation(location);
  }

  LOG_DBG((0, "InitSignature", "szLocation OK"));

  PdfDate now;
#if PODOFO_VERSION_MINOR >= 10 || PODOFO_VERSION_MAJOR >= 1
  now = PdfDate::LocalNow();
#endif
  m_pSignatureField->SetSignatureDate(now);

  LOG_DBG((0, "InitSignature", "Date OK"));

  // This is the signature date
  // Shouldn't this go in /M? Goes in /Name
  if (szName && szName[0]) {
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
    m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
        PdfName("Name"), PdfObject(PdfString(szName)));
#else
    m_pSignatureField->SetSignerName(PdfString(szName));
#endif
  }

  LOG_DBG((0, "InitSignature", "szName OK"));

  // Create graphical signature with stamp if we have a picture
  if (width * height > 0) {
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
    PdfXObject sigXObject(rect, m_pPdfDocument);
#else
    auto sigXObject = m_pPdfDocument->CreateXObjectForm(rect);
#endif
    PdfPainter painter;

    try {
      char* imgBuffer = NULL;
      double scale;
      streampos imgBufferSize = 0;
      ifstream img(szImagePath, ios::in | ios::binary | ios::ate);
      std::string signatureStamp;
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
      PdfImage image(m_pPdfDocument);
#else
      auto image = m_pPdfDocument->CreateImage();
#endif

      // Copy the image in a buffer
      if (img.is_open()) {
        imgBufferSize = img.tellg();

        // Increase space we have to allocate
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
        fulllen += imgBufferSize * 2;
#endif

        imgBuffer = new char[imgBufferSize];
        img.seekg(0, ios::beg);
        img.read(imgBuffer, imgBufferSize);
        img.close();
      }

      // Generate signature string
      // Append date
      if (szName && szName[0]) signatureStamp.append(szName);

      // Append name
      if (szReason && szReason[0]) {
        signatureStamp.append("\n");
        signatureStamp.append(szReason);
      }

#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
      image.LoadFromPngData((const unsigned char*)imgBuffer, imgBufferSize);
      // Scale using width to try to avoid squeezing image
      scale = (width0 / image.GetWidth());
#else
      image->LoadFromBuffer(bufferview(imgBuffer, imgBufferSize));
      scale = (width0 / image->GetWidth());
#endif

      // Draw signature
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
      painter.SetPage(&sigXObject);
#else
      painter.SetCanvas(*sigXObject);
#endif
      painter.Save();
      painter.Restore();
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
      painter.DrawImage(left0, bottom0, &image, scale, scale);
#else
      painter.DrawImage(*image, left0, bottom0, scale, scale);
#endif

      // Release buffer memory
      if (imgBufferSize != 0) delete[] imgBuffer;

      // Create signature stamp
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
      PdfFont* font = m_pPdfDocument->CreateFont(
          FONT_NAME, false, PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
          // We set no embedding but it doesn't work
          PdfFontCache::eFontCreationFlags_AutoSelectBase14, false);
      PdfRect sigRect =
          PdfRect(left0 + TXT_PAD, bottom0 - TXT_PAD, width0, height0);
      painter.SetFont(font);
      font->SetFontSize(FONT_SIZE);
      painter.DrawMultiLineText(sigRect, PdfString(signatureStamp));

      m_pSignatureField->SetAppearanceStream(&sigXObject);

      LOG_DBG((0, "InitSignature", "SetAppearanceStream OK"));

      // Remove the font we embedded
      acroForm->GetObject()->GetDictionary().RemoveKey(PdfName("DR"));
      acroForm->GetObject()->GetDictionary().RemoveKey(PdfName("DA"));
    } catch (PdfError& error) {
      LOG_ERR((0, "InitSignature", "SetAppearanceStream error: %s, %s",
               PdfError::ErrorMessage(error.GetError()), error.what()));
      if (painter.GetPage()) {
        try {
          painter.FinishPage();
        } catch (...) {
        }
      }
    }

    painter.FinishPage();
  }

  // Set SubFilter
  if (szSubFilter && szSubFilter[0]) {
    m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
        "SubFilter", PdfName(szSubFilter));
  }

  LOG_DBG((0, "InitSignature", "fulllen %d", fulllen));

  m_pSignDocbuffer = new char[fulllen];

  LOG_DBG((0, "InitSignature", "m_pSignDocbuffer %d", fulllen));

  m_pFinalOutDevice = new PdfOutputDevice(m_pSignDocbuffer, fulllen);
  m_pSignOutputDevice = new PdfSignOutputDevice(m_pFinalOutDevice);

  LOG_DBG((0, "InitSignature", "buffers OK %d", fulllen));

  m_pSignOutputDevice->SetSignatureSize(SIGNATURE_SIZE);

  LOG_DBG((0, "InitSignature", "SetSignatureSize OK %d", SIGNATURE_SIZE));

  m_pSignatureField->SetSignature(*m_pSignOutputDevice->GetSignatureBeacon());
  try {
    m_pPdfDocument->WriteUpdate(m_pSignOutputDevice);
  } catch (::PoDoFo::PdfError err) {
    printf("PdfError: %s\n", err.what());
  }
#else
      PdfFont* font = m_pPdfDocument->GetFonts().SearchFont(FONT_NAME);
      Rect sigRect = Rect(left0 + TXT_PAD, bottom0 - TXT_PAD, width0, height0);
      painter.TextState.SetFont(*font, FONT_SIZE);
      painter.DrawTextMultiLine(signatureStamp, sigRect);

      m_pSignatureField->MustGetWidget().SetAppearanceStream(*sigXObject);

      LOG_DBG((0, "InitSignature", "SetAppearanceStream OK"));

      // Remove the font we embedded
      m_pPdfDocument->GetAcroForm()->GetObject().GetDictionary().RemoveKey(
          PdfName("DR"));
      m_pPdfDocument->GetAcroForm()->GetObject().GetDictionary().RemoveKey(
          PdfName("DA"));
    } catch (...) {
    }

    painter.FinishDrawing();
  }
#endif
}

#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
void PdfSignatureGenerator::GetBufferForSignature(UUCByteArray& toSign) {
  int len = m_pSignOutputDevice->GetLength() * 2;
  char* buffer = new char[len];
  int nRead;

  m_pSignOutputDevice->AdjustByteRange();
  LOG_DBG((0, "SetSignature", "AdjustByteRange OK"));

  m_pSignOutputDevice->Seek(0);

  nRead = m_pSignOutputDevice->ReadForSignature(buffer, len);
  if (nRead == -1) throw nRead;

  toSign.append((BYTE*)buffer, nRead);

  delete[] buffer;
}

void PdfSignatureGenerator::SetSignature(const char* signature, int len) {
  PdfData signatureData(signature, len);
  m_pSignOutputDevice->SetSignature(signatureData);
  m_pSignOutputDevice->Flush();
}
#endif

void PdfSignatureGenerator::GetSignedPdf(UUCByteArray& signedPdf) {
  int finalLength = m_pSignOutputDevice->GetLength();
  char* szSignedPdf = new char[finalLength];

  m_pSignOutputDevice->Seek(0);
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
  int nRead = m_pSignOutputDevice->Read(szSignedPdf, finalLength);

  signedPdf.append((BYTE*)szSignedPdf, nRead);
#else
  m_pSignOutputDevice->Read(szSignedPdf, finalLength);
  signedPdf.append((BYTE*)szSignedPdf, finalLength);
#endif

  delete[] szSignedPdf;
}

const double PdfSignatureGenerator::getWidth(int pageIndex) {
  if (m_pPdfDocument) {
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
    PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
    return pPage->GetPageSize().GetWidth();
#else
    return m_pPdfDocument->GetPages().GetPageAt(pageIndex).GetRect().Width;
#endif
  }
  return 0;
}

const double PdfSignatureGenerator::getHeight(int pageIndex) {
  if (m_pPdfDocument) {
#if PODOFO_VERSION_MINOR < 10 && PODOFO_VERSION_MAJOR < 1
    PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
    return pPage->GetPageSize().GetHeight();
#else
    return m_pPdfDocument->GetPages().GetPageAt(pageIndex).GetRect().Height;
#endif
  }
  return 0;
}
