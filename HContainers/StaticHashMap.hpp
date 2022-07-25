#pragma once
#include "Common.hpp"
#include <utility>
#include <cassert>

namespace HS
{
	// const size, non growable
	// all operations is O(1), and memory allocation, initialization is also fast,
	// chace frendly stack allocated contigues memory
	template<typename Key, typename Value, int Size = 90, int8 BucketSize = 32>
	class StaticHashMap
	{
	private:
		int8 numElements[Size] { 0 };
		Key keys[Size * BucketSize] { };
		mutable Value arr[Size * BucketSize] { };
	public:

		HSCONSTEXPR StaticHashMap()  { }

		~StaticHashMap()  { }

		HSCONSTEXPR StaticHashMap(const StaticHashMap& other) // copy constructor
		{
			ArrayCpy(numElements, other.numElements, BucketSize);
			ArrayCpy(arr, other.arr, Size * BucketSize);
		}

		HSCONSTEXPR StaticHashMap(const StaticHashMap&& other) // move constructor, we cant move just copy
		{
			ArrayMove(numElements, other.numElements, BucketSize);
			ArrayMove(arr, other.arr, Size * BucketSize);
		}

	public:

		HSCONSTEXPR inline void Insert(const Key& key, Value&& value)
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			uint arrIndex = bucketIndex * BucketSize + (numElements[bucketIndex]++);
			assert(bucketIndex < BucketSize); // BucketSize is small
			assert(arrIndex < (Size * BucketSize)); // Size is small
			keys[arrIndex] = key;
			arr[arrIndex] = std::move(value);
		}

		template<typename... T>
		HSCONSTEXPR inline void Emplace(const Key& key, T&&... values)
		{
			uint bucketIndex = Hasher::Hash(key) % Size;
			uint arrIndex = bucketIndex * BucketSize + (numElements[bucketIndex]++);
			assert(bucketIndex < BucketSize); // BucketSize is small
			assert(arrIndex < (Size * BucketSize)); // Size is small

			keys[arrIndex] = key;
			arr[arrIndex] = Value(std::forward<T>(values)...);
		}

		// returns true if removed correctly
		HSCONSTEXPR bool Remove(const Key& key)
		{
			const uint bucketIndex = Hasher::Hash(key) % Size;
			const uint  valueIndex = bucketIndex * BucketSize;
			const uint8 bucketSize = numElements[bucketIndex];

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

			for (uint8 i = 0; i < bucketSize; ++i)
				if (keys[valueIndex + i] == key) return true;

			return false;
		}

		HSCONSTEXPR Value* Find(const Key& key)
		{
			const uint bucketIndex = Hasher::Hash(key) % Size;
			const uint  valueIndex = bucketIndex * BucketSize;
			uint8 currBucketIndex = numElements[bucketIndex];

			while (currBucketIndex--)
			{
				if (keys[valueIndex + currBucketIndex] == key) return &arr[valueIndex + currBucketIndex];
			}
			return nullptr;
		}

		HSCONSTEXPR Value* FindOrCreate(const Key& key)
		{
			const uint bucketIndex = Hasher::Hash(key) % Size;
			uint  valueIndex = bucketIndex * BucketSize;
			uint8 currBucketIndex = numElements[bucketIndex];
			while (currBucketIndex--)
			{
				if (keys[valueIndex + currBucketIndex] == key) return &arr[valueIndex + currBucketIndex];
			}

			valueIndex += numElements[bucketIndex]++;

			assert(bucketIndex < BucketSize); // BucketSize is small
			assert(valueIndex  < (Size * BucketSize)); // Size is small

			keys[valueIndex] = key;
			arr[valueIndex] = Value();
			return &arr[valueIndex] ;
		}

		HSCONSTEXPR inline bool Empty() const
		{
			int currentBucket = Size;
			while (currentBucket--) if (numElements[currentBucket]) return true;
			return false;
		}

		HSCONSTEXPR int8 Count(const Key& key) const
		{
			const uint bucketIndex = Hasher::Hash(key) % Size;
			const uint  valueIndex = bucketIndex * BucketSize;
			const uint8 bucketSize = numElements[bucketIndex];
			int8 count = 0;

			for (uint8 i = 0; i < bucketSize; ++i)
				count += keys[valueIndex + i] == key;

			return count;
		}

		[[maybe_unused]] HSCONSTEXPR int8 CountBucket(const Key& key) const
		{
			return numElements[Hasher::Hash(key) % Size];
		}

		template<typename Callable_t>
		HSCONSTEXPR void Iterate(const Callable_t& func)
		{
			int currentBucket = Size;
			while (currentBucket--)
			{
				for (int i = 0; i < numElements[currentBucket]; ++i)
				{
					func(arr[currentBucket * BucketSize + i]);
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

		HSCONSTEXPR void Reload()
		{
			int currentBucket = Size;
			while (currentBucket--)
			{
				while (numElements[currentBucket]--)
				{
					arr[currentBucket * BucketSize + numElements[currentBucket]].~Value();
					arr[currentBucket * BucketSize + numElements[currentBucket]] = Value();
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
				currBucketIndex += ++currIndex >= hashMap->numElements[currBucketIndex];
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

		class ConstIterator
		{
		public:
			int currBucketIndex = 0;
			mutable int currIndex = 0;
			const StaticHashMap* hashMap;
		public:
			HSCONSTEXPR ConstIterator(const StaticHashMap* map, int cBucketIndex, int cIndex)
				: hashMap(map), currBucketIndex(cBucketIndex), currIndex(cIndex) { }

			HSCONSTEXPR const Value& operator*() const
			{
				return hashMap->arr[currBucketIndex * BucketSize + currIndex];
			}

			// prefix increment
			HSCONSTEXPR const ConstIterator& operator++() const
			{
				currBucketIndex += ++currIndex >= hashMap->numElements[currBucketIndex];
				currIndex *= currIndex < hashMap->numElements[currBucketIndex];
				return *this;
			}

			// postfix increment
			HSCONSTEXPR const ConstIterator operator++(int amount) const
			{
				currBucketIndex += ++currIndex >= hashMap->numElements[currBucketIndex];
				currIndex *= currIndex < hashMap->numElements[currBucketIndex];
				return *this;
			}

			HSCONSTEXPR const Value* operator->() const
			{
				return &hashMap->arr[currBucketIndex * BucketSize + currIndex];
			}

			HSCONSTEXPR bool operator == (const ConstIterator& other) const
			{
				return currBucketIndex == other.currBucketIndex && currIndex == other.currIndex;
			}
			HSCONSTEXPR bool operator != (const ConstIterator& other) const
			{
				return currBucketIndex != other.currBucketIndex || currIndex != other.currIndex;
			}
			HSCONSTEXPR bool operator < (const ConstIterator& other) const
			{
				return currBucketIndex * BucketSize + currIndex < other.currBucketIndex * BucketSize + other.currIndex;
			}
			HSCONSTEXPR bool operator > (const ConstIterator& other) const
			{
				return currBucketIndex * BucketSize + currIndex > other.currBucketIndex * BucketSize + other.currIndex;
			}
		};

		HSCONSTEXPR Iterator begin() const { return Iterator(this, 0, 0); }
		HSCONSTEXPR Iterator end()   const { return Iterator(this, Size-1, BucketSize-1); }

		HSCONSTEXPR ConstIterator cbegin() const { return Iterator(this, 0, 0); }
		HSCONSTEXPR ConstIterator cend()   const { return Iterator(this, Size-1, BucketSize-1); }

	};

	template<typename Key, typename Value, int Size = 61, int8 BucketSize = 4>
	class StaticSet : public StaticHashMap<Key, Value, Size, BucketSize>
	{
		
	};
}