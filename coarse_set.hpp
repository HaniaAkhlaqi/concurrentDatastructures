#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a set in the [`CoarseSet`]
/// class. This struct is used for task 3
struct CoarseSetNode {
    // A03: You can add or remove fields as needed.
    int value;
    CoarseSetNode* next;
};

/// A set implementation using a linked list with coarse grained locking.
class CoarseSet: public Set {
private:
    // A03: You can add or remove fields as needed. Just having the `head`
    // pointer and the `lock` should be sufficient for task 3
    CoarseSetNode* head;
    std::mutex lock;
    EventMonitor<CoarseSet, StdSet, SetOperator>* monitor;
public:
    CoarseSet(EventMonitor<CoarseSet, StdSet, SetOperator>* monitor) :
        monitor(monitor)
    {
        // A03: Initiate the internal state
    }

    ~CoarseSet() override {
        // A03: Cleanup any memory that was allocated
    }

    bool add(int elem) override {
        bool result = false;
        // A03: Add code to insert the element into the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        this->monitor->add(SetEvent(SetOperator::Add, elem, result));
        return result;
    }

    bool rmv(int elem) override {
        bool result = false;
        // A03: Add code to remove the element from the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        this->monitor->add(SetEvent(SetOperator::Remove, elem, result));
        return result;
    }

    bool ctn(int elem) override {
        bool result = false;
        // A03: Add code to check if the element is inside the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        this->monitor->add(SetEvent(SetOperator::Contains, elem, result));
        return result;
    }

    void print_state() override {
        // A03: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "CoarseSet {...}";
    }
};

