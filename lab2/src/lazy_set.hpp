#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a set in the [`LazySet`]
/// class. This struct is used for task 3
struct LazySetNode {
    // A02: You can add or remove fields as needed.
    int value;
    int mark;
    LazySetNode* next;
    std::mutex lock;
};

/// A set implementation using a linked list with optimistic syncronization.
class LazySet: public Set {
private:
    // A02: You can add or remove fields as needed. Just having the `head`
    // pointer should be sufficient for this task
    LazySetNode* head;
public:
    LazySet()
    {
        // A02: Initiate the internal state
    }

    ~LazySet() override {
        // A02: Cleanup any memory that was allocated
    }

private:
    LazySetNode* locate(int value) {
        // A02: Implement the `locate` function used for lazy synchronization.
        return nullptr;
    }

public:
    bool add(int elem) override {
        bool result = false;
        // A02: Add code to insert the element into the set and update `result`.
        return result;
    }

    bool rmv(int elem) override {
        bool result = false;
        // A02: Add code to remove the element from the set and update `result`.
        return result;
    }

    bool ctn(int elem) override {
        bool result = false;
        // A02: Add code to check if the element is inside the set and update `result`.
        return result;
    }

    void print_state() override {
        // A02: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "LazySet {...}";
    }
};

