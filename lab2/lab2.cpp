#include <windows.h>

#include <iostream>
#include <limits>
#include <vector>

struct ThreadData {
  std::vector<double> array;
  int min = INT_MAX;
  int max = INT_MIN;
  double average = 0.0;
};

DWORD WINAPI findMinMax(LPVOID param) {
  ThreadData* data = static_cast<ThreadData*>(param);
  for (double num : data->array) {
    if (num < data->min) data->min = num;
    if (num > data->max) data->max = num;
    Sleep(7);
  }
  std::cout << "Min: " << data->min << ", Max: " << data->max << std::endl;
  return 0;
}

DWORD WINAPI calculateAverage(LPVOID param) {
  ThreadData* data = static_cast<ThreadData*>(param);
  double sum = 0;
  for (double num : data->array) {
    sum += num;
    Sleep(12);
  }
  data->average = static_cast<double>(sum) / data->array.size();
  std::cout << "Average: " << data->average << std::endl;
  return 0;
}

int main() {
  int size;
  std::cout << "Enter array size: ";
  std::cin >> size;

  ThreadData data;
  data.array.resize(size);
  std::cout << "Enter " << size << " elements: ";
  for (int i = 0; i < size; ++i) {
    std::cin >> data.array[i];
  }

  HANDLE hMinMax = CreateThread(NULL, 0, findMinMax, &data, 0, NULL);
  HANDLE hAverage = CreateThread(NULL, 0, calculateAverage, &data, 0, NULL);

  WaitForSingleObject(hMinMax, INFINITE);
  WaitForSingleObject(hAverage, INFINITE);

  CloseHandle(hMinMax);
  CloseHandle(hAverage);

  for (double& num : data.array) {
    if (num == data.min || num == data.max) {
      num = static_cast<double>(data.average);
    }
  }

  std::cout << "Modified array: ";
  for (double num : data.array) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  return 0;
}
