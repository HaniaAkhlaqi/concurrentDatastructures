# Code Preset for Lab 2

Template for Lab Assignment 2 for the course "Concurrent Data Structures".

## Overview

* `src/main.cpp`: Contains the main function and functions to run the implementation of the individual tasks.
* `src/monitoring.hpp`: Contains the definition of operators, operations, events, and infrastructure to monitor a shared sequence of events.
* `src/set.hpp`: An abstract class defining the `Set` and `Multiset` datatypes.
* `src/test.hpp`: Contains infrastructure to test concurrent data structures.
* `src/bench.hpp`: Contains infrastructure to benchmark data structures.
* `src/std_set.hpp`: An implementation of a `Set` based on `std::set`, used for validation.
* `src/std_multiset.hpp`: An implementation of a `Multiset` based on `std::multiset`, used for validation.
* `src/optimistic_set.hpp`: A template to implement a `Set` with optimistic synchronization for task 1.
* `src/coarse_set.hpp`: A template to implement a `Set` with lazy synchronization for task 2.
* `src/fine_set.hpp`: A template to implement/copy a `Set` with fine-grained locking for task 3.
* `src/fine_multiset.hpp`: A template to implement a `Multiset` with fine-grained locking for task 5.

## Templates

This preset uses templates to make code generic. There are some type parameter names that are worthy of note:
* `Op`: An enum identifying the operator used by an operation.
* `DS`: A data structure
* `CAD`: For functions taking two data structures, this defines the data structure that will be tested for concurrency.

## Compilation

The lab template contains a `makefile` file that can be used to build the project. The file has been tested on Ubuntu with g++ v11.4.0, but it should also work on macOS and other Unix-based systems. You can also manually invoke the following g++ command in the `src` folder.

```bash
g++ -c main.cpp -o a.out -Wall
```

This will generate a `a.out` file.

Your final submission should compile and run on the Linux lab machines from Uppsala University(https://www.it.uu.se/datordrift/maskinpark/linux). If your submission requires a different command for compilation, please document it in your report.

## Run Instructions

The created binary takes the task number as the first argument. For example `./a.out 1` will run the first task.
