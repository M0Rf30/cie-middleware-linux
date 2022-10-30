#include "ContentType.h"

#include "ASN1Exception.h"

const char* CContentType::OID_TYPE_DATA = "1.2.840.113549.1.7.1";
const char* CContentType::OID_TYPE_SIGNED = "1.2.840.113549.1.7.2";
const char* CContentType::OID_TYPE_ENVELOPED = "1.2.840.113549.1.7.3";
const char* CContentType::OID_TYPE_SIGNED_ENVELOPED = "1.2.840.113549.1.7.4";
const char* CContentType::OID_TYPE_DIGEST = "1.2.840.113549.1.7.5";
const char* CContentType::OID_TYPE_ENCRYPTED = "1.2.840.113549.1.7.6";
const char* CContentType::OID_TYPE_TSTINFO = "1.2.840.113549.1.9.16.1.4";

CContentType::CContentType(const CASN1ObjectIdentifier& algoId)
    : CASN1ObjectIdentifier(algoId) {}

CContentType::CContentType(char* lpszOId) : CASN1ObjectIdentifier(lpszOId) {}

CContentType::CContentType(UUCBufferedReader& reader)
    : CASN1ObjectIdentifier(reader) {}

CContentType::CContentType(const CASN1Object& contentType)
    : CASN1ObjectIdentifier(contentType) {}

CContentType::~CContentType() {}
