#pragma once

#include <openssl/sha.h>

#include "Util/Array.h"

#define SHA512_DIGEST_LENGTH 64

class CSHA512 {
  bool isInit;
  SHA512_CTX ctx;
  void Init();
  void Update(ByteArray data);
  ByteDynArray Final();

 public:
  ByteDynArray Digest(ByteArray &data);
};
