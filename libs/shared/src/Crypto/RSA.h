#pragma once

#include <cryptopp/rsa.h>
#include <openssl/rsa.h>

#include "Sign/definitions.h"
#include "Util/Array.h"

class CRSA {
  CryptoPP::RSA::PublicKey pubKey;
  DWORD GenerateKey(DWORD size, ByteDynArray &module, ByteDynArray &pubexp,
                    ByteDynArray &privexp);

 public:
  CRSA(ByteArray &mod, ByteArray &exp);
  ~CRSA(void);

  ByteDynArray RSA_PURE(ByteArray &data);
  bool RSA_PSS(ByteArray &signatureData, ByteArray &toSign);
};
