
#include "Util/UtilException.h"

#include <stdio.h>

#include "Util/util.h"

logged_error::logged_error(std::string message)
    : std::runtime_error(message.c_str()) {
  logged_error(message.c_str());
}

logged_error::logged_error(const char *message) : std::runtime_error(message) {
  OutputDebugString("%s", what());
}

scard_error::scard_error(StatusWord sw)
    : logged_error(stdPrintf("Errore smart card:%x", sw)) {}

windows_error::windows_error(long ris)
    : logged_error(stdPrintf("Errore windows:(%08x) ", ris)) {}
