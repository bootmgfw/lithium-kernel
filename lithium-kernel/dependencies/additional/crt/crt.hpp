#pragma once

#include <dependencies/includes.hpp>

namespace utilities
{
	class c_crt
	{
	public:
		ALWAYS_INLINE c_crt( ) = default;
		ALWAYS_INLINE ~c_crt( ) = default;

		static auto rand( ) -> uint32_t;
		static auto memcpy( void* dst , const void* src , uint64_t count ) -> void*;
		static auto memset( void* src , int val , uint64_t count ) -> void*;
		static auto memcmp( const void* s1 , const void* s2 , uint64_t n ) -> int;

		static auto chrlwr( char c ) -> char;
		static auto strlen( const char* str ) -> int;
		static auto strcmp( const char* cs , const char* ct ) -> int;
		static auto stricmp( const char* cs , const char* ct ) -> int;
		static auto strcpy( char* dst , char* src ) -> char*;
		static auto strstr( char const* str , char const* sub_str ) -> const char*;
		static auto strcat( char* dest , const char* src ) -> char*;

		static auto wcslen( const wchar_t* s ) -> int;
		static auto wcscmp( const wchar_t* cs , const wchar_t* ct , bool two ) -> int;
	};

}

namespace utilities::crt
{
	inline auto rand( ) -> uint32_t { return c_crt::rand( ); }
	inline auto memcpy( void* dst , const void* src , uint64_t count ) -> void* { return c_crt::memcpy( dst , src , count ); }
	inline auto memset( void* src , int val , uint64_t count ) -> void* { return c_crt::memset( src , val , count ); }
	inline auto memcmp( const void* s1 , const void* s2 , uint64_t n ) -> int { return c_crt::memcmp( s1 , s2 , n ); }
	inline auto chrlwr( char c ) -> char { return c_crt::chrlwr( c ); }
	inline auto strlen( const char* str ) -> int { return c_crt::strlen( str ); }
	inline auto strcmp( const char* cs , const char* ct ) -> int { return c_crt::strcmp( cs , ct ); }
	inline auto stricmp( const char* cs , const char* ct ) -> int { return c_crt::stricmp( cs , ct ); }
	inline auto strcpy( char* dst , char* src ) -> char* { return c_crt::strcpy( dst , src ); }
	inline auto strstr( char const* str , char const* sub_str ) -> const char* { return c_crt::strstr( str , sub_str ); }
	inline auto strcat( char* dest , const char* src ) -> char* { return c_crt::strcat( dest , src ); }
	inline auto wcslen( const wchar_t* s ) -> int { return c_crt::wcslen( s ); }
	inline auto wcscmp( const wchar_t* cs , const wchar_t* ct , bool two ) -> int { return c_crt::wcscmp( cs , ct , two ); }
}
