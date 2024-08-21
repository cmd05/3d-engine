#pragma once

#include <memory>
#include <cstddef> // for std::size_t

template<typename T, typename S = std::size_t, typename A = std::allocator<T>>
class SimpleVector {
public:
    // typedefs
    using size_type = S;
    using allocator_type = A;

    // CTORs
    SimpleVector();
    explicit SimpleVector(size_type s);

    // modifiers
    void push_back(T val);
    void reserve(size_type new_alloc);
    void pop_back();
    void clear();

    // Destructor
    ~SimpleVector();

    // setters
    T& operator[] (size_type n) { return elem[n]; }
    T operator[] (size_type n) const { return elem[n]; }

    // getters
    size_type size() const { return sz; }
    size_type capacity() const { return space; }
private:
    allocator_type alloc;
    size_type sz;
    size_type space;
    T* elem;
    size_type initial_size = 8;
};

template<typename T, typename S, typename A>
SimpleVector<T,S,A>::SimpleVector(): sz{0}, space{0}, elem{nullptr} {}

template<typename T, typename S, typename A>
SimpleVector<T,S,A>::SimpleVector(size_type s): sz{0}, space{0}, elem{nullptr} {
    reserve(s);
}

template<typename T, typename S, typename A>
void SimpleVector<T,S,A>::push_back(T val) {
    size_type index = sz;

    if(space == 0)
        reserve(initial_size);
    else if(sz == space)
        reserve(2 * space);
    
    alloc.construct(&elem[index], val);
    sz = index + 1;
}

// reserve() does not change the `sz` of the vector
template<typename T, typename S, typename A>
void SimpleVector<T,S,A>::reserve(size_type new_alloc) {
    if(new_alloc <= space) return;
    
    // copy to larger location
    T* new_elem = alloc.allocate(new_alloc);
    
    if(sz > 0) // avoid dereferencing nullptr
        std::uninitialized_copy(elem, &elem[sz], new_elem);

    // clear old elements
    size_type old_size = sz;
    clear();
            
    // set new state
    sz = old_size;
    space = new_alloc;
    elem = new_elem;
}

template<typename T, typename S, typename A>
void SimpleVector<T,S,A>::pop_back() {
    if(sz == 0) return;
    
    alloc.destroy(&elem[--sz]);
}

// clear will reset capacity also
// elem is set to nullptr
template<typename T, typename S, typename A>
void SimpleVector<T,S,A>::clear() {
    for(size_type i = 0; i < sz; i++) 
        alloc.destroy(&elem[i]);

    alloc.deallocate(elem, space);

    space = sz = 0;
    elem = nullptr;
}

template<typename T, typename S, typename A>
SimpleVector<T,S,A>::~SimpleVector() {
    clear();
}