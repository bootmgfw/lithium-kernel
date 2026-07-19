#pragma once

#include <dependencies/includes.hpp>

typedef struct _KNMI_HANDLER_CALLBACK
{
	struct _KNMI_HANDLER_CALLBACK* Next;
	void( *Callback )( );
	void* Context;
	void* Handle;
} KNMI_HANDLER_CALLBACK , * PKNMI_HANDLER_CALLBACK;

typedef ULONG KEPROCESSORINDEX;

inline PKNMI_HANDLER_CALLBACK KiNmiCallbackListHead = nullptr;

namespace nmi
{
	class c_nmi
	{
	public:
		ALWAYS_INLINE c_nmi( ) = default;
		ALWAYS_INLINE ~c_nmi( ) = default;

		static auto disable_callbacks( ) -> bool;
	};

}

inline auto disable_nmi_callbacks( ) -> bool { return nmi::c_nmi::disable_callbacks( ); }
