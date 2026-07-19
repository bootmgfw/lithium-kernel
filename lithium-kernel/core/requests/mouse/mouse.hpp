#pragma once

#include <dependencies/includes.hpp>

namespace mouse
{
	class c_mouse
	{
	public:
		ALWAYS_INLINE c_mouse( ) = default;
		ALWAYS_INLINE ~c_mouse( ) = default;

		static auto open( ) -> bool;

		static auto set( int x , int y , unsigned short flags , unsigned short button_flags ) -> void;
	};

	inline auto open( ) -> bool { return c_mouse::open( ); }

}
