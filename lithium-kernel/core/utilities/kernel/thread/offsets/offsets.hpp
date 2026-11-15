#pragma once

#include <dependencies/includes.hpp>

namespace utilities::kernel::offsets
{
	inline unsigned int g_unique_process_id = 0;
	inline unsigned int g_active_process_links = 0;
	inline unsigned int g_image_file_name = 0;
	inline unsigned int g_active_threads = 0;

	inline unsigned int g_initial_stack = 0x28;
	inline unsigned int g_kernel_stack = 0x58;
	inline unsigned int g_create_time = 0x430;
	inline unsigned int g_start_address = 0x450;
	inline unsigned int g_cid = 0x478;
	inline unsigned int g_win32_start_address = 0x4d0;
	inline unsigned int g_exit_status = 0x548;

	class c_offset_discovery
	{
	public:
		ALWAYS_INLINE c_offset_discovery( ) = default;
		ALWAYS_INLINE ~c_offset_discovery( ) = default;

		static auto initialize( ) -> bool;
	};

	inline auto initialize( ) -> bool { return c_offset_discovery::initialize( ); }

}
