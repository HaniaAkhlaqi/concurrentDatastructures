#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a set in the [`OptimisticSet`]
/// class. This struct is used for task 3
struct OptimisticSetNode {
    // A01: You can add or remove fields as needed.
    int value;
    OptimisticSetNode* next;
    std::mutex lock;
};

/// A set implementation using a linked list with optimistic syncronization.
class OptimisticSet: public Set {
private:
    // A01: You can add or remove fields as needed. Just having the `head`
    // pointer should be sufficient for this task
    OptimisticSetNode* head;
public:
    OptimisticSet()
    {
        // A01: Initiate the internal state
    }

    ~OptimisticSet() override {
        // A01: Cleanup any memory that was allocated
    }

private:
    bool validate(OptimisticSetNode* p, OptimisticSetNode* c) {
        // A01: Implement the `validate` function used during
        // optimistic synchronization.
        return false;
    }

public:
    bool add(int elem) override {
        bool result = false;
        // A01: Add code to insert the element into the set and update `result`.
        return result;
    }

    bool rmv(int elem) override {
        bool result = false;
        // A01: Add code to remove the element from the set and update `result`.
        return result;
    }

    bool ctn(int elem) override {
        bool result = false;
        // A01: Add code to check if the element is inside the set and update `result`.
        return result;
    }

    void print_state() override {
        // A01: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "OptimisticSet {...}";
    }
};

