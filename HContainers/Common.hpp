#pragma once
#include <cstdlib>

#define HS_ARRAY_IMPL() T& operator[](int index) { return ptr[index]; }                        \
                        const T& operator[](int index) const { return ptr[index]; }            \
                        T* begin() { return ptr; }								               \
                        T* end() { return ptr + size; }							               \
                        const T* cbegin() const { return ptr; }					               \
                        const T* cend() const { return ptr + size; }				           \
                        T GetLast() { return ptr[size - 1]; }                                  \
                        T GetFirst(){ return ptr[0]; }                                         \
						inline bool Any() { return size > 0; }								   \
						void Clear()  { std::memset(ptr, 0, capacity * sizeof(T)); size = 0; }


using uint8  = unsigned char;
using uint16 = unsigned short;
using uint   = unsigned int;
using ulong  = unsigned long;

using int8  = char;
using int16 = short;

namespace HS
{
	namespace Compare
	{
		template<typename T> bool Less(T a, T b) { return a < b; }
		template<typename T> bool Equal(T a, T b) { return a == b; }
		template<typename T> bool NotEqual(T a, T b) { return !Equal(a,b); }
		template<typename T> bool Greater(T a, T b) { return !Less(a, b) && !Equal(a, b); }
		template<typename T> bool GreaterEqual(T a, T b) { return !Less(a, b); }
		template<typename T> bool LessEqual(T a, T b) { return Less(a, b) && Equal(a, b); }
	
		/*for qsort*/ template<typename T>
		int QLess(const void* a, const void* b) { return *(T*)a < *(T*)b; }
		/*for qsort*/ template<typename T>
		int QGreater(const void* a, const void* b) { return *(T*)a > *(T*)b; }
	}

	namespace Hasher
	{
		template <typename T> unsigned int Hash(const T& val)
		{
			return val.Hash();
		}

		template <> unsigned int Hash(float f)
		{
			union Converter { float fVal; uint uval; } converter;
			return converter.uval;
		}

		template <> unsigned int Hash(int in)
		{
			return (unsigned int)in;
		}
	}

	template<typename = T>
	constexpr void MemCpy(T* a, const T* b, int count)
	{
		while (count--) a[count] = b[count];
	}
	
	template<typename = T>
	constexpr void MemSet(T* a, T value, int count)
	{
		while (count--) a[count] = b[count];
	}
	
	template<typename = T>
	constexpr void MemSetRef(T* a, T value, int count)
	{
		while (count--) a[count] = b[count];
	}

	template<typename T>
	T* BinarySearch(T value, T* arr, int size)
	{
		int mid = 0; 
		int low = 0; int high = size;

		while (low != high)
		{
			mid = (low + high) / 2;
			if (value == arr[mid]) return arr + mid;
			else if (value > arr[mid])
				low = mid + 1;
			else
				high = mid - 1;
		}
		return nullptr;
	}	
}

