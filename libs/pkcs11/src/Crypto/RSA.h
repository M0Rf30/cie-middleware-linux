#pragma once

#ifdef WIN32
#include <bcrypt.h>
#else
#include <cryptopp/rsa.h>
#include <openssl/rsa.h>
#endif
#include "../PKCS11/wintypes.h"
#include "../Util/Array.h"

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
