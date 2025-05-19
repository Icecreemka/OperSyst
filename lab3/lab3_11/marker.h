#include <condition_variable>
#include <iostream>
#include <vector>

// ��������� ��� �������� ���������� � ������ marker
struct MarkerThreadInfo {
  int id;                          // ���������� ����� ������
  std::vector<int> markedIndices;  // ������� ���������, ���������� ���� �������
  bool canContinue;                // ���� ����������� ����������� ������
  bool shouldTerminate;            // ���� ��� ���������� ������ ������
  std::condition_variable
      continueCV;         // �������� ���������� ��� ������� ����������� ������
  bool waitingForSignal;  // ���� �������� ������� �� main

  // ����������� � �����������
  MarkerThreadInfo(int threadId);
};