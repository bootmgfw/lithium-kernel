#pragma once
#include <dependencies/includes.hpp>

namespace physical::memory
{
	class c_virtual_writer
	{
	public:
		ALWAYS_INLINE c_virtual_writer( ) = default;
		ALWAYS_INLINE ~c_virtual_writer( ) = default;

		static auto write_virtual_memory( invoke_data* ) -> NTSTATUS;
	};

}
