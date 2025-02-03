#pragma once

#include <queue>
#include <optional>
#include <iostream>
#include <mutex>
#include <cassert>
#include <chrono>
#include <thread>
#include <atomic>

#include "set.hpp"

#define DEQUE_SLEEP_DELAY 50

enum SetOperator {
    Add = 1,
    Remove = 2,
    Contains = 3,
};

char const* operator_name(SetOperator op) {
    switch (op) {
        case SetOperator::Add:
            return "add";
        case SetOperator::Remove:
            return "rmv";
        case SetOperator::Contains:
            return "ctn";
        default:
            return "[UNKNOWN]";
    }
}

template<typename Op>
struct Operation {
    /// The operator that this operation used.
    Op op;
    /// The argument for the operator.
    int argument;

    Operation(Op op, int argument) :
        op(op),
        argument(argument)
    {}

    void print() {
        std::cout << operator_name(this->op) << "(" << this->argument << ")";
    }
};
typedef Operation<SetOperator> SetOperation;

bool apply_op(Set* set, Operation<SetOperator>& op) {
    switch (op.op) {
        case SetOperator::Add:
            return set->add(op.argument);
        case SetOperator::Remove:
            return set->rmv(op.argument);
        case SetOperator::Contains:
            return set->ctn(op.argument);
        default:
            return false;
    }
}

/// A struct defining the weigh of an operator for the [`OpGenerator`] class.
template<typename Op>
struct OpWeights {
    /// The operator which might be generated
    Op op;
    /// The argument for the operator.
    int weight;
};

template<typename Op>
class OpGenerator {
    int target_op_count;
    /// The amount of operations, which have been generated. This is an atomic
    /// integer, to share the count across all threads.
    std::atomic_int counter;
    /// The weight of the operators, that can be generated.
    std::vector<OpWeights<Op>> weights;
    /// The total weight used as a modulo during generation.
    int total_weight;
    /// A modulo applied to the number generated as the argument for the operations.
    int argument_modulo;
    /// A lock around consecutive `rand()` calls, to make sure they
    /// generate the same pair of values.
    std::mutex rand_lock;
    int seed;

public:
    OpGenerator(
        std::vector<OpWeights<Op>> weights,
        int target_op_count,
        int argument_modulo,
        int seed
    ) :
        target_op_count(target_op_count),
        weights(weights),
        argument_modulo(argument_modulo),
        seed(seed)
    {
        total_weight = 0;
        for (int i = 0; i < (int)this->weights.size(); i++) {
            total_weight += this->weights[i].weight;
        }

        this->reset();
    }

    void reset() {
        this->counter = 0;
        srand(this->seed);
    }

    std::optional<Operation<Op>> next() {
        if (this->counter.fetch_add(1) >= target_op_count) {
            return std::nullopt;
        }

        // Rand generation
        this->rand_lock.lock();
        int op_selection = rand() % this->total_weight;
        int argument = rand() % argument_modulo;
        this->rand_lock.unlock();

        // Select operator
        int weight_index = 0;
        while (op_selection > this->weights[weight_index].weight) {
            op_selection -= this->weights[weight_index].weight;
            weight_index += 1;
        }
        Op op = this->weights[weight_index].op;

        return Operation<Op>(op, argument);
    }
};

template<typename Op>
struct Event {
    /// The operator that this event used.
    Operation<Op> op;
    /// The result of the performed operation.
    bool output;

    Event(Op op, int arg, bool output) :
        op(op, arg),
        output(output)
    {}

    void print() {
        this->op.print();
        std::cout << " -> " << this->output;
    }
};
typedef Event<SetOperator> SetEvent;

/// Returns `true` if the given event can be successfully applied to the given data structure
template<typename DS, typename Op>
bool test_event(DS* data_structure, Event<Op>& event) {
    bool result = apply_op(data_structure, event.op);
    if (result != event.output) {
        std::cout << "- ";
        event.print();
        std::cout << " is invalid, since " << result << " was returned." << std::endl;
        std::cout << "  - Test State: ";
        data_structure->print_state();
        std::cout << std::endl;

        return false;
    }
    return true;
}

/// Returns `true` if the given events can be successfully applied to the given data structure
template<typename DS, typename Op>
bool test_events(DS* data_structure, std::queue<Event<Op>>* events, bool verbose = false) {
    while (!events->empty()) {
        if (!test_event(data_structure, events->front())) {
            // Any errors are printed by `test_event`
            return false;
        }

        if (verbose) {
            std::cout << "- ";
            events->front().print();
            std::cout << std::endl;
        }

        events->pop();
    }

    return true;
}

/// This class uses coarse grained locking, because this is the simplest thing
/// to implement. During the course you'll learn about more efficient algorithms
/// for concurrent data structures.
template<typename CAS, typename DS, typename Op>
class EventMonitor {
public:
    EventMonitor(DS* data_structure) :
        data_structure(data_structure),
        concurrent_data_structure(nullptr)
    {
    }

    void add(Event<Op> event) {
        this->lock.lock(); // Linearization point (For anyone that is interested)
        this->events_to_test.push(event);
        this->lock.unlock();
    }

    void finish() {
        this->stop = true;
    }

    bool monitor() {
        bool running = true;
        while (running) {
            // `running` and `this->finish` are separate flags, to ensure that
            // we deque one last time, after all other threads have finished
            if (this->stop) {
                running = false;
            }

            std::queue<Event<Op>> events_to_test;
            this->lock.lock(); // Linearization point
            this->events_to_test.swap(events_to_test);
            this->lock.unlock();

            if (events_to_test.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(DEQUE_SLEEP_DELAY));
            } else {
                this->event_count += events_to_test.size();
                this->valid &= test_events(this->data_structure, &events_to_test, false);

                if (!this->valid) {
                    if (this->concurrent_data_structure) {
                        std::cout << "  - Concurrent State: ";
                        this->concurrent_data_structure->print_state();
                        std::cout << std::endl;
                    }
                    this->event_count -= events_to_test.size();
                    break;
                }
            }
        }

        if (this->valid) {
            std::cout << "Successfully validated " << this->event_count << " events" << std::endl;
        } else {
            std::cout << "Validation failed after " << this->event_count << " events" << std::endl;
        }

        return this->valid;
    }

    bool is_valid() {
        return this->valid;
    }

    void set_concurrent_data_structure(CAS* cas) {
        this->concurrent_data_structure = cas;
    }

private:
    std::queue<Event<Op>> events_to_test;
    std::mutex lock;

    // For monitoring and validation
    DS* data_structure;
    int event_count = 0;
    bool stop = false;
    bool valid = true;

    /// The concurrent data structure, to print the internal state:
    CAS* concurrent_data_structure;
};
