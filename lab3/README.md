# Code Preset for Lab 3

Template for Lab Assignment 3 for the course "Concurrent Data Structures".

## Overview

* `src/main.cpp`: Contains the main function and functions to run the implementation of the individual tasks.
* `src/monitoring.hpp`: Contains the definition of operators, operations, events, and infrastructure to monitor a shared sequence of events.
* `src/adt.hpp`: Abstract classes defining the `Set`, `Multiset` and `Stack`` data types.
* `src/test.hpp`: Contains infrastructure to test concurrent data structures.
* `src/bench.hpp`: Contains infrastructure to benchmark data structures.
* `src/std_set.hpp`: An implementation of a `Set` based on `std::set`, used for validation.
* `src/std_stack.hpp`: An implementation of a `Stack` based on `std::stack`, used for validation.
* `src/treiber_stack.hpp`: A template to implement a `Stack` using the Treiber algorithm for task 1.
* `src/lock_free_set.hpp`: A template to implement a `LockFreeSet` based on the `LockFreeList` data type in the course book for task 2.

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
