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
* Description:  Execution sampler data and functions.
*
****************************************************************************/


#define MKMKSTR( data ) # data
#define MKSTR( data )   MKMKSTR( data )

#define PREFIX_STRING   "WATCOM Profiler data file V" \
                        MKSTR( SAMP_MAJOR_VER ) "." MKSTR( SAMP_MINOR_VER ) \
                        "\r\n\x1a"


#define LO_WORD( lng )      (unsigned)( lng )

#define MN_CEIL     1
#define MX_CEIL     63          /* in kilobytes (must never exceed 63k) */
#define DEF_CEIL    MX_CEIL

#define TIMER0          0x40    /* timer port */
#define INT_CTRL        0x20    /* interrupt controler port */
#define EOI             0x20    /* End-Of-Interrupt */

#if defined( __DOS__ ) && !defined( _RSI ) && !defined( _PLS )
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
#endif

#include "sampglob.h"

extern void REPORT_TYPE report( void );
extern void             fatal( void );

extern void             Output( char FAR_PTR * );
extern unsigned         GetNumber( unsigned, unsigned, char **, unsigned );
extern void             StopAndSave( void );
extern void             WriteCodeLoad( seg_offset, char *, samp_block_kinds );
extern void             WriteAddrMap( seg, seg, off );
extern void             AllocSamples( unsigned );
extern void             RecordCGraph( void );

extern int              SampCreate( char * );
extern int              SampWrite( void FAR_PTR *, unsigned );
extern int              SampSeek( unsigned long );
extern int              SampClose( void );
extern void             SaveSamples( void );

extern void             StartProg( char *cmd, char *prog, char *full_args, char *dos_args );
extern void             StopProg( void );
extern void             GetProg( char *,char * );
#if defined( __DOS__ ) && !defined( _PLS ) && !defined( _RSI )
extern bool             __near VersionCheck( void );
extern void             __near WriteMark( char FAR_PTR *str, seg_offset where );
#else
extern bool             VersionCheck( void );
extern void             WriteMark( char FAR_PTR *str, seg_offset where );
#endif
extern void             Usage( void );

extern void             FAR_PTR *my_alloc( int size );
extern void             my_free( void FAR_PTR *chunk );

extern void             InitTimerRate( void );
extern void             SetTimerRate( char ** );
extern unsigned long    TimerRate( void );
extern unsigned         SafeMargin( void );
extern unsigned         NextThread( unsigned );

extern void             SysInit( void );
extern void             SysOptions( void );
extern void             SysExplain( void );
extern void             SysDefaultOptions( void );
extern void             SysParseOptions( char, char ** );

extern void             GetCommArea( void );
extern void             GetNextAddr( void );
extern void             ResetCommArea( void );
