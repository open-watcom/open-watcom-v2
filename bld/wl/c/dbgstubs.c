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

void DBIInit( void )
/*************************/
{
}

void DBISectInit( section *sect )
/**************************************/
{
    sect = sect;
}

void DBIInitModule( mod_entry *obj )
/*****************************************/
{
    obj = obj;
}

void DBIP1Source( byte *buff, byte *endbuff )
/**************************************************/
{
    buff = buff;
    endbuff = endbuff;
}

void DBIP2Source( byte *buff, byte *endbuff )
/**************************************************/
{
    buff = buff;
    endbuff = endbuff;
}

section * DBIGetSect( char *clname )
/*****************************************/
{
    clname = clname;
    return NULL;
}

void DBIColClass( class_entry *class )
/*******************************************/
{
    class = class;
}

unsigned_16 DBIColSeg( class_entry *class )
/************************************************/
{
    class = class;
    return 0;
}

void DBIP1ModuleScanned( void )
/************************************/
{
}

bool DBISkip( seg_leader *seg )
/*************************************/
{
    seg = seg;
    return FALSE;
}

bool DBINoReloc( seg_leader *seg )
/****************************************/
{
    seg = seg;
    return FALSE;
}

void DBIP1ModuleFinished( mod_entry *obj )
/***********************************************/
{
     obj = obj;
}

void DBIComment( void )
/****************************/
{
}

void DBIAddModule( mod_entry *obj, section *sect )
/*******************************************************/
{
    obj = obj;
    sect = sect;
}

void DBIGenModule( void )
/*********************************/
{
}

void DBIDefClass( class_entry *cl, unsigned_32 size )
/**********************************************************/
{
    cl = cl;
    size = size;
}

void DBIAddLocal( seg_leader *seg, offset length )
/**********************************************************/
{
    seg = seg;
    length = length;
}

void DBIGenLocal( seg_leader * leader, offset length )
/***********************************************************/
{
    leader = leader;
    length = length;
}

void DBIAddGlobal( symbol *sym, unsigned add )
/*****************************************************/
{
    sym = sym;
    add = add;
}

void DBIGenGlobal( symbol * sym, section *sect )
/********************************************************/
{
    sym = sym;
    sect = sect;
}

void DBIAddLines( seg_leader * seg, bool is32bit, bool issdata )
/***********************************************************************/
{
    seg = seg;
    is32bit = is32bit;
    issdata = issdata;
}

void DBIStoreLines( unsigned_32 size )
/*******************************************/
{
    size = size;
}

void DBIGenLines( seg_leader * seg, bool is32bit )
/*******************************************************/
{
    seg = seg;
    seg2 = seg2;
    adjust = adjust;
    is32bit = is32bit;
}

void DBIAddAddrInfo( seg_leader *seg, offset oldsize )
/***********************************************************/
{
    seg = seg;
    oldsize = oldsize;
}

void DBIGenAddrInfo( seg_leader *seg, offset oldsize, unsigned_32 pad,
                            section *sect )
/**************************************************************************/
{
    seg = seg;
    oldsize = oldsize;
    pad = pad;
    sect = sect;
}

virt_mem DBIAlloc( unsigned long size )
/********************************************/
{
    size = size;
    return( 0 );
}

void DBIAddrStart( void )
/******************************/
{
}

void DBIAddrSectStart( section * sect )
/********************************************/
{
    sect = sect;
}

void DBIP2Start( section *sect )
/*************************************/
{
    sect = sect;
}

void DBIFini( section *sect )
/**********************************/
{
    sect = sect;
}

void DBISectCleanup( section *sect )
/*****************************************/
{
    sect = sect;
}

void DBICleanup( void )
/****************************/
{
}

void DBIWrite( void )
/**************************/
{
}
