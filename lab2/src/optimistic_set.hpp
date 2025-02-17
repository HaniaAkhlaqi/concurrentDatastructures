
#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>
#include <iostream>
#include <climits>

/// The node used for the linked list implementation of a set in the [`OptimisticSet`]
/// class. This struct is used for task 3
struct OptimisticSetNode {
    // A01: You can add or remove fields as needed.
    int value;
    OptimisticSetNode* next;
    std::mutex lock;

    OptimisticSetNode(int val) : value(val), next(nullptr) {}
};

/// A set implementation using a linked list with optimistic synchronization.
class OptimisticSet: public Set {
private:
    // A01: You can add or remove fields as needed. Just having the `head`
    // pointer should be sufficient for this task
    OptimisticSetNode* head;

public:
    OptimisticSet() : head(new OptimisticSetNode(INT_MIN)) {
        // A01: Initiate the internal state
        head->next = new OptimisticSetNode(INT_MAX);
    }

    ~OptimisticSet() override {
        // A01: Cleanup any memory that was allocated
        OptimisticSetNode* current = head;
        while (current != nullptr) {
            OptimisticSetNode* next = current->next;
            delete current;
            current = next;
        }
    }

private:
    bool validate(OptimisticSetNode* pred, OptimisticSetNode* curr) {
        // A01: Implement the `validate` function used during
        // optimistic synchronization.
        OptimisticSetNode* node = head;
        while (node != nullptr && node->value <= pred->value) {
            if (node == pred) {
                return pred->next == curr;
            }
            node = node->next;
        }
        return false;
    }

public:
    bool add(int elem) override {
        while (true) {
            OptimisticSetNode* pred = head;
            OptimisticSetNode* curr = head->next;
            while (curr->value < elem) {
                pred = curr;
                curr = curr->next;
            }
            std::lock_guard<std::mutex> predLock(pred->lock);
            std::lock_guard<std::mutex> currLock(curr->lock);
            if (validate(pred, curr)) {
                if (curr->value == elem) {
                    return false;
                } else {
                    OptimisticSetNode* newNode = new OptimisticSetNode(elem);
                    newNode->next = curr;
                    pred->next = newNode;
                    return true;
                }
            }
        }
    }

    bool rmv(int elem) override {
        while (true) {
            OptimisticSetNode* pred = head;
            OptimisticSetNode* curr = head->next;
            while (curr->value < elem) {
                pred = curr;
                curr = curr->next;
            }
            std::lock_guard<std::mutex> predLock(pred->lock);
            std::lock_guard<std::mutex> currLock(curr->lock);
            if (validate(pred, curr)) {
                if (curr->value != elem) {
                    return false;
                } else {
                    pred->next = curr->next;
                    delete curr;
                    return true;
                }
            }
        }
    }

    bool ctn(int elem) override {
        while (true) {
            OptimisticSetNode* pred = head;
            OptimisticSetNode* curr = head->next;
            while (curr->value < elem) {
                pred = curr;
                curr = curr->next;
            }
            std::lock_guard<std::mutex> predLock(pred->lock);
            std::lock_guard<std::mutex> currLock(curr->lock);
            if (validate(pred, curr)) {
                return curr->value == elem;
            }
        }
    }

    void print_state() override {
        // A01: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "OptimisticSet {...}";
    }
};
