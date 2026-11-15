#pragma once

#include <dependencies/includes.hpp>

namespace utilities::kernel::thread
{
	class c_thread_manager
	{
		inline static void* m_saved_create_time;
		inline static void* m_saved_start_address;
		inline static void* m_saved_win32_start_address;
		inline static void* m_saved_kernel_stack;
		inline static void* m_saved_cid;
		inline static void* m_saved_exit_status;

		static auto spoof( void* address , void** save , void* target = 0 ) -> void;

	public:
		ALWAYS_INLINE c_thread_manager( ) = default;
		ALWAYS_INLINE ~c_thread_manager( ) = default;

		static auto hide( ) -> bool;
		static auto terminate( ) -> bool;
	};

	inline auto hide( ) -> bool { return c_thread_manager::hide( ); }
	inline auto terminate( ) -> bool { return c_thread_manager::terminate( ); }

}
