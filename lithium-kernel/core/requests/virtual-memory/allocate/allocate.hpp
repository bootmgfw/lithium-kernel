#pragma once

#include <dependencies/includes.hpp>

namespace allocation
{
	class c_allocator
	{
	public:
		ALWAYS_INLINE c_allocator( ) = default;
		ALWAYS_INLINE ~c_allocator( ) = default;

		static auto allocate_virtual_memory( invoke_data* ) -> NTSTATUS;
	};

}
