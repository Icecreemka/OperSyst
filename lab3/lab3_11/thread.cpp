#include "thread.h"

  ThreadSynchronizer::ThreadSynchronizer(int arraySize)
      : array(arraySize, 0),
        waitingThreadsCount(0),
        startFlag(false),
        activeThreads(0) {}

  // Деструктор для корректного завершения потоков
  ThreadSynchronizer::~ThreadSynchronizer() {
    // Завершаем все оставшиеся потоки
    for (size_t i = 0; i < markerThreads.size(); ++i) {
      if (markerThreads[i] && markerThreads[i]->joinable()) {
        markerThreads[i]->join();
      }
    }
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
      auto threadInfo = std::make_unique<MarkerThreadInfo>(i + 1);

      // Сохраняем указатель на информацию о потоке
      MarkerThreadInfo* infoPtr = threadInfo.get();
      markerInfos.push_back(std::move(threadInfo));

      // Создаем поток marker
      auto thread = std::make_unique<std::thread>(
          &ThreadSynchronizer::markerThreadFunction, this, infoPtr);
      markerThreads.push_back(std::move(thread));
    }
  }

  // Метод для сигнализации о начале работы всем потокам marker
  void ThreadSynchronizer::signalStartAll() {
    {
      std::lock_guard<std::mutex> lock(syncMutex);
      startFlag = true;
    }
    startCV.notify_all();
  }

  // Метод ожидания сигналов о невозможности продолжения от всех потоков
  void ThreadSynchronizer::waitForAllThreadsBlocked() {
    std::unique_lock<std::mutex> lock(syncMutex);
    mainCV.wait(
        lock, [this]() { return waitingThreadsCount.load() == activeThreads; });
  }

  // Метод для печати содержимого массива
  void ThreadSynchronizer::printArray() {
    std::lock_guard<std::mutex> lock(arrayMutex);
    std::cout << "Текущее состояние массива: [";
    for (size_t i = 0; i < array.size(); ++i) {
      std::cout << array[i];
      if (i < array.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << "]" << std::endl;
  }

  // Метод для завершения работы указанного потока marker
  void ThreadSynchronizer::terminateMarkerThread(int threadNum) {
    if (threadNum < 1 || threadNum > static_cast<int>(markerInfos.size()) ||
        !markerThreads[threadNum - 1]) {
      std::cout << "Некорректный номер потока или поток уже завершен!"
                << std::endl;
      return;
    }

    // Индекс в векторе на 1 меньше порядкового номера
    int idx = threadNum - 1;

    // Сигнализируем потоку о необходимости завершения
    {
      std::lock_guard<std::mutex> lock(syncMutex);
      markerInfos[idx]->shouldTerminate = true;
      markerInfos[idx]->canContinue = true;
    }
    markerInfos[idx]->continueCV.notify_one();

    // Дожидаемся завершения потока
    if (markerThreads[idx]->joinable()) {
      markerThreads[idx]->join();
      markerThreads[idx].reset();  // Освобождаем ресурсы потока
    }

    // Уменьшаем счетчик активных потоков
    activeThreads--;

    // Уменьшаем счетчик ожидающих потоков (если поток ожидал)
    if (markerInfos[idx]->waitingForSignal) {
      waitingThreadsCount.fetch_sub(1);
    }
  }

  // Метод для сигнализации всем оставшимся потокам о продолжении работы
  void ThreadSynchronizer::continueRemainingThreads() {
    {
      std::lock_guard<std::mutex> lock(syncMutex);
      for (auto& threadInfo : markerInfos) {
        if (threadInfo && !threadInfo->shouldTerminate) {
          threadInfo->canContinue = true;
        }
      }
    }

    // Отправляем сигнал всем ожидающим потокам
    for (auto& threadInfo : markerInfos) {
      if (threadInfo && !threadInfo->shouldTerminate) {
        threadInfo->continueCV.notify_one();
      }
    }

    // Сбрасываем счетчик ожидающих потоков
    waitingThreadsCount.store(0);
  }

  // Проверка, остались ли активные потоки marker
  bool ThreadSynchronizer::hasActiveMarkerThreads() {
    return activeThreads > 0;
  }

  // Функция потока marker
  void ThreadSynchronizer::markerThreadFunction(MarkerThreadInfo* threadInfo) {
    // Ожидаем сигнал от main для начала работы
    {
      std::unique_lock<std::mutex> lock(syncMutex);
      startCV.wait(lock, [this]() { return startFlag; });
    }

    // Инициализируем генератор случайных чисел
    std::mt19937 rng(static_cast<unsigned int>(threadInfo->id));
    std::uniform_int_distribution<int> dist(0,
                                            static_cast<int>(array.size() - 1));

    // Счетчик помеченных элементов
    int markedCount = 0;

    // Основной цикл работы потока marker
    while (true) {
      // Генерируем случайный индекс
      int randomIndex = dist(rng);

      bool canMark = false;

      // Проверяем, можем ли пометить элемент
      {
        std::lock_guard<std::mutex> lock(arrayMutex);
        if (array[randomIndex] == 0) {
          canMark = true;
        }
      }

      if (canMark) {
        // Можем пометить элемент
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        {
          std::lock_guard<std::mutex> lock(arrayMutex);
          array[randomIndex] = threadInfo->id;
          threadInfo->markedIndices.push_back(randomIndex);
        }

        markedCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
      } else {
        // Не можем пометить элемент
        {
          std::lock_guard<std::mutex> lock(arrayMutex);
          std::cout << "Поток " << threadInfo->id << ": помечено элементов - "
                    << markedCount << ", не могу пометить элемент с индексом "
                    << randomIndex << " (значение = " << array[randomIndex]
                    << ")" << std::endl;
        }

        // Сигнализируем потоку main о невозможности продолжения работы
        {
          std::unique_lock<std::mutex> lock(syncMutex);
          waitingThreadsCount.fetch_add(1);
          threadInfo->canContinue = false;
          threadInfo->waitingForSignal = true;
        }

        // Если мы последний поток, уведомляем main
        if (waitingThreadsCount.load() == activeThreads) {
          mainCV.notify_one();
        }

        // Ждем сигнала от потока main
        {
          std::unique_lock<std::mutex> lock(syncMutex);
          threadInfo->continueCV.wait(
              lock, [threadInfo]() { return threadInfo->canContinue; });
          threadInfo->waitingForSignal = false;
        }

        // Проверяем, нужно ли завершить работу
        if (threadInfo->shouldTerminate) {
          // Заполняем нулями все помеченные элементы
          {
            std::lock_guard<std::mutex> lock(arrayMutex);
            for (int index : threadInfo->markedIndices) {
              array[index] = 0;
            }
          }

          std::cout << "Поток " << threadInfo->id
                    << " завершил работу, очистив "
                    << threadInfo->markedIndices.size() << " элементов"
                    << std::endl;
          return;
        }
      }
    }
  }
