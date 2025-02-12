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
    CoarseSetNode(int val) : value(val), next(nullptr) {}
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
        head(nullptr),monitor(monitor)
    {
        // A03: Initiate the internal state

    }

    ~CoarseSet() override {
        // A03: Cleanup any memory that was allocated
        std::lock_guard<std::mutex> gaurd(lock);  // LINEARIZATION POINT
        CoarseSetNode* current = head;
        while (current) {
            CoarseSetNode* temp = current;
            current = current->next;
            delete temp;
        }
        // gaurd lock is automatically released when lock goes out of scope    
    }

    bool add(int elem) override {
        // A03: Add code to insert the element into the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        std::lock_guard<std::mutex> guard(lock); // LINEARIZATION POINT
        // Check if element already exists
        bool result = false;
        CoarseSetNode* current = head;
        while (current) {
            if (current->value == elem) {
                monitor->add(SetEvent(SetOperator::Add, elem, false)); 
                result = false;
                return result;
            }
            current = current->next;
        }
        // Inserting at the head of the list
        CoarseSetNode* new_node = new CoarseSetNode(elem);
        new_node->next = head;
        head = new_node;

        this->monitor->add(SetEvent(SetOperator::Add, elem, true)); 
        result = true;
        return result;
        // gaurd lock is automatically released when lock goes out of scope
    }

    bool rmv(int elem) override {
        std::lock_guard<std::mutex> guard(lock); // LINEARIZATION POINT
        // A03: Add code to remove the element from the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        bool result = false;
        CoarseSetNode* current = head;
        CoarseSetNode* prev = nullptr;

        while (current) {
            if (current->value == elem) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    head = current->next;
                }
                delete current;
                monitor->add(SetEvent(SetOperator::Remove, elem, true)); 
                result = true;
                return result;
            }
            prev = current;
            current = current->next;
        }

        this->monitor->add(SetEvent(SetOperator::Remove, elem, false)); 
        result = false;
        return result;
        // gaurd lock is automatically released when lock goes out of scope
    }

    bool ctn(int elem) override {
        std::lock_guard<std::mutex> guard(lock); // LINEARIZATION POINT
        // A03: Add code to check if the element is inside the set and update `result`.
        //      Also make sure, to insert the event inside the locked region of
        //      the linearization point.
        bool result = false;
        CoarseSetNode* current = head;
        while (current) {
            if (current->value == elem) {
                monitor->add(SetEvent(SetOperator::Contains, elem, true)); 
                result = true;
                return result;
            }
            current = current->next;
        }
        this->monitor->add(SetEvent(SetOperator::Contains, elem, false)); 
        result = false;
        return result;
        // gaurd lock is automatically released when lock goes out of scope
    }

    void print_state() override {
        // A03: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "CoarseSet {...}";
    }
};

