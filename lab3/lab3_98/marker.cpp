#include "marker.h"

  // ����������� � �����������
  MarkerThreadInfo::MarkerThreadInfo(int threadId)
      : id(threadId),
        canContinue(true),
        shouldTerminate(false),
        waitingForSignal(false) {
    continueEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
  }

  // ���������� ��� ������������ ��������
  MarkerThreadInfo::~MarkerThreadInfo() {
    if (continueEvent != NULL) {
      CloseHandle(continueEvent);
    }
  }
