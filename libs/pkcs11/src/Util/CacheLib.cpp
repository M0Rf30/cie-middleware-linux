
#include "CacheLib.h"

#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/misc.h>
#include <cryptopp/modes.h>
#include <cryptopp/sha.h>
#include <pwd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "../keys.h"
#include "./util.h"

using namespace CryptoPP;

int decrypt(std::string &ciphertext, std::string &message);

/// Questa implementazione della cache del PIN e del certificato è fornita solo
/// a scopo dimostrativo. Questa versione NON protegge a sufficienza il PIN
/// dell'utente, che potrebbe essere ricavato da un'applicazione malevola. Si
/// raccomanda di utilizzare, in contesti di produzione, un'implementazione che
/// fornisca un elevato livello di sicurezza

bool file_exists(const char *name) {
  struct stat buffer;
  return (stat(name, &buffer) == 0);
}

std::string GetCardDir() {
  char *home = getenv("HOME");
  if (home == NULL) {
    struct passwd *pw = getpwuid(getuid());

    home = pw->pw_dir;
    printf("home: %s", home);
  }

  std::string path(home);

  path.append("/.CIEPKI/");

  printf("Card Dir: %s\n", path.c_str());

  return path.c_str();
}

void GetCardPath(const char *PAN, std::string &sPath) {
  auto Path = GetCardDir();

  Path += std::string(PAN);
  Path += ".cache";
  sPath = Path;
}

bool CacheExists(const char *PAN) {
  std::string sPath;
  GetCardPath(PAN, sPath);
  return file_exists(sPath.c_str());
}

bool CacheRemove(const char *PAN) {
  std::string sPath;
  GetCardPath(PAN, sPath);
  return remove(sPath.c_str());
}

void CacheGetCertificate(const char *PAN, std::vector<uint8_t> &certificate) {
  if (PAN == nullptr) throw logged_error("Il PAN è necessario");

  std::string sPath;
  GetCardPath(PAN, sPath);

  if (file_exists(sPath.c_str())) {
    ByteDynArray data, Cert;
    data.load(sPath.c_str());

    std::string ciphertext((char *)data.data(), data.size());
    std::string plaintext;

    decrypt(ciphertext, plaintext);

    uint8_t *ptr = (uint8_t *)plaintext.c_str();

    uint32_t len = *(uint32_t *)ptr;
    ptr += sizeof(uint32_t);
    // salto il PIN
    ptr += len;
    len = *(uint32_t *)ptr;
    ptr += sizeof(uint32_t);
    Cert.resize(len);
    Cert.copy(ByteArray(ptr, len));

    certificate.resize(Cert.size());
    ByteArray(certificate.data(), certificate.size()).copy(Cert);
  } else {
    throw logged_error("CIE non abilitata");
  }
}

void CacheGetPIN(const char *PAN, std::vector<uint8_t> &PIN) {
  if (PAN == nullptr) throw logged_error("Il PAN è necessario");

  std::string sPath;
  GetCardPath(PAN, sPath);

  if (file_exists(sPath.c_str())) {
    ByteDynArray data, ClearPIN;
    data.load(sPath.c_str());

    std::string ciphertext((char *)data.data(), data.size());
    std::string plaintext;

    decrypt(ciphertext, plaintext);

    uint8_t *ptr = (uint8_t *)plaintext.c_str();
    uint32_t len = *(uint32_t *)ptr;
    ptr += sizeof(uint32_t);
    ClearPIN.resize(len);
    ClearPIN.copy(ByteArray(ptr, len));

    PIN.resize(ClearPIN.size());
    ByteArray(PIN.data(), PIN.size()).copy(ClearPIN);

  } else
    throw logged_error("CIE non abilitata");
}

void CacheSetData(const char *PAN, uint8_t *certificate, int certificateSize,
                  uint8_t *FirstPIN, int FirstPINSize) {
  if (PAN == nullptr) throw logged_error("Il PAN è necessario");

  auto szDir = GetCardDir();

  struct stat st = {0};

  if (stat(szDir.c_str(), &st) == -1) {
    int r = mkdir(szDir.c_str(), 0700);
    printf("mkdir: %d, %x\n", r, errno);
  }

  std::string sPath;
  GetCardPath(PAN, sPath);

  ByteArray baCertificate(certificate, certificateSize);
  ByteArray baFirstPIN(FirstPIN, FirstPINSize);

  uint32_t pinlen = (uint32_t)baFirstPIN.size();
  uint32_t certlen = (uint32_t)baCertificate.size();

  byte key[CryptoPP::AES::DEFAULT_KEYLENGTH], iv[CryptoPP::AES::BLOCKSIZE];
  memset(key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
  memset(iv, 0x00, CryptoPP::AES::BLOCKSIZE);

  std::string ciphertext;
  std::string enckey = ENCRYPTION_KEY;

  byte digest[SHA1::DIGESTSIZE];
  SHA1().CalculateDigest(digest, (byte *)enckey.c_str(), enckey.length());
  memcpy(key, digest, CryptoPP::AES::DEFAULT_KEYLENGTH);
  //
  // Create Cipher Text
  //
  CryptoPP::AES::Encryption aesEncryption(key,
                                          CryptoPP::AES::DEFAULT_KEYLENGTH);
  CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption,
                                                              iv);

  CryptoPP::StreamTransformationFilter stfEncryptor(
      cbcEncryption, new CryptoPP::StringSink(ciphertext));
  stfEncryptor.Put(reinterpret_cast<const unsigned char *>(&pinlen),
                   sizeof(pinlen));
  stfEncryptor.Put(reinterpret_cast<const unsigned char *>(baFirstPIN.data()),
                   pinlen);

  stfEncryptor.Put(reinterpret_cast<const unsigned char *>(&certlen),
                   sizeof(certlen));
  stfEncryptor.Put(
      reinterpret_cast<const unsigned char *>(baCertificate.data()), certlen);

  stfEncryptor.MessageEnd();

  std::ofstream file(sPath.c_str(), std::ofstream::out | std::ofstream::binary);
  file.write(ciphertext.c_str(), ciphertext.length());
  file.close();
}
