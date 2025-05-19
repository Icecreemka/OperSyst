#include <condition_variable>
#include <iostream>
#include <vector>

// Структура для хранения информации о потоке marker
struct MarkerThreadInfo {
  int id;                          // Порядковый номер потока
  std::vector<int> markedIndices;  // Индексы элементов, помеченных этим потоком
  bool canContinue;                // Флаг возможности продолжения работы
  bool shouldTerminate;            // Флаг для завершения работы потока
  std::condition_variable
      continueCV;         // Условная переменная для сигнала продолжения работы
  bool waitingForSignal;  // Флаг ожидания сигнала от main

  // Конструктор с параметрами
  MarkerThreadInfo(int threadId);
};