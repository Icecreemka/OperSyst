#include "thread.h"

  ThreadSynchronizer::ThreadSynchronizer(int arraySize)
      : array(arraySize, 0),
        waitingThreadsCount(0),
        startFlag(false),
        activeThreads(0) {}

  // ���������� ��� ����������� ���������� �������
  ThreadSynchronizer::~ThreadSynchronizer() {
    // ��������� ��� ���������� ������
    for (size_t i = 0; i < markerThreads.size(); ++i) {
      if (markerThreads[i] && markerThreads[i]->joinable()) {
        markerThreads[i]->join();
      }
    }
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
      auto threadInfo = std::make_unique<MarkerThreadInfo>(i + 1);

      // ��������� ��������� �� ���������� � ������
      MarkerThreadInfo* infoPtr = threadInfo.get();
      markerInfos.push_back(std::move(threadInfo));

      // ������� ����� marker
      auto thread = std::make_unique<std::thread>(
          &ThreadSynchronizer::markerThreadFunction, this, infoPtr);
      markerThreads.push_back(std::move(thread));
    }
  }

  // ����� ��� ������������ � ������ ������ ���� ������� marker
  void ThreadSynchronizer::signalStartAll() {
    {
      std::lock_guard<std::mutex> lock(syncMutex);
      startFlag = true;
    }
    startCV.notify_all();
  }

  // ����� �������� �������� � ������������� ����������� �� ���� �������
  void ThreadSynchronizer::waitForAllThreadsBlocked() {
    std::unique_lock<std::mutex> lock(syncMutex);
    mainCV.wait(
        lock, [this]() { return waitingThreadsCount.load() == activeThreads; });
  }

  // ����� ��� ������ ����������� �������
  void ThreadSynchronizer::printArray() {
    std::lock_guard<std::mutex> lock(arrayMutex);
    std::cout << "������� ��������� �������: [";
    for (size_t i = 0; i < array.size(); ++i) {
      std::cout << array[i];
      if (i < array.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
  }

  // ����� ��� ���������� ������ ���������� ������ marker
  void ThreadSynchronizer::terminateMarkerThread(int threadNum) {
    if (threadNum < 1 || threadNum > static_cast<int>(markerInfos.size()) ||
        !markerThreads[threadNum - 1]) {
      std::cout << "������������ ����� ������ ��� ����� ��� ��������!"
                << std::endl;
      return;
    }

    // ������ � ������� �� 1 ������ ����������� ������
    int idx = threadNum - 1;

    // ������������� ������ � ������������� ����������
    {
      std::lock_guard<std::mutex> lock(syncMutex);
      markerInfos[idx]->shouldTerminate = true;
      markerInfos[idx]->canContinue = true;
    }
    markerInfos[idx]->continueCV.notify_one();

    // ���������� ���������� ������
    if (markerThreads[idx]->joinable()) {
      markerThreads[idx]->join();
      markerThreads[idx].reset();  // ����������� ������� ������
    }

    // ��������� ������� �������� �������
    activeThreads--;

    // ��������� ������� ��������� ������� (���� ����� ������)
    if (markerInfos[idx]->waitingForSignal) {
      waitingThreadsCount.fetch_sub(1);
    }
  }

  // ����� ��� ������������ ���� ���������� ������� � ����������� ������
  void ThreadSynchronizer::continueRemainingThreads() {
    {
      std::lock_guard<std::mutex> lock(syncMutex);
      for (auto& threadInfo : markerInfos) {
        if (threadInfo && !threadInfo->shouldTerminate) {
          threadInfo->canContinue = true;
        }
      }
    }

    // ���������� ������ ���� ��������� �������
    for (auto& threadInfo : markerInfos) {
      if (threadInfo && !threadInfo->shouldTerminate) {
        threadInfo->continueCV.notify_one();
      }
    }

    // ���������� ������� ��������� �������
    waitingThreadsCount.store(0);
  }

  // ��������, �������� �� �������� ������ marker
  bool ThreadSynchronizer::hasActiveMarkerThreads() {
    return activeThreads > 0;
  }

  // ������� ������ marker
  void ThreadSynchronizer::markerThreadFunction(MarkerThreadInfo* threadInfo) {
    // ������� ������ �� main ��� ������ ������
    {
      std::unique_lock<std::mutex> lock(syncMutex);
      startCV.wait(lock, [this]() { return startFlag; });
    }

    // �������������� ��������� ��������� �����
    std::mt19937 rng(static_cast<unsigned int>(threadInfo->id));
    std::uniform_int_distribution<int> dist(0,
                                            static_cast<int>(array.size() - 1));

    // ������� ���������� ���������
    int markedCount = 0;

    // �������� ���� ������ ������ marker
    while (true) {
      // ���������� ��������� ������
      int randomIndex = dist(rng);

      bool canMark = false;

      // ���������, ����� �� �������� �������
      {
        std::lock_guard<std::mutex> lock(arrayMutex);
        if (array[randomIndex] == 0) {
          canMark = true;
        }
      }

      if (canMark) {
        // ����� �������� �������
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        {
          std::lock_guard<std::mutex> lock(arrayMutex);
          array[randomIndex] = threadInfo->id;
          threadInfo->markedIndices.push_back(randomIndex);
        }

        markedCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      } else {
        // �� ����� �������� �������
        {
          std::lock_guard<std::mutex> lock(arrayMutex);
          std::cout << "����� " << threadInfo->id << ": �������� ��������� - "
                    << markedCount << ", �� ���� �������� ������� � �������� "
                    << randomIndex << " (�������� = " << array[randomIndex]
                    << ")" << std::endl;
        }

        // ������������� ������ main � ������������� ����������� ������
        {
          std::unique_lock<std::mutex> lock(syncMutex);
          waitingThreadsCount.fetch_add(1);
          threadInfo->canContinue = false;
          threadInfo->waitingForSignal = true;
        }

        // ���� �� ��������� �����, ���������� main
        if (waitingThreadsCount.load() == activeThreads) {
          mainCV.notify_one();
        }

        // ���� ������� �� ������ main
        {
          std::unique_lock<std::mutex> lock(syncMutex);
          threadInfo->continueCV.wait(
              lock, [threadInfo]() { return threadInfo->canContinue; });
          threadInfo->waitingForSignal = false;
        }

        // ���������, ����� �� ��������� ������
        if (threadInfo->shouldTerminate) {
          // ��������� ������ ��� ���������� ��������
          {
            std::lock_guard<std::mutex> lock(arrayMutex);
            for (int index : threadInfo->markedIndices) {
              array[index] = 0;
            }
          }

          std::cout << "����� " << threadInfo->id
                    << " �������� ������, ������� "
                    << threadInfo->markedIndices.size() << " ���������"
                    << std::endl;
          return;
        }
      }
    }
  }
