/*
 * cpuid.c - A sample program showing a method to identify CPU features
 * using the processor's CPUID instruction.
 *
 * For 16-bit or 32-bit environments. A 32-bit CPU is required.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

/* Miscellaneous constants */
#define EFL_CPUID           (1L << 21)      /* The CPUID bit in EFLAGS. */

#define CPUID_EAX           0               /* Index of EAX value in array. */
#define CPUID_EBX           1               /* Index of EBX value in array. */
#define CPUID_ECX           2               /* Index of ECX value in array. */
#define CPUID_EDX           3               /* Index of EDX value in array. */

#define CPUID_STD_BASE      0x00000000      /* Standard leaves base. */
#define CPUID_HV_BASE       0x40000000      /* Hypervisor leaves base. */
#define CPUID_EXT_BASE      0x80000000      /* Extended leaves base. */
#define CPUID_CENTAUR_BASE  0xC0000000      /* Centaur leaves base. */
#define CPUID_BRAND_1ST     0x80000002      /* First brand string leaf. */
#define CPUID_BRAND_COUNT   3               /* Number of brand leaves. */


/* Read the EFLAGS register. */
uint32_t eflags_read( void );
#if defined( _M_I386 )
#pragma aux eflags_read = \
    "pushfd"              \
    "pop  eax"            \
    value [eax] modify [eax];
#elif defined( _M_I86 )
#pragma aux eflags_read = \
    ".386"                \
    "pushfd"              \
    "pop  edx"            \
    "mov  ax,dx"          \
    "shr  edx,16"         \
    value [dx ax] modify [dx ax]
#else
#error Unsupported target architecture!
#endif


/* Write the EFLAGS register. */
uint32_t eflags_write( uint32_t eflg );
#if defined( _M_I386 )
#pragma aux eflags_write = \
    "push eax"             \
    "popfd"                \
    parm [eax] modify [];
#elif defined( _M_I86 )
#pragma aux eflags_write = \
    ".386"                \
    "shl  edx,16"         \
    "mov  dx,ax"          \
    "push edx"            \
    "popfd"               \
    parm [dx ax] modify [dx ax]
#else
#error Unsupported target architecture!
#endif


/* A CPUID instruction wrapper, both 16-bit and 32-bit. */
void cpu_id( uint32_t cpuinfo[4], uint32_t infotype );
#if defined( _M_I386 )
#pragma aux cpu_id =      \
    ".586"                \
    "cpuid"               \
    "mov  [esi+0],eax"    \
    "mov  [esi+4],ebx"    \
    "mov  [esi+8],ecx"    \
    "mov  [esi+12],edx"   \
    parm [esi] [eax] modify [ebx ecx edx];
#elif defined( _M_I86 )
#pragma aux cpu_id =      \
    ".586"                \
    "shl  edx,16"         \
    "mov  dx,ax"          \
    "mov  eax,edx"        \
    "cpuid"               \
    "mov  [di+0],eax"     \
    "mov  [di+4],ebx"     \
    "mov  [di+8],ecx"     \
    "mov  [di+12],edx"    \
    parm [di] [dx ax] modify [bx cx dx]
#else
#error Unsupported target architecture!
#endif

static const char   *feature_edx_desc[][2] =
{
    { "On-chip x87", "FPU" },
    { "Virtual 8086 Mode Enhancements", "VME" },
    { "Debugging Extensions", "DE" },
    { "Page Size Extensions", "PSE" },
    { "Time Stamp Counter", "TSC" },
    { "RDMSR/WRMSR Instructions", "MSR" },
    { "Physical Address Extensions", "PAE" },
    { "Machine Check Exception", "MCE" },
    { "CMPXCHG8B Instruction", "CX8" },
    { "On-chip APIC", "APIC" },
    { "Reserved 1", "R1!!" },                   /* Bit 10 */
    { "SYSENTER/SYSEXIT Instructions", "SEP" },
    { "Memory Type Range Registers", "MTRR" },
    { "PTE Global Bit", "PGE" },
    { "Machine Check Architecture", "MCA" },
    { "Conditional Move Instructions", "CMOV" },
    { "Page Attribute Table", "PAT" },
    { "36-bit Page Size Extension", "PSE-36" },
    { "Processor Serial Number", "PSN" },
    { "CFLUSH Instruction", "CLFSH" },
    { "Reserved 2", "R2!!" },                   /* Bit 20 */
    { "Debug Store", "DS" },
    { "Thermal Monitor and Clock Control", "ACPI" },
    { "MMX Technology", "MMX" },
    { "FXSAVE/FXRSTOR Instructions", "FXSR" },
    { "SSE Extensions", "SSE" },
    { "SSE2 Extensions", "SSE2" },
    { "Self Snoop", "SS" },
    { "Hyper-threading Technology", "HTT" },
    { "Thermal Monitor", "TM" },
    { "Reserved 3", "R3!!" },                   /* Bit 30 */
    { "Pending Break Enable", "PBE" }
};

static const char   *feature_ecx_desc[][2] =
{
    { "Streaming SIMD Extensions 3", "SSE3" },
    { "PCLMULDQ Instruction", "PCLMULDQ" },
    { "64-bit Debug Store", "DTES64" },
    { "MONITOR/MWAIT Instructions", "MONITOR" },
    { "CPL Qualified Debug Store", "DS-CPL" },
    { "Virtual Machine Extensions", "VMX" },
    { "Safer Mode Extensions", "SMX" },
    { "Enhanced Intel SpeedStep", "EIST" },
    { "Thermal Monitor 2", "TM2" },
    { "Supplemental SSE3", "SSSE3" },
    { "L1 Context ID", "CNTX-ID" },             /* Bit 10 */
    { "Reserved 1", "R1!!" },
    { "Fused Multiply Add", "FMA" },
    { "CMPXCHG16B Instruction", "CX16" },
    { "xTPR Update Control", "xTPR" },
    { "Perfmon/Debug Capability", "PDCM" },
    { "Reserved 2", "R2!!" },
    { "Process Context Identifiers", "PCID" },
    { "Direct Cache Access", "DCA" },
    { "Streaming SIMD Extensions 4.1", "SSE4.1" },
    { "Streaming SIMD Extensions 4.2", "SSE4.2" }, /* Bit 20 */
    { "Extended xAPIC Support", "x2APIC" },
    { "MOVBE Instruction", "MOVBE" },
    { "POPCNT Instruction", "POPCNT" },
    { "Time Stamp Counter Deadline", "TSC-DEADLINE" },
    { "AES Instruction Extensions", "AES" },
    { "XSAVE/XRSTOR States", "XSAVE" },
    { "OS-Enabled Ext State Mgmt", "OSXSAVE" },
    { "Advanced Vector Extensions", "AVX" },
    { "Reserved 3", "R3!!" },
    { "Reserved 4", "R4!!" },                   /* Bit 30 */
    { "Hypervisor Present", "HVP" }
};

/* Determine the number of CPUID leaves available for basic/extended/etc.
 * CPUID. Due to poor design, it is difficult to reliably detect certain
 * unimplemented leaves. We assume that at least two leaves must be present
 * for a range to be valid.
 */
int cpuid_get_leaf_count( uint32_t base )
{
    uint32_t    cpu_info[4];
    uint32_t    leaf_count;

    cpu_id( cpu_info, base );
    leaf_count = cpu_info[CPUID_EAX] > base ?
                 cpu_info[CPUID_EAX] - base : 0;

    /* A result higher than 1024 is considered garbage. */
    return( leaf_count > 1024 ? 0 : leaf_count );
}

/* Dump available CPUID leaves in a machine-readable format. */
void dump_raw_cpuid( void )
{
    uint32_t    cpu_info[4];
    uint32_t    base;
    int         max_id;
    int         i, j;

    for( i = 0; i < 4; ++i ) {
        base = (uint32_t)i << 30;
        max_id = cpuid_get_leaf_count( base );
        if( max_id ) {
            for( j = 0; j <= max_id; ++j ) {
                cpu_id( cpu_info, base + j );
                printf( "%08lx: %08lx %08lx %08lx %08lx\n", base + j,
                        cpu_info[CPUID_EAX], cpu_info[CPUID_EBX],
                        cpu_info[CPUID_ECX], cpu_info[CPUID_EDX] );
            }
        }
    }
}

/* Print the hypervisor identification, if present. */
void print_hyper_id( int verbose )
{
    uint32_t    cpu_info[4];
    uint32_t    max_hv_id;
    char        vendor_string[3 * 4];
    char        intrfc_string[3 * 4];

    max_hv_id = cpuid_get_leaf_count( CPUID_HV_BASE );
    if( max_hv_id ) {
        cpu_id( cpu_info, CPUID_HV_BASE );
        *(uint32_t *)(vendor_string + 0) = cpu_info[CPUID_EBX];
        *(uint32_t *)(vendor_string + 4) = cpu_info[CPUID_ECX];
        *(uint32_t *)(vendor_string + 8) = cpu_info[CPUID_EDX];
        /* Note that the signature strings may not be null-terminated. */
        printf( "Hypervisor signature: %.12s", vendor_string );
        cpu_id( cpu_info, CPUID_HV_BASE + 1 );
        *(uint32_t *)intrfc_string = cpu_info[CPUID_EAX];
        printf( "  interface: %.4s", intrfc_string );
    }
}

void print_cpuid( int verbose )
{
    uint32_t    cpu_info[4];
    uint32_t    max_id;
    uint32_t    max_ext_id;
    uint32_t    i;
    uint32_t    features_edx;
    uint32_t    features_ecx;
    char        cpu_vendor[16];
    char        brand_string[12 * 4];

    /* Obtain the number of CPUID leaves and the vendor string. */
    cpu_id( cpu_info, 0 );
    max_id = cpu_info[CPUID_EAX];
    memset( cpu_vendor, 0, sizeof( cpu_vendor ) );
    *(uint32_t *)(cpu_vendor + 0) = cpu_info[CPUID_EBX];
    *(uint32_t *)(cpu_vendor + 4) = cpu_info[CPUID_EDX];
    *(uint32_t *)(cpu_vendor + 8) = cpu_info[CPUID_ECX];
    printf( "CPU vendor string: %s\n", cpu_vendor );

    /* Obtain the model and features. */
    cpu_id( cpu_info, 1 );
    features_edx = cpu_info[CPUID_EDX];
    features_ecx = cpu_info[CPUID_ECX];

    /* Interpret the feature bits. */

    printf( "Processor features:\n" );
    for( i = 0; i < 32; ++i ) {
        if( features_edx & (1L << i) ) {
            if( verbose )
                printf( "%s (%s)\n", feature_edx_desc[i][0],
                        feature_edx_desc[i][1] );
            else
                printf( "%s ", feature_edx_desc[i][1] );
        }
    }
    for( i = 0; i < 32; ++i ) {
        if( features_ecx & (1L << i) ) {
            if( verbose )
                printf( "%s (%s)\n", feature_ecx_desc[i][0],
                        feature_ecx_desc[i][1] );
            else
                printf( "%s ", feature_ecx_desc[i][1] );
        }
    }
    if( !verbose )
        printf( "\n" );

    /* Obtain the brand string, if available. */
    max_ext_id = cpuid_get_leaf_count( CPUID_EXT_BASE );
    if( max_ext_id > 4 ) {
        for( i = 0; i < CPUID_BRAND_COUNT; ++i ) {
            cpu_id( cpu_info, CPUID_BRAND_1ST + i );
            memcpy( brand_string + i * sizeof( cpu_info ), 
                    cpu_info, sizeof( cpu_info ) );
        }
        /* Note that the brand string is null-terminated. */
        printf( "CPU brand string: %s\n", brand_string );
    }

    /* Print the hypervisor signature, if any. */
    print_hyper_id( verbose );
}

/* Check if CPUID instruction is supported. Some CPUs without CPUID support
 * may not raise an invalid opcode exception when CPUID is executed.
 */
int have_cpu_id( void )
{
    uint32_t    old_eflg;
    uint32_t    new_eflg;

    old_eflg = eflags_read();
    new_eflg = old_eflg ^ EFL_CPUID;
    eflags_write( new_eflg );
    new_eflg = eflags_read();
    return( new_eflg != old_eflg );
}

int main( int argc, char **argv )
{
    int         c;
    int         verbose;
    int         raw_dump;

    verbose = raw_dump = 0;

    while( (c = getopt( argc, argv, ":rv" )) != -1 ) {
        switch( c ) {
        case 'r':
            raw_dump = 1;
            break;
        case 'v':
            verbose = 1;
            break;
        case '?':
            printf( "usage: %s [-r] [-v]\n", argv[0] );
            return( EXIT_FAILURE );
        }
    }

    if( !have_cpu_id() ) {
        printf( "CPUID instruction not supported!\n" );
        return( EXIT_FAILURE );
    }

    if( raw_dump ) {
        dump_raw_cpuid();
    } else {
        print_cpuid( verbose );
    }
    return( EXIT_SUCCESS );
}
