#include <algorithm>
#include <string>
#include "palindrome.h"

bool isPalindrome(int number) {
  std::string str = std::to_string(number);
  return std::equal(str.begin(), str.begin() + str.size() / 2, str.rbegin());
}
