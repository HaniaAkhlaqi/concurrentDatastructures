#pragma once

#include "monitoring.hpp"
#include "adt.hpp"

const uintptr_t FLAG_MASK = 0x00000001;
const uintptr_t PTR_MASK = ~FLAG_MASK;

#define ATOMIC_PTR_AND_FLAG_MERGE(ptr, flag) (((uintptr_t) ptr) | (flag ? FLAG_MASK : 0))

/// Memory addresses of allocations on modern processors are (at least) 4 byte aligned,
/// this means that the lower 2 bytes of an allocated pointer are always 0. By using
/// some bit masking magic, it's possible to use the two lower bytes as storage for
/// flags. This class wraps an atomic pointer and allows the storage of a flag in the
/// lower unused bytes.
///
/// Note: It's possible to get pointers where the lower two bytes are used by indexing
/// into an allocation. If you always allocate a node using `malloc` or `new` this
/// will not be a problem here.
template <typename T>
class AtomicPtrAndFlag {
    std::atomic<intptr_t> ptr;

public:
    AtomicPtrAndFlag(T* ptr, bool flag) {
        this->ptr = ATOMIC_PTR_AND_FLAG_MERGE(ptr, flag);
    }

    /// Performs a compare-and-set operation. The value will only be
    /// updated if the current pointer and mark is equal to the ones
    /// given in the `test_*` arguments. The operation returns true
    /// if the comparison was successful and the value was updated.
    bool cas(T* test_ptr, T* new_ptr, bool test_mark, bool new_mark) {
        intptr_t test = ATOMIC_PTR_AND_FLAG_MERGE(test_ptr, test_mark);
        intptr_t set = ATOMIC_PTR_AND_FLAG_MERGE(new_ptr, new_mark);
        return this->ptr.compare_exchange_strong(test, set);
    }

    /// This tries to set the flag to `true`. It returns `true`,
    /// if the pointer matched and the value was update.
    bool try_set_mark(T* test_ptr) {
        return cas(test_ptr, test_ptr, false, true);
    }

    /// This returns the stored pointer and flag as a tuple. You can access
    /// the values using `std::tie` as shown below:
    ///
    /// ```c++
    /// LockFreeSetNode* ptr = nullptr;
    /// bool mark = false;
    /// std::tie (ptr, mark) = atomic.get();
    /// ```
    ///
    /// Or alternatively using `std::get`, like this:
    ///
    /// ```c++
    /// auto tuple = atomic.get()
    /// LockFreeSetNode* ptr = std::get<0>(tuple);
    /// bool mark = std::get<1>(tuple);
    /// ```
    std::tuple<T*, bool> get() {
        intptr_t value = this->ptr.load();
        T* ptr = (T*)(value & PTR_MASK);
        bool mark = (value & FLAG_MASK);

        return std::tuple<T*, bool>(ptr, mark);
    }

    /// This returns the stored pointer.
    ///
    /// Note: If you want to retrieve both the pointer and mark, you should use
    /// `AtomicPtrAndFlag::get()` to access both values atomically at once.
    T* get_ptr() {
        return std::get<0>(this->get());
    }

    /// This returns the flag pointer.
    ///
    /// Note: If you want to retrieve both the pointer and mark, you should use
    /// `AtomicPtrAndFlag::get()` to access both values atomically at once.
    bool get_flag() {
        return std::get<1>(this->get());
    }
};

struct LockFreeSetNode {
    // A02: You can add or remove fields as needed.
    int value;
    AtomicPtrAndFlag<LockFreeSetNode> next;

    LockFreeSetNode(int value, LockFreeSetNode* next) :
        value(value),
        next(AtomicPtrAndFlag(next, false))
    {}
};

struct LockFreeWindow {
    // A02: You can add or remove fields as needed.
    LockFreeSetNode* pred;
    LockFreeSetNode* curr;

    LockFreeWindow(LockFreeSetNode* pred, LockFreeSetNode* curr)
        : pred(pred), curr(curr)
    {}
};

/// A template for the implementation of a lock-free set, as shown in chapter
/// 9.8 in the course book. There this data structure is called `LockFreeList`.
class LockFreeSet: public Set {
private:
    // A02: You can add or remove fields as needed.
    LockFreeSetNode* head;

    LockFreeWindow find(int key) {
        // A02: Implement the `find()` method as shown in figure 9.24 in the book
        //      Here you can use `this->head` instead of having the pointer as a
        //      function parameter.
        return LockFreeWindow(nullptr, nullptr);
    }

public:
    LockFreeSet() {
        // A02: Initialize the internal state.
        //      - The book doesn't specify how the state should be initialized.
        //        The code used in the book requires that there is always a valid
        //        head and tail pointer, since null chases are not specifically
        //        handled. The code below initializes a head and tail pointer.
        //        You're welcome to modify the code as needed for your implementation.
        LockFreeSetNode* tail = new LockFreeSetNode(INT_MAX, nullptr);
        head = new LockFreeSetNode(INT_MIN, tail);
    }
    ~LockFreeSet() {
        // A02: Cleanup any memory that was allocated
    }

    bool add(int value) override {
        bool result = false;
        // A02: Add code to insert the element into the set and update `result`.
        return result;
    }

    bool rmv(int value) override {
        bool result = false;
        // A02: Add code to remove the element from the set and update `result`.
        return result;
    }

    bool ctn(int value) override {
        bool result = false;
        // A02: Add code to check if the element is in the set and update `result`.
        return result;
    }

    void print_state() override {
        // A02: Optionally, add code to print the state. This is useful for debugging,
        // but not part of the assignment
        std::cout << "LockFreeSet {...}";
    }
};
