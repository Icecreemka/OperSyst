#include "marker.h"

#include <random>

class ThreadSynchronizer {
 private:
  std::vector<int> array;  // Массив для обработки
  std::vector<std::unique_ptr<MarkerThreadInfo>>
      markerInfos;  // Информация о потоках marker
  std::vector<std::unique_ptr<std::thread>> markerThreads;  // Потоки marker
  std::mutex arrayMutex;  // Мьютекс для защиты доступа к массиву
  std::mutex syncMutex;   // Мьютекс для синхронизации потоков
  std::condition_variable
      mainCV;  // Условная переменная для сигнализации потоку main
  std::atomic<int> waitingThreadsCount;  // Счетчик потоков, ожидающих сигнала
  std::condition_variable
      startCV;        // Условная переменная для сигнала начала работы
  bool startFlag;     // Флаг начала работы потоков
  int activeThreads;  // Количество активных потоков

 public:
  // Инициализация синхронизатора с размером массива
  ThreadSynchronizer(int arraySize);

  // Деструктор для корректного завершения потоков
  ~ThreadSynchronizer();
  // Метод для запуска потоков marker
  void startMarkerThreads(int numThreads);

  // Метод для сигнализации о начале работы всем потокам marker
  void signalStartAll();

  // Метод ожидания сигналов о невозможности продолжения от всех потоков
  void waitForAllThreadsBlocked();

  // Метод для печати содержимого массива
  void printArray();

  // Метод для завершения работы указанного потока marker
  void terminateMarkerThread(int threadNum);

  // Метод для сигнализации всем оставшимся потокам о продолжении работы
  void continueRemainingThreads();

  // Проверка, остались ли активные потоки marker
  bool hasActiveMarkerThreads();

 private:
  // Функция потока marker
  void markerThreadFunction(MarkerThreadInfo* threadInfo);
};
