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


/* walk program statements */

typedef struct { /* current state of stmt prog */
    uint_32 offset;
    uint_32 line;
    uint_16 col;
    uint_16 file;
    uint_16 seg;
    uint_8  is_stmt   :1;
    uint_8  basic_blk :1;
    uint_8  end_seq   :1;
    uint_8  addr_set  :1;
} dr_line_data;

/* walk directory and  file names */
typedef struct {
    uint_16  index;
    char*    name;
    uint_16  dir;
    uint_32  time;
    uint_32  len;
} dr_line_file;

typedef struct {
    uint_16  index;
    char*    name;
} dr_line_dir;

typedef bool (*DRCUEWLK)( void *, dr_line_data * );
typedef bool (*DRLFILEWLK)( void *, dr_line_file * );
typedef bool (*DRLDIRWLK)( void *, dr_line_dir * );

/* get stmt list from ccu, if none return NULL */
extern dr_handle    DRGetStmtList( dr_handle ccu );

/* walk lines given statement list handle */
extern bool         DRWalkLines( dr_handle, uint_16, DRCUEWLK, void * );

/* walk files given statement list handle */
extern bool         DRWalkLFiles( dr_handle, DRLFILEWLK, void *, DRLDIRWLK, void * );
