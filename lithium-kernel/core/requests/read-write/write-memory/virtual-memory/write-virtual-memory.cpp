#include "write-virtual-memory.hpp"

namespace physical::memory
{
	auto c_virtual_writer::write_virtual_memory( invoke_data* request ) -> NTSTATUS
	{
		write_virtual_invoke data = { 0 };
		PEPROCESS process = 0;

		NTSTATUS status = STATUS_UNSUCCESSFUL;

		utilities::crt::memcpy( &data , request->data , sizeof( write_virtual_invoke ) );

		if ( !NT_SUCCESS( l_import( PsLookupProcessByProcessId )( reinterpret_cast< HANDLE >( data.pid ) , &process ) ) ) {
			return STATUS_UNSUCCESSFUL;
		}

		size_t bytes = 0;
		status = MmCopyVirtualMemory( PsGetCurrentProcess( ) , data.buffer , process , ( PVOID ) data.address , data.size , UserMode , &bytes );

		l_import( ObfDereferenceObject )( process );

		return status;
	}
}
