#pragma once

#include <dependencies/includes.hpp>

namespace physical
{
	class c_physical_memory
	{
	public:
		ALWAYS_INLINE c_physical_memory( ) = default;
		ALWAYS_INLINE ~c_physical_memory( ) = default;

		static auto read_physical_address( PVOID , PVOID , SIZE_T , SIZE_T* ) -> NTSTATUS;
		static auto translate_linear_address( uint64_t , uint64_t ) -> uint64_t;
		static auto write_physical_address( PVOID , PVOID , SIZE_T , SIZE_T* ) -> NTSTATUS;
		static auto write_physical( uintptr_t , PVOID , size_t , size_t* ) -> NTSTATUS;
		static auto find_min( INT32 , SIZE_T ) -> ULONG64;
	};

}
