// TSTInfo.cpp: implementation of the CTSTInfo class.
//

#include "TSTInfo.h"
#include "ASN1OptionalField.h"
#include "ASN1Exception.h"
#include "ASN1UTCTime.h"

// Construction/Destruction

CTSTInfo::CTSTInfo(UUCBufferedReader& reader)
    :CASN1Sequence(reader) {
}

CTSTInfo::CTSTInfo(const CASN1Object& tstInfo)
    :CASN1Sequence(tstInfo) {
}

CTSTInfo::~CTSTInfo() {
}

CASN1Sequence CTSTInfo::getMessageImprint() {
    return elementAt(2);
}

CASN1UTCTime CTSTInfo::getUTCTime() {
    return elementAt(4);
}

CAlgorithmIdentifier CTSTInfo::getDigestAlgorithn() {
    CASN1Sequence messageImprint(elementAt(2));
    return (CAlgorithmIdentifier)messageImprint.elementAt(0);
}

CASN1Integer CTSTInfo::getSerialNumber() {
    return elementAt(3);
}

CName CTSTInfo::getTSAName() {
    int siz = size();
    for (int i = 1; i < siz; i++) {
        CASN1Object obj = elementAt(i);
        if (obj.getTag() == 0xA0) {
            CASN1Sequence val(obj);
            CASN1Sequence val1(val.elementAt(0));
            return val1.elementAt(0);
        }
    }

    throw -1;
}

