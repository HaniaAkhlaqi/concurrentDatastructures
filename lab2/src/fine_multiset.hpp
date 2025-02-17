#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>
#include <iostream>

/// The node used for the linked list implementation of a multiset in the
/// [`FineMultiset`] class. This struct is used for task 4.
struct FineMultisetNode {
    // A05: You can add or remove fields as needed.
    int value;
    int count; //instead of traversing the list to find the count, we store it in the node
    FineMultisetNode* next;
    std::mutex lock;
    FineMultisetNode(int val) : value(val), count(1), next(nullptr) {}
};

/// A multiset implementation using a linked list with fine-grained locking.
class FineMultiset: public Multiset {
private:
    // A05: You can add or remove fields as needed.
    FineMultisetNode* head;
    std::mutex head_lock;
    EventMonitor<FineMultiset, StdMultiset, MultisetOperator>* monitor;

public:
    FineMultiset(EventMonitor<FineMultiset, StdMultiset, MultisetOperator>* monitor) :
        monitor(monitor), head(nullptr) {}

    ~FineMultiset() override {
        // A05: Cleanup any memory that was allocated
        FineMultisetNode* current = head;
        while (current) {
            FineMultisetNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    int add(int elem) override {
        // A05: Add code to insert the element into the set.
        //      Make sure, to insert the event inside the locked region of
        //      the linearization point.
        std::unique_lock<std::mutex> lock(head_lock);
        FineMultisetNode** curr = &head;
        while (*curr && (*curr)->value < elem) {
            curr = &((*curr)->next);
        }

        if (*curr && (*curr)->value == elem) {
            std::lock_guard<std::mutex> node_lock((*curr)->lock);
            (*curr)->count++;
        } else {
            FineMultisetNode* newNode = new FineMultisetNode(elem);
            newNode->next = *curr;
            *curr = newNode;
        }

        this->monitor->add(MultisetEvent(MultisetOperator::MSetAdd, elem, true));
        return true;
    }

    int rmv(int elem) override {
        // A05: Add code to remove the element from the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        std::unique_lock<std::mutex> lock(head_lock);
        FineMultisetNode** curr = &head;
        while (*curr && (*curr)->value < elem) {
            curr = &((*curr)->next);
        }

        if (*curr && (*curr)->value == elem) {
            std::lock_guard<std::mutex> node_lock((*curr)->lock);
            if ((*curr)->count > 1) {
                (*curr)->count--;
            } else {
                FineMultisetNode* temp = *curr;
                *curr = (*curr)->next;
                delete temp;
            }
            this->monitor->add(MultisetEvent(MultisetOperator::MSetRemove, elem, true));
            return true;
        }

        this->monitor->add(MultisetEvent(MultisetOperator::MSetRemove, elem, false));
        return false;
    }

    int ctn(int elem) override {
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
        std::unique_lock<std::mutex> lock(head_lock);
        FineMultisetNode* curr = head;
        while (curr && curr->value < elem) {
            curr = curr->next;
        }

        int result = (curr && curr->value == elem) ? curr->count : 0;
        this->monitor->add(MultisetEvent(MultisetOperator::MSetCount, elem, result));
        return result;
    }

    void print_state() override {
        // A05: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::unique_lock<std::mutex> lock(head_lock);
        FineMultisetNode* curr = head;
        std::cout << "FineMultiset: ";
        while (curr) {
            std::cout << "(" << curr->value << ", " << curr->count << ") ";
            curr = curr->next;
        }
        std::cout << std::endl;
    }
};
