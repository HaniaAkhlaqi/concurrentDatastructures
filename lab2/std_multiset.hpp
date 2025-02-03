#pragma once

#include "set.hpp"

class StdMultiset: public Multiset {
   public:
    int add(int value) override {
        this->state.insert(value);
        return true;
    }

    int rmv(int value) override {
        if (this->state.count(value) > 0) {
            this->state.erase(this->state.find(value));
            return true;
        }
        return false;
    }

    int ctn(int value) override {
        return this->state.count(value);
    }

    void print_state() override {
        std::cout << "{";
        bool first = true;
        for (auto it = this->state.begin(); it != this->state.end(); it++) {
            if (first) {
                first = false;
            } else {
                std::cout << ", ";
            }
            std::cout << *it;
        }
        std::cout << "}";
    }

   private:
    std::multiset<int> state;
};
