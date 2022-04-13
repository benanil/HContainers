#include "HStack.hpp"

namespace HS
{
	
	Slice::~Slice()
	{
		if (isValid) HStack::Deallocate(this);
		isValid = false;
	}

	Slice::Slice(byte* _ptr, int _SizeInBytes)
	: ptr(_ptr), sizeInBytes(_SizeInBytes)
	{

	}
}
