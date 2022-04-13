#pragma once

#ifndef HSTACK_MAX_BYTES // 128 megabyte
#	define HSTACK_MAX_BYTES (128'000'000) 
#endif

#include <memory>
#include "HStackStructs.hpp"

namespace HS
{
	namespace HStack
	{
		Slice* Allocate(int size, int stride);
		// compress spaces between slices
		void Optimize();
		byte* DebugPtr();
		void Deallocate(Slice* slice);
	}
}