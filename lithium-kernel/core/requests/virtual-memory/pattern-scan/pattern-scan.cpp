#include "pattern-scan.hpp"

namespace physical::memory
{
	auto c_pattern_scanner::pattern_scan( invoke_data* request ) -> NTSTATUS
	{
		pattern_invoke data = { 0 };
		PEPROCESS process = 0;

		utilities::crt::memcpy( &data , request->data , sizeof( pattern_invoke ) );

		if ( !NT_SUCCESS( l_import( PsLookupProcessByProcessId )( reinterpret_cast< HANDLE >( data.pid ) , &process ) ) ) {
			return STATUS_UNSUCCESSFUL;
		}

		KAPC_STATE state;
		l_import( KeStackAttachProcess )( process , &state );

		auto address = scanner::find_pattern( data.address , data.signature );

		l_import( KeUnstackDetachProcess )( &state );

		reinterpret_cast< pattern_invoke* >( request->data )->pattern_address = address;

		l_import( ObfDereferenceObject )( process );
		return STATUS_SUCCESS;
	}
}
