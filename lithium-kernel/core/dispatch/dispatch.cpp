#include "dispatch.hpp"
#include <core/requests/requests.hpp>

namespace dispatch
{
	auto c_dispatch::hooked_ioctl( PDEVICE_OBJECT obj , IRP* irp ) -> NTSTATUS
	{
		if ( !irp || !irp->AssociatedIrp.SystemBuffer )
			return STATUS_INVALID_PARAMETER;

		PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation( irp );
		if ( stack->Parameters.DeviceIoControl.InputBufferLength < sizeof( invoke_data ) )
			return STATUS_INVALID_PARAMETER;

		auto buffer = reinterpret_cast< invoke_data* >( irp->AssociatedIrp.SystemBuffer );
		irp->IoStatus.Status = STATUS_SUCCESS;

		switch ( buffer->code )
		{
		case invoke_base:
			if ( request::get_image_base( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_dtb:
			if ( request::resolve_dtb( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_read:
			if ( request::read_memory( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_write:
			if ( request::write_memory( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_write_virtual:
			if ( request::write_virtual_memory( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_read_virtual:
			if ( request::read_virtual_memory( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_virtual_query:
			if ( request::virtual_query( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_allocate:
			if ( request::allocate_virtual_memory( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_protect:
			if ( request::protect_virtual_memory( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_pattern_scan:
			if ( request::pattern_scan( buffer ) != STATUS_SUCCESS )
				irp->IoStatus.Status = STATUS_UNSUCCESSFUL;
			break;
		case invoke_move_mouse:
		{
			mouse_invoke data = { 0 };

			utilities::crt::memcpy( &data , buffer->data , sizeof( mouse_invoke ) );

			mouse::c_mouse::set( data.x , data.y , data.flags , data.button_flags );

			break;
		}

		default:
			irp->IoStatus.Status = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}

		IoCompleteRequest( irp , IO_NO_INCREMENT );

		return irp->IoStatus.Status;
	}
}
