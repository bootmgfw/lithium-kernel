#include "allocate.hpp"

namespace allocation
{
	auto c_allocator::allocate_virtual_memory( invoke_data* request ) -> NTSTATUS
	{
		allocate_invoke data = { 0 };

		utilities::crt::memcpy( &data , request->data , sizeof( allocate_invoke ) );

		PEPROCESS process;
		if ( l_import( PsLookupProcessByProcessId )( ( HANDLE ) data.pid , &process ) != STATUS_SUCCESS )
		{
			return STATUS_UNSUCCESSFUL;
		}

		KAPC_STATE state;
		l_import( KeStackAttachProcess )( process , &state );

		auto status = l_import( ZwAllocateVirtualMemory )( reinterpret_cast< HANDLE >( -1 ) , reinterpret_cast< void** >( &data.address ) , 0 , &data.size , data.type , data.protection );

		l_import( KeUnstackDetachProcess )( &state );

		reinterpret_cast< allocate_invoke* >( request->data )->address = data.address;

		l_import( ObfDereferenceObject )( process );

		return STATUS_SUCCESS;
	}

}
