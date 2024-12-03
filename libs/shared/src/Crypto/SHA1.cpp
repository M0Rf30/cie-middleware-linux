#include "Crypto/SHA1.h"

#include <openssl/evp.h>

CSHA1::CSHA1() : isInit(false) {}

CSHA1::~CSHA1() {}

void CSHA1::Init() {
  ctx = EVP_MD_CTX_new();
  EVP_DigestInit_ex(ctx, EVP_sha1(), NULL);
  isInit = true;
}
void CSHA1::Update(ByteArray data) {
  if (!isInit) throw logged_error("Hash non inizializzato");
  EVP_DigestUpdate(ctx, data.data(), data.size());
}
ByteDynArray CSHA1::Final() {
  if (!isInit) throw logged_error("Hash non inizializzato");
  ByteDynArray resp(SHA_DIGEST_LENGTH);
  EVP_DigestFinal_ex(ctx, resp.data(), NULL);
  isInit = false;

  return resp;
}

ByteDynArray CSHA1::Digest(ByteArray data) {
  Init();
  Update(data);
  return Final();
}
