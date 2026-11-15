#include "image-base.hpp"

namespace image_base
{
	auto c_image_base::get_image_base( invoke_data* request ) -> NTSTATUS
	{
		l_log( "entered image-base function." );

		base_invoke data = { 0 };

		utilities::crt::memcpy( &data , request->data , sizeof( base_invoke ) );

		if ( !data.pid ) { l_log( "failed to get pid." ); return STATUS_UNSUCCESSFUL; }

		PEPROCESS process = 0;
		if ( l_import( PsLookupProcessByProcessId )( ( HANDLE ) data.pid , &process ) != STATUS_SUCCESS ) { return STATUS_UNSUCCESSFUL; }

		uintptr_t base = ( uintptr_t ) l_import( PsGetProcessSectionBaseAddress )( process );
		l_log( "image-base: 0x%llx." , base );

		reinterpret_cast< base_invoke* >( request->data )->handle = base;

		return STATUS_SUCCESS;
	}
}
