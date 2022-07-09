#pragma once
#include <PCSC/winscard.h>

#include "../Util/SyncroMutex.h"
#include "Token.h"

class CCardLocker {
  SCARDHANDLE hCard;

 public:
  CCardLocker(SCARDHANDLE card);
  ~CCardLocker(void);
  void Lock();
  void Unlock();
};
