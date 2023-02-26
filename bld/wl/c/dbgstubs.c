/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
    (void)sect;
}

void DBIInitModule( mod_entry *obj )
/*****************************************/
{
    (void)obj;
}

void DBIP1Source( byte *buff, byte *endbuff )
/**************************************************/
{
    (void)buff;
    (void)endbuff;
}

void DBIP2Source( byte *buff, byte *endbuff )
/**************************************************/
{
    (void)buff;
    (void)endbuff;
}

section * DBIGetSect( const char *classname )
/*******************************************/
{
    (void)classname;
    return( NULL );
}

void DBIColClass( class_entry *class )
/*******************************************/
{
    (void)class;
}

unsigned_16 DBIColSeg( class_entry *class )
/************************************************/
{
    (void)class;
    return( 0 );
}

void DBIP1ModuleScanned( void )
/************************************/
{
}

bool DBISkip( seg_leader *seg )
/*************************************/
{
    (void)seg;
    return( false );
}

bool DBINoReloc( seg_leader *seg )
/****************************************/
{
    (void)seg;
    return( false );
}

void DBIP1ModuleFinished( mod_entry *obj )
/***********************************************/
{
     (void)obj;
}

void DBIComment( void )
/****************************/
{
}

void DBIAddModule( mod_entry *obj, section *sect )
/*******************************************************/
{
    (void)obj;
    (void)sect;
}

void DBIGenModule( void )
/*********************************/
{
}

void DBIDefClass( class_entry *class, unsigned_32 size )
/**********************************************************/
{
    (void)class;
    (void)size;
}

void DBIAddLocal( seg_leader *seg, offset length )
/**********************************************************/
{
    (void)seg;
    (void)length;
}

void DBIGenLocal( seg_leader * leader, offset length )
/***********************************************************/
{
    (void)leader;
    (void)length;
}

void DBIAddGlobal( symbol *sym, unsigned add )
/*****************************************************/
{
    (void)sym;
    (void)add;
}

void DBIGenGlobal( symbol * sym, section *sect )
/********************************************************/
{
    (void)sym;
    (void)sect;
}

void DBIAddLines( seg_leader * seg, bool is32bit, bool issdata )
/***********************************************************************/
{
    (void)seg;
    (void)is32bit;
    (void)issdata;
}

void DBIStoreLines( unsigned_32 size )
/*******************************************/
{
    (void)size;
}

void DBIGenLines( seg_leader * seg, bool is32bit )
/*******************************************************/
{
    (void)seg;
    (void)seg2;
    (void)adjust;
    (void)is32bit;
}

void DBIAddAddrInfo( seg_leader *seg, offset oldsize )
/***********************************************************/
{
    (void)seg;
    (void)oldsize;
}

void DBIGenAddrInfo( seg_leader *seg, offset oldsize, unsigned_32 pad,
                            section *sect )
/**************************************************************************/
{
    (void)seg;
    (void)oldsize;
    (void)pad;
    (void)sect;
}

virt_mem DBIAlloc( virt_mem_size size )
/*************************************/
{
    (void)size;
    return( 0 );
}

void DBIAddrStart( void )
/******************************/
{
}

void DBIAddrSectStart( section * sect )
/********************************************/
{
    (void)sect;
}

void DBIP2Start( section *sect )
/*************************************/
{
    (void)sect;
}

void DBIFini( section *sect )
/**********************************/
{
    (void)sect;
}

void DBISectCleanup( section *sect )
/*****************************************/
{
    (void)sect;
}

void DBICleanup( void )
/****************************/
{
}

void DBIWrite( void )
/**************************/
{
}
