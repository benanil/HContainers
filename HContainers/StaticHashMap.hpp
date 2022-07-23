#pragma once
#include "Common.hpp"
#include "Pair.hpp"

// you may don't want to use constexpr because of compile times.
// #define HUSTLE_USE_CONSTEXPR_StaticHashMap

#ifdef HUSTLE_USE_CONSTEXPR_StaticHashMap
#	define HSCONSTEXPR constexpr
#else
#	define HSCONSTEXPR 
#endif

namespace HS
{
	// const size, non growable
	// all operations is O(1), and memory allocation, initialization is also fast,
	// chace frendly stack allocated contigues memory
	// 61 is prime number, prime numbers more efficient 
	template<typename Key, typename Value, int Size = 61, int8 BucketSize = 4>
	class StaticHashMap
	{

	private:
		mutable int8 numElements[Size] { 0 };
		mutable KeyValuePair<Key, Value> arr[Size * BucketSize];
	public:

		inline StaticHashMap()  { }
		
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
			arr[arrIndex].key   = key;
			arr[arrIndex].value = value;
		}

		// returns true if removed correctly
		HSCONSTEXPR bool Remove(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size; 
			const uint  valueIndex = bucketIndex * BucketSize;
			const uint8 bucketSize = numElements[bucketIndex];

			for (uint8 i = 0; i < bucketSize; ++i)
			{
				if (arr[valueIndex + i].key == key)
				{
					arr[valueIndex + i] = arr[numElements[bucketIndex]--];
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

			for (uint8 i = 0; i < bucketSize; ++i)
				if (arr[valueIndex + i].key == key) return true;
			
			return false;
		}

		HSCONSTEXPR Value* Find(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size; 
			const uint  valueIndex = bucketIndex * BucketSize;
			uint8 currBucketIndex = numElements[bucketIndex];

			while (currBucketIndex--)
			{
				KeyValuePair<Key, Value>& pair = arr[valueIndex + currBucketIndex];
				if (pair.key == key) return &pair.value;
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
			int8 count = 0;

			for (uint8 i = 0; i < bucketSize; ++i)
				count += arr[valueIndex + i].key == key;

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
					Func(arr[currentBucket * BucketSize + i].value);
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
					arr[currentBucket * BucketSize + numElements[currentBucket]].value.~Value();
				}
			}
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

			HSCONSTEXPR Value operator*() const
			{
				return hashMap->arr[currBucketIndex * BucketSize + currIndex].value;
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
				return &hashMap->arr[currBucketIndex * BucketSize + currIndex].value;
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
		HSCONSTEXPR Iterator end()   const { return Iterator(this, Size-1, BucketSize); }
		
		HSCONSTEXPR Iterator cbegin() const { return Iterator(this, 0, 0); }
		HSCONSTEXPR Iterator cend()   const { return Iterator(this, Size-1, BucketSize); }

	};	

	template<typename Key, typename Value, int Size = 61, int8 BucketSize = 4>
	class StaticSet : public StaticHashMap<Key, Value, Size, BucketSize>
	{
		
	};
}