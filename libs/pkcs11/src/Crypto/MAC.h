#pragma once
#include <openssl/des.h>

#include "Util/UtilException.h"
#include "Util/util.h"

class CMAC {
  DES_key_schedule k1, k2, k3;
  DES_cblock initVec;

 public:
  CMAC();
  CMAC(const ByteArray &key, const ByteArray &iv);
  ~CMAC(void);

  void Init(const ByteArray &key, const ByteArray &iv);
  ByteDynArray Mac(const ByteArray &data);
};
