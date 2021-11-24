#pragma once
#include <PCSC/winscard.h>
#include "Token.h"
#include "../Util/SyncroMutex.h"

class CCardLocker {
    SCARDHANDLE hCard;
  public:
    CCardLocker(SCARDHANDLE card);
    ~CCardLocker(void);
    void Lock();
    void Unlock();
};
