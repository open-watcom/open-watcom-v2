/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Execution sampler data and functions.
*
****************************************************************************/


#define MKMKSTR( data ) # data
#define MKSTR( data )   MKMKSTR( data )

#define PREFIX_STRING   "WATCOM Profiler data file V" \
                        MKSTR( SAMP_MAJOR_VER ) "." MKSTR( SAMP_MINOR_VER ) \
                        "\r\n\x1a"


#define LO_WORD( lng )      (unsigned)( lng )

#define MN_CEIL         1U
#define MX_CEIL         63U     /* in kilobytes (must never exceed 63k) */
#define DEF_CEIL        MX_CEIL

#define TIMER0          0x40    /* timer port */
#define INT_CTRL        0x20    /* interrupt controler port */
#define EOI             0x20    /* End-Of-Interrupt */

#if defined( __DOS__ ) && !defined( __DOS4G__ ) && !defined( __PHARLAP__ )
#define REPORT_TYPE __interrupt
#else
#define REPORT_TYPE
#endif

typedef unsigned short seg;
#ifdef SHORT_OFF
typedef unsigned short off;
#else
typedef unsigned long off;
#endif

typedef struct {
    off         offset;
    seg         segment;
} seg_offset;


typedef struct info_struct {
    struct  samp_block_prefix   pref;
    struct  samp_info           d;
} info_struct;

typedef struct comm_region {
    unsigned char       in_hook;
    unsigned long       cgraph_top;
    off                 top_ip;
    seg                 top_cs;
    unsigned short      pop_no;
    unsigned short      push_no;
} comm_region;

#ifdef __WINDOWS__
typedef struct {
    bool                TaskStarted;
    bool                TaskEnded;
    long                SampOffset;
    int                 SampleHandle;
    WORD                SampleIndex;
    WORD                SampleCount;
    WORD                LastSampleIndex;
    DWORD               CurrTick;
    bool                FarWriteProblem;
    info_struct         Info;
    bool                LostData;
    char                SampName[256];
    bool                ShopClosed;
} shared_data;

#include "wdebug.h"
#include "winintrf.h"

#endif

#include "sampglob.h"

extern void REPORT_TYPE report( void );
extern void             fatal( void );

extern void             Output( const char FAR_PTR * );
extern void             OutputNL( void );
extern void             OutputMsg( int msg );
extern void             OutputMsgNL( int msg );
extern void             OutputMsgParmNL( int msg, const char FAR_PTR *str );
extern void             OutputMsgCharNL( int msg, char chr );
extern unsigned         GetNumber( unsigned, unsigned, const char **, unsigned );
extern void             StopAndSave( void );
extern void             WriteCodeLoad( seg_offset, const char *, samp_block_kinds );
extern void             WriteAddrMap( seg, seg, off );
extern void             AllocSamples( unsigned );
extern void             RecordCGraph( void );
extern int              sample_main( char * );

extern int              SampCreate( const char * );
extern int              SampWrite( const void FAR_PTR *, size_t );
extern int              SampSeek( unsigned long );
extern int              SampClose( void );
extern void             SaveSamples( void );

extern void             StartProg( const char *cmd, const char *prog, const char *full_args, char *dos_args );
extern void             StopProg( void );
extern void             GetProg( const char *cmd, size_t len );
#if defined( __DOS__ ) && !defined( __PHARLAP__ ) && !defined( __DOS4G__ )
extern bool             __near VersionCheck( void );
extern void             __near WriteMark( const char FAR_PTR *str, seg_offset where );
#else
extern bool             VersionCheck( void );
extern void             WriteMark( const char FAR_PTR *str, seg_offset where );
#endif

extern void             FAR_PTR *my_alloc( size_t size );
extern void             my_free( void FAR_PTR *chunk );

extern void             InitTimerRate( void );
extern void             SetTimerRate( const char ** );
extern unsigned long    TimerRate( void );
extern unsigned         SafeMargin( void );
extern unsigned         NextThread( unsigned );
extern void             ResetThread( unsigned );

extern void             SysInit( void );
extern void             SysOptions( void );
extern void             SysExplain( void );
extern void             SysDefaultOptions( void );
extern void             SysParseOptions( char, const char ** );

extern void             GetCommArea( void );
extern void             GetNextAddr( void );
extern void             ResetCommArea( void );
