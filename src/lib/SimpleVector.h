#pragma once

#include <memory>

template<typename T, typename size_type = std::size_t, typename A = std::allocator<T>>
struct SimpleVector {
	// using size_type = uint32_t; // same as MAX_ENTITIES
	
	T* elem;
	A alloc;
	size_type sz;
	size_type space;
	size_type initial_size = 8;

	SimpleVector() {
		space = sz = 0;
		elem = nullptr;
	}

	explicit SimpleVector(size_type s): sz{0}, space{0}, elem{nullptr} {
		reserve(s);
	}

	void push_back(T val) {
		size_type index = sz;

		if(space == 0)
			reserve(initial_size);
		else if(sz == space)
			reserve(2 * space);
		
		alloc.construct(&elem[index], val);
		sz = index + 1;
	}

	// reserve() does not change the `sz` of the vector
	void reserve(size_type new_alloc) {
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

	void pop() {
		if(sz == 0) return;
		
		alloc.destroy(&elem[--sz]);
	}

	// clear will reset capacity also
	// elem is set to nullptr
	void clear() {
		for(size_type i = 0; i < sz; i++) 
			alloc.destroy(&elem[i]);

		alloc.deallocate(elem, space);

		space = sz = 0;
		elem = nullptr;
	}

	~SimpleVector() {
		clear();
	}

	T& operator[] (size_type n) { return elem[n]; }
	T operator[] (size_type n) const { return elem[n]; }

	size_type size() const { return sz; }
	size_type capacity() const { return space; }
};
