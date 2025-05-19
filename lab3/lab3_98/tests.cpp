#include <gtest/gtest.h>

#include "thread.h" 

// ����: �������� ������������� MarkerThreadInfo
TEST(MarkerThreadInfoTest, Initialization) {
  MarkerThreadInfo info(1);
  EXPECT_EQ(info.id, 1);
  EXPECT_TRUE(info.canContinue);
  EXPECT_FALSE(info.shouldTerminate);
  EXPECT_FALSE(info.waitingForSignal);
  EXPECT_NE(info.continueEvent, nullptr);
}

// ����: �������� ������������� �������
TEST(ThreadSynchronizerTest, ArrayInitialization) {
  ThreadSynchronizer sync(10);
  ThreadSynchronizer::setInstance(&sync);

  std::vector<int> snapshot = sync.getArraySnapshot();
  ASSERT_EQ(snapshot.size(), 10);
  for (int val : snapshot) {
    EXPECT_EQ(val, 0);
  }
}

// ����: ������� � ������� ����� �������
TEST(ThreadSynchronizerTest, SingleThreadMarkingAndTerminate) {
  ThreadSynchronizer sync(20);
  ThreadSynchronizer::setInstance(&sync);

  sync.startMarkerThreads(1);
  sync.signalStartAll();

  // �������� �������, ����� ����� ����� ���-�� ��������
  Sleep(200);
  sync.waitForAllThreadsBlocked();

  std::vector<int> beforeTerminate = sync.getArraySnapshot();
  bool hasMarks = std::any_of(beforeTerminate.begin(), beforeTerminate.end(),
                              [](int v) { return v != 0; });
  EXPECT_TRUE(hasMarks);  // ������ ���� �������

  sync.terminateMarkerThread(1);

  std::vector<int> afterTerminate = sync.getArraySnapshot();
  bool allZero = std::all_of(afterTerminate.begin(), afterTerminate.end(),
                             [](int v) { return v == 0; });
  EXPECT_TRUE(allZero);  // ��� ������ ���� �������
}
