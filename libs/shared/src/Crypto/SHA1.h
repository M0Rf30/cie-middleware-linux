#pragma once
#include <openssl/evp.h>
#include <openssl/sha.h>

#include "Util/UtilException.h"
#include "Util/util.h"

class CSHA1 {
  bool isInit;
  EVP_MD_CTX* ctx;

 public:
  CSHA1();
  ~CSHA1(void);

  ByteDynArray Digest(ByteArray data);

  void Init();
  void Update(ByteArray data);
  ByteDynArray Final();
};
