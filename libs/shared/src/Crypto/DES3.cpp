#include "Crypto/DES3.h"

#include <cryptopp/misc.h>
#include <openssl/des.h>
#include <openssl/evp.h>

extern CLog Log;

void CDES3::Init(const ByteArray &key, const ByteArray &iv) {
  init_func long KeySize = key.size();
  DES_cblock *keyVal1 = nullptr, *keyVal2 = nullptr, *keyVal3 = nullptr;
  CryptoPP::memcpy_s(initVec, sizeof(DES_cblock), iv.data(), 8);

  switch (KeySize) {
    case 8:
      throw logged_error("Chiave 3DES 8 byte non supportata");
      break;
    case 16:
      keyVal1 = keyVal3 = (DES_cblock *)key.left(8).data();  // data();
      keyVal2 = (DES_cblock *)key.mid(8, 8).data();
      break;
    case 24:
      keyVal1 = (DES_cblock *)key.left(8).data();
      keyVal2 = (DES_cblock *)key.mid(8, 8).data();
      keyVal3 = (DES_cblock *)key.mid(16, 8).data();
      break;
  }

  DES_set_key(keyVal1, &k1);
  DES_set_key(keyVal2, &k2);
  DES_set_key(keyVal3, &k3);
  exit_func
}

CDES3::~CDES3(void) {}

CDES3::CDES3() {}

ByteDynArray CDES3::Des3(const ByteArray &data, int encOp) {
  init_func

      DES_cblock iv;
  CryptoPP::memcpy_s(iv, sizeof(DES_cblock), initVec, sizeof(initVec));
  size_t AppSize = data.size() - 1;
  ByteDynArray resp(AppSize - (AppSize % 8) + 8);
  DES_ede3_cbc_encrypt(data.data(), resp.data(), (long)data.size(), &k1, &k2,
                       &k3, &iv, encOp);

  return resp;
}

CDES3::CDES3(const ByteArray &key, const ByteArray &iv) { Init(key, iv); }

ByteDynArray CDES3::Encode(const ByteArray &data) {
  init_func return Des3(ISOPad(data), DES_ENCRYPT);
}

ByteDynArray CDES3::RawEncode(const ByteArray &data) {
  init_func ByteDynArray result;
  ER_ASSERT((data.size() % 8) == 0,
            "La dimensione dei dati da cifrare deve essere multipla di 8");

  return Des3(data, DES_ENCRYPT);
}

ByteDynArray CDES3::Decode(const ByteArray &data) {
  init_func auto result = Des3(data, DES_DECRYPT);
  result.resize(RemoveISOPad(result), true);
  return result;
}

ByteDynArray CDES3::RawDecode(const ByteArray &data) {
  init_func ByteDynArray result;
  ER_ASSERT((data.size() % 8) == 0,
            "La dimensione dei dati da cifrare deve essere multipla di 8");

  return Des3(data, DES_DECRYPT);
}
