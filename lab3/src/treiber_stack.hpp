#pragma once

#include "adt.hpp"

struct TreiberStackNode {
    // A01: You can add or remove fields as needed.
    int value;
    TreiberStackNode* next = nullptr;
};

class TreiberStack: public Stack {
private:
    // A04: You can add or remove fields as needed.
    std::atomic<TreiberStackNode*> top;

    EventMonitor<TreiberStack, StdStack, StackOperator>* monitor;
    /// This lock can be used around the CAS operation, to insert the
    /// operation into the monitor at the linearization point. This is
    /// just one way to do it, you can also try alternative options.
    std::mutex cas_lock;
public:
    TreiberStack(EventMonitor<TreiberStack, StdStack, StackOperator>* monitor)
        : monitor(monitor)
    {
        // A01: Initiate the internal state
    }

    ~TreiberStack() {
        // A01: Cleanup any memory that was allocated
    }

    int push(int value) override {
        int result = true;
        // A01: Add code to insert the element at the top of the stack.
        //      Make sure, to insert the event at the linearization point.
        //      You can use the `cas_lock` to ensure that the event is
        //      inserted at the linearization point.
        this->monitor->add(StackEvent(StackOperator::StackPush, value, result));
        return result;
    }

    int pop() override {
        int result = EMPTY_STACK_VALUE;
        // A01: Add code to pop the element at the top of the stack.
        //      Make sure, to insert the event at the linearization point.
        this->monitor->add(StackEvent(StackOperator::StackPop, NO_ARGUMENT_VALUE, result));
        return result;
    }

    int size() override {
        int result = 0;
        // A01: Add code to get the size of the stack.
        //      Make sure, to insert the event at the linearization point.
        this->monitor->add(StackEvent(StackOperator::StackPop, NO_ARGUMENT_VALUE, result));
        return result;
    }

    void print_state() override {
        std::cout << "TreiberStack { ... }";
    }
};