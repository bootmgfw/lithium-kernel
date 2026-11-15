#include "virtual-query.hpp"

namespace virtual_query
{
	auto c_virtual_query::virtual_query( invoke_data* request ) -> NTSTATUS
	{
		query_virtual_invoke data = { 0 };

		utilities::crt::memcpy( &data , request->data , sizeof( query_virtual_invoke ) );

		PEPROCESS process;
		if ( l_import( PsLookupProcessByProcessId )( ( HANDLE ) data.pid , &process ) != STATUS_SUCCESS )
		{
			return STATUS_UNSUCCESSFUL;
		}

		KAPC_STATE state;
		l_import( KeStackAttachProcess )( process , &state );

		MEMORY_BASIC_INFORMATION mbi = { 0 };
		auto status = l_import( ZwQueryVirtualMemory )( NtCurrentProcess( ) , ( void* ) data.address ,
			MemoryBasicInformation , &mbi , sizeof( mbi ) , 0 );

		l_import( KeUnstackDetachProcess )( &state );

		reinterpret_cast< query_virtual_invoke* >( request->data )->mbi = mbi;
		l_import( ObfDereferenceObject )( process );

		return STATUS_SUCCESS;

	}
}
