#include "nmi-callbacks.hpp"

extern "C" NTSYSAPI BOOLEAN NTAPI KeInterlockedSetProcessorAffinityEx( PKAFFINITY_EX pAffinity , KEPROCESSORINDEX idxProcessor );

template <typename T = uint8_t*>
__forceinline T resolve_mov( uint8_t* address )
{
	return ( T ) ( address + *reinterpret_cast< int32_t* >( address + 3 ) + 7 );
}

namespace nmi
{
	auto c_nmi::disable_callbacks( ) -> bool {
		const auto ntoskrnl_base = ( PVOID ) utilities::kernel::get_module( l_enc( "ntoskrnl.exe" ) );

		if ( !ntoskrnl_base ) {
			return 0;
		}

		auto nmi_in_progress = reinterpret_cast< uint8_t* >( utilities::kernel::pattern::find_pattern( ( uintptr_t ) ntoskrnl_base , l_enc( "\x81\x25\x00\x00\x00\x00\x00\x00\x00\x00\xB9\x00\x00\x00\x00" ) , l_enc( "xx????????x????" ) ) );

		if ( !nmi_in_progress ) {
			return 0;
		}

		if ( nmi_in_progress ) {

			while ( *nmi_in_progress != 0x48 ) {
				++nmi_in_progress;
			}

			nmi_in_progress = resolve_mov( nmi_in_progress );

			auto irql = l_import( KfRaiseIrql )( 0 );

			ULONG cores = l_import( KeQueryActiveProcessorCount )( NULL );

			for ( auto i = 0ul; i < cores; ++i ) {

				l_import( KeInterlockedSetProcessorAffinityEx )( ( PKAFFINITY_EX ) nmi_in_progress , i );
				InterlockedBitTestAndSet64( ( LONG64* ) ( nmi_in_progress ) , i );

			}

			l_import( KeLowerIrql )( irql );
		}

		return true;

	}
}
