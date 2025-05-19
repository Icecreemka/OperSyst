#ifndef MARKER_H
#define MARKER_H

#include <windows.h>

#include <iostream>
#include <vector>

struct MarkerThreadInfo {
  int id;                          // ���������� ����� ������
  std::vector<int> markedIndices;  // ������� ���������, ���������� ���� �������
  bool canContinue;                // ���� ����������� ����������� ������
  bool shouldTerminate;            // ���� ��� ���������� ������ ������
  HANDLE continueEvent;            // ������� ��� ������� ����������� ������
  bool waitingForSignal;           // ���� �������� ������� �� main

  MarkerThreadInfo(int threadId);
  ~MarkerThreadInfo();

  // ��������� �����������
 private:
  MarkerThreadInfo(const MarkerThreadInfo&);
  MarkerThreadInfo& operator=(const MarkerThreadInfo&);
};

#endif  // MARKER_H
