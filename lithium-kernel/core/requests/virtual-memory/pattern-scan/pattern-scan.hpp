#pragma once

#include <dependencies/includes.hpp>


namespace physical::memory
{
	class c_pattern_scanner
	{
	public:
		ALWAYS_INLINE c_pattern_scanner( ) = default;
		ALWAYS_INLINE ~c_pattern_scanner( ) = default;

		static auto pattern_scan( invoke_data* ) -> NTSTATUS;
	};

}
