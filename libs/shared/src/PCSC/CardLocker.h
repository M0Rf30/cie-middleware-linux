#pragma once
#include <PCSC/wintypes.h>
#include <PCSC/winscard.h>

#include "Util/SyncroMutex.h"
#include "PCSC/Token.h"

class CCardLocker {
  SCARDHANDLE hCard;

 public:
  CCardLocker(SCARDHANDLE card);
  ~CCardLocker(void);
  void Lock();
  void Unlock();
};
