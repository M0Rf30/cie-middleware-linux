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

PdfSignatureGenerator::PdfSignatureGenerator()
    : m_pPdfDocument(NULL),
      m_pSignatureField(NULL),
      m_pSignOutputDevice(NULL),
      m_pFinalOutDevice(NULL),
      m_pMainDocbuffer(NULL),
      m_pSignDocbuffer(NULL) {
  PoDoFo::PdfError::EnableLogging(false);
}

PdfSignatureGenerator::~PdfSignatureGenerator() {
  if (m_pPdfDocument) delete m_pPdfDocument;

  if (m_pSignatureField) delete m_pSignatureField;

  if (m_pSignOutputDevice) delete m_pSignOutputDevice;

  if (m_pFinalOutDevice) delete m_pFinalOutDevice;

  if (m_pMainDocbuffer) delete m_pMainDocbuffer;

  if (m_pSignDocbuffer) delete m_pSignDocbuffer;
}

int PdfSignatureGenerator::Load(const char* pdf, int len) {
  if (m_pPdfDocument) delete m_pPdfDocument;

  try {
    printf("PDF LENGTH");
    printf("%i", len);
    printf("STOP");

    m_pPdfDocument = new PdfMemDocument();
    m_pPdfDocument->LoadFromBuffer(pdf, len, true);
    printf("OK m_pPdfDocument");
    int nSigns = PDFVerifier::GetNumberOfSignatures(m_pPdfDocument);
    printf("OK nSigns: %d", nSigns);

    // TODO: vanilla
    // if (nSigns > 0) {
    //   m_pPdfDocument->SetIncrementalUpdates(true);
    // }

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
  m_pPdfDocument->CreateFont(
      szFontName, false, false, false,
      PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
      PdfFontCache::eFontCreationFlags_AutoSelectBase14, true, szFontPath);
  m_pPdfDocument->CreateFont(
      szFontName, true, false, false,
      PdfEncodingFactory::GlobalWinAnsiEncodingInstance(),
      PdfFontCache::eFontCreationFlags_AutoSelectBase14, true, szFontPath);
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

  PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
  PdfRect cropBox = pPage->GetCropBox();

  float left0 = left * cropBox.GetWidth();
  float bottom0 = cropBox.GetHeight() - (bottom * cropBox.GetHeight());

  float width0 = width * cropBox.GetWidth();
  float height0 = height * cropBox.GetHeight();

  printf("pdf rect: %f, %f, %f, %f\n", left0, bottom0, width0, height0);

  PdfRect rect(left0, bottom0, width0, height0);

  LOG_DBG((0, "InitSignature", "PdfSignatureField"));

  m_pSignatureField = new PdfSignatureField(pPage, rect, m_pPdfDocument);

  LOG_DBG((0, "InitSignature", "PdfSignatureField OK"));

  if (szReason && szReason[0]) {
    PdfString reason(szReason);
    m_pSignatureField->SetSignatureReason(reason);
  }

  LOG_DBG((0, "InitSignature", "szReason OK"));

  if (szLocation && szLocation[0]) {
    PdfString location(szLocation);
    m_pSignatureField->SetSignatureLocation(location);
  }

  LOG_DBG((0, "InitSignature", "szLocation OK"));

  PdfDate now;
  m_pSignatureField->SetSignatureDate(now);

  LOG_DBG((0, "InitSignature", "Date OK"));

  // TODO: vanilla
  if (szName && szName[0]) {
    PdfString name(szName);
    PdfString nameLabel(szNameLabel);
    // m_pSignatureField->SetSignatureName(nameLabel, name);
    SetSignatureName(m_pSignatureField, name);
  }

  LOG_DBG((0, "InitSignature", "szName OK"));

  // TODO: vanilla
  // m_pSignatureField->SetSignatureSize(SIGNATURE_SIZE);
  SetSignatureSize(m_pSignatureField, SIGNATURE_SIZE);

  LOG_DBG((0, "InitSignature", "SIGNATURE_SIZE OK"));

  if (width * height > 0) {
    try {
      // TODO: vanilla
      SetAppearance(m_pSignatureField, szImagePath, szDescription, false);
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

  // TODO: vanilla
  if (szGraphometricData && szGraphometricData[0])
    SetGraphometricData(m_pSignatureField, PdfString(szGraphometricData),
                        PdfString(szVersion));

  LOG_DBG((0, "InitSignature", "szGraphometricData OK"));

  LOG_DBG((0, "InitSignature", "m_actualLen %d", m_actualLen));
  // crea il nuovo doc con il campo di firma
  int fulllen = m_actualLen * 2 + SIGNATURE_SIZE * 2 +
                (szGraphometricData
                     ? (strlen(szGraphometricData) + strlen(szVersion) + 100)
                     : 0);

  int mainDoclen = 0;
  m_pMainDocbuffer = NULL;
  while (!m_pMainDocbuffer) {
    try {
      LOG_DBG((0, "InitSignature", "fulllen %d", fulllen));
      m_pMainDocbuffer = new char[fulllen];
      PdfOutputDevice pdfOutDevice(m_pMainDocbuffer, fulllen);
      m_pPdfDocument->Write(&pdfOutDevice);
      mainDoclen = pdfOutDevice.GetLength();
    } catch (const ::PoDoFo::PdfError& err) {
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

  m_pFinalOutDevice = new PdfOutputDevice(m_pSignDocbuffer, fulllen);
  m_pSignOutputDevice = new PdfSignOutputDevice(m_pFinalOutDevice);

  LOG_DBG((0, "InitSignature", "buffers OK %d", fulllen));

  // imposta la firma
  m_pSignOutputDevice->SetSignatureSize(SIGNATURE_SIZE);

  LOG_DBG((0, "InitSignature", "SetSignatureSize OK %d", SIGNATURE_SIZE));

  // Scrive il documento reale
  m_pSignOutputDevice->Write(m_pMainDocbuffer, mainDoclen);

  LOG_DBG((0, "InitSignature", "Write OK %d", mainDoclen));

  m_pSignOutputDevice->AdjustByteRange();

  LOG_DBG((0, "InitSignature", "AdjustByteRange OK"));
}

void PdfSignatureGenerator::GetBufferForSignature(UUCByteArray& toSign) {
  // int fulllen = m_actualLen * 2 + SIGNATURE_SIZE * 2;
  int len = m_pSignOutputDevice->GetLength() * 2;

  char* buffer = new char[len];

  m_pSignOutputDevice->Seek(0);

  int nRead = m_pSignOutputDevice->ReadForSignature(buffer, len);
  if (nRead == -1) throw nRead;

  toSign.append((BYTE*)buffer, nRead);

  delete[] buffer;
}

void PdfSignatureGenerator::SetSignature(const char* signature, int len) {
  PdfData signatureData(signature, len);
  m_pSignOutputDevice->SetSignature(signatureData);
}

void PdfSignatureGenerator::GetSignedPdf(UUCByteArray& signedPdf) {
  int finalLength = m_pSignOutputDevice->GetLength();
  char* szSignedPdf = new char[finalLength];

  m_pSignOutputDevice->Seek(0);
  int nRead = m_pSignOutputDevice->Read(szSignedPdf, finalLength);

  signedPdf.append((BYTE*)szSignedPdf, nRead);

  delete[] szSignedPdf;
}

const double PdfSignatureGenerator::getWidth(int pageIndex) {
  if (m_pPdfDocument) {
    PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
    return pPage->GetPageSize().GetWidth();
  }
  return 0;
}

const double PdfSignatureGenerator::getHeight(int pageIndex) {
  if (m_pPdfDocument) {
    PdfPage* pPage = m_pPdfDocument->GetPage(pageIndex);
    return pPage->GetPageSize().GetHeight();
  }
  return 0;
}

const double PdfSignatureGenerator::lastSignatureY(int left, int bottom) {
  if (!m_pPdfDocument) return -1;
  /// Find the document catalog dictionary
  const PdfObject* const trailer = m_pPdfDocument->GetTrailer();
  if (!trailer->IsDictionary()) return -1;
  const PdfObject* const catalogRef =
      trailer->GetDictionary().GetKey(PdfName("Root"));
  if (catalogRef == 0 || !catalogRef->IsReference())
    return -2;  // throw std::invalid_argument("Invalid /Root entry");
  const PdfObject* const catalog =
      m_pPdfDocument->GetObjects().GetObject(catalogRef->GetReference());
  if (catalog == 0 || !catalog->IsDictionary())
    return -3;  // throw std::invalid_argument("Invalid or non-dictionary
  // referenced by /Root entry");

  /// Find the Fields array in catalog dictionary
  const PdfObject* acroFormValue =
      catalog->GetDictionary().GetKey(PdfName("AcroForm"));
  if (acroFormValue == 0) return bottom;
  if (acroFormValue->IsReference())
    acroFormValue =
        m_pPdfDocument->GetObjects().GetObject(acroFormValue->GetReference());

  if (!acroFormValue->IsDictionary()) return bottom;

  const PdfObject* fieldsValue =
      acroFormValue->GetDictionary().GetKey(PdfName("Fields"));
  if (fieldsValue == 0) return bottom;

  if (fieldsValue->IsReference())
    fieldsValue =
        m_pPdfDocument->GetObjects().GetObject(acroFormValue->GetReference());

  if (!fieldsValue->IsArray()) return bottom;

  vector<const PdfObject*> signatureVector;

  /// Verify if each object of the array is a signature field
  const PdfArray& array = fieldsValue->GetArray();

  int minY = bottom;

  for (unsigned int i = 0; i < array.size(); i++) {
    const PdfObject* pObj =
        m_pPdfDocument->GetObjects().GetObject(array[i].GetReference());
    if (IsSignatureField(m_pPdfDocument, pObj)) {
      const PdfObject* const keyRect =
          pObj->GetDictionary().GetKey(PdfName("Rect"));
      if (keyRect == 0) {
        return bottom;
      }
      PdfArray rectArray = keyRect->GetArray();
      PdfRect rect;
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

void PdfSignatureGenerator::SetAppearance(PdfSignatureField* m_pSignatureField,
                                          const char* szImagePath,
                                          const char* szDescription,
                                          const bool showData) {
  if (!m_pSignatureField) {
    PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);
  }

  //  bool bVar1;
  //  char cVar2;
  //  EFontCreationFlags EVar3;
  //  PdfVecObjects *pPVar4;
  //  PdfImage *this_00;
  //  PdfName *pPVar5;
  //  char *pcVar6;
  //  long lVar7;
  //  PdfFont *this_01;
  //  undefined8 uVar8;
  //  undefined8 uVar9;
  //  PdfReference *pPVar10;
  //  long in_FS_OFFSET;
  //  double width;
  //  double height;
  //  double width_1;
  //  double height_1;
  //  int local_1154;
  //  double local_1150;
  //  double local_1148;
  PdfRect rect;
  //  PdfRect local_10c8 [32];
  //  PdfDictionary local_10a8 [80];
  //  allocator<char> local_1058 [80];
  //  basic_string<> local_1008 [32];
  //  basic_string local_fe8 [32];
  //  basic_string<> local_fc8 [32];
  //  basic_string<> local_fa8 [32];
  //  basic_string<> local_f88 [32];
  //  basic_string local_f68 [32];
  //  basic_string<> local_f48 [32];
  //  basic_string<> local_f28 [32];
  //  basic_string<> local_f08 [32];
  //  basic_string<> local_ee8 [32];
  //  basic_string<> local_ec8 [32];
  //  basic_string<> local_ea8 [32];
  //  basic_string<> local_e88 [32];
  //  basic_string<> local_e68 [32];
  //  basic_string<> local_e48 [32];
  //  basic_string<> local_e28 [32];
  //  basic_string<> local_e08 [32];
  //  basic_string<> local_de8 [32];
  //  basic_string<> local_dc8 [32];
  //  basic_string<> local_da8 [32];
  //  basic_string<> local_d88 [48];
  //  PdfXObject local_d58 [144];
  //  PdfXObject local_cc8 [144];
  //  basic_string<> local_c38 [144];
  //  basic_string<> local_ba8 [144];
  //  PdfPainter local_b18 [592];
  //  basic_string<> local_8c8 [592];
  //  PdfName local_678 [592];
  //  char local_428 [1032];
  //  long local_20;

  //  local_20 = *(long *)(in_FS_OFFSET + 0x28);
  m_pSignatureField->SetBackgroundColorTransparent();
  //  width = (double)PdfRect::GetWidth();
  //  height = (double)PdfRect::GetHeight();
  //  width_1 = (double)PdfRect::GetHeight();
  //  height_1 = (double)PdfRect::GetWidth();
  // PoDoFo::PdfRect::PdfRect(rect, 0.0, 0.0, height_1, width_1);
  //  *(undefined8 *)(this + 0xa8) = 0;
  //  if ((szImagePath != (char *)0x0) && (*szImagePath != '\0')) {
  //       PdfField::GetFieldObject();
  //       pPVar4 = (PdfVecObjects *)PdfObject::GetOwner();
  //       this_00 = (PdfImage *)operator.new(0x90);
  //                                                /* try { // try from
  //                                                00100cd6 to 00100cda has
  //                                                its CatchHandler @
  //                                                00101e80 */
  //       PoDoFo::PdfImage::PdfImage(this_00,pPVar4,"Im");
  //       *(PdfImage **)(this + 0xa8) = this_00;
  //       PoDoFo::PdfImage::LoadFromPng(*(char **)(this + 0xa8));
  //       PoDoFo::PdfImage::SetImageChromaKeyMask(*(long *)(this +
  //       0xa8),0xf0,0xf0,0xf0); PdfElement::GetObject(); pPVar5 = (PdfName
  //       *)PdfVariant::GetDictionary(); PdfName::PdfName(local_678,"BBox");
  //                                                /* try { // try from
  //                                                00100d78 to 00100d7c has
  //                                                its CatchHandler @
  //                                                00101e96 */
  //       PoDoFo::PdfDictionary::RemoveKey(pPVar5);
  //       PoDoFo::PdfName::~PdfName(local_678);
  //  }
  //  PoDoFo::PdfRect::PdfRect(local_10c8,0.0,0.0,100.0,100.0);
  //  PoDoFo::PdfXObject::PdfXObject(local_d58,local_10c8,*(PdfDocument
  //  **)(this + 0xb0),"#n0"); std::allocator<char>::allocator();
  //                                                /* try { // try from
  //                                                00100e0d to 00100e11 has
  //                                                its CatchHandler @
  //                                                00101eb3 */
  //  std::__cxx11::basic_string<>::basic_string((char *)local_1008,(allocator
  //  *)"% DSBlank\n"); std::allocator<char>::~allocator(local_1058);
  //  PdfElement::GetObject();
  //                                                /* try { // try from
  //                                                00100e33 to 00100e9f has
  //                                                its CatchHandler @
  //                                                0010241d */
  //  pcVar6 = (char *)PoDoFo::PdfObject::GetStream();
  //  std::__cxx11::basic_string<>::length();
  //  lVar7 = std::__cxx11::basic_string<>::c_str();
  //  PoDoFo::PdfStream::Set(pcVar6,lVar7);
  //  PoDoFo::PdfXObject::PdfXObject(local_cc8,rect,*(PdfDocument
  //  **)(this + 0xb0),"#n2"); local_1150 = 1.0; if (*(long *)(this + 0xa8) !=
  //  0) {
  //                                                /* try { // try from
  //                                                00100efc to 0010107a has
  //                                                its CatchHandler @
  //                                                00102409 */
  //       local_1148 = (double)PdfImage::GetHeight();
  //       width_1 = (double)PdfImage::GetWidth();
  //       local_1148 = height / local_1148;
  //       if (width / width_1 < local_1148) {
  //            local_1148 = width / width_1;
  //       }
  //       if (local_1148 < 1.0) {
  //            local_1150 = local_1148;
  //       }
  //       PdfImage::GetHeight();
  //       PdfImage::GetWidth();
  //  }
  //  pcVar6 = *(char **)(this + 0xb0);
  //  EVar3 = PoDoFo::PdfEncodingFactory::GlobalWinAnsiEncodingInstance();
  //  this_01 = (PdfFont *)
  //                           PoDoFo::PdfDocument::CreateFont
  //                                                     (pcVar6,true,false,(PdfEncoding
  //                                                     *)0x0,EVar3,true,(char
  //                                                     *)0x1);
  //  PdfFont::SetFontSize(this_01,10.0);
  //  PoDoFo::PdfPainter::PdfPainter(local_b18);
  //                                                /* try { // try from
  //                                                00101093 to 001011a7 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //  PoDoFo::PdfPainter::SetPage((PdfCanvas *)local_b18);
  //  if (*(long *)(this + 0xa8) != 0) {
  //       PoDoFo::PdfPainter::DrawImage(0.0,0.0,(PdfImage
  //       *)local_b18,local_1150,local_1150);
  //  }
  //  PoDoFo::PdfPainter::SetFont((PdfFont *)local_b18);
  //  height = height - 10.0;
  //  local_1154 = 0;
  //  if ((szDescription != (char *)0x0) && (*szDescription != '\0')) {
  //       local_1154 = 10;
  //       strcpy(local_428,szDescription);
  //       PoDoFo::PdfString::PdfString((PdfString
  //       *)local_678,local_428,(PdfEncoding *)0x0);
  //                                                /* try { // try from
  //                                                001011f4 to 001011f8 has
  //                                                its CatchHandler @
  //                                                00101eca */
  //       PoDoFo::PdfPainter::DrawText(5.0,(height + 0.0) - 10.0,(PdfString
  //       *)local_b18);
  //                                                /* try { // try from
  //                                                00101203 to 00101233 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //       PoDoFo::PdfString::~PdfString((PdfString *)local_678);
  //  }
  //  pPVar5 = (PdfName *)PdfVariant::GetDictionary();
  //  PdfName::PdfName(local_678,"Name");
  //                                                /* try { // try from
  //                                                00101241 to 00101245 has
  //                                                its CatchHandler @
  //                                                00101ee1 */
  //  cVar2 = PoDoFo::PdfDictionary::HasKey(pPVar5);
  //                                                /* try { // try from
  //                                                00101252 to 001012d1 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //  PoDoFo::PdfName::~PdfName(local_678);
  //  if (cVar2 != '\0') {
  //       local_1154 = local_1154 + 10;
  //       uVar8 = PdfString::GetString();
  //       uVar9 = PdfString::GetString();
  //       sprintf(local_428,"%s %s",uVar9,uVar8);
  //       PoDoFo::PdfString::PdfString((PdfString
  //       *)local_678,local_428,(PdfEncoding *)0x0);
  //                                                /* try { // try from
  //                                                0010131e to 00101322 has
  //                                                its CatchHandler @
  //                                                00101ef8 */
  //       PoDoFo::PdfPainter::DrawText(5.0,(height + 0.0) -
  //       (double)local_1154,(PdfString *)local_b18);
  //                                                /* try { // try from
  //                                                0010132d to 0010135d has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //       PoDoFo::PdfString::~PdfString((PdfString *)local_678);
  //  }
  //  pPVar5 = (PdfName *)PdfVariant::GetDictionary();
  //  PdfName::PdfName(local_678,"Reason");
  //                                                /* try { // try from
  //                                                0010136b to 0010136f has
  //                                                its CatchHandler @
  //                                                00101f0f */
  //  cVar2 = PoDoFo::PdfDictionary::HasKey(pPVar5);
  //                                                /* try { // try from
  //                                                0010137c to 001013fb has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //  PoDoFo::PdfName::~PdfName(local_678);
  //  if (cVar2 != '\0') {
  //       local_1154 = local_1154 + 10;
  //       uVar8 = PdfString::GetString();
  //       uVar9 = PdfString::GetString();
  //       sprintf(local_428,"%s %s",uVar9,uVar8);
  //       PoDoFo::PdfString::PdfString((PdfString
  //       *)local_678,local_428,(PdfEncoding *)0x0);
  //                                                /* try { // try from
  //                                                00101448 to 0010144c has
  //                                                its CatchHandler @
  //                                                00101f26 */
  //       PoDoFo::PdfPainter::DrawText(5.0,(height + 0.0) -
  //       (double)local_1154,(PdfString *)local_b18);
  //                                                /* try { // try from
  //                                                00101457 to 00101487 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //       PoDoFo::PdfString::~PdfString((PdfString *)local_678);
  //  }
  //  pPVar5 = (PdfName *)PdfVariant::GetDictionary();
  //  PdfName::PdfName(local_678,"Location");
  //                                                /* try { // try from
  //                                                00101495 to 00101499 has
  //                                                its CatchHandler @
  //                                                00101f3d */
  //  cVar2 = PoDoFo::PdfDictionary::HasKey(pPVar5);
  //                                                /* try { // try from
  //                                                001014a6 to 00101525 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //  PoDoFo::PdfName::~PdfName(local_678);
  //  if (cVar2 != '\0') {
  //       local_1154 = local_1154 + 10;
  //       uVar8 = PdfString::GetString();
  //       uVar9 = PdfString::GetString();
  //       sprintf(local_428,"%s %s",uVar9,uVar8);
  //       PoDoFo::PdfString::PdfString((PdfString
  //       *)local_678,local_428,(PdfEncoding *)0x0);
  //                                                /* try { // try from
  //                                                00101572 to 00101576 has
  //                                                its CatchHandler @
  //                                                00101f54 */
  //       PoDoFo::PdfPainter::DrawText(5.0,(height + 0.0) -
  //       (double)local_1154,(PdfString *)local_b18);
  //                                                /* try { // try from
  //                                                00101581 to 00101585 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //       PoDoFo::PdfString::~PdfString((PdfString *)local_678);
  //  }
  //                                                /* try { // try from
  //                                                00101599 to 001015cd has
  //                                                its CatchHandler @
  //                                                00101f6b */
  //  pPVar5 = (PdfName *)PdfVariant::GetDictionary();
  //  PdfName::PdfName(local_678,"M");
  //  cVar2 = PoDoFo::PdfDictionary::HasKey(pPVar5);
  //  if ((cVar2 == '\0') || (!showData)) {
  //       bVar1 = false;
  //  }
  //  else {
  //       bVar1 = true;
  //  }
  //                                                /* try { // try from
  //                                                001015f7 to 00101635 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //  PoDoFo::PdfName::~PdfName(local_678);
  //  if (bVar1) {
  //       PdfString::GetStringUtf8[abi:cxx11]();
  //       std::__cxx11::basic_string<>::basic_string(local_fe8);
  //       std::allocator<char>::allocator();
  //                                                /* try { // try from
  //                                                0010165d to 00101661 has
  //                                                its CatchHandler @
  //                                                00101f89 */
  //       std::__cxx11::basic_string<>::basic_string((char
  //       *)local_fc8,(allocator *)&DAT_00103d3c);
  //       std::allocator<char>::~allocator(local_1058);
  //       std::allocator<char>::allocator();
  //                                                /* try { // try from
  //                                                00101698 to 0010169c has
  //                                                its CatchHandler @
  //                                                00101fa0 */
  //       std::__cxx11::basic_string<>::basic_string((char
  //       *)local_fa8,(allocator *)&DAT_00103d3e);
  //       std::allocator<char>::~allocator(local_1058);
  //       std::allocator<char>::allocator();
  //                                                /* try { // try from
  //                                                001016d3 to 001016d7 has
  //                                                its CatchHandler @
  //                                                00101fb7 */
  //       std::__cxx11::basic_string<>::basic_string((char
  //       *)local_f88,(allocator *)&DAT_00103d40);
  //       std::allocator<char>::~allocator(local_1058);
  //                                                /* try { // try from
  //                                                00101702 to 00101706 has
  //                                                its CatchHandler @
  //                                                001022ee */
  //       std::__cxx11::basic_string<>::substr((ulong)local_678,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                00101722 to 00101726 has
  //                                                its CatchHandler @
  //                                                0010229e */
  //       std::__cxx11::basic_string<>::substr((ulong)local_ba8,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                00101742 to 00101746 has
  //                                                its CatchHandler @
  //                                                00102276 */
  //       std::__cxx11::basic_string<>::substr((ulong)local_d88,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                00101762 to 00101766 has
  //                                                its CatchHandler @
  //                                                0010224e */
  //       std::__cxx11::basic_string<>::substr((ulong)local_de8,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                00101782 to 00101786 has
  //                                                its CatchHandler @
  //                                                00102226 */
  //       std::__cxx11::basic_string<>::substr((ulong)local_e48,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                001017a2 to 001017a6 has
  //                                                its CatchHandler @
  //                                                001021fe */
  //       std::__cxx11::basic_string<>::substr((ulong)local_ea8,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                001017c2 to 001017c6 has
  //                                                its CatchHandler @
  //                                                001021d6 */
  //       std::__cxx11::basic_string<>::substr((ulong)local_f08,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                001017e2 to 001017e6 has
  //                                                its CatchHandler @
  //                                                001021ae */
  //       std::__cxx11::basic_string<>::substr((ulong)local_f48,(ulong)local_fe8);
  //                                                /* try { // try from
  //                                                00101802 to 00101806 has
  //                                                its CatchHandler @
  //                                                00102186 */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_f28,local_f48,local_fc8);
  //                                                /* try { // try from
  //                                                00101822 to 00101826 has
  //                                                its CatchHandler @
  //                                                0010215e */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_ee8,local_f28,local_f08);
  //                                                /* try { // try from
  //                                                00101842 to 00101846 has
  //                                                its CatchHandler @
  //                                                00102136 */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_ec8,local_ee8,local_fc8);
  //                                                /* try { // try from
  //                                                00101862 to 00101866 has
  //                                                its CatchHandler @
  //                                                0010210e */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_e88,local_ec8,local_ea8);
  //                                                /* try { // try from
  //                                                00101882 to 00101886 has
  //                                                its CatchHandler @
  //                                                001020e6 */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_e68,local_e88,local_f88);
  //                                                /* try { // try from
  //                                                001018a2 to 001018a6 has
  //                                                its CatchHandler @
  //                                                001020be */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_e28,local_e68,local_e48);
  //                                                /* try { // try from
  //                                                001018c2 to 001018c6 has
  //                                                its CatchHandler @
  //                                                00102096 */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_e08,local_e28,local_fa8);
  //                                                /* try { // try from
  //                                                001018e2 to 001018e6 has
  //                                                its CatchHandler @
  //                                                0010206e */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_dc8,local_e08,local_de8);
  //                                                /* try { // try from
  //                                                00101902 to 00101906 has
  //                                                its CatchHandler @
  //                                                00102046 */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_da8,local_dc8,local_fa8);
  //                                                /* try { // try from
  //                                                00101922 to 00101926 has
  //                                                its CatchHandler @
  //                                                0010201e */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_c38,local_da8,local_d88);
  //                                                /* try { // try from
  //                                                00101942 to 00101946 has
  //                                                its CatchHandler @
  //                                                00101ff6 */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_8c8,local_c38,local_ba8);
  //                                                /* try { // try from
  //                                                00101962 to 00101966 has
  //                                                its CatchHandler @
  //                                                00101fce */
  //       _ZStplIcSt11char_traitsIcESaIcEENSt7__cxx1112basic_stringIT_T0_T1_EERKS8_SA_
  //                                 (local_f68,local_8c8,local_678);
  //                                                /* try { // try from
  //                                                00101971 to 00101975 has
  //                                                its CatchHandler @
  //                                                00101fe2 */
  //       std::__cxx11::basic_string<>::~basic_string(local_8c8);
  //                                                /* try { // try from
  //                                                00101980 to 00101984 has
  //                                                its CatchHandler @
  //                                                0010200a */
  //       std::__cxx11::basic_string<>::~basic_string(local_c38);
  //                                                /* try { // try from
  //                                                0010198f to 00101993 has
  //                                                its CatchHandler @
  //                                                00102032 */
  //       std::__cxx11::basic_string<>::~basic_string(local_da8);
  //                                                /* try { // try from
  //                                                0010199e to 001019a2 has
  //                                                its CatchHandler @
  //                                                0010205a */
  //       std::__cxx11::basic_string<>::~basic_string(local_dc8);
  //                                                /* try { // try from
  //                                                001019ad to 001019b1 has
  //                                                its CatchHandler @
  //                                                00102082 */
  //       std::__cxx11::basic_string<>::~basic_string(local_e08);
  //                                                /* try { // try from
  //                                                001019bc to 001019c0 has
  //                                                its CatchHandler @
  //                                                001020aa */
  //       std::__cxx11::basic_string<>::~basic_string(local_e28);
  //                                                /* try { // try from
  //                                                001019cb to 001019cf has
  //                                                its CatchHandler @
  //                                                001020d2 */
  //       std::__cxx11::basic_string<>::~basic_string(local_e68);
  //                                                /* try { // try from
  //                                                001019da to 001019de has
  //                                                its CatchHandler @
  //                                                001020fa */
  //       std::__cxx11::basic_string<>::~basic_string(local_e88);
  //                                                /* try { // try from
  //                                                001019e9 to 001019ed has
  //                                                its CatchHandler @
  //                                                00102122 */
  //       std::__cxx11::basic_string<>::~basic_string(local_ec8);
  //                                                /* try { // try from
  //                                                001019f8 to 001019fc has
  //                                                its CatchHandler @
  //                                                0010214a */
  //       std::__cxx11::basic_string<>::~basic_string(local_ee8);
  //                                                /* try { // try from
  //                                                00101a07 to 00101a0b has
  //                                                its CatchHandler @
  //                                                00102172 */
  //       std::__cxx11::basic_string<>::~basic_string(local_f28);
  //                                                /* try { // try from
  //                                                00101a16 to 00101a1a has
  //                                                its CatchHandler @
  //                                                0010219a */
  //       std::__cxx11::basic_string<>::~basic_string(local_f48);
  //                                                /* try { // try from
  //                                                00101a25 to 00101a29 has
  //                                                its CatchHandler @
  //                                                001021c2 */
  //       std::__cxx11::basic_string<>::~basic_string(local_f08);
  //                                                /* try { // try from
  //                                                00101a34 to 00101a38 has
  //                                                its CatchHandler @
  //                                                001021ea */
  //       std::__cxx11::basic_string<>::~basic_string(local_ea8);
  //                                                /* try { // try from
  //                                                00101a43 to 00101a47 has
  //                                                its CatchHandler @
  //                                                00102212 */
  //       std::__cxx11::basic_string<>::~basic_string(local_e48);
  //                                                /* try { // try from
  //                                                00101a52 to 00101a56 has
  //                                                its CatchHandler @
  //                                                0010223a */
  //       std::__cxx11::basic_string<>::~basic_string(local_de8);
  //                                                /* try { // try from
  //                                                00101a61 to 00101a65 has
  //                                                its CatchHandler @
  //                                                00102262 */
  //       std::__cxx11::basic_string<>::~basic_string(local_d88);
  //                                                /* try { // try from
  //                                                00101a70 to 00101a74 has
  //                                                its CatchHandler @
  //                                                0010228a */
  //       std::__cxx11::basic_string<>::~basic_string(local_ba8);
  //                                                /* try { // try from
  //                                                00101a7f to 00101a83 has
  //                                                its CatchHandler @
  //                                                001022b2 */
  //       std::__cxx11::basic_string<>::~basic_string((basic_string<>
  //       *)local_678);
  //                                                /* try { // try from
  //                                                00101a9d to 00101aa1 has
  //                                                its CatchHandler @
  //                                                001022da */
  //       PoDoFo::PdfString::PdfString((PdfString
  //       *)local_678,local_f68,(PdfEncoding *)0x0);
  //                                                /* try { // try from
  //                                                00101aee to 00101af2 has
  //                                                its CatchHandler @
  //                                                001022c6 */
  //       PoDoFo::PdfPainter::DrawText
  //                                 (5.0,(height + 0.0) - (double)(local_1154
  //                                 + 10),(PdfString *)local_b18);
  //                                                /* try { // try from
  //                                                00101afd to 00101b01 has
  //                                                its CatchHandler @
  //                                                001022da */
  //       PoDoFo::PdfString::~PdfString((PdfString *)local_678);
  //                                                /* try { // try from
  //                                                00101b0c to 00101b10 has
  //                                                its CatchHandler @
  //                                                001022ee */
  //       std::__cxx11::basic_string<>::~basic_string((basic_string<>
  //       *)local_f68);
  //                                                /* try { // try from
  //                                                00101b1b to 00101b1f has
  //                                                its CatchHandler @
  //                                                00102302 */
  //       std::__cxx11::basic_string<>::~basic_string(local_f88);
  //                                                /* try { // try from
  //                                                00101b2a to 00101b2e has
  //                                                its CatchHandler @
  //                                                00102316 */
  //       std::__cxx11::basic_string<>::~basic_string(local_fa8);
  //                                                /* try { // try from
  //                                                00101b39 to 00101b3d has
  //                                                its CatchHandler @
  //                                                0010232a */
  //       std::__cxx11::basic_string<>::~basic_string(local_fc8);
  //                                                /* try { // try from
  //                                                00101b48 to 00101b86 has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //       std::__cxx11::basic_string<>::~basic_string((basic_string<>
  //       *)local_fe8);
  //  }
  //  PoDoFo::PdfPainter::FinishPage();
  //  PoDoFo::PdfXObject::PdfXObject
  //                           ((PdfXObject
  //                           *)local_c38,rect,*(PdfDocument **)(this +
  //                           0xb0),"#FRM");
  //                                                /* try { // try from
  //                                                00101b91 to 00101b95 has
  //                                                its CatchHandler @
  //                                                001023e1 */
  //  PoDoFo::PdfPainter::PdfPainter((PdfPainter *)local_8c8);
  //                                                /* try { // try from
  //                                                00101bae to 00101c5c has
  //                                                its CatchHandler @
  //                                                001023cd */
  //  PoDoFo::PdfPainter::SetPage((PdfCanvas *)local_8c8);
  //  PoDoFo::PdfPainter::DrawXObject(0.0,0.0,(PdfXObject
  //  *)local_8c8,1.0,1.0);
  //  PoDoFo::PdfPainter::DrawXObject(0.0,0.0,(PdfXObject
  //  *)local_8c8,1.0,1.0); PoDoFo::PdfPainter::FinishPage();
  //  PoDoFo::PdfXObject::PdfXObject
  //                           ((PdfXObject
  //                           *)local_ba8,rect,*(PdfDocument **)(this +
  //                           0xb0),(char *)0x0);
  //                                                /* try { // try from
  //                                                00101c67 to 00101c6b has
  //                                                its CatchHandler @
  //                                                001023b9 */
  //  PoDoFo::PdfPainter::PdfPainter((PdfPainter *)local_678);
  //                                                /* try { // try from
  //                                                00101c84 to 00101cdf has
  //                                                its CatchHandler @
  //                                                001023a5 */
  //  PoDoFo::PdfPainter::SetPage((PdfCanvas *)local_678);
  //  PoDoFo::PdfPainter::DrawXObject(0.0,0.0,(PdfXObject
  //  *)local_678,1.0,1.0); PoDoFo::PdfPainter::FinishPage();
  //  PoDoFo::PdfDictionary::PdfDictionary(local_10a8);
  //  PdfElement::GetObject();
  //  pPVar10 = (PdfReference *)PdfObject::Reference();
  //                                                /* try { // try from
  //                                                00101d07 to 00101d0b has
  //                                                its CatchHandler @
  //                                                00102391 */
  //  PoDoFo::PdfObject::PdfObject((PdfObject *)local_1058,pPVar10);
  //                                                /* try { // try from
  //                                                00101d1d to 00101d21 has
  //                                                its CatchHandler @
  //                                                00102355 */
  //  PdfName::PdfName((PdfName *)local_d88,"N");
  //                                                /* try { // try from
  //                                                00101d3d to 00101d41 has
  //                                                its CatchHandler @
  //                                                00102341 */
  //  PoDoFo::PdfDictionary::AddKey((PdfName *)local_10a8,(PdfObject
  //  *)local_d88);
  //                                                /* try { // try from
  //                                                00101d4c to 00101d50 has
  //                                                its CatchHandler @
  //                                                00102355 */
  //  PoDoFo::PdfName::~PdfName((PdfName *)local_d88);
  //                                                /* try { // try from
  //                                                00101d5b to 00101d92 has
  //                                                its CatchHandler @
  //                                                00102391 */
  //  PoDoFo::PdfObject::~PdfObject((PdfObject *)local_1058);
  //  PdfField::GetFieldObject();
  //  pPVar5 = (PdfName *)PdfVariant::GetDictionary();
  //  PoDoFo::PdfObject::PdfObject((PdfObject *)local_1058,local_10a8);
  //                                                /* try { // try from
  //                                                00101da4 to 00101da8 has
  //                                                its CatchHandler @
  //                                                0010237d */
  //  PdfName::PdfName((PdfName *)local_d88,"AP");
  //                                                /* try { // try from
  //                                                00101dbd to 00101dc1 has
  //                                                its CatchHandler @
  //                                                00102369 */
  //  PoDoFo::PdfDictionary::AddKey(pPVar5,(PdfObject *)local_d88);
  //                                                /* try { // try from
  //                                                00101dcc to 00101dd0 has
  //                                                its CatchHandler @
  //                                                0010237d */
  //  PoDoFo::PdfName::~PdfName((PdfName *)local_d88);
  //                                                /* try { // try from
  //                                                00101ddb to 00101ddf has
  //                                                its CatchHandler @
  //                                                00102391 */
  //  PoDoFo::PdfObject::~PdfObject((PdfObject *)local_1058);
  //                                                /* try { // try from
  //                                                00101dea to 00101dee has
  //                                                its CatchHandler @
  //                                                001023a5 */
  //  PoDoFo::PdfDictionary::~PdfDictionary(local_10a8);
  //                                                /* try { // try from
  //                                                00101df9 to 00101dfd has
  //                                                its CatchHandler @
  //                                                001023b9 */
  //  PoDoFo::PdfPainter::~PdfPainter((PdfPainter *)local_678);
  //                                                /* try { // try from
  //                                                00101e08 to 00101e0c has
  //                                                its CatchHandler @
  //                                                001023cd */
  //  PdfXObject::~PdfXObject((PdfXObject *)local_ba8);
  //                                                /* try { // try from
  //                                                00101e17 to 00101e1b has
  //                                                its CatchHandler @
  //                                                001023e1 */
  //  PoDoFo::PdfPainter::~PdfPainter((PdfPainter *)local_8c8);
  //                                                /* try { // try from
  //                                                00101e26 to 00101e2a has
  //                                                its CatchHandler @
  //                                                001023f5 */
  //  PdfXObject::~PdfXObject((PdfXObject *)local_c38);
  //                                                /* try { // try from
  //                                                00101e35 to 00101e39 has
  //                                                its CatchHandler @
  //                                                00102409 */
  //  PoDoFo::PdfPainter::~PdfPainter(local_b18);
  //                                                /* try { // try from
  //                                                00101e44 to 00101e48 has
  //                                                its CatchHandler @
  //                                                0010241d */
  //  PdfXObject::~PdfXObject(local_cc8);
  //                                                /* try { // try from
  //                                                00101e53 to 00101e57 has
  //                                                its CatchHandler @
  //                                                00102431 */
  //  std::__cxx11::basic_string<>::~basic_string(local_1008);
  //  PdfXObject::~PdfXObject(local_d58);
  //  if (local_20 != *(long *)(in_FS_OFFSET + 0x28)) {
  //                                                /* WARNING: Subroutine
  //                                                does not return */
  //       __stack_chk_fail();
  //  }
  //  return;
}

void PdfSignatureGenerator::SetGraphometricData(
    PdfSignatureField* m_pSignatureField, const PdfString& rsGraphometricData,
    const PdfString& rsVersion) {
  if (!m_pSignatureField) {
    PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);
  }

  if (m_pSignatureField->GetSignatureObject()->GetDictionary().HasKey(
          PdfName("Aruba_Sign_Biometric_Data"))) {
    m_pSignatureField->GetSignatureObject()->GetDictionary().RemoveKey(
        PdfName("Aruba_Sign_Biometric_Data"));
  }
  if (m_pSignatureField->GetSignatureObject()->GetDictionary().HasKey(
          PdfName("Version"))) {
    m_pSignatureField->GetSignatureObject()->GetDictionary().RemoveKey(
        PdfName("Version"));
  }

  m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
      PdfName("Aruba_Sign_Biometric_Data"), rsGraphometricData);
  m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
      PdfName("Version"), rsVersion);
}

void PdfSignatureGenerator::SetSignatureName(
    PdfSignatureField* m_pSignatureField, const PdfString& rsText) {
  if (!m_pSignatureField) {
    PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);
  }
  if (m_pSignatureField->GetSignatureObject()->GetDictionary().HasKey(
          PdfName("Name"))) {
    m_pSignatureField->GetSignatureObject()->GetDictionary().RemoveKey(
        PdfName("Name"));
  }
  m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
      PdfName("Name"), rsText);
}

void PdfSignatureGenerator::SetSignatureSize(
    PdfSignatureField* m_pSignatureField, const size_t size) {
  // Prepare source data
  const PdfData& sSignatureData =
      m_pSignatureField->GetSignatureObject()->GetRawData();
  char* pData = static_cast<char*>(podofo_malloc(size + 2));
  if (!pData) {
    PODOFO_RAISE_ERROR(ePdfError_OutOfMemory);
  }

  pData[0] = '<';
  pData[size + 1] = '>';
  memcpy(pData + 1, sSignatureData.data().c_str(), size);
  PdfData signatureData(pData, size + 2);
  podofo_free(pData);
  // Content of the signature
  if (!m_pSignatureField) {
    PODOFO_RAISE_ERROR(ePdfError_InvalidHandle);
  }
  // Remove old data
  if (m_pSignatureField->GetSignatureObject()->GetDictionary().HasKey(
          "ByteRange")) {
    m_pSignatureField->GetSignatureObject()->GetDictionary().RemoveKey(
        "ByteRange");
  }
  if (m_pSignatureField->GetSignatureObject()->GetDictionary().HasKey(
          PdfName::KeyContents)) {
    m_pSignatureField->GetSignatureObject()->GetDictionary().RemoveKey(
        PdfName::KeyContents);
  }

  // Byte range
  PdfData rangeData("[ 0 1234567890 1234567890 1234567890]");
  m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
      "ByteRange", PdfVariant(rangeData));

  m_pSignatureField->GetSignatureObject()->GetDictionary().AddKey(
      PdfName::KeyContents, PdfVariant(signatureData));
}

// void PdfSignatureGenerator::SetSignatureSize(
//     PdfSignatureField* m_pSignatureField, const size_t size) {
//   char cVar1;
//   char* __ptr;
//   size_t __n;
//   void* __src;
//   long lVar2;
//   PdfError* this_00;
//   PdfName* pPVar3;
//   long in_FS_OFFSET;
//   PdfVariant local_148[32];
//   undefined4 local_128[20];
//   basic_string<> local_d8[32];
//   PdfData local_b8[48];
//   PdfData local_88[48];
//   PdfName local_58[56];
//   long local_20;

//   local_20 = *(long*)(in_FS_OFFSET + 0x28);
//   __ptr = (char*)malloc((size + 1) * 2);
//   *__ptr = '<';
//   __ptr[SIGNATURE_SIZE * 2 + 1] = '>';
//   std::allocator<char>::allocator();
//   /* try { // try from 0010371c to 00103720 has its CatchHandler @ 00103a61
//   */ std::__cxx11::basic_string<>::basic_string(
//       (ulong)local_d8, (char)size * '\x02', (allocator*)0x30);
//   std::allocator<char>::~allocator((allocator<char>*)local_128);
//   /* try { // try from 0010373a to 00103798 has its CatchHandler @ 00103b3c
//   */
//   __n = std::__cxx11::basic_string<>::size();
//   __src = (void*)std::__cxx11::basic_string<>::c_str();
//   memcpy(__ptr + 1, __src, __n);
//   lVar2 = std::__cxx11::basic_string<>::size();
//   PdfData::PdfData(local_b8, __ptr, lVar2 + 2);
//   free(__ptr);
//   if (*(long*)(this + 0x20) == 0) {
//     this_00 = (PdfError*)__cxa_allocate_exception(0x60);
//     local_128[0] = 2;
//     /* try { // try from 001037ee to 001037f2 has its CatchHandler @
//     00103a7e
//     */ PoDoFo::PdfError::PdfError(
//         this_00, (EPdfError*)local_128,
//         "/home/piero/Desktop/IPZS/podofo-0.9.1/src/doc/PdfSignatureField.cpp",
//         0x186, (char*)0x0);
//     /* WARNING: Subroutine does not return */
//     /* try { // try from 0010380a to 00103837 has its CatchHandler @
//     00103b28
//     */
//     __cxa_throw(this_00, &PdfError::typeinfo, PoDoFo::PdfError::~PdfError);
//   }
//   pPVar3 = (PdfName*)PdfVariant::GetDictionary();
//   PdfName::PdfName(local_58, "ByteRange");
//   /* try { // try from 00103842 to 00103846 has its CatchHandler @ 00103a91
//   */ cVar1 = PoDoFo::PdfDictionary::HasKey(pPVar3);
//   /* try { // try from 00103850 to 00103881 has its CatchHandler @ 00103b28
//   */ PoDoFo::PdfName::~PdfName(local_58); if (cVar1 != '\0') {
//     pPVar3 = (PdfName*)PdfVariant::GetDictionary();
//     PdfName::PdfName(local_58, "ByteRange");
//     /* try { // try from 0010388c to 00103890 has its CatchHandler @
//     00103aa5
//     */ PoDoFo::PdfDictionary::RemoveKey(pPVar3);
//     /* try { // try from 00103898 to 00103903 has its CatchHandler @
//     00103b28
//     */ PoDoFo::PdfName::~PdfName(local_58);
//   }
//   pPVar3 = (PdfName*)PdfVariant::GetDictionary();
//   cVar1 = PoDoFo::PdfDictionary::HasKey(pPVar3);
//   if (cVar1 != '\0') {
//     pPVar3 = (PdfName*)PdfVariant::GetDictionary();
//     PoDoFo::PdfDictionary::RemoveKey(pPVar3);
//   }
//   PdfData::PdfData(local_88, "[ 0 1234567890 1234567890 1234567890]");
//   /* try { // try from 00103912 to 0010392f has its CatchHandler @ 00103b17
//   */ pPVar3 = (PdfName*)PdfVariant::GetDictionary();
//   PoDoFo::PdfVariant::PdfVariant(local_148, local_88);
//   /* try { // try from 00103944 to 00103948 has its CatchHandler @ 00103adb
//   */ PoDoFo::PdfObject::PdfObject((PdfObject*)local_128, local_148);
//   /* try { // try from 00103957 to 0010395b has its CatchHandler @ 00103ac7
//   */ PdfName::PdfName(local_58, "ByteRange");
//   /* try { // try from 0010396d to 00103971 has its CatchHandler @ 00103ab6
//   */ PoDoFo::PdfDictionary::AddKey(pPVar3, (PdfObject*)local_58);
//   /* try { // try from 00103979 to 0010397d has its CatchHandler @ 00103ac7
//   */ PoDoFo::PdfName::~PdfName(local_58);
//   /* try { // try from 00103988 to 0010398c has its CatchHandler @ 00103adb
//   */ PoDoFo::PdfObject::~PdfObject((PdfObject*)local_128);
//   /* try { // try from 00103997 to 001039ca has its CatchHandler @ 00103b17
//   */ PoDoFo::PdfVariant::~PdfVariant(local_148); pPVar3 =
//   (PdfName*)PdfVariant::GetDictionary();
//   PoDoFo::PdfVariant::PdfVariant(local_148, local_b8);
//   /* try { // try from 001039df to 001039e3 has its CatchHandler @ 00103b03
//   */ PoDoFo::PdfObject::PdfObject((PdfObject*)local_128, local_148);
//   /* try { // try from 001039fb to 001039ff has its CatchHandler @ 00103aef
//   */ PoDoFo::PdfDictionary::AddKey(pPVar3,
//   (PdfObject*)&PdfName::KeyContents);
//   /* try { // try from 00103a0a to 00103a0e has its CatchHandler @ 00103b03
//   */ PoDoFo::PdfObject::~PdfObject((PdfObject*)local_128);
//   /* try { // try from 00103a19 to 00103a1d has its CatchHandler @ 00103b17
//   */ PoDoFo::PdfVariant::~PdfVariant(local_148);
//   /* try { // try from 00103a25 to 00103a29 has its CatchHandler @ 00103b28
//   */ PdfData::~PdfData(local_88);
//   /* try { // try from 00103a34 to 00103a38 has its CatchHandler @ 00103b3c
//   */ PdfData::~PdfData(local_b8);
//   std::__cxx11::basic_string<>::~basic_string(local_d8);
//   if (local_20 != *(long*)(in_FS_OFFSET + 0x28)) {
//     /* WARNING: Subroutine does not return */
//     __stack_chk_fail();
//   }
//   return;
// }
