#include "monitoring.hpp"
#include "std_set.hpp"
#include "simple_set.hpp"
#include "coarse_set.hpp"
#include "fine_set.hpp"

#include <stdio.h>
#include <cstring>
#include <iostream>

#define OPERATION_COUNT 200
#define DEFAULT_GENERATOR_SEED 0
#define DEFAULT_OP_MOD 128

const std::vector<OpWeights<SetOperator>> DEFAULT_SET_GEN_WEIGHTS = {
    OpWeights<SetOperator> {op: SetOperator::Add, weight: 4},
    OpWeights<SetOperator> {op: SetOperator::Remove, weight: 3},
    OpWeights<SetOperator> {op: SetOperator::Contains, weight: 3},
};

template <class CDS, typename Op>
void worker_thread_func(CDS* data_structure, OpGenerator<Op>* generator, int thread_id) {
    while (auto maybe_operation = generator->next()) {
        SetOperation operation = maybe_operation.value();
        apply_op(data_structure, operation);
    }
}

template <class CDS, class DS, typename Op>
void monitor_thread_func(EventMonitor<CDS, DS, Op>* monitor) {
    monitor->monitor();
}

template <typename CDS, typename DS, typename Op>
bool test_data_structure_n_threads(
    CDS* concurrent_data_structure,
    OpGenerator<Op>* generator,
    EventMonitor<CDS, DS, Op>* monitor,
    int thread_count
) {
    // Setup threads
    void *ptr = malloc(sizeof(std::thread) * thread_count);
    memset(ptr, 0, sizeof(std::thread) * thread_count);
    std::thread *workers = (std::thread *)ptr;
    for (int thread_id = 0; thread_id < thread_count; thread_id++) {
        workers[thread_id] = std::thread(
            worker_thread_func<CDS, Op>,
            std::ref(concurrent_data_structure),
            std::ref(generator),
            std::ref(thread_id)
        );
    }

    // Start monitor thread
    std::thread monitor_thread(monitor_thread_func<CDS, DS, Op>, std::ref(monitor));

    // Join threads
    for (int i = 0; i < thread_count; i++) {
        workers[i].join();
    }

    // Finish the monitor and monitor thread
    monitor->finish();
    monitor_thread.join();

    // Cleanup
    free(workers);

    return monitor->is_valid();
}

template <typename Set>
bool test_set_n_threads(int thread_count, int op_arg_mod) {
    StdSet test_set;
    EventMonitor<Set, StdSet, SetOperator> monitor(&test_set);
    OpGenerator<SetOperator> generator(DEFAULT_SET_GEN_WEIGHTS, OPERATION_COUNT, op_arg_mod, DEFAULT_GENERATOR_SEED);
    Set set(&monitor);
    monitor.set_concurrent_data_structure(&set);

    return test_data_structure_n_threads<Set, StdSet, SetOperator>(&set, &generator, &monitor, thread_count);
}

int task_1() {
    std::cout << "# Task 1" << std::endl;

    {
        std::cout << "## Test 1" << std::endl;
        std::queue<SetEvent> valid;
        valid.push(SetEvent(SetOperator::Add, 1, true));
        valid.push(SetEvent(SetOperator::Add, 1, false));
        valid.push(SetEvent(SetOperator::Remove, 1, true));
        StdSet set;
        test_events(&set, &valid, true);
        std::cout << std::endl;
    }

    {
        std::cout << "## Test 2" << std::endl;
        std::queue<SetEvent> invalid_1;
        invalid_1.push(SetEvent(SetOperator::Add, 1, true));
        invalid_1.push(SetEvent(SetOperator::Add, 2, false));
        StdSet set;
        test_events(&set, &invalid_1, true);
        std::cout << std::endl;
    }

    {
        std::cout << "## Test 3" << std::endl;
        std::queue<SetEvent> invalid_2;
        invalid_2.push(SetEvent(SetOperator::Add, 1, true));
        invalid_2.push(SetEvent(SetOperator::Contains, 1, true));
        invalid_2.push(SetEvent(SetOperator::Remove, 2, true));
        StdSet set;
        test_events(&set, &invalid_2, true);
        std::cout << std::endl;
    }

    // A01: Add a valid sequence of 10+ instructions

    return 0;
}

// A02: This function tests your implementation of the `SimpleSet` class in `simple_set.hpp`
int task_2() {
    bool valid = true;
    std::cout << "# Task 2: Simple Set" << std::endl;

    std::cout << "## Testing `SimpleSet` with 1 thread" << std::endl;
    valid &= test_set_n_threads<SimpleSet>(1, DEFAULT_OP_MOD);
    std::cout << std::endl;

    std::cout << "## Testing `SimpleSet` with 4 threads" << std::endl;
    valid &= test_set_n_threads<SimpleSet>(4, DEFAULT_OP_MOD);
    std::cout << std::endl;

    if (valid) {
        return 0;
    } else {
        return -1;
    }
}

/// A03: This function tests your implementation of the `CoarseSet` class in `coarse_set.hpp`
int task_3() {
    bool valid = true;
    std::cout << "# Task 3: Coarse Set" << std::endl;

    for (int test_run = 0; test_run < 8; test_run++) {
        std::cout << "## Testing `CoarseSet` with 4 thread and seed: " << test_run << std::endl;
        valid &= test_set_n_threads<CoarseSet>(4, DEFAULT_OP_MOD);
        std::cout << std::endl;

        if (!valid) {
            break;
        }
    }

    if (valid) {
        return 0;
    } else {
        return -1;
    }
}

/// A04: This function tests your implementation of the `FineSet` class in `fine_set.hpp`
int task_4() {
    bool valid = true;
    std::cout << "# Task 4: Fine Set" << std::endl;

    for (int test_run = 0; test_run < 8; test_run++) {
        std::cout << "## Testing `FineSet` with 4 thread and seed: " << test_run << std::endl;
        valid &= test_set_n_threads<FineSet>(4, DEFAULT_OP_MOD);
        std::cout << std::endl;

        if (!valid) {
            break;
        }
    }

    if (valid) {
        return 0;
    } else {
        return -1;
    }
}


int main(int argc, char* argv[]) {
    // Input validation
    if (argc < 2) {
        fprintf(stderr, "Please select the task you want to execute\n");
        return -1;
    }

    int selection;
    try {
        selection = std::stoi(argv[1]);
    }
    catch (const std::invalid_argument& ia) {
        fprintf(stderr, "The first argument has to be an integer\n");
        return -1;
    }

    srand(0);
    switch (selection) {
        case 1:
            return task_1();
        case 2:
            return task_2();
        case 3:
            return task_3();
        case 4:
            return task_4();
        default:
            fprintf(stderr, "Please enter a valid task, as the first argument\n");
            return -1;
    }
}

