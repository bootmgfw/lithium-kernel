#pragma once

#include <core/requests/image-base/image-base.hpp>
#include <core/requests/read-write/read-memory/read-memory.hpp>


namespace request
{
	inline auto get_image_base( invoke_data* data ) -> NTSTATUS { return image_base::c_image_base::get_image_base( data ); }
	inline auto resolve_dtb( invoke_data* data ) -> NTSTATUS { return directory_table_base::c_directory_table::resolve_dtb( data ); }
	inline auto read_memory( invoke_data* data ) -> NTSTATUS { return physical::memory::c_memory_reader::read_memory( data ); }
	inline auto write_memory( invoke_data* data ) -> NTSTATUS { return physical::memory::c_memory_writer::write_memory( data ); }
	inline auto write_virtual_memory( invoke_data* data ) -> NTSTATUS { return physical::memory::c_virtual_writer::write_virtual_memory( data ); }
	inline auto read_virtual_memory( invoke_data* data ) -> NTSTATUS { return physical::memory::c_virtual_reader::read_virtual_memory( data ); }
	inline auto virtual_query( invoke_data* data ) -> NTSTATUS { return virtual_query::c_virtual_query::virtual_query( data ); }
	inline auto allocate_virtual_memory( invoke_data* data ) -> NTSTATUS { return allocation::c_allocator::allocate_virtual_memory( data ); }
	inline auto protect_virtual_memory( invoke_data* data ) -> NTSTATUS { return protection::c_memory_protector::protect_virtual_memory( data ); }
	inline auto pattern_scan( invoke_data* data ) -> NTSTATUS { return physical::memory::c_pattern_scanner::pattern_scan( data ); }
}
