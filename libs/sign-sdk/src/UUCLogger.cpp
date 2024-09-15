#include "UUCLogger.h"

#include <stdio.h>
#include <time.h>

logFunc pfnCrashliticsLog = NULL;

UUCLogger::UUCLogger(void) : m_nLogLevel(1) {}

UUCLogger::UUCLogger(const char* szLogFileName) : m_nLogLevel(1) {
  setLogFile(szLogFileName);
}

UUCLogger::~UUCLogger(void) {}

void UUCLogger::setLogLevel(int loglevel) { m_nLogLevel = loglevel; }

void UUCLogger::setLogFile(const char* szLogFileName) {
  strcpy(m_szLogFileName, szLogFileName);
}

void UUCLogger::log(const unsigned int nType, const char* szMsg,
                    const unsigned int nID, const char* szModuleName,
                    va_list args) {
  try {
    vsnprintf(m_szBuffer, 5000, szMsg, args);  // C4996
    log(nType, m_szBuffer, nID, szModuleName);
  } catch (...) {
  }
}

void UUCLogger::log(const unsigned int nType, const char* szMsg,
                    const unsigned int nID, const char* szModuleName) {
  if (static_cast<int>(nType) > m_nLogLevel) return;
  // Get UNIX-style time and display as number and string.
  time_t ltime;
  time(&ltime);
  tm* pCurTime = localtime(&ltime);

  char* szTime = asctime(pCurTime);
  szTime[strlen(szTime) - 1] = 0;

  char szLogMsg[5000];
  sprintf(szLogMsg, "[%s], %d, %X, %s, %s\n", szTime, nType, nID, szModuleName,
          szMsg);
  printf("%s", szLogMsg);
  if (pfnCrashliticsLog != NULL) pfnCrashliticsLog(szLogMsg);
}

void UUCLogger::logMessage(const unsigned int nID, const char* szModuleName,
                           const char* szMsg, ...) {
  if (LOG_TYPE_MESSAGE > m_nLogLevel) return;

  va_list args;

  szMsg = (szMsg == NULL) ? (char*)"" : szMsg;

  va_start(args, szMsg);

  log(LOG_TYPE_MESSAGE, szMsg, nID, szModuleName, args);

  va_end(args);
}

void UUCLogger::logWarning(const unsigned int nID, const char* szModuleName,
                           const char* szMsg, ...) {
  if (LOG_TYPE_WARNING > m_nLogLevel) return;

  va_list args;

  szMsg = (szMsg == NULL) ? (char*)"" : szMsg;

  va_start(args, szMsg);

  log(LOG_TYPE_WARNING, szMsg, nID, szModuleName, args);

  va_end(args);
}
void UUCLogger::logError(const unsigned int nID, const char* szModuleName,
                         const char* szMsg, ...) {
  if (LOG_TYPE_ERROR > m_nLogLevel) return;

  va_list args;

  szMsg = (szMsg == NULL) ? (char*)"" : szMsg;

  va_start(args, szMsg);

  log(LOG_TYPE_ERROR, szMsg, nID, szModuleName, args);

  va_end(args);
}

void UUCLogger::logDebug(const unsigned int nID, const char* szModuleName,
                         const char* szMsg, ...) {
  if (LOG_TYPE_DEBUG > m_nLogLevel) return;

  va_list args;

  szMsg = (szMsg == NULL) ? (char*)"" : szMsg;

  va_start(args, szMsg);

  log(LOG_TYPE_DEBUG, szMsg, nID, szModuleName, args);

  va_end(args);
}
