#include <stdexcept>
#include <vector>
#include "fibonacci.h"

std::vector<unsigned long long> generateFibonacci(size_t n) {
  if (n == 0) throw std::invalid_argument("n must be a natural number.");
  std::vector<unsigned long long> fibonacci{0, 1};
  while (fibonacci.size() < n) {
    fibonacci.push_back(fibonacci[fibonacci.size() - 1] +
                        fibonacci[fibonacci.size() - 2]);
  }
  return {fibonacci.begin(), fibonacci.begin() + n};
}
