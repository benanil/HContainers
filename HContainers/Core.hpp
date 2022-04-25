#pragma once

#ifndef HUSTLE_CORE
#define HUSTLE_CORE

#include <iostream>
#include <type_traits>

#ifndef HS_FORCE_INLINE 
#	ifndef _MSC_VER 
#		define HS_FORCE_INLINE inline
#	else
#		define HS_FORCE_INLINE __forceinline
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

#define BitSet  (arg, posn) ((arg) |  (1L << (posn)))
#define BitClear(arg, posn) ((arg) & ~(1L << (posn)))
#define BitFlip (arg, posn) ((arg) ^  (1L << (posn)))

typedef void(*Action)();

using byte = unsigned char;
using uint8 = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using uint64 = unsigned long;

template< typename T, uint N >
constexpr uint HS_ArraySize(const T(&)[N]) { return (uint)N; }

namespace HSCore
{
	// remove flag from enum class
	template<typename Enum_T>
	inline constexpr Enum_T RemoveFlag(Enum_T& flag, Enum_T value)
	{
		using T = std::underlying_type_t<Enum_T>;
		T& result = reinterpret_cast<Enum_T&>(flag);
		result &= ~static_cast<Enum_T>(value);
		return result;
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
}

#endif