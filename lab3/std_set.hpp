#pragma once

#include "adt.hpp"

#include <set>

class StdSet: public Set {
   public:
    bool add(int elem) override {
        return this->state.insert(elem).second;
    }

    bool rmv(int elem) override {
        return this->state.erase(elem);
    }

    bool ctn(int elem) override {
        return this->state.find(elem) != this->state.end();
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
    std::set<int> state;
};
