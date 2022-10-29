
#include "MD5.h"

CMD5::CMD5() : isInit(false) {}

CMD5::~CMD5() {}

void CMD5::Init() {
  // throw logged_error("Un'operazione di hash � gi� in corso");
  MD5_Init(&ctx);
  isInit = true;
}
void CMD5::Update(ByteArray data) {
  if (!isInit) throw logged_error("Hash non inizializzato");
  MD5_Update(&ctx, data.data(), data.size());
}
ByteDynArray CMD5::Final() {
  if (!isInit) throw logged_error("Hash non inizializzato");
  ByteDynArray resp(MD5_DIGEST_LENGTH);
  MD5_Final(resp.data(), &ctx);
  isInit = false;

  return resp;
}

ByteDynArray CMD5::Digest(ByteArray data) {
  Init();
  Update(data);
  return Final();
}
