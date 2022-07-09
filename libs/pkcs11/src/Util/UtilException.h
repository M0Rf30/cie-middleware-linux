#pragma once

#include <memory>
#include <stdexcept>

#include "./defines.h"

class logged_error : public std::runtime_error {
 public:
  logged_error(std::string message);
  logged_error(const char *message);
};

class scard_error : public logged_error {
 public:
  scard_error(StatusWord sw);
};

class windows_error : logged_error {
 public:
  windows_error(long ris);
};
