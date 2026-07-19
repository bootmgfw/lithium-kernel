#pragma once

#include <dependencies/includes.hpp>

namespace physical::memory
{
	class c_memory_reader
	{
	public:
		ALWAYS_INLINE c_memory_reader( ) = default;
		ALWAYS_INLINE ~c_memory_reader( ) = default;

		static auto read_memory( invoke_data* ) -> NTSTATUS;
	};

}
