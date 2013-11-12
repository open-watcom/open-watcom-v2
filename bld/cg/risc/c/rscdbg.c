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


#include "cgstd.h"
#include "coderep.h"
#include "pattern.h"
#include "procdef.h"
#include "cgdefs.h"
#include "model.h"
#include "ocentry.h"
#include "objrep.h"
#include "zoiks.h"
#include "cgaux.h"
#include "far.h"
#include "typedef.h"
#include "dbgstrct.h"
#include "dbcue.h"

extern  uint    DBSrcFile( const char *fname )
/********************************************/
{
    fname = fname;
    return( 0 );
}

extern  dbg_loc         DBLocInit() {
/***********************************/

    return( NULL );
}

extern dbg_loc          DBLocSym( dbg_loc loc, cg_sym_handle sym ) {
/****************************************************************/

    sym = sym;
    return( loc );
}

extern  dbg_loc         DBLocOp(dbg_loc loc, dbg_loc_op op, unsigned other) {
/***************************************************************************/

    op = op;
    other = other;
    return( loc );
}

extern  dbg_loc         DBLocConst( dbg_loc loc, unsigned_32 val ) {
/******************************************************************/

    val = val;
    return( loc );
}

extern  dbg_type        DBBasedPtr( cg_type ptr_type, dbg_type base, dbg_loc loc_segment ) {
/******************************************************************************************/

    ptr_type = ptr_type;
    loc_segment = loc_segment;
    return( base );
}

extern  void            DBLocFini( dbg_loc loc ) {
/************************************************/

    loc = loc;
}

extern  dbg_type        DBIntArrayCG( cg_type tipe, unsigned_32 hi, dbg_type base ) {
/***********************************************************************************/

    tipe = tipe;
    hi = hi;
    return( base );
}

struct_list     *DBBegNameStruct( const char *nm, cg_type tipe, bool is_struct ) {
/********************************************************************************/

    nm = nm;
    tipe = tipe;
    is_struct = is_struct;
    return( NULL );
}

extern  dbg_type      DBStructForward( struct_list  *st ) {
/*********************************************************/
    return( 0 );
}

extern  void    DBSrcCue( uint fno, uint line, uint col ) {
/*********************************************************/

    fno = fno;
    line = line;
    col = col;
}

extern  void    DBAllocReg( name *reg, name *temp ) {
/***************************************************/
    reg = reg;
    temp = temp;
}

extern  void    InitDbgInfo() {
/******************************/
}

extern  void    FiniDbgInfo() {
/******************************/
}
