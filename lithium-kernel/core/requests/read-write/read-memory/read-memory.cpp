#include "read-memory.hpp"

namespace physical::memory
{
	auto c_memory_reader::read_memory( invoke_data* request ) -> NTSTATUS
	{
		read_invoke data = { 0 };
		PEPROCESS process = 0;

		NTSTATUS status = STATUS_UNSUCCESSFUL;

		utilities::crt::memcpy( &data , request->data , sizeof( read_invoke ) );

		if ( !NT_SUCCESS( l_import( PsLookupProcessByProcessId )( reinterpret_cast< HANDLE >( data.pid ) , &process ) ) ) {
			return STATUS_UNSUCCESSFUL;
		}

		auto physical_address = directory_table_base::pml4::translate_linear( data.directory_table , data.address );
		DbgPrintEx( 0 , 0 , "physical_address: 0x%llx" , physical_address );

		if ( !physical_address )
		{
			return STATUS_UNSUCCESSFUL;
			utilities::crt::memset( cached_pdpte , { 0 } , sizeof( cached_pdpte ) );
			utilities::crt::memset( cached_pde , { 0 } , sizeof( cached_pde ) );
			utilities::crt::memset( cached_pte , { 0 } , sizeof( cached_pte ) );
		}

		auto final_size = physical::c_physical_memory::find_min( PAGE_SIZE - ( physical_address & 0xFFF ) , data.size );

		DbgPrintEx( 0 , 0 , "final_size: 0x%llx" , final_size );

		size_t bytes = 0;
		if ( !NT_SUCCESS( physical::c_physical_memory::read_physical_address( ( PVOID ) physical_address , reinterpret_cast< void* >( ( reinterpret_cast< ULONG64 >( data.buffer ) ) ) , final_size , &bytes ) ) )
		{
			l_import( ObfDereferenceObject )( process );
			return STATUS_UNSUCCESSFUL;
		}

		l_import( ObfDereferenceObject )( process );

		return STATUS_SUCCESS;
	}

}
