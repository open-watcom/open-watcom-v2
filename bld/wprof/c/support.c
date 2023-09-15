/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2023 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Profiler support functions dealing with executable formats.
*
****************************************************************************/


#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "walloca.h"
#include "common.h"
#include "dip.h"
#include "mad.h"
#include "aui.h"
#include "myassert.h"
#include "exedefs.h"
#include "sampinfo.h"
#include "ovltab.h"
#include "msg.h"
#include "memutil.h"
#include "support.h"
#include "wpdata.h"
#include "pathgrp2.h"

#include "clibext.h"


#define BYTE_SIZE       (8)      /* Number of bits in a byte */
#define MAX_INSTR_BYTES (8)
#define BUFF_SIZE       (512)

#define OFFSET_ERROR    ((unsigned long)-1)

#if BUFF_SIZE > 0x7fff
#error buffer size is too large for strict POSIX compliance
#endif

static struct {
    boolbit             end_of_file     : 1;    /* EOF was reached */
    boolbit             end_of_segment  : 1;    /* EOS was reached */
    boolbit             is_32_bit       : 1;    /* 386/486 application */
    boolbit             segment_split   : 1;    /* cached seg isnt continuous */
} exeFlags;

static uint_16          cacheSegment;
static unsigned long    cacheExeOffset;
static uint_32          cacheLo;
static uint_32          cacheHi;
static bool             isHole = false;
static address          currAddr;
static uint_16          segmentShift;
static FILE             *exeFP;
static unsigned long    exeImageOffset;
static image_info       *exeImage;
static EXE_TYPE         exeType;
static uint_16          exeAmount;
static uint_16          exeCurrent;
static unsigned long    exePosition;
static uint_16          nbytes = BUFF_SIZE;
static uint_16          numBytes = MAX_INSTR_BYTES;
static char             exeBuff[BUFF_SIZE];

STATIC void             AdvanceCurrentOffset( uint_32 );
STATIC unsigned long    TransformExeOffset( uint_16, uint_32, uint_16 );

static mad_disasm_data  *MDData;
static unsigned         MDSize;


void AsmSize( void )
{
    unsigned    new;

    new = MADDisasmDataSize();
    if( new > MDSize ) {
        MDData = ProfRealloc( MDData, new );
        //MAD: error check for realloc fail
        MDSize = new;
    }
}

void AsmFini( void )
{
    ProfFree( MDData );
}



FILE *ExeOpen( const char *name )
/*******************************/
{
    return( fopen( name, "rb" ) );
}



void ExeClose( FILE *fp )
/***********************/
{
    fclose( fp );
}



STATIC bool exeSeek( unsigned long fileoffset )
/*********************************************/
{
    if( ( fileoffset >= exePosition ) && ( fileoffset < ( exePosition + exeAmount ) ) ) {
        exeCurrent = fileoffset - exePosition;
        return( true );
    }
    if( fseek( exeFP, fileoffset, SEEK_SET ) ) {
        exeFlags.end_of_file = true;
        return( false );
    }
    exePosition = fileoffset;
    exeAmount = 0;
    exeCurrent = 0;
    exeFlags.end_of_file = false;
    return( true );
}



STATIC void MapSetExeOffset( address a )
/**************************************/
{
    unsigned long   fileoffset;

    fileoffset = TransformExeOffset( a.mach.segment, a.mach.offset, a.sect_id );
    if( fileoffset != OFFSET_ERROR ) {
        exeSeek( fileoffset );
    } else {
        exeFlags.end_of_file = true;
    }
    currAddr = a;
    AdvanceCurrentOffset( 0 );
}

void SetExeOffset( address a )
/****************************/
{
    MapAddressToMap( &a.mach );
    MapSetExeOffset( a );
}


STATIC bool isWATCOM386Windows( FILE *fp )
/****************************************/
{
    uint_32     rex_header_off;
    uint_16     signature;

    if( fseek( fp, REX_HEADER_OFFSET, SEEK_SET ) ) {
        return( false );
    }
    if( fread( &rex_header_off, 1, sizeof( rex_header_off ), fp ) != sizeof( rex_header_off ) ) {
        return( false );
    }
    if( fseek( fp, rex_header_off, SEEK_SET ) ) {
        return( false );
    }
    if( fread( &signature, 1, sizeof( signature ), fp ) != sizeof( signature ) ) {
        return( false );
    }
    if( signature != EXESIGN_REX ) {
        return( false );
    }
    return( true );
}



STATIC int exeHeader( FILE *fp )
/******************************/
{
    dos_exe_header      head;
    uint_32             pharlap_config;
    uint_32             pharlap_header;
    uint_32             ne_header_off;
    uint_16             format_level;
    uint_16             data16;
    uint_16             signature;
    char                copyrite[10];

    exeFlags.is_32_bit = false;
    exeFlags.segment_split = false;
    if( fseek( fp, 0, SEEK_SET ) ) {
        return( EXE_TYPE_NONE );
    }
    if( fread( &head, 1, sizeof( head ), fp ) != sizeof( head ) ) {
        return( EXE_TYPE_NONE );
    }
    switch( head.signature ) {
    case EXESIGN_MP:
    case EXESIGN_REX:
        exeFlags.is_32_bit = true;
        return( EXE_TYPE_MP );
    case EXESIGN_DOS:
        if( isWATCOM386Windows( fp ) ) {
            /* C/386 for Windows bound executable */
            exeFlags.is_32_bit = true;
            return( EXE_TYPE_MP_BOUND );
        }
        if( NE_HEADER_FOLLOWS( head.reloc_offset ) ) {
            if( fseek( fp, NE_HEADER_OFFSET, SEEK_SET ) ) {
                return( EXE_TYPE_MZ );
            }
            if( fread( &ne_header_off, 1, sizeof( ne_header_off ), fp ) != sizeof( ne_header_off ) ) {
                return( EXE_TYPE_MZ );
            }
            if( fseek( fp, ne_header_off, SEEK_SET ) ) {
                return( EXE_TYPE_MZ );
            }
            if( fread( &signature, 1, sizeof( signature ), fp ) != sizeof( signature ) ) {
                return( EXE_TYPE_MZ );
            }
            switch( signature ) {
            case EXESIGN_NE:
                return( EXE_TYPE_OS2 );
            case EXESIGN_PE:
                exeFlags.is_32_bit = true;
                return( EXE_TYPE_PE );
            case EXESIGN_PL:
                exeFlags.is_32_bit = true;
                return( EXE_TYPE_PL );
            case EXESIGN_LE:
                exeFlags.is_32_bit = true;
                return( EXE_TYPE_OS2_FLAT );
            case EXESIGN_LX:
                exeFlags.is_32_bit = true;
                return( EXE_TYPE_OS2_LX );
            }
            return( EXE_TYPE_MZ );
        }
        /* check for bound PharLap Extended 386 executable */
        pharlap_config = head.hdr_size * 16;
        if( fseek( fp, pharlap_config, SEEK_SET ) ) {
            return( EXE_TYPE_MZ );
        }
        if( fread( copyrite, 1, sizeof( copyrite ), fp ) != sizeof( copyrite ) ) {
            return( EXE_TYPE_MZ );
        }
        if( memcmp( copyrite, "Copyright", 9 ) != 0 ) {
            return( EXE_TYPE_MZ );
        }
        if( fseek( fp, pharlap_config + 0x32, SEEK_SET ) ) {
            return( EXE_TYPE_MZ );
        }
        if( fread( &signature, 1, sizeof( signature ), fp ) != sizeof( signature ) ) {
            return( EXE_TYPE_MZ );
        }
        if( signature != EXESIGN_C5 ) {
            return( EXE_TYPE_MZ );
        }
        if( fseek( fp, pharlap_config + 0x32 + 6, SEEK_SET ) ) {
            return( EXE_TYPE_MZ );
        }
        if( fread( &signature, 1, sizeof( signature ), fp ) != sizeof( signature ) ) {
            return( EXE_TYPE_MZ );
        }
        if( signature != EXESIGN_P6 ) {
            return( EXE_TYPE_MZ );
        }
        pharlap_header = head.file_size * 512L - (-head.mod_size & 0x1ff);
        if( fseek( fp, pharlap_header, SEEK_SET ) ) {
            return( EXE_TYPE_MZ );
        }
        if( fread( &signature, 1, sizeof( signature ), fp ) != sizeof( signature ) ) {
            return( EXE_TYPE_MZ );
        }
        if( signature != EXESIGN_P3 ) {
            return( EXE_TYPE_MZ );
        }
        pharlap_header += offsetof( extended_header, format_level );
        if( fseek( fp, pharlap_header, SEEK_SET ) ) {
            return( EXE_TYPE_MZ );
        }
        fread( &format_level, 1, sizeof( format_level ), fp );
        if( format_level != 1 ) {
            return( EXE_TYPE_MZ );
        }
        exeFlags.is_32_bit = true;
        return( EXE_TYPE_P3_BOUND );
    case EXESIGN_P3:
        fseek( fp, offsetof( extended_header, format_level ), SEEK_SET );
        fread( &format_level, 1, sizeof( format_level ), fp );
        if( format_level != 1 ) {
            break;
        }
        exeFlags.is_32_bit = true;
        return( EXE_TYPE_P3 );
    case EXESIGN_P2:
        /* return( EXE_TYPE_P2 );        (not supported 03-may-90 AFS) */
        break;
    case EXESIGN_NE:
        exeFlags.is_32_bit = true;
        return( EXE_TYPE_NW );
    case EXESIGN_ZERO:
        fseek( fp, sizeof( lmf_record ) + offsetof( lmf_header, version ), SEEK_SET );
        data16 = 0;
        fread( &data16, 1, sizeof( data16 ), fp );
        if( data16 >= QNX_VERSION ) {
            exeFlags.segment_split = true;
            fseek( fp, sizeof( lmf_record ) + offsetof( lmf_header, cflags ), SEEK_SET );
            fread( &data16, 1, sizeof( data16 ), fp );
            if( data16 & _TCF_32BIT ) {
                exeFlags.is_32_bit = true;
                if( data16 & _TCF_FLAT ) {
                    return( EXE_TYPE_QNX_386_FLAT );
                }
                return( EXE_TYPE_QNX_386 );
            }
            return( EXE_TYPE_QNX );
        }
        break;
    default:
        if( memcmp( &head, ELF_SIGNATURE, ELF_SIGNATURE_LEN ) == 0 ) {
            exeFlags.is_32_bit = true;
            return( EXE_TYPE_ELF );
        }
        break;
    }
    return( EXE_TYPE_UNKNOWN );
}



void SetExeImage( image_info *image )
/***********************************/
{
    exeImage = image;
}



bool SetExeFile( FILE *fp, bool overlay )
/***************************************/
{
    dos_exe_header      head;
    unsigned long       ne_header_off;
    unsigned long       header_base;
    unsigned long       seek_off;
    uint_16             data16;
    uint_32             data32;

    exeFP = fp;
    exeType = exeHeader( exeFP );
    if( exeType != EXE_TYPE_MZ && overlay ) {        /* 16-may-90 AFS */
        exeType = EXE_TYPE_OVL;
        exeFlags.is_32_bit = false;
    }
    header_base = 0;
    switch( exeType ) {
    case EXE_TYPE_MZ:
        /* exeImageOffset is in 16-byte paragraphs  */
        fseek( exeFP, offsetof( dos_exe_header, hdr_size ), SEEK_SET );
        fread( &data16, 1, sizeof( data16 ), exeFP );
        exeImageOffset = data16;
        break;
    case EXE_TYPE_MP_BOUND:
        fseek( exeFP, REX_HEADER_OFFSET, SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        header_base = data32;
        /* fall through to MP */
    case EXE_TYPE_MP:
        /* exeImageOffset is in bytes */
        seek_off = header_base + offsetof( simple_header, hdr_size );
        fseek( exeFP, seek_off, SEEK_SET );
        fread( &data16, 1, sizeof( data16 ), exeFP );
        exeImageOffset = data16 * 16L;
        exeImageOffset += header_base;
        break;
    case EXE_TYPE_P3_BOUND:
        fseek( exeFP, 0, SEEK_SET );
        fread( &head, 1, sizeof( head ), exeFP );
        header_base = head.file_size * 512L - (-head.mod_size & 0x1ff);
        /* fall through to P3 */
    case EXE_TYPE_P2:
    case EXE_TYPE_P3:
        /* exeImageOffset is in bytes */
        seek_off = header_base + offsetof( extended_header, load_offset );
        fseek( exeFP, seek_off, SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        exeImageOffset = data32;
        seek_off = header_base + offsetof( extended_header, offset );
        fseek( exeFP, seek_off, SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        exeImageOffset -= data32;
        exeImageOffset += header_base;
        break;
    case EXE_TYPE_NW:
        /* exeImageOffset is in bytes */
        fseek( exeFP, offsetof( nlm_header, codeImageOffset ), SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        exeImageOffset = data32;
        break;
    case EXE_TYPE_OS2:
        /* offset into "reserved for future use" portion of the DOS EXE hdr */
        fseek( exeFP, NE_HEADER_OFFSET, SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        ne_header_off = data32;
        fseek( exeFP, ne_header_off + offsetof( os2_exe_header, segment_off ), SEEK_SET );
        fread( &data16, 1, sizeof( data16 ), exeFP );
        /* exeImageOffset is in bytes and points to the segment table */
        exeImageOffset = ne_header_off + data16;
        fseek( exeFP, ne_header_off + offsetof( os2_exe_header, align ), SEEK_SET);
        fread( &data16, 1, sizeof( data16 ), exeFP );
        segmentShift = data16;
        if( segmentShift == 0 ) {
            segmentShift = OS2_DEF_SEGMENT_SHIFT;
        }
        cacheSegment = 0;       /* no 0 segment in OS/2 (03-may-90 AFS) */
        break;
    case EXE_TYPE_OS2_FLAT:
    case EXE_TYPE_OS2_LX:
        fseek( exeFP, NE_HEADER_OFFSET, SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        /* exeImageOffset is in bytes and points to the OS/2 FLAT header */
        exeImageOffset = data32;
        cacheSegment = 0;       /* no 0 segment in OS/2 FLAT */
        break;
    case EXE_TYPE_QNX:
    case EXE_TYPE_QNX_386:
    case EXE_TYPE_QNX_386_FLAT:
        /* exeImageOffset is in bytes and points to the end of the header */
        exeImageOffset = sizeof( lmf_record ) + sizeof( lmf_header );
        cacheSegment = 0;
        break;
    case EXE_TYPE_PE:
    case EXE_TYPE_PL:
        /* offset into "reserved for future use" portion of the DOS EXE hdr */
        fseek( exeFP, NE_HEADER_OFFSET, SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        ne_header_off = data32;
#if 0
        fseek( exeFP, ne_header_off + offsetof( pe_header, num_objects ), SEEK_SET );
        fread( exeFP, &data16, 1, sizeof( data16 ), exeFP );
        fseek( exeFP, ne_header_off + offsetof( pe_header, headers_size ), SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        /* exeImageOffset is in bytes and points to the object table */
        exeImageOffset = data32 - data16 * sizeof( pe_object );
#else
        {
            pe_exe_header   pehdr;

            fseek( exeFP, ne_header_off, SEEK_SET );
            fread( &pehdr, 1, PE_HDR_SIZE, exeFP );
            fread( (char *)&pehdr + PE_HDR_SIZE, 1, PE_OPT_SIZE( pehdr ), exeFP );
            /* exeImageOffset is in bytes and points to the object table */
            exeImageOffset = ne_header_off + PE_SIZE( pehdr );
        }
#endif
        cacheSegment = 0;       /* no 0 segment in OS/2 (03-may-90 AFS) */
        break;
    case EXE_TYPE_OVL:
        /* overlay file (not an exe) */
        exeImageOffset = 0;
        break;
    case EXE_TYPE_ELF:
        fseek( exeFP, offsetof( Elf32_Ehdr, e_phoff ), SEEK_SET );
        fread( &data32, 1, sizeof( data32 ), exeFP );
        exeImageOffset = data32;
        fseek( exeFP, offsetof( Elf32_Ehdr, e_phentsize ), SEEK_SET );
        fread( &data16, 1, sizeof( data16 ), exeFP );
        cacheLo = data16;             /* entry size */
        fread( &data16, 1, sizeof( data16 ), exeFP );
        cacheSegment = data16;        /* number of entries */
        break;
    default:
        return( false );
    }
    exePosition = 0;
    exeCurrent = 0;
    exeAmount = 0;
    currAddr.mach.segment = 0;
    currAddr.mach.offset = 0;
    return( true );
}



STATIC void readEntry( uint_32 *v, size_t mini_off )
/**************************************************/
{
    fseek( exeFP, exeImageOffset + mini_off, SEEK_SET );
    fread( v, 1, sizeof( *v ), exeFP );
}



STATIC void searchQNX( uint_16 seg, uint_32 off )
/***********************************************/
{
    uint_16             seg_idx;
    uint_32             hi;
    uint_32             lo;
    lmf_record          rec_head;
    lmf_data            load_data;
    uint_32             hole_lo;
    uint_32             hole_hi;

    cacheSegment = seg;
    seg_idx = seg >> 3;
    cacheLo = 0;
    cacheHi = _UI32_MAX;
    hole_lo = 0;
    hole_hi = _UI32_MAX;
    isHole = false;
    fseek( exeFP, 0, SEEK_SET );
    for( ;; ) {
        if( fread( &rec_head, 1, sizeof( rec_head ), exeFP ) != sizeof( rec_head ) )
            break;
        nbytes = rec_head.data_nbytes;
        if( rec_head.rec_type == LMF_IMAGE_END_REC )
            break;
        if( rec_head.rec_type != LMF_LOAD_REC ) {
            fseek( exeFP, rec_head.data_nbytes, SEEK_CUR );
            continue;
        }
        if( fread( &load_data, 1, sizeof( load_data ), exeFP ) != sizeof( load_data ) )
            break;
        rec_head.data_nbytes -= sizeof( load_data );
        lo = load_data.offset;
        hi = lo + ( rec_head.data_nbytes - 1 );
        if( load_data.segment == seg_idx ) {
            if( lo <= off && off <= hi ) {
                cacheLo = lo;
                cacheHi = hi;
                cacheExeOffset = ftell( exeFP );
                return;
            }
            if( hole_lo < hi && hi < off ) {
                hole_lo = hi + 1;
            }
            if( hole_hi > lo && lo > off ) {
                hole_hi = lo - 1;
            }
        }
        fseek( exeFP, rec_head.data_nbytes, SEEK_CUR );
    }
    isHole = true;
    cacheLo = hole_lo;
    cacheHi = hole_hi;
    exeFlags.end_of_segment = true;
}



STATIC unsigned long TransformExeOffset( uint_16 seg, uint_32 off, uint_16 sect_id )
/**********************************************************************************/
{
    ovl_entry               *entry;
    segment_record          segment;
    map_entry               mapping;
    unsigned long           tmp_offset;
    image_info              *curr_image;
    unsigned long           subresult;
    uint_32                 page_offset_shift;
    uint_32                 object_table;
    uint_32                 objmap_table;
    uint_32                 map_idx;
    uint_32                 page_num;
    uint_32                 page_off;
    uint_32                 page_size;
    uint_32                 seg_start;

    if( sect_id != ROOT_SECTION ) {
        curr_image = CurrSIOData->curr_image;
        if( sect_id > curr_image->ovl_count ) {
            return( OFFSET_ERROR );
        }
        entry = curr_image->ovl_data;
        entry += sect_id - 1;
        subresult = seg * 16L + off;
        subresult += entry->disk_addr;
        subresult -= entry->start_para * 16L;
        return( subresult );
    }
    switch( exeType ) {
    case EXE_TYPE_NW:
    case EXE_TYPE_P3:
    case EXE_TYPE_P3_BOUND:
    case EXE_TYPE_MP:
    case EXE_TYPE_MP_BOUND:
        subresult = exeImageOffset + off;
        break;
    case EXE_TYPE_OS2:
        if( seg != cacheSegment ) {
            tmp_offset = exeImageOffset + ( seg - 1 ) * sizeof(segment_record);
            fseek( exeFP, tmp_offset, SEEK_SET );
            fread( &segment, 1, sizeof( segment ), exeFP );
            cacheSegment = seg;
            cacheExeOffset = ((unsigned long)segment.address) << segmentShift;
        }
        subresult = cacheExeOffset + off;
        break;
    case EXE_TYPE_OS2_FLAT:
    case EXE_TYPE_OS2_LX:
        if( seg != cacheSegment ) {
            readEntry( &page_offset_shift,
                       offsetof(os2_flat_header,l.page_shift));
            readEntry( &object_table, offsetof(os2_flat_header,objtab_off));
            tmp_offset = object_table + exeImageOffset;
            tmp_offset += (seg - 1) * sizeof( object_record );
            tmp_offset += offsetof( object_record, mapidx );
            fseek( exeFP, tmp_offset, SEEK_SET );
            fread( &map_idx, 1, sizeof( map_idx ), exeFP );
            readEntry( &objmap_table, offsetof(os2_flat_header,objmap_off));
            tmp_offset = objmap_table + exeImageOffset;
            if( exeType == EXE_TYPE_OS2_LX ) {
                tmp_offset += ( map_idx - 1 ) * sizeof( lx_map_entry );
            } else {
                tmp_offset += ( map_idx - 1 ) * sizeof( le_map_entry );
            }
            fseek( exeFP, tmp_offset, SEEK_SET );
            fread( &mapping, 1, sizeof( mapping ), exeFP );
            readEntry( &page_off, offsetof( os2_flat_header, page_off ) );
            if( exeType == EXE_TYPE_OS2_LX ) {
                page_off += mapping.lx.page_offset << page_offset_shift;
            } else {
                page_num = ( (uint_32)mapping.le.page_num[0] << 16 ) +
                           ( mapping.le.page_num[1] << 8 ) +
                           ( mapping.le.page_num[2] << 0 );
                --page_num; /* origin 1 -> origin 0 */
                readEntry( &page_size, offsetof( os2_flat_header, page_size ) );
                page_off += page_num * page_size;
            }
            cacheExeOffset = page_off;
        }
        subresult = cacheExeOffset + off;
        break;
    case EXE_TYPE_QNX:
    case EXE_TYPE_QNX_386:
    case EXE_TYPE_QNX_386_FLAT:
        if( seg != cacheSegment || off < cacheLo || off > cacheHi ) {
            searchQNX( seg, off );
        }
        subresult = cacheExeOffset + ( off - cacheLo );
        break;
    case EXE_TYPE_MZ:
        subresult = ( seg + exeImageOffset ) * 16 + off;
        break;
    case EXE_TYPE_PE:
        if( seg != cacheSegment ) {
            tmp_offset = exeImageOffset;
            tmp_offset += (seg - 1) * sizeof( pe_object );
            tmp_offset += offsetof( pe_object, physical_offset );
            fseek( exeFP, tmp_offset, SEEK_SET );
            fread( &seg_start, 1, sizeof( seg_start ), exeFP );
            cacheExeOffset = seg_start;
        }
        subresult = cacheExeOffset + off;
        break;
    case EXE_TYPE_ELF:
        /*
         * exeImageOffset:      start of the program header table
         * cacheLo:             size of program header entry
         * cacheSegment:        number of program header entries
         */
        {
            Elf32_Phdr  phe;
            unsigned    i;

            tmp_offset = exeImageOffset;
            i = 0;
            for( ;; ) {
                if( i > cacheSegment ) {
                    return( OFFSET_ERROR );
                }
                fseek( exeFP, tmp_offset, SEEK_SET );
                fread( &phe, 1, sizeof( phe ), exeFP );
                if( phe.p_type == PT_LOAD
                 && off >= phe.p_vaddr
                 && off < (phe.p_vaddr+phe.p_memsz) )
                    break;
                tmp_offset += cacheLo;
                ++i;
            }
            cacheHi = phe.p_memsz;
            cacheExeOffset = phe.p_offset;
            subresult = phe.p_offset + (off - phe.p_vaddr);
        }
        break;
    case EXE_TYPE_OVL:
/**/    myassert( 0 );
    case EXE_TYPE_P2:    /* P2 format is not supported 03-may-90 AFS */
    default:
        subresult = OFFSET_ERROR;
        break;
    }
    return( subresult );
}



image_info *AddrImage( address *addr )
/************************************/
{
    addr_ptr        closest;
    image_info      *closest_image;
    image_info      *curr_image;
    map_to_actual   *map;
    ovl_entry       *overlay;
    unsigned        count;
    int             count2;

    closest.segment = 0;
    closest.offset = 0;
    closest_image = NULL;
    count = 0;
    for( ;; ) {
        if( count >= CurrSIOData->image_count ) {
            if( closest.segment != 0 )
                break;
            return( NULL );
        }
        curr_image = CurrSIOData->images[count];
        if( addr->sect_id != 0 ) {
            count2 = curr_image->ovl_count;
            overlay = curr_image->ovl_data;
            while( count2-- > 0 ) {
                if( addr->mach.segment == overlay[count2].base_para ) {
                    return( curr_image );
                }
            }
        } else {
            map = curr_image->map_data;
            for( count2 = 0; count2 < curr_image->map_count; ++count2, ++map ) {
                if( map->actual.mach.segment == addr->mach.segment
                  && addr->mach.offset >= map->actual.mach.offset
                  && (addr->mach.offset-map->actual.mach.offset) < map->length ) {
                    if( map->actual.mach.offset > closest.offset ) {
                        closest = map->actual.mach;
                        closest_image = curr_image;
                    }
                }
            }
        }
        count++;
    }
    return( closest_image );
}



void MapAddressToActual( image_info *curr_image, addr_ptr *addr )
/***************************************************************/
{
    map_to_actual       *map;
    int                 count;
    int                 index;
    addr_seg            seg;

    map = curr_image->map_data;
    count = curr_image->map_count;

    if ( count == 0 ) {
        addr->segment = 0;
        addr->offset  = 0;
        return;
    }

/**/myassert( map != NULL );
    index = 0;
    seg = addr->segment;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        seg = 1;
        break;
    default:
        break;
    }
    for( ;; ) {
        if( count-- <= 0 ) {
            /* address could be EXTERNAL or in an overlay section */
            addr->segment = seg + map[0].actual.mach.segment;
            break;
        }
        if( map[index].map.mach.segment == seg ) {
            addr->segment = map[index].actual.mach.segment;
            addr->offset += map[index].actual.mach.offset - map[index].map.mach.offset;
            break;
        }
        index++;
    }
}


void MapAddressToMap( addr_ptr *addr )
/************************************/
{
    map_to_actual   *map;
    int             count;
    int             index;
    bool            first_save;
    addr_ptr        save_addr;

    save_addr.offset = 0;
    save_addr.segment = 0;
    first_save = true;
    map = CurrSIOData->curr_image->map_data;
    count = CurrSIOData->curr_image->map_count;
    for( index = 0; index < count; ++index, ++map ) {
        //NYI: should check map->length
        if( map->actual.mach.segment == addr->segment ) {
            if( first_save
             || (addr->offset >= map->actual.mach.offset
                 && addr->offset >= save_addr.offset) ) {
                save_addr.segment = map->map.mach.segment;
                save_addr.offset = map->actual.mach.offset - map->map.mach.offset;
                first_save = false;
            }
        }
    }
    if( first_save ) {
        /* address could be EXTERNAL or in an overlay section */
        addr->segment -= CurrSIOData->curr_image->map_data[0].actual.mach.segment;
    } else {
        addr->segment = save_addr.segment;
        addr->offset -= save_addr.offset;
    }
}



void MapAddressIntoSection( address *addr )
/*****************************************/
{
    image_info          *curr_image;
    ovl_entry           *overlay;
    unsigned            count;

    if( addr == 0 ) {
        return;
    }
    addr->sect_id = 0;
    count = 0;
    for(;;) {
        if( count >= CurrSIOData->image_count ) {
            return;
        }
        curr_image = CurrSIOData->curr_image;
        if( curr_image->ovl_count != 0 ) break;
        count++;
    }
    overlay = curr_image->ovl_data;
    for( count = 0; count < curr_image->ovl_count; ++count ) {
        if( addr->mach.segment == overlay[count].base_para ) {
            addr->sect_id = count + 1;
            break;
        }
    }
}



STATIC void ResolveOverlays( image_info *image )
/**********************************************/
{
    int                 ovl_count;
    int                 count;
    int                 index;
    int                 count2;
    int                 index2;
    int                 count3;
    clicks_t            curr_tick;
    clicks_t            map_tick;
    ovl_entry           *overlay;
    remap_data          *remap;
    addr_seg            *remap_segment;
    thread_data         *thd;
    address             *samp_data;
    addr_seg            base_segment;
    addr_seg            actual_segment;

    base_segment = image->map_data[0].actual.mach.segment;
    overlay = image->ovl_data;
    ovl_count = image->ovl_count;
    remap_segment = ProfCAlloc( ovl_count * sizeof(addr_seg) );
    remap = CurrSIOData->remaps->next;
    map_tick = remap->tick;
    /* If we've got overlays, there's only one thread */
    thd = CurrSIOData->samples;
    curr_tick = thd->start_time;
    count = RAW_BUCKET_IDX( thd->end_time - thd->start_time ) + 1;
    for( index = 0; index < count; ++index ) {
        samp_data = thd->raw_bucket[index];
        count2 = MAX_RAW_BUCKET_INDEX;
        if( (int)(thd->end_time - curr_tick) < count2 ) {
            count2 = thd->end_time - curr_tick;
        }
        for( index2 = 0; index2 < count2; ++index2 ) {
            while( curr_tick == map_tick ) {
                remap_segment[remap->section - 1] = remap->segment;
                if( remap == CurrSIOData->remaps ) {
                    map_tick = CurrSIOData->total_samples;
                } else {
                    remap = remap->next;
                    map_tick = remap->tick;
                }
            }
            actual_segment = samp_data[index2].mach.segment;
            if( actual_segment != base_segment ) {
                for( count3 = 0; count3 < ovl_count; ++count3 ) {
                    if( actual_segment == remap_segment[count3] ) {
                        samp_data[index2].mach.segment = overlay[count3].base_para;
                        samp_data[index2].sect_id = count3 + 1;
                        break;
                    }
                }
            }
            curr_tick++;
        }
    }
    ProfFree( remap_segment );
}



STATIC bool LoadOverlayInfo( void )
/*********************************/
{
    image_info              *image;
    FILE                    *fp;
    address                 map_addr;
    addr_seg                base_segment;
    unsigned long           fileoffset;
    unsigned long           filenameoffset;
    ovltab_entry            formal_entry;
    ovltab_prolog           prolog;
    ovl_entry               *entry;
    ovl_entry               *entry_ptr;
    char                    *ovl_name;
    char                    buffer1[_MAX_PATH];
    char                    buffer2[_MAX_PATH2];
    int                     count;
    size_t                  len;
    pgroup2                 pg1;
    pgroup2                 pg2;
    struct stat             file_stat;

    image = CurrSIOData->curr_image;
    fp = ExeOpen( image->name );
    if( fp == NULL ) {
        ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
        return( false );
    }
    SetExeFile( fp, false );
    map_addr = image->overlay_table;
    MapAddressToMap( &map_addr.mach );
    fileoffset = TransformExeOffset( map_addr.mach.segment, map_addr.mach.offset, ROOT_SECTION );
    if( fseek( fp, fileoffset, SEEK_SET ) ) {
        ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
        return( false );
    }
    if( fread( &prolog, 1, sizeof( prolog ), fp ) != sizeof( prolog ) ) {
        ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
        return( false );
    }
    if( prolog.major != OVL_MAJOR_VERSION
     || prolog.minor > OVL_MINOR_VERSION ) {
        ErrorMsg( LIT( Incompat_Ver_Ovly ), image->name );
        return( false );
    }
    base_segment = image->map_data[0].actual.mach.segment;
    count = 0;
    entry_ptr = NULL;
    for(;;) {
        if( fread( &formal_entry, 1, sizeof( formal_entry ), fp ) != sizeof( formal_entry ) ) {
            ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
            return( false );
        }
        if( formal_entry.flags_anc == OVLTAB_TERMINATOR )
            break;
        ++count;
        if( entry_ptr == NULL ) {
            entry_ptr = ProfAlloc( count * sizeof(ovl_entry) );
        } else {
            entry_ptr = ProfRealloc( entry_ptr, count * sizeof(ovl_entry) );
        }
        entry = entry_ptr + count - 1;
        /* make sure we are properly initialized in case of error */
        image->ovl_count = count;
        image->ovl_data = entry_ptr;
        entry->fname = NULL;
        entry->start_para = formal_entry.start_para;
        /* add 1 to the segment to account for both */
        /* segments being base zero values */
        entry->base_para = entry->start_para + base_segment + 1;
        entry->disk_addr  = formal_entry.disk_addr;
        if( formal_entry.fname & OVE_EXE_FILENAME ) {
            ovl_name = image->name;
            entry->separate_overlay = false;
        } else {
            fileoffset = ftell( fp );  /* save current posn */
            map_addr = image->overlay_table;
            map_addr.mach.offset += formal_entry.fname;
            MapAddressToMap( &map_addr.mach );
            filenameoffset = TransformExeOffset( map_addr.mach.segment,
                                                 map_addr.mach.offset,
                                                 ROOT_SECTION );
            if( fseek( fp, filenameoffset, SEEK_SET ) ) {
                ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
                return( false );
            }
            if( fread( buffer1, 1, _MAX_PATH, fp ) != _MAX_PATH ) {
                ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
                return( false );
            }
            /* find overlay file */
            if( stat( buffer1, &file_stat ) == -1 ) {
                strcpy( buffer2, buffer1 );
                _searchenv( buffer2, "PATH", buffer1 );
                if( buffer1[0] == '\0' ) {
                    _splitpath2( image->name, pg1.buffer, &pg1.drive, &pg1.dir, NULL, NULL );
                    _splitpath2( buffer2, pg2.buffer, NULL, NULL, &pg2.fname, &pg2.ext );
                    _makepath( buffer1, pg1.drive, pg1.dir, pg2.fname, pg2.ext );
                }
            }
            ovl_name = buffer1;
            entry->separate_overlay = true;
            /* restore current posn */
            if( fseek( fp, fileoffset, SEEK_SET ) ) {
                ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
                return( false );
            }
        }
        len = strlen( ovl_name ) + 1;
        entry->fname = ProfAlloc( len );
        memcpy( entry->fname, ovl_name, len );
    }
    ExeClose( fp );
    return( true );
}



bool LoadImageOverlays( void )
/****************************/
{
    image_info      *curr_image;
    unsigned        image_index;

    /* assume 32-bit addresses until proven otherwise */
    exeFlags.is_32_bit = true;

    for( image_index = 0; image_index < CurrSIOData->image_count; ++image_index ) {
        curr_image = CurrSIOData->curr_image;
        if( curr_image->overlay_table.mach.segment != 0 ) {
            CurrSIOData->curr_image = curr_image;
            if( !LoadOverlayInfo() ) {
                return( false );
            }
            if( CurrSIOData->remaps != NULL ) {
                ResolveOverlays( curr_image );
            }
        }
    }
    return( true );
}



STATIC void AdvanceCurrentOffset( uint_32 advance )
/*************************************************/
{
    addr_off     old_offset;

    exeFlags.end_of_segment = false;
    old_offset = currAddr.mach.offset;
    MADAddrAdd( &currAddr, advance, MAF_OFFSET );
    if( currAddr.mach.offset < old_offset ) {
        exeFlags.end_of_segment = true;
        return;
    }
    if( exeFlags.segment_split ) {
        if( currAddr.mach.offset > cacheHi ) {
            MapSetExeOffset( currAddr );
        }
    }
}



STATIC void exeRewind( unsigned long offset )
/*******************************************/
{
    if( offset > exeCurrent ) {
        exeSeek( ( exePosition + exeCurrent ) - offset );
    } else {
        exeCurrent -= offset;
    }
}



void SetNumBytes( uint_16 num )
/*****************************/
{
    numBytes = num;
}



void CodeAdvance( address *addr )
/*******************************/
{
    MADDisasm( MDData, addr, 0 );
    SetExeOffset( *addr );
}



STATIC unsigned char exeGetChar( void )
/*************************************/
{
    uint_16     size;

    if( isHole ) {
        return( 0 );
    }
    if( exeCurrent >= exeAmount ) {
        exePosition += exeAmount;
        fseek( exeFP, exePosition, SEEK_SET );
        size = BUFF_SIZE;
        if( size > nbytes )
            size = nbytes;
        exeAmount = fread( exeBuff, 1, size, exeFP );
        if( exeAmount == 0 ) {
            exeFlags.end_of_file = true;
            return( 0 );
        }
        exeCurrent = 0;
    }
    return( exeBuff[exeCurrent++] );
}

size_t FormatAddr( address a, char *buffer, size_t max )
{
    mad_type_info       host_mti;
    mad_type_info       mti;
    unsigned_8          item[16];

    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host_mti );
    MADTypeInfo( MADTypeDefault( MTK_ADDRESS, MAF_FULL, NULL, &a ), &mti );
    MADTypeConvert( &host_mti, &a, &mti, item, 0 );
    MADTypeToString( 16, &mti, item, buffer, &max );
    return( max );
}

void GetFullInstruct( address a, char * buffer, size_t max )
/**********************************************************/
{
    unsigned            i;
    char *              tail;
    address             start;
    unsigned            ins_size;
    mad_type_info       host_mti;
    unsigned_8          item;
    size_t              mad_max = max;

    start = a;
    MADDisasm( MDData, &a, 0 );

    tail = &buffer[FormatAddr( start, buffer, max )];
    *tail++ = ' ';              /* two spaces */
    *tail++ = ' ';

    if( numBytes > 0 ) {
        SetExeOffset( start );
        ins_size = MADDisasmInsSize( MDData );
        for( i = 0; i < ins_size || i < numBytes; i++ ) {
            if( i < ins_size ) {
                item = exeGetChar();
                MADTypeInfoForHost( MTK_INTEGER, sizeof( item ), &host_mti );
                mad_max = 4;
                MADTypeToString( 16, &host_mti, &item, tail, &mad_max );
                tail += mad_max;
            } else if( i < numBytes ) {
                *tail++ = ' ';          /* two spaces */
                *tail++ = ' ';
            }
            *tail++ = ' ';
        }
        *tail++ = ' ';          /* two spaces */
        *tail++ = ' ';
        exeRewind( ins_size );
    }
    MADDisasmFormat( MDData, MDP_ALL, 16, tail, max - ( tail - buffer ) );
}



/*
 * Convert address to a symbol string.  Return false if this could not be done.
 */

bool CnvAddr( address addr, char *buff, size_t buff_len )
{
    sym_handle *    sym;
    search_result   sr;
    size_t          name_len;

    sym = alloca( DIPHandleSize( HK_SYM ) );
//    MapAddressToActual( exeImage, &addr.mach );
    sr = DIPAddrSym( NO_MOD, addr, sym );
    switch( sr ) {
    case SR_EXACT:
    //case SR_CLOSEST: /* add in if we have symbol+offset */
        break;
    default:
        return( false );
    }
    name_len = DIPSymName( sym, NULL, SNT_DEMANGLED, buff, buff_len );
    if( name_len == 0 ) {
        DIPSymName( sym, NULL, SNT_SOURCE, buff, buff_len );
    }
    return( true );
}


bool IsX86BigAddr( address a )
{
    /* unused parameters */ (void)a;

    return( exeFlags.is_32_bit );
}

bool IsX86RealAddr( address a )
{
    /* unused parameters */ (void)a;

    switch( exeType ) {
    case EXE_TYPE_MZ:
    case EXE_TYPE_OVL:
        return( true );
    }
    return( false );
}


int_8 GetDataByte( void )
/***********************/
{
    int_8   value;

    value = exeGetChar();
    AdvanceCurrentOffset( sizeof( int_8 ) );
    return( value );
}


bool EndOfSegment( void )
/***********************/
{
    if( exeFlags.end_of_segment ) {
        return( true );
    }
    if( exeFlags.end_of_file ) {
        return( true );
    }
    return( false );
}
