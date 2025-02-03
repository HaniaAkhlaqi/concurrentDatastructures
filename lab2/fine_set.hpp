#pragma once

#include "set.hpp"
#include "std_set.hpp"

#include <mutex>

// A03: Copy your `FineSet` implementation from Lab 01 into this file and
// remove all references to the monitor. We want to benchmark the data
// structure and monitoring the performed operation would influence the
// results.

class FineSet: public Set {
private:
public:
    bool add(int elem) override {
        bool result = false;
        return result;
    }

    bool rmv(int elem) override {
        bool result = false;
        return result;
    }

    bool ctn(int elem) override {
        bool result = false;
        return result;
    }

    void print_state() override {
        std::cout << "FineSet {...}";
    }
};

