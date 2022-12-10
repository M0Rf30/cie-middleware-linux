#include "Crypto/sha512.h"

#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

void CSHA512::Init() {
  SHA512_Init(&ctx);
  isInit = true;
}
void CSHA512::Update(ByteArray data) {
  if (!isInit) throw logged_error("Hash non inizializzato");
  SHA512_Update(&ctx, data.data(), data.size());
}
ByteDynArray CSHA512::Final() {
  if (!isInit) throw logged_error("Hash non inizializzato");
  ByteDynArray resp(SHA_DIGEST_LENGTH);
  SHA512_Final(resp.data(), &ctx);
  isInit = false;

  return resp;
}

ByteDynArray CSHA512::Digest(ByteArray& data) {
  const BYTE* pbData = (BYTE*)data.data();
  unsigned int nDataLen = data.size();
  BYTE abDigest[CryptoPP::SHA512::DIGESTSIZE];

  CryptoPP::SHA512().CalculateDigest(abDigest, pbData, nDataLen);

  ByteArray resp(abDigest, CryptoPP::SHA512::DIGESTSIZE);

  return resp;
}
