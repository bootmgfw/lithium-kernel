#pragma once

#include <dependencies/includes.hpp>
#define to_lower_c(Char) ((Char >= (char*)'A' && Char <= (char*)'Z') ? (Char + 32) : Char)

typedef union _virt_addr_t
{
	void* value;
	struct
	{
		uintptr_t offset : 12;
		uintptr_t pt_index : 9;
		uintptr_t pd_index : 9;
		uintptr_t pdpt_index : 9;
		uintptr_t pml4_index : 9;
		uintptr_t reserved : 16;
	};
} virt_addr_t , * pvirt_addr_t;

typedef struct _MI_ACTIVE_PFN
{
	union
	{
		struct
		{
			struct
			{
				unsigned __int64 Tradable : 1;
				unsigned __int64 NonPagedBuddy : 43;
			};
		} Leaf;
		struct
		{
			struct
			{
				unsigned __int64 Tradable : 1;
				unsigned __int64 WsleAge : 3;
				unsigned __int64 OldestWsleLeafEntries : 10;
				unsigned __int64 OldestWsleLeafAge : 3;
				unsigned __int64 NonPagedBuddy : 43;
			};
		} PageTable;
		unsigned __int64 EntireActiveField;
	};
} MI_ACTIVE_PFN , * PMI_ACTIVE_PFN;

typedef struct _MMPTE_HARDWARE
{
	struct
	{
		unsigned __int64 Valid : 1;
		unsigned __int64 Dirty1 : 1;
		unsigned __int64 Owner : 1;
		unsigned __int64 WriteThrough : 1;
		unsigned __int64 CacheDisable : 1;
		unsigned __int64 Accessed : 1;
		unsigned __int64 Dirty : 1;
		unsigned __int64 LargePage : 1;
		unsigned __int64 Global : 1;
		unsigned __int64 CopyOnWrite : 1;
		unsigned __int64 Unused : 1;
		unsigned __int64 Write : 1;
		unsigned __int64 PageFrameNumber : 40;
		unsigned __int64 ReservedForSoftware : 4;
		unsigned __int64 WsleAge : 4;
		unsigned __int64 WsleProtection : 3;
		unsigned __int64 NoExecute : 1;
	};
} MMPTE_HARDWARE , * PMMPTE_HARDWARE;

typedef struct _MMPTE_PROTOTYPE
{
	struct
	{
		unsigned __int64 Valid : 1;
		unsigned __int64 DemandFillProto : 1;
		unsigned __int64 HiberVerifyConverted : 1;
		unsigned __int64 ReadOnly : 1;
		unsigned __int64 SwizzleBit : 1;
		unsigned __int64 Protection : 5;
		unsigned __int64 Prototype : 1;
		unsigned __int64 Combined : 1;
		unsigned __int64 Unused1 : 4;
		__int64 ProtoAddress : 48;
	};
} MMPTE_PROTOTYPE , * PMMPTE_PROTOTYPE;

typedef struct _MMPTE_SOFTWARE
{
	struct
	{
		unsigned __int64 Valid : 1;
		unsigned __int64 PageFileReserved : 1;
		unsigned __int64 PageFileAllocated : 1;
		unsigned __int64 ColdPage : 1;
		unsigned __int64 SwizzleBit : 1;
		unsigned __int64 Protection : 5;
		unsigned __int64 Prototype : 1;
		unsigned __int64 Transition : 1;
		unsigned __int64 PageFileLow : 4;
		unsigned __int64 UsedPageTableEntries : 10;
		unsigned __int64 ShadowStack : 1;
		unsigned __int64 Unused : 5;
		unsigned __int64 PageFileHigh : 32;
	};
} MMPTE_SOFTWARE , * PMMPTE_SOFTWARE;

typedef struct _MMPTE_TIMESTAMP
{
	struct
	{
		unsigned __int64 MustBeZero : 1;
		unsigned __int64 Unused : 3;
		unsigned __int64 SwizzleBit : 1;
		unsigned __int64 Protection : 5;
		unsigned __int64 Prototype : 1;
		unsigned __int64 Transition : 1;
		unsigned __int64 PageFileLow : 4;
		unsigned __int64 Reserved : 16;
		unsigned __int64 GlobalTimeStamp : 32;
	};
} MMPTE_TIMESTAMP , * PMMPTE_TIMESTAMP;

typedef struct _MMPTE_TRANSITION
{
	struct
	{
		unsigned __int64 Valid : 1;
		unsigned __int64 Write : 1;
		unsigned __int64 Spare : 1;
		unsigned __int64 IoTracker : 1;
		unsigned __int64 SwizzleBit : 1;
		unsigned __int64 Protection : 5;
		unsigned __int64 Prototype : 1;
		unsigned __int64 Transition : 1;
		unsigned __int64 PageFrameNumber : 40;
		unsigned __int64 Unused : 12;
	};
} MMPTE_TRANSITION , * PMMPTE_TRANSITION;

typedef struct _MMPTE_SUBSECTION
{
	struct
	{
		unsigned __int64 Valid : 1;
		unsigned __int64 Unused0 : 3;
		unsigned __int64 SwizzleBit : 1;
		unsigned __int64 Protection : 5;
		unsigned __int64 Prototype : 1;
		unsigned __int64 ColdPage : 1;
		unsigned __int64 Unused1 : 3;
		unsigned __int64 ExecutePrivilege : 1;
		__int64 SubsectionAddress : 48;
	};
} MMPTE_SUBSECTION , * PMMPTE_SUBSECTION;

typedef struct _MMPTE_LIST
{
	struct
	{
		unsigned __int64 Valid : 1;
		unsigned __int64 OneEntry : 1;
		unsigned __int64 filler0 : 2;
		unsigned __int64 SwizzleBit : 1;
		unsigned __int64 Protection : 5;
		unsigned __int64 Prototype : 1;
		unsigned __int64 Transition : 1;
		unsigned __int64 filler1 : 16;
		unsigned __int64 NextEntry : 36;
	};
} MMPTE_LIST , * PMMPTE_LIST;

typedef struct _MMPTE
{
	union
	{
		union
		{
			unsigned __int64 Long;
			volatile unsigned __int64 VolatileLong;
			struct _MMPTE_HARDWARE Hard;
			struct _MMPTE_PROTOTYPE Proto;
			struct _MMPTE_SOFTWARE Soft;
			struct _MMPTE_TIMESTAMP TimeStamp;
			struct _MMPTE_TRANSITION Trans;
			struct _MMPTE_SUBSECTION Subsect;
			struct _MMPTE_LIST List;
		};
	} u;
} MMPTE , * PMMPTE;

typedef struct _MIPFNBLINK
{
	union
	{
		struct
		{
			unsigned __int64 Blink : 40;
			unsigned __int64 NodeBlinkLow : 19;
			unsigned __int64 TbFlushStamp : 3;
			unsigned __int64 PageBlinkDeleteBit : 1;
			unsigned __int64 PageBlinkLockBit : 1;
		};
		struct
		{
			unsigned __int64 ShareCount : 62;
			unsigned __int64 PageShareCountDeleteBit : 1;
			unsigned __int64 PageShareCountLockBit : 1;
		};
		unsigned __int64 EntireField;
		volatile __int64 Lock;
		struct
		{
			unsigned __int64 LockNotUsed : 62;
			unsigned __int64 DeleteBit : 1;
			unsigned __int64 LockBit : 1;
		};
	};
} MIPFNBLINK , * PMIPFNBLINK;

typedef struct _MMPFNENTRY1
{
	struct
	{
		unsigned char PageLocation : 3;
		unsigned char WriteInProgress : 1;
		unsigned char Modified : 1;
		unsigned char ReadInProgress : 1;
		unsigned char CacheAttribute : 2;
	};
} MMPFNENTRY1 , * PMMPFNENTRY1;

typedef struct _MMPFNENTRY3
{
	struct
	{
		unsigned char Priority : 3;
		unsigned char OnProtectedStandby : 1;
		unsigned char InPageError : 1;
		unsigned char SystemChargedPage : 1;
		unsigned char RemovalRequested : 1;
		unsigned char ParityError : 1;
	};
} MMPFNENTRY3 , * PMMPFNENTRY3;

typedef struct _MI_PFN_ULONG5
{
	union
	{
		unsigned long EntireField;
		struct
		{
			struct
			{
				unsigned long NodeBlinkHigh : 21;
				unsigned long NodeFlinkMiddle : 11;
			};
		} StandbyList;
		struct
		{
			unsigned char ModifiedListBucketIndex : 4;
		} MappedPageList;
		struct
		{
			struct
			{
				unsigned char AnchorLargePageSize : 2;
				unsigned char Spare1 : 6;
			};
			unsigned char ViewCount;
			unsigned short Spare2;
		} Active;
	};
} MI_PFN_ULONG5 , * PMI_PFN_ULONG5;

typedef struct _MMPFN
{
	union
	{
		struct _LIST_ENTRY ListEntry;
		struct _RTL_BALANCED_NODE TreeNode;
		struct
		{
			union
			{
				union
				{
					struct _SINGLE_LIST_ENTRY NextSlistPfn;
					void* Next;
					struct
					{
						unsigned __int64 Flink : 40;
						unsigned __int64 NodeFlinkLow : 24;
					};
					struct _MI_ACTIVE_PFN Active;
				};
			} u1;
			union
			{
				struct _MMPTE* PteAddress;
				unsigned __int64 PteLong;
			};
			struct _MMPTE OriginalPte;
		};
	};
	struct _MIPFNBLINK u2;
	union
	{
		union
		{
			struct
			{
				unsigned short ReferenceCount;
				struct _MMPFNENTRY1 e1;
				struct _MMPFNENTRY3 e3;
			};
			struct
			{
				unsigned short ReferenceCount;
			} e2;
			struct
			{
				unsigned long EntireField;
			} e4;
		};
	} u3;
	struct _MI_PFN_ULONG5 u5;
	union
	{
		union
		{
			struct
			{
				unsigned __int64 PteFrame : 40;
				unsigned __int64 ResidentPage : 1;
				unsigned __int64 Unused1 : 1;
				unsigned __int64 Unused2 : 1;
				unsigned __int64 Partition : 10;
				unsigned __int64 FileOnly : 1;
				unsigned __int64 PfnExists : 1;
				unsigned __int64 NodeFlinkHigh : 5;
				unsigned __int64 PageIdentity : 3;
				unsigned __int64 PrototypePte : 1;
			};
			unsigned __int64 EntireField;
		};
	} u4;
} MMPFN , * PMMPFN;


typedef union _pte
{
	unsigned __int64 value;
	struct
	{
		unsigned __int64 present : 1;
		unsigned __int64 rw : 1;
		unsigned __int64 user_supervisor : 1;
		unsigned __int64 page_write : 1;
		unsigned __int64 page_cache : 1;
		unsigned __int64 accessed : 1;
		unsigned __int64 dirty : 1;
		unsigned __int64 page_access_type : 1;
		unsigned __int64 global : 1;
		unsigned __int64 ignored2 : 3;
		unsigned __int64 pfn : 36;
		unsigned __int64 reserved : 4;
		unsigned __int64 ignored3 : 7;
		unsigned __int64 protect_key : 4;
		unsigned __int64 nx : 1;
	};
} pte , * ppte;

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName [ 256 ];
} RTL_PROCESS_MODULE_INFORMATION , * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules [ 1 ];
} RTL_PROCESS_MODULES , * PRTL_PROCESS_MODULES;

typedef struct _RTL_CRITICAL_SECTION {
	VOID* DebugInfo;
	LONG LockCount;
	LONG RecursionCount;
	PVOID OwningThread;
	PVOID LockSemaphore;
	ULONG SpinCount;
} RTL_CRITICAL_SECTION , * PRTL_CRITICAL_SECTION;

typedef struct _PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY ModuleListLoadOrder;
	LIST_ENTRY ModuleListMemoryOrder;
	LIST_ENTRY ModuleListInitOrder;
} PEB_LDR_DATA , * PPEB_LDR_DATA;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE Reserved1 [ 16 ];
	PVOID Reserved2 [ 10 ];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS , * PRTL_USER_PROCESS_PARAMETERS;

typedef void( __stdcall* PPS_POST_PROCESS_INIT_ROUTINE )( void );

typedef struct _PEB {
	BYTE Reserved1 [ 2 ];
	BYTE BeingDebugged;
	BYTE Reserved2 [ 1 ];
	PVOID Reserved3 [ 2 ];
	PPEB_LDR_DATA Ldr;
	PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
	PVOID Reserved4 [ 3 ];
	PVOID AtlThunkSListPtr;
	PVOID Reserved5;
	ULONG Reserved6;
	PVOID Reserved7;
	ULONG Reserved8;
	ULONG AtlThunkSListPtr32;
	PVOID Reserved9 [ 45 ];
	BYTE Reserved10 [ 96 ];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE Reserved11 [ 128 ];
	PVOID Reserved12 [ 1 ];
	ULONG SessionId;
} PEB , * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	PVOID SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY , * PLDR_DATA_TABLE_ENTRY;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation ,
	SystemProcessorInformation ,
	SystemPerformanceInformation ,
	SystemTimeOfDayInformation ,
	SystemPathInformation ,
	SystemProcessInformation ,
	SystemCallCountInformation ,
	SystemDeviceInformation ,
	SystemProcessorPerformanceInformation ,
	SystemFlagsInformation ,
	SystemCallTimeInformation ,
	SystemModuleInformation ,
	SystemLocksInformation ,
	SystemStackTraceInformation ,
	SystemPagedPoolInformation ,
	SystemNonPagedPoolInformation ,
	SystemHandleInformation ,
	SystemObjectInformation ,
	SystemPageFileInformation ,
	SystemVdmInstemulInformation ,
	SystemVdmBopInformation ,
	SystemFileCacheInformation ,
	SystemPoolTagInformation ,
	SystemInterruptInformation ,
	SystemDpcBehaviorInformation ,
	SystemFullMemoryInformation ,
	SystemLoadGdiDriverInformation ,
	SystemUnloadGdiDriverInformation ,
	SystemTimeAdjustmentInformation ,
	SystemSummaryMemoryInformation ,
	SystemNextEventIdInformation ,
	SystemEventIdsInformation ,
	SystemCrashDumpInformation ,
	SystemExceptionInformation ,
	SystemCrashDumpStateInformation ,
	SystemKernelDebuggerInformation ,
	SystemContextSwitchInformation ,
	SystemRegistryQuotaInformation ,
	SystemExtendServiceTableInformation ,
	SystemPrioritySeperation ,
	SystemPlugPlayBusInformation ,
	SystemDockInformation ,
	SystemProcessorSpeedInformation ,
	SystemCurrentTimeZoneInformation ,
	SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS , * PSYSTEM_INFORMATION_CLASS;

typedef struct _KPRCB* PKPRCB;

typedef struct _KAFFINITY_EX {
	USHORT count;
	USHORT size;
	ULONG reserved;
	ULONGLONG bitmap [ 20 ];
} KAFFINITY_EX , * PKAFFINITY_EX;

#define MM_EXECUTE_READWRITE (6)

typedef struct _MMVAD_FLAGS {
	ULONG Lock : 1;
	ULONG LockContended : 1;
	ULONG DeleteInProgress : 1;
	ULONG NoChange : 1;
	ULONG VadType : 3;
	ULONG Protection : 5;
	ULONG PreferredNode : 6;
	ULONG PageSize : 2;
	ULONG PrivateMemory : 1;
} MMVAD_FLAGS , * PMMVAD_FLAGS;

typedef struct _MMVAD_SHORT {
	union {
		struct _MMVAD_SHORT* NextVad;
		RTL_BALANCED_NODE VadNode;
	};

	ULONG StartingVpn;
	ULONG EndingVpn;
	UCHAR StartingVpnHigh;
	UCHAR EndingVpnHigh;
	UCHAR CommitChargeHigh;
	UCHAR SpareNT64VadUChar;
	LONG ReferenceCount;
	EX_PUSH_LOCK PushLock;

	union {
		ULONG LongFlags;
	} u1;
} MMVAD , * PMMVAD;

typedef struct _POOL_TRACKER_BIG_PAGES {
	volatile ULONGLONG Va;
	ULONG Key;
	ULONG Pattern : 8;
	ULONG PoolType : 12;
	ULONG SlushSize : 12;
	ULONGLONG NumberOfBytes;
} POOL_TRACKER_BIG_PAGES , * PPOOL_TRACKER_BIG_PAGES;

typedef struct _memory_basic_information {
	void* BaseAddress;
	void* AllocationBase;
	uint32_t  AllocationProtect;
	uint32_t   PartitionId;
	uint64_t RegionSize;
	uint32_t  State;
	uint32_t  Protect;
	uint32_t  Type;
} memory_basic_information , * pmemory_basic_information;

extern "C"
{
	NTSYSCALLAPI
		NTSTATUS
		NTAPI
		ZwQuerySystemInformation(
			ULONG InfoClass ,
			PVOID Buffer ,
			ULONG Length ,
			PULONG ReturnLength
		);

	NTKERNELAPI
		PETHREAD
		PsGetNextProcessThread(
			__in PEPROCESS Process ,
			__in_opt PETHREAD Thread
		);

	NTSYSCALLAPI
		LONG
		NTAPI
		KeSuspendThread(
			PETHREAD Thread
		);

	NTSYSCALLAPI
		LONG
		NTAPI
		KeResumeThread(
			PETHREAD Thread
		);

	NTSYSCALLAPI
		POBJECT_TYPE* IoDriverObjectType;

	NTSYSCALLAPI
		NTSTATUS
		ObReferenceObjectByName(
			__in PUNICODE_STRING ObjectName ,
			__in ULONG Attributes ,
			__in_opt PACCESS_STATE AccessState ,
			__in_opt ACCESS_MASK DesiredAccess ,
			__in POBJECT_TYPE ObjectType ,
			__in KPROCESSOR_MODE AccessMode ,
			__inout_opt PVOID ParseContext ,
			__out PVOID* Object
		);

}

extern "C" {
	void copy_data( void* destination , const void* source , size_t count );
	__declspec( dllimport ) NTSTATUS __stdcall ZwProtectVirtualMemory( IN HANDLE , PVOID* , SIZE_T* , ULONG , PULONG );
	NTSTATUS __stdcall MmCopyVirtualMemory( PEPROCESS , void* , PEPROCESS , void* , unsigned long long , KPROCESSOR_MODE , unsigned long long* );
	__declspec( dllimport ) PPEB PsGetProcessPeb( PEPROCESS );
	__declspec( dllimport ) PVOID __stdcall RtlFindExportedRoutineByName( void* , PCCH );
	__declspec( dllimport ) VOID NTAPI KeInitializeAffinityEx( PKAFFINITY_EX );
	__declspec( dllimport ) VOID NTAPI KeAddProcessorAffinityEx( PKAFFINITY_EX , INT );
	__declspec( dllimport ) VOID NTAPI HalSendNMI( PKAFFINITY_EX );
	__declspec( dllimport ) PKPRCB NTAPI KeQueryPrcbAddress( __in ULONG );
	__declspec( dllimport ) PVOID PsGetProcessSectionBaseAddress( __in PEPROCESS Process );
}
