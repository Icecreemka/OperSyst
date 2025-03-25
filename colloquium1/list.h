#ifndef LIST_H
#define LIST_H

struct Node {
  int value;
  Node* next;
  Node(int v) : value(v), next(nullptr) {}
};

Node* reverseList(Node* head);

#endif LIST_H
