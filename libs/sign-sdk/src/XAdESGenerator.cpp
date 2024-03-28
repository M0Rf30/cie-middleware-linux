#include "XAdESGenerator.h"

#include <time.h>

#include <map>

#include "ASN1/DigestInfo.h"
#include "Base64.h"
#include "BigIntegerLibrary.h"
#include "RSA/sha1.h"
#include "RSA/sha2.h"
#include "Util/UUCByteArray.h"
#include "base64-std.h"

#define DIGEST_METHOD_SHA1 "http://www.w3.org/2000/09/xmldsig#sha1"
#define DIGEST_METHOD_RSASHA256 \
  "http://www.w3.org/2001/04/xmldsig-more#rsa-sha256"
#define DIGEST_METHOD_SHA256 "http://www.w3.org/2001/04/xmlenc#sha256"
#define NAMESPACE_XML_DSIG "http://www.w3.org/2000/09/xmldsig#"
#define NAMESPACE_XADES_111 "http://uri.etsi.org/01903/v1.1.1#"
#define NAMESPACE_XADES_132 "http://uri.etsi.org/01903/v1.3.2#"
#define NAMESPACE_XADES_1410 "http://uri.etsi.org/01903/v1.4.1"
#define NAMESPACE_XADES "http://uri.etsi.org/01903#"

#define REFTYPE_CONTENT 1
#define REFTYPE_URI 2

typedef unsigned long uint32;
char* Bin128ToDec(const uint32 N[4]);

void printBigInt(const BYTE* buffer, int buflen, string& sDecimalValue);

CXAdESGenerator::CXAdESGenerator(CBaseSigner* pCryptoki)
    : CSignatureGeneratorBase(pCryptoki), m_bXAdES(false) {}

CXAdESGenerator::CXAdESGenerator(CSignatureGeneratorBase* pGenerator)
    : CSignatureGeneratorBase(pGenerator), m_bXAdES(false) {}

CXAdESGenerator::~CXAdESGenerator(void) {}

void CXAdESGenerator::SetXAdES(bool xades) { m_bXAdES = xades; }

void CXAdESGenerator::SetFileName(char* szFileName) {
  strcpy(m_szFileName, szFileName);
}

static void extractNs(xmlDocPtr pDoc, xmlNode* a_node,
                      map<const xmlChar*, xmlNsPtr*>* pNsPtrMap) {
  xmlNode* cur_node = NULL;

  for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
    if (cur_node->type == XML_ELEMENT_NODE) {
      xmlNsPtr* nsPtr = xmlGetNsList(pDoc, cur_node);
      if (nsPtr) {
        pNsPtrMap->insert(
            pair<const xmlChar*, xmlNsPtr*>(nsPtr[0]->href, nsPtr));
      }
    }

    extractNs(pDoc, cur_node->children, pNsPtrMap);
  }
}

long CXAdESGenerator::Generate(UUCByteArray& xadesData, BOOL bDetached,
                               BOOL bVerifyCertificate) {
  // get the certificate based on alias
  UUCByteArray id;
  CCertificate* pSignerCertificate;
  m_pSigner->GetCertificate(m_szAlias, &pSignerCertificate, id);

  if (bVerifyCertificate) {
    if (!pSignerCertificate->isValid()) {
      SAFEDELETE(pSignerCertificate);
      m_pSigner->Close();
      return DISIGON_ERROR_CERT_EXPIRED;
    }

    int bitmask = pSignerCertificate->verify();

    if ((bitmask & VERIFIED_CACERT_FOUND) == 0) {
      SAFEDELETE(pSignerCertificate);
      m_pSigner->Close();
      return DISIGON_ERROR_CACERT_NOTFOUND;
    }

    if ((bitmask & VERIFIED_CERT_CHAIN) == 0) {
      SAFEDELETE(pSignerCertificate);
      m_pSigner->Close();
      return DISIGON_ERROR_CERT_INVALID;
    }

    if (pSignerCertificate->verifyStatus(NULL) != REVOCATION_STATUS_GOOD) {
      SAFEDELETE(pSignerCertificate);
      m_pSigner->Close();
      return DISIGON_ERROR_CERT_REVOKED;
    }
  }

  xmlDocPtr pDoc;
  xmlNodePtr root;

  // Build an XML tree from the file;
  pDoc = xmlParseMemory((char*)m_data.getContent(), m_data.getLength());

  if (pDoc == NULL) return DISIGON_ERROR_INVALID_FILE;

  // Check the document is of the right kind

  root = xmlDocGetRootElement(pDoc);
  if (root == NULL) {
    // fprintf(stderr,"empty document\n");
    xmlFreeDoc(pDoc);
    return DISIGON_ERROR_INVALID_FILE;
  }

  map<const xmlChar*, xmlNsPtr*> nsPtrMap;

  extractNs(pDoc, root, &nsPtrMap);

  // generate ID
  time_t t = time(NULL);
  sprintf(m_szID, "signature_%ld", t);

  // hash del documento
  // string strDocHashB64;
  // string strDocCanonicalForm;

  // CanonicalizeAndHashBase64(pDoc, strDocHashB64, strDocCanonicalForm);

  // QualifyingProperties
  xmlDocPtr docQualifyingProperties = NULL;
  string strQualifPropsHashB64;
  string strCanonicalForm;

  if (m_bXAdES) {
    docQualifyingProperties =
        CreateQualifyingProperties(pDoc, pSignerCertificate);

    xmlDocPtr doc0 = xmlCopyDoc(docQualifyingProperties, TRUE);
    xmlDocPtr doc1 = xmlNewDoc((const xmlChar*)"1.0");
    xmlNodePtr rootNode = &(doc0->children[0].children[0]);

    xmlDocSetRootElement(doc1, rootNode);

    std::map<const xmlChar*, xmlNsPtr*>::iterator it;

    for (it = nsPtrMap.begin(); it != nsPtrMap.end(); ++it) {
      xmlNsPtr* nsPtr = it->second;
      while (nsPtr) {
        nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
      }
    }

    CanonicalizeAndHashBase64(doc1, strQualifPropsHashB64, strCanonicalForm);

    xmlFreeDoc(doc0);
    xmlFreeDoc(doc1);
  }

  // SignedInfo
  xmlDocPtr docSignedInfo =
      CreateSignedInfo(pDoc, strQualifPropsHashB64, bDetached, m_szFileName);

  std::map<const xmlChar*, xmlNsPtr*>::iterator it;
  for (it = nsPtrMap.begin(); it != nsPtrMap.end(); ++it) {
    xmlNsPtr* nsPtr = it->second;
    while (nsPtr) {
      nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
    }
  }

  string strSignedInfoHashB64;
  string strSigInfoCanonicalForm;
  xmlDocPtr doc0 = xmlCopyDoc(docSignedInfo, TRUE);

  xmlDocPtr doc1 = xmlNewDoc((const xmlChar*)"1.0");
  xmlNodePtr rootNode = &(doc0->children[0].children[0]);

  xmlDocSetRootElement(doc1, rootNode);

  for (it = nsPtrMap.begin(); it != nsPtrMap.end(); ++it) {
    xmlNsPtr* nsPtr = it->second;
    while (nsPtr) {
      nsPtr = nsPtr[0]->next ? &(nsPtr[0]->next) : NULL;
    }
  }

  CanonicalizeAndHashBase64(doc1, strSignedInfoHashB64,
                            strSigInfoCanonicalForm);

  // compute signature
  UUCByteArray signature;
  UUCByteArray data((BYTE*)strSigInfoCanonicalForm.c_str(),
                    strSigInfoCanonicalForm.length());

  UUCByteArray hashaux;

  BYTE hash[50];
  sha2(data.getContent(), data.getLength(), hash, 0);
  hashaux.append(hash, 32);

  CAlgorithmIdentifier hashOID(szSHA256OID);
  UUCByteArray digest;

  CASN1OctetString digestString(hashaux.getContent(), hashaux.getLength());
  CDigestInfo digestInfo(hashOID, digestString);

  digestInfo.toByteArray(digest);

  // make signature on the digest info
  CK_RV rv = m_pSigner->Sign(digest, id, CKM_RSA_PKCS, signature);
  if (rv) return rv;

  // static xmlChar nl[] = "\n";

  // base64
  const string in2((char*)signature.getContent(), signature.getLength());
  string out2;
  Base64::Encode(in2, &out2);

  string strSignatureB64 = out2.c_str();

  xmlNodePtr pSignatureRoot;

  pSignatureRoot = xmlDocGetRootElement(docSignedInfo);

  // KeyInfo
  xmlNewChild(pSignatureRoot, NULL, BAD_CAST "ds:KeyInfo", NULL);

  // certificate in B64
  UUCByteArray baCert;
  pSignerCertificate->toByteArray(baCert);

  const string in1((char*)baCert.getContent(), baCert.getLength());
  string out1;
  Base64::Encode(in1, &out1);

  string strCertB64 = out1.c_str();

  if (docQualifyingProperties) {
    // Object
    xmlNodePtr pObject =
        xmlNewChild(pSignatureRoot, NULL, BAD_CAST "ds:Object", NULL);

    // QualifyingProperties
    xmlNodePtr rootQualifyingProperties =
        xmlDocGetRootElement(docQualifyingProperties);
    xmlAddChild(pObject, rootQualifyingProperties);
  }

  xmlChar* membuf;
  int nSize;

  if (bDetached) {
    xmlDocPtr newdoc = xmlNewDoc((const xmlChar*)"1.0");
    xmlDocSetRootElement(newdoc, pSignatureRoot);
    xmlKeepBlanksDefault(0);
    xmlDocDumpFormatMemory(newdoc, &membuf, &nSize, 1);
  } else {
    // signature
    xmlAddChild(root, pSignatureRoot);
    xmlKeepBlanksDefault(0);
    xmlDocDumpFormatMemory(pDoc, &membuf, &nSize, 1);
  }

  xadesData.append(membuf, nSize);

  xmlFree(membuf);
  xmlFreeDoc(pDoc);

  return CKR_OK;
}

void CXAdESGenerator::CanonicalizeAndHashBase64(xmlDocPtr pDoc,
                                                string& strDocHashB64,
                                                string& strCanonical) {
  // hash del documento
  xmlChar* pCanonicalDoc = NULL;
  int docLen =
      xmlC14NDocDumpMemory(pDoc, NULL, XML_C14N_1_0, NULL, 0, &pCanonicalDoc);
  if (docLen > 0) pCanonicalDoc[docLen] = 0;

  printf("\n%s\n", pCanonicalDoc);

  strCanonical.append((char*)pCanonicalDoc);

  UUCByteArray hashaux;
  if (m_bXAdES) {
    BYTE hash[50];
    sha2(pCanonicalDoc, docLen, hash, 0);
    hashaux.append(hash, 32);
  } else {
    // calcola l'hash SHA1
    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha, pCanonicalDoc, docLen);
    SHA1Result(&sha);

    char szAux[100];
    sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0],
            sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3],
            sha.Message_Digest[4]);

    hashaux.load(szAux);
  }

  const string in((char*)hashaux.getContent(), hashaux.getLength());
  string out;
  Base64::Encode(in, &out);

  strDocHashB64 = out.c_str();
}

xmlDocPtr CXAdESGenerator::CreateSignedInfo(
    xmlDocPtr pDocument, string& strQualifyingPropertiesB64Hash, bool bDetached,
    char* szFileName) {
  // XML doc
  xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
  doc->children = xmlNewDocNode(doc, NULL, BAD_CAST "ds:Signature", NULL);

  xmlNodePtr pSignatureNode = doc->children;
  xmlNewProp(pSignatureNode, BAD_CAST "Id", BAD_CAST m_szID);

  // <SignedInfo>
  xmlNodePtr pSignedInfo =
      xmlNewChild(pSignatureNode, NULL, (const xmlChar*)"ds:SignedInfo", NULL);

  // in ver 1.1 we need this namespace def
  // xmlNewNs(pSignedInfo, (const xmlChar*)NAMESPACE_XML_DSIG, NULL);

  // <CanonicalizationMethod>
  xmlNodePtr pN1 = xmlNewChild(
      pSignedInfo, NULL, (const xmlChar*)"ds:CanonicalizationMethod", NULL);
  xmlSetProp(pN1, (const xmlChar*)"Algorithm",
             (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");

  // <SignatureMethod>
  pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:SignatureMethod",
                    NULL);
  if (m_bXAdES) {
    xmlSetProp(
        pN1, (const xmlChar*)"Algorithm",
        (const xmlChar*)"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256");
  } else {
    xmlSetProp(pN1, (const xmlChar*)"Algorithm",
               (const xmlChar*)"http://www.w3.org/2000/09/xmldsig#rsa-sha1");
  }

  // <Reference>
  pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:Reference", NULL);
  if (bDetached) {
    xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)szFileName);
  } else {
    xmlSetProp(pN1, (const xmlChar*)"URI", (const xmlChar*)"");
  }

  if (!bDetached) {
    // Transformation
    xmlNodePtr pN2 =
        xmlNewChild(pN1, NULL, (const xmlChar*)"ds:Transforms", NULL);
    xmlNodePtr pN3 =
        xmlNewChild(pN2, NULL, (const xmlChar*)"ds:Transform", NULL);

    // filter xpath
    xmlSetProp(pN3, (const xmlChar*)"Algorithm",
               (const xmlChar*)"http://www.w3.org/2002/06/xmldsig-filter2");

    xmlNodePtr pN31 = xmlNewChild(pN3, NULL, (const xmlChar*)"dsig-xpath:XPath",
                                  (const xmlChar*)"/descendant::ds:Signature");
    xmlSetProp(pN31, (const xmlChar*)"Filter", (const xmlChar*)"subtract");

    // c14N
    xmlNodePtr pN4 =
        xmlNewChild(pN2, NULL, (const xmlChar*)"ds:Transform", NULL);
    xmlSetProp(
        pN4, (const xmlChar*)"Algorithm",
        (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");
  }

  // documents digest
  xmlNodePtr pN2 =
      xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestMethod", NULL);
  xmlSetProp(pN2, (const xmlChar*)"Algorithm",
             (const xmlChar*)"http://www.w3.org/2001/04/xmlenc#sha256");

  // digest value
  // hash del documento
  string strDocHashB64;
  string strCanonicalDoc;

  CanonicalizeAndHashBase64(pDocument, strDocHashB64, strCanonicalDoc);

  pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestValue",
                    (const xmlChar*)strDocHashB64.c_str());

  if (!strQualifyingPropertiesB64Hash.empty()) {
    // XAdES xadesSignedProperties
    // Type="http://www.w3.org/2000/09/xmldsig#SignatureProperties
    // <Reference>
    pN1 = xmlNewChild(pSignedInfo, NULL, (const xmlChar*)"ds:Reference", NULL);
    xmlSetProp(pN1, (const xmlChar*)"URI",
               (const xmlChar*)"#xadesSignedProperties");
    xmlSetProp(pN1, (const xmlChar*)"Type",
               (const xmlChar*)"http://uri.etsi.org/01903#SignedProperties");

    // Transformation
    xmlNodePtr pN02 =
        xmlNewChild(pN1, NULL, (const xmlChar*)"ds:Transforms", NULL);
    // c14N
    xmlNodePtr pN04 =
        xmlNewChild(pN02, NULL, (const xmlChar*)"ds:Transform", NULL);
    xmlSetProp(
        pN04, (const xmlChar*)"Algorithm",
        (const xmlChar*)"http://www.w3.org/TR/2001/REC-xml-c14n-20010315");

    // documents digest
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestMethod", NULL);
    xmlSetProp(pN2, (const xmlChar*)"Algorithm",
               (const xmlChar*)"http://www.w3.org/2001/04/xmlenc#sha256");
    pN2 = xmlNewChild(pN1, NULL, (const xmlChar*)"ds:DigestValue",
                      (const xmlChar*)strQualifyingPropertiesB64Hash.c_str());
  }

  return doc;
}

xmlDocPtr CXAdESGenerator::CreateQualifyingProperties(
    xmlDocPtr pDocument, CCertificate* pCertificate) {
  // XML doc
  xmlDocPtr doc = xmlNewDoc((const xmlChar*)"1.0");
  doc->children =
      xmlNewDocNode(doc, NULL, BAD_CAST "xades:QualifyingProperties", NULL);

  // QualifyingProperties
  xmlNodePtr pQualifyingProperties = doc->children;
  xmlNewProp(pQualifyingProperties, BAD_CAST "Target", BAD_CAST m_szID);
  // xmlNsPtr ns = xmlNewNs(pQualifyingProperties, BAD_CAST NAMESPACE_XADES_132,
  // NULL);

  // <SignedProperties>
  xmlNodePtr pSignedProperties =
      xmlNewChild(pQualifyingProperties, NULL,
                  (const xmlChar*)"xades:SignedProperties", NULL);
  xmlNewProp(pSignedProperties, BAD_CAST "Id",
             BAD_CAST "xadesSignedProperties");

  // <SignedSignatureProperties>
  xmlNodePtr pSignedSignatureProperties =
      xmlNewChild(pSignedProperties, NULL,
                  (const xmlChar*)"xades:SignedSignatureProperties", NULL);

  // <SigningTime>
  /* Get UNIX-style time and display as number and string. */
  time_t ltime;
  time(&ltime);
  tm* pCurTime = gmtime(&ltime);  // localtime(&ltime);

  char szTime[100];

  strftime(szTime, 100, "%Y-%m-%dT%H:%M:%SZ", pCurTime);

  // xmlNodePtr pSigningTime = xmlNewChild(pSignedSignatureProperties, NULL,
  // (const xmlChar*)"xades:SigningTime", BAD_CAST szTime);
  // xmlNodeAddContent(pSignedSignatureProperties, nl);

  // SigningCertificate
  xmlNodePtr pSigningCertificate =
      xmlNewChild(pSignedSignatureProperties, NULL,
                  (const xmlChar*)"xades:SigningCertificate", NULL);

  // Cert
  xmlNodePtr pCert = xmlNewChild(pSigningCertificate, NULL,
                                 (const xmlChar*)"xades:Cert", NULL);

  // CertDigest
  xmlNodePtr pCertDigest =
      xmlNewChild(pCert, NULL, (const xmlChar*)"xades:CertDigest", NULL);

  // DigestMethod
  xmlNodePtr pDigestMethod = xmlNewChild(
      pCertDigest, NULL, (const xmlChar*)"ds:DigestMethod",
      NULL);  //(const xmlChar*)"http://www.w3.org/2000/09/xmldsig#sha1");
  // ns = xmlNewNs(pDigestMethod, BAD_CAST NAMESPACE_XML_DSIG, NULL);

  xmlNewProp(pDigestMethod, BAD_CAST "Algorithm",
             BAD_CAST DIGEST_METHOD_SHA256);
  // xmlNodeAddContent(pCertDigest, nl);

  // DigestValue
  // extract the cert value
  BYTE cv[50];

  UUCByteArray certval;
  pCertificate->toByteArray(certval);

  // calcola l'hash SHA1
  sha2(certval.getContent(), certval.getLength(), cv, 0);

  /*
  SHA1Context sha;
  SHA1Reset(&sha);
  SHA1Input(&sha, certval.getContent(), certval.getLength());
  SHA1Result(&sha);

  char szAux[100];
  sprintf(szAux, "%08X%08X%08X%08X%08X ", sha.Message_Digest[0],
  sha.Message_Digest[1], sha.Message_Digest[2], sha.Message_Digest[3],
  sha.Message_Digest[4]);
  */

  UUCByteArray hashaux(cv, 32);

  // base 64
  const string in((char*)hashaux.getContent(), hashaux.getLength());
  string out;
  Base64::Encode(in, &out);

  string strHashB64 = out.c_str();

  // X509IssuerName
  UUCByteArray strIssuerName;
  pCertificate->getIssuer().getNameAsString(strIssuerName);

  // X509SerialNumber
  CASN1Integer serialNumber(pCertificate->getSerialNumber());
  UUCByteArray* pSerialNumber = (UUCByteArray*)serialNumber.getValue();

  BigInteger sernum = dataToBigInteger<const BYTE>(pSerialNumber->getContent(),
                                                   pSerialNumber->getLength(),
                                                   BigInteger::positive);

  string strSerNum = bigIntegerToString(sernum);

  return doc;
}

/* N[0] - contains least significant bits, N[3] - most significant */
char* Bin128ToDec(const uint32 N[4]) {
  // log10(x) = log2(x) / log2(10) ~= log2(x) / 3.322
  static char s[128 / 3 + 1 + 1];
  uint32 n[4];
  char* p = s;
  int i;

  memset(s, '0', sizeof(s) - 1);
  s[sizeof(s) - 1] = '\0';

  memcpy(n, N, sizeof(n));

  for (i = 0; i < 128; i++) {
    int j, carry;

    carry = (n[3] >= 0x80000000);
    // Shift n[] left, doubling it
    n[3] = ((n[3] << 1) & 0xFFFFFFFF) + (n[2] >= 0x80000000);
    n[2] = ((n[2] << 1) & 0xFFFFFFFF) + (n[1] >= 0x80000000);
    n[1] = ((n[1] << 1) & 0xFFFFFFFF) + (n[0] >= 0x80000000);
    n[0] = ((n[0] << 1) & 0xFFFFFFFF);

    // Add s[] to itself in decimal, doubling it
    for (j = sizeof(s) - 2; j >= 0; j--) {
      s[j] += s[j] - '0' + carry;

      carry = (s[j] > '9');

      if (carry) {
        s[j] -= 10;
      }
    }
  }

  while ((p[0] == '0') && (p < &s[sizeof(s) - 2])) {
    p++;
  }

  return p;
}

void printBigInt(const BYTE* buffer, int buflen, string& sDecimalValue) {
  for (int i = 0; i < buflen; i++) {
    unsigned x = buffer[i];
    char buf[(sizeof(x) * CHAR_BIT) / 3 + 2];  // slightly oversize buffer
    char* result = buf + sizeof(buf) - 1;      // index of next output digit

    // add digits to result, starting at
    // the end (least significant digit)

    *result = '\0';  // terminating null
    do {
      *--result = '0' + (x % 10);  // remainder gives the next digit
      x /= 10;
    } while (x);  // keep going until x reaches zero

    sDecimalValue.append(result);
  }
}
