#pragma once

#include <dependencies/includes.hpp>

namespace threads
{
	class c_thread_suspender
	{
	public:
		ALWAYS_INLINE c_thread_suspender( ) = default;
		ALWAYS_INLINE ~c_thread_suspender( ) = default;
	};

}
