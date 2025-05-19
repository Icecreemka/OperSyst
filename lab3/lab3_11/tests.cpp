#include <gtest/gtest.h>

#include "thread.h"

// ������� �������, ����� �� ��������� �������� �������� � I/O
class ThreadTestHelper {
 public:
  static int countMarkedElements(const std::vector<int>& array) {
    int count = 0;
    for (int val : array) {
      if (val != 0) ++count;
    }
    return count;
  }
};

// ���� ����������� �������� � ������� �������
TEST(ThreadSynchronizerTest, CreateAndStartThreads) {
  ThreadSynchronizer sync(10);
  sync.startMarkerThreads(3);
  sync.signalStartAll();

  // ��� �������� 1 �������, ���� ������ ���-������ �������
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // ��������, ��� ���� ���-�� ���� ��������
  // ��� ����� ������������� ���������� ����� printArray (��� �������������
  // ������)

  // ������ ��������, ��� ������ �� ��� �������
  EXPECT_TRUE(sync.hasActiveMarkerThreads());

  // �������� ��� ������, ����� ���� �� �����
  sync.terminateMarkerThread(1);
  sync.terminateMarkerThread(2);
  sync.terminateMarkerThread(3);
}

// ���� ���������� ������ ������
TEST(ThreadSynchronizerTest, TerminateOneThread) {
  ThreadSynchronizer sync(10);
  sync.startMarkerThreads(2);
  sync.signalStartAll();
  sync.waitForAllThreadsBlocked();

  sync.terminateMarkerThread(1);
  EXPECT_TRUE(sync.hasActiveMarkerThreads());  // ���� ����� ��� ���

  sync.terminateMarkerThread(2);
  EXPECT_FALSE(sync.hasActiveMarkerThreads());  // ��� ���������
}

// ���� ���������� ���������� ��� ������������ ������
TEST(ThreadSynchronizerTest, TerminateInvalidThread) {
  ThreadSynchronizer sync(5);
  sync.startMarkerThreads(1);
  sync.signalStartAll();
  sync.waitForAllThreadsBlocked();

  sync.terminateMarkerThread(1);  // ������ ��� OK
  sync.terminateMarkerThread(1);  // ������ ��� - ��� ��������, �� ������ ������
  EXPECT_FALSE(sync.hasActiveMarkerThreads());
}

// ���� ����������� ������ �������
TEST(ThreadSynchronizerTest, ContinueRemainingThreads) {
  ThreadSynchronizer sync(5);
  sync.startMarkerThreads(2);
  sync.signalStartAll();
  sync.waitForAllThreadsBlocked();

  // ��������� ���� �����
  sync.terminateMarkerThread(1);
  sync.continueRemainingThreads();

  // ���� ���� ���������� ����� ����� �����������
  sync.waitForAllThreadsBlocked();

  EXPECT_TRUE(sync.hasActiveMarkerThreads());

  sync.terminateMarkerThread(2);
  EXPECT_FALSE(sync.hasActiveMarkerThreads());
}
