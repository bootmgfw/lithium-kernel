#pragma once

#include <dependencies/includes.hpp>

#define device_name "\\\\.\\ProcObsrvesx"

namespace driver
{
	typedef enum _requests {
		invoke_unique ,
		invoke_base ,
		invoke_read ,
		invoke_write ,
		invoke_dtb ,
		invoke_allocate ,
		invoke_protect ,
		invoke_write_virtual ,
		invoke_read_virtual ,
		invoke_pattern_scan ,
		invoke_virtual_query ,
		invoke_suspend_threads ,
		invoke_resume_threads ,
		invoke_move_mouse
	} requests , * prequests;

	typedef struct _base_invoke {
		uint32_t pid;
		uintptr_t handle;
	} base_invoke , * pbase_invoke;

	typedef struct _read_invoke {
		uint32_t pid;
		uint64_t directory_table;
		uint64_t address;
		void* buffer;
		size_t size;
	} read_invoke , * pread_invoke;

	typedef struct _dtb_invoke {
		uint32_t pid;
		uintptr_t dtb;
	} dtb_invoke , * pdtb_invoke;

	typedef struct _pml4_invoke {
		uint32_t pid;
	} pml4_invoke , * ppml4_invoke;

	typedef struct _physical_ranges_invoke {
		PHYSICAL_MEMORY_RANGE* ranges;
	} physical_ranges_invoke , * pphysical_ranges_invoke;


	typedef struct _mouse_invoke {
		int x;
		int y;
		short flags;
		short button_flags;
	} mouse_invoke , * pmouse_invoke;

	typedef struct _invoke_data {
		uint32_t unique;
		requests code;
		void* data;
	} invoke_data , * pinvoke_data;

	class c_driver_communicator
	{
		std::int32_t m_pid = 0;
		void* m_handle = 0;

		std::unordered_map<std::uintptr_t , ::pml4e> m_cache_pml4e { };
		std::unordered_map<std::uintptr_t , ::pdpte> m_cache_pdpte { };
		std::unordered_map<std::uintptr_t , ::pde> m_cache_pde { };
		std::unordered_map<std::uintptr_t , ::pte> m_cache_pte { };

		pt_entries m_entries { };

		std::unordered_map<std::uintptr_t , ::pml4e>  m_pml4e_cache { };
		std::unordered_map<std::uintptr_t , ::pdpte>  m_pdpte_cache { };
		std::unordered_map<std::uintptr_t , ::pde>    m_pd_cache { };
		std::unordered_map<std::uintptr_t , ::pte>    m_pte_cache { };



	public:

		std::uintptr_t image_base = 0;
		std::uintptr_t dtb = 0;

		ALWAYS_INLINE c_driver_communicator( ) = default;
		ALWAYS_INLINE ~c_driver_communicator( ) = default;

		auto send_cmd( void* , requests ) -> bool;
		auto initialize_handle( ) -> bool;
		auto attach( int ) -> bool;
		auto get_process_id( const LPCWSTR ) -> uint32_t;
		auto get_image_base( const char* ) -> std::uintptr_t;
		auto resolve_directory_table_base( ) -> std::uintptr_t;
		auto mouse_event( long , long , unsigned short , unsigned short ) -> void;
		auto get_physical_ranges( ) -> const PHYSICAL_MEMORY_RANGE*;

		auto read_physical( uintptr_t address , void* buffer , size_t size ) -> bool;
		template <typename t>
		auto read( const uintptr_t address ) -> t
		{
			t response {};
			read_physical( address , &response , sizeof( t ) );
			return response;
		}

		template <typename t>
		auto read_kernel_physical( std::uintptr_t addr ) -> t {
			t response { };

			read_physical(
				addr ,
				&response ,
				sizeof( t )
			);

			return response;
		}

	};

} inline const auto g_driver = std::make_unique<driver::c_driver_communicator>( );
