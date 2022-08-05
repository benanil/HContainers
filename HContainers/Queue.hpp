#pragma once
#include "Common.hpp"

namespace HS
{
	// recommended for integers and floats this container will optimize heap performance
	// for this tree you need to provide Compare::Equal<T>, Compare::Less<T> for non primitive types
	template<typename T>
	class PriarotyQueue
	{
	public:
		~PriarotyQueue() {
			if (ptr) {  Clear();  std::free(ptr);  ptr = nullptr;  }
		}
		PriarotyQueue() : size(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T)); }
		PriarotyQueue(int _size) : size(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T)); }
		// send sorted array
		PriarotyQueue(T* begin, int _size) : ptr(begin), size(_size), capacity(32) { }

		// copy constructor
		PriarotyQueue(const PriarotyQueue& other) : size(other.size), capacity(other.capacity), ptr((char*)std::calloc(capacity, sizeof(T)))
		{
			std::memcpy(ptr, other.ptr, size);
		}
		// move constructor 
		PriarotyQueue(PriarotyQueue&& other) : size(other.size), capacity(other.capacity), ptr(other.ptr)
		{
			other.ptr = nullptr;
		}

		// initialize operator[] begin(), end(), GetFirst(), GetLast(), Clear()
		HS_ARRAY_IMPL()

		void Add(T value)
		{
			if (Compare::Greater<T>(value, GetLast())) { _AddAtEnd(value); return; }
			for (int i = size - 1, j = 1; i >= 0; --i, ++j)
			{
				if (Compare::Greater<T>(value, ptr[i])) {
					std::memmove(ptr + i + 1, ptr + i, sizeof(T) * j);
					ptr[i] = value;
					break;
				}
			}
			++size;
		}

		T Pull() {
			T value = GetLast();
			RemoveBack();
			return value;
		}

		void Clear() const { std::memset(ptr, 0, sizeof(T) * capacity); }

		HArrayResult Remove(T value)
		{
			for (int i = 0; i < size; ++i)
				if (Compare::Equal<T>(value, ptr[i])) {
					RemoveAtIndex(i); return HArrayResult::Success;
				}

				return HArrayResult::NotFinded;
		}

		void RemoveBack() { std::memset(&ptr[--size], 0, sizeof(T)); }
		void RemoveFront() { std::memmove(ptr, ptr + 1, size * sizeof(T)); std::memset(&ptr[--size], 0, sizeof(T)); }

		void RemoveAtIndex(int index) {
			std::memset(&ptr[index], 0, sizeof(T));
			std::memmove(ptr + index, ptr + index + 1, size * sizeof(T));
			--size;
		}
	private:
		void _AddAtEnd(T value) {
			if (size + 1 == capacity) {
				capacity += 32;
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
			}
			ptr[size++] = value;
		}
	public:
		T* ptr;
		int size;
		int capacity;
	};


	template<typename T>
	class Queue
	{
	public:
		~Queue() { if (ptr) { Clear(); std::free(ptr); ptr = nullptr; }}

		Queue() : front(0), rear(0), capacity(32) { ptr = (T*)std::calloc(capacity, sizeof(T));  }
		Queue(int _size) : front(0), rear(0), capacity(_size) { ptr = (T*)std::calloc(capacity, sizeof(T));  }

		// copy constructor
		Queue(const Queue& other)
			:  capacity(other.capacity), ptr((char*)std::calloc(capacity, sizeof(T))), front(other.front), rear(other.rear)
		{
			std::memcpy(ptr, other.ptr, capacity);
		}
		// move constructor 
		Queue(Queue&& other) :  capacity(other.capacity), ptr(other.ptr), front(other.front), rear(other.rear)
		{
			other.ptr = nullptr;
		}

		void Clear() {
			std::memset(ptr, 0, sizeof(T) * capacity);
			rear = front = 0;
		}

		T* begin() { return ptr + rear;  }
		T* end()   { return ptr + front; }
		const T* cbegin() const { return ptr + rear; }
		const T* cend()   const { return ptr + front; }

		inline bool Any() { return GetSize() > 0; }

		
		void Squeeze()
		{
			const int newSize = GetSize() - rear;
			int first = 0, second = rear;
			while (first < newSize) ptr[first++] = ptr[second++];
		}

		void Enqueue(T value) {
			if (front + 1 >= capacity)
			{
				capacity += 32;
				const int size = GetSize();
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
				std::memmove(ptr, ptr + rear, size * sizeof(T));
				rear = 0;
				front = size;
			}
			ptr[front++] = value;
		}

		void Enqueue(T* begin, T* end)
		{
			const int count = (end - begin) * sizeof(T);

			if (front + count >= capacity)
			{
				capacity += 32 + count;
				const int size = GetSize();
				ptr = (T*)std::realloc(ptr, capacity * sizeof(T));
				std::memmove(ptr, ptr + rear, size * sizeof(T));
				rear = 0;
				front = size;
			}
			std::memcpy(ptr + rear + front, begin, count * sizeof(T));
		}

		// returns true if size is enough
		bool Dequeue(T** result, int count)
		{
			if (GetSize() < count) return false;
			*result = (T*)std::malloc(sizeof(T) * count);
			std::memcpy(result, ptr + rear, sizeof(count));
			rear += count;
			return true;
		}

		T Dequeue() {
			return ptr[rear++];
		}

		bool TryDequeue(T& out)
		{
			if (GetSize() > 0) out = Dequeue();
			return GetSize();
		}

	public:
		T* ptr;
		inline int GetSize() { return front - rear; }
	private:
		int capacity;
		int front = 0;
		int rear  = 0;
	};

	template<typename T, int MaxSize>
	class FixedQueue
	{
	public:
		T ptr[MaxSize];
	public:

		constexpr FixedQueue() { }

		constexpr int GetSize() const { return front - rear; }
		constexpr bool Any() const { return GetSize() > 0; }

		constexpr void Enqueue(T value) { ptr[front++] = value; }

		constexpr T Dequeue() { return ptr[rear++]; }

		constexpr void Squeeze()
		{
			const int newSize = GetSize() - rear;
			int first = 0, second = rear;
			while (first < newSize) ptr[first++] = ptr[second++];
		}

	private:
		int capacity;
		int front = 0;
		int rear = 0;
	};

}