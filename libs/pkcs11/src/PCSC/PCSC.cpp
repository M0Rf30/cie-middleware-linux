
#include "PCSC.h"

#include <unistd.h>

#include <thread>

#include "../Util/UtilException.h"

struct transData {
  SCARDCONTEXT context;
  bool started;
};
bool safeTransaction::isLocked() { return locked; }
safeTransaction::safeTransaction(safeConnection &conn, DWORD dwDisposition) {
  this->hCard = conn.hCard;
  this->dwDisposition = dwDisposition;
  locked = false;

  if (SCardBeginTransaction(hCard) != SCARD_S_SUCCESS) {
    this->hCard = NULL;
    this->dwDisposition = 0;
    return;
  } else {
    locked = true;
  }
}

void safeTransaction::unlock() {
  if (hCard != NULL && locked) {
    SCardEndTransaction(hCard, dwDisposition);
    locked = false;
  }
}

safeTransaction::~safeTransaction() {
  if (hCard != NULL && locked) {
    SCardEndTransaction(hCard, dwDisposition);
  }
}

safeConnection::safeConnection(SCARDHANDLE hCard) { this->hCard = hCard; }

safeConnection::safeConnection(SCARDCONTEXT hContext, LPCSTR szReader,
                               DWORD dwShareMode) {
  DWORD dwProtocol;
  this->hContext = hContext;
  if (SCardConnect(hContext, szReader, dwShareMode, SCARD_PROTOCOL_T1, &hCard,
                   &dwProtocol) != SCARD_S_SUCCESS)
    hCard = NULL;
}

safeConnection::~safeConnection() {
  if (hCard) {
    SCardDisconnect(hCard, SCARD_RESET_CARD);
  }
}
safeConnection::operator SCARDHANDLE() { return hCard; }

readerMonitor::~readerMonitor() {
  stopMonitor = true;
  SCardCancel(hContext);
  Thread.join();
  SCardReleaseContext(hContext);
}

readerMonitor::readerMonitor(void (*eventHandler)(std::string &reader,
                                                  bool insert, void *appData),
                             void *appData)
    : appData(appData) {
  LONG _call_ris;
  if ((_call_ris = (SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL,
                                          &hContext))) != 0) {
    throw windows_error(_call_ris);
  }
  stopMonitor = false;
  readerEvent = eventHandler;

  Thread = std::thread(
      [](readerMonitor *rm) -> DWORD {
        std::vector<std::string> readerList;
        std::vector<SCARD_READERSTATE> states;

        auto loadReaderList = [&]() -> void {
          char *readers = nullptr;
          unsigned long int len = 0;

          if (SCardListReaders(rm->hContext, NULL, NULL, &len))
            throw logged_error("Nessun lettore installato");

          readers = (char *)calloc(len, sizeof(char));
          SCardListReaders(rm->hContext, NULL, readers, &len);

          const char *curReader = readers;
          readerList.clear();
          for (; curReader[0] != 0; curReader += strnlen(curReader, len) + 1)
            readerList.push_back(std::string(curReader));

          free(readers);
          states.resize((DWORD)readerList.size() + 1);
          for (DWORD i = 0; i < readerList.size(); i++) {
            states[i].szReader = readerList[i].c_str();
          }
          auto &PnP = states[(DWORD)readerList.size()];
          PnP.szReader = "\\\\?PnP?\\Notification";
          PnP.pvUserData = (void *)PnP.szReader;

          SCardGetStatusChange(rm->hContext, 0, states.data(), states.size());
          for (DWORD i = 0; i < states.size(); i++)
            states[i].dwCurrentState = states[i].dwEventState;
        };
        loadReaderList();

        while (!rm->stopMonitor) {
          if (SCardGetStatusChange(rm->hContext, INFINITE, states.data(),
                                   states.size()) == SCARD_E_CANCELLED)
            break;
          for (DWORD i = 0; i < states.size(); i++) {
            auto &state = states[i];
            if (state.pvUserData != nullptr &&
                (state.dwEventState & SCARD_STATE_CHANGED) ==
                    SCARD_STATE_CHANGED) {
              loadReaderList();
              break;
            }
            if (((state.dwCurrentState & SCARD_STATE_PRESENT) ==
                 SCARD_STATE_PRESENT) &&
                ((state.dwEventState & SCARD_STATE_PRESENT) == 0))
              rm->readerEvent(readerList[i], false, rm->appData);

            else if (((state.dwCurrentState & SCARD_STATE_PRESENT) == 0) &&
                     ((state.dwEventState & SCARD_STATE_PRESENT) ==
                      SCARD_STATE_PRESENT))
              rm->readerEvent(readerList[i], true, rm->appData);

            state.dwCurrentState = state.dwEventState;
          }
        }
        return 0;
      },
      this);
}
