#include "thread.h"

  // Инициализация синхронизатора с размером массива
ThreadSynchronizer::ThreadSynchronizer(int arraySize)
    : waitingThreadsCount(0), activeThreads(0) {
    // Инициализация массива
    array.resize(arraySize, 0);

    // Инициализация критических секций
    InitializeCriticalSection(&arrayCS);
    InitializeCriticalSection(&syncCS);

    // Создание событий синхронизации
    mainEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    startEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
  }

  // Деструктор для корректного завершения потоков
  ThreadSynchronizer::~ThreadSynchronizer() {
    // Завершаем все оставшиеся потоки
    for (size_t i = 0; i < markerThreads.size(); ++i) {
      if (markerThreads[i] != NULL) {
        WaitForSingleObject(markerThreads[i], INFINITE);
        CloseHandle(markerThreads[i]);
      }
    }

    // Освобождаем информацию о потоках
    for (size_t i = 0; i < markerInfos.size(); ++i) {
      delete markerInfos[i];
    }

    // Освобождаем системные ресурсы
    CloseHandle(mainEvent);
    CloseHandle(startEvent);
    DeleteCriticalSection(&arrayCS);
    DeleteCriticalSection(&syncCS);
  }

  // Метод для запуска потоков marker
  void ThreadSynchronizer::startMarkerThreads(int numThreads) {
    // Инициализируем счетчик активных потоков
    activeThreads = numThreads;

    // Резервируем место для данных потоков
    markerInfos.reserve(numThreads);
    markerThreads.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i) {
      // Создаем и инициализируем объект MarkerThreadInfo
      MarkerThreadInfo* threadInfo = new MarkerThreadInfo(i + 1);
      markerInfos.push_back(threadInfo);

      // Создаем поток marker
      unsigned int threadId;
      HANDLE thread = (HANDLE)_beginthreadex(NULL, 0, markerThreadFunction,
                                             (void*)threadInfo, 0, &threadId);

      markerThreads.push_back(thread);
    }
  }

  // Метод для сигнализации о начале работы всем потокам marker
  void ThreadSynchronizer::signalStartAll() { SetEvent(startEvent); }

  // Метод ожидания сигналов о невозможности продолжения от всех потоков
  void ThreadSynchronizer::waitForAllThreadsBlocked() {
    WaitForSingleObject(mainEvent, INFINITE);
    // После сигнала сбрасываем событие для следующего использования
    ResetEvent(mainEvent);
  }

  // Метод для печати содержимого массива
  void ThreadSynchronizer::printArray() {
    EnterCriticalSection(&arrayCS);
    std::cout << "Текущее состояние массива: [";
    for (size_t i = 0; i < array.size(); ++i) {
      std::cout << array[i];
      if (i < array.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
    LeaveCriticalSection(&arrayCS);
  }

  // Метод для завершения работы указанного потока marker
  void ThreadSynchronizer::terminateMarkerThread(int threadNum) {
    if (threadNum < 1 || threadNum > static_cast<int>(markerInfos.size()) ||
        markerThreads[threadNum - 1] == NULL) {
      std::cout << "Некорректный номер потока или поток уже завершен!"
                << std::endl;
      return;
    }

    // Индекс в векторе на 1 меньше порядкового номера
    int idx = threadNum - 1;

    // Сигнализируем потоку о необходимости завершения
    EnterCriticalSection(&syncCS);
    markerInfos[idx]->shouldTerminate = true;
    markerInfos[idx]->canContinue = true;
    LeaveCriticalSection(&syncCS);

    SetEvent(markerInfos[idx]->continueEvent);

    // Дожидаемся завершения потока
    WaitForSingleObject(markerThreads[idx], INFINITE);
    CloseHandle(markerThreads[idx]);
    markerThreads[idx] = NULL;

    // Уменьшаем счетчик активных потоков
    activeThreads--;

    // Уменьшаем счетчик ожидающих потоков (если поток ожидал)
    if (markerInfos[idx]->waitingForSignal) {
      EnterCriticalSection(&syncCS);
      waitingThreadsCount--;
      LeaveCriticalSection(&syncCS);
    }
  }

  // Метод для сигнализации всем оставшимся потокам о продолжении работы
  void ThreadSynchronizer::continueRemainingThreads() {
    EnterCriticalSection(&syncCS);
    for (size_t i = 0; i < markerInfos.size(); ++i) {
      if (markerInfos[i] && markerThreads[i] != NULL) {
        markerInfos[i]->canContinue = true;
        SetEvent(markerInfos[i]->continueEvent);
      }
    }
    waitingThreadsCount = 0;
    LeaveCriticalSection(&syncCS);
  }

  // Проверка, остались ли активные потоки marker
  bool ThreadSynchronizer::hasActiveMarkerThreads() {
    return activeThreads > 0;
  }

  ThreadSynchronizer* ThreadSynchronizer::instance = nullptr;

  // Функция потока marker
  unsigned int ThreadSynchronizer::markerThreadWork(
      MarkerThreadInfo* threadInfo) {
    // Ожидаем сигнал от main для начала работы
    WaitForSingleObject(startEvent, INFINITE);

    // Инициализируем генератор случайных чисел
    srand((unsigned int)time(NULL) + threadInfo->id);

    // Счетчик помеченных элементов
    int markedCount = 0;

    // Основной цикл работы потока marker
    while (true) {
      // Генерируем случайный индекс
      int randomIndex = rand() % array.size();

      bool canMark = false;

      // Проверяем, можем ли пометить элемент
      EnterCriticalSection(&arrayCS);
      if (array[randomIndex] == 0) {
        canMark = true;
      }
      LeaveCriticalSection(&arrayCS);

      if (canMark) {
        // Можем пометить элемент
        Sleep(5);

        EnterCriticalSection(&arrayCS);
        array[randomIndex] = threadInfo->id;
        threadInfo->markedIndices.push_back(randomIndex);
        LeaveCriticalSection(&arrayCS);

        markedCount++;
        Sleep(5);
      } else {
        // Не можем пометить элемент
        EnterCriticalSection(&arrayCS);
        std::cout << "Поток " << threadInfo->id << ": помечено элементов - "
                  << markedCount << ", не могу пометить элемент с индексом "
                  << randomIndex << " (значение = " << array[randomIndex] << ")"
                  << std::endl;
        LeaveCriticalSection(&arrayCS);

        // Сигнализируем потоку main о невозможности продолжения работы
        EnterCriticalSection(&syncCS);
        waitingThreadsCount++;
        threadInfo->canContinue = false;
        threadInfo->waitingForSignal = true;

        // Если мы последний поток, уведомляем main
        if (waitingThreadsCount == activeThreads) {
          SetEvent(mainEvent);
        }
        LeaveCriticalSection(&syncCS);

        // Ждем сигнала от потока main
        WaitForSingleObject(threadInfo->continueEvent, INFINITE);

        EnterCriticalSection(&syncCS);
        threadInfo->waitingForSignal = false;
        LeaveCriticalSection(&syncCS);

        // Проверяем, нужно ли завершить работу
        EnterCriticalSection(&syncCS);
        bool shouldTerminate = threadInfo->shouldTerminate;
        LeaveCriticalSection(&syncCS);

        if (shouldTerminate) {
          // Заполняем нулями все помеченные элементы
          EnterCriticalSection(&arrayCS);
          for (size_t i = 0; i < threadInfo->markedIndices.size(); ++i) {
            array[threadInfo->markedIndices[i]] = 0;
          }
          LeaveCriticalSection(&arrayCS);

          std::cout << "Поток " << threadInfo->id
                    << " завершил работу, очистив "
                    << threadInfo->markedIndices.size() << " элементов"
                    << std::endl;
          return 0;
        }
      }
    }
    return 0;
  }

  std::vector<int> ThreadSynchronizer::getArraySnapshot() {
    EnterCriticalSection(&arrayCS);
    std::vector<int> copy = array;
    LeaveCriticalSection(&arrayCS);
    return copy;
  }