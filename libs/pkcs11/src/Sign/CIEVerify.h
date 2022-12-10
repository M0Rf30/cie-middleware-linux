#pragma once

#include "Sign/disigonsdk.h"

#define MAX_INFO 20

struct verifyInfo_t {
  char name[MAX_LEN * 2];
  char surname[MAX_LEN * 2];
  char cn[MAX_LEN * 2];
  char signingTime[MAX_LEN * 2];
  char cadn[MAX_LEN * 2];
  int CertRevocStatus;
  bool isSignValid;
  bool isCertValid;
};

class CIEVerify {
 public:
  CIEVerify();
  ~CIEVerify();

  long verify(const char* input_file, VERIFY_RESULT* verifyResult,
              const char* proxy_address, int proxy_port, const char* userPass);
  long get_file_from_p7m(const char* input_file, const char* output_file);
};
