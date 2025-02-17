#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

// A03: Copy your `FineSet` implementation from Lab 01 into this file and
// remove all references to the monitor. We want to benchmark the data
// structure and monitoring the performed operation would influence the
// results.

struct FineSetNode {
    int value;
    FineSetNode* next;
    std::mutex lock;
};
class FineSet: public Set {
private:
    FineSetNode* head;
public:
    FineSet() : head(new FineSetNode{0, nullptr, {}}) {
        // Initiate the internal state
    }

    ~FineSet() override {
        // Cleanup any memory that was allocated
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
                    return false;
                }
                FineSetNode* new_node = new FineSetNode{elem, curr, {}};
                prev->next = new_node;
                // Linearization point: new node inserted
                return true;
            }
            prev = curr;
            curr = curr->next;
        }

        FineSetNode* new_node = new FineSetNode{elem, nullptr, {}};
        prev->next = new_node;
        // Linearization point: new node inserted at the end
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
                // Linearization point: node removed
                return true;
            }
            if (curr->value > elem) {
                // Linearization point: element not found
                return false;
            }
            prev = curr;
            curr = curr->next;
        }

        // Linearization point: element not found
        return false;
    }

    bool ctn(int elem) override {
        std::unique_lock<std::mutex> head_lock(head->lock);
        FineSetNode* curr = head->next;

        while (curr != nullptr) {
            std::unique_lock<std::mutex> curr_lock(curr->lock);
            if (curr->value == elem) {
                // Linearization point: element found
                return true;
            }
            if (curr->value > elem) {
                // Linearization point: element not found
                return false;
            }
            curr = curr->next;
        }

        // Linearization point: element not found
        return false;
    }

    void print_state() override {
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

