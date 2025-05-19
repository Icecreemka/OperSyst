#ifndef MARKER_H
#define MARKER_H

#include <windows.h>

#include <iostream>
#include <vector>

struct MarkerThreadInfo {
  int id;                          // Порядковый номер потока
  std::vector<int> markedIndices;  // Индексы элементов, помеченных этим потоком
  bool canContinue;                // Флаг возможности продолжения работы
  bool shouldTerminate;            // Флаг для завершения работы потока
  HANDLE continueEvent;            // Событие для сигнала продолжения работы
  bool waitingForSignal;           // Флаг ожидания сигнала от main

  MarkerThreadInfo(int threadId);
  ~MarkerThreadInfo();

  // Запрещаем копирование
 private:
  MarkerThreadInfo(const MarkerThreadInfo&);
  MarkerThreadInfo& operator=(const MarkerThreadInfo&);
};

#endif  // MARKER_H
