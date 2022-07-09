
#include "SyncroMutex.h"

#include "./util.h"


CSyncroMutex::CSyncroMutex(void) { hMutex = NULL; }

void CSyncroMutex::Create(void) {}

void CSyncroMutex::Create(const char *szName) {}

CSyncroMutex::~CSyncroMutex(void) {}

void CSyncroMutex::Lock() {}

void CSyncroMutex::Unlock() {}

CSyncroLocker::CSyncroLocker(CSyncroMutex &mutex) {}

CSyncroLocker::~CSyncroLocker() {}
