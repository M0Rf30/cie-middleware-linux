#pragma once

#include <openssl/md5.h>

#include "Util/UtilException.h"
#include "Util/util.h"

class CMD5 {
  bool isInit;
  MD5_CTX ctx;

 public:
  CMD5();
  ~CMD5(void);

  ByteDynArray Digest(ByteArray data);

  void Init();
  void Update(ByteArray data);
  ByteDynArray Final();
};
