#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>
#include <iostream> // Required for print_state()

/// The node used for the linked list implementation of a set in [`FineSet`].
/// This struct is used for task 4.
struct FineSetNode {
    int value;
    FineSetNode* next;
    std::mutex lock;

    FineSetNode(int val) : value(val), next(nullptr) {}
};

/// A set implementation using a linked list with fine-grained locking (hand-over-hand locking).
class FineSet : public Set {
private:
    FineSetNode* head;
    std::mutex head_lock; // Protects head pointer to avoid race conditions.
    EventMonitor<FineSet, StdSet, SetOperator>* monitor;

public:
    FineSet(EventMonitor<FineSet, StdSet, SetOperator>* monitor) :
        head(nullptr), monitor(monitor) {}

    ~FineSet() override {
        FineSetNode* current = head;
        while (current) {
            FineSetNode* temp = current;
            current = current->next;
            delete temp;
        }
    }

    bool add(int elem) override {
        std::unique_lock<std::mutex> head_lock(this->head_lock);

         // Check if the list is empty or if the new element should be inserted at the head
        if (!head || head->value > elem) {
            FineSetNode* new_node = new FineSetNode(elem);
            new_node->next = head;
            head = new_node;
            this->monitor->add(SetEvent(SetOperator::Add, elem, true));
            return true;
        }

        // Traverse the list with hand-over-hand locking
        FineSetNode* prev = head;
        prev->lock.lock();

        FineSetNode* curr = prev->next;
        if (curr) curr->lock.lock();

        // Release the head lock only after acquiring the next node lock
        head_lock.unlock();

        while (curr && curr->value < elem) {
            prev->lock.unlock();
            prev = curr;
            curr = curr->next;
            if (curr) curr->lock.lock();
        }

        // Check for duplicates
        if (curr && curr->value == elem) {
            // If the element already exists, release the locks and return false
        if (curr) curr->lock.unlock();
        prev->lock.unlock();
            this->monitor->add(SetEvent(SetOperator::Add, elem, false));
            return false;
        }

        // Insert the new node into the list
        FineSetNode* new_node = new FineSetNode(elem);
        new_node->next = curr;
        prev->next = new_node;

        // Release the locks 
        if (curr) curr->lock.unlock();
        prev->lock.unlock();
        
        this->monitor->add(SetEvent(SetOperator::Add, elem, true));
        return true;
    }

    bool rmv(int elem) override {
        std::unique_lock<std::mutex> head_lock(this->head_lock);

        // Check if the element to be removed is at the head
        if (head && head->value == elem) {
            FineSetNode* temp = head;
            head = head->next;
            head_lock.unlock();
            delete temp;
            this->monitor->add(SetEvent(SetOperator::Remove, elem, true));
            return true;
        }

        // If the list is empty, release the head lock and return false
        if (!head) {
            head_lock.unlock();
            this->monitor->add(SetEvent(SetOperator::Remove, elem, false));
            return false;
        }

        // Traverse the list with hand-over-hand locking
        FineSetNode* prev = head;
        prev->lock.lock();

        FineSetNode* curr = prev->next;
        if (curr) curr->lock.lock();

        // Release the head lock only after acquiring the next node lock
        head_lock.unlock();

        while (curr && curr->value < elem) {
            prev->lock.unlock();
            prev = curr;
            curr = curr->next;
            if (curr) curr->lock.lock();
        }

        // If the element is not found, release the locks and return false
        if (!curr || curr->value != elem) {
            if (curr) curr->lock.unlock();
            prev->lock.unlock();
            this->monitor->add(SetEvent(SetOperator::Remove, elem, false));
            return false;
        }

        // Remove the node from the list
        prev->next = curr->next;
        curr->lock.unlock();
        delete curr;
        prev->lock.unlock();

        this->monitor->add(SetEvent(SetOperator::Remove, elem, true));
        return true;
    }

    bool ctn(int elem) override {
        std::unique_lock<std::mutex> head_lock(this->head_lock);

        // If the list is empty, record the event and return false
        if (!head) {
            this->monitor->add(SetEvent(SetOperator::Contains, elem, false));
            return false;
        }

        // Traverse the list with hand-over-hand locking
        FineSetNode* curr = head;
        curr->lock.lock();

        // Release the head lock only after acquiring the next node lock
        head_lock.unlock();

        while (curr && curr->value < elem) {
            FineSetNode* next = curr->next;
            if (next) next->lock.lock();
            curr->lock.unlock();
            curr = next;
        }

        // Check if the element is found
        bool found = (curr && curr->value == elem);
        if (curr) curr->lock.unlock();

        this->monitor->add(SetEvent(SetOperator::Contains, elem, found));
        return found;
    }

    void print_state() override {
        std::unique_lock<std::mutex> head_lock(this->head_lock);
        std::cout << "FineSet { ";
        FineSetNode* curr = head;
        while (curr) {
            std::cout << curr->value << " ";
            curr = curr->next;
        }
        std::cout << "}\n";
    }
};
