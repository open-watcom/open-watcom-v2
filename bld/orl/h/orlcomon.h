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


#ifndef ORL_COMMON_INCLUDED
#define ORL_COMMON_INCLUDED

/* Used in both orl.h and orlintl.h
 */

typedef struct {
    orl_sec_handle                      section;
    orl_sec_offset                      offset;
    orl_symbol_handle                   symbol;
    orl_reloc_type                      type;
    orl_reloc_addend                    addend;
    orl_symbol_handle                   frame; // used in OMF should be NULL otherwise
} orl_reloc;

typedef
    orl_return          (*orl_sec_return_func)( orl_sec_handle );

typedef
    orl_return          (*orl_reloc_return_func)( orl_reloc * );

typedef
    orl_return          (*orl_symbol_return_func)( orl_symbol_handle );

typedef
    orl_return          (*orl_group_return_func)( orl_group_handle );

typedef struct {
    orl_return  (*export_fn)( char *, void * );
    orl_return  (*deflib_fn)( char *, void * );
    orl_return  (*entry_fn)( char *, void * );
    orl_return  (*scantab_fn)( orl_sec_handle, orl_sec_offset, orl_sec_offset, void * );
} orl_note_callbacks;

#endif
