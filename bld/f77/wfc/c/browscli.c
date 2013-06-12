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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>

#include "watcom.h"
#include "dw.h"
#include "dwarf.h"
#include "ftnstd.h"
#include "errcod.h"
#include "fio.h"
#include "cioconst.h"
#include "omodes.h"
#include "browscli.h"
#include "brow2elf.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "cspawn.h"

#define SWAP( x, y )    {x^=y^=x^=y;}

static section_data             Sections[ DW_DEBUG_MAX ];
static char                     initial_section_type;

extern  file_handle     SDOpen(char *,int);
extern  void            SDClose(file_handle);
extern  void            SDSeek(file_handle,int,int);
extern  uint            SDRead(file_handle,char *,uint);
extern  void            SDWrite(file_handle,byte *,int);
extern  bool            SDError(file_handle,char *);
extern  bool            SDEof(file_handle);
extern  void            SDScratch(char *);
extern  void            SDSetAttr(file_attr);

/* Forward declarations */
static void chkIOErr( file_handle fp, int error, char *filename );


static void CLIWrite( dw_sectnum sect, const void *block, dw_size_t size ) {
/*********************************************************************/

    char                        *temp;
    section_data                *cur_sec;

    cur_sec = &Sections[sect];
    if ( cur_sec->sec_type == DEFAULT_SECTION ) {
        if ( ( initial_section_type == DEFAULT_SECTION ) ||
           ( initial_section_type == FILE_SECTION ) ) {
                cur_sec->sec_type = FILE_SECTION;
                SDSetAttr( REC_FIXED | SEEK );
                temp = tmpnam( NULL );
                cur_sec->filename = FMemAlloc( strlen( temp ) + 1 );
                strcpy( cur_sec->filename, temp );
                cur_sec->fp = SDOpen( temp, UPDATE_FILE );
                chkIOErr( cur_sec->fp, SM_OPENING_FILE, temp );
        } else {
                cur_sec->sec_type = initial_section_type;
                cur_sec->size = MEM_INCREMENT;
                cur_sec->data = FMemAlloc( MEM_INCREMENT );
        }
    }

    switch( cur_sec->sec_type ) {
    case( MEM_SECTION ):
        if ( cur_sec->size <= ( cur_sec->cur_offset + size ) ) {
            temp = FMemAlloc( cur_sec->size + MEM_INCREMENT );
            memcpy( temp, cur_sec->data, cur_sec->size );
            FMemFree( cur_sec->data );
            cur_sec->data = temp;
            cur_sec->size += MEM_INCREMENT;
        }
        memcpy( ( cur_sec->data + cur_sec->cur_offset ), block, size );
        break;
    case( FILE_SECTION ):
        SDWrite( cur_sec->fp, (byte *)block, size );
        chkIOErr( cur_sec->fp, SM_IO_WRITE_ERR, "temporary file" );
        break;
    default:
        Error( CP_FATAL_ERROR, "Internal browse generator error" );
        CSuicide();
    };
    cur_sec->cur_offset += size;
    cur_sec->max_offset = max( cur_sec->cur_offset, cur_sec->max_offset );
}


static void CLIReloc( dw_sectnum sect, dw_relocs reloc_type, ... ) {
/******************************************************/
    static char                 zeros[] = { 0, 0 };
    dw_sym_handle               sym;
    uint                        section;
    va_list                     args;
    unsigned_32                 u32_data;

    va_start( args, reloc_type );
    switch( reloc_type ) {
    case DW_W_LOW_PC:
    case DW_W_LABEL:
    case DW_W_DEFAULT_FUNCTION:
    case DW_W_ARANGE_ADDR:
        u32_data = 0;   // NOTE: assumes little-endian byte order
        CLIWrite( sect, &u32_data, sizeof( int ) );
        break;
    case DW_W_HIGH_PC:
        u32_data = 1;   // NOTE: assumes little-endian byte order
        CLIWrite( sect, &u32_data, sizeof( int ) );
        break;
    case DW_W_UNIT_SIZE:
        u32_data = 1;   // NOTE: assumes little-endian byte order
        CLIWrite( sect, &u32_data, sizeof( uint_32 ) );
        break;
    case DW_W_SECTION_POS:
        section = va_arg( args, uint );
        CLIWrite( sect, &Sections[ section ].cur_offset, sizeof( uint_32 ) );
        break;
    case DW_W_STATIC:
        sym = va_arg( args, dw_sym_handle );
        u32_data = 0;
        CLIWrite( sect, &u32_data, sizeof( uint_32 ) );
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    case DW_W_SEGMENT:
        CLIWrite( sect, zeros, sizeof( zeros ) );
        break;
    default:
        abort();
        break;
    }
}


static void CLIZeroWrite( dw_sectnum sect, uint size ) {
/*************************************************/

    char        *btmp;

    btmp = FMemAlloc( size+1 );
    memset( btmp, 0, size );
    CLIWrite( sect, btmp, size );
    FMemFree( btmp );
}

static void CLISeek( dw_sectnum sect, long offs, uint type ) {
/******************************************************/

    section_data                *cur_sec;
    long                        temp;
    int                         typ;
    long                        new_off;

    cur_sec = &Sections[sect];

    switch( type ) {
    case DW_SEEK_CUR:
        new_off = cur_sec->cur_offset + offs;
        typ = SEEK_CUR;
        break;
    case DW_SEEK_SET:
        new_off = offs;
        typ = SEEK_SET;
        break;
    case DW_SEEK_END:
        new_off = Sections[ sect ].max_offset - offs;
        typ = SEEK_END;
        break;
    }
    if ( cur_sec->sec_type == MEM_SECTION ) {
        if ( cur_sec->max_offset < new_off ) {
            temp = new_off - cur_sec->max_offset;
            cur_sec->cur_offset = cur_sec->max_offset;
            CLIZeroWrite( sect, temp );
        }
    } else if ( cur_sec->sec_type == FILE_SECTION ) {
        if ( !( cur_sec->fp ) ) {
            CLIZeroWrite( sect, offs );
        } else {
            if ( cur_sec->max_offset < new_off ) {
                SDSeek( cur_sec->fp, cur_sec->max_offset, 1 );
                chkIOErr( cur_sec->fp, SM_IO_READ_ERR, "temporary file" );
                cur_sec->cur_offset = cur_sec->max_offset;
                temp = new_off - cur_sec->max_offset;
                CLIZeroWrite( sect, temp );
            } else {
                SDSeek( cur_sec->fp, new_off, 1 );
                chkIOErr( cur_sec->fp, SM_IO_READ_ERR, "temporary file" );
            }
        }
    } else {
        CLIZeroWrite( sect, offs );
    }
    cur_sec->cur_offset = new_off;
}

static long CLITell( dw_sectnum sect ) {
/*********************************/

    return( Sections[ sect ].cur_offset );
}


static void *CLIAlloc( size_t size ) {
/*************************************/

    void        *p;

    p = FMemAlloc( size );
    if( p == NULL && size ) {
        Error( MO_DYNAMIC_OUT );
        CSuicide();
    }
    return( p );
}


static void CLIFree( void *p ) {
/******************************/

    FMemFree( p );
}


void CLIInit( dw_funcs *cfuncs, int is_type ) {
/**********************************************/

    unsigned_32         x = 0;

    if( !cfuncs ) return;
    cfuncs->reloc = CLIReloc;
    cfuncs->write = CLIWrite;
    cfuncs->seek = CLISeek;
    cfuncs->tell = CLITell;
    cfuncs->alloc = CLIAlloc;
    cfuncs->free = CLIFree;
    initial_section_type = is_type;
    for( x = 0; x < DW_DEBUG_MAX; x++ ) {
        Sections[x].sec_number = x;
    }
}

void CLILock( void ) {
/********************/

}

void CLIRewind( void ) {
/***********************/

    int         x = 0;

    for( x = 0; x < DW_DEBUG_MAX; x++ ) {
        CLISeek( x, 0, DW_SEEK_SET );
    }
}

int CLIRead( char *buf, int size, int sec ) {
/*******************************************/

    size = min(size, Sections[sec].max_offset - Sections[sec].cur_offset);
    if ( !size ) return( 0 );
    if ( ( Sections[sec].sec_type == FILE_SECTION ) && Sections[sec].fp ) {
        SDRead( Sections[sec].fp, buf, size );
        chkIOErr( Sections[sec].fp, SM_IO_READ_ERR, "temporary file" );
    } else if (( Sections[sec].sec_type==MEM_SECTION) && Sections[sec].data) {
        memcpy( buf, Sections[sec].data + Sections[sec].cur_offset, size );
    } else {
        size = 0;
    }
    Sections[sec].cur_offset += size;
    return( size );
}

static void chkIOErr( file_handle fp, int error, char *filename ) {
/******************************************************************/

    char        err_msg[ERR_BUFF_SIZE+1];

    if( SDError( fp, err_msg ) ) {
        Error( error, filename, err_msg );
        CSuicide();
    }
}

void CLIDump( char *filename ) {
/******************************/

    file_handle         fp;

    SDSetAttr( REC_FIXED | SEEK );
    fp = SDOpen( filename, WRITE_FILE );
    chkIOErr( fp, SM_OPENING_FILE, filename );
    CLILock();
    CLIRewind();
    CreateBrowseFile( fp, &Sections[DW_DEBUG_ABBREV],
                        &Sections[DW_DEBUG_INFO], &Sections[DW_DEBUG_REF],
                        &Sections[DW_DEBUG_LINE], &Sections[DW_DEBUG_MACINFO],
                        CLIRead, filename );
    SDClose( fp );
}

void CLIClear( void ) {
/****************/

    int         x = 0;

    for( x = 0; x < DW_DEBUG_MAX; x++ ) {
        if ( ( Sections[x].sec_type == FILE_SECTION ) && Sections[x].fp ) {
            SDClose( Sections[x].fp );
            if ( Sections[x].filename ) {
                SDScratch( Sections[x].filename );
                FMemFree( Sections[x].filename );
            }
        } else if ((Sections[x].sec_type == MEM_SECTION) && Sections[x].data){
            FMemFree( Sections[x].data );
        }
        memset( &(Sections[x] ), 0, sizeof( section_data ) );
    }
}
