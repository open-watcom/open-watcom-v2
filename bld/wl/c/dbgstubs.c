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
 *  DBGSTUBS -- stubs for removing debugging information support
 *
*/

#include "linkstd.h"
#include "dbgall.h"

extern void DBIInit( void )
/*************************/
{
}

extern void DBISectInit( section *sect )
/**************************************/
{
    sect = sect;
}

extern void DBIInitModule( mod_entry *obj )
/*****************************************/
{
    obj = obj;
}

extern void DBIP1Source( byte *buff, byte *endbuff )
/**************************************************/
{
    buff = buff;
    endbuff = endbuff;
}

extern void DBIP2Source( byte *buff, byte *endbuff )
/**************************************************/
{
    buff = buff;
    endbuff = endbuff;
}

extern section * DBIGetSect( char *clname )
/*****************************************/
{
    clname = clname;
    return NULL;
}

extern void DBIColClass( class_entry *class )
/*******************************************/
{
    class = class;
}

extern unsigned_16 DBIColSeg( class_entry *class )
/************************************************/
{
    class = class;
    return 0;
}

extern void DBIP1ModuleScanned( void )
/************************************/
{
}

extern bool DBISkip( unsigned_16 info )
/*************************************/
{
    info = info;
    return FALSE;
}

extern bool DBINoReloc( unsigned_16 info )
/****************************************/
{
    info = info;
    return FALSE;
}

extern void DBIP1ModuleFinished( mod_entry *obj )
/***********************************************/
{
     obj = obj;
}

extern void DBIComment( void )
/****************************/
{
}

extern void DBIAddModule( mod_entry *obj, section *sect )
/*******************************************************/
{
    obj = obj;
    sect = sect;
}

extern void DBIGenModule( void )
/*********************************/
{
}

extern void DBIDefClass( class_entry *cl, unsigned_32 size )
/**********************************************************/
{
    cl = cl;
    size = size;
}

extern void DBIAddLocal( unsigned_16 info, offset length )
/**********************************************************/
{
    info = info;
    length = length;
}

extern void DBIGenLocal( seg_leader * leader, offset length )
/***********************************************************/
{
    leader = leader;
    length = length;
}

extern void DBIAddGlobal( symbol *sym, unsigned add )
/*****************************************************/
{
    sym = sym;
    add = add;
}

extern void DBIGenGlobal( symbol * sym, section *sect )
/********************************************************/
{
    sym = sym;
    sect = sect;
}

extern void DBIAddLines( seg_leader * seg, bool is32bit, bool issdata )
/***********************************************************************/
{
    seg = seg;
    is32bit = is32bit;
    issdata = issdata;
}

extern void DBIStoreLines( unsigned_32 size )
/*******************************************/
{
    size = size;
}

extern void DBIGenLines( seg_leader * seg, bool is32bit )
/*******************************************************/
{
    seg = seg;
    seg2 = seg2;
    adjust = adjust;
    is32bit = is32bit;
}

extern void DBIAddAddrInfo( seg_leader *seg, offset oldsize )
/***********************************************************/
{
    seg = seg;
    oldsize = oldsize;
}

extern void DBIGenAddrInfo( seg_leader *seg, offset oldsize, unsigned_32 pad,
                            section *sect )
/**************************************************************************/
{
    seg = seg;
    oldsize = oldsize;
    pad = pad;
    sect = sect;
}

extern virt_mem DBIAlloc( unsigned long size )
/********************************************/
{
    size = size;
    return( 0 );
}

extern void DBIAddrStart( void )
/******************************/
{
}

extern void DBIAddrSectStart( section * sect )
/********************************************/
{
    sect = sect;
}

extern void DBIP2Start( section *sect )
/*************************************/
{
    sect = sect;
}

extern void DBIFini( section *sect )
/**********************************/
{
    sect = sect;
}

extern void DBISectCleanup( section *sect )
/*****************************************/
{
    sect = sect;
}

extern void DBICleanup( void )
/****************************/
{
}

extern void WriteDBI( void )
/**************************/
{
}
