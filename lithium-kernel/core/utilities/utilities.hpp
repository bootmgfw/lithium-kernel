#pragma once

#include <dependencies/includes.hpp>

namespace utilities
{
	class c_pool_cleaner
	{
	public:
		ALWAYS_INLINE c_pool_cleaner( ) = default;
		ALWAYS_INLINE ~c_pool_cleaner( ) = default;

		static auto clean_mdl( PMDL mdl ) -> NTSTATUS;
		static auto clean_big_pools( uint64_t pool ) -> NTSTATUS;

	private:
		static auto resolve_relative_address( void* instruction , unsigned long offset , unsigned long instruction_size ) -> void*;
		static auto find_pool_table( uint64_t* ppool_big_table , uint64_t* ppool_big_table_size ) -> bool;
	};

}
