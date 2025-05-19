#include <gtest/gtest.h>

#include "thread.h"

// Простая обертка, чтобы не выполнять реальные задержки и I/O
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

// Тест корректного создания и запуска потоков
TEST(ThreadSynchronizerTest, CreateAndStartThreads) {
  ThreadSynchronizer sync(10);
  sync.startMarkerThreads(3);
  sync.signalStartAll();

  // Ждём максимум 1 секунду, пока потоки что-нибудь сделают
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Проверим, что хоть что-то было помечено
  // Для этого воспользуемся рефлексией через printArray (или модернизацией
  // класса)

  // Просто проверим, что потоки всё ещё активны
  EXPECT_TRUE(sync.hasActiveMarkerThreads());

  // Завершим все потоки, чтобы тест не завис
  sync.terminateMarkerThread(1);
  sync.terminateMarkerThread(2);
  sync.terminateMarkerThread(3);
}

// Тест завершения одного потока
TEST(ThreadSynchronizerTest, TerminateOneThread) {
  ThreadSynchronizer sync(10);
  sync.startMarkerThreads(2);
  sync.signalStartAll();
  sync.waitForAllThreadsBlocked();

  sync.terminateMarkerThread(1);
  EXPECT_TRUE(sync.hasActiveMarkerThreads());  // один поток еще жив

  sync.terminateMarkerThread(2);
  EXPECT_FALSE(sync.hasActiveMarkerThreads());  // все завершены
}

// Тест повторного завершения уже завершенного потока
TEST(ThreadSynchronizerTest, TerminateInvalidThread) {
  ThreadSynchronizer sync(5);
  sync.startMarkerThreads(1);
  sync.signalStartAll();
  sync.waitForAllThreadsBlocked();

  sync.terminateMarkerThread(1);  // первый раз OK
  sync.terminateMarkerThread(1);  // второй раз - уже завершен, не должно падать
  EXPECT_FALSE(sync.hasActiveMarkerThreads());
}

// Тест продолжения работы потоков
TEST(ThreadSynchronizerTest, ContinueRemainingThreads) {
  ThreadSynchronizer sync(5);
  sync.startMarkerThreads(2);
  sync.signalStartAll();
  sync.waitForAllThreadsBlocked();

  // Завершаем один поток
  sync.terminateMarkerThread(1);
  sync.continueRemainingThreads();

  // Ждем пока оставшийся поток снова остановится
  sync.waitForAllThreadsBlocked();

  EXPECT_TRUE(sync.hasActiveMarkerThreads());

  sync.terminateMarkerThread(2);
  EXPECT_FALSE(sync.hasActiveMarkerThreads());
}
