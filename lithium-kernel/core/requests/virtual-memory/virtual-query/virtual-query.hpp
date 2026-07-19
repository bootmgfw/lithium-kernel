#pragma once

#include <dependencies/includes.hpp>

namespace virtual_query
{
	class c_virtual_query
	{
	public:
		ALWAYS_INLINE c_virtual_query( ) = default;
		ALWAYS_INLINE ~c_virtual_query( ) = default;

		static auto virtual_query( invoke_data* ) -> NTSTATUS;
	};

}
