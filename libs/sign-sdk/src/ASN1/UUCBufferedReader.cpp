/* BufferedReader.cpp: implementation of the UUCBufferedReader class.
 *  Copyright (c) 2000-2006 by Ugo Chirico - http://www.ugosweb.com
 *  All Rights Reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "UUCBufferedReader.h"

#include <stdlib.h>

#define MAX_BUF 2000
#define MAX_STACK_SIZE 100

// Construction/Destruction
UUCBufferedReader::UUCBufferedReader(const UUCByteArray& buffer) {
  m_pbtBuffer = (BYTE*)buffer.getContent();
  m_nBufLen = buffer.getLength();

  m_nBufPos = 0;
  m_nIndex = 0;
  m_bEOF = true;
  m_pnStack =
      (unsigned int*)realloc(NULL, MAX_STACK_SIZE * sizeof(unsigned int));
  m_nStackSize = MAX_STACK_SIZE;
  m_nTop = -1;
}

UUCBufferedReader::UUCBufferedReader(const BYTE* pbtBuffer, int len)
    : m_pbtBuffer(NULL) {
  m_pbtBuffer = (BYTE*)pbtBuffer;
  m_nBufLen = len;

  m_nBufPos = 0;
  m_nIndex = 0;
  m_bEOF = true;
  m_pnStack =
      (unsigned int*)realloc(NULL, MAX_STACK_SIZE * sizeof(unsigned int));
  m_nStackSize = MAX_STACK_SIZE;
  m_nTop = -1;
}

UUCBufferedReader::~UUCBufferedReader() {
  try {
    free(m_pnStack);
  } catch (...) {
  }
}

unsigned int UUCBufferedReader::read(BYTE* pbtBuffer, unsigned int nLen) {
  int nRead = 0;
  if (m_nIndex + nLen > m_nBufLen) {
    if (!m_bEOF) {
      return read(pbtBuffer, nLen);
    } else {
      memcpy(pbtBuffer, m_pbtBuffer + m_nIndex, (m_nBufLen - m_nIndex));
      nRead = (m_nBufLen - m_nIndex);
      m_nIndex += nRead;
    }
  } else {
    memcpy(pbtBuffer, m_pbtBuffer + m_nIndex, nLen);
    nRead = nLen;
    m_nIndex += nRead;
  }

  return nRead;
}

unsigned int UUCBufferedReader::read(UUCByteArray& byteArray) {
  BYTE pbtBuf[255];
  unsigned int nRead = 0;
  unsigned int nCount = 0;
  while ((nRead = read(pbtBuf, 255)) != 0) {
    byteArray.append(pbtBuf, nRead);
    nCount += nRead;
  }

  nCount += nRead;
  byteArray.append(pbtBuf, nRead);

  return nCount;
}

void UUCBufferedReader::mark() {
  m_nTop++;
  if (m_nTop >= m_nStackSize) {
    m_nStackSize += MAX_STACK_SIZE;
    m_pnStack =
        (unsigned int*)realloc(m_pnStack, m_nStackSize * sizeof(unsigned int));
  }

  m_pnStack[m_nTop] = m_nIndex;
}

void UUCBufferedReader::releaseMark() {
  if (m_nTop > 0) {
    m_nTop--;
  }
}

void UUCBufferedReader::reset() {
  if (static_cast<int>(m_nTop) > -1) {
    m_nIndex = m_pnStack[m_nTop];
    m_nTop--;
  }
}

unsigned int UUCBufferedReader::getPosition() { return m_nIndex; }

void UUCBufferedReader::setPosition(unsigned int index) { m_nIndex = index; }
