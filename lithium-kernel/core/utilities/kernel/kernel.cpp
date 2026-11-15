#include "kernel.hpp"

#define o

#define in_range( x, a, b ) ( x >= a && x <= b ) 
#define get_bits( x ) ( in_range( ( x & ( o( ~0x20 ) ) ), o( 'A' ), o( 'F' ) ) ? ( ( x & ( o( ~0x20 ) ) ) - o( 'A' ) + o( 0xA ) ) : ( in_range( x, o( '0' ), o( '9' ) ) ? x - o( '0' ) : o( 0 ) ) )
#define get_byte( x ) ( get_bits( x[ o( 0 ) ] ) << o( 4 ) | get_bits( x[ o( 1 ) ] ) )

namespace utilities::kernel
{
    namespace pattern {
        uint64_t find_pattern( uint64_t base , size_t range , const char* pattern , const char* mask ) {
            const auto check_mask = [ ] ( const char* base , const char* pattern , const char* mask ) -> bool {
                for ( ; *mask; ++base , ++pattern , ++mask ) {
                    if ( *mask == o( 'x' ) && *base != *pattern ) {
                        return o( false );
                    }
                }

                return o( true );
                };

            range = range - crt::strlen( mask );

            for ( size_t i = 0; i < range; ++i ) {
                if ( check_mask( ( const char* ) base + i , pattern , mask ) ) {
                    return base + i;
                }
            }

            return o( NULL );
        }

        uint64_t find_pattern( uint64_t base , const char* pattern , const char* mask ) {
            const PIMAGE_NT_HEADERS headers = ( PIMAGE_NT_HEADERS ) ( base + ( ( PIMAGE_DOS_HEADER ) base )->e_lfanew );
            const PIMAGE_SECTION_HEADER sections = IMAGE_FIRST_SECTION( headers );

            for ( size_t i = 0; i < headers->FileHeader.NumberOfSections; i++ ) {
                const PIMAGE_SECTION_HEADER section = &sections [ i ];

                if ( section->Characteristics & o( IMAGE_SCN_MEM_EXECUTE ) ) {
                    const auto match = find_pattern( base + section->VirtualAddress , section->Misc.VirtualSize , pattern , mask );

                    if ( match ) {
                        return match;
                    }
                }
            }

            return o( 0 );
        }

        uint64_t find_pattern( uint64_t base , const char* pattern , const char* mask , int len )
        {
            ULONG_PTR ret = 0;
            PIMAGE_DOS_HEADER pidh = ( PIMAGE_DOS_HEADER ) base;
            PIMAGE_NT_HEADERS pinh = ( PIMAGE_NT_HEADERS ) ( ( BYTE* ) pidh + pidh->e_lfanew );
            PIMAGE_SECTION_HEADER pish = ( PIMAGE_SECTION_HEADER ) ( ( BYTE* ) pinh + sizeof( IMAGE_NT_HEADERS64 ) );


            for ( USHORT sec = 0; sec < pinh->FileHeader.NumberOfSections; sec++ )
            {

                if ( ( pish [ sec ].Characteristics & 0x00000020 ) )
                {
                    QWORD address = find_pattern( pish [ sec ].VirtualAddress + ( ULONG_PTR ) ( base ) ,
                        pish [ sec ].Misc.VirtualSize - len , pattern , mask );

                    if ( address ) {
                        ret = address;

                        break;
                    }
                }

            }
            return ret;

        }

        uint64_t find_pattern( uint64_t module_base , const char* pattern ) {
            auto pattern_ = pattern;
            uint64_t first_match = 0;

            if ( !module_base ) {
                return o( 0 );
            }

            const auto nt = reinterpret_cast< IMAGE_NT_HEADERS* >( module_base + reinterpret_cast< IMAGE_DOS_HEADER* >( module_base )->e_lfanew );

            for ( auto current = module_base; current < module_base + nt->OptionalHeader.SizeOfImage; current++ ) {
                if ( !*pattern_ ) {
                    return first_match;
                }

                if ( *( uint8_t* ) pattern_ == o( '\?' ) || *( uint8_t* ) current == get_byte( pattern_ ) ) {
                    if ( !first_match )
                        first_match = current;

                    if ( !pattern_ [ o( 2 ) ] )
                        return first_match;

                    if ( *( uint16_t* ) pattern_ == o( '\?\?' ) || *( uint8_t* ) pattern_ != o( '\?' ) )
                        pattern_ += o( 3 );

                    else
                        pattern_ += o( 2 );
                }
                else {
                    pattern_ = pattern;
                    first_match = o( 0 );
                }
            }

            return o( 0 );
        }

        uintptr_t find_empty_space( uintptr_t module_base , size_t size ) {
            auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( module_base +
                reinterpret_cast< PIMAGE_DOS_HEADER >( module_base )->e_lfanew );

            auto section = IMAGE_FIRST_SECTION( nt_headers );
            for ( WORD i = 0; i < nt_headers->FileHeader.NumberOfSections; ++i , ++section ) {
                if ( section->Misc.VirtualSize > section->SizeOfRawData ) {
                    uintptr_t start = module_base + section->VirtualAddress + section->SizeOfRawData;
                    size_t available = section->Misc.VirtualSize - section->SizeOfRawData;
                    if ( available >= size ) {
                        return start;
                    }
                }
            }
            return 0;
        }
    }

    void* get_system_information( SYSTEM_INFORMATION_CLASS information_class )
    {
        unsigned long size = 32;
        char buffer [ 32 ];

        l_import( ZwQuerySystemInformation )( information_class , buffer , size , &size );

        void* info = l_import( ExAllocatePool )( NonPagedPool , size );

        if ( !info )
            return nullptr;

        if ( !NT_SUCCESS( l_import( ZwQuerySystemInformation )( information_class , info , size , &size ) ) )
        {
            l_import( ExFreePoolWithTag )( info , 0 );
            return nullptr;
        }

        return info;
    }

    uint64_t get_module( const char* name )
    {
        const auto to_lower = [ ] ( char* string ) -> const char*
            {
                for ( char* pointer = string; *pointer != '\0'; ++pointer )
                {
                    *pointer = ( char ) ( short ) l_import( tolower )( *pointer );
                }

                return string;
            };

        const PRTL_PROCESS_MODULES info = ( PRTL_PROCESS_MODULES ) get_system_information( SystemModuleInformation );

        if ( !info )
            return NULL;

        for ( size_t i = 0; i < info->NumberOfModules; ++i )
        {
            const auto& mod = info->Modules [ i ];

            if ( crt::strcmp( to_lower_c( ( char* ) mod.FullPathName + mod.OffsetToFileName ) , name ) == 0 )
            {
                const void* address = mod.ImageBase;
                l_import( ExFreePoolWithTag )( info , 0 );
                return ( uintptr_t ) address;
            }
        }

        l_import( ExFreePoolWithTag )( info , 0 );
        return NULL;
    }


    uintptr_t get_nt_callback_address( ) {
        // 1. Получаем базовый адрес ntdll.dll
        uintptr_t ntdll_base = get_module( l_enc( "ntdll.dll" ) );
        if ( !ntdll_base ) return 0;

        // 2. Парсим PE-заголовки
        auto* dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( ntdll_base );
        auto* nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( ntdll_base + dos_header->e_lfanew );

        // 3. Ищем экспорт NtCallbackReturn
        auto* export_dir = reinterpret_cast< PIMAGE_EXPORT_DIRECTORY >(
            ntdll_base + nt_headers->OptionalHeader.DataDirectory [ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress );

        uint32_t* names = reinterpret_cast< uint32_t* >( ntdll_base + export_dir->AddressOfNames );
        uint16_t* ordinals = reinterpret_cast< uint16_t* >( ntdll_base + export_dir->AddressOfNameOrdinals );
        uint32_t* functions = reinterpret_cast< uint32_t* >( ntdll_base + export_dir->AddressOfFunctions );

        for ( uint32_t i = 0; i < export_dir->NumberOfNames; i++ ) {
            const char* name = reinterpret_cast< const char* >( ntdll_base + names [ i ] );
            if ( strcmp( name , "NtCallbackReturn" ) == 0 ) {
                return ntdll_base + functions [ ordinals [ i ] ];
            }
        }

        return 0;
    }

    bool write_to_rw_memory( void* address , void* buffer , size_t size ) {
        PMDL mdl = l_import( IoAllocateMdl )( address , size , FALSE , FALSE , NULL );
        if ( !mdl )
            return false;

        l_import( MmProbeAndLockPages )( mdl , KernelMode , IoReadAccess );

        PVOID mapping = l_import( MmMapLockedPagesSpecifyCache )( mdl , KernelMode , MmNonCached , 0 , 0 , NormalPagePriority );

        l_import( MmProtectMdlSystemAddress )( mdl , PAGE_READWRITE );

        crt::memcpy( mapping , buffer , size );

        l_import( MmUnmapLockedPages )( mapping , mdl );
        l_import( MmUnlockPages )( mdl );
        l_import( IoFreeMdl )( mdl );

        return true;
    }

    bool read_from_rw_memory( void* destination , void* source , size_t size )
    {
        PMDL mdl = l_import( IoAllocateMdl )( source , ( ULONG ) size , FALSE , FALSE , nullptr );
        if ( !mdl ) return false;

        __try {
            l_import( MmProbeAndLockPages )( mdl , KernelMode , IoReadAccess );
        }
        __except ( EXCEPTION_EXECUTE_HANDLER ) {
            l_import( IoFreeMdl )( mdl );
            return false;
        }

        void* mapped_address = l_import( MmMapLockedPagesSpecifyCache )( mdl ,
            KernelMode ,
            MmNonCached ,
            nullptr ,
            FALSE ,
            NormalPagePriority );
        if ( !mapped_address ) {
            l_import( MmUnlockPages )( mdl );
            l_import( IoFreeMdl )( mdl );
            return false;
        }

        crt::memcpy( destination , mapped_address , size );

        l_import( MmUnmapLockedPages )( mapped_address , mdl );
        l_import( MmUnlockPages )( mdl );
        l_import( IoFreeMdl )( mdl );
        return true;
    }



    uintptr_t resolve_address( uintptr_t Instruction , ULONG OffsetOffset , ULONG InstructionSize )
    {
        LONG RipOffset = *( PLONG ) ( Instruction + OffsetOffset );
        auto ResolvedAddr = (
            Instruction +
            InstructionSize +
            RipOffset );

        return ResolvedAddr;
    }

    uintptr_t get_ntos_base_address( )
    {
        typedef unsigned char uint8_t;
        auto Idt_base = reinterpret_cast< uintptr_t >( KeGetPcr( )->IdtBase );
        auto align_page = *reinterpret_cast< uintptr_t* >( Idt_base + 4 ) >> 0xc << 0xc;

        for ( ; align_page; align_page -= PAGE_SIZE )
        {
            for ( int index = 0; index < PAGE_SIZE - 0x7; index++ )
            {
                auto current_address = static_cast< intptr_t >( align_page ) + index;

                if ( *reinterpret_cast< uint8_t* >( current_address ) == 0x48
                    && *reinterpret_cast< uint8_t* >( current_address + 1 ) == 0x8D
                    && *reinterpret_cast< uint8_t* >( current_address + 2 ) == 0x1D
                    && *reinterpret_cast< uint8_t* >( current_address + 6 ) == 0xFF ) //48 8d 1D ?? ?? ?? FF
                {
                    auto Ntosbase = resolve_address( current_address , 3 , 7 );
                    if ( !( ( UINT64 ) Ntosbase & 0xfff ) )
                    {
                        return Ntosbase;
                    }
                }
            }
        }
        return 0;
    }

    PVOID create_trampoline( PVOID original_func , SIZE_T overwritten_len ) {
        PVOID trampoline = ExAllocatePool( NonPagedPoolExecute , overwritten_len + 14 );

        utilities::crt::memcpy( trampoline , original_func , overwritten_len );

        UCHAR jmp_code [ ] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
        ULONG64 jmp_target = ( ULONG64 ) original_func + overwritten_len;
        utilities::crt::memcpy( ( PUCHAR ) trampoline + overwritten_len , jmp_code , sizeof( jmp_code ) );
        utilities::crt::memcpy( ( PUCHAR ) trampoline + overwritten_len + 6 , &jmp_target , sizeof( jmp_target ) );

        return trampoline;
    }
}