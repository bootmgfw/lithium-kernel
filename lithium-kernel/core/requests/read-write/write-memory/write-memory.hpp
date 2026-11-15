#pragma once

#include <dependencies/includes.hpp>

namespace physical::memory
{
	class c_memory_writer
	{
	public:
		ALWAYS_INLINE c_memory_writer( ) = default;
		ALWAYS_INLINE ~c_memory_writer( ) = default;

		static auto write_memory( invoke_data* ) -> NTSTATUS;
	};

}
