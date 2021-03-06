#pragma once
#include "Common.hpp"

namespace HS
{
	enum class HArrayResult : int
	{
		None, Success, Fail, IndexBoundsOutOfArray, NotFinded, Size0
	};

	template<typename T>
	class Array
	{
	public:
		~Array() 
		{
			if (ptr) {  Clear();  std::free(ptr);  ptr = nullptr;  }
		}
		Array() : size(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T)); }
		Array(int _size) : size(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T)); }
		
		Array(int _size, T value) : size(0), capacity(_size) 
		{ 
			ptr = (T*)std::malloc(capacity * sizeof(T));
			for (int i = 0; i < _size; i++) ptr[i] = value;
		}

		// copy constructor
		Array(const Array& other) : size(other.size), capacity(other.capacity), ptr((char*)std::calloc(capacity, sizeof(T)))
		{
			std::memcpy(ptr, other.ptr, size);
		}

		// move constructor 
		Array(Array&& other) : size(other.size), capacity(other.capacity), ptr(other.ptr)
		{
			other.ptr = nullptr;
		}

		// initialize operator[] begin(), end(), GetFirst(), GetLast(), Clear()
		HS_ARRAY_IMPL()

		void Reserve(int _size)
		{
			if (_size >= capacity)
			{
				capacity = _size;
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
				std::memset(ptr + capacity - 32, 0, sizeof(T) * 32); // clear generated new area
			}
		}

		void Add(T value) {

			if (size + 1 == capacity) {
				capacity += 32;
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
				std::memset(ptr + capacity - 32, 0, sizeof(T) * 32); // clear generated new area
			}
			ptr[size++] = value;
		}

		void SetRange(int start, int end, T value) {
			for (; start < end; ++start) ptr[start] = value;
		}

		void SetRange(T* start, T* end, T value) {
			for (; start < end; ++start) *start = value;
		}

		void AddAtIndex(int index, T value) {
			if (size + 2 >= capacity) {
				capacity += 32;
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			}

			std::memmove(ptr + index + 1, ptr + index, sizeof(T) * std::abs(index - size));
			ptr[index] = value;
			++size;
		}

		HArrayResult Remove(T value)
		{
			for (int i = 0; i < size; ++i)
			{
				if (value == ptr[i]) {
					RemoveAtIndex(i); 
					return HArrayResult::Success;
				}
			}
			return HArrayResult::NotFinded;
		}

		HArrayResult Remove(bool(*match)(const T&))
		{
			for (int i = 0; i < size; ++i)
			{
				if (match(ptr[i])) {
					RemoveAtIndex(i);
					return HArrayResult::Success;
				}
			}
			return HArrayResult::NotFinded;
		}

		// faster version of remove
		// returns true if value was exist
		bool RemoveUnordered(const T& value)
		{
			for (int i = 0; i < size; ++i)
			{
				if (ptr[i] == value)
				{
					ptr[i] = ptr[size-1];
					return true;
				}
			}
			return false;
		}

		int RemoveAll(const T& value)
		{
			int freeIndex = 0;   // the first free slot in items array

			// Find the first item which needs to be removed.
			while (freeIndex < size && ptr[freeIndex] != value) freeIndex++;
			if (freeIndex >= size) return 0;

			int current = freeIndex + 1;
			while (current < size) {
				// Find the first item which needs to be kept.
				while (current < size && ptr[current] == value) current++;

				if (current < size) {
					// copy item to the free slot.
					ptr[freeIndex++] = ptr[current++];
				}
			}

			memset(ptr + freeIndex, 0, size - freeIndex);
			int result = size - freeIndex;
			size = freeIndex;

			return result; // removed item count
		}

		int RemoveAll(bool(*match)(const T&))
		{
			int freeIndex = 0;   // the first free slot in items array

			// Find the first item which needs to be removed.
			while (freeIndex < size && !match(ptr[freeIndex]) ) freeIndex++;
			if (freeIndex >= size) return 0;

			int current = freeIndex + 1;
			while (current < size) {
				// Find the first item which needs to be kept.
				while (current < size && match(ptr[current]) ) current++;

				if (current < size) {
					// copy item to the free slot.
					ptr[freeIndex++] = ptr[current++];
				}
			}

			memset(ptr + freeIndex, 0, size - freeIndex);
			int result = size - freeIndex;
			size = freeIndex;

			return result; // removed item count
		}

		void AddFront(T value) {
			if (size + 1 >= capacity)  {
				capacity += capacity / 2; 
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			}
			ptr[0] = value;


			std::memmove(ptr, ptr + 1, size * sizeof(T));
			++size;
		}

		void RemoveBack() { std::memset(&ptr[--size], 0, sizeof(T)); }
		void RemoveFront() { std::memmove(ptr, ptr + 1, size * sizeof(T)); std::memset(&ptr[--size], 0, sizeof(T)); }

		void RemoveAtIndex(int index) 
		{
			if (size == 0) return;
			std::memmove(ptr + index, ptr + index + 1, size - index+1 * sizeof(T));
			--size;
		}

	public:
		T* ptr;
		int size;
		int capacity;
	};	
}

	