#pragma once
#include "Common.hpp"

namespace HS
{
	template<typename T>
	class Stack
	{
	public:
		~Stack() { if (ptr) {  Clear();  std::free(ptr);  ptr = nullptr;  } }

		Stack()
		: size(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T));  }
		Stack(int _size)
		: size(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T));  }

		// copy constructor
		Stack(const Stack& other) : size(other.size), capacity(other.capacity), ptr((char*)std::calloc(capacity, sizeof(T)))
		{
			std::memcpy(ptr, other.ptr, size);
		}
		// move constructor 
		Stack(Stack&& other) : size(other.size), capacity(other.capacity), ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		// initialize operator[] begin(), end(), GetFirst(), GetLast(), Clear()
		HS_ARRAY_IMPL()

		void Push(T value) {
			if (size + 1 == capacity) {
				capacity += 32;
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			}
			ptr[size++] = value;
		}

		T Pop() { return ptr[--size]; }
		bool TryPop(T& out) {
			if (size > 0) out = ptr[--size];
			return size > 0;
		}

	public:
		int size;
		int capacity;
		T* ptr;
	};
}
