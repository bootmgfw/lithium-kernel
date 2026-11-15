#include <dependencies/includes.hpp>
#include <core/driver/driver.hpp>


auto main( ) -> std::int32_t
{

	SetConsoleTitleA( "usermode" );

	if ( !g_driver->initialize_handle( ) )
	{
		l_log( "Failed to create the handle." );
	}

	auto pid = g_driver->get_process_id( L"notepad.exe" );

	l_log( "Process ID: %d." , pid );

	if ( !g_driver->attach( pid ) ) { }

	g_driver->image_base = g_driver->get_image_base( 0 );

	std::cin.get( );

	l_log( "image-base: 0x%llx." , g_driver->image_base );

	g_driver->dtb = g_driver->resolve_directory_table_base( );

	l_log( "directory-table-base: 0x%llx." , g_driver->dtb );

	auto test_read = g_driver->read<uintptr_t>( g_driver->image_base );

	printf( "test_read: 0x%llx\n" , test_read );

	std::cin.get( );
	return 1;
}
