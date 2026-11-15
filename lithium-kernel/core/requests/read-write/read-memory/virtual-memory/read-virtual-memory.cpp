#include "read-virtual-memory.hpp"

namespace physical::memory
{
	auto c_virtual_reader::read_virtual_memory( invoke_data* request ) -> NTSTATUS
	{
		read_virtual_invoke data = { 0 };
		PEPROCESS process = 0;

		NTSTATUS status = STATUS_UNSUCCESSFUL;

		utilities::crt::memcpy( &data , request->data , sizeof( read_virtual_invoke ) );

		if ( !NT_SUCCESS( l_import( PsLookupProcessByProcessId )( reinterpret_cast< HANDLE >( data.pid ) , &process ) ) ) {
			return STATUS_UNSUCCESSFUL;
		}


		size_t bytes = 0;
		status = MmCopyVirtualMemory( process , ( PVOID ) data.address , PsGetCurrentProcess( ) , data.buffer , data.size , UserMode , &bytes );

		l_import( ObfDereferenceObject )( process );

		return status;

	}
}
