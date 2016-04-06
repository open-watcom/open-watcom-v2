/****************************************************************************
*
*                            Open Watcom Project
*
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
#include "walloca.h"
#include "wio.h"
#include "common.h"
#include "dip.h"
#include "mad.h"
#include "myassert.h"
#include "exedefs.h"
#include "machtype.h"
#include "sampinfo.h"
#include "ovltab.h"
#include "msg.h"
#include "memutil.h"
#include "clibext.h"
#include "support.h"


#define BYTE_SIZE       (8)      /* Number of bits in a byte */
#define MAX_INSTR_BYTES (8)
#define BUFF_SIZE       (512)

#if BUFF_SIZE > 0x7fff
#error buffer size is too large for strict POSIX compliance
#endif

static struct {
    uint_8              end_of_file : 1;        /* EOF was reached */
    uint_8              end_of_segment : 1;     /* EOS was reached */
    uint_8              is_32_bit : 1;          /* 386/486 application */
    uint_8              segment_split : 1;      /* cached seg isnt continuous */
} exeFlags;

static uint_16          cacheSegment;
static off_t            cacheExeOffset;
static uint_32          cacheLo;
static uint_32          cacheHi;
static bool             isHole = false;
static address          currAddr;
static uint_16          segmentShift;
static file_handle      exeFH;
static off_t            exeImageOffset;
static image_info       *exeImage;
static EXE_TYPE         exeType;
static uint_16          exeAmount;
static uint_16          exeCurrent;
static off_t            exePosition;
static unsigned_16      nbytes = BUFF_SIZE;
static uint_16          numBytes = MAX_INSTR_BYTES;
static char             exeBuff[BUFF_SIZE];


STATIC void             AdvanceCurrentOffset( uint_32 );
STATIC uint_32          TransformExeOffset( uint_16, uint_32, uint_16 );
extern void             MapAddressToMap( addr_ptr *addr );

extern sio_data         *CurrSIOData;


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



file_handle ExeOpen( char * name )
/********************************/
{
    file_handle     fh;

    fh = open( name, O_RDONLY | O_BINARY );
//    if( fh == -1 ) {
//        if( SourceRelease() != 0 ) {
//            fh = open( name, O_RDONLY | O_BINARY, S_IREAD );
//        }
//    }
    return( fh );
}



void ExeClose( file_handle fh )
/*****************************/
{
    close( fh );
}



STATIC bool exeSeek( off_t posn )
/*******************************/
{
    if(( posn >= exePosition )&&( posn < ( exePosition + exeAmount ))) {
        exeCurrent = posn - exePosition;
        return( true );
    }
    if( lseek( exeFH, posn, SEEK_SET ) == posn ) {
        exePosition = posn;
        exeAmount = 0;
        exeCurrent = 0;
        exeFlags.end_of_file = false;
        return( true );
    }
    exeFlags.end_of_file = true;
    return( false );
}



STATIC void MapSetExeOffset( address a )
/**************************************/
{
    off_t   pos;

    pos = TransformExeOffset( a.mach.segment, a.mach.offset, a.sect_id );
    if( pos != -1 ) {
        exeSeek( pos );
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


STATIC bool isWATCOM386Windows( file_handle fh )
/**********************************************/
{
    uint_32     MQ_offset;
    char        sig[2];

    if( lseek( fh, 0x38, SEEK_SET ) < 0 ) {
        return( false );
    }
    if( read( fh, &MQ_offset, sizeof( MQ_offset ) ) != sizeof( MQ_offset ) ) {
        return( false );
    }
    if( lseek( fh, MQ_offset, SEEK_SET ) < 0 ) {
        return( false );
    }
    if( read( fh, &sig, sizeof( sig ) ) != sizeof( sig ) ) {
        return( false );
    }
    if( sig[0] != 'M' || sig[1] != 'Q' ) {
        return( false );
    }
    return( true );
}



STATIC int exeHeader( file_handle fh )
/************************************/
{
    dos_exe_header      head;
    uint_32             pharlap_config;
    uint_32             pharlap_header;
    uint_32             os2_header;
    uint_16             format_level;
    uint_16             dummy_16;
    char                signature[2];
    char                copyrite[10];
    char                *sig;

    exeFlags.is_32_bit = false;
    exeFlags.segment_split = false;
    if( lseek( fh, 0, SEEK_SET ) != 0 ) {
        return( EXE_NOTYPE );
    }
    if( read( fh, &head, sizeof( head ) ) != sizeof( head ) ) {
        return( EXE_NOTYPE );
    }
    sig = (char *) &head.signature;
    switch( sig[0] ) {
    case 'M':
        if( sig[1] == 'P' || sig[1] == 'Q' ) {          /* MP & MQ */
            exeFlags.is_32_bit = true;
            return( EXE_MP );
        }
        if( sig[1] == 'Z' ) {                           /* MZ */
            if( isWATCOM386Windows( fh ) ) {
                /* C/386 for Windows bound executable */
                exeFlags.is_32_bit = true;
                return( EXE_MP_BOUND );
            }
            if( head.reloc_offset >= 0x40 ) {
                if( lseek( fh, sizeof( head ) + 32, SEEK_SET ) < 0 ) {
                    return( EXE_MZ );
                }
                if(read(fh,&os2_header,sizeof(os2_header))!=sizeof(os2_header)){
                    return( EXE_MZ );
                }
                if( lseek( fh, os2_header, SEEK_SET ) < 0 ) {
                    return( EXE_MZ );
                }
                if( read(fh,signature,sizeof(signature)) != sizeof(signature)) {
                    return( EXE_MZ );
                }
                if( signature[0] == 'N' && signature[1] == 'E' ) {  /* NE */
                    return( EXE_OS2 );
                }
                if( signature[0] == 'P' && signature[1] == 'E' ) {  /* PE */
                    exeFlags.is_32_bit = true;
                    return( EXE_PE );
                }
                if( signature[0] == 'P' && signature[1] == 'L' ) {  /* PL */
                    exeFlags.is_32_bit = true;
                    return( EXE_PL );
                }
                if( signature[0] == 'L' ) {
                    if( signature[1] == 'E' ) {         /* LE */
                        exeFlags.is_32_bit = true;
                        return( EXE_OS2_FLAT );
                    }
                    if( signature[1] == 'X' ) {         /* LX */
                        exeFlags.is_32_bit = true;
                        return( EXE_OS2_LX );
                    }
                }
                return( EXE_MZ );
            }
            /* check for bound PharLap Extended 386 executable */
            pharlap_config = head.hdr_size * 16;
            if( lseek( fh, pharlap_config, SEEK_SET ) < 0 ) {
                return( EXE_MZ );
            }
            if( read( fh, copyrite, sizeof(copyrite) ) != sizeof(copyrite)){
                return( EXE_MZ );
            }
            if( memcmp( copyrite, "Copyright", 9 ) != 0 ) {
                return( EXE_MZ );
            }
            if( lseek( fh, pharlap_config + 0x32, SEEK_SET ) < 0 ) {
                return( EXE_MZ );
            }
            if( read(fh,signature,sizeof(signature)) != sizeof(signature)) {
                return( EXE_MZ );
            }
            if( signature[0] != 'C' || signature[1] != '5' ) {
                return( EXE_MZ );
            }
            if( lseek( fh, pharlap_config + 0x32 + 6, SEEK_SET ) < 0 ) {
                return( EXE_MZ );
            }
            if( read(fh,signature,sizeof(signature)) != sizeof(signature)) {
                return( EXE_MZ );
            }
            if( signature[0] != 'P' || signature[1] != '6' ) {
                return( EXE_MZ );
            }
            pharlap_header = head.file_size * 512L;
            if( head.mod_size != 0 ) {
                pharlap_header += head.mod_size;
                pharlap_header -= 512;
            }
            if( lseek( fh, pharlap_header, SEEK_SET ) < 0 ) {
                return( EXE_MZ );
            }
            if( read(fh,signature,sizeof(signature)) != sizeof(signature)) {
                return( EXE_MZ );
            }
            if( signature[0] != 'P' || signature[1] != '3' ) {
                return( EXE_MZ );
            }
            pharlap_header += offsetof( extended_header, format_level );
            if( lseek( fh, pharlap_header, SEEK_SET ) < 0 ) {
                return( EXE_MZ );
            }
            read( fh, &format_level, sizeof( format_level ) );
            if( format_level != 1 ) {
                return( EXE_MZ );
            }
            exeFlags.is_32_bit = true;
            return( EXE_P3_BOUND );
        }
        break;
    case 'P':
        if( sig[1] == '3' ) {                           /* P3 */
            lseek( fh, offsetof( extended_header, format_level ), SEEK_SET );
            read( fh, &format_level, sizeof( format_level ) );
            if( format_level != 1 ) {
                return( EXE_UNKNOWN );
            }
            exeFlags.is_32_bit = true;
            return( EXE_P3 );
        }
        if( sig[1] == '2' ) {                           /* P2 */
            return( EXE_UNKNOWN );
            /* return( EXE_P2 );        (not supported 03-may-90 AFS) */
        }
        break;
    case 'N':
        if( sig[1] == 'e' ) {                           /* Ne */
            exeFlags.is_32_bit = true;
            return( EXE_NW );
        }
        break;
    case '\0':
        if( sig[1] == '\0' ) {
            lseek( fh, sizeof( lmf_record ) + offsetof( lmf_header, version ),
                   SEEK_SET );
            dummy_16 = 0;
            read( fh, &dummy_16, sizeof( dummy_16 ) );
            if( dummy_16 >= QNX_VERSION ) {
                exeFlags.segment_split = true;
                lseek( fh, sizeof( lmf_record ) + offsetof( lmf_header, cflags ),
                       SEEK_SET );
                read( fh, &dummy_16, sizeof( dummy_16 ) );
                if( dummy_16 & _TCF_32BIT ) {
                    exeFlags.is_32_bit = true;
                    if( dummy_16 & _TCF_FLAT ) {
                        return( EXE_QNX_386_FLAT );
                    }
                    return( EXE_QNX_386 );
                }
                return( EXE_QNX );
            }
        }
        break;
    case ELFMAG0:
        if( memcmp( sig, ELF_SIGNATURE, ELF_SIGNATURE_LEN ) == 0 ) {
            exeFlags.is_32_bit = true;
            return( EXE_ELF );
        }
        break;

    }
    return( EXE_UNKNOWN );
}



void SetExeImage( image_info *image )
/***********************************/
{
    exeImage = image;
}



bool SetExeFile( file_handle fh, bool overlay )
/*********************************************/
{
    dos_exe_header      head;
    off_t               new_header;
    off_t               header_base;
    off_t               seek_off;
    uint_16             dummy_16;
    uint_32             dummy_32;

    exeFH = fh;
    exeType = exeHeader( exeFH );
    if( exeType != EXE_MZ && overlay ) {        /* 16-may-90 AFS */
        exeType = EXE_OVL;
        exeFlags.is_32_bit = false;
    }
    header_base = 0;
    switch( exeType ) {
    case EXE_MZ:
        /* exeImageOffset is in 16-byte paragraphs  */
        lseek( exeFH, offsetof( dos_exe_header, hdr_size ), SEEK_SET );
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        exeImageOffset = dummy_16;
        break;
    case EXE_MP_BOUND:
        lseek( exeFH, 0x38, SEEK_SET );
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        header_base = dummy_32;
        /* fall through to MP */
    case EXE_MP:
        /* exeImageOffset is in bytes */
        seek_off = header_base + offsetof( simple_header, hdr_size );
        lseek( exeFH, seek_off, SEEK_SET );
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        exeImageOffset = dummy_16 * 16L;
        exeImageOffset += header_base;
        break;
    case EXE_P3_BOUND:
        lseek( exeFH, 0, SEEK_SET );
        read( exeFH, &head, sizeof( head ) );
        header_base = head.file_size * 512L;
        if( head.mod_size != 0 ) {
            header_base += head.mod_size;
            header_base -= 512;
        }
        /* fall through to P3 */
    case EXE_P2:
    case EXE_P3:
        /* exeImageOffset is in bytes */
        seek_off = header_base + offsetof( extended_header, load_offset );
        lseek( exeFH, seek_off, SEEK_SET );
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        exeImageOffset = dummy_32;
        seek_off = header_base + offsetof( extended_header, offset );
        lseek( exeFH, seek_off, SEEK_SET );
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        exeImageOffset -= dummy_32;                     /* 09-aug-90 AFS */
        exeImageOffset += header_base;
        break;
    case EXE_NW:
        /* exeImageOffset is in bytes */
        lseek( exeFH, offsetof( nlm_header, codeImageOffset ), SEEK_SET );
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        exeImageOffset = dummy_32;
        break;
    case EXE_OS2:
        /* offset into "reserved for future use" portion of the DOS EXE hdr */
        lseek( exeFH, sizeof( dos_exe_header ) + 32, SEEK_SET );
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        new_header = dummy_32;
        lseek(exeFH,new_header + offsetof(os2_exe_header,segment_off),SEEK_SET);
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        /* exeImageOffset is in bytes and points to the segment table */
        exeImageOffset = new_header + dummy_16;
        lseek( exeFH, new_header + offsetof( os2_exe_header, align ), SEEK_SET);
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        segmentShift = dummy_16;
        if( segmentShift == 0 ) {
            segmentShift = OS2_DEF_SEGMENT_SHIFT;
        }
        cacheSegment = 0;       /* no 0 segment in OS/2 (03-may-90 AFS) */
        break;
    case EXE_OS2_FLAT:
    case EXE_OS2_LX:
        lseek( exeFH, sizeof( head ) + 32, SEEK_SET );
        read( exeFH, &dummy_32, sizeof(dummy_32) );
        /* exeImageOffset is in bytes and points to the OS/2 FLAT header */
        exeImageOffset = dummy_32;
        cacheSegment = 0;       /* no 0 segment in OS/2 FLAT */
        break;
    case EXE_QNX:
    case EXE_QNX_386:
    case EXE_QNX_386_FLAT:
        /* exeImageOffset is in bytes and points to the end of the header */
        exeImageOffset = sizeof( lmf_record ) + sizeof( lmf_header );
        cacheSegment = 0;
        break;
    case EXE_PE:
    case EXE_PL:
        /* offset into "reserved for future use" portion of the DOS EXE hdr */
        lseek( exeFH, sizeof( dos_exe_header ) + 32, SEEK_SET );
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        new_header = dummy_32;
#if 0
        lseek(exeFH,new_header + offsetof(pe_header,num_objects),SEEK_SET);
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        lseek(exeFH,new_header + offsetof(pe_header,header_size),SEEK_SET);
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        /* exeImageOffset is in bytes and points to the object table */
        exeImageOffset = dummy_32 - dummy_16 * sizeof( pe_object );
#else
        lseek(exeFH,new_header + offsetof(pe_header,nt_hdr_size),SEEK_SET);
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        /* exeImageOffset is in bytes and points to the object table */
        exeImageOffset = new_header + offsetof(pe_header,magic) + dummy_16;
#endif
        cacheSegment = 0;       /* no 0 segment in OS/2 (03-may-90 AFS) */
        break;
    case EXE_OVL:
        /* overlay file (not an exe) */
        exeImageOffset = 0;
        break;
    case EXE_ELF:
        lseek(exeFH,offsetof(Elf32_Ehdr,e_phoff),SEEK_SET);
        read( exeFH, &dummy_32, sizeof( dummy_32 ) );
        exeImageOffset = dummy_32;
        lseek(exeFH,offsetof(Elf32_Ehdr,e_phentsize),SEEK_SET);
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        cacheLo = dummy_16;             /* entry size */
        read( exeFH, &dummy_16, sizeof( dummy_16 ) );
        cacheSegment = dummy_16;        /* number of entries */
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
    lseek( exeFH, exeImageOffset + mini_off, SEEK_SET );
    read( exeFH, v, sizeof( *v ) );
}



STATIC void searchQNX( uint_16 seg, uint_32 off )
/***********************************************/
{
    int                 st;
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
    lseek( exeFH, 0, SEEK_SET );
    for( ;; ) {
        st = read( exeFH, &rec_head, sizeof( rec_head ) );
        if( st != sizeof( rec_head ) ) break;
        nbytes = rec_head.data_nbytes;
        if( rec_head.rec_type == LMF_IMAGE_END_REC ) break;
        if( rec_head.rec_type != LMF_LOAD_REC ) {
            lseek( exeFH, rec_head.data_nbytes, SEEK_CUR );
            continue;
        }
        st = read( exeFH, &load_data, sizeof( load_data ) );
        if( st != sizeof( load_data ) ) break;
        rec_head.data_nbytes -= sizeof( load_data );
        lo = load_data.offset;
        hi = lo + ( rec_head.data_nbytes - 1 );
        if( load_data.segment == seg_idx ) {
            if( lo <= off && off <= hi ) {
                cacheLo = lo;
                cacheHi = hi;
                cacheExeOffset = tell( exeFH );
                return;
            }
            if( hole_lo < hi && hi < off ) {
                hole_lo = hi + 1;
            }
            if( hole_hi > lo && lo > off ) {
                hole_hi = lo - 1;
            }
        }
        lseek( exeFH, rec_head.data_nbytes, SEEK_CUR );
    }
    isHole = true;
    cacheLo = hole_lo;
    cacheHi = hole_hi;
    exeFlags.end_of_segment = true;
}



STATIC uint_32 TransformExeOffset( uint_16 seg, uint_32 off, uint_16 sect_id )
/****************************************************************************/
{
    ovl_entry               *entry;
    segment_record          segment;
    map_entry               mapping;
    off_t                   tmp_offset;
    image_info              *curr_image;
    uint_32                 subresult;
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
            return( -1 );
        }
        entry = curr_image->ovl_data;
        entry += sect_id - 1;
        subresult = seg * 16L + off;
        subresult += entry->disk_addr;
        subresult -= entry->start_para * 16L;
        return( subresult );
    }
    switch( exeType ) {
    case EXE_NW:
    case EXE_P3:
    case EXE_P3_BOUND:
    case EXE_MP:
    case EXE_MP_BOUND:
        subresult = exeImageOffset + off;
        break;
    case EXE_OS2:
        if( seg != cacheSegment ) {
            tmp_offset = exeImageOffset + (seg-1) * sizeof(segment_record);
            lseek( exeFH, tmp_offset, SEEK_SET );
            read( exeFH, &segment, sizeof( segment ) );
            cacheSegment = seg;
            cacheExeOffset = ((off_t) segment.address) << segmentShift;
        }
        subresult = cacheExeOffset + off;
        break;
    case EXE_OS2_FLAT:
    case EXE_OS2_LX:
        if( seg != cacheSegment ) {
            readEntry( &page_offset_shift,
                       offsetof(os2_flat_header,l.page_shift));
            readEntry( &object_table, offsetof(os2_flat_header,objtab_off));
            tmp_offset = object_table + exeImageOffset;
            tmp_offset += (seg - 1) * sizeof( object_record );
            tmp_offset += offsetof( object_record, mapidx );
            lseek( exeFH, tmp_offset, SEEK_SET );
            read( exeFH, &map_idx, sizeof( map_idx ) );
            readEntry( &objmap_table, offsetof(os2_flat_header,objmap_off));
            tmp_offset = objmap_table + exeImageOffset;
            if( exeType == EXE_OS2_LX ) {
                tmp_offset += ( map_idx-1 ) * sizeof( lx_map_entry );
            } else {
                tmp_offset += ( map_idx-1 ) * sizeof( le_map_entry );
            }
            lseek( exeFH, tmp_offset, SEEK_SET );
            read( exeFH, &mapping, sizeof( mapping ) );
            readEntry( &page_off, offsetof( os2_flat_header, page_off ) );
            if( exeType == EXE_OS2_LX ) {
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
    case EXE_QNX:
    case EXE_QNX_386:
    case EXE_QNX_386_FLAT:
        if( seg != cacheSegment || off < cacheLo || off > cacheHi ) {
            searchQNX( seg, off );
        }
        subresult = cacheExeOffset + ( off - cacheLo );
        break;
    case EXE_MZ:
        subresult = ( seg + exeImageOffset ) * 16 + off;
        break;
    case EXE_PE:
        if( seg != cacheSegment ) {
            tmp_offset = exeImageOffset;
            tmp_offset += (seg - 1) * sizeof( pe_object );
            tmp_offset += offsetof( pe_object, physical_offset );
            lseek( exeFH, tmp_offset, SEEK_SET );
            read( exeFH, &seg_start, sizeof( seg_start ) );
            cacheExeOffset = seg_start;
        }
        subresult = cacheExeOffset + off;
        break;
    case EXE_ELF:
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
                    return( -1 );
                }
                lseek( exeFH, tmp_offset, SEEK_SET );
                read( exeFH, &phe, sizeof( phe ) );
                if( phe.p_type == PT_LOAD
                 && off >= phe.p_vaddr
                 && off < (phe.p_vaddr+phe.p_memsz) ) break;
                tmp_offset += cacheLo;
                ++i;
            }
            cacheHi = phe.p_memsz;
            cacheExeOffset = phe.p_offset;
            subresult = phe.p_offset + (off - phe.p_vaddr);
        }
        break;
    case EXE_OVL:
/**/    myassert( 0 );
    case EXE_P2:    /* P2 format is not supported 03-may-90 AFS */
    default:
        subresult = -1;
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
    int             count;
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
    int                 count;

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
    count = 0;
    while( count < curr_image->ovl_count ) {
        if( addr->mach.segment == overlay[count].base_para ) {
            addr->sect_id = count + 1;
            break;
        }
        count++;
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
        if( (thd->end_time - curr_tick) < count2 ) {
            count2 = thd->end_time - curr_tick;
        }
        for( index2 = 0; index2 < count2; ++index2 ) {
            while( curr_tick == map_tick ) {
                remap_segment[remap->section-1] = remap->segment;
                if( remap == CurrSIOData->remaps ) {
                    map_tick = CurrSIOData->total_samples;
                } else {
                    remap = remap->next;
                    map_tick = remap->tick;
                }
            }
            actual_segment = samp_data[index2].mach.segment;
            if( actual_segment != base_segment ) {
                count3 = 0;
                while( count3 < ovl_count ) {
                    if( actual_segment == remap_segment[count3] ) {
                        samp_data[index2].mach.segment
                            = overlay[count3].base_para;
                        samp_data[index2].sect_id = count3 + 1;
                        break;
                    }
                    count3++;
                }
            }
            curr_tick++;
            count3++;
        }
    }
    ProfFree( remap_segment );
}



STATIC bool LoadOverlayInfo( void )
/*********************************/
{
    image_info              *image;
    file_handle             fh;
    address                 map_addr;
    addr_seg                base_segment;
    off_t                   fileoffset;
    off_t                   filenameoffset;
    ovltab_entry            formal_entry;
    ovltab_prolog           prolog;
    ovl_entry               *entry;
    ovl_entry               *entry_ptr;
    char                    *ovl_name;
    char                    buffer[_MAX_PATH];
    char                    buffer1[_MAX_PATH2];
    char                    buffer2[_MAX_PATH2];
    char                    buffer3[_MAX_PATH2];
    char                    *drive;
    char                    *dir;
    char                    *name;
    char                    *ext;
    int                     count;
    int                     len;

    image = CurrSIOData->curr_image;
    fh = ExeOpen( image->name );
    if( fh == -1 ) {
        ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
        return( false );
    }
    SetExeFile( fh, false );
    map_addr = image->overlay_table;
    MapAddressToMap( &map_addr.mach );
    fileoffset = TransformExeOffset( map_addr.mach.segment,
                                     map_addr.mach.offset, ROOT_SECTION );
    if( lseek( fh, fileoffset, SEEK_SET ) != fileoffset ) {
        ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
        return( false );
    }
    if( read( fh, &prolog, sizeof(prolog) ) != sizeof(prolog) ) {
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
        if( read( fh, &formal_entry, sizeof(formal_entry) )
         != sizeof(formal_entry) ) {
            ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
            return( false );
        }
        if( formal_entry.flags_anc == OVLTAB_TERMINATOR ) break;
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
            fileoffset = lseek( fh, 0, SEEK_CUR );  /* save current posn */
            map_addr = image->overlay_table;
            map_addr.mach.offset += formal_entry.fname;
            MapAddressToMap( &map_addr.mach );
            filenameoffset = TransformExeOffset( map_addr.mach.segment,
                                                 map_addr.mach.offset,
                                                 ROOT_SECTION );
            if( lseek( fh, filenameoffset, SEEK_SET ) != filenameoffset ) {
                ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
                return( false );
            }
            if( read( fh, buffer, _MAX_PATH ) != _MAX_PATH ) {
                ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
                return( false );
            }
            /* find overlay file */
            if( access( buffer, 0 ) != 0 ) {
                strcpy( buffer3, buffer );
                _searchenv( buffer3, "PATH", buffer );
                if( buffer[0] == '\0' ) {
                    _splitpath2( image->name, buffer1, &drive, &dir, NULL, NULL );
                    _splitpath2( buffer3, buffer2, NULL, NULL, &name, &ext );
                    _makepath( buffer, drive, dir, name, ext );
                }
            }
            ovl_name = buffer;
            entry->separate_overlay = true;
            /* restore current posn */
            if( lseek( fh, fileoffset, SEEK_SET ) != fileoffset ) {
                ErrorMsg( LIT( Cannot_Process_Ovly ), image->name );
                return( false );
            }
        }
        len = strlen( ovl_name ) + 1;
        entry->fname = ProfAlloc( len );
        memcpy( entry->fname, ovl_name, len );
    }
    ExeClose( fh );
    return( true );
}



bool LoadImageOverlays( void )
/****************************/
{
    image_info      *curr_image;
    int             image_index;

    /* assume 32-bit addresses until proven otherwise */
    exeFlags.is_32_bit = true;

    image_index = 0;
    while( image_index < CurrSIOData->image_count ) {
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
        image_index++;
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



STATIC void exeRewind( size_t offset )
/************************************/
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
    if( isHole ) {
        return( 0 );
    }
    if( exeCurrent >= exeAmount ) {
        exePosition += exeAmount;
        lseek( exeFH, exePosition, SEEK_SET );
        if( nbytes < BUFF_SIZE ) {
            exeAmount = read( exeFH, exeBuff, nbytes );
        } else {
            exeAmount = read( exeFH, exeBuff, BUFF_SIZE );
        }
        if( exeAmount == 0 || exeAmount == (uint_16)-1 ) {
            exeFlags.end_of_file = true;
            return( 0 );
        }
        exeCurrent = 0;
    }
    return( exeBuff[ exeCurrent++ ] );
}

size_t FormatAddr( address a, char *buffer, size_t max )
{
    mad_type_info       host;
    mad_type_info       mti;
    unsigned_8          item[16];

    MADTypeInfoForHost( MTK_ADDRESS, sizeof( address ), &host );
    MADTypeInfo( MADTypeDefault( MTK_ADDRESS, MAF_FULL, NULL, &a ), &mti );
    MADTypeConvert( &host, &a, &mti, item, 0 );
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
    mad_type_info       host;
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
                MADTypeInfoForHost( MTK_INTEGER, sizeof( item ), &host );
                mad_max = 4;
                MADTypeToString( 16, &host, &item, tail, &mad_max );
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
    sr = AddrSym( NO_MOD, addr, sym );
    switch( sr ) {
    case SR_EXACT:
    //case SR_CLOSEST: /* add in if we have symbol+offset */
        break;
    default:
        return( false );
    }
    name_len = SymName( sym, NULL, SN_DEMANGLED, buff, buff_len );
    if( name_len == 0 ) {
        SymName( sym, NULL, SN_SOURCE, buff, buff_len );
    }
    return( true );
}


bool IsX86BigAddr( address a )
{
    return( exeFlags.is_32_bit );
}

bool IsX86RealAddr( address a )
{
    switch( exeType ) {
    case EXE_MZ:
    case EXE_OVL:
        return( true );
    }
    return( false );
}


int_16 GetDataByte( void )
/************************/
{
    int_16 value;

    value = exeGetChar();
    AdvanceCurrentOffset( sizeof( int_8 ) );
    return( (int_8)value );
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
