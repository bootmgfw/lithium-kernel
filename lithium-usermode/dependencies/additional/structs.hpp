#pragma once
#include <dependencies/includes.hpp>


struct IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID Pointer;
	};
	ULONG_PTR Information;
};

using PIO_STATUS_BLOCK = IO_STATUS_BLOCK*;
using PIO_APC_ROUTINE = void( NTAPI* )(
	PVOID ,
	PIO_STATUS_BLOCK ,
	std::uint32_t );

extern "C" __int64 direct_device_control(
	HANDLE FileHandle ,
	HANDLE Event ,
	PIO_APC_ROUTINE ApcRoutine ,
	PVOID ApcContext ,
	PIO_STATUS_BLOCK IoStatusBlock ,
	std::uint32_t IoControlCode ,
	PVOID InputBuffer ,
	std::uint32_t InputBufferLength ,
	PVOID OutputBuffer ,
	std::uint32_t OutputBufferLength );


typedef union _pml4e {
	std::uint64_t value;
	struct {
		std::uint64_t present : 1;
		std::uint64_t writeable : 1;
		std::uint64_t user_supervisor : 1;
		std::uint64_t PageWriteThrough : 1;
		std::uint64_t page_cache : 1;
		std::uint64_t accessed : 1;
		std::uint64_t Ignored1 : 1;
		std::uint64_t large_page : 1;
		std::uint64_t Ignored2 : 4;
		std::uint64_t pfn : 36;
		std::uint64_t Reserved : 4;
		std::uint64_t Ignored3 : 11;
		std::uint64_t nx : 1;
	};
} pml4e , * ppml4e;

typedef union _pdpte {
	std::uint64_t value;
	struct {
		std::uint64_t present : 1;
		std::uint64_t rw : 1;
		std::uint64_t user_supervisor : 1;
		std::uint64_t PageWriteThrough : 1;
		std::uint64_t page_cache : 1;
		std::uint64_t accessed : 1;
		std::uint64_t Ignored1 : 1;
		std::uint64_t large_page : 1;
		std::uint64_t Ignored2 : 4;
		std::uint64_t pfn : 36;
		std::uint64_t Reserved : 4;
		std::uint64_t Ignored3 : 11;
		std::uint64_t nx : 1;
	};
} pdpte , * ppdpte;

typedef union _pde {
	std::uint64_t value;
	struct {
		std::uint64_t present : 1;
		std::uint64_t rw : 1;
		std::uint64_t user_supervisor : 1;
		std::uint64_t PageWriteThrough : 1;
		std::uint64_t page_cache : 1;
		std::uint64_t accessed : 1;
		std::uint64_t Ignored1 : 1;
		std::uint64_t large_page : 1;
		std::uint64_t Ignored2 : 4;
		std::uint64_t pfn : 36;
		std::uint64_t Reserved : 4;
		std::uint64_t Ignored3 : 11;
		std::uint64_t nx : 1;
	};
} pde , * ppde;

typedef union _pte {
	std::uint64_t value;
	struct {
		std::uint64_t present : 1;
		std::uint64_t rw : 1;
		std::uint64_t user_supervisor : 1;
		std::uint64_t PageWriteThrough : 1;
		std::uint64_t page_cache : 1;
		std::uint64_t accessed : 1;
		std::uint64_t Dirty : 1;
		std::uint64_t PageAccessType : 1;
		std::uint64_t Global : 1;
		std::uint64_t Ignored2 : 3;
		std::uint64_t pfn : 36;
		std::uint64_t reserved : 4;
		std::uint64_t Ignored3 : 7;
		std::uint64_t ProtectionKey : 4;
		std::uint64_t nx : 1;
	};
} pte , * ppte;

struct pt_entries {
	std::pair<ppml4e , pml4e>	pml4;
	std::pair<ppdpte , pdpte>	pdpt;
	std::pair<ppde , pde>		pd;
	std::pair<ppte , pte>		pt;
};



typedef LARGE_INTEGER PHYSICAL_ADDRESS , * PPHYSICAL_ADDRESS;

typedef struct _PHYSICAL_MEMORY_RANGE {
	PHYSICAL_ADDRESS BaseAddress;
	LARGE_INTEGER NumberOfBytes;
} PHYSICAL_MEMORY_RANGE , * PPHYSICAL_MEMORY_RANGE;

typedef union _virt_addr_t {
	std::uintptr_t value;
	struct {
		std::uint64_t offset : 12;
		std::uint64_t pt_index : 9;
		std::uint64_t pd_index : 9;
		std::uint64_t pdpt_index : 9;
		std::uint64_t pml4_index : 9;
		std::uint64_t reserved : 16;
	};
} virt_addr_t , * pvirt_addr_t;

typedef struct _MI_ACTIVE_PFN {
	union {
		struct {
			struct
			{
				unsigned __int64 Tradable : 1;
				unsigned __int64 NonPagedBuddy : 43;
			};
		} Leaf;
		struct {
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

typedef struct _MMPTE_HARDWARE {
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

typedef struct _MMPTE_PROTOTYPE {
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

typedef struct _MMPTE_SOFTWARE {
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

typedef struct _MMPTE_TIMESTAMP {
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

typedef struct _MMPTE_TRANSITION {
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

typedef struct _MMPTE_SUBSECTION {
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

typedef struct _MMPTE_LIST {
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

typedef struct _MMPTE {
	union {
		union {
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

struct cache {
	uintptr_t Address;
	MMPTE Value;
};

static cache cached_pml4e [ 512 ];
static cache cached_pdpte [ 512 ];
static cache cached_pde [ 512 ];
static cache cached_pte [ 512 ];
