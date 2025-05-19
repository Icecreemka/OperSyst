#include <process.h>
#include "thread.h"

#include <algorithm>
#include <clocale>
#include <cstdlib>
#include <ctime>


// ������������� ������������ ���������
//ThreadSynchronizer* ThreadSynchronizer::instance = NULL;

int main() {
  // ��������� ������� ������ ��� ����������� ����������� ���������
  setlocale(LC_ALL, "Russian");

  // ������ ������� �������
  int arraySize;
  std::cout << "������� ������ �������: ";
  std::cin >> arraySize;

  if (arraySize <= 0) {
    std::cout << "������ ������� ������ ���� ������������� ������."
              << std::endl;
    return 1;
  }

  // ������� ������ �������������� �������
  ThreadSynchronizer synchronizer(arraySize);
  // ������������� ��������� ��� ������� �� ����������� �������
  ThreadSynchronizer::setInstance(&synchronizer);

  // ������ ���������� ������� marker
  int numThreads;
  std::cout << "������� ���������� ������� marker: ";
  std::cin >> numThreads;

  if (numThreads <= 0) {
    std::cout << "���������� ������� ������ ���� ������������� ������."
              << std::endl;
    return 1;
  }

  // ��������� ������ marker
  synchronizer.startMarkerThreads(numThreads);

  // ������ �� ������ ������ ���� ������� marker
  synchronizer.signalStartAll();

  // �������� ���� ������ ���������
  while (synchronizer.hasActiveMarkerThreads()) {
    // ����, ���� ��� ������ marker �� ������� ������� � �������������
    // �����������
    synchronizer.waitForAllThreadsBlocked();

    // ������� ���������� �������
    synchronizer.printArray();

    // ����������� ����� ������ ��� ����������
    int threadToTerminate;
    std::cout << "������� ����� ������ ��� ���������� (1-" << numThreads
              << "): ";
    std::cin >> threadToTerminate;

    // ������ ������ �� ���������� ���������� ������
    synchronizer.terminateMarkerThread(threadToTerminate);

    // ������� ����������� ���������� �������
    synchronizer.printArray();

    // ������ ������ �� ����������� ���������� �������
    synchronizer.continueRemainingThreads();
  }

  std::cout << "��� ������ marker ��������� ������. ��������� �����������."
            << std::endl;

  return 0;
}