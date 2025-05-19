#include <process.h>
#include "marker.h"

#include <algorithm>
#include <clocale>
#include <cstdlib>
#include <ctime>


class ThreadSynchronizer {
 private:
  std::vector<int> array;                      // Массив для обработки
  std::vector<MarkerThreadInfo*> markerInfos;  // Информация о потоках marker
  std::vector<HANDLE> markerThreads;           // Дескрипторы потоков marker
  CRITICAL_SECTION arrayCS;  // Критическая секция для защиты доступа к массиву
  CRITICAL_SECTION syncCS;   // Критическая секция для синхронизации потоков
  HANDLE mainEvent;          // Событие для сигнализации потоку main
  int waitingThreadsCount;   // Счетчик потоков, ожидающих сигнала
  HANDLE startEvent;         // Событие для сигнала начала работы
  int activeThreads;         // Количество активных потоков

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

  // Статическая функция потока для передачи в _beginthreadex
  static unsigned int __stdcall markerThreadFunction(void* arg) {
    ThreadSynchronizer* sync = ThreadSynchronizer::getInstance();
    return sync->markerThreadWork((MarkerThreadInfo*)arg);
  }

  // Статический указатель на экземпляр класса (для доступа из потоков)
  static ThreadSynchronizer* instance;

  // Получение статического экземпляра класса
  static ThreadSynchronizer* getInstance() { return instance; }

  // Установка экземпляра
  static void setInstance(ThreadSynchronizer* sync) { instance = sync; }

  std::vector<int> getArraySnapshot();

 private:
  // Функция потока marker
  unsigned int markerThreadWork(MarkerThreadInfo* threadInfo);
};