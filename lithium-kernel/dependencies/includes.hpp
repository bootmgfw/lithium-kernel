#pragma once

#include <ntdef.h>
#include <stdint.h>
#include <ntifs.h>
#include <ntddk.h>
#include <ntimage.h>
#include <windef.h>
#include <intrin.h>
#include <ntstrsafe.h>

#include <dependencies/additional/definitions.hpp>

#include <dependencies/additional/security/imports/kli_importer.hpp>
#include <dependencies/additional/security/strings/enc_strings.hpp>

#define ALWAYS_INLINE __forceinline

#define l_log( fmt, ... )

#include <dependencies/additional/crt/crt.hpp>

#include <dependencies/additional/structures/structs.hpp>

#include <core/utilities/utilities.hpp>
#include <core/utilities/kernel/kernel.hpp>

#include <core/requests/read-write/physical/physical.hpp>
#include <core/requests/image-base/image-base.hpp>

#include <core/requests/directory-table-base/dtb.hpp>
#include <core/requests/read-write/read-memory/read-memory.hpp>
#include <core/requests/read-write/write-memory/write-memory.hpp>
#include <core/requests/read-write/write-memory/virtual-memory/write-virtual-memory.hpp>
#include <core/requests/read-write/read-memory/virtual-memory/read-virtual-memory.hpp>

#include <core/requests/virtual-memory/thread/threads.hpp>
#include <core/requests/virtual-memory/allocate/allocate.hpp>
#include <core/requests/virtual-memory/protect/protect.hpp>
#include <core/requests/virtual-memory/virtual-query/virtual-query.hpp>
#include <core/requests/virtual-memory/pattern-scan/find-pattern/find-pattern.hpp>
#include <core/requests/virtual-memory/pattern-scan/pattern-scan.hpp>
#include <core/requests/mouse/emulation/mouse.hpp>
#include <core/requests/mouse/mouse.hpp>

#include <core/dispatch/dispatch.hpp>

#include <core/utilities/kernel/thread/offsets/offsets.hpp>
#include <core/utilities/kernel/thread/thread.hpp>

#include <core/utilities/kernel/nmi-callbacks/nmi-callbacks.hpp>
