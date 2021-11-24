#pragma once
#include <string>

#include "../Util/Array.h"

class CBase64 {
  public:
    CBase64();
    ~CBase64();

    std::string &Encode(ByteArray &data, std::string &encodedData);
    ByteDynArray &Decode(const char *encodedData, ByteDynArray &data);
};
