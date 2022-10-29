#pragma once

#include <openssl/des.h>

#include "Util/UtilException.h"
#include "Util/util.h"

#define DESKEY_LENGHT 8

class CDES3 {
  ByteDynArray Des3(const ByteArray &data, int encOp);
  DES_key_schedule k1, k2, k3;
  DES_cblock initVec;

 public:
  CDES3();
  CDES3(const ByteArray &key, const ByteArray &iv);
  ~CDES3(void);

  void Init(const ByteArray &key, const ByteArray &iv);
  ByteDynArray Encode(const ByteArray &data);
  ByteDynArray Decode(const ByteArray &data);
  ByteDynArray RawEncode(const ByteArray &data);
  ByteDynArray RawDecode(const ByteArray &data);
};
