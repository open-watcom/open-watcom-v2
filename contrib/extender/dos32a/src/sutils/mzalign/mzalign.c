/*
 * mzalign.c - Deterministic MZ Header 512-Byte (32-Paragraph) Aligner
 *
 * Aligns DOS MZ executables and bindable DOS/32A stubs to e_cparhdr = 32
 * paragraphs (512 bytes) with e_lfarlc = 0x0040 so that:
 *   1. Microsoft MS-DOS 1.25 COMMAND.COM EXELOAD loads the executable
 *      natively with zero post-hoc binary patching.
 *   2. Open Watcom wlink (`option stub=dos32a.stb`) and SUNSYS Bind (`sb.exe`)
 *      preserve e_cparhdr = 32 paragraphs (via 112 relocation entries when
 *      scratch relocation padding is enabled) when binding 32-bit LE payloads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Target MZ header size in 16-byte paragraphs (32 * 16 = 512 bytes). */
#define TARGET_HDR_PARAS 32U

/** Target MZ header size in bytes. */
#define TARGET_HDR_BYTES (TARGET_HDR_PARAS * 16U)

/** Standard extended-executable relocation table offset (0x0040). */
#define TARGET_RELOC_OFF 0x0040U

/** Number of 4-byte relocations spanning 0x0040..0x0200 ((512 - 64) / 4 = 112). */
#define STUB_PAD_RELOCS  112U

/**
 * Reads a little-endian 16-bit unsigned integer from `buf` at byte offset `off`.
 */
static unsigned int read_u16_le( const unsigned char *buf, size_t off )
{
    return (unsigned int)buf[off] | ( (unsigned int)buf[off + 1U] << 8U );
}

/**
 * Writes a little-endian 16-bit unsigned integer `val` to `buf` at byte offset `off`.
 */
static void write_u16_le( unsigned char *buf, size_t off, unsigned int val )
{
    buf[off]      = (unsigned char)( val & 0xFFU );
    buf[off + 1U] = (unsigned char)( ( val >> 8U ) & 0xFFU );
}

/**
 * Reads a little-endian 32-bit unsigned integer from `buf` at byte offset `off`.
 */
static unsigned long read_u32_le( const unsigned char *buf, size_t off )
{
    return (unsigned long)buf[off]
         | ( (unsigned long)buf[off + 1U] << 8U )
         | ( (unsigned long)buf[off + 2U] << 16U )
         | ( (unsigned long)buf[off + 3U] << 24U );
}

/**
 * Writes a little-endian 32-bit unsigned integer `val` to `buf` at byte offset `off`.
 */
static void write_u32_le( unsigned char *buf, size_t off, unsigned long val )
{
    buf[off]      = (unsigned char)( val & 0xFFUL );
    buf[off + 1U] = (unsigned char)( ( val >> 8U ) & 0xFFUL );
    buf[off + 2U] = (unsigned char)( ( val >> 16U ) & 0xFFUL );
    buf[off + 3U] = (unsigned char)( ( val >> 24U ) & 0xFFUL );
}

/**
 * Entry point: `mzalign <input.exe> [output.exe] [--pad-relocs <off_hex>]`
 *
 * When `--pad-relocs <off_hex>` is supplied (e.g. `0x007E` for `_ID32:007Eh`),
 * unused relocation slots up to 112 entries are populated with `0000:<off_hex>`
 * so `wlink option stub=...` and `sb.exe` emit `e_cparhdr = 32` automatically.
 */
int main( int argc, char **argv )
{
    const char      *in_path;
    const char      *out_path;
    int             pad_relocs;
    unsigned int    pad_off;
    int             arg_idx;
    FILE            *fp;
    long            fsize;
    size_t          in_len;
    unsigned char   *in_buf;
    unsigned char   *out_buf;
    unsigned int    e_cblp;
    unsigned int    e_cp;
    unsigned int    e_crlc;
    unsigned int    e_cparhdr;
    unsigned int    e_lfarlc;
    unsigned long   e_lfanew;
    size_t          old_hdr_bytes;
    size_t          mz_img_end;
    size_t          code_len;
    size_t          tail_len;
    size_t          new_mz_len;
    size_t          new_total_len;
    unsigned int    new_crlc;
    unsigned int    i;

    if( argc < 2 ) {
        fputs( "Usage: mzalign <input.exe> [output.exe] [--pad-relocs <hex_off>]\012", stderr );
        return 1;
    }

    in_path    = argv[1];
    out_path   = in_path;
    pad_relocs = 0;
    pad_off    = 0x007EU;
    arg_idx    = 2;

    while( arg_idx < argc ) {
        if( strcmp( argv[arg_idx], "--pad-relocs" ) == 0 ) {
            if( arg_idx + 1 >= argc ) {
                fputs( "mzalign: --pad-relocs requires an offset argument\012", stderr );
                return 1;
            }
            pad_relocs = 1;
            pad_off    = (unsigned int)strtoul( argv[arg_idx + 1], NULL, 0 );
            arg_idx   += 2;
        } else {
            out_path = argv[arg_idx];
            arg_idx += 1;
        }
    }

    fp = fopen( in_path, "rb" );
    if( fp == NULL ) {
        fprintf( stderr, "mzalign: cannot open input '%s'\012", in_path );
        return 1;
    }
    if( fseek( fp, 0L, SEEK_END ) != 0 ) {
        fclose( fp );
        return 1;
    }
    fsize = ftell( fp );
    if( fsize < 0x40L || fseek( fp, 0L, SEEK_SET ) != 0 ) {
        fclose( fp );
        fprintf( stderr, "mzalign: file '%s' too small for MZ header\012", in_path );
        return 1;
    }
    in_len = (size_t)fsize;
    in_buf = (unsigned char *)malloc( in_len );
    if( in_buf == NULL ) {
        fclose( fp );
        return 1;
    }
    if( fread( in_buf, 1U, in_len, fp ) != in_len ) {
        free( in_buf );
        fclose( fp );
        return 1;
    }
    fclose( fp );

    if( !( ( in_buf[0] == 'M' && in_buf[1] == 'Z' ) || ( in_buf[0] == 'Z' && in_buf[1] == 'M' ) ) ) {
        free( in_buf );
        fprintf( stderr, "mzalign: '%s' is not an MZ executable\012", in_path );
        return 1;
    }

    e_cblp        = read_u16_le( in_buf, 0x02U );
    e_cp          = read_u16_le( in_buf, 0x04U );
    e_crlc        = read_u16_le( in_buf, 0x06U );
    e_cparhdr     = read_u16_le( in_buf, 0x08U );
    e_lfarlc      = read_u16_le( in_buf, 0x18U );
    old_hdr_bytes = (size_t)e_cparhdr * 16U;

    if( old_hdr_bytes > in_len || (size_t)e_lfarlc + (size_t)e_crlc * 4U > old_hdr_bytes ) {
        free( in_buf );
        fprintf( stderr, "mzalign: corrupt MZ header in '%s'\012", in_path );
        return 1;
    }
    if( e_crlc > STUB_PAD_RELOCS ) {
        /* Already >= 512-byte header; nothing to pad if already 32-para aligned */
        if( ( e_cparhdr % TARGET_HDR_PARAS ) == 0U ) {
            free( in_buf );
            return 0;
        }
        free( in_buf );
        fprintf( stderr, "mzalign: '%s' has %u relocs (> %u)\012", in_path, e_crlc, STUB_PAD_RELOCS );
        return 1;
    }

    e_lfanew = ( e_lfarlc >= 0x40U ) ? read_u32_le( in_buf, 0x3CU ) : 0UL;

    mz_img_end = ( e_cp == 0U )
               ? in_len
               : ( e_cblp == 0U ? (size_t)e_cp * 512U : ( (size_t)( e_cp - 1U ) * 512U + (size_t)e_cblp ) );
    if( mz_img_end > in_len || mz_img_end < old_hdr_bytes ) {
        mz_img_end = in_len;
    }

    code_len      = mz_img_end - old_hdr_bytes;
    tail_len      = in_len - mz_img_end;
    new_mz_len    = TARGET_HDR_BYTES + code_len;
    new_total_len = new_mz_len + tail_len;

    out_buf = (unsigned char *)calloc( 1U, new_total_len );
    if( out_buf == NULL ) {
        free( in_buf );
        return 1;
    }

    /* Copy base 0x1C-byte MZ header (or up to 0x40 if e_lfarlc >= 0x40) */
    memcpy( out_buf, in_buf, ( e_lfarlc >= 0x40U ) ? 0x40U : 0x1CU );

    /* Copy existing relocation entries to offset 0x0040 */
    if( e_crlc > 0U ) {
        memcpy( out_buf + TARGET_RELOC_OFF, in_buf + e_lfarlc, (size_t)e_crlc * 4U );
    }

    new_crlc = e_crlc;
    if( pad_relocs ) {
        for( i = e_crlc; i < STUB_PAD_RELOCS; ++i ) {
            write_u16_le( out_buf, TARGET_RELOC_OFF + (size_t)i * 4U, pad_off );
            write_u16_le( out_buf, TARGET_RELOC_OFF + (size_t)i * 4U + 2U, 0x0000U );
        }
        new_crlc = STUB_PAD_RELOCS;
    }

    /* Copy load module and any trailing bound extended payload */
    memcpy( out_buf + TARGET_HDR_BYTES, in_buf + old_hdr_bytes, code_len + tail_len );

    /* Update MZ header fields */
    write_u16_le( out_buf, 0x02U, (unsigned int)( new_mz_len % 512U ) );
    write_u16_le( out_buf, 0x04U, (unsigned int)( ( new_mz_len + 511U ) / 512U ) );
    write_u16_le( out_buf, 0x06U, new_crlc );
    write_u16_le( out_buf, 0x08U, TARGET_HDR_PARAS );
    write_u16_le( out_buf, 0x18U, TARGET_RELOC_OFF );
    if( e_lfanew != 0UL && e_lfanew >= old_hdr_bytes ) {
        write_u32_le( out_buf, 0x3CU, e_lfanew + (unsigned long)( TARGET_HDR_BYTES - old_hdr_bytes ) );
    }

    fp = fopen( out_path, "wb" );
    if( fp == NULL ) {
        free( out_buf );
        free( in_buf );
        fprintf( stderr, "mzalign: cannot open output '%s'\012", out_path );
        return 1;
    }
    if( fwrite( out_buf, 1U, new_total_len, fp ) != new_total_len ) {
        fclose( fp );
        free( out_buf );
        free( in_buf );
        return 1;
    }
    fclose( fp );
    free( out_buf );
    free( in_buf );
    return 0;
}
