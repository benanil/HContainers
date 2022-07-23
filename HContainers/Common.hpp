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


#ifndef FINLINE
#	ifndef _MSC_VER 
#		define FINLINE inline
#	else
#		define FINLINE __forceinline
#	endif
#endif 

// coppied from here: winnt.h line 2481  DEFINE_ENUM_FLAG_OPERATORS we are not using this because we don't want to include winnt.h
// Define operator overloads to enable bit operations on enum values that are
// used to define flags. Use HS_CREATE_ENUM_OPERATORS(YOUR_TYPE) to enable these
// Templates are defined here in order to avoid a dependency on C++ <type_traits> header file,
template <size_t S> struct _ENUM_TO_INT;
template <> struct _ENUM_TO_INT<1> { typedef char  type; };
template <> struct _ENUM_TO_INT<2> { typedef short type; };
template <> struct _ENUM_TO_INT<4> { typedef int  type; };
template <> struct _ENUM_TO_INT<8> { typedef long type; };
// used as an approximation of std::underlying_type<T>
template <class T> struct UnderlyingType {
	typedef typename _ENUM_TO_INT<sizeof(T)>::type type;
};

#define HS_CREATE_ENUM_OPERATORS(ENUMTYPE) \
inline constexpr ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((UnderlyingType<ENUMTYPE>::type)a) | ((UnderlyingType<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE&)(((UnderlyingType<ENUMTYPE>::type&)a) |= ((UnderlyingType<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((UnderlyingType<ENUMTYPE>::type)a) & ((UnderlyingType<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE&)(((UnderlyingType<ENUMTYPE>::type&)a) &= ((UnderlyingType<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator ~ (ENUMTYPE a) noexcept { return ENUMTYPE(~((UnderlyingType<ENUMTYPE>::type)a)); } \
inline constexpr ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) noexcept { return ENUMTYPE(((UnderlyingType<ENUMTYPE>::type)a) ^ ((UnderlyingType<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) noexcept { return (ENUMTYPE&)(((UnderlyingType<ENUMTYPE>::type&)a) ^= ((UnderlyingType<ENUMTYPE>::type)b)); } 

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
			return 0;
		}

		template <typename float> unsigned int Hash(float f)
		{
			union Converter { float fVal; uint uval; } converter;
			return converter.uval;
		}

		template <typename int> unsigned int Hash(int in)
		{
			return (unsigned int)in;
		}
	}

	// very fast hash function + compile time
	static inline constexpr uint KnuthHash(uint a, uint shift)
	{
		const uint knuth_hash = 2654435769u;
		return ((a * knuth_hash) >> shift);
	}

	static inline constexpr void hash_combine(uint& s, const uint v, uint shift)
	{
		s ^= KnuthHash(v, shift) + 0x9e3779b9 + (s << 6) + (s >> 2);
	}

	// very fast hash function + compile time
	static inline constexpr uint StringToHash(const char* string)
	{
		uint hash = KnuthHash(string[0], 0);

		for (uint i = 1; i < __builtin_strlen(string); ++i) {
			hash_combine(hash, uint(string[i]), i);
		}

		return hash;
	}

	template<typename T>
	constexpr void MemCpy(T* a, const T* b, int count)
	{
		while (count--) a[count] = b[count];
	}
	
	template<typename T>
	constexpr void MemSet(T* a, T value, int count)
	{
		while (count--) a[count] = value;
	}
	
	template<typename T>
	constexpr void MemSetRef(T* a, T value, int count)
	{
		while (count--) a[count] = value;
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

