#include "crt.hpp"

#define to_lower( chr ) ( ( chr >= ( 'A' ) && chr <= ( 'Z' ) ) ? ( chr + ( 32 ) ) : chr )

namespace utilities
{
	auto c_crt::rand( ) -> uint32_t
	{
		static uint32_t state [ 4 ] = {
			KeQueryInterruptTime( ) & 0xFFFFFFFF,
			( KeQueryInterruptTime( ) >> 32 ) & 0xFFFFFFFF,
			__readcr3( ) & 0xFFFFFFFF,
			( uint32_t ) PsGetCurrentThreadId( )
		};

		uint32_t t = state [ 3 ];
		t ^= t << 11;
		t ^= t >> 8;
		state [ 3 ] = state [ 2 ];
		state [ 2 ] = state [ 1 ];
		state [ 1 ] = state [ 0 ];
		t ^= state [ 0 ];
		t ^= state [ 0 ] >> 19;
		state [ 0 ] = t;

		if ( ( t & 0xFF ) == 0 ) {
			LARGE_INTEGER perf_count;
			KeQueryPerformanceCounter( &perf_count );
			state [ 1 ] ^= perf_count.LowPart;
			state [ 2 ] ^= perf_count.HighPart;
		}

		return t;
	}

	auto c_crt::memcpy( void* dst , const void* src , uint64_t count ) -> void* {
		( __movsb ) ( PBYTE( dst ) , PBYTE( src ) , count );
		return dst;
	}

	auto c_crt::memset( void* src , int val , uint64_t count ) -> void* {
		__stosb( ( unsigned char* ) ( ( unsigned long long )( volatile char* )src ) , val , count );
		return src;
	}

	auto c_crt::memcmp( const void* s1 , const void* s2 , uint64_t n ) -> int {
		if ( n != 0 ) {
			const unsigned char* p1 = ( unsigned char* ) s1 , * p2 = ( unsigned char* ) s2;
			do {
				if ( *p1++ != *p2++ ) return ( *--p1 - *--p2 );
			} while ( --n != 0 );
		}

		return 0;
	}

	auto c_crt::chrlwr( char c ) -> char {
		if ( c >= ( 'A' ) && c <= ( 'Z' ) ) return c - ( 'A' ) + ( 'a' );
		return c;
	}

	auto c_crt::strlen( const char* str ) -> int {
		int chr = ( 0 );
		if ( str ) {
			for ( ; *str != ( 0 ); ++str ) ++chr;
		}
		return chr;
	}

	auto c_crt::strcmp( const char* cs , const char* ct ) -> int {
		if ( cs && ct ) {
			while ( *cs == *ct ) {
				if ( *cs == ( 0 ) && *ct == ( 0 ) ) return ( 0 );
				if ( *cs == ( 0 ) || *ct == ( 0 ) ) break;
				cs++;
				ct++;
			}

			return *cs - *ct;
		}

		return ( -1 );
	}

	auto c_crt::stricmp( const char* cs , const char* ct ) -> int {
		if ( cs && ct ) {
			while ( chrlwr( *cs ) == chrlwr( *ct ) ) {
				if ( *cs == ( 0 ) && *ct == ( 0 ) ) return ( 0 );
				if ( *cs == ( 0 ) || *ct == ( 0 ) ) break;
				cs++;
				ct++;
			}
			return chrlwr( *cs ) - chrlwr( *ct );
		}
		return -1;
	}

	auto c_crt::strcpy( char* dst , char* src ) -> char* {
		char* ret = dst;
		while ( *src ) *dst++ = *src++;
		*dst = ( 0 );
		return ret;
	}

	auto c_crt::strstr( char const* str , char const* sub_str ) -> const char* {
		const char* bp = sub_str;
		const char* back_pos;

		while ( *str != ( 0 ) && str != 0 && sub_str != 0 ) {
			back_pos = str;
			while ( chrlwr( *back_pos++ ) == chrlwr( *sub_str++ ) ) {
				if ( *sub_str == ( 0 ) ) {
					return ( char* ) ( back_pos - strlen( bp ) );
				}
			}
			++str;
			sub_str = bp;
		}

		return 0;
	}

	auto c_crt::strcat( char* dest , const char* src ) -> char* {
		if ( ( dest == 0 ) || ( src == 0 ) )
			return dest;

		while ( *dest != ( 0 ) )
			dest++;

		while ( *src != ( 0 ) ) {
			*dest = *src;
			dest++;
			src++;
		}

		*dest = ( 0 );
		return dest;
	}

	auto c_crt::wcslen( const wchar_t* s ) -> int {
		int cnt = ( 0 );

		if ( !s )
			return ( 0 );
		for ( ; *s != ( 0 ); ++s )
			++cnt;

		return cnt;
	}

	auto c_crt::wcscmp( const wchar_t* cs , const wchar_t* ct , bool two ) -> int {
		if ( !cs || !ct )
			return false;

		wchar_t c1 , c2;
		do {
			c1 = *cs++; c2 = *ct++;
			c1 = to_lower( c1 ); c2 = to_lower( c2 );

			if ( !c1 && ( two ? !c2 : ( 1 ) ) )
				return ( true );

		} while ( c1 == c2 );

		return ( false );
	}
}
