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


typedef struct sf_parm {
    struct sf_parm      *link;  // next parm -- must be first field!
    sym_id              shadow; // pointer to shadow entry for parm
    sym_id              actual; // pointer to actual entry for parm
} sf_parm;

typedef struct parameter {
    struct parameter    *link;  // next parm -- must be first field!
    sym_id              id;     // symbol table id
    unsigned_8          flags;  // flags
} parameter;

#define ARG_STMTNO      0x01    // argument is "*"
#define ARG_DUPLICATE   0x02    // argument appeared in previous argument list

typedef struct entry_pt {
    struct parameter    *parms; // parameter list --  must be first field!
    sym_id              id;     // symbol table id
    struct entry_pt     *link;  // next entry point
    unsigned_32         dbh;    // Browsing info handle
} entry_pt;
