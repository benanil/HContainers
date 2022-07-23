#pragma once
#include "Common.hpp"
#include <type_traits>

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
	template<typename Key, typename Value, int Size = 61, int BucketSize = 4>
	class StaticHashMap
	{
	public:
		constexpr int HArraySize = Size * BucketSize;
		
		int8 numElements[BucketSize] = { 0 };
		T arr[Size * BucketSize];
	public:
		HSCONSTEXPR inline StaticHashMap() {  }

		~StaticHashMap() 
		{
			MemSet(numElements, 0, BucketSize);
			if constexpr (std::is_destructible<Value>())
			{
				for (int i = 0; i < HArraySize; ++i)
					arr[i].~Value();
			}
		}

		HSCONSTEXPR StaticHashMap(const StaticHashMap& other) // copy constructor
		{
			MemCpy(other.numElements, numElements, BucketSize);
			MemCpy(other.arr, arr, HArraySize);
		}
		
		HSCONSTEXPR StaticHashMap(const StaticHashMap&& other) // move constructor, we cant move just copy
		{
			MemCpy(other.numElements, numElements, BucketSize);
			MemCpy(other.arr, arr, HArraySize);
		}

	public:

		HSCONSTEXPR inline void Insert(const Key& key, const Value& value) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			arr[bucketIndex * BucketSize + (numElements[bucketIndex]++)] = value;
		}

		// returns true if removed correctly
		HSCONSTEXPR bool Remove(const Key& key, const Value& value) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			for (int i = bucketIndex * BucketSize, end = i + BucketSize; i < end; ++i)
			{
				if (Compare::Equals(value, arr[i]))
				{
					int matchIndex = bucketIndex * BucketSize + i;
					arr[matchIndex] = arr[matchIndex + numElements[bucketIndex]--];
				}
			}
			return false;
		}

		HSCONSTEXPR bool Contains(const Key& key) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			for (int i = bucketIndex * BucketSize, end = i + BucketSize; i < end; ++i)
			{
				if (Compare::Equals(value, arr[i]))
					return true;
			}
			return false;
		}

		HSCONSTEXPR Value& Find(const Key& key) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			for (int i = bucketIndex * BucketSize, end = i + BucketSize; i < end; ++i)
			{
				if (Compare::Equals(value, arr[i]))
					return arr[i];
			}
			return Value();
		}

		HSCONSTEXPR inline  bool Empty() const
		{
			int bucketIndex = Size;
			while (bucketIndex--)
				if (numElements[bucketIndex]) return false;
			return true;
		}

		HSCONSTEXPR int8 Count(const Key& key) const
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			int count = 0;
			for (int i = bucketIndex * BucketSize, end = i + BucketSize; i < end; ++i)
				if (Compare::Equals(value, arr[i]))
					++count;
			return count;
		}

		HSCONSTEXPR inline int8 CountBucket(const Key& key) const
		{
			return numElements[Hasher::Hash(key) % Size];
		}

		HSCONSTEXPR void Clear() const
		{
			int bucketIndex = Size-1;
			while (bucketIndex--)
			{
				int i = numElements[bucketIndex];
				if constexpr (std::is_destructible<Value>())
					while (i--)
						arr[bucketIndex * BucketSize + i]~Value();

				numElements[bucketIndex] = 0;
			}
		}

		HSCONSTEXPR inline Value& operator[](const Key& key) const
		{
			return arr[(Hasher::Hash(key) % Size) * BucketSize];
		}

		HSCONSTEXPR inline const Value& operator[](const Key& key) const
		{
			return arr[(Hasher::Hash(key) % Size) * BucketSize];
		}

		class Iterator
		{
		private:
			int currBucketIndex = 0;
			int currIndex = 0;
			const StaticHashMap* hashMap;
		public:
			HSCONSTEXPR Iterator(const StaticHashMap* map, int cBucketIndex, int cIndex)
			: hashMap(map), currBucketIndex(cBucketIndex), currIndex(cIndex) { }

			// prefix increment
			HSCONSTEXPR Iterator& operator++()
			{
				currBucketIndex += currIndex++ >= hashMap->numElements[currBucketIndex];
				return *this;
			}

			// postfix increment
			HSCONSTEXPR Iterator operator++(int amount)
			{
				currBucketIndex += currIndex++ >= hashMap->numElements[currBucketIndex];
				return this;
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
		};

		HSCONSTEXPR Iterator begin() const { return Iterator(this, numElements, 0, 0); }
		HSCONSTEXPR Iterator end()   const { return Iterator(this, numElements, Size, BucketSize); }
		
		HSCONSTEXPR Iterator cbegin() const { return Iterator(this, numElements, 0, 0); }
		HSCONSTEXPR Iterator cend()   const { return Iterator(this, numElements, Size, BucketSize); }

	};	
}