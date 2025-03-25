#include "list.h"
Node* reverseList(Node* head) {
  Node* prev = nullptr;
  Node* current = head;
  while (current) {
    Node* nextTemp = current->next;
    current->next = prev;
    prev = current;
    current = nextTemp;
  }
  return prev;
}
