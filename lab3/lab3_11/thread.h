#include "marker.h"

#include <random>

class ThreadSynchronizer {
 private:
  std::vector<int> array;  // ������ ��� ���������
  std::vector<std::unique_ptr<MarkerThreadInfo>>
      markerInfos;  // ���������� � ������� marker
  std::vector<std::unique_ptr<std::thread>> markerThreads;  // ������ marker
  std::mutex arrayMutex;  // ������� ��� ������ ������� � �������
  std::mutex syncMutex;   // ������� ��� ������������� �������
  std::condition_variable
      mainCV;  // �������� ���������� ��� ������������ ������ main
  std::atomic<int> waitingThreadsCount;  // ������� �������, ��������� �������
  std::condition_variable
      startCV;        // �������� ���������� ��� ������� ������ ������
  bool startFlag;     // ���� ������ ������ �������
  int activeThreads;  // ���������� �������� �������

 public:
  // ������������� �������������� � �������� �������
  ThreadSynchronizer(int arraySize);

  // ���������� ��� ����������� ���������� �������
  ~ThreadSynchronizer();
  // ����� ��� ������� ������� marker
  void startMarkerThreads(int numThreads);

  // ����� ��� ������������ � ������ ������ ���� ������� marker
  void signalStartAll();

  // ����� �������� �������� � ������������� ����������� �� ���� �������
  void waitForAllThreadsBlocked();

  // ����� ��� ������ ����������� �������
  void printArray();

  // ����� ��� ���������� ������ ���������� ������ marker
  void terminateMarkerThread(int threadNum);

  // ����� ��� ������������ ���� ���������� ������� � ����������� ������
  void continueRemainingThreads();

  // ��������, �������� �� �������� ������ marker
  bool hasActiveMarkerThreads();

 private:
  // ������� ������ marker
  void markerThreadFunction(MarkerThreadInfo* threadInfo);
};
