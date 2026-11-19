/*
 * syst32.c - 32-Bit Protected-Mode DOS/32A Verification & Instrumentation Suite
 *
 * Exercises and measures DOS/32A across MS-DOS 1.25, 2.00, and 4.00:
 *   1. Flat 32-bit address space & 2 MB extended memory allocation/checksum.
 *   2. Protected-mode INT 21h file creation, write, lseek, read, and unlink.
 *   3. Pre-DOS-3.0 synthesized/augmented environment & argv[] propagation.
 *   4. Null-IVT multiplex (INT 2Ah / INT 2Fh) safety in protected mode.
 *   5. DOS/32A kernel mode query (INT 21h AX=0FF8Ah: raw INT 15h / XMS / VCPI / DPMI),
 *      low-memory buffer overhead (INT 21h AX=0FF89h), and 100-iteration
 *      PM->RM->PM mode-switch cycle latency via RDTSC.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <i86.h>

/**
 * Reads the 64-bit x86 Time Stamp Counter (RDTSC).
 */
static unsigned __int64 read_rdtsc( void )
{
    unsigned int lo = 0U;
    unsigned int hi = 0U;
    __asm {
        db 0Fh, 31h
        mov lo, eax
        mov hi, edx
    }
    return ( (unsigned __int64)hi << 32U ) | (unsigned __int64)lo;
}

/**
 * Queries DOS/32A kernel configuration via INT 21h AX=0FF8Ah and client
 * low-memory buffer size via INT 21h AX=0FF89h.
 */
static void query_dos32a_telemetry( unsigned int *pmode_type,
                                    unsigned int *cpu_level,
                                    unsigned int *lowbuf_bytes )
{
    union REGS r;

    *pmode_type   = 0xFFU;
    *cpu_level    = 0U;
    *lowbuf_bytes = 0U;

    memset( &r, 0, sizeof( r ) );
    r.w.ax = 0xFF8AU;
    int386( 0x21, &r, &r );
    if( r.x.eax == 0x49443332U || r.x.eax == 0x32334449U ) {
        *cpu_level  = (unsigned int)r.h.cl;
        *pmode_type = (unsigned int)r.h.ch;
    }

    memset( &r, 0, sizeof( r ) );
    r.w.ax = 0xFF89U;
    int386( 0x21, &r, &r );
    if( r.x.eax == 0x49443332U || r.x.eax == 0x32334449U ) {
        *lowbuf_bytes = r.x.ecx;
    }
}

/**
 * Main verification entry point; returns exit code 42 on full pass (or 0 when
 * `--exit0` is passed as first argument).
 */
int main( int argc, char **argv )
{
    const size_t        ext_bytes = 2U * 1024U * 1024U;
    unsigned char       *ext_buf;
    unsigned int        checksum = 0U;
    size_t              i;
    FILE                *fp;
    char                io_buf[16];
    union REGS          r;
    unsigned __int64    t0;
    unsigned __int64    t1;
    unsigned int        pmode_type;
    unsigned int        cpu_level;
    unsigned int        lowbuf_bytes;
    unsigned int        dos_maj;
    unsigned int        dos_min;
    volatile size_t     ptr_sz = sizeof( void * );
    volatile size_t     int_sz = sizeof( int );
    int                 want_exit0 = 0;
    const char          *modes[5] = { "raw-int15h", "xms", "vcpi", "dpmi", "unknown" };

    if( argc > 1 && argv[1] != NULL && strcmp( argv[1], "--exit0" ) == 0 ) {
        want_exit0 = 1;
    }

    if( ptr_sz != 4U || int_sz != 4U ) {
        puts( "SYST32_FAIL: pointer/int size" );
        return 10;
    }

    /* 1. Extended memory allocation & page-strided checksum */
    ext_buf = (unsigned char *)malloc( ext_bytes );
    if( ext_buf == NULL ) {
        puts( "SYST32_FAIL: 2MB malloc NULL" );
        return 11;
    }
    for( i = 0U; i < ext_bytes; i += 4096U ) {
        ext_buf[i] = (unsigned char)( ( i >> 12U ) ^ 0xA5U );
        checksum  += (unsigned int)ext_buf[i];
    }
    free( ext_buf );
    if( checksum == 0U ) {
        puts( "SYST32_FAIL: checksum zero" );
        return 12;
    }

    /* 2. Protected-mode file create, write, seek, read, and unlink */
    fp = fopen( "D32CHK.TMP", "wb" );
    if( fp == NULL ) {
        puts( "SYST32_FAIL: fopen wb" );
        return 13;
    }
    if( fwrite( "D32_IO_OK!", 1U, 10U, fp ) != 10U ) {
        fclose( fp );
        puts( "SYST32_FAIL: fwrite" );
        return 14;
    }
    fclose( fp );

    fp = fopen( "D32CHK.TMP", "rb" );
    if( fp == NULL ) {
        puts( "SYST32_FAIL: fopen rb" );
        return 15;
    }
    memset( io_buf, 0, sizeof( io_buf ) );
    if( fread( io_buf, 1U, 10U, fp ) != 10U || fseek( fp, 4L, SEEK_SET ) != 0 ) {
        fclose( fp );
        puts( "SYST32_FAIL: fread/fseek" );
        return 16;
    }
    fclose( fp );
    remove( "D32CHK.TMP" );
    if( strcmp( io_buf, "D32_IO_OK!" ) != 0 ) {
        puts( "SYST32_FAIL: io mismatch" );
        return 17;
    }

    /* 3. Verify sanitized INT 2Ah and INT 2Fh do not fault on pre-3.0 DOS */
    memset( &r, 0, sizeof( r ) );
    r.w.ax = 0x0000U;
    int386( 0x2A, &r, &r );
    memset( &r, 0, sizeof( r ) );
    r.w.ax = 0x1680U;
    int386( 0x2F, &r, &r );

    /* 4. Measure 100 PM->RM->PM mode-switch roundtrips */
    memset( &r, 0, sizeof( r ) );
    t0 = read_rdtsc();
    for( i = 0U; i < 100U; ++i ) {
        r.h.ah = 0x30U;
        int386( 0x21, &r, &r );
    }
    t1      = read_rdtsc();
    dos_maj = (unsigned int)r.h.al;
    dos_min = (unsigned int)r.h.ah;

    query_dos32a_telemetry( &pmode_type, &cpu_level, &lowbuf_bytes );
    if( pmode_type > 3U ) {
        pmode_type = 4U;
    }

    printf( "SYST32_PASS dos=%u.%02u mode=%s cpu=%u lowbuf=%u argc=%d argv0=%s arg1=%s cycles100=%I64u",
            dos_maj,
            dos_min,
            modes[pmode_type],
            cpu_level,
            lowbuf_bytes,
            argc,
            ( argc > 0 && argv[0] != NULL && argv[0][0] != '\0' ) ? argv[0] : "(empty)",
            ( argc > 1 && argv[1] != NULL ) ? argv[1] : "(none)",
            (unsigned __int64)( t1 - t0 ) );
    putchar( 10 );

    return want_exit0 ? 0 : 42;
}
