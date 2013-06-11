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


#ifndef DWCNF_H_INCLUDED
#define DWCNF_H_INCLUDED

/*
    dw_sym_handle has a client defined meaning; the DW library will pass these
    back to the client in CLIRelocs for DW_W_STATIC
*/
typedef void            *dw_sym_handle;


/*
    the contents of dw_targ_addr is unimportant to the DW library; it is
    only used for sizeof( dw_targ_addr ).
*/
typedef uint_32         dw_targ_addr;
typedef uint_16         dw_targ_seg;


/*
    dw_addr_offset is the type used for offsets from some base address.
    Such as the start_scope parameter to typing routines, or the addr
    parameter to line number information.
    The code assumes this is an unsigned integer type.
*/
typedef uint_32         dw_addr_offset;
/*
    dw_addr_delta is an integer type that can hold the largest possible
    difference between the addr parameter for two subsequent calls to
    DWLineNum.
*/
typedef int_32          dw_addr_delta;


/*
    dw_linenum is a line number.  It must be an unsigned integer type.
    dw_linenum_delta is a type that can hold the largest possible
    difference between two adjacent line numbers passed to DWLineNum.
*/
typedef uint_32         dw_linenum;
typedef int_32          dw_linenum_delta;


/*
    dw_column is a column number.  It must be an unsigned integer type.
*/
typedef uint_16         dw_column;
typedef int             dw_column_delta;


/*
    dw_size_t is used for sizes of various things such as block constants
    (i.e. for DWAddConstant)
*/
typedef uint_32         dw_size_t;


/*
    dw_uconst is an unsigned integer constant of maximum size
    dw_uconst is a signed integer constant of maximum size
*/
typedef uint_32         dw_uconst;
typedef int_32          dw_sconst;


#endif
