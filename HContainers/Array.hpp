#pragma once
#include "Common.hpp"

namespace HS
{
	
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

		HS_ARRAY_IMPL();

		FINLINE void CapacityCheck(int _size)
		{
			if (_size >= capacity)
			{
				capacity += capacity / 2;
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
				std::memset(ptr + capacity - 32, 0, sizeof(T) * 32); // clear generated new area
			}
		}

		void Reserve(int _size)
		{
			CapacityCheck(_size);
		}

		void Add(T value) {
			CapacityCheck(size + 1);
			ptr[size++] = value;
		}

		void SetRange(int start, int end, T value) {
			for (; start < end; ++start) ptr[start] = value;
		}

		void SetRange(T* start, T* end, T value) {
			for (; start < end; ++start) *start = value;
		}

		void AddAtIndex(int index, T value) {
			CapacityCheck(size + 2);
			std::memmove(ptr + index + 1, ptr + index, sizeof(T) * abs(index - size));
			ptr[index] = value;
			++size;
		}

		template<typename TFunc>
		HArrayResult Remove(TFunc match)
		{
			for (int i = 0; i < size; ++i) {
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
			for (int i = 0; i < size; ++i) {
				if (ptr[i] == value) {
					ptr[i] = ptr[size-1];
					return true;
				}
			}
			return false;
		}

		template<typename TFunc>
		int RemoveAll(const TFunc& match)
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

			memset(ptr + freeIndex, 0, size - freeIndex * sizeof(T));
			int result = size - freeIndex;
			size = freeIndex;

			return result; // removed item count
		}

		void AddFront(T value) {
			CapacityCheck(size + 2);
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

	//////////////////////////////////////////////////////////////////

	//	ObjArray

	//////////////////////////////////////////////////////////////////

	template<typename T>
	class ObjArray
	{
	public:
		~ObjArray() 
		{
			if (ptr) {  Clear();  delete[] ptr;  ptr = nullptr;  }
		}
		ObjArray() : size(0), capacity(32) { ptr = new T[capacity]; }
		ObjArray(int _size) : size(0), capacity(_size) { ptr = new T[capacity]; }
		
		ObjArray(int _size, T value) : size(0), capacity(_size) 
		{ 
			ptr = new T[capacity]; 
			for (int i = 0; i < _size; i++) ptr[i] = value;
		}

		// copy constructor
		ObjArray(const ObjArray& other) : size(other.size), capacity(other.capacity)
		{
			ptr = new T[capacity];
			ArrayCpy(ptr, other.ptr, size);
		}

		// move constructor 
		ObjArray(ObjArray&& other) : size(other.size), capacity(other.capacity), ptr(other.ptr)
		{
			other.ptr = nullptr;
		}
		
		T& operator[](int index) { return ptr[index]; }                        
		const T& operator[](int index) const { return ptr[index]; }            
		T* begin() { return ptr; }								               
		T* end() { return ptr + size; }							               
		const T* cbegin() const { return ptr; }					               
		const T* cend() const { return ptr + size; }				           
		T GetLast() { return ptr[size - 1]; }                                  
		T GetFirst(){ return ptr[0]; }                                         
		inline bool Any() { return size > 0; }								   
		void Clear() { ArrayClear(ptr, size); size = 0; }
		
		void CapacityCheck(int _size)
		{
			if (_size >= capacity)
			{
				capacity += capacity / 2; 
				T* old = ptr;
				ptr = new T[capacity];
				ArrayCopy(ptr, old, _size-1);
			}
		}

		void Reserve(int _size) {
			CapacityCheck(_size);
		}

		void Add(T value) {

			CapacityCheck(size + 1);
			ptr[size++] = value;
		}

		void SetRange(int start, int end, const T& value) {
			for (; start < end; ++start) ptr[start] = value;
		}

		void AddAtIndex(int index, T value) {
			CapacityCheck(size + 2);
			ArrayMove(ptr + index + 1, ptr + index, sizeof(T) * abs(index - size));
			ptr[index] = value;
			++size;
		}
		
		template<typename TFunc>
		HArrayResult Remove(TFunc match)
		{
			for (int i = 0; i < size; ++i) {
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
			for (int i = 0; i < size; ++i) {
				if (ptr[i] == value) {
					ptr[i] = ptr[size-1];
					return true;
				}
			}
			return false;
		}

		template<typename TFunc>
		int RemoveAll(TFunc match)
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

			ArrayClear(ptr + freeIndex, size - freeIndex);
			int result = size - freeIndex;
			size = freeIndex;

			return result; // removed item count
		}

		void AddFront(T value) {
			CapacityCheck(size + 1);
			ptr[0] = value;
			ArrayMove(ptr, ptr + 1, size * sizeof(T));
			++size;
		}

		void RemoveBack() { ptr[--size].~T(); }
		void RemoveFront() 
		{
			ArrayMove(ptr, ptr + 1, size); 
			ptr[--size].~T();  
		}

		void RemoveAtIndex(int index) 
		{
			if (size == 0) return;
			ArrayMove(ptr + index, ptr + index + 1, size - index + 1);
			--size;
		}

	public:
		T* ptr;
		int size;
		int capacity;
	};
}

	