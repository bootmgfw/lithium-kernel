#include "physical.hpp"

namespace physical
{
	auto c_physical_memory::read_physical_address( PVOID target_address , PVOID lp_buffer , SIZE_T size , SIZE_T* bytes_read ) -> NTSTATUS {
		MM_COPY_ADDRESS copy = { 0 };
		copy.PhysicalAddress.QuadPart = ( LONGLONG ) target_address;
		return l_import( MmCopyMemory )( lp_buffer , copy , size , MM_COPY_MEMORY_PHYSICAL , bytes_read );
	}

	auto c_physical_memory::translate_linear_address( uint64_t directory_table_base , uint64_t virtual_address ) -> uint64_t {
		static const uintptr_t PMASK = ( ~0xfull << 8 ) & 0xfffffffffull;

		directory_table_base &= ~0xf;

		uintptr_t page_offset = virtual_address & ~( ~0ul << 12 );
		uintptr_t pte = ( ( virtual_address >> 12 ) & ( 0x1ffll ) );
		uintptr_t pt = ( ( virtual_address >> 21 ) & ( 0x1ffll ) );
		uintptr_t pd = ( ( virtual_address >> 30 ) & ( 0x1ffll ) );
		uintptr_t pdp = ( ( virtual_address >> 39 ) & ( 0x1ffll ) );

		size_t readsize = 0;
		uintptr_t pdpe = 0;

		read_physical_address( PVOID( directory_table_base + 8 * pdp ) , &pdpe , sizeof( pdpe ) , &readsize );
		if ( ~pdpe & 1 ) {
			return 0;
		}

		uintptr_t pde = 0;
		read_physical_address( PVOID( ( pdpe & PMASK ) + 8 * pd ) , &pde , sizeof( pde ) , &readsize );

		if ( ~pde & 1 ) {
			return 0;
		}

		if ( pde & 0x80 ) {
			return ( pde & ( ~0ull << 42 >> 12 ) ) + ( virtual_address & ~( ~0ull << 30 ) );
		}

		uintptr_t pte_addr = 0;
		read_physical_address( PVOID( ( pde & PMASK ) + 8 * pt ) , &pte_addr , sizeof( pte_addr ) , &readsize );

		if ( ~pte_addr & 1 ) {
			return 0;
		}

		if ( pte_addr & 0x80 ) {
			return ( pte_addr & PMASK ) + ( virtual_address & ~( ~0ull << 21 ) );
		}

		virtual_address = 0;
		read_physical_address( PVOID( ( pte_addr & PMASK ) + 8 * pte ) , &virtual_address , sizeof( virtual_address ) , &readsize );
		virtual_address &= PMASK;

		if ( !virtual_address ) {
			return 0;
		}

		return virtual_address + page_offset;
	}

	auto c_physical_memory::write_physical( uintptr_t address , PVOID buffer , size_t size , size_t* bytes ) -> NTSTATUS
	{
		if ( !address )
			return STATUS_UNSUCCESSFUL;

		PHYSICAL_ADDRESS AddrToWrite = { 0 };
		AddrToWrite.QuadPart = ( LONGLONG ) address;

		PVOID pmapped_mem = l_import( MmMapIoSpaceEx )( AddrToWrite , size , PAGE_READWRITE );

		if ( !pmapped_mem )
			return STATUS_UNSUCCESSFUL;

		utilities::crt::memcpy( pmapped_mem , buffer , size );

		*bytes = size;
		l_import( MmUnmapIoSpace )( pmapped_mem , size );

		return STATUS_SUCCESS;
	}

	auto c_physical_memory::write_physical_address( PVOID target_address , PVOID lp_buffer , SIZE_T size , SIZE_T* bytes_written ) -> NTSTATUS {
		if ( !target_address ) {
			return STATUS_UNSUCCESSFUL;
		}

		PHYSICAL_ADDRESS addr_to_write = { 0 };
		addr_to_write.QuadPart = LONGLONG( target_address );

		PVOID pmapped_mem = l_import( MmMapIoSpaceEx )( addr_to_write , size , PAGE_READWRITE );
		if ( !pmapped_mem ) {
			return STATUS_UNSUCCESSFUL;
		}

		__movsb( PBYTE( pmapped_mem ) , PBYTE( lp_buffer ) , size );

		*bytes_written = size;
		l_import( MmUnmapIoSpace )( pmapped_mem , size );

		return STATUS_SUCCESS;
	}


	auto c_physical_memory::find_min( INT32 g , SIZE_T f ) -> ULONG64
	{
		INT32 h = ( INT32 ) f;
		ULONG64 result = 0;

		result = ( ( ( g ) < ( h ) ) ? ( g ) : ( h ) );

		return result;
	}
}
