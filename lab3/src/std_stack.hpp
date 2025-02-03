#pragma once

#include "adt.hpp"

#include <stack>

class StdStack: public Stack {
   public:
    int push(int value) override {
        state.push(value);
        return true;
    }

    int pop() override {
        if (state.empty()) {
            return EMPTY_STACK_VALUE;
        }
        int value = state.top();
        state.pop();
        return value;
    }

    int size() override {
        return state.size();
    }

    void print_state() override {
        std::cout << "{";
        bool first = true;
        int top = this->pop();
        while (top != EMPTY_STACK_VALUE) {
            if (first) {
                first = false;
            } else {
                std::cout << ", ";
            }
            std::cout << top;
            top = this->pop();
        }
        std::cout << "}";
    }

   private:
    std::stack<int> state;
};