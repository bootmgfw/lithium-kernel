#include "protect.hpp"

namespace protection
{
	auto c_memory_protector::protect_virtual_memory( invoke_data* request ) -> NTSTATUS
	{
		protect_invoke data = { 0 };
		DWORD old_protect;

		utilities::crt::memcpy( &data , request->data , sizeof( protect_invoke ) );

		PEPROCESS process;
		if ( l_import( PsLookupProcessByProcessId )( ( HANDLE ) data.pid , &process ) != STATUS_SUCCESS )
		{
			return STATUS_UNSUCCESSFUL;
		}

		void* base_address = reinterpret_cast< void* >( data.address );

		KAPC_STATE state;
		l_import( KeStackAttachProcess )( process , &state );

		auto status = l_import( ZwProtectVirtualMemory )( reinterpret_cast< HANDLE >( -1 ) , &base_address , &data.size , ULONG( data.protection ) , &old_protect );

		l_import( KeUnstackDetachProcess )( &state );

		l_import( ObfDereferenceObject )( process );

		return status;
	}
}
