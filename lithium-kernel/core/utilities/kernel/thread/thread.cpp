#include "thread.hpp"

namespace utilities::kernel::thread
{
	auto c_thread_manager::spoof( void* address , void** save , void* target ) -> void {
		*save = *reinterpret_cast< void** >( address );
		*reinterpret_cast< void** >( address ) = target;
	}

	auto c_thread_manager::hide( ) -> bool {
		auto ntoskrnl = reinterpret_cast< void* >( kernel::get_module( l_enc( "ntoskrnl.exe" ) ) + 0x172b8e );
		if ( !ntoskrnl )
			return false;

		auto current_thread = reinterpret_cast< uint64_t >( KeGetCurrentThread( ) );
		if ( !current_thread )
			return false;

		auto create_time = reinterpret_cast< void* >( current_thread + offsets::g_create_time );
		auto start_address = reinterpret_cast< void* >( current_thread + offsets::g_start_address );
		auto win32_start_address = reinterpret_cast< void* >( current_thread + offsets::g_win32_start_address );
		auto kernel_stack = reinterpret_cast< void* >( current_thread + offsets::g_kernel_stack );
		auto cid = reinterpret_cast< void* >( current_thread + offsets::g_cid );
		auto exit_status = reinterpret_cast< void* >( current_thread + offsets::g_exit_status );

		spoof( create_time , &m_saved_create_time , reinterpret_cast< void* >( 1944358309 ) );
		spoof( start_address , &m_saved_start_address , ntoskrnl );
		spoof( win32_start_address , &m_saved_win32_start_address , ntoskrnl );
		spoof( kernel_stack , &m_saved_kernel_stack );
		spoof( cid , &m_saved_cid );
		spoof( exit_status , &m_saved_exit_status );

		return true;
	}

	auto c_thread_manager::terminate( ) -> bool {
		auto current_thread = reinterpret_cast< uint64_t >( l_import( KeGetCurrentThread )( ) );
		if ( !current_thread )
			return false;

		auto create_time = reinterpret_cast< void* >( current_thread + offsets::g_create_time );
		auto start_address = reinterpret_cast< void* >( current_thread + offsets::g_start_address );
		auto win32_start_address = reinterpret_cast< void* >( current_thread + offsets::g_win32_start_address );
		auto kernel_stack = reinterpret_cast< void* >( current_thread + offsets::g_kernel_stack );
		auto cid = reinterpret_cast< void* >( current_thread + offsets::g_cid );
		auto exit_status = reinterpret_cast< void* >( current_thread + offsets::g_exit_status );

		*reinterpret_cast< void** >( create_time ) = m_saved_create_time;
		*reinterpret_cast< void** >( start_address ) = m_saved_start_address;
		*reinterpret_cast< void** >( win32_start_address ) = m_saved_win32_start_address;
		*reinterpret_cast< void** >( kernel_stack ) = m_saved_kernel_stack;
		*reinterpret_cast< void** >( cid ) = m_saved_cid;
		*reinterpret_cast< void** >( exit_status ) = m_saved_exit_status;

		l_import( PsTerminateSystemThread )( STATUS_SUCCESS );
		return true;
	}
}
