#pragma once

#include "monitoring.hpp"
#include "std_set.hpp"
#include "adt.hpp"

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

/// This namespace holds all functions required for benchmarking.
namespace bench {

    const int OP_COUNT = 500;
    const int VALUE_MODS[] = {8, 1024};
    const int CTN_WEIGHTS[] = {10, 50, 90};
    const int THREAD_COUNTS[] = {1, 2, 4, 8, 16, 32};
    const int DEFAULT_GENERATOR_SEED = 0;

    double time_now() {
        struct timeval t;
        gettimeofday(&t, NULL);
        return (t.tv_sec + t.tv_usec / 1000000.0) * 1000.0;
    }

    struct BenchConfig {
        int value_mod;
        int ctn_weight;
        int threads;

        BenchConfig(int value_mod, int ctn_weight, int threads) {
            this->value_mod = value_mod;
            this->ctn_weight = ctn_weight;
            this->threads = threads;
        }

        int get_add_weight() {
            return (100 - this->ctn_weight) * 0.9;
        }

        int get_rmv_weight() {
            return 100 - this->ctn_weight - this->get_add_weight();
        }
    };

    void print_table_header() {
        printf("          name,  values, ctn [%%], add [%%], rmv [%%], threads, time [ms], total ops\n");
    }

    void print_table_row(char const* ds_name, BenchConfig& config, double time) {
        printf(
            "%14s, 0..%-4d,     %3d,     %3d,     %3d,      %2d, %9.4f,    %6d\n",
            ds_name,
            config.value_mod,
            config.ctn_weight,
            config.get_add_weight(),
            config.get_rmv_weight(),
            config.threads,
            time,
            config.threads * OP_COUNT
        );
    }

    template <class Set>
    void run_config(char const* set_name, BenchConfig& config) {
        std::vector<OpWeights<SetOperator>> op_weights = {
            OpWeights<SetOperator> {op: SetOperator::Add, weight: config.ctn_weight},
            OpWeights<SetOperator> {op: SetOperator::Remove, weight: config.get_add_weight()},
            OpWeights<SetOperator> {op: SetOperator::Contains, weight: config.get_rmv_weight()},
        };
        OpGenerator<SetOperator>** generators = new OpGenerator<SetOperator>*[config.threads];
        for (int i = 0; i < config.threads; i++) {
            generators[i] = new OpGenerator(op_weights, OP_COUNT, config.value_mod, DEFAULT_GENERATOR_SEED + i);
        }

        Set set;
        double start = time_now();
        run_data_structure_n_threads<Set, SetOperator>(&set, generators, config.threads);
        double end = time_now();

        print_table_row(set_name, config, end - start);

        for (int i = 0; i < config.threads; i++) {
            delete generators[i];
        }
        delete generators;
    }

    template <class Set>
    void benchmark_set(char const* set_name) {
        print_table_header();

        for (int value_mod : VALUE_MODS) {
            for (int ctn_weight : CTN_WEIGHTS) {
                for (int threads : THREAD_COUNTS) {
                    BenchConfig config = BenchConfig(value_mod, ctn_weight, threads);
                    run_config<Set>(set_name, config);
                }
            }
        }
    }
}
