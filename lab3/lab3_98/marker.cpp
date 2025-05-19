#include "marker.h"

  // Конструктор с параметрами
  MarkerThreadInfo::MarkerThreadInfo(int threadId)
      : id(threadId),
        canContinue(true),
        shouldTerminate(false),
        waitingForSignal(false) {
    continueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  }

  // Деструктор для освобождения ресурсов
  MarkerThreadInfo::~MarkerThreadInfo() {
    if (continueEvent != NULL) {
      CloseHandle(continueEvent);
    }
  }
