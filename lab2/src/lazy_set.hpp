#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>
#include <iostream>
#include <climits>


//lab 2 task 4 seg fault maybe change all constructors like in task 3


/// The node used for the linked list implementation of a set in the [`LazySet`]
/// class. This struct is used for task 3
struct LazySetNode {
    // A02: You can add or remove fields as needed.
    int value;
    bool marked;
    LazySetNode* next;
    std::mutex lock;

    LazySetNode(int val) : value(val), marked(false), next(nullptr) {}
};

/// A set implementation using a linked list with optimistic syncronization.
class LazySet: public Set {
private:
    // A02: You can add or remove fields as needed. Just having the `head`
    // pointer should be sufficient for this task
    LazySetNode* head;
public:
    LazySet() {
        // A02: Initiate the internal state
        head = new LazySetNode(INT_MIN); // Sentinel node
        head->next = new LazySetNode(INT_MAX); // Sentinel node
    }

    ~LazySet() override {
        // A02: Cleanup any memory that was allocated
        LazySetNode* current = head;
        while (current != nullptr) {
            LazySetNode* next = current->next;
            delete current;
            current = next;
        }
    }

private:
    LazySetNode* locate(int value, LazySetNode** pred) {
        LazySetNode* curr = head;
        *pred = nullptr;
        while (curr->value < value) {
            *pred = curr;
            curr = curr->next;
        }
        return curr;
    }

public:
    bool add(int elem) override {
        while (true) {
            LazySetNode* pred;
            LazySetNode* curr = locate(elem, &pred);

            std::lock_guard<std::mutex> pred_lock(pred->lock);
            std::lock_guard<std::mutex> curr_lock(curr->lock);

            if (validate(pred, curr)) {
                if (curr->value == elem) {
                    return false;
                } else {
                    LazySetNode* newNode = new LazySetNode(elem);
                    newNode->next = curr;
                    pred->next = newNode;
                    return true;
                }
            }
        }
    }

    bool rmv(int elem) override {
        while (true) {
            LazySetNode* pred;
            LazySetNode* curr = locate(elem, &pred);

            std::lock_guard<std::mutex> pred_lock(pred->lock);
            std::lock_guard<std::mutex> curr_lock(curr->lock);

            if (validate(pred, curr)) {
                if (curr->value != elem) {
                    return false;
                } else {
                    curr->marked = true;
                    pred->next = curr->next;
                    delete curr;
                    return true;
                }
            }
        }
    }

    bool ctn(int elem) override {
        LazySetNode* curr = head;
        while (curr->value < elem) {
            curr = curr->next;
        }
        return (curr->value == elem && !curr->marked);
    }

    void print_state() override {
        // A02: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        LazySetNode* curr = head;
        while (curr != nullptr) {
            std::cout << curr->value << (curr->marked ? " (marked)" : "") << " -> ";
            curr = curr->next;
        }
        std::cout << "nullptr" << std::endl;
    }

private:
    bool validate(LazySetNode* pred, LazySetNode* curr) {
        return !pred->marked && !curr->marked && pred->next == curr;
    }
};

