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


#ifndef _PCHEADER_H
#define _PCHEADER_H

#include <stdio.h>

// constants used for non-carve pointer indices (similar to CARVE_* in carve.h)
enum {
    PCH_NULL_INDEX      = 0,
    PCH_ERROR_INDEX     = 1,
    PCH_FIRST_INDEX     = 2,
};

#if _CPU == 386
#define PCH_DEFAULT_FILE_NAME   "wpp386.pch"
#elif _CPU == 8086
#define PCH_DEFAULT_FILE_NAME   "wpp.pch"
#elif _CPU == _AXP
#define PCH_DEFAULT_FILE_NAME   "wppaxp.pch"
#elif _CPU == _PPC
#define PCH_DEFAULT_FILE_NAME   "wppppc.pch"
#else
#error missing _CPU check
#endif

#define PHH_MAJOR               0x03
#define PHH_MINOR               0x22

#define TEXT_HEADER_SIZE        40
#ifdef __QNX__
#define PHH_TEXT_HEADER         "WATCOM C++ Precompiled Header File\n\x0c\x04   "
#else
#define PHH_TEXT_HEADER         "WATCOM C++ Precompiled Header File\r\n\x1a   "
#endif

// use a different signature for the debugging version of the compiler
// since extra info may be written into the pre-compiled header file
#define SIGNATURE_SIZE          4
#ifndef NDEBUG
#define PHH_SIGNATURE_0         'D'
#else
#define PHH_SIGNATURE_0         'W'
#endif
#define PHH_SIGNATURE_1         'C'
#define PHH_SIGNATURE_2         '+'
#define PHH_SIGNATURE_3         '+'

#define PHH_ARCHITECTURE_286    0x01
#define PHH_ARCHITECTURE_386    0x02
#define PHH_ARCHITECTURE_AXP    0x03
#define PHH_ARCHITECTURE_PPC    0x04

#if _CPU == 386
#define PHH_TARG_ARCHITECTURE   PHH_ARCHITECTURE_386
#elif _CPU == 8086
#define PHH_TARG_ARCHITECTURE   PHH_ARCHITECTURE_286
#elif _CPU == _AXP
#define PHH_TARG_ARCHITECTURE   PHH_ARCHITECTURE_AXP
#else
#error missing _CPU check
#endif

#define PHH_SYSTEM_DOS          0x01
#define PHH_SYSTEM_UNIX         0x02

#ifdef __QNX__
#define PHH_HOST_SYSTEM         PHH_SYSTEM_UNIX
#else
#define PHH_HOST_SYSTEM         PHH_SYSTEM_DOS
#endif

#if defined(__I86__)
#define PHH_HOST_ARCHITECTURE   PHH_ARCHITECTURE_286
#elif defined(__386__)
#define PHH_HOST_ARCHITECTURE   PHH_ARCHITECTURE_386
#elif defined(__AXP__)
#define PHH_HOST_ARCHITECTURE   PHH_ARCHITECTURE_AXP
#else
#error missing host architecture check
#endif

#define PHH_CORRUPTED_YES       0
#define PHH_CORRUPTED_NO        sizeof( precompiled_header_header )

typedef struct {
    char                text_header[TEXT_HEADER_SIZE];
    char                signature[SIGNATURE_SIZE];
    uint_8              major_version;
    uint_8              minor_version;
    uint_8              target_architecture;
    uint_8              host_system;
    uint_8              host_architecture;
    uint_8              corrupted;
    unsigned            : 0;
    unsigned long       browse_info;
} precompiled_header_header;

#ifndef _PCH_HEADER_ONLY

// PCH callback status values
typedef enum {
    PCHCB_OK,
    PCHCB_ERROR,
} pch_status;

typedef struct pch_reloc_info {
    fpos_t              start;
    fpos_t              stop;
} pch_reloc_info;

/* R/W function indices */
typedef enum {
#define PCH_EXEC( s, g )        PCHRW_##s,
#include "pcregdef.h"
    PCHRW_MAX
} pch_rw_index;

/* Read function prototypes */
#define PCH_EXEC( s, g )        extern pch_status PCHRead##g( void );
#include "pcregdef.h"

/* Write function prototypes */
#define PCH_EXEC( s, g )        extern pch_status PCHWrite##g( void );
#include "pcregdef.h"

/* Init function prototypes */
#define PCH_EXEC( s, g )        extern pch_status PCHInit##g( boolean writing );
#include "pcregdef.h"

/* Fini function prototypes */
#define PCH_EXEC( s, g )        extern pch_status PCHFini##g( boolean writing );
#include "pcregdef.h"

/* Relocation function indices */
typedef enum {
#define PCH_RELOC( s, g )       PCHRELOC_##s,
#include "pcregdef.h"
    PCHRELOC_MAX
} pch_reloc_index;

/* Relocation function prototypes */
#define PCH_RELOC( s, g )       extern pch_status PCHReloc##g( char *, size_t );
#include "pcregdef.h"

typedef enum {
    PCHA_OK,                    // pre-compiled header was loaded
    PCHA_ERRORS_PRESENT,        // compiler errors occured before load
    PCHA_NOT_PRESENT,           // no file was present
    PCHA_HEADER,                // file header is incorrect
    PCHA_STALE,                 // pre-compiled header is out of date
    PCHA_ERROR,                 // problem was detected
    PCHA_IGNORE,                // cmdline option forced compiler to ignore PCH
    PCHA_NULL
} pch_absorb;


// MACROS

#define _pch_align_size( x )    (((x)+(sizeof(unsigned)-1))&~(sizeof(unsigned)-1))


#define PCHReadLocSize( tgt, src, size )        \
{   void* cursor = CompInfo.pch_buff_cursor;    \
    void* last = (char*)cursor + _pch_align_size(size);  \
    if( last <= CompInfo.pch_buff_end ) {       \
        PCHTrashAlreadyRead();                  \
        tgt = cursor;                           \
        CompInfo.pch_buff_cursor = last;        \
    } else {                                    \
        tgt = PCHRead( src, size );             \
    }                                           \
}

#define PCHReadLoc( tgt, src )                  \
    PCHReadLocSize( tgt, src, sizeof(*tgt) );

#define PCHLocateCVIndex( tgt )                 \
{                                               \
    cv_index* p_value;                          \
    void* buff_ptr;                             \
    void* end;                                  \
    buff_ptr = CompInfo.pch_buff_cursor;        \
    end = (char*)buff_ptr + sizeof( cv_index ); \
    if( end <= CompInfo.pch_buff_end ) {        \
        PCHTrashAlreadyRead();                  \
        p_value = (cv_index*)buff_ptr;          \
        CompInfo.pch_buff_cursor = end;         \
        tgt = *p_value;                         \
    } else {                                    \
        tgt = PCHReadCVIndex();                 \
    }                                           \
}

#define PCHReadMapped( tgt, src, index, init )  \
{   void* cursor;                               \
    void* last;                                 \
    void* end;                                  \
    end = CompInfo.pch_buff_end;                \
    cursor = CompInfo.pch_buff_cursor;          \
    last = (char*)cursor + sizeof(cv_index);    \
    if( last <= end ) {                         \
        PCHTrashAlreadyRead();                  \
        index = *(cv_index*)cursor;             \
    } else {                                    \
        index = PCHReadCVIndex();               \
        last = CompInfo.pch_buff_cursor;        \
    }                                           \
    if( index == CARVE_NULL_INDEX ) {           \
        CompInfo.pch_buff_cursor = last;        \
        break;                                  \
    }                                           \
    src = CarveInitElement( &init, index );     \
    cursor = last;                              \
    last = (char*)last + _pch_align_size(sizeof(*tgt));          \
    if( last <= end ) {                         \
        tgt = cursor;                           \
        CompInfo.pch_buff_cursor = last;        \
    } else {                                    \
        CompInfo.pch_buff_cursor = cursor;      \
        tgt = PCHRead( src, sizeof(*tgt) );     \
    }                                           \
}


// PROTOTYPES

extern void PCHActivate( void );
extern void PCHSetFileName( char * );
extern void PCHeaderCreate( char * );
extern pch_absorb PCHeaderAbsorb( char * );
extern void PCHWrite( void const *, size_t );
extern void PCHWriteUnaligned( void const *, size_t );
extern void* PCHRead( void *, size_t );
extern void* PCHReadUnaligned( void *, size_t );
extern void* PCHReadLocate( void *p, size_t size );
extern void* PCHReadLocateUnaligned( void *p, size_t size );
extern unsigned PCHReadUInt( void );
extern unsigned PCHReadUIntUnaligned( void );
extern void* PCHReadPtr( void );
extern void PCHWriteUInt( unsigned );
extern void PCHRelocStart( pch_reloc_index );
extern void PCHRelocStop( pch_reloc_index );
extern void PCHPerformReloc( pch_reloc_index );
extern void PCHWarn2p( unsigned, void * );
extern char *PCHDebugInfoName( void );
extern void PCHFlushBuffer( void );
extern char *PCHFileName( void );
#ifdef OPT_BR
long PCHSeek( long offset, int type );
#endif
#ifndef NDEBUG
extern void PCHVerifyFile( int handle );
#else
#define  PCHVerifyHandle( handle ) handle = handle
#endif

#define PCHWriteCVIndex( x )    PCHWriteUInt( x )
#define PCHReadCVIndex()        PCHReadUInt()

#ifndef NDEBUG
extern void PCHTrashAlreadyRead( void );
#else
#define PCHTrashAlreadyRead()
#endif

#endif // _PCH_HEADER_ONLY
#endif
