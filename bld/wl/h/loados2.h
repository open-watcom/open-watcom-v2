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


#ifndef _LOADOS2_H_
#define _LOADOS2_H_

// linker specific OS/2 load file stuff.

typedef struct os2_seg_flags {
    struct os2_seg_flags *  next;
    unsigned_16             flags;      // as above.
    char *                  name;
    segflag_type            type;    // true if flags for a class.
    unsigned_16             specified;  // used for enforcing mutual exclusion
} os2_seg_flags;

// These used in the specified field.
#define SEG_IOPL_SPECD  0x800
#define SEG_READ_SPECD  0x400
#define SEG_RFLAG (SEG_READ_SPECD | SEG_READ_ONLY)

// These are segment flags used for the PE format and OS/2 v2.x These should
// hopefully not correspond to any existing OS/2 or 16-bit windows flag -
// if a new one appears, make sure these don't conflict!
// actually, SEG_NOPAGE conflicts with SEG_ITERATED, but since SEG_ITERATED
// isn't used, hopefully that should not matter.

#define SEG_NOPAGE              0x8
#define SEG_INVALID             0x8000
#define SEG_RESIDENT_MASK       0x0006
#define SEG_RESIDENT            0x0004
#define SEG_CONTIGUOUS          0x0006
#define SEG_RESIDENT_SHIFT      7

// NOTE: in these flags, CLEAN_MEMORY and INIT_THREAD_FLAG have the same value,
// since CLEAN_MEMORY is windows 16-bit only, and INIT_THREAD_FLAG is windows pe
// only.  Same goes for PROPOTIONAL_FONT and TERM_THREAD_FLAG.

#define SINGLE_AUTO_DATA    0x0001
#define MULTIPLE_AUTO_DATA  0x0002
#define INIT_INSTANCE_FLAG  0x0004
#define PHONEY_STACK_FLAG   0x0008
#define PM_COMPATIBLE       0x0010
#define PM_APPLICATION      0x0020
#define LONG_FILENAMES      0x0040
#define CLEAN_MEMORY        0x0080
#define INIT_THREAD_FLAG    0x0080
#define PROPORTIONAL_FONT   0x0100
#define TERM_THREAD_FLAG    0x0100
#define PROTMODE_ONLY       0x0200
#define PHYS_DEVICE         0x0400
#define VIRT_DEVICE         0x0800
#define SHARABLE_DGROUP     0x1000
#define PM_NOT_COMPATIBLE   0x4000
#define TERM_INSTANCE_FLAG  0x8000

#define FLAT_GRANULARITY    (64UL * 1024)
#define FLAT_ROUND( x )     ROUND_UP( x, FLAT_GRANULARITY )

#include "exeos2.h"

/* in loadflat.c */

extern void             FiniOS2FlatLoadFile( void );
extern bool             FindOS2ExportSym( symbol *, dll_sym_info ** );

/* in loados2.c */

extern unsigned long    ImportProcTable( unsigned long * );
extern unsigned long    ImportModTable( unsigned long * );
extern void             CheckExport( char *, unsigned_16,
                                     int (*)(const char *,const char *));
extern unsigned long    ResNonResNameTable( bool );
extern void             ChkOS2Data( void );
extern void             ChkOS2Exports( void );
extern void             PhoneyStack( void );
extern void             FiniOS2LoadFile( void );
extern void             FreeImpNameTab( void );
extern unsigned_32      GetStubSize( void );
extern unsigned_32      Write_Stub_File( void );

#endif
