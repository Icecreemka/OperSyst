#include "thread.h"

  // ������������� �������������� � �������� �������
ThreadSynchronizer::ThreadSynchronizer(int arraySize)
    : waitingThreadsCount(0), activeThreads(0) {
    // ������������� �������
    array.resize(arraySize, 0);

    // ������������� ����������� ������
    InitializeCriticalSection(&arrayCS);
    InitializeCriticalSection(&syncCS);

    // �������� ������� �������������
    mainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  }

  // ���������� ��� ����������� ���������� �������
  ThreadSynchronizer::~ThreadSynchronizer() {
    // ��������� ��� ���������� ������
    for (size_t i = 0; i < markerThreads.size(); ++i) {
      if (markerThreads[i] != NULL) {
        WaitForSingleObject(markerThreads[i], INFINITE);
        CloseHandle(markerThreads[i]);
      }
    }

    // ����������� ���������� � �������
    for (size_t i = 0; i < markerInfos.size(); ++i) {
      delete markerInfos[i];
    }

    // ����������� ��������� �������
    CloseHandle(mainEvent);
    CloseHandle(startEvent);
    DeleteCriticalSection(&arrayCS);
    DeleteCriticalSection(&syncCS);
  }

  // ����� ��� ������� ������� marker
  void ThreadSynchronizer::startMarkerThreads(int numThreads) {
    // �������������� ������� �������� �������
    activeThreads = numThreads;

    // ����������� ����� ��� ������ �������
    markerInfos.reserve(numThreads);
    markerThreads.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i) {
      // ������� � �������������� ������ MarkerThreadInfo
      MarkerThreadInfo* threadInfo = new MarkerThreadInfo(i + 1);
      markerInfos.push_back(threadInfo);

      // ������� ����� marker
      unsigned int threadId;
      HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, markerThreadFunction,
                                             (void*)threadInfo, 0, &threadId);

      markerThreads.push_back(thread);
    }
  }

  // ����� ��� ������������ � ������ ������ ���� ������� marker
  void ThreadSynchronizer::signalStartAll() { SetEvent(startEvent); }

  // ����� �������� �������� � ������������� ����������� �� ���� �������
  void ThreadSynchronizer::waitForAllThreadsBlocked() {
    WaitForSingleObject(mainEvent, INFINITE);
    // ����� ������� ���������� ������� ��� ���������� �������������
    ResetEvent(mainEvent);
  }

  // ����� ��� ������ ����������� �������
  void ThreadSynchronizer::printArray() {
    EnterCriticalSection(&arrayCS);
    std::cout << "������� ��������� �������: [";
    for (size_t i = 0; i < array.size(); ++i) {
      std::cout << array[i];
      if (i < array.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
    LeaveCriticalSection(&arrayCS);
  }

  // ����� ��� ���������� ������ ���������� ������ marker
  void ThreadSynchronizer::terminateMarkerThread(int threadNum) {
    if (threadNum < 1 || threadNum > static_cast<int>(markerInfos.size()) ||
        markerThreads[threadNum - 1] == NULL) {
      std::cout << "������������ ����� ������ ��� ����� ��� ��������!"
                << std::endl;
      return;
    }

    // ������ � ������� �� 1 ������ ����������� ������
    int idx = threadNum - 1;

    // ������������� ������ � ������������� ����������
    EnterCriticalSection(&syncCS);
    markerInfos[idx]->shouldTerminate = true;
    markerInfos[idx]->canContinue = true;
    LeaveCriticalSection(&syncCS);

    SetEvent(markerInfos[idx]->continueEvent);

    // ���������� ���������� ������
    WaitForSingleObject(markerThreads[idx], INFINITE);
    CloseHandle(markerThreads[idx]);
    markerThreads[idx] = NULL;

    // ��������� ������� �������� �������
    activeThreads--;

    // ��������� ������� ��������� ������� (���� ����� ������)
    if (markerInfos[idx]->waitingForSignal) {
      EnterCriticalSection(&syncCS);
      waitingThreadsCount--;
      LeaveCriticalSection(&syncCS);
    }
  }

  // ����� ��� ������������ ���� ���������� ������� � ����������� ������
  void ThreadSynchronizer::continueRemainingThreads() {
    EnterCriticalSection(&syncCS);
    for (size_t i = 0; i < markerInfos.size(); ++i) {
      if (markerInfos[i] && markerThreads[i] != NULL) {
        markerInfos[i]->canContinue = true;
        SetEvent(markerInfos[i]->continueEvent);
      }
    }
    waitingThreadsCount = 0;
    LeaveCriticalSection(&syncCS);
  }

  // ��������, �������� �� �������� ������ marker
  bool ThreadSynchronizer::hasActiveMarkerThreads() {
    return activeThreads > 0;
  }

  ThreadSynchronizer* ThreadSynchronizer::instance = nullptr;

  // ������� ������ marker
  unsigned int ThreadSynchronizer::markerThreadWork(
      MarkerThreadInfo* threadInfo) {
    // ������� ������ �� main ��� ������ ������
    WaitForSingleObject(startEvent, INFINITE);

    // �������������� ��������� ��������� �����
    srand((unsigned int)time(NULL) + threadInfo->id);

    // ������� ���������� ���������
    int markedCount = 0;

    // �������� ���� ������ ������ marker
    while (true) {
      // ���������� ��������� ������
      int randomIndex = rand() % array.size();

      bool canMark = false;

      // ���������, ����� �� �������� �������
      EnterCriticalSection(&arrayCS);
      if (array[randomIndex] == 0) {
        canMark = true;
      }
      LeaveCriticalSection(&arrayCS);

      if (canMark) {
        // ����� �������� �������
        Sleep(5);

        EnterCriticalSection(&arrayCS);
        array[randomIndex] = threadInfo->id;
        threadInfo->markedIndices.push_back(randomIndex);
        LeaveCriticalSection(&arrayCS);

        markedCount++;
        Sleep(5);
      } else {
        // �� ����� �������� �������
        EnterCriticalSection(&arrayCS);
        std::cout << "����� " << threadInfo->id << ": �������� ��������� - "
                  << markedCount << ", �� ���� �������� ������� � �������� "
                  << randomIndex << " (�������� = " << array[randomIndex] << ")"
                  << std::endl;
        LeaveCriticalSection(&arrayCS);

        // ������������� ������ main � ������������� ����������� ������
        EnterCriticalSection(&syncCS);
        waitingThreadsCount++;
        threadInfo->canContinue = false;
        threadInfo->waitingForSignal = true;

        // ���� �� ��������� �����, ���������� main
        if (waitingThreadsCount == activeThreads) {
          SetEvent(mainEvent);
        }
        LeaveCriticalSection(&syncCS);

        // ���� ������� �� ������ main
        WaitForSingleObject(threadInfo->continueEvent, INFINITE);

        EnterCriticalSection(&syncCS);
        threadInfo->waitingForSignal = false;
        LeaveCriticalSection(&syncCS);

        // ���������, ����� �� ��������� ������
        EnterCriticalSection(&syncCS);
        bool shouldTerminate = threadInfo->shouldTerminate;
        LeaveCriticalSection(&syncCS);

        if (shouldTerminate) {
          // ��������� ������ ��� ���������� ��������
          EnterCriticalSection(&arrayCS);
          for (size_t i = 0; i < threadInfo->markedIndices.size(); ++i) {
            array[threadInfo->markedIndices[i]] = 0;
          }
          LeaveCriticalSection(&arrayCS);

          std::cout << "����� " << threadInfo->id
                    << " �������� ������, ������� "
                    << threadInfo->markedIndices.size() << " ���������"
                    << std::endl;
          return 0;
        }
      }
    }
    return 0;
  }

  std::vector<int> ThreadSynchronizer::getArraySnapshot() {
    EnterCriticalSection(&arrayCS);
    std::vector<int> copy = array;
    LeaveCriticalSection(&arrayCS);
    return copy;
  }