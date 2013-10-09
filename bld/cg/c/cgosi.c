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
* Description:  User I/O interface for OSI.
*
****************************************************************************/


#include <stdlib.h>
#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "cg.h"
#include "feprotos.h"

extern  char            *_BreakFlagPtr;
static  int             TicCount;
bool                    BlipsOn;


int TBreak( void )
/****************/
{
    int         brk;

    brk = *_BreakFlagPtr;
    *_BreakFlagPtr = 0;
    return( brk );
}

void CauseTBreak( void )
/**********************/
{
    *_BreakFlagPtr = 1;
}

void FatalError( const char * str )
/*********************************/
{
     ScratchObj();
     FEMessage( MSG_FATAL, (pointer)str );
}

unsigned GetTickCnt( void )
{
    return( TicCount );
}

void Blip( unsigned short loc, char ch )
{
    loc = loc;
    ch = ch;
}

void BlipInit( void )
{
}

void    FiniBlip( void ) { }


void    InitBlip( void )
{
    BlipsOn = FALSE;
}

bool    WantZoiks2( void )
{
    return( FALSE );
}

static  void    CheckEvents( void )
{
    if( *_BreakFlagPtr ) {              // if( TBreak() )
        *_BreakFlagPtr = 0;
        FatalError( "Program interrupted from keyboard" );
    }
}

void    LNBlip( source_line_number num )
{
    num = num;
    CheckEvents();
}

void    PGBlip( char *name )
{
    name = name;
    CheckEvents();
}

void    URBlip() { CheckEvents(); }
void    TGBlip() { CheckEvents(); }
void    LPBlip() { CheckEvents(); }
void    SXBlip() { CheckEvents(); }
void    EXBlip() { CheckEvents(); }
void    GRBlip() { CheckEvents(); }
void    IMBlip() { CheckEvents(); }
void    SCBlip() { CheckEvents(); }
void    PSBlip() { CheckEvents(); }
void    PLBlip() { CheckEvents(); }
void    DGBlip() { CheckEvents(); }
