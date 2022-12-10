#include "Crypto/AES.h"

extern CLog Log;

ByteDynArray CAES::AES(const ByteArray &data, int encOp) {
  init_func

      ByteDynArray iv2 = iv;

  AES_KEY aesKey;
  if (encOp == AES_ENCRYPT)
    AES_set_encrypt_key(key.data(), (int)key.size() * 8, &aesKey);
  else
    AES_set_decrypt_key(key.data(), (int)key.size() * 8, &aesKey);
  size_t AppSize = data.size() - 1;
  ByteDynArray resp(AppSize - (AppSize % 16) + 16);
  AES_cbc_encrypt(data.data(), resp.data(), data.size(), &aesKey, iv2.data(),
                  encOp);

  return resp;
}
void CAES::Init(const ByteArray &key, const ByteArray &iv) {
  init_func this->iv = iv;
  this->key = key;

  exit_func
}

CAES::CAES() {}

CAES::~CAES(void) {}

CAES::CAES(const ByteArray &key, const ByteArray &iv) { Init(key, iv); }

ByteDynArray CAES::Encode(const ByteArray &data) {
  init_func return AES(ISOPad16(data), AES_ENCRYPT);
}

ByteDynArray CAES::RawEncode(const ByteArray &data) {
  init_func ER_ASSERT(
      (data.size() % AES_BLOCK_SIZE) == 0,
      "La dimensione dei dati da cifrare deve essere multipla di 16");
  return AES(data, AES_ENCRYPT);
}

ByteDynArray CAES::Decode(const ByteArray &data) {
  init_func ByteDynArray result = AES(data, AES_DECRYPT);
  result.resize(RemoveISOPad(result), true);
  return result;
}

ByteDynArray CAES::RawDecode(const ByteArray &data) {
  init_func ER_ASSERT(
      (data.size() % AES_BLOCK_SIZE) == 0,
      "La dimensione dei dati da cifrare deve essere multipla di 16");
  return AES(data, AES_DECRYPT);
}
