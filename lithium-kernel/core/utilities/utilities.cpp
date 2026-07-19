#include "utilities.hpp"

namespace utilities
{
	auto c_pool_cleaner::clean_mdl( PMDL mdl ) -> NTSTATUS
	{
		PPFN_NUMBER mdl_pages = MmGetMdlPfnArray( mdl );
		if ( !mdl_pages )
		{
			return STATUS_UNSUCCESSFUL;
		}

		ULONG mdl_page_count = ADDRESS_AND_SIZE_TO_SPAN_PAGES( MmGetMdlVirtualAddress( mdl ) , MmGetMdlByteCount( mdl ) );
		ULONG null_pfn = 0x0;

		MM_COPY_ADDRESS source_address = { 0 };
		source_address.VirtualAddress = &null_pfn;

		for ( ULONG i = 0; i < mdl_page_count; i++ )
		{
			size_t bytes = 0;
			l_import( MmCopyMemory )( &mdl_pages [ i ] , source_address , sizeof( ULONG ) , MM_COPY_MEMORY_VIRTUAL , &bytes );
		}

		return STATUS_SUCCESS;
	}

	auto c_pool_cleaner::resolve_relative_address( void* instruction , unsigned long offset , unsigned long instruction_size ) -> void*
	{
		uint64_t instric = ( uint64_t ) instruction;
		long rip_offset = *( long* ) ( instric + offset );
		void* resolved = ( void* ) ( instric + instruction_size + rip_offset );

		return resolved;
	}

	auto c_pool_cleaner::find_pool_table( uint64_t* ppool_big_table , uint64_t* ppool_big_table_size ) -> bool {
		auto ntoskrnl = kernel::get_module( l_enc( "ntoskrnl.exe" ) );
		if ( !ntoskrnl )
			return false;

		IMAGE_DOS_HEADER* dos_header = reinterpret_cast< IMAGE_DOS_HEADER* >( ntoskrnl );
		IMAGE_NT_HEADERS* nt_header = reinterpret_cast< IMAGE_NT_HEADERS* >( ntoskrnl + dos_header->e_lfanew );

		if ( dos_header->e_magic != 0x5a4d )
			return false;

		IMAGE_OPTIONAL_HEADER* optional_header = reinterpret_cast< IMAGE_OPTIONAL_HEADER* >( &nt_header->OptionalHeader );

		void* ex_protect_pool_ex_ptr = ( void* ) kernel::pattern::find_pattern( ( uint64_t ) ntoskrnl , optional_header->SizeOfImage , l_enc( "\xE8\x00\x00\x00\x00\x83\x67\x0C\x00" ) , l_enc( "x????xxxx" ) );

		if ( !ex_protect_pool_ex_ptr )
			return false;

		void* ex_protect_pool_ex = resolve_relative_address( ex_protect_pool_ex_ptr , 1 , 5 );

		if ( !ex_protect_pool_ex )
			return false;

		void* pool_big_table_ptr = ( void* ) ( ( uint64_t ) ex_protect_pool_ex + 0x95 );
		*ppool_big_table = ( uint64_t ) resolve_relative_address( pool_big_table_ptr , 3 , 7 );

		void* pool_big_table_size_ptr = ( void* ) ( ( uint64_t ) ex_protect_pool_ex + 0x8E );
		*ppool_big_table_size = ( uint64_t ) resolve_relative_address( pool_big_table_size_ptr , 3 , 7 );

		return true;
	}

	auto c_pool_cleaner::clean_big_pools( uint64_t pool ) -> NTSTATUS {
		uint64_t ppool_big_table = 0;
		uint64_t ppool_big_table_size = 0;

		if ( find_pool_table( &ppool_big_table , &ppool_big_table_size ) ) {
			PPOOL_TRACKER_BIG_PAGES pool_big_table = 0;
			utilities::crt::memcpy( &pool_big_table , ( void* ) ppool_big_table , 8 );

			SIZE_T pool_big_table_size = 0;
			utilities::crt::memcpy( &pool_big_table_size , ( void* ) ppool_big_table_size , 8 );

			for ( int i = 0; i < pool_big_table_size; i++ ) {
				if ( pool_big_table [ i ].Va == pool || pool_big_table [ i ].Va == ( pool + 0x1 ) ) {
					pool_big_table [ i ].Va = 0x1;
					pool_big_table [ i ].NumberOfBytes = 0x0;

					return STATUS_SUCCESS;
				}
			}

			return STATUS_UNSUCCESSFUL;
		}

		return STATUS_UNSUCCESSFUL;
	}
}
