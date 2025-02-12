#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

/// The node used for the linked list implementation of a set in the [`FineSet`]
/// class. This struct is used for task 4.
struct FineSetNode {
    // A04: You can add or remove fields as needed.
    int value;
    FineSetNode* next;
    std::mutex lock;
};

/// A set implementation using a linked list with fine grained locking.
class FineSet: public Set {
private:
    // A04: You can add or remove fields as needed. Just having the `head`
    // pointer should be sufficient for task 4
    FineSetNode* head;
    EventMonitor<FineSet, StdSet, SetOperator>* monitor;
public:
    FineSet(EventMonitor<FineSet, StdSet, SetOperator>* monitor) :
        monitor(monitor), head(new FineSetNode{0, nullptr, {}})
    {
        // A04: Initiate the internal state
    }

    ~FineSet() override {
        // A04: Cleanup any memory that was allocated
        FineSetNode* curr = head;
        while (curr != nullptr) {
            FineSetNode* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    bool add(int elem) override {
        std::unique_lock<std::mutex> head_lock(head->lock);
        FineSetNode* prev = head;
        FineSetNode* curr = head->next;

        while (curr != nullptr) {
            std::unique_lock<std::mutex> curr_lock(curr->lock);
            if (curr->value >= elem) {
                if (curr->value == elem) {
                    // Linearization point: element already exists
                    this->monitor->add(SetEvent(SetOperator::Add, elem, false));
                    return false;
                }
                FineSetNode* new_node = new FineSetNode{elem, curr, {}};
                prev->next = new_node;
                // Linearization point: new node inserted
                this->monitor->add(SetEvent(SetOperator::Add, elem, true));
                return true;
            }
            prev = curr;
            curr = curr->next;
        }

        FineSetNode* new_node = new FineSetNode{elem, nullptr, {}};
        prev->next = new_node;
        // Linearization point: new node inserted at the end
        this->monitor->add(SetEvent(SetOperator::Add, elem, true));
        return true;
    }

    bool rmv(int elem) override {
        std::unique_lock<std::mutex> head_lock(head->lock);
        FineSetNode* prev = head;
        FineSetNode* curr = head->next;

        while (curr != nullptr) {
            std::unique_lock<std::mutex> curr_lock(curr->lock);
            if (curr->value == elem) {
                prev->next = curr->next;
                delete curr;
                this->monitor->add(SetEvent(SetOperator::Remove, elem, true));
                return true;
            }
            if (curr->value > elem) {
                this->monitor->add(SetEvent(SetOperator::Remove, elem, false));
                return false;
            }
            prev = curr;
            curr = curr->next;
        }

        this->monitor->add(SetEvent(SetOperator::Remove, elem, false));
        return false;
    }

    bool ctn(int elem) override {
        std::unique_lock<std::mutex> head_lock(head->lock);
        FineSetNode* curr = head->next;

        while (curr != nullptr) {
            std::unique_lock<std::mutex> curr_lock(curr->lock);
            if (curr->value == elem) {
                this->monitor->add(SetEvent(SetOperator::Contains, elem, true));
                return true;
            }
            if (curr->value > elem) {
                this->monitor->add(SetEvent(SetOperator::Contains, elem, false));
                return false;
            }
            curr = curr->next;
        }

        this->monitor->add(SetEvent(SetOperator::Contains, elem, false));
        return false;
    }

    void print_state() override {
        // A04: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "FineSet {";
        FineSetNode* curr = head->next;
        while (curr != nullptr) {
            std::cout << curr->value;
            if (curr->next != nullptr) {
                std::cout << ", ";
            }
            curr = curr->next;
        }
        std::cout << "}" << std::endl;
    }
};

