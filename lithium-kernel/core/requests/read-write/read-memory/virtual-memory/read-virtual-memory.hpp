#pragma once
#include <dependencies/includes.hpp>

namespace physical::memory
{
	class c_virtual_reader
	{
	public:
		ALWAYS_INLINE c_virtual_reader( ) = default;
		ALWAYS_INLINE ~c_virtual_reader( ) = default;

		static auto read_virtual_memory( invoke_data* ) -> NTSTATUS;
	};

}
