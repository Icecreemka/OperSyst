#include <process.h>
#include "marker.h"

#include <algorithm>
#include <clocale>
#include <cstdlib>
#include <ctime>


class ThreadSynchronizer {
 private:
  std::vector<int> array;                      // ������ ��� ���������
  std::vector<MarkerThreadInfo*> markerInfos;  // ���������� � ������� marker
  std::vector<HANDLE> markerThreads;           // ����������� ������� marker
  CRITICAL_SECTION arrayCS;  // ����������� ������ ��� ������ ������� � �������
  CRITICAL_SECTION syncCS;   // ����������� ������ ��� ������������� �������
  HANDLE mainEvent;          // ������� ��� ������������ ������ main
  int waitingThreadsCount;   // ������� �������, ��������� �������
  HANDLE startEvent;         // ������� ��� ������� ������ ������
  int activeThreads;         // ���������� �������� �������

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

  // ����������� ������� ������ ��� �������� � _beginthreadex
  static unsigned int __stdcall markerThreadFunction(void* arg) {
    ThreadSynchronizer* sync = ThreadSynchronizer::getInstance();
    return sync->markerThreadWork((MarkerThreadInfo*)arg);
  }

  // ����������� ��������� �� ��������� ������ (��� ������� �� �������)
  static ThreadSynchronizer* instance;

  // ��������� ������������ ���������� ������
  static ThreadSynchronizer* getInstance() { return instance; }

  // ��������� ����������
  static void setInstance(ThreadSynchronizer* sync) { instance = sync; }

  std::vector<int> getArraySnapshot();

 private:
  // ������� ������ marker
  unsigned int markerThreadWork(MarkerThreadInfo* threadInfo);
};