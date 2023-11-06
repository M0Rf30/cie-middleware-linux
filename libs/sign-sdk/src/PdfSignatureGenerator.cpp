/*
 *  PdfSignatureGenerator.cpp
 *  SignPoDoFo
 *
 *  Created by svp on 26/05/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "PdfSignatureGenerator.h"

#include "PdfVerifier.h"
#include "UUCLogger.h"

#define SIGNATURE_SIZE 10000

#ifdef CreateFont
#undef CreateFont
#endif

#ifdef GetObject
#undef GetObject
#endif

int GetNumberOfSignatures(PdfMemDocument* pPdfDocument);

USE_LOG;

size_t PdfSignatureGenerator::ReadForSignature(StreamDevice& device,
                                               size_t conentsBeaconOffset,
                                               size_t conentsm_beaconsize,
                                               char* buffer,
                                               size_t bufferSize) {
  if (device.Eof()) return 0;

  size_t pos = device.GetPosition();
  size_t readSize = 0;
  // Check if we are before beacon
  if (pos < conentsBeaconOffset) {
    readSize = std::min(bufferSize, conentsBeaconOffset - pos);
    if (readSize > 0) {
      device.Read(buffer, readSize);
      buffer += readSize;
      bufferSize -= readSize;
      if (bufferSize == 0) return readSize;
    }
  }

  // shift at the end of beacon
  if ((pos + readSize) >= conentsBeaconOffset &&
      pos < (conentsBeaconOffset + conentsm_beaconsize)) {
    device.Seek(conentsBeaconOffset + conentsm_beaconsize);
  }

  // read after beacon
  bufferSize = std::min(bufferSize, device.GetLength() - device.GetPosition());
  if (bufferSize == 0) return readSize;

  device.Read(buffer, bufferSize);
  return readSize + bufferSize;
}

PdfSignatureGenerator::PdfSignatureGenerator()
    : m_beacons(NULL),
      m_pPdfMemDocument(NULL),
      m_pSignatureField(NULL),
      m_pSignOutputDevice(NULL),
      m_pFinalOutDevice(NULL),
      m_pMainDocbuffer(NULL),
      m_pSignDocbuffer(NULL) {}

PdfSignatureGenerator::~PdfSignatureGenerator() {
  if (m_pPdfMemDocument) delete m_pPdfMemDocument;

  if (m_pSignatureField) delete m_pSignatureField;

  if (m_pSignOutputDevice) delete m_pSignOutputDevice;

  if (m_pFinalOutDevice) delete m_pFinalOutDevice;

  if (m_pMainDocbuffer) delete m_pMainDocbuffer;

  if (m_pSignDocbuffer) delete m_pSignDocbuffer;
}

int PdfSignatureGenerator::Load(const char* pdf, int len) {
  if (m_pPdfMemDocument) delete m_pPdfMemDocument;

  try {
    printf("PDF LENGTH");
    printf("%i", len);
    printf("STOP");

    m_pPdfMemDocument = new PdfMemDocument();
    m_pPdfMemDocument->Load(pdf);
    printf("OK m_pPdfMemDocument");
    int nSigns = PDFVerifier::GetNumberOfSignatures(m_pPdfMemDocument);
    printf("OK nSigns: %d", nSigns);

    if (nSigns > 0) {
      m_pPdfWriter->PdfWriter::SetIncrementalUpdate(true);
    }
    m_actualLen = len;

    return nSigns;
  } catch (::PoDoFo::PdfError& err) {
    return -2;
  } catch (...) {
    return -1;
  }
}

void PdfSignatureGenerator::AddFont(const char* szFontName,
                                    const char* szFontPath) {
  PdfFontCreateParams params;
  params.Encoding = PdfEncodingMapFactory::WinAnsiEncodingInstance();

  m_pPdfDocument->GetFonts().GetOrCreateFont(szFontPath, params);

  // old code
  // m_pPdfDocument->PoDoFo::PdfDocument::CreateFont(
  //     szFontName, false, PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
  //     PdfFontCache::eFontCreationFlags_AutoSelectBase14, true);

  // m_pPdfDocument->PoDoFo::PdfDocument::CreateFont(
  //     szFontName, true, PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
  //     PdfFontCache::eFontCreationFlags_AutoSelectBase14, true);
}

void PdfSignatureGenerator::SetGraphometricData(
    PdfSignature* pSignatureField, nullable<const PdfString&> vendor,
    nullable<const PdfString&> szGraphometricData,
    nullable<const PdfString&> szVersion) {

  if (pSignatureField == nullptr) throw std::invalid_argument("pSignatureField");
  if (szGraphometricData.has_value())
    pSignatureField->GetDictionary().AddKey("GraphometricData", *szGraphometricData);
  else
    pSignatureField->GetDictionary().RemoveKey("GraphometricData");

  if (pSignatureField == nullptr) throw std::invalid_argument("pSignatureField");
  if (szVersion.has_value())
    pSignatureField->GetDictionary().AddKey("Version", *szVersion);
  else
    pSignatureField->GetDictionary().RemoveKey("Version");
}

void PdfSignatureGenerator::AdjustByteRange(OutputStreamDevice& device,
                                            size_t byteRangeOffset,
                                            size_t conentsBeaconOffset,
                                            size_t conentsm_beaconsize,
                                            charbuff& buffer) {
  // Get final position
  size_t fileEnd = device.GetLength();
  PdfArray arr;
  arr.Add(PdfObject(static_cast<int64_t>(0)));
  arr.Add(PdfObject(static_cast<int64_t>(conentsBeaconOffset)));
  arr.Add(PdfObject(
      static_cast<int64_t>(conentsBeaconOffset + conentsm_beaconsize)));
  arr.Add(PdfObject(static_cast<int64_t>(
      fileEnd - (conentsBeaconOffset + conentsm_beaconsize))));

  device.Seek(byteRangeOffset);
  arr.Write(device, PdfWriteFlags::None, {}, buffer);
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
  LOG_DBG((0, "--> InitSignature",
           "%d, %f, %f, %f, %f, %s, %s, %s, %s, %s, %s, %s, %s", pageIndex,
           left, bottom, width, height, szReason, szName, szLocation,
           szFieldName, szSubFilter, szImagePath, szGraphometricData,
           szVersion));

  if (m_pSignatureField) delete m_pSignatureField;

  auto& pPage = m_pPdfMemDocument->GetPages().GetPageAt(pageIndex);
  Rect cropBox = pPage.GetCropBox();

  float left0 = left * cropBox.GetLeft();
  float bottom0 = cropBox.GetBottom() - (bottom * cropBox.GetBottom());

  float width0 = width * cropBox.GetRight();
  float height0 = height * cropBox.GetTop();

  printf("pdf rect: %f, %f, %f, %f\n", left0, bottom0, width0, height0);

  Rect rect(left0, bottom0, width0, height0);

  LOG_DBG((0, "InitSignature", "PdfSignatureField"));

  auto& m_pSignatureField = pPage.CreateField<PdfSignature>("Signature", rect);

  LOG_DBG((0, "InitSignature", "PdfSignatureField OK"));

  if (szReason && szReason[0]) {
    PdfString reason(szReason);
    PdfString reasonLabel(szReasonLabel);
    m_pSignatureField.SetSignatureReason(reason);
  }

  LOG_DBG((0, "InitSignature", "szReason OK"));

  if (szLocation && szLocation[0]) {
    PdfString location(szLocation);
    PdfString locationLabel(szLocationLabel);
    m_pSignatureField.SetSignatureLocation(location);
  }

  LOG_DBG((0, "InitSignature", "szLocation OK"));

  PdfDate now;
  m_pSignatureField.SetSignatureDate(now);

  LOG_DBG((0, "InitSignature", "Date OK"));

  // m_pSignOutputDevice->PdfSignOutputDevice::SetSignatureSize(SIGNATURE_SIZE);

  LOG_DBG((0, "InitSignature", "SIGNATURE_SIZE OK"));

  if (width * height > 0) {
    try {
      auto sigXObject = m_pPdfMemDocument->CreateXObjectForm(rect);
      m_pSignatureField.SetAppearanceStream(*sigXObject);
      LOG_DBG((0, "InitSignature", "SetAppearance OK"));
    } catch (PdfError& error) {
      LOG_ERR((0, "InitSignature", "SetAppearance error: %s, %s",
               PdfError::ErrorMessage(error.GetError()), error.what()));
    } catch (PdfError* perror) {
      LOG_ERR((0, "InitSignature", "SetAppearance error2: %s, %s",
               PdfError::ErrorMessage(perror->GetError()), perror->what()));
    } catch (std::exception& ex) {
      LOG_ERR(
          (0, "InitSignature", "SetAppearance std exception, %s", ex.what()));
    } catch (std::exception* pex) {
      LOG_ERR((0, "InitSignature", "SetAppearance std exception2, %s",
               pex->what()));
    } catch (...) {
      LOG_ERR((0, "InitSignature", "SetAppearance unknown error"));
    }
  }

  if (szGraphometricData && szGraphometricData[0])
    PdfSignatureGenerator::SetGraphometricData(
        &m_pSignatureField, PdfString("Aruba_Sign_Biometric_Data"),
        PdfString(szGraphometricData), PdfString(szVersion));

  // old code
  // m_pSignatureField->SetGraphometricData(PdfString("Aruba_Sign_Biometric_Data"),
  //                                        PdfString(szGraphometricData),
  //                                        PdfString(szVersion));

  LOG_DBG((0, "InitSignature", "szGraphometricData OK"));

  LOG_DBG((0, "InitSignature", "m_actualLen %d", m_actualLen));
  // crea il nuovo doc con il campo di firma
  int fulllen = m_actualLen * 2 + SIGNATURE_SIZE * 2;

  int mainDoclen = 0;
  m_pMainDocbuffer = NULL;
  while (!m_pMainDocbuffer) {
    try {
      LOG_DBG((0, "InitSignature", "fulllen %d", fulllen));
      m_pMainDocbuffer = new char[fulllen];
      OutputStreamDevice* pdfOutDevice;
      pdfOutDevice->Write(m_pMainDocbuffer, fulllen);
      m_pPdfMemDocument->Save(*pdfOutDevice);
      mainDoclen = pdfOutDevice->GetLength();
    } catch (::PoDoFo::PdfError err) {
      if (m_pMainDocbuffer) {
        delete m_pMainDocbuffer;
        m_pMainDocbuffer = NULL;
      }

      LOG_DBG((0, "PdfError", "what %s", err.what()));
      fulllen *= 2;
    }
  }

  LOG_DBG((0, "InitSignature", "m_pMainDocbuffer %d", fulllen));

  // alloca un SignOutputDevice
  m_pSignDocbuffer = new char[fulllen];

  LOG_DBG((0, "InitSignature", "m_pSignDocbuffer %d", fulllen));

  // old code
  // m_pFinalOutDevice = new PdfOutputDevice(m_pSignDocbuffer, fulllen);
  // m_pSignOutputDevice = new PdfSignOutputDevice(m_pFinalOutDevice);

  OutputStreamDevice* m_pFinalOutDevice;
  OutputStreamDevice* m_pSignOutputDevice;
  m_pFinalOutDevice->Write(m_pSignDocbuffer, fulllen);
  LOG_DBG((0, "InitSignature", "buffers OK %d", fulllen));

  // imposta la firma
  // old code
  // m_pSignOutputDevice->SetSignatureSize(SIGNATURE_SIZE);

  // LOG_DBG((0, "InitSignature", "SetSignatureSize OK %d", SIGNATURE_SIZE));

  // Scrive il documento reale
  m_pSignOutputDevice->Write(m_pMainDocbuffer, mainDoclen);

  LOG_DBG((0, "InitSignature", "Write OK %d", mainDoclen));
  // old code
  // m_pSignOutputDevice->AdjustByteRange();

  charbuff buffer;
  AdjustByteRange(*m_pSignOutputDevice, *m_beacons->ByteRangeOffset,
                  *m_beacons->ContentsOffset, m_beacons->ContentsBeacon.size(),
                  buffer);

  LOG_DBG((0, "InitSignature", "AdjustByteRange OK"));
}

void PdfSignatureGenerator::GetBufferForSignature(UUCByteArray& toSign) {
  // old code
  // char* buffer = new char[len];

  int len = m_pSignOutputDevice->GetLength() * 2;
  size_t readBytes;
  PoDoFo::charbuff* buffer = new PoDoFo::charbuff(len);

  m_pSignOutputDevice->Seek(0);

  // old code
  // int nRead = m_pSignOutputDevice->ReadForSignature(buffer, len);
  // if (nRead == -1) throw nRead;

  while ((readBytes = ReadForSignature(
              *m_pSignOutputDevice, *m_beacons->ContentsOffset,
              m_beacons->ContentsBeacon.size(), buffer->data(), len)) != 0) {
    toSign.append((BYTE*)buffer, readBytes);
  }
  // old code
  // toSign.append((BYTE*)buffer, readBytes);

  delete[] buffer;
}

void PdfSignatureGenerator::SetSignature(const char* signature, int len) {
  // old code
  // PdfData signatureData(signature, len);
  // m_pSignOutputDevice->SetSignature(signatureData);
  m_pSignOutputDevice->Write(signature, len);
}

void PdfSignatureGenerator::GetSignedPdf(UUCByteArray& signedPdf) {
  int finalLength = m_pSignOutputDevice->GetLength();
  // old code
  // char* szSignedPdf = new char[finalLength];
  PoDoFo::charbuff* szSignedPdf = new PoDoFo::charbuff(finalLength);
  size_t readBytes;

  m_pSignOutputDevice->Seek(0);
  // old code
  // int nRead = m_pSignOutputDevice->Read(szSignedPdf, finalLength);

  while ((readBytes =
              ReadForSignature(*m_pSignOutputDevice, *m_beacons->ContentsOffset,
                               m_beacons->ContentsBeacon.size(),
                               szSignedPdf->data(), finalLength)) != 0) {
    signedPdf.append((BYTE*)szSignedPdf, readBytes);
  }

  // old code
  // signedPdf.append((BYTE*)szSignedPdf, nRead);

  delete[] szSignedPdf;
}

const double PdfSignatureGenerator::getWidth(int pageIndex) {
  if (m_pPdfMemDocument) {
    auto& pPage = m_pPdfMemDocument->GetPages().GetPageAt(pageIndex);
    return pPage.GetCropBox().Width;
  }
  return 0;
}

const double PdfSignatureGenerator::getHeight(int pageIndex) {
  if (m_pPdfMemDocument) {
    auto& pPage = m_pPdfMemDocument->GetPages().GetPageAt(pageIndex);
    return pPage.GetCropBox().Height;
  }
  return 0;
}

const double PdfSignatureGenerator::lastSignatureY(int left, int bottom) {
  if (!m_pPdfMemDocument) return -1;
  /// Find the document catalog dictionary
  const PdfTrailer& trailer = m_pPdfMemDocument->GetTrailer();

  const PdfObject* const catalogRef =
      trailer.GetDictionary().GetKey(PdfName("Root"));
  if (catalogRef == 0 || !catalogRef->IsReference())
    return -2;  // throw std::invalid_argument("Invalid /Root entry");
  const PdfObject* const catalog =
      m_pPdfMemDocument->GetObjects().GetObject(catalogRef->GetReference());
  if (catalog == 0 || !catalog->IsDictionary())
    return -3;  // throw std::invalid_argument("Invalid or non-dictionary
  // referenced by /Root entry");

  /// Find the Fields array in catalog dictionary
  const PdfObject* acroFormValue =
      catalog->GetDictionary().GetKey(PdfName("AcroForm"));
  if (acroFormValue == 0) return bottom;
  if (acroFormValue->IsReference())
    acroFormValue = m_pPdfMemDocument->GetObjects().GetObject(
        acroFormValue->GetReference());

  if (!acroFormValue->IsDictionary()) return bottom;

  const PdfObject* fieldsValue =
      acroFormValue->GetDictionary().GetKey(PdfName("Fields"));
  if (fieldsValue == 0) return bottom;

  if (fieldsValue->IsReference())
    fieldsValue = m_pPdfMemDocument->GetObjects().GetObject(
        acroFormValue->GetReference());

  if (!fieldsValue->IsArray()) return bottom;

  vector<const PdfObject*> signatureVector;

  /// Verify if each object of the array is a signature field
  const PdfArray& array = fieldsValue->GetArray();

  int minY = bottom;

  for (unsigned int i = 0; i < array.size(); i++) {
    const PdfObject* pObj =
        m_pPdfMemDocument->GetObjects().GetObject(array[i].GetReference());
    if (IsSignatureField(m_pPdfMemDocument, pObj)) {
      const PdfObject* const keyRect =
          pObj->GetDictionary().GetKey(PdfName("Rect"));
      if (keyRect == 0) {
        return bottom;
      }
      PdfArray rectArray = keyRect->GetArray();
      Rect rect;
      rect.FromArray(rectArray);

      if (rect.GetLeft() == left) {
        minY = (rect.GetBottom() <= minY && rect.GetBottom() != 0)
                   ? rect.GetBottom() - 85
                   : minY;
      }
    }
  }
  return minY;
}

bool PdfSignatureGenerator::IsSignatureField(const PdfMemDocument* pDoc,
                                             const PdfObject* const pObj) {
  if (pObj == 0) return false;

  if (!pObj->IsDictionary()) return false;

  const PdfObject* const keyFTValue =
      pObj->GetDictionary().GetKey(PdfName("FT"));
  if (keyFTValue == 0) return false;

  string value;
  keyFTValue->ToString(value);
  if (value != "/Sig") return false;

  const PdfObject* const keyVValue = pObj->GetDictionary().GetKey(PdfName("V"));
  if (keyVValue == 0) return false;

  const PdfObject* const signature =
      pDoc->GetObjects().GetObject(keyVValue->GetReference());
  if (signature->IsDictionary())
    return true;
  else
    return false;
}
