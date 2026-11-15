#include "mouse.hpp"

namespace mouse
{
	auto c_mouse::open( ) -> bool {
		_KeAcquireSpinLockAtDpcLevel = reinterpret_cast< UINT64 >( l_import( KeAcquireSpinLockAtDpcLevel ) );
		_KeReleaseSpinLockFromDpcLevel = reinterpret_cast< UINT64 >( l_import( KeReleaseSpinLockFromDpcLevel ) );
		_IofCompleteRequest = reinterpret_cast< UINT64 >( l_import( IofCompleteRequest ) );
		_IoReleaseRemoveLockEx = reinterpret_cast< UINT64 >( l_import( IoReleaseRemoveLockEx ) );

		UNICODE_STRING class_string;
		l_import( RtlInitUnicodeString )( &class_string , L"\\Driver\\MouClass" );

		PDRIVER_OBJECT class_driver_object = NULL;
		NTSTATUS status = ObReferenceObjectByName( &class_string , OBJ_CASE_INSENSITIVE , NULL , 0 , *IoDriverObjectType , KernelMode , NULL , ( PVOID* ) &class_driver_object );
		if ( !NT_SUCCESS( status ) )
		{
			return false;
		}

		UNICODE_STRING hid_string;
		l_import( RtlInitUnicodeString )( &hid_string , L"\\Driver\\MouHID" );

		PDRIVER_OBJECT hid_driver_object = NULL;

		status = l_import( ObReferenceObjectByName )( &hid_string , OBJ_CASE_INSENSITIVE , NULL , 0 , *IoDriverObjectType , KernelMode , NULL , ( PVOID* ) &hid_driver_object );
		if ( !NT_SUCCESS( status ) )
		{
			if ( class_driver_object ) l_import( ObfDereferenceObject )( class_driver_object );
			return false;
		}

		PDEVICE_OBJECT hid_device_object = hid_driver_object->DeviceObject;

		while ( hid_device_object && !gMouseObject.service_callback )
		{
			PDEVICE_OBJECT class_device_object = class_driver_object->DeviceObject;

			while ( class_device_object && !gMouseObject.service_callback )
			{
				if ( !class_device_object->NextDevice && !gMouseObject.mouse_device )
					gMouseObject.mouse_device = class_device_object;

				PULONG_PTR device_extension = reinterpret_cast< PULONG_PTR >( hid_device_object->DeviceExtension );
				ULONG_PTR device_ext_size = ( ( reinterpret_cast< ULONG_PTR >( hid_device_object->DeviceObjectExtension ) - reinterpret_cast< ULONG_PTR >( hid_device_object->DeviceExtension ) ) / 4 );

				for ( ULONG_PTR i = 0; i < device_ext_size; i++ )
				{
					if ( device_extension [ i ] == reinterpret_cast< ULONG_PTR >( class_device_object ) && device_extension [ i + 1 ] > reinterpret_cast< ULONG_PTR >( class_driver_object ) )
					{
						gMouseObject.service_callback = reinterpret_cast< MouseClassServiceCallbackFn >( device_extension [ i + 1 ] );
						break;
					}
				}
				class_device_object = class_device_object->NextDevice;
			}
			hid_device_object = hid_device_object->AttachedDevice;
		}

		if ( !gMouseObject.mouse_device )
		{
			PDEVICE_OBJECT target_device_object = class_driver_object->DeviceObject;
			while ( target_device_object )
			{
				if ( !target_device_object->NextDevice )
				{
					gMouseObject.mouse_device = target_device_object;
					break;
				}
				target_device_object = target_device_object->NextDevice;
			}
		}

		l_import( ObfDereferenceObject )( class_driver_object );
		l_import( ObfDereferenceObject )( hid_driver_object );

		return gMouseObject.mouse_device && gMouseObject.service_callback;
	}

	auto c_mouse::set( int x , int y , unsigned short flags , unsigned short button_flags ) -> void
	{
		char rdi_buffer [ 0x500 ];
		char rbp_buffer [ 0x100 ];

		for ( QWORD i = 0; i < 0x500; i++ )
			rdi_buffer [ i ] = 0;

		for ( QWORD i = 0; i < 0x100; i++ )
			rbp_buffer [ i ] = 0;

		MOUSE_INPUT_DATA* mid = ( MOUSE_INPUT_DATA* ) &rdi_buffer [ 0x160 ];
		*( QWORD* ) &rdi_buffer [ 0x178 ] = ( QWORD ) ( PMOUSE_INPUT_DATA ) mid + 1;

		mid->LastX = x;
		mid->LastY = y;
		mid->ButtonFlags = button_flags;
		mid->Flags = flags;
		mid->UnitId = 1;

		*( QWORD* ) &rdi_buffer [ 0xE0 ] = ( QWORD ) gMouseObject.mouse_device;
		*( QWORD* ) &rdi_buffer [ 0xE8 ] = ( QWORD ) gMouseObject.service_callback;
		MouseClassServiceCallbackTrick( ( QWORD ) rdi_buffer , ( QWORD ) rbp_buffer , ( QWORD ) g_target_routine );
	}
}
