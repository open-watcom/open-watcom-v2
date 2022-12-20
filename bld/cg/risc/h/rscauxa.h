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


#include "offset.h"
#include "owl.h"


#define FECALL_LINKAGE_OS           ( 0x00000001L << _TARG_AUX_SHIFT )
#define FECALL_LINKAGE_OSFUNC       ( 0x00000002L << _TARG_AUX_SHIFT )
#define FECALL_LINKAGE_OSENTRY      ( 0x00000004L << _TARG_AUX_SHIFT )
#define FECALL_LINKAGE_CLINK        ( 0x00000008L << _TARG_AUX_SHIFT )
#define FECALL_CALLER_POPS          ( 0x00000010L << _TARG_AUX_SHIFT )
#define FECALL_NO_MEMORY_READ       ( 0x00000020L << _TARG_AUX_SHIFT )
#define FECALL_NO_MEMORY_CHANGED    ( 0x00000040L << _TARG_AUX_SHIFT )
#define FECALL_DLL_EXPORT           ( 0x00000080L << _TARG_AUX_SHIFT )
#define LAST_TARG_AUX_ATTRIBUTE     ( 0x00000080L << _TARG_AUX_SHIFT )

#if LAST_TARG_AUX_ATTRIBUTE == 0
    #error Overflowed a long constant in rscauxa.h
#endif

#define FECALL_LINKAGES (FECALL_LINKAGE_OS+FECALL_LINKAGE_OSFUNC+FECALL_LINKAGE_OSENTRY+FECALL_LINKAGE_CLINK)

typedef unsigned        call_class;

typedef unsigned        byte_seq_len;

typedef struct byte_seq_reloc {
    struct byte_seq_reloc   *next;
    offset                  off;
    void                    *sym;
    owl_reloc_type          type;
} byte_seq_reloc;

#define STRUCT_BYTE_SEQ( x ) \
{ \
    byte_seq_len    length; \
    byte_seq_reloc  *relocs; \
    byte            data[x]; \
}

typedef struct byte_seq STRUCT_BYTE_SEQ( 1 ) byte_seq;
