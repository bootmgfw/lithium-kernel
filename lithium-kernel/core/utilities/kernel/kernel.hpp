#pragma once

#include <dependencies/includes.hpp>

namespace utilities::kernel
{
	namespace pattern {
		uint64_t find_pattern( uint64_t , size_t , const char* , const char* );
		uint64_t find_pattern( uint64_t , const char* , const char* );
		uint64_t find_pattern( uint64_t , const char* , const char* , int );
		uint64_t find_pattern( uint64_t , const char* );

		uintptr_t find_empty_space( uintptr_t , size_t );
	}

	class c_kernel_utils
	{
	public:
		ALWAYS_INLINE c_kernel_utils( ) = default;
		ALWAYS_INLINE ~c_kernel_utils( ) = default;

		static auto get_system_information( SYSTEM_INFORMATION_CLASS ) -> void*;
		static auto get_module( const char* ) -> uint64_t;
		static auto get_nt_callback_address( ) -> uintptr_t;

		static auto write_to_rw_memory( void* , void* , size_t ) -> bool;
		static auto read_from_rw_memory( void* , void* , size_t ) -> bool;

		template<typename T>
		static auto read_rw_memory( uintptr_t address ) -> T {
			if ( !address || address < 0x10000 ) return T {};

			MDL* mdl = IoAllocateMdl( reinterpret_cast< PVOID >( address ) , sizeof( T ) , FALSE , FALSE , NULL );
			if ( !mdl ) return T {};

			__try {
				MmProbeAndLockPages( mdl , KernelMode , IoReadAccess );
				PVOID mapped_address = MmGetSystemAddressForMdlSafe( mdl , NormalPagePriority );

				if ( mapped_address ) {
					T result;
					RtlCopyMemory( &result , mapped_address , sizeof( T ) );
					MmUnlockPages( mdl );
					IoFreeMdl( mdl );
					return result;
				}
			}
			__except ( EXCEPTION_EXECUTE_HANDLER ) {
				IoFreeMdl( mdl );
				return T {};
			}

			IoFreeMdl( mdl );
			return T {};
		}

		static auto get_ntos_base_address( ) -> uintptr_t;
		static auto resolve_address( uintptr_t , ULONG , ULONG ) -> uintptr_t;
		static auto create_trampoline( PVOID , SIZE_T ) -> PVOID;
	};

	void* get_system_information( SYSTEM_INFORMATION_CLASS );
	uint64_t get_module( const char* );
	uintptr_t get_nt_callback_address( );
	bool write_to_rw_memory( void* , void* , size_t );
	bool read_from_rw_memory( void* , void* , size_t );
	uintptr_t get_ntos_base_address( );
	uintptr_t resolve_address( uintptr_t , ULONG , ULONG );
	PVOID create_trampoline( PVOID , SIZE_T );

}
