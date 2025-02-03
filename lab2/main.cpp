#include "monitoring.hpp"
#include "test.hpp"
#include "bench.hpp"
#include "std_set.hpp"
#include "fine_set.hpp"
#include "lazy_set.hpp"
#include "optimistic_set.hpp"
#include "std_multiset.hpp"
#include "fine_multiset.hpp"

#include <stdio.h>
#include <cstring>
#include <iostream>

#define OPERATION_COUNT 500
#define DEFAULT_GENERATOR_SEED 0
#define DEFAULT_OP_MOD 128

const std::vector<OpWeights<SetOperator>> DEFAULT_SET_GEN_WEIGHTS = {
    OpWeights<SetOperator> {op: SetOperator::Add, weight: 3},
    OpWeights<SetOperator> {op: SetOperator::Remove, weight: 4},
    OpWeights<SetOperator> {op: SetOperator::Contains, weight: 3},
};

const std::vector<OpWeights<MultisetOperator>> DEFAULT_MULTISET_GEN_WEIGHTS = {
    OpWeights<MultisetOperator> {op: MultisetOperator::MSetAdd, weight: 3},
    OpWeights<MultisetOperator> {op: MultisetOperator::MSetRemove, weight: 4},
    OpWeights<MultisetOperator> {op: MultisetOperator::MSetCount, weight: 3},
};

template <typename Set>
void run_set_n_threads(int thread_count, int op_arg_mod) {
    StdSet test_set;
    OpGenerator<SetOperator> generator(DEFAULT_SET_GEN_WEIGHTS, OPERATION_COUNT, op_arg_mod, DEFAULT_GENERATOR_SEED);
    Set set;

    run_data_structure_n_threads<Set, SetOperator>(&set, &generator, thread_count);
}

template <typename Multiset>
bool run_multiset_n_threads(int thread_count, int op_arg_mod, int seed = DEFAULT_GENERATOR_SEED) {
    StdMultiset test_set;
    EventMonitor<Multiset, StdMultiset, MultisetOperator> monitor(&test_set);
    OpGenerator<MultisetOperator> generator(
        DEFAULT_MULTISET_GEN_WEIGHTS,
        OPERATION_COUNT,
        op_arg_mod,
        seed
    );
    Multiset set(&monitor);
    monitor.set_concurrent_data_structure(&set);

    return run_data_structure_n_threads_with_monitor<Multiset, StdMultiset, MultisetOperator>(
        &set,
        &generator,
        &monitor,
        thread_count
    );
}

template <typename Set>
bool test_final_state() {
    // A lower max number makes the console output more readable
    int max_num = 64;
    int op_count = 400;
    bool valid = true;

    StdSet validation_set;
    OpGenerator<SetOperator> generator1(DEFAULT_SET_GEN_WEIGHTS, op_count, max_num, DEFAULT_GENERATOR_SEED);
    worker_thread_func<StdSet>(&validation_set, &generator1, 0);

    Set test_set;
    OpGenerator<SetOperator> generator2(DEFAULT_SET_GEN_WEIGHTS, op_count, max_num, DEFAULT_GENERATOR_SEED);
    worker_thread_func<Set>(&test_set, &generator2, 0);

    for (int i = 0; i < max_num; i++) {
        if (test_set.ctn(i) != validation_set.ctn(i)) {
            valid = false;
            std::cout << "The final state of the tested state is incorrect" << std::endl;
            std::cout << "  - ctn(" << i << ") of the tested set returned: " << test_set.ctn(i) << std::endl;
            std::cout << "  - ctn(" << i << ") of the validation set returned: " << validation_set.ctn(i) << std::endl;

            std::cout << "  - Validation State: ";
            validation_set.print_state();
            std::cout << std::endl;

            std::cout << "  - Test State:       ";
            test_set.print_state();
            std::cout << std::endl;
            break;
        }
    }

    if (valid) {
        std::cout
            << "Validation of the final state after "
            << op_count
            << " operations was successful"
            << std::endl;
    }

    return valid;
}

template <typename Set>
bool test_set_implementation(char const* set_name) {
    bool valid = true;

    std::cout << "## Running `" << set_name << "` with 1 thread" << std::endl;
    valid &= test_final_state<Set>();
    std::cout << std::endl;

    std::cout << "## Running `"<< set_name <<"` with 8 thread" << std::endl;
    run_set_n_threads<Set>(8, DEFAULT_OP_MOD);
    std::cout << "This multithreaded test doesn't validate the state." << std::endl;
    std::cout << "If it didn't crash it's probably fine." << std::endl;
    std::cout << std::endl;

    if (valid) {
        return 0;
    } else {
        return -1;
    }
}

int task_1() {
    // A01: This code does some basic validation of the `OptimisticSet`
    std::cout << "# Task 1: OptimisticSet" << std::endl;
    std::cout << std::endl;

    return test_set_implementation<OptimisticSet>("OptimisticSet");
}

int task_2() {
    // A02: This code does some basic validation of the `LazySet`
    std::cout << "# Task 2: LazySet" << std::endl;
    std::cout << std::endl;

    return test_set_implementation<LazySet>("LazySet");
}

int task_3() {
    // A03: This code does some basic validation of the `FineSet`
    std::cout << "# Task 3: FineSet" << std::endl;
    std::cout << std::endl;

    return test_set_implementation<FineSet>("FineSet");
}

int task_4() {
    std::cout << "# Task 4: FineSet" << std::endl;
    std::cout << std::endl;
    bench::benchmark_set<FineSet>("FineSet");
    bench::benchmark_set<OptimisticSet>("OptimisticSet");
    bench::benchmark_set<LazySet>("LazySet");

    return 0;
}

int task_5() {
    bool valid = true;
    std::cout << "# Task 5: `FineMultiset`" << std::endl;

    for (int test_run = 0; test_run < 8; test_run++) {
        std::cout << "## Testing `FineMultiset` with 4 thread and seed: " << test_run << std::endl;
        valid &= run_multiset_n_threads<FineMultiset>(4, DEFAULT_OP_MOD, test_run);
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
        case 5:
            return task_5();
        default:
            fprintf(stderr, "Please enter a valid task, as the first argument\n");
            return -1;
    }
}

