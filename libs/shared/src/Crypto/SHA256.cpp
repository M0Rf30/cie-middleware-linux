#include "Crypto/sha256.h"

#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

void CSHA256::Init() {
  SHA256_Init(&ctx);
  isInit = true;
}
void CSHA256::Update(ByteArray data) {
  if (!isInit) throw logged_error("Hash non inizializzato");
  SHA256_Update(&ctx, data.data(), data.size());
}
ByteDynArray CSHA256::Final() {
  if (!isInit) throw logged_error("Hash non inizializzato");
  ByteDynArray resp(SHA_DIGEST_LENGTH);
  SHA256_Final(resp.data(), &ctx);
  isInit = false;

  return resp;
}
ByteDynArray CSHA256::Digest(ByteArray& data) {
  const BYTE* pbData = (BYTE*)data.data();
  unsigned int nDataLen = data.size();
  BYTE abDigest[CryptoPP::SHA256::DIGESTSIZE];
  CryptoPP::SHA256().CalculateDigest(abDigest, pbData, nDataLen);
  ByteArray resp(abDigest, CryptoPP::SHA256::DIGESTSIZE);

  return resp;
}
