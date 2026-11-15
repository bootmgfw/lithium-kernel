#pragma once

#include <dependencies/includes.hpp>

typedef ULONG_PTR QWORD;

#pragma warning(disable : 4201)
typedef struct _MOUSE_INPUT_DATA {
	USHORT UnitId;
	USHORT Flags;
	union {
		ULONG Buttons;
		struct {
			USHORT ButtonFlags;
			USHORT ButtonData;
		};
	};
	ULONG  RawButtons;
	LONG   LastX;
	LONG   LastY;
	ULONG  ExtraInformation;
} MOUSE_INPUT_DATA , * PMOUSE_INPUT_DATA;

typedef VOID
( *MouseClassServiceCallbackFn )(
	PDEVICE_OBJECT DeviceObject ,
	PMOUSE_INPUT_DATA InputDataStart ,
	PMOUSE_INPUT_DATA InputDataEnd ,
	PULONG InputDataConsumed
	);

typedef struct _MOUSE_OBJECT
{
	PDEVICE_OBJECT              mouse_device;
	MouseClassServiceCallbackFn service_callback;
	BOOL                        use_mouse;
} MOUSE_OBJECT , * PMOUSE_OBJECT;

inline MOUSE_OBJECT gMouseObject;

inline QWORD g_target_routine = 0;

#define KeMRaiseIrql(a,b) *(b) = KfRaiseIrql(a)

EXTERN_C VOID MouseClassServiceCallbackTrick( QWORD rdi_buffer , QWORD rbp_buffer , QWORD target_address );

extern "C"
{
	inline UINT64 _KeAcquireSpinLockAtDpcLevel;
	inline UINT64 _KeReleaseSpinLockFromDpcLevel;
	inline UINT64 _IofCompleteRequest;
	inline UINT64 _IoReleaseRemoveLockEx;
}
