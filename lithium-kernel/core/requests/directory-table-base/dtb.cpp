#include "dtb.hpp"

namespace directory_table_base
{
	auto c_directory_table::dtb_cache( ) -> bool {
		if ( !utilities::kernel::offsets::initialize( ) )
			return false;

		if ( !utilities::kernel::thread::hide( ) )
			return false;

		while ( 1 ) {
			PEPROCESS temp_process;
			if ( NT_SUCCESS( l_import( PsLookupProcessByProcessId )( HANDLE( globals::pid ) , &temp_process ) ) != STATUS_SUCCESS ) {
			}

			uint64_t directory_table = directory_table_base::pml4::dirbase_from_base_address(
				( void* ) l_import( PsGetProcessSectionBaseAddress )( temp_process )
			);

			globals::directory_table_base = directory_table;

			LARGE_INTEGER delayTime;
			delayTime.QuadPart = -1000000;
			l_import( KeDelayExecutionThread )( KernelMode , FALSE , &delayTime );
		}

		return true;
	}

	auto c_directory_table::get_physical_ranges( invoke_data* request ) -> NTSTATUS
	{
		physical_ranges_invoke data = { 0 };

		utilities::crt::memcpy( &data , request->data , sizeof( physical_ranges_invoke ) );

		auto ranges = l_import( MmGetPhysicalMemoryRanges )( );

		reinterpret_cast< physical_ranges_invoke* >( request->data )->ranges = ranges;

		return STATUS_SUCCESS;
	}


	auto c_directory_table::resolve_dtb( invoke_data* request ) -> NTSTATUS
	{
		dtb_invoke data = { 0 };

		utilities::crt::memcpy( &data , request->data , sizeof( dtb_invoke ) );

		PEPROCESS process = 0;
		if ( l_import( PsLookupProcessByProcessId )( ( HANDLE ) data.pid , &process ) != STATUS_SUCCESS )
		{
			return STATUS_UNSUCCESSFUL;
		}

		globals::pid = data.pid;
		globals::process = process;

		l_log( "entered dtb function." );

		auto process_dtb = pml4::dirbase_from_base_address( ( void* ) l_import( PsGetProcessSectionBaseAddress )( process ) );

		l_log( "dtb -> 0x%llx." , process_dtb );

		reinterpret_cast< dtb_invoke* >( request->data )->dtb = process_dtb;

		globals::directory_table_base = process_dtb;
		l_import( ObfDereferenceObject )( process );

		return STATUS_SUCCESS;
	}

	namespace pml4
	{
		auto split_memory( PVOID SearchBase , SIZE_T SearchSize , const void* Pattern , SIZE_T PatternSize ) -> PVOID
		{
			const UCHAR* searchBase = static_cast< const UCHAR* >( SearchBase );
			const UCHAR* pattern = static_cast< const UCHAR* >( Pattern );

			for ( SIZE_T i = 0; i <= SearchSize - PatternSize; ++i ) {
				SIZE_T j = 0;
				for ( ; j < PatternSize; ++j ) {
					if ( searchBase [ i + j ] != pattern [ j ] )
						break;
				}

				if ( j == PatternSize )
					return const_cast< UCHAR* >( &searchBase [ i ] );
			}

			return nullptr;
		}

		static auto InitializeMmPfnDatabase( ) -> NTSTATUS
		{
			struct MmPfnDatabaseSearchPattern
			{
				const UCHAR* bytes;
				SIZE_T bytes_size;
				bool hard_coded;
			};

			MmPfnDatabaseSearchPattern patterns;

			static const UCHAR kPatternWin10x64 [ ] = {
				0x48, 0x8B, 0xC1,
				0x48, 0xC1, 0xE8, 0x0C,
				0x48, 0x8D, 0x14, 0x40,
				0x48, 0x03, 0xD2,
				0x48, 0xB8,
			};

			patterns.bytes = kPatternWin10x64;
			patterns.bytes_size = sizeof( kPatternWin10x64 );
			patterns.hard_coded = true;

			const auto p_MmGetVirtualForPhysical = reinterpret_cast< UCHAR* >( l_import( MmGetVirtualForPhysical ) );
			if ( !p_MmGetVirtualForPhysical ) {
				return STATUS_PROCEDURE_NOT_FOUND;
			}

			auto found = reinterpret_cast< UCHAR* >( split_memory( p_MmGetVirtualForPhysical , 0x20 , patterns.bytes , patterns.bytes_size ) );
			if ( !found ) {
				return STATUS_UNSUCCESSFUL;
			}


			found += patterns.bytes_size;
			if ( patterns.hard_coded ) {
				g_mmonp_MmPfnDatabase = *reinterpret_cast< void** >( found );
			}
			else {
				const auto mmpfn_address = *reinterpret_cast< ULONG_PTR* >( found );
				g_mmonp_MmPfnDatabase = *reinterpret_cast< void** >( mmpfn_address );
			}

			g_mmonp_MmPfnDatabase = PAGE_ALIGN( g_mmonp_MmPfnDatabase );

			return STATUS_SUCCESS;
		}


		auto translate_linear(
			uintptr_t directory_base ,
			uintptr_t address ) -> uintptr_t {

			directory_base &= ~0xf;

			auto virt_addr = address & ~( ~0ul << 12 );
			auto pte = ( ( address >> 12 ) & ( 0x1ffll ) );
			auto pt = ( ( address >> 21 ) & ( 0x1ffll ) );
			auto pd = ( ( address >> 30 ) & ( 0x1ffll ) );
			auto pdp = ( ( address >> 39 ) & ( 0x1ffll ) );
			auto p_mask = ( ( ~0xfull << 8 ) & 0xfffffffffull );

			size_t readsize = 0;
			uintptr_t pdpe = 0;
			physical::c_physical_memory::read_physical_address( PVOID( directory_base + 8 * pdp ) , &pdpe , sizeof( pdpe ) , &readsize );
			if ( ~pdpe & 1 ) {
				return 0;
			}

			uintptr_t pde = 0;
			physical::c_physical_memory::read_physical_address( PVOID( ( pdpe & p_mask ) + 8 * pd ) , &pde , sizeof( pde ) , &readsize );
			if ( ~pde & 1 ) {
				return 0;
			}

			if ( pde & 0x80 )
				return ( pde & ( ~0ull << 42 >> 12 ) ) + ( address & ~( ~0ull << 30 ) );

			uintptr_t pteAddr = 0;
			physical::c_physical_memory::read_physical_address( PVOID( ( pde & p_mask ) + 8 * pt ) , &pteAddr , sizeof( pteAddr ) , &readsize );
			if ( ~pteAddr & 1 ) {
				return 0;
			}

			if ( pteAddr & 0x80 ) {
				return ( pteAddr & p_mask ) + ( address & ~( ~0ull << 21 ) );
			}

			address = 0;
			physical::c_physical_memory::read_physical_address( PVOID( ( pteAddr & p_mask ) + 8 * pte ) , &address , sizeof( address ) , &readsize );
			address &= p_mask;

			if ( !address ) {
				return 0;
			}

			return address + virt_addr;
		}

		auto dirbase_from_base_address( void* base ) -> uintptr_t
		{
			if ( !NT_SUCCESS( InitializeMmPfnDatabase( ) ) )
				return 0;

			virt_addr_t virt_base {}; virt_base.value = base;

			size_t read {};

			auto ranges = l_import( MmGetPhysicalMemoryRanges )( );

			for ( int i = 0;; i++ ) {

				auto elem = &ranges [ i ];

				if ( !elem->BaseAddress.QuadPart || !elem->NumberOfBytes.QuadPart )
					break;

				uintptr_t current_phys_address = elem->BaseAddress.QuadPart;

				for ( int j = 0; j < ( elem->NumberOfBytes.QuadPart / 0x1000 ); j++ , current_phys_address += 0x1000 ) {

					_MMPFN* pnfinfo = ( _MMPFN* ) ( ( uintptr_t ) g_mmonp_MmPfnDatabase + ( current_phys_address >> 12 ) * sizeof( _MMPFN ) );

					if ( pnfinfo->u4.PteFrame == ( current_phys_address >> 12 ) ) {
						MMPTE pml4e {};
						if ( !NT_SUCCESS( physical::c_physical_memory::read_physical_address( PVOID( current_phys_address + 8 * virt_base.pml4_index ) , &pml4e , 8 , &read ) ) )
							continue;

						if ( !pml4e.u.Hard.Valid )
							continue;

						MMPTE pdpte {};
						if ( !NT_SUCCESS( physical::c_physical_memory::read_physical_address( PVOID( ( pml4e.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pdpt_index ) , &pdpte , 8 , &read ) ) )
							continue;

						if ( !pdpte.u.Hard.Valid )
							continue;

						MMPTE pde {};
						if ( !NT_SUCCESS( physical::c_physical_memory::read_physical_address( PVOID( ( pdpte.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pd_index ) , &pde , 8 , &read ) ) )
							continue;

						if ( !pde.u.Hard.Valid )
							continue;

						MMPTE pte {};
						if ( !NT_SUCCESS( physical::c_physical_memory::read_physical_address( PVOID( ( pde.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pt_index ) , &pte , 8 , &read ) ) )
							continue;

						if ( !pte.u.Hard.Valid )
							continue;

						globals::virt_base = virt_base;

						return current_phys_address;
					}
				}
			}

			return 0;
		}

		auto virtual_to_physical( uintptr_t directory_table_base , uintptr_t virtual_address ) -> uintptr_t
		{
			virt_addr_t virt_base {};
			virt_base.value = PVOID( virtual_address );

			SIZE_T Size = 0;

			if ( cached_pml4e [ virt_base.pml4_index ].Address != directory_table_base + 8 * virt_base.pml4_index || !cached_pml4e [ virt_base.pml4_index ].Value.u.Hard.Valid ) {
				cached_pml4e [ virt_base.pml4_index ].Address = directory_table_base + 8 * virt_base.pml4_index;
				physical::c_physical_memory::read_physical_address( PVOID( cached_pml4e [ virt_base.pml4_index ].Address ) , reinterpret_cast< PVOID >( &cached_pml4e [ virt_base.pml4_index ].Value ) , 8 , &Size );
			}

			if ( !cached_pml4e [ virt_base.pml4_index ].Value.u.Hard.Valid )
			{
				return 0;
			}


			if ( cached_pdpte [ virt_base.pdpt_index ].Address != ( cached_pml4e [ virt_base.pml4_index ].Value.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pdpt_index || !cached_pdpte [ virt_base.pdpt_index ].Value.u.Hard.Valid ) {
				cached_pdpte [ virt_base.pdpt_index ].Address = ( cached_pml4e [ virt_base.pml4_index ].Value.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pdpt_index;
				physical::c_physical_memory::read_physical_address( PVOID( cached_pdpte [ virt_base.pdpt_index ].Address ) , reinterpret_cast< PVOID >( &cached_pdpte [ virt_base.pdpt_index ].Value ) , 8 , &Size );
			}

			if ( !cached_pdpte [ virt_base.pdpt_index ].Value.u.Hard.Valid )
			{
				return 0;
			}

			if ( cached_pde [ virt_base.pd_index ].Address != ( cached_pdpte [ virt_base.pdpt_index ].Value.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pd_index || !cached_pde [ virt_base.pd_index ].Value.u.Hard.Valid ) {
				cached_pde [ virt_base.pd_index ].Address = ( cached_pdpte [ virt_base.pdpt_index ].Value.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pd_index;
				physical::c_physical_memory::read_physical_address( PVOID( cached_pde [ virt_base.pd_index ].Address ) , reinterpret_cast< PVOID >( &cached_pde [ virt_base.pd_index ].Value ) , 8 , &Size );
			}

			if ( !cached_pde [ virt_base.pd_index ].Value.u.Hard.Valid )
			{
				return 0;
			}

			if ( cached_pte [ virt_base.pt_index ].Address != ( cached_pde [ virt_base.pd_index ].Value.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pt_index || !cached_pte [ virt_base.pt_index ].Value.u.Hard.Valid ) {
				cached_pte [ virt_base.pt_index ].Address = ( cached_pde [ virt_base.pd_index ].Value.u.Hard.PageFrameNumber << 12 ) + 8 * virt_base.pt_index;
				physical::c_physical_memory::read_physical_address( PVOID( cached_pte [ virt_base.pt_index ].Address ) , reinterpret_cast< PVOID >( &cached_pte [ virt_base.pt_index ].Value ) , 8 , &Size );
			}

			if ( !cached_pte [ virt_base.pt_index ].Value.u.Hard.Valid )
			{
				return 0;
			}

			return ( cached_pte [ virt_base.pt_index ].Value.u.Hard.PageFrameNumber << 12 ) + virt_base.offset;
		}
	}
}
