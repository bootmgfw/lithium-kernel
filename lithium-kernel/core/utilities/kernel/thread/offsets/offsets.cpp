#include "offsets.hpp"

namespace utilities::kernel::offsets
{
	auto c_offset_discovery::initialize( ) -> bool {
		auto system_process = ( PEPROCESS ) PsInitialSystemProcess;
		if ( !system_process )
			return false;

		for ( auto i = 0; i < 0xFFF; ++i ) {
			if ( !g_unique_process_id && !g_active_process_links ) {
				if ( *( uint64_t* ) ( ( uint64_t ) system_process + i ) == 4 &&
					*( uint64_t* ) ( ( uint64_t ) system_process + i + 0x8 ) > 0xFFFF000000000000 ) {
					g_unique_process_id = i;
					g_active_process_links = i + 0x8;
				}
			}

			if ( !g_image_file_name && !g_active_threads ) {
				if ( *( uint64_t* ) ( ( uint64_t ) system_process + i ) > 0x0000400000000000 && *( UINT64* ) ( ( UINT64 ) system_process + i ) < 0x0000800000000000 &&
					*( uint64_t* ) ( ( uint64_t ) system_process + i + 0x48 ) > 0 && *( UINT64* ) ( ( UINT64 ) system_process + i + 0x48 ) < 0xFFF ) {
					g_image_file_name = i;
					g_active_threads = i + 0x48;
				}
			}

			if ( g_unique_process_id && g_active_process_links && g_image_file_name && g_active_threads )
				return true;
		}

		return false;
	}
}
