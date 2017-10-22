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


#ifndef DWHANDLE_H_INCLUDED
#define DWHANDLE_H_INCLUDED

/* this is used to represent a chain of relocations */
typedef struct reloc_chain {
    struct reloc_chain  *next;
    dw_out_offset       offset;         /* CLISeek() offset */
    dw_sectnum          section;        /* section to seek in */
} reloc_chain;

#ifdef _M_I86
#define    HANDLE_MODIFIED     0x80000000    // handle is a type modifier
#define    HANDLE_MASK         0x0fffffff    // only 2^28 handles ;)
#else
enum {
    HANDLE_MODIFIED     = 0x80000000,   // handle is a type modifier
    HANDLE_MASK         = 0x0fffffff    // only 2^28 handles ;)
};
#endif

#define IS_FUNDAMENTAL( __h )   ( (__h) < DW_FT_MAX )
#define GET_FUNDAMENTAL( __h )  ( __h )

#define GET_HANDLE_LOCATION(__c)        (((__c)->reloc.u.offset & 1) ? (__c)->reloc.u.offset >> 1 : 0)
#define SET_HANDLE_LOCATION(__c,__o)    (__c)->reloc.u.offset = (1 | (__o << 1))
#define IS_FORWARD_LOCATION(__c)        (((__c)->reloc.u.offset & 1) == 0)

typedef struct {
    struct {
        union {
            dw_sect_offs    offset;
            reloc_chain     *chain;
        } u;
    } reloc;
} handle_common;


typedef struct handle_extra_base {
    union handle_extra  *next;
    dw_handle           handle;
} handle_extra_base;


struct modified_type {
    handle_extra_base   base;
    dw_handle           base_type;
    uint_8              modifiers;
};

struct structure_type {
    handle_extra_base   base;
    uint_8              kind;
};


/*
    All the handles with extended information must be in this union;
    it is used to create a second carver that allocates larger pieces.
*/
typedef union handle_extra {
    handle_extra_base           base;
    struct modified_type        mod;
    struct structure_type       structure;
} handle_extra;


/* maximum height of a node in the skip list */
#define MAX_HANDLE_HEIGHT       12


#define InitHandles             DW_InitHandles
#define FiniHandles             DW_FiniHandles
#define NewHandle               DW_NewHandle
#define GetCommon               DW_GetCommon
#define CreateExtra             DW_CreateExtra
#define DestroyExtra            DW_DestroyExtra
#define GetExtra                DW_GetExtra
#define LabelNewHandle          DW_LabelNewHandle
#define SetHandleLocation       DW_SetHandleLocation
#define HandleReference         DW_HandleReference
#define HandleWriteOffset       DW_HandleWriteOffset


extern void             InitHandles( dw_client );
extern void             FiniHandles( dw_client );
extern dw_handle        NewHandle( dw_client );
extern handle_common    *GetCommon( dw_client, dw_handle );
extern handle_extra     *CreateExtra( dw_client, dw_handle );
extern void             DestroyExtra( dw_client, dw_handle );
extern handle_extra     *GetExtra( dw_client, dw_handle );
extern dw_handle        LabelNewHandle( dw_client );
extern dw_handle        GetHandle( dw_client cli );
extern void             SetHandleLocation( dw_client, dw_handle );
extern void             HandleReference( dw_client, dw_handle, dw_sectnum );
extern void             HandleWriteOffset( dw_client, dw_handle, dw_sectnum );


#endif
