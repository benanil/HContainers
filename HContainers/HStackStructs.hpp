#pragma once
#include "Core.hpp"

namespace HS
{
	class Slice
	{
	public:
		~Slice();
		Slice(byte* _ptr, int _sizeInBytes);
		template<typename T>
		T* As() { return reinterpret_cast<T*>(ptr); }
		bool IsValid() const { return isValid; }
	private:
		bool isValid = true; 
	public:
		mutable int sizeInBytes;
		mutable byte* ptr;
	};

}