#pragma once

#include <dependencies/includes.hpp>

namespace dispatch
{
	using IoctlHandlerType = NTSTATUS( * )( PDEVICE_OBJECT , PIRP );

	class c_dispatch
	{
	public:
		ALWAYS_INLINE c_dispatch( ) = default;
		ALWAYS_INLINE ~c_dispatch( ) = default;

		static auto hooked_ioctl( PDEVICE_OBJECT , IRP* ) -> NTSTATUS;
	};

	inline auto hooked_ioctl( PDEVICE_OBJECT obj , IRP* irp ) -> NTSTATUS { return c_dispatch::hooked_ioctl( obj , irp ); }

}
