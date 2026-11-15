#pragma once

#include <dependencies/includes.hpp>

struct cache {
	uintptr_t Address;
	MMPTE Value;
};

inline cache cached_pml4e [ 512 ];
inline cache cached_pdpte [ 512 ];
inline cache cached_pde [ 512 ];
inline cache cached_pte [ 512 ];

namespace directory_table_base
{
	class c_directory_table
	{
	public:
		ALWAYS_INLINE c_directory_table( ) = default;
		ALWAYS_INLINE ~c_directory_table( ) = default;

		static auto resolve_dtb( invoke_data* ) -> NTSTATUS;
		static auto get_physical_ranges( invoke_data* ) -> NTSTATUS;

	private:
		static auto dtb_cache( ) -> bool;
	};

	namespace pml4
	{
		auto split_memory( PVOID , SIZE_T , const void* , SIZE_T ) -> PVOID;

		inline void* g_mmonp_MmPfnDatabase;

		static auto InitializeMmPfnDatabase( ) -> NTSTATUS;

		auto dirbase_from_base_address( void* ) -> uintptr_t;
		auto translate_linear( uintptr_t , uintptr_t ) -> uintptr_t;
		auto virtual_to_physical( uintptr_t , uintptr_t ) -> uintptr_t;
	}

}
