/***************************************************************************
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
* Description:  Execution Sampler mainline.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#ifdef __WATCOMC__
    #include <process.h>
#else
    #include "clibext.h"
#endif
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "banner.h"
#include "os.h"
#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"
#include "digtypes.h"

#ifdef __WINDOWS__
#include "sampwin.h"
#define Info SharedMemory->Info
#define SampleIndex SharedMemory->SampleIndex
#define SampleCount SharedMemory->SampleCount
#define LastSampleIndex SharedMemory->LastSampleIndex
#define FarWriteProblem SharedMemory->FarWriteProblem
#define CurrTick SharedMemory->CurrTick
#define LostData SharedMemory->LostData
#endif

#define NL      "\r\n"

samp_header             Header = {
                                SAMP_SIGNATURE,
                                SAMP_MAJOR_VER,
                                SAMP_MINOR_VER,
                                sizeof( PREFIX_STRING )
                                };

samp_block_prefix       Last = {
                                0,
                                sizeof( samp_block_prefix ),
                                SAMP_LAST
                               };

bool                    FirstSample = TRUE;
static int              stackSize = 0;


void WriteMark( char FAR_PTR *str, seg_offset where )
{
    struct {
        struct samp_block_prefix    pref;
        struct samp_mark            mark;
    }           proto;
    unsigned    size;
    char        FAR_PTR *ptr;

    SamplerOff++;
    size = 1;
    for( ptr = str; *ptr != '\0'; ++ptr, ++size )
        ;
#if defined(__WINDOWS__)
    proto.pref.tick = WinGetCurrTick();
#else
    proto.pref.tick = CurrTick;
#endif
    proto.pref.length = (sizeof( proto ) - 1) + size;
    proto.pref.kind = SAMP_MARK;
    proto.mark.addr.segment = where.segment;
    proto.mark.addr.offset  = where.offset;
    proto.mark.thread_id = 1;
    Info.d.count[ SAMP_MARK ].size += proto.pref.length;
    Info.d.count[ SAMP_MARK ].number++;
    SampWrite( &proto, sizeof( proto ) - 1 );
    SampWrite( str, size );
    SamplerOff--;
}

void WriteCodeLoad( seg_offset ovl_tbl, char *name, samp_block_kinds kind )
{
    struct {
        struct samp_block_prefix    pref;
        struct samp_code_load       d;
    }                       *code;
    unsigned                len;

    len = strlen( name );
    code = alloca( sizeof( *code ) + len );
    code->pref.tick = 0;
    code->pref.length = sizeof( *code ) + len;
    code->pref.kind = kind;
    code->d.ovl_tab.segment = ovl_tbl.segment;
    code->d.ovl_tab.offset = ovl_tbl.offset;
#ifndef __NETWARE__
    {
    struct  stat            state;

    stat( name, (struct stat *)&state );
    code->d.time_stamp = state.st_mtime;
    }
#else
    code->d.time_stamp = 0;
#endif
    memcpy( code->d.name, name, len + 1 ); /* including NULLCHAR */
    Info.d.count[ kind ].size += code->pref.length;
    Info.d.count[ kind ].number += 1;
    SampWrite( code, code->pref.length );
}


void WriteAddrMap( seg map_start,  seg load_start, off load_offset )
{
    struct {
        struct samp_block_prefix    pref;
        struct samp_addr_map        d;
    }                       addr;

    addr.pref.tick = 0;
    addr.pref.length = sizeof( addr );
    addr.pref.kind = SAMP_ADDR_MAP;
    addr.d.data[0].map.segment = map_start;
    addr.d.data[0].map.offset = 0;
    addr.d.data[0].actual.segment = load_start;
    addr.d.data[0].actual.offset = load_offset;
    Info.d.count[ SAMP_ADDR_MAP ].size += sizeof( addr );
    Info.d.count[ SAMP_ADDR_MAP ].number += 1;
    SampWrite( &addr, sizeof( addr ) );
}


void StopAndSave( void )
/* called from int08_handler, int21_handler, and int28_handler */
{
    /*  We don't want our int08_handler to interfere at this time;
        We are running here on a stolen time.
    */
    SamplerOff++;
    SaveSamples();
    SamplerOff--;
}


void SaveSamples( void )
/* called from StopAndSave, and report */
{
    unsigned size;
    unsigned tid;
    unsigned i, j;
    cgraph_sample xfer;

    tid = 0;
    while( (tid = NextThread( tid )) ) {
        if( !CallGraphMode ) {         /* record actual sample only */
            if( SampleIndex > 0 ) {
                size = sizeof( samp_block_prefix ) + sizeof( struct samp_samples )
                            + (SampleIndex - 1) * sizeof( samp_address );
                Samples->pref.length = size;
                if( SampWrite( Samples, size ) == 0 ) {
                    Info.d.count[ SAMP_SAMPLES ].size += size;
                    Info.d.count[ SAMP_SAMPLES ].number += 1;
                }
                SampleIndex = 0;
            }
        } else {        /* record sample and callgraph information */
            for( i=0; i < SampleIndex; i++ )
            if( SampleCount > 0 ) {
                /* write sample record */
                size = SIZE_PREFIX + SIZE_SAMPLE + SampleCount * SIZE_SAMP_ADDR;
                Samples->pref.length = size;
                Info.d.count[ SAMP_SAMPLES ].size += size;
                Info.d.count[ SAMP_SAMPLES ].number += 1;
                SampWrite( Samples, SIZE_PREFIX + SIZE_SAMPLE );
                for( i = 0, j = 0; i < SampleCount; i++ ) {
                    SampWrite( &Samples->d.sample.sample[ j ], SIZE_SAMP_ADDR );
                    j += ( Samples->d.sample.sample[ j+1 ].offset >> 16 ) + 2;
                }
                /* write callgraph record */
                size = SIZE_PREFIX + SIZE_CALLGRAPH +      /* prefix stuff */
                       SampleCount * SIZE_CGRAPH_SAMPLE +  /* push/pop info */
                       (SampleIndex - 2*SampleCount) *     /* cgraph samples */
                                            SIZE_SAMP_ADDR;
                CallGraph->pref.length = size;
                CallGraph->d.cgraph.number = SampleCount;
                Info.d.count[ SAMP_CALLGRAPH ].size += size;
                Info.d.count[ SAMP_CALLGRAPH ].number += 1;
                SampWrite( CallGraph, SIZE_PREFIX + SIZE_CALLGRAPH);
                for( i = 0, j = 1; i < SampleCount; i++ ) {
                    if( Samples->d.sample.sample[ j ].segment == 0 ) {
                        xfer.pop_n = (uint_16)
                                Samples->d.sample.sample[ j ].offset;
                        xfer.push_n =
                                Samples->d.sample.sample[ j ].offset >> 16;
                        stackSize -= xfer.pop_n;
                        if( stackSize < 0 ) {
                            xfer.pop_n += stackSize;
                        }
                        stackSize += xfer.push_n;
                        SampWrite( &xfer, SIZE_CGRAPH_SAMPLE );
                        SampWrite( &Samples->d.sample.sample[ j + 1 ],
                                                xfer.push_n * SIZE_SAMP_ADDR );
                        j += xfer.push_n + 2;
                    } else {    /* the callgraph info was not available */
                        xfer.pop_n = -1;        /* flag this condition for */
                        xfer.push_n = -1;       /* the profiler. */
                        SampWrite( &xfer, SIZE_CGRAPH_SAMPLE );
                        j += 2;
                    }
                }
                SampleIndex = 0;
                SampleCount = 0;
                LastSampleIndex = 0;
            }
        }
    }
}


void RecordCGraph( void )
/*
 *  Record the callgraph information.  It won't be written in this
 *  form, but it's easiest to store it with the actual samples and
 *  separate the information as it's written.
 */
{
    int         i;

    GetCommArea();

    if( Comm.in_hook ) {        /* we don't really want to sample here */
        SampleIndex--;
        SampleCount--;          /* forget about this sample */
        return;                 /* and quit */
    }

/* first, we record the push/pop values */
    if( FirstSample ) {
        FirstSample = FALSE;
        Comm.push_no++;
    } else if( Comm.push_no != 0 || Comm.pop_no != 0 ) {
        Comm.push_no++;
        Comm.pop_no++;
    }
    Samples->d.sample.sample[ SampleIndex ].segment = 0;
    Samples->d.sample.sample[ SampleIndex ].offset = Comm.push_no;
    Samples->d.sample.sample[ SampleIndex ].offset <<= 16;
    Samples->d.sample.sample[ SampleIndex ].offset += Comm.pop_no;
    SampleIndex++;

/* then we write the cs:ip of the last known routine */
    if( Comm.push_no ) {
        Samples->d.sample.sample[ SampleIndex ].offset
                                    = (off) Comm.top_ip;
        Samples->d.sample.sample[ SampleIndex ].segment
                                    = (seg) Comm.top_cs;
        SampleIndex++;
    }

/* finally, record the necessary callgraph information */
    for( i = 0; i < (int) Comm.push_no - 1; i++ ) {
        GetNextAddr();
        Samples->d.sample.sample[ SampleIndex ].offset = CGraphOff;
        Samples->d.sample.sample[ SampleIndex ].segment = CGraphSeg;
        SampleIndex++;
    }

/* and reset the communication area to prepare for next sample */
    ResetCommArea();
}


static void AllFull( void )
{
    SampClose();
    Output( MsgArray[ERR_DISK_FULL-ERR_FIRST_MESSAGE] );
    Output( "\r\n" );
    fatal();
}


void REPORT_TYPE report( void )
{
    StopProg();
    SaveSamples();

    /* write the header on the sample file */
    Info.d.count[ SAMP_LAST ].number++;
    if( SampWrite( &Last, sizeof( Last ) ) != 0 ) AllFull();
    if( SampWrite( &Header, sizeof( Header ) ) != 0 ) AllFull();
#if defined(__WINDOWS__)
    Info.pref.tick = WinGetCurrTick();
#else
    Info.pref.tick = CurrTick;
#endif
    SampSeek( Header.sample_start );
    SampWrite( &Info, sizeof( Info ) );
    SampClose();
    if( LostData )  {
        Output( MsgArray[ERR_SAMPLES_LOST-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
    }
    if( FarWriteProblem ) {
        Output( MsgArray[ERR_SAMPLE_TRUNCATE-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
#if !defined(__NETWARE__) && !defined(__WINDOWS__)
    MsgFini();
    _exit( 0 );
#endif
}


void Usage( void )
{
    Output(
        banner1w( "Execution Sampler", _WSAMP_VERSION_ ) NL
        banner2 NL
        banner2a( "1989" ) NL
        banner3 NL
        banner3a NL
        NL
    );
    MsgPrintfUsage( MSG_USAGE_LN_1, MSG_USAGE_LN_3 );
//  MSG_USAGE_4 is the option for call graph support
//  (undocumented for now)
    MsgPrintfUsage( MSG_USAGE_LN_5, MSG_USAGE_LN_9 );
#ifndef __WINDOWS__
    fatal();
#endif
}


char *skip( char *ptr )
{
    while( *ptr == ' ' || *ptr == '\t' )  ++ptr;
    return( ptr );
}


unsigned GetNumber( unsigned min, unsigned max, char **atstr, unsigned base )
/* handles command line items of the sort "b=23" (up to base 16) */
{
    char *scan;
    int c;
    unsigned res;
    unsigned value;
    char buff[2];

    scan = skip( *atstr );
    if( scan[0] != '=' && scan[0] != '#' ) {
        Output( MsgArray[MSG_EXPECTING-ERR_FIRST_MESSAGE] );
        buff[0] = scan[0];
        buff[1] = '\0';
        Output( buff );
        Output( "\r\n" );
        fatal();
    }
    scan = skip( &scan[1] );
    res = 0;
    for(;;) {
        c = tolower( *scan );
        if( ! isxdigit( c ) ) break;
        if( isalpha( c ) ) {
            value = ( c - 'a' ) + 10;
        } else {
            value = c - '0';
        }
        res *= base;
        res += value;
        ++scan;
    }
    if( c !='\0' && c !=' ' && c !='\t' ) {
        Output( MsgArray[MSG_INVALID_CHAR-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    if(( res < min ) || ( res > max )) {
        Output( MsgArray[MSG_OUT_OF_RANGE-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    *atstr = scan;
    return( res );
}

#define CNV_CEIL( size )    ((size*1024U                            \
        - (sizeof( struct samp_samples )-sizeof( samp_address )))   \
        / sizeof( samp_address ))


char *Parse( char *line, char arg[], char **eoc )
{
    char        *cmd, *ptr;
    int         c, len;
    char        buff[2];

    InitTimerRate();
    SysDefaultOptions();
    Ceiling = CNV_CEIL( DEF_CEIL );
    cmd = line;
    SampName[0] = '\0';
    for( ;; ) {
        cmd = skip( cmd );
#ifdef __UNIX__
        if( *cmd != '-' ) break;
#else
        if( (*cmd != '/') && (*cmd != '-') ) break;
#endif
        cmd = skip( ++cmd );
        c = *(cmd++);
        c = tolower( c );
        switch( c ) {
#ifndef __WINDOWS__
        case 'b':
            Ceiling = CNV_CEIL( GetNumber( MN_CEIL, MX_CEIL, (char **)&cmd, 10 ) );
            break;
#endif
        case 'c':
            CallGraphMode = TRUE;
            break;
        case 'f':
            if( *cmd != '=' && *cmd != '#' ) {
                Output( MsgArray[MSG_EXPECTING-ERR_FIRST_MESSAGE] );
                buff[0] = *cmd;
                buff[1] = '\0';
                Output( buff );
                Output( "\r\n" );
                fatal();
            }
            ++cmd;
            ptr = SampName;
            while( *cmd != '\t' && *cmd != ' ' && *cmd != '\0' ) {
                *ptr = *cmd;
                ++ptr;
                ++cmd;
            }
            *ptr = '\0';
            break;
        default:
            SysParseOptions( c, (char **)&cmd );
            break;
        }
    }
    if( *cmd == '\0' || *cmd == '?' ) {
        Usage();
#ifdef __WINDOWS__
        fatal();
#endif
    }

    Margin = SafeMargin();

    /* scan over command name */
    ptr = cmd;
    for( ;; ) {
        if( *ptr == ' ' ) break;
#ifndef __UNIX__
        if( *ptr == '/' ) break;
#endif
        if( *ptr == '-' ) break;
        if( *ptr == '\t' ) break;
        if( *ptr == '\0' ) break;
        ++ptr;
    }
    /* collect program arguments - arg will contain DOS-style command tail,
     * possibly truncated (max 126 usable chars).
     */
    *eoc = ptr;
    arg[0] = len = 0;
    if( *ptr != '\0' ) {
        arg[0] = 1;
        arg[1] = *ptr++;
    }
    while( (*ptr != '\0') && (len < 126) ) {
        arg[++len] = *ptr++;
    }
    arg[len + 1] = '\r';
    arg[0] = len;
    return( cmd );
}


void AllocSamples( unsigned tid )
{
    Samples = my_alloc( sizeof( struct samp_samples )
            + Ceiling * sizeof( samp_address ) );
    if( Samples == NULL ) {
        Output( MsgArray[MSG_SAMPLE_BUFF-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    Samples->pref.kind = SAMP_SAMPLES;
    Samples->d.sample.thread_id = tid;
    if( CallGraphMode ) {       /* allocate callgraph prefix storage */
        CallGraph = my_alloc( sizeof( struct samp_block ) );
        if( CallGraph == NULL ) {
            Output( MsgArray[MSG_CALLGRAPH_BUFF-ERR_FIRST_MESSAGE] );
            Output( "\r\n" );
            fatal();
        }
        CallGraph->pref.kind = SAMP_CALLGRAPH;
        CallGraph->d.cgraph.thread_id = tid;
    }
    SampleIndex = 0;
    SampleCount = 0;
    LastSampleIndex = 0;
}

#if defined( __WINDOWS__ )
int sample_main( char far *win_cmd )
#else
int main( int argc, char **argv )
#endif
{
    char        *cmd_line;
    char        *arg;
    char        *cmd;
    char        *tmp_cmd;
    char        *eoc;
    int         cmdlen;

#if !defined( __WINDOWS__ ) && !defined( __WATCOMC__ )
    _argv = argv;
    _argc = argc;
#endif

    SysInit();
#if !defined( __WINDOWS__ )
    if( !MsgInit() )
        fatal();

    /* Command line may be several KB large on most OSes */
    cmdlen = _bgetcmd( NULL, 0 );
    cmd_line = malloc( cmdlen + 1 );
    arg = malloc( cmdlen + 1 );
    if( ( cmd_line == NULL ) || ( arg == NULL ) ) {
        Output( MsgArray[MSG_SAMPLE_BUFF-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    getcmd( cmd_line );
#else
    cmdlen = cmdlen;
    cmd_line = malloc( 256 ); /* Just hope for the best */
    arg = malloc( 256 );
    if( ( cmd_line == NULL ) || ( arg == NULL ) ) {
        Output( MsgArray[MSG_SAMPLE_BUFF-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
    _fstrcpy( cmd_line, win_cmd );
#endif
    tmp_cmd = cmd_line;
    while( *tmp_cmd ) ++tmp_cmd;
    while( *--tmp_cmd == ' ' || *tmp_cmd == '\t' ) ;
    *++tmp_cmd = '\0';
    cmd = Parse( cmd_line, arg, &eoc );    /*
          will set Ceiling, Margin, TimerMult, cmd, and arg
                                 */
    GetProg( cmd, eoc );

    AllocSamples( 1 );

    LostData = FALSE;
    SamplerOff = 0;
    InsiderTime = 0;    /* set non-zero whenever inside an INT 08H */
    FarWriteProblem = FALSE;

    if( !VersionCheck() ) {
        Output( MsgArray[MSG_VERSION-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }

#ifndef __WINDOWS__
    if( SampCreate( SampName ) != 0 ) {
        Output( MsgArray[MSG_SAMPLE_FILE-ERR_FIRST_MESSAGE] );
        Output( "\r\n" );
        fatal();
    }
#endif
    SampWrite( PREFIX_STRING, sizeof( PREFIX_STRING ) );
    Info.pref.length = sizeof( Info );
    Info.pref.kind = SAMP_INFO;
    Info.d.timer_rate = TimerRate();
    /* could do a better job filling in this information */
    Info.d.config.cpu           = 0;
    Info.d.config.fpu           = 0;
    Info.d.config.osmajor       = 0;
    Info.d.config.osminor       = 0;
    Info.d.config.os            = OS_IDUNNO;
    Info.d.config.huge_shift    = 12;
#if defined( _M_IX86 )
    Info.d.config.mad           = MAD_X86;
#elif defined(__ALPHA__)
    Info.d.config.mad           = MAD_AXP;
#elif defined(__PPC__)
    Info.d.config.mad           = MAD_PPC;
#else
    #error Machine type not configured
#endif
    /* record get re-written with other information filled in later */
    SampWrite( &Info, sizeof( Info ) );
    CurrTick  = 0L;
    /* Some systems need a simple null-terminated string, others need
     * a DOS-style 128-byte array with length in the first byte and CR
     * at the end. We pass both and let the callee pick & choose.
     */
    StartProg( cmd, ExeName, eoc, arg );
    MsgFini();
    free( cmd_line );
    free( arg );
    return( 0 );
}
