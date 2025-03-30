#include "Crypto/sha512.h"

#include <cryptopp/base64.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

void CSHA512::Init() {
    ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha512(), NULL);
    isInit = true;
}
void CSHA512::Update(ByteArray data) {
  if (!isInit) throw logged_error("Hash non inizializzato");
  EVP_DigestUpdate(ctx, data.data(), data.size());
}
ByteDynArray CSHA512::Final() {
  if (!isInit) throw logged_error("Hash non inizializzato");
  ByteDynArray resp(SHA_DIGEST_LENGTH);
  EVP_DigestFinal_ex(ctx, resp.data(), NULL);
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
