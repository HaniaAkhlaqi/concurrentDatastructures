#pragma once

#include "monitoring.hpp"

#include <cstring>

template <class CDS, typename Op>
void worker_thread_func(CDS* data_structure, OpGenerator<Op>* generator, int thread_id) {
    while (auto maybe_operation = generator->next()) {
        Operation<Op> operation = maybe_operation.value();
        apply_op(data_structure, operation);
    }
}

template <class CDS, class DS, typename Op>
void monitor_thread_func(EventMonitor<CDS, DS, Op>* monitor) {
    monitor->monitor();
}

template <typename CDS, typename Op>
void run_data_structure_n_threads(
    CDS* concurrent_data_structure,
    OpGenerator<Op>** generator,
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
            std::ref(generator[thread_id]),
            std::ref(thread_id)
        );
    }

    // Join threads
    for (int i = 0; i < thread_count; i++) {
        workers[i].join();
    }

    // Cleanup
    free(workers);
}

template <typename CDS, typename Op>
void run_data_structure_n_threads(
    CDS* concurrent_data_structure,
    OpGenerator<Op>* generator,
    int thread_count
) {
    // Use the same generator for every thread
    OpGenerator<Op>** generators = new OpGenerator<Op>*[thread_count];
    for (int i = 0; i < thread_count; i++) {
        generators[i] = generator;
    }

    run_data_structure_n_threads(concurrent_data_structure, generators, thread_count);

    delete generators;
}

template <typename CDS, typename DS, typename Op>
bool run_data_structure_n_threads_with_monitor(
    CDS* concurrent_data_structure,
    OpGenerator<Op>* generator,
    EventMonitor<CDS, DS, Op>* monitor,
    int thread_count
) {
    // Start monitor thread
    std::thread monitor_thread(monitor_thread_func<CDS, DS, Op>, std::ref(monitor));

    run_data_structure_n_threads(concurrent_data_structure, generator, thread_count);

    // Finish the monitor and monitor thread
    monitor->finish();
    monitor_thread.join();

    return monitor->is_valid();
}
