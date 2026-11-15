#include <dependencies/includes.hpp>

#define sig "\x48\x89\x5C\x24\x00\x48\x89\x6C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x20"
#define mask "xxxx?xxxx?xxxx?xxxxx"

bool thread( )
{
	if ( !utilities::kernel::offsets::initialize( ) ) return false;

	if ( !utilities::kernel::thread::hide( ) ) return false;

	const auto driver = utilities::kernel::get_module( l_enc( "ProcObsrvesx.sys" ) );
	if ( !driver )
		return ( STATUS_FAILED_DRIVER_ENTRY );

	const auto ioctl_start = utilities::kernel::pattern::find_pattern( driver , l_enc( sig ) , l_enc( mask ) );
	if ( !ioctl_start )
		return ( STATUS_FAILED_DRIVER_ENTRY );

	const auto to_hook = reinterpret_cast< uintptr_t >( &dispatch::hooked_ioctl );

	char shellcode_final [ ] = {
		0xF3, 0x0F, 0x1E, 0xFA,
		0x90, 0x90, 0x90, 0x90,
		0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	char endbr64 [ ] = { 0xF3, 0x0F, 0x1E, 0xFA };
	char mov_rax [ ] = { 0x48, 0xB8 };
	char jmp_rax [ ] = { 0xFF, 0xE0 };
	char nops [ ] = { 0x90, 0x90, 0x90, 0x90 };

	utilities::crt::memcpy( &shellcode_final [ 0 ] , endbr64 , sizeof( endbr64 ) );
	utilities::crt::memcpy( &shellcode_final [ 4 ] , nops , sizeof( nops ) );
	utilities::crt::memcpy( &shellcode_final [ 8 ] , mov_rax , sizeof( mov_rax ) );
	utilities::crt::memcpy( &shellcode_final [ 10 ] , &to_hook , sizeof( to_hook ) );
	utilities::crt::memcpy( &shellcode_final [ 18 ] , jmp_rax , sizeof( jmp_rax ) );

	if ( utilities::kernel::write_to_rw_memory( ( void* ) ioctl_start , &shellcode_final , sizeof( shellcode_final ) ) )
	{
		l_log( "successfully patched dispatch." );
	}

	utilities::kernel::thread::terminate( );

	return true;
}

bool find_routine( )
{
	const auto driver = utilities::kernel::get_module( l_enc( "mouhid.sys" ) );

	if ( !driver ) return false;

	g_target_routine = utilities::kernel::pattern::find_pattern( driver , l_enc( "\x74\x54" ) , l_enc( "xx" ) , 2 );

	if ( g_target_routine == 0 ) return false;

	g_target_routine += 0x56;

	return true;
}

NTSTATUS entry( )
{
	globals::ntos_image_base = utilities::kernel::get_ntos_base_address( );

	disable_nmi_callbacks( );

	mouse::open( );

	if ( !find_routine( ) ) return STATUS_UNSUCCESSFUL;

	HANDLE handle = nullptr;

	OBJECT_ATTRIBUTES object_attribues {};
	InitializeObjectAttributes( &object_attribues , nullptr , OBJ_KERNEL_HANDLE , nullptr , nullptr );

	l_import( PsCreateSystemThread )( &handle , 0 , &object_attribues , nullptr , nullptr , reinterpret_cast< PKSTART_ROUTINE >( &thread ) , nullptr );

	return STATUS_SUCCESS;
}
