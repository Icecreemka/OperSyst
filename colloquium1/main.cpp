#include <iostream>

#include "fibonacci.h"
#include "list.h"
#include "palindrome.h"

void printFibonacci(size_t n) {
  try {
    auto fibonacci = generateFibonacci(n);
    std::cout << "Fibonacci sequence: ";
    for (const auto& num : fibonacci) {
      std::cout << num << " ";
    }
    std::cout << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void checkPalindrome(int number) {
  if (isPalindrome(number)) {
    std::cout << number << " is a palindrome." << std::endl;
  } else {
    std::cout << number << " is not a palindrome." << std::endl;
  }
}

void reverseLinkedList() {
  Node* head = nullptr;
  Node* tail = nullptr;
  int value;

  std::cout << "Enter numbers for linked list (end with -1): ";
  while (std::cin >> value && value != -1) {
    Node* newNode = new Node(value);
    if (!head)
      head = tail = newNode;
    else {
      tail->next = newNode;
      tail = newNode;
    }
  }

  head = reverseList(head);
  std::cout << "Reversed list: ";
  while (head) {
    std::cout << head->value << " ";
    Node* temp = head;
    head = head->next;
    delete temp;
  }
  std::cout << std::endl;
}

int main() {
  int choice;
  do {
    std::cout << "Menu:\n1. Generate Fibonacci\n2. Check Palindrome\n3. "
                 "Reverse Linked List\n4. Exit\nChoose: ";
    std::cin >> choice;
    switch (choice) {
      case 1: {
        size_t n;
        std::cout << "Enter n: ";
        std::cin >> n;
        printFibonacci(n);
        break;
      }
      case 2: {
        int number;
        std::cout << "Enter number: ";
        std::cin >> number;
        checkPalindrome(number);
        break;
      }
      case 3:
        reverseLinkedList();
        break;
      case 4:
        std::cout << "Exiting..." << std::endl;
        break;
      default:
        std::cout << "Invalid choice." << std::endl;
    }
  } while (choice != 4);
  return 0;
}
