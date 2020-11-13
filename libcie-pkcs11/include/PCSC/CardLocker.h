#pragma once
#include <winscard.h>
#include <PCSC/Token.h>
#include <Util/SyncroMutex.h>

class CCardLocker
{
    SCARDHANDLE hCard;
public:
    CCardLocker(SCARDHANDLE card);
    ~CCardLocker(void);
    void Lock();
    void Unlock();
};
