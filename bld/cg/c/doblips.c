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
* Description:  Output "blips".
*
****************************************************************************/


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "utils.h"
#include "onexit.h"
#include "qtimer.h"
#include "blips.h"
#include "feprotos.h"

#define Version         "WATCOM Code Generator --------"

#ifdef __DOS__
extern  int             OSCall(void);
 #pragma aux OSCall = 0xb4 0x30 0xcd 0x21 value [eax] modify [eax ebx ecx];
#else
 static void OSCall(void){}
#endif

extern  bool            GetEnvVar(const char *,char *,int);

static uint             LastBlipCount;
static uint             NextTickCount;
static uint             NextBlipCount;

#define MAX_FNAME_LEN   20
#define BLPos           (0)
#define DGPos           (BLPos+78)
#define PLPos           (DGPos-2)
#define PSPos           (PLPos-2)
#define SCPos           (PSPos-2)
#define IMPos           (SCPos-2)
#define GRPos           (IMPos-2)
#define EXPos           (GRPos-2)
#define SXPos           (EXPos-2)
#define LPPos           (SXPos-2)
#define URPos           (LPPos-2)
#define TGPos           (URPos-2)
#define PGPos           (TGPos-MAX_FNAME_LEN)
#define LNSize          5
#define LNPos           (PGPos-LNSize-1)
#define BRK_CHECK_TICKS 18              /* 1 sec.*/
#define BLIP_TICKS      (4*BRK_CHECK_TICKS)     /* 4 sec.*/

static    int           TGCount;
static    int           LPCount;
static    int           URCount;
static    int           SXCount;
static    int           SCCount;
static    int           EXCount;
static    int           GRCount;
static    int           IMCount;
static    int           PSCount;
static    int           PLCount;
static    int           DGCount;
static    bool          Zoiks2;


bool                    BlipsOn;

static  void    DoBlip( int *count, uint pos, char ch )
/*****************************************************/
{
    if( ( ++(*count) & 1 ) != 0 ) {
        Blip( pos + 1, ' ' );
        Blip( pos    , ch );
    } else {
        Blip( pos    , ' ' );
        Blip( pos + 1, ch );
    }
}


static  void    SetNextTickCount( void )
/**************************************/
{
    NextTickCount = NextTickCount + BRK_CHECK_TICKS;
    if( NextTickCount < BRK_CHECK_TICKS )  NextTickCount = ~0;
}


static  void    SetNextBlipCount( void )
/**************************************/
{
    NextBlipCount = LastBlipCount + BLIP_TICKS;
    if( NextBlipCount < BLIP_TICKS )  NextBlipCount = ~0;
}


static  void    CheckEvents( void )
/*********************************/
{
    uint        ticks;

    ticks = GetTickCnt();
    if( ticks < LastBlipCount || ticks >= NextTickCount ) {
        OSCall();       /* force a DOS call to get version number */
        if( ticks < LastBlipCount || ticks >= NextBlipCount ) {
            FEMessage( MSG_BLIP, NULL );
            LastBlipCount = ticks;
            SetNextBlipCount();
        }
        SetNextTickCount();
    }
    if( TBreak() ) {
        FatalError( "Program interrupted from keyboard" );
    }
}

void    FiniBlip( void )
/**********************/
{
}


void    InitBlip( void )
/**********************/
{
    char        buff[80];

    LastBlipCount = GetTickCnt();
    NextTickCount = LastBlipCount;
    SetNextTickCount();
    SetNextBlipCount();
    Zoiks2 = false;
    BlipsOn = false;
#if defined( __NT__ ) || defined( __OSI__ )
    buff[ 0 ] = 0;
#else
    if( GetEnvVar( "WCGBLIPON", buff, 9 ) ) {
        BlipInit();
        BlipsOn = true;
        if( Length( buff ) == 7 && Equal( buff, "ALLERRS", 7 ) ) {
            Zoiks2 = true;
        }
    }
#endif
}

bool    WantZoiks2( void )
/************************/
{
    return( Zoiks2 );
}


void    LNBlip( source_line_number num )
/**************************************/
{
    int         i;
    char        ch;

    CheckEvents();
    if( BlipsOn ) {
        Blip( LNPos+LNSize, ' ' );
        for( i = LNSize; i-- > 0; ) {
            if( num == 0 ) {
                ch = ' ';
            } else {
                ch = num % 10 + '0';
                num /= 10;
            }
            Blip( LNPos + i, ch );
        }
    }
}


void    PGBlip( const char *name )
/********************************/
{
    int         count;

    CheckEvents();
    if( BlipsOn ) {
        for( count = 0; *name != NULLCHAR && count < MAX_FNAME_LEN; ++count, ++name) {
            Blip( count + PGPos, *name );
        }
        for( ; count < MAX_FNAME_LEN; ++count ) {
            Blip( count + PGPos, ' ' );
        }
        for( count = PGPos; count-- > BLPos; ) {
            Blip( count, ' ' );
        }
        for( count = 0; Version[count] != NULLCHAR; ++count ) {
            Blip( count, Version[count] );
        }
    }
}


void    TGBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &TGCount, TGPos, 'T' );
    }
}

void    LPBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &LPCount, LPPos, 'L' );
    }
}

void    URBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &URCount, URPos, 'U' );
    }
}

void    SXBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &SXCount, SXPos, 'X' );
    }
}

void    EXBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &EXCount, EXPos, 'E' );
    }
}

void    GRBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &GRCount, GRPos, 'R' );
    }
}

void    IMBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &IMCount, IMPos, 'M' );
    }
}

void    SCBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &SCCount, SCPos, 'S' );
    }
}

void    PSBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &PSCount, PSPos, 'O' );
    }
}

void    PLBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &PLCount, PLPos, 'o' );
    }
}

void    DGBlip( void )
/********************/
{
    CheckEvents();
    if( BlipsOn ) {
        DoBlip( &DGCount, DGPos, 'D' );
    }
}
