#include "thread.h"

int main() {
  // Установка русской локали для корректного отображения кириллицы
  std::setlocale(LC_ALL, "Russian");

  // Запрос размера массива
  int arraySize;
  std::cout << "Введите размер массива: ";
  std::cin >> arraySize;

  if (arraySize <= 0) {
    std::cout << "Размер массива должен быть положительным числом."
              << std::endl;
    return 1;
  }

  // Создаем объект синхронизатора потоков
  ThreadSynchronizer synchronizer(arraySize);

  // Запрос количества потоков marker
  int numThreads;
  std::cout << "Введите количество потоков marker: ";
  std::cin >> numThreads;

  if (numThreads <= 0) {
    std::cout << "Количество потоков должно быть положительным числом."
              << std::endl;
    return 1;
  }

  // Запускаем потоки marker
  synchronizer.startMarkerThreads(numThreads);

  // Сигнал на начало работы всех потоков marker
  synchronizer.signalStartAll();

  // Основной цикл работы программы
  while (synchronizer.hasActiveMarkerThreads()) {
    // Ждем, пока все потоки marker не подадут сигналы о невозможности
    // продолжения
    synchronizer.waitForAllThreadsBlocked();

    // Выводим содержимое массива
    synchronizer.printArray();

    // Запрашиваем номер потока для завершения
    int threadToTerminate;
    std::cout << "Введите номер потока для завершения (1-" << numThreads
              << "): ";
    std::cin >> threadToTerminate;

    // Подаем сигнал на завершение указанному потоку
    synchronizer.terminateMarkerThread(threadToTerminate);

    // Выводим обновленное содержимое массива
    synchronizer.printArray();

    // Подаем сигнал на продолжение оставшимся потокам
    synchronizer.continueRemainingThreads();
  }

  std::cout << "Все потоки marker завершили работу. Программа завершается."
            << std::endl;

  return 0;
}
