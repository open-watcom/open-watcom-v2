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
  Mem : memory management routines for fcenable

  Modified: By:             Reason:
  --------- ---             -------
  86/08/06  T.G.Galvin      Initial implementation
  28-mar-88 A.F.Scian       converted to C
  22-jun-89 Jim Randall     added tryalloc, realloc & dump objcache when needed
  22-aug-89 Jim Randall     swiped from wlink.
  22-feb-90 Jim Randall     swiped from ms2wlink.
  27-mar-90 Jim Randall     swiped from tdcvt.  (nice history, eh?)
*/
#include <stdlib.h>
#include <malloc.h>

#ifdef TRACKER
extern void *TrMemInit(char *,void * (*)(int ),void * (*)(void *, int),
                       void (*)(void *), void (*)(char *, unsigned));
extern void TrCheck(void *);
extern void TrMemFini(void *);
extern void TrPrtUsage(void *);
extern void TrPrtMemUse(void *);
extern void *TrAlloc(unsigned int ,void (*)(void),void *);
extern int  TrValidate(void *,void (*)(void),void *);
extern int  TrChkRange(void *,unsigned int ,void (*)(void),void *);
extern int  TrFree(void *,void *);
extern int  TrFreeSize(void *,unsigned int ,void *);

extern void (*FindRet(void))();
#pragma aux FindRet modify [sp];

extern void putlen( char *, unsigned );
extern void put( char * );

void    *TrHdl;
#endif

extern void Error( char * );

#ifdef TRACKER

extern void PrintLine( char *buff, unsigned len )
/***********************************************/
{
    putlen( buff, len );
    put( "\n" );
}
#endif


extern void MemInit( void )
/*************************/
{
#ifdef TRACKER
    TrHdl = TrMemInit( "FCE", malloc, realloc, free, PrintLine );
#endif
}


extern void MemFini( void )
/*************************/
{
#ifdef TRACKER
    TrPrtMemUse( TrHdl );
    TrMemFini( TrHdl );
#endif
}

#ifdef TRACKER
extern void *TryAlloc( unsigned size )
{
    extern void *DoLAlloc( unsigned, void (*)() );
    void        (*ra)();

    ra = FindRet(); /* must be first thing */

    return( DoLAlloc( size, ra ) );
}

void *DoLAlloc( unsigned size, void (*ra)() )
#else
extern void *TryAlloc( unsigned size )
#endif
{
    void    *p;

#ifdef TRACKER
    p = TrAlloc( size, ra, TrHdl );
#else
    p = malloc( size );
#endif
    return( p );
}

extern void * MemAlloc( unsigned size )
/*************************************/
{
    void                *ptr;
#ifdef TRACKER
    void                (*ra)();

    ra = FindRet(); /* must be first thing */

    ptr = DoLAlloc( size, ra );
#else
    ptr = TryAlloc( size );
#endif
    if( ptr == NULL ) {
        Error( "Dynamic Memory Exhausted!!!" );
    }
    return( ptr );
}

extern void MemFree( void *p )
/****************************/
{
    if( p == NULL ) return;
#ifdef TRACKER
    TrFree( p, TrHdl );
#else
    free( p );
#endif
}
