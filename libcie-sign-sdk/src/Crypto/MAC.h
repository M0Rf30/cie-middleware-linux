#pragma once
#ifdef WIN32
#include <bcrypt.h>

#include "..\stdafx.h"
#else
#include <openssl/des.h>
#endif
#include "../Util/UtilException.h"
#include "../Util/util.h"

class CMAC {
#ifdef WIN32
    BCRYPT_KEY_HANDLE key1;
    BCRYPT_KEY_HANDLE key2;
    ByteDynArray iv;

#else
    DES_key_schedule k1, k2, k3;
    DES_cblock initVec;
#endif
  public:
    CMAC();
    CMAC(const ByteArray &key, const ByteArray &iv);
    ~CMAC(void);

    void Init(const ByteArray &key, const ByteArray &iv);
    ByteDynArray Mac(const ByteArray &data);
};
