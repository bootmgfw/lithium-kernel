#pragma once

#include <dependencies/includes.hpp>

namespace globals
{
	inline uintptr_t ntos_image_base = 0;
	inline uintptr_t directory_table_base = 0;
	inline uint32_t pid = 0;
	inline virt_addr_t virt_base { 0 };
	inline PEPROCESS process = 0;
	inline bool invalid = false;

	inline void* original_dispatch_bytes = nullptr;
	inline void* trampoline_address;
	inline const size_t patch_size = 12;
}

typedef enum _requests {
	invoke_unique ,
	invoke_base ,
	invoke_read ,
	invoke_write ,
	invoke_dtb ,
	invoke_allocate ,
	invoke_protect ,
	invoke_write_virtual ,
	invoke_read_virtual ,
	invoke_pattern_scan ,
	invoke_virtual_query ,
	invoke_suspend_threads ,
	invoke_resume_threads ,
	invoke_move_mouse
} requests , * prequests;

typedef struct _base_invoke {
	uint32_t pid;
	uintptr_t handle;
} base_invoke , * pbase_invoke;

typedef struct _read_invoke {
	uint32_t pid;
	uint64_t directory_table;
	uint64_t address;
	void* buffer;
	size_t size;
} read_invoke , * pread_invoke;

typedef struct _write_invoke {
	uint32_t pid;
	uint64_t directory_table;
	uint64_t address;
	void* buffer;
	size_t size;
} write_invoke , * pwrite_invoke;

typedef struct _write_virtual_invoke {
	uint32_t pid;
	uint64_t address;
	void* buffer;
	size_t size;
} write_virtual_invoke , * pwrite_virtual_invoke;

typedef struct _read_virtual_invoke {
	uint32_t pid;
	uint64_t address;
	void* buffer;
	size_t size;
} read_virtual_invoke , * pread_virtual_invoke;

typedef struct _query_virtual_invoke {
	uint32_t pid;
	uintptr_t address;
	MEMORY_BASIC_INFORMATION mbi;
} query_virtual_invoke , * pquery_virtual_invoke;

typedef struct _dtb_invoke {
	uint32_t pid;
	uintptr_t dtb;
} dtb_invoke , * pdtb_invoke;

typedef struct _dtb_check_invoke {
	uintptr_t image_base;
	uintptr_t dtb;
	bool is_valid;
} dtb_check_invoke , * pdtb_check_invoke;


typedef struct _allocate_invoke {
	uint32_t pid , type , protection;
	uintptr_t address;
	size_t size;
} allocate_invoke , * pallocate_invoke;

typedef struct _protect_invoke {
	uint32_t pid , protection;
	uintptr_t address;
	size_t size;
} protect_invoke , * pprotect_invoke;

typedef struct _pattern_invoke {
	uint32_t pid;
	char signature [ 260 ];
	uintptr_t address , pattern_address;
} pattern_invoke , * ppattern_invoke;

typedef struct _physical_ranges_invoke {
	PHYSICAL_MEMORY_RANGE* ranges;
} physical_ranges_invoke , * pphysical_ranges_invoke;

typedef struct _cache_invoke {
	uint32_t pid;
} cache_invoke , * pcache_invoke;

typedef struct _suspend_threads_invoke {
	uint32_t pid;
} suspend_threads_invoke , * psuspend_threads_invoke;

typedef struct _resume_threads_invoke {
	uint32_t pid;
} resume_threads_invoke , * presume_threads_invoke;

typedef struct _mouse_invoke {
	int x;
	int y;
	short flags;
	short button_flags;
} mouse_invoke , * pmouse_invoke;

typedef struct _invoke_data {
	uint32_t unique;
	requests code;
	void* data;
} invoke_data , * pinvoke_data;
