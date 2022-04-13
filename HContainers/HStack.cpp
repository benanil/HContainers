#include "HStack.hpp"
#include "HContainers.hpp"
#include "HStackStructs.hpp"
#include <iostream>
#include <cassert>

namespace HS
{
	namespace HStack
	{
		byte Bytes[HSTACK_MAX_BYTES];
		LinkedList<Slice*> slices;
	}

	void HStack::Deallocate(Slice* slice)
	{
		std::memset(slice->ptr, 0, slice->sizeInBytes);
		slices.Remove(slice);
	}

	void HStack::Optimize()
	{
		// compress spaces between slices
	}
	
	byte* HStack::DebugPtr() { return Bytes; }

	Slice* HStack::Allocate(int size, int stride)
	{
		static const byte* MaxPtrLocation = Bytes + HSTACK_MAX_BYTES;
		byte* curr = Bytes; // start at beginning of bigdata
		const int sizeInBytes = size * stride;
		LinkedList<Slice*>::Node* node = slices.rootNode;

		while (node && curr <= node->data->ptr + node->data->sizeInBytes)
		{
			curr += sizeInBytes;
			node = node->next;
		}
		// check maximum stack value 
		assert(curr < (byte*)MaxPtrLocation);

		std::memset(curr, 0, sizeInBytes);

		Slice* slice = new Slice(curr, sizeInBytes);
		slices.AddFront(slice);
		return slice;
	}
}