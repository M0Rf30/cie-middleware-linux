#include "MAC.h"

#include <cryptopp/hmac.h>

extern CLog Log;

void CMAC::Init(const ByteArray &key, const ByteArray &iv) {
  init_func long KeySize = key.size();

  DES_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;
  CryptoPP::memcpy_s(initVec, sizeof(DES_cblock), iv.data(), 8);

  switch (KeySize) {
    case 8:
      throw logged_error("Errore nella cifratura DES");
      break;
    case 16:
      keyVal1 = keyVal3 = (DES_cblock *)key.data();
      keyVal2 = (DES_cblock *)key.mid(8).data();
      break;
    case 24:
      keyVal1 = (DES_cblock *)key.data();
      keyVal2 = (DES_cblock *)key.mid(8).data();
      keyVal3 = (DES_cblock *)key.mid(16).data();
      break;
  }

  DES_set_key(keyVal1, &k1);
  DES_set_key(keyVal2, &k2);
  DES_set_key(keyVal3, &k3);

  exit_func
}

CMAC::~CMAC(void) {}

ByteDynArray CMAC::Mac(const ByteArray &data) {
  init_func

      ByteDynArray resp(8);

  DES_cblock iv;
  CryptoPP::memcpy_s(iv, sizeof(DES_cblock), initVec, sizeof(iv));

  size_t ANSILen = ANSIPadLen(data.size());
  if (data.size() > 8) {
    ByteDynArray baOutTmp(ANSILen - 8);
    DES_ncbc_encrypt(data.data(), baOutTmp.data(), (long)ANSILen - 8, &k1, &iv,
                     DES_ENCRYPT);
  }
  uint8_t dest[8];
  DES_ede3_cbc_encrypt(data.mid(ANSILen - 8).data(), dest,
                       (long)(data.size() - ANSILen) + 8, &k1, &k2, &k3, &iv,
                       DES_ENCRYPT);
  resp.copy(ByteArray(dest, 8));

  return resp;

  exit_func
}

CMAC::CMAC() {}

CMAC::CMAC(const ByteArray &key, const ByteArray &iv) { Init(key, iv); }
