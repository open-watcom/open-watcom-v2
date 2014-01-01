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

/*

these structures are valid only for 32-bit OS2/NT targets

struct  scope_entry {
        unsigned char           parent_scope;
        unsigned char           try_type;
        void                    (*exception_handler);
};

struct  _EXCEPTION_RECORD;
struct  _CONTEXT;

struct  try_block {
        struct try_block        *next;
        void                    (*try_handler)( void );
        unsigned                saved_EBP;
        struct scope_entry      *scope_table;
        unsigned char           scope_index;
        unsigned char           unwindflag;
        unsigned char           unwind_index;
        unsigned char           slack3;
        struct _EXCEPTION_RECORD *exception_info;
        struct _CONTEXT         *context_info;
};

*/

#if ( _CPU != 8086 )

/*
 * next definition define field offsets in structure above
 * see CRTL startup directory for seh386.asm
 */

#define TRY_BLOCK_SIZE      28

#define FLD_next            0
#define FLD_scope_table     12
#define FLD_scope_index     16
#define FLD_unwindflag      17
#define FLD_exception_info  20
#define FLD_context_info    24

#endif
