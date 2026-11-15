<div align="center">

# Lithium

**Historical Windows kernel driver and user-mode client for defensive analysis**

![Language](https://img.shields.io/badge/language-C%2B%2B-blue?style=flat-square)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey?style=flat-square)
![Arch](https://img.shields.io/badge/arch-x64-orange?style=flat-square)
![Mode](https://img.shields.io/badge/mode-kernel--mode-red?style=flat-square)


<p>
  <img src="https://visitor-badge.laobi.icu/badge?page_id=bootmgfw.lithium-kernel" alt="Repository Views" />
</p>

</div>

> [!WARNING]
> Lithium was previously used for game cheating. Its kernel component modifies a loaded third-party driver and exposes operations that can read or write memory across processes, inject mouse input, and conceal a kernel thread. Loading the driver can destabilize Windows. Use an isolated lab system that you own or are explicitly authorized to test.

---

## Overview

Lithium pairs a Windows x64 kernel driver (`lithium-kernel`) with a user-mode client (`lithium-usermode`). Once loaded in kernel mode, Lithium redirects IOCTL handling in MicroWorld's eScan driver `ProcObsrvesx.sys` to its own request handler. This repository preserves the original implementation so researchers can study the hook and evaluate detection and mitigation techniques. Please share defensive findings in GitHub Issues.

The bundled `ProcObsrvesx.sys` reports version `4.0.0.49`. Northwave documented a kernel handle leak in that version as CVE-2024-28519. Whether Lithium's IOCTL hook relies on that handle leak has not been established. [Northwave vulnerability disclosure](https://northwave-cybersecurity.com/vulnerability-notice-escan-antivirus)

---

## Features

| Feature | Description |
|---|---|
| **Physical Memory R/W** | Direct read/write physical addresses via `MmCopyMemory` |
| **Virtual Memory R/W** | Cross-process virtual memory access via `MmCopyVirtualMemory` |
| **DTB Resolution** | Directory table base extraction via PFN database enumeration |
| **Page Table Walking** | Linear address translation with PML4/PDPT/PD/PT layer caching |
| **Pattern Scanning** | IDA-style signature scanning across process memory |
| **Memory Allocation** | `ZwAllocateVirtualMemory` in target process context |
| **Memory Protection** | `ZwProtectVirtualMemory` in target process context |
| **Mouse Emulation** | Kernel-level mouse input via MouClass service callback hook |
| **Thread Hiding** | ETHREAD field spoofing (start address, CID, create time, etc.) |
| **NMI Callback Disable** | NMI callback suppression for debugger evasion |
| **Pool Cleaning** | Big pool table entry removal from kernel pool tracker |

---

## Project Structure

```
lithium/
├── lithium-kernel/              # Kernel-mode driver (KMDF)
│   ├── entry.cpp                # DriverEntry entry point
│   ├── core/
│   │   ├── dispatch/            # IOCTL dispatch and request routing
│   │   ├── requests/
│   │   │   ├── directory-table-base/  # DTB resolution & page table walk
│   │   │   ├── image-base/           # Process module base retrieval
│   │   │   ├── mouse/                # Mouse input emulation via MouClass
│   │   │   ├── read-write/           # Physical and virtual memory R/W
│   │   │   └── virtual-memory/       # Allocate, protect, query, pattern scan
│   │   └── utilities/
│   │       ├── kernel/          # Module enumeration, NMI, thread ops
│   │       └── ...              # Pool cleaner, CRT shim
│   └── dependencies/
│       ├── additional/
│       │   ├── crt/             # Minimal CRT implementation
│       │   ├── security/        # skCrypter string encryption, oxorany
│       │   └── structures/      # Shared invoke data structures
│       └── includes.hpp         # Master include header
│
└── lithium-usermode/            # User-mode client
    ├── entry.cpp                # Client entry point
    ├── core/
    │   └── driver/              # IOCTL communication layer
    └── dependencies/
        ├── additional/
        │   └── structs.hpp      # Page table entry unions & helpers
        └── includes.hpp
```

---

## IOCTL Interface

Communication is established by hooking the IOCTL dispatch routine of an existing driver (`ProcObsrvesx.sys`). At load time, the kernel module locates the target driver, scans for its IRP dispatch handler, and overwrites it with a trampoline to lithium's own `hooked_ioctl`. The user-mode client opens `\\.\ProcObsrvesx` and all IOCTLs are transparently redirected through the hook.

| Code | Input Struct | Action |
|---|---|---|
| `invoke_base` | `base_invoke { pid, handle }` | Retrieve process image base |
| `invoke_dtb` | `dtb_invoke { pid, dtb }` | Resolve directory table base |
| `invoke_read` | `read_invoke { pid, directory_table, address, buffer, size }` | Read physical memory |
| `invoke_write` | `write_invoke { pid, directory_table, address, buffer, size }` | Write physical memory |
| `invoke_read_virtual` | `read_virtual_invoke { pid, address, buffer, size }` | Read virtual memory |
| `invoke_write_virtual` | `write_virtual_invoke { pid, address, buffer, size }` | Write virtual memory |
| `invoke_allocate` | `allocate_invoke { pid, type, protection, address, size }` | Allocate virtual memory |
| `invoke_protect` | `protect_invoke { pid, protection, address, size }` | Change memory protection |
| `invoke_virtual_query` | `query_virtual_invoke { pid, address, mbi }` | Query virtual memory info |
| `invoke_pattern_scan` | `pattern_invoke { pid, signature, address, pattern_address }` | Pattern scan memory |
| `invoke_move_mouse` | `mouse_invoke { x, y, flags, button_flags }` | Emulate mouse input |

---

## Internals

### DTB Resolution

The driver iterates over the physical memory ranges returned by `MmGetPhysicalMemoryRanges`, walks the PFN database to locate the self-referencing PML4 entry, and validates each page table layer to confirm the correct directory table base for a given process.

### Page Table Walking

Linear-to-physical translation traverses the four-level x64 page table hierarchy (PML4 → PDPT → PD → PT) with per-level caching. Large page (2MB / 1GB) support is included on a best-effort basis.

### Mouse Emulation

The driver locates the `MouClass` and `MouHID` driver objects, resolves the `MouseClassServiceCallback` from the HID device extension, and injects `MOUSE_INPUT_DATA` packets directly into the callback chain.

### Thread Hiding

Key `ETHREAD` fields — create time, start address, Win32 start address, kernel stack, CID, and exit status — are saved and replaced with benign or null values. On teardown, original values are restored before thread termination.

---

## Requirements

- Windows 10/11 x64
- Visual Studio 2022 or later with **Windows Driver Kit (WDK) 10.0.26100.0**
- A kernel driver loading method (test signing, kernel mapper, etc.)

---

## Build

1. Open `lithium.sln` in Visual Studio
2. Set configuration to **Release / x64**
3. Build `lithium-kernel` → `lithium-kernel.sys`
4. Build `lithium-usermode` → `lithium-usermode.exe`

Or via MSBuild:

```
msbuild lithium.sln /p:Configuration=Release /p:Platform=x64
```

---

## Usage

```cpp
#include <core/driver/driver.hpp>

g_driver->initialize_handle();
DWORD pid = g_driver->get_process_id(L"target.exe");
g_driver->attach(pid);

g_driver->image_base = g_driver->get_image_base(0);
g_driver->dtb = g_driver->resolve_directory_table_base();

auto value = g_driver->read<uintptr_t>(g_driver->image_base + 0xOFFSET);
g_driver->mouse_event(-10, -10, 0, 0);
```

---

## Dependencies

- [skCrypter](https://github.com/skadro-official/skCrypter) — compile-time string encryption
- [oxorany](https://github.com/oxorany/oxorany) — compile-time constant obfuscation
