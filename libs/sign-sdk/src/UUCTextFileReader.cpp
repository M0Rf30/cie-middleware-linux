// UUCTextFile.cpp: implementation of the UUCTextFile class.
#include "UUCTextFileReader.h"

#include "UUCLogger.h"
USE_LOG;

// Construction/Destruction
UUCTextFileReader::UUCTextFileReader(const char* szFilePath) {
  m_pf = fopen(szFilePath, "rt");
  if (!m_pf) {
    throw (long)ERROR_FILE_NOT_FOUND;
  }
}

UUCTextFileReader::~UUCTextFileReader() { fclose(m_pf); }

long UUCTextFileReader::readLine(UUCByteArray& line) {
  char szLine[2];
  unsigned int i = 0;
  while ((fread(szLine, 1, 1, m_pf) > 0) && (szLine[0] != '\n')) {
    i++;
    line.append(szLine[0]);
  }

  if (i > 0) {
    line.append((BYTE)0);
    return 0;
  } else if ((i == 0) && szLine[0] == '\n') {
    return readLine(line);
  } else {
    return -1;
  }
}

long UUCTextFileReader::readLine(char* szLine, unsigned long nLen) {
  unsigned int i = 0;
  while ((fread(szLine + i, 1, 1, m_pf) > 0) && (szLine[i] != '\n')) {
    i++;
    if (i == nLen) {
      throw (long)ERROR_MORE_DATA;
    }
  }

  if (i > 0) {
    szLine[i] = 0;
    return 0;
  } else if ((i == 0) && szLine[i] == '\n') {
    return readLine(szLine, nLen);
  } else {
    return -1;
  }
}
