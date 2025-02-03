#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a multiset in the
/// [`FineMultiset`] class. This struct is used for task 4.
struct FineMultisetNode {
    // A05: You can add or remove fields as needed.
    int value;
    FineMultisetNode* next;
    std::mutex lock;
};

/// A multiset implementation using a linked list with fine grained locking.
class FineMultiset: public Multiset {
private:
    // A05: You can add or remove fields as needed.
    FineMultisetNode* head;
    EventMonitor<FineMultiset, StdMultiset, MultisetOperator>* monitor;
public:
    FineMultiset(EventMonitor<FineMultiset, StdMultiset, MultisetOperator>* monitor) :
        monitor(monitor)
    {
        // A05: Initiate the internal state
    }

    ~FineMultiset() override {
        // A05: Cleanup any memory that was allocated
    }

    int add(int elem) override {
        int result = true;
        // A05: Add code to insert the element into the set.
        //      Make sure, to insert the event inside the locked region of
        //      the linearization point.
        this->monitor->add(MultisetEvent(MultisetOperator::MSetAdd, elem, result));
        return result;
    }

    int rmv(int elem) override {
        int result = false;
        // A05: Add code to remove the element from the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        this->monitor->add(MultisetEvent(MultisetOperator::MSetRemove, elem, result));
        return result;
    }

    int ctn(int elem) override {
        int result = 0;
        // A05: Add code to count how often elem is inside the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        //
        //      There are different ways to implement a multiset ADT. The 
        //      skeleton code provides `monitor->add()`, `monitor->reserve()`,
        //      and `event->complete()` functions for this purpose. One can
        //      use only `monitor->add() or a combination of `monitor->reserve()`
        //      and `event->complete()` depending on their multiset 
        //      implementation. Go to `monitoring.hpp` and see the descriptions
        //      of these function.
        return result;
    }

    void print_state() override {
        // A05: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "FineMultiset {...}";
    }
};

