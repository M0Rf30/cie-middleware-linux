#pragma once

#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>

class CCardContext {
 public:
  SCARDCONTEXT hContext;

  CCardContext(void);
  ~CCardContext(void);

  operator SCARDCONTEXT();
  void validate();
  void renew();

 private:
  void getContext();
};
