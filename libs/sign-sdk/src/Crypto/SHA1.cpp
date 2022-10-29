#include "SHA1.h"

CSHA1::CSHA1() : isInit(false) {}

CSHA1::~CSHA1() {}

void CSHA1::Init() {
  SHA1_Init(&ctx);
  isInit = true;
}
void CSHA1::Update(ByteArray data) {
  if (!isInit) throw logged_error("Hash non inizializzato");
  SHA1_Update(&ctx, data.data(), data.size());
}
ByteDynArray CSHA1::Final() {
  if (!isInit) throw logged_error("Hash non inizializzato");
  ByteDynArray resp(SHA_DIGEST_LENGTH);
  SHA1_Final(resp.data(), &ctx);
  isInit = false;

  return resp;
}

ByteDynArray CSHA1::Digest(ByteArray data) {
  Init();
  Update(data);
  return Final();
}
