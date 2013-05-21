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


#include <stdarg.h>
#include "cgstd.h"
#include "coderep.h"
#include "cgdefs.h"

extern  void    DBLineNum( uint no )
{
}

extern  void    DBModSym( pointer sym, cg_type tipe )
{
}

extern  void    DBLocalSym( pointer sym, cg_type tipe )
{
}

extern  void    DBBegBlock( )
{
}

extern  void    DBEndBlock()
{
}

extern  dbg_type        DBFtnType( char *nm, dbg_ftn_type tipe )
{
    return( 0 );
}

extern  dbg_type        DBScalar( char *nm, cg_type tipe )
{
    return( 0 );
}

extern  dbg_type        DBScope( char *nm )
{
    return( 0 );
}

extern  dbg_name        DBBegName( char *nm, dbg_type scope )
{
    return( 0 );
}

extern  dbg_type        DBForward( void *nm )
{
    return( 0 );
}

extern  dbg_type        DBEndName( void *nm, dbg_type tipe )
{
    return( 0 );
}

extern  dbg_type        DBCharBlock( unsigned_32 len )
{
    return( 0 );
}

extern  dbg_type        DBIndCharBlock( bck_info *len, cg_type len_type, int off )
{
    return( 0 );
}

extern  dbg_type        DBDereference( cg_type ptr_type, dbg_type base )
{
    return( 0 );
}

extern  dbg_type        DBFtnArray( bck_info *bk, cg_type lo_bound,
                                  cg_type num_elts, int dim_off,dbg_type base )
                                  {
                                      return( 0 );
                                  }

extern  dbg_type        DBArray( dbg_type idx, dbg_type base )
{
    return( 0 );
}

extern  dbg_type        DBIntArray( unsigned_32 hi, dbg_type base )
{
    return( 0 );
}

extern  dbg_type        DBSubRange( signed_32 lo, signed_32 hi, dbg_type base )
{
    return( 0 );
}

extern  dbg_type        DBPtr( cg_type ptr_tipe, dbg_type base )
{
    return( 0 );
}

extern  void    *DBBegStruct()
{
    return( 0 );
}

extern void DBAddField(void *st,unsigned_32 off,char *nm,dbg_type base)
{
}

extern  void    DBAddBitField(void *st,unsigned_32 off,byte strt,byte len,char *nm,dbg_type base)
{
}

extern  dbg_type        DBEndStruct( void *st )
{
    return( 0 );
}

extern  void    *DBBegEnum( cg_type tipe )
{
    return( 0 );
}

extern  void    DBAddConst( void *en, char *nm, signed_32 val )
{
}

extern  dbg_type        DBEndEnum( void *en )
{
    return( 0 );
}

extern  void    *DBBegProc( cg_type call_type, dbg_type ret )
{
    return( 0 );
}

extern  void    DBAddParm( void *pr, dbg_type tipe )
{
}

extern  dbg_type        DBEndProc( void *pr )
{
    return( 0 );
}

extern  void    DbgParmLoc( name_def *parm ) {
/**********************************************/

}

extern  void    DbgRetLoc() {
/**********************************************/

}

extern  void    FiniSymDebug() {
/**********************************************/

}

extern  void    InitSymDebug() {
/**********************************************/

}
