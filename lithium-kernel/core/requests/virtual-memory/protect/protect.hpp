#pragma once

#include <dependencies/includes.hpp>

namespace protection
{
	class c_memory_protector
	{
	public:
		ALWAYS_INLINE c_memory_protector( ) = default;
		ALWAYS_INLINE ~c_memory_protector( ) = default;

		static auto protect_virtual_memory( invoke_data* ) -> NTSTATUS;
	};

}
