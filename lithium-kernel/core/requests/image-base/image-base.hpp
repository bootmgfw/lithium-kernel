#pragma once

#include <dependencies/includes.hpp>

namespace image_base
{
	class c_image_base
	{
	public:
		ALWAYS_INLINE c_image_base( ) = default;
		ALWAYS_INLINE ~c_image_base( ) = default;

		static auto get_image_base( invoke_data* ) -> NTSTATUS;
	};

}
