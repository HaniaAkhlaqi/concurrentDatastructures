#pragma once

class Set {
   public:
    /// The deconstructor can be used to deallocate memory, once the Set is no
    /// longer needed.
    virtual ~Set() {};

    /// This method adds the given element into the set. It will return
    /// `true`, if the insertion was successful, `false` otherwise. An
    /// insertion might fail, if the set already contains the given element.
    virtual bool add(int elem) = 0;

    /// This method removes the given element from the set. It will return
    /// `true`, if the element was in the set and was removed, `false` otherwise.
    virtual bool rmv(int elem) = 0;

    /// This method checks if a given element is inside the set. It returns
    /// `true` if the element is present, `false` otherwise.
    virtual bool ctn(int elem) = 0;

    /// This method can be overwritten to provide a better debug message.
    /// This is not part of the assignment and only intended to help with debugging.
    virtual void print_state() {};
};
