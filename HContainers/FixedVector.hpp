#pragma once
#include "Common.hpp"

namespace HS
{
	template<typename T, int MaxSize>
	class FixedArray
	{
	private:
		int size = 0;
	public:
		T arr[MaxSize];
		void Add(const T& value)
		{
			arr[size++] = value;
		}

		HArrayReslut AddAtIndex(int index, const T& value)
		{
			if (index >= size || index <= 0) return HArrayResult::IndexBoundsOutOfArray;
			int slow = size;
			int fast = size-1;
			while (fast >= index) arr[slow--] = arr[fast--];
			arr[index] = value;
			return HArrayResult::Success;
		}

		HArrayReslut RemoveUnordered(int index)
		{
			if (index >= size || index <= 0) return HArrayResult::IndexBoundsOutOfArray;
			arr[index] = arr[--size];
			return HArrayResult::Success;
		}

		// linear search
		HArrayReslut FindIndex(const T& val)
		{
			for (int i = 0; i < size; ++i) if (arr[i] == val) return i;
			return -1;
		}

		HArrayReslut Remove(const T& val)
		{
			int findIndex = FindIndex(val);
			if(findIndex == -1) return HArrayResult::NotFinded;
			int right = findIndex + 1;

			while (right < size) arr[findIndex++] = arr[right++];

			return HArrayReslut::Success;
		}

		void RemoveLast() { size--; }

		int RemoveAll(const T& value)
		{
			return RemoveAll(Compare::Equals<T>); // removed item count
		}

		int RemoveAll(bool(*match)(const T&))
		{
			int freeIndex = 0;   // the first free slot in items array

			// Find the first item which needs to be removed.
			while (freeIndex < size && !match(arr[freeIndex]) ) freeIndex++;
			if (freeIndex >= size) return 0;

			int current = freeIndex + 1;
			while (current < size) {
				// Find the first item which needs to be kept.
				while (current < size && match(arr[current]) ) current++;

				if (current < size) {
					// copy item to the free slot.
					arr[freeIndex++] = arr[current++];
				}
			}

			memset(arr + freeIndex, 0, size - freeIndex);
			int result = size - freeIndex;
			size = freeIndex;

			return result; // removed item count
		}

		T* begin() { return arr; }
		T* end()   { return arr + size; }
		const T* cbegin() const { return arr; }
		const T* cend()   const { return arr + size; }
	};
}