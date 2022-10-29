
#include "funccallinfo.h"

#include <stdio.h>

thread_local size_t tlsCallDepth = 0;
thread_local std::unique_ptr<CFuncCallInfoList> callQueue = nullptr;
extern bool FunctionLog;
extern unsigned int GlobalDepth;
extern bool GlobalParam;
char szEmpty[] = {'\0'};

CFuncCallInfo::CFuncCallInfo(const char *name, CLog &logInfo) : log(logInfo) {
  fName = name;
  if (FunctionLog) {
    if (tlsCallDepth < GlobalDepth) {
      LogNum =
          logInfo.write("%*sIN -> %s", (DWORD)tlsCallDepth, szEmpty, fName);
    }
  }

  tlsCallDepth = tlsCallDepth + 1;
}

CFuncCallInfo::~CFuncCallInfo() {
  tlsCallDepth = tlsCallDepth - 1;
  if (fName)
    log.write("%*sOUT -> %s (%u)", (DWORD)tlsCallDepth, szEmpty, fName,
              LogNum - 1);
}

const char *CFuncCallInfo::FunctionName() { return fName; }
