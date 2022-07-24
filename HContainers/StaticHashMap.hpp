#pragma once
#include "Common.hpp"
#include "Pair.hpp"
#include <cassert>

namespace HS
{

	// const size, non growable
	// all operations is O(1), and memory allocation, initialization is also fast,
	// chace frendly stack allocated contigues memory
	// prime numbers more efficient 
	// 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97.
	template<typename Key, typename Value, int Size = 62, int8 BucketSize = 4>
	class StaticHashMap
	{
	public:
		mutable int8 numElements[Size] { 0 };
		mutable Key keys[Size * BucketSize] { };
		mutable Value arr[Size * BucketSize] { };
	public:

		HSCONSTEXPR StaticHashMap()  { }
		
		~StaticHashMap()  { }

		HSCONSTEXPR StaticHashMap(const StaticHashMap& other) // copy constructor
		{
			MemCpy(numElements, other.numElements, BucketSize);
			MemCpy(arr, other.arr, Size * BucketSize);
		}
		
		HSCONSTEXPR StaticHashMap(const StaticHashMap&& other) // move constructor, we cant move just copy
		{
			MemCpy(numElements, other.numElements, BucketSize);
			MemCpy(arr, other.arr, Size * BucketSize);
		}

	public:

		HSCONSTEXPR inline void Insert(const Key& key, const Value& value) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			uint arrIndex = bucketIndex * BucketSize + (numElements[bucketIndex]++);
			assert((bucketIndex < BucketSize || arrIndex < (Size* BucketSize)), "too many object pushed please increase Size or BucketSize");

			keys[arrIndex] = key;
			arr[arrIndex] = value;
		}

		template<typename... T>
		HSCONSTEXPR inline void Emplace(const Key& key, T&&... values) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			uint arrIndex = bucketIndex * BucketSize + (numElements[bucketIndex]++);
			assert((bucketIndex < BucketSize || arrIndex < (Size * BucketSize)), "too many object pushed please increase Size or BucketSize");

			keys[arrIndex] = key;
			arr[arrIndex] = Value(std::forward<T>(values)...);
		}

		// returns true if removed correctly
		HSCONSTEXPR bool Remove(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size; 
			const uint  valueIndex = bucketIndex * BucketSize;
			const uint8 bucketSize = numElements[bucketIndex];
			assert((bucketIndex < BucketSize || valueIndex < (Size * BucketSize)), "too many object pushed please increase Size or BucketSize");
			
			for (uint8 i = 0; i < bucketSize; ++i)
			{
				if (keys[valueIndex + i] == key)
				{
					arr[valueIndex + i] = arr[--numElements[bucketIndex]];
					return true;
				}
			}
			return false;
		}

		HSCONSTEXPR bool Contains(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size; 
			const uint  valueIndex = bucketIndex * BucketSize;
			const uint8 bucketSize = numElements[bucketIndex];
			assert((bucketIndex < BucketSize || valueIndex < (Size * BucketSize)), "too many object pushed please increase Size or BucketSize");

			for (uint8 i = 0; i < bucketSize; ++i)
				if (keys[valueIndex + i] == key) return true;
			
			return false;
		}

		HSCONSTEXPR Value* Find(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size; 
			const uint  valueIndex = bucketIndex * BucketSize;
			uint8 currBucketIndex = numElements[bucketIndex];
			assert((bucketIndex < BucketSize || valueIndex < (Size* BucketSize)), "too many object pushed please increase Size or BucketSize");

			while (currBucketIndex--)
			{
				if (keys[valueIndex + currBucketIndex] == key) return &arr[valueIndex + currBucketIndex];
			}
			return nullptr;
		}

		HSCONSTEXPR inline bool Empty() const
		{
			int currentBucket = Size;
			while (currentBucket--) if (numElements[currentBucket]) return true;
			return true;
		}

		HSCONSTEXPR int8 Count(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size; 
			const uint  valueIndex = bucketIndex * BucketSize;
			const uint8 bucketSize = numElements[bucketIndex];
			assert((bucketIndex < BucketSize || valueIndex < (Size* BucketSize)), "too many object pushed please increase Size or BucketSize");
			int8 count = 0;

			for (uint8 i = 0; i < bucketSize; ++i)
				count += keys[valueIndex + i] == key;

			return count;
		}

		HSCONSTEXPR inline int8 CountBucket(const Key& key) const
		{
			return numElements[Hasher::Hash(key) % Size];
		}

		HSCONSTEXPR void Iterate(void(*Func)(float&))
		{
			int currentBucket = Size;
			while (currentBucket--)
			{
				for (int i = 0; i < numElements[currentBucket]; ++i)
				{
					Func(arr[currentBucket * BucketSize + i]);
				}
			}
		}

		HSCONSTEXPR void Clear() 
		{
			int currentBucket = Size;
			while (currentBucket--)
			{
				while (numElements[currentBucket]--)
				{
					arr[currentBucket * BucketSize + numElements[currentBucket]].~Value();
				}
				numElements[currentBucket] = 0;
			}
		}

		// calls default constructor for each element
		HSCONSTEXPR inline void Initialize()
		{
			for (int i = 0; i < Size * BucketSize; ++i) arr[i] = Value();
		}

		HSCONSTEXPR inline Value& operator[](const Key& key) const
		{
			return *Find(key);
		}

		class Iterator
		{
		public:
			int currBucketIndex = 0;
			int currIndex = 0;
			const StaticHashMap* hashMap;
		public:
			HSCONSTEXPR Iterator(const StaticHashMap* map, int cBucketIndex, int cIndex)
			: hashMap(map), currBucketIndex(cBucketIndex), currIndex(cIndex) { }

			HSCONSTEXPR Value& operator*() const
			{
				return hashMap->arr[currBucketIndex * BucketSize + currIndex];
			}

			// prefix increment
			HSCONSTEXPR Iterator& operator++()
			{
				currBucketIndex += ++currIndex >= hashMap->numElements[currBucketIndex];
				currIndex *= currIndex < hashMap->numElements[currBucketIndex];
				return *this;
			}

			// postfix increment
			HSCONSTEXPR Iterator operator++(int amount)
			{
				currBucketIndex += currIndex++ >= hashMap->numElements[currBucketIndex];
				currIndex *= currIndex < hashMap->numElements[currBucketIndex];
				return *this;
			}

			HSCONSTEXPR Value* operator->() 
			{
				return &hashMap->arr[currBucketIndex * BucketSize + currIndex];
			}

			HSCONSTEXPR bool operator == (const Iterator& other) const 
			{
				return currBucketIndex == other.currBucketIndex && currIndex == other.currIndex;
			}

			HSCONSTEXPR bool operator != (const Iterator& other) const 
			{
				return currBucketIndex != other.currBucketIndex || currIndex != other.currIndex;
			}

			HSCONSTEXPR bool operator < (const Iterator& other) const
			{
				return currBucketIndex * BucketSize + currIndex < other.currBucketIndex * BucketSize + other.currIndex;
			}

			HSCONSTEXPR bool operator > (const Iterator& other) const
			{
				return currBucketIndex * BucketSize + currIndex > other.currBucketIndex * BucketSize + other.currIndex;
			}
		};

		HSCONSTEXPR Iterator begin() const { return Iterator(this, 0, 0); }
		HSCONSTEXPR Iterator end()   const { return Iterator(this, Size-1, BucketSize-1); }
		
		HSCONSTEXPR Iterator cbegin() const { return Iterator(this, 0, 0); }
		HSCONSTEXPR Iterator cend()   const { return Iterator(this, Size-1, BucketSize-1); }

	};	

	template<typename Key, typename Value, int Size = 61, int8 BucketSize = 4>
	class StaticSet : public StaticHashMap<Key, Value, Size, BucketSize>
	{
		
	};
}