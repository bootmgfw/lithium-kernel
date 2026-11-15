#pragma once

#include <dependencies/includes.hpp>

#define in_range(x,a,b)    (x >= a && x <= b)
#define get_bits( x )    (in_range((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xA) : (in_range(x,'0','9') ? x - '0' : 0))
#define get_byte( x )    (get_bits(x[0]) << 4 | get_bits(x[1]))
#define rva(addr, size)       ((uintptr_t)((uintptr_t)(addr) + *(int*)((uintptr_t)(addr) + ((size) - sizeof(INT))) + (size)))

namespace scanner
{
	class c_pattern_scanner
	{
	public:
		ALWAYS_INLINE c_pattern_scanner( ) = default;
		ALWAYS_INLINE ~c_pattern_scanner( ) = default;

		static auto find_pattern( uintptr_t , size_t , const char* , const char* ) -> uintptr_t;
		static auto find_pattern( uintptr_t , const char* , const char* ) -> uintptr_t;
		static auto find_pattern( uintptr_t , const char* ) -> uintptr_t;
	};

	inline auto find_pattern( uintptr_t a , size_t b , const char* c , const char* d ) -> uintptr_t { return c_pattern_scanner::find_pattern( a , b , c , d ); }
	inline auto find_pattern( uintptr_t a , const char* b , const char* c ) -> uintptr_t { return c_pattern_scanner::find_pattern( a , b , c ); }
	inline auto find_pattern( uintptr_t a , const char* b ) -> uintptr_t { return c_pattern_scanner::find_pattern( a , b ); }

}
