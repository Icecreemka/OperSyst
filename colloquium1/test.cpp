#include <gtest/gtest.h>

#include "fibonacci.h"
#include "list.h"
#include "palindrome.h"

TEST(FibonacciTest, GeneratesCorrectSequence) {
  auto result = generateFibonacci(5);
  std::vector<unsigned long long> expected = {0, 1, 1, 2, 3};
  EXPECT_EQ(result, expected);
}

TEST(PalindromeTest, IdentifiesPalindromes) {
  EXPECT_TRUE(isPalindrome(121));
  EXPECT_FALSE(isPalindrome(123));
}

TEST(LinkedListTest, ReversesList) {
  Node* head = new Node(1);
  head->next = new Node(2);
  head->next->next = new Node(3);
  head = reverseList(head);
  EXPECT_EQ(head->value, 3);
  EXPECT_EQ(head->next->value, 2);
  EXPECT_EQ(head->next->next->value, 1);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
