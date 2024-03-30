#pragma once

#include <libxml/c14n.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

#include <vector>

#include "SignatureGenerator.h"

/*
typedef struct
{
        int nType;
        union
        {
                UUCByteArray content;
                char* szURI;
        } reference;
} Reference;
*/

class CXAdESGenerator : public CSignatureGeneratorBase {
 public:
  CXAdESGenerator(CBaseSigner* pCryptoki);
  CXAdESGenerator(CSignatureGeneratorBase* pGenerator);

  virtual ~CXAdESGenerator(void);

  void SetXAdES(bool xades);

  void SetFileName(char* szFileName);

  virtual long Generate(UUCByteArray& xadesData, BOOL bDetached,
                        BOOL bVerifyRevocation);
  /*
          void AddContent(UUCByteArray& content);

          void AddReference(char* szURI);
  */
 private:
  bool m_bXAdES;

  xmlDocPtr CreateSignedInfo(xmlDocPtr pDocument,
                             string& strQualifyingPropertiesB64Hash,
                             bool bDetached, char* szFileName);
  xmlDocPtr CreateQualifyingProperties(xmlDocPtr pDocument,
                                       CCertificate* pCertificate);

  void CanonicalizeAndHashBase64(xmlDocPtr pDoc, string& strDocHashB64,
                                 string& strCanonical);

  //	vector<Reference*> m_refecenceVect;

  char m_szID[100];
  char m_szFileName[MAX_PATH];
};
