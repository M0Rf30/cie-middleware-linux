#include "ModuleInfo.h"
#include "UtilException.h"

CModuleInfo::CModuleInfo() {}

HANDLE CModuleInfo::getApplicationModule() {
  return 0;
}

HANDLE CModuleInfo::getModule() { return module; }

void CModuleInfo::init(HANDLE module) { this->module = module; }

CModuleInfo::~CModuleInfo(void) {}
