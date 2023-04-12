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
#endif
#include <sys/types.h>
#include <sys/stat.h>
#ifdef __WINDOWS__
    #include <wwindows.h>
#endif
#include "sample.h"
#include "wmsg.h"
#include "smpstuff.h"
#include "digtypes.h"
#ifdef __WINDOWS__
    #include "sampwin.h"
#endif

#include "clibint.h"


static samp_header          Header = {
    SAMP_SIGNATURE,
    SAMP_MAJOR_VER,
    SAMP_MINOR_VER,
    sizeof( PREFIX_STRING )
};

static samp_block_prefix    Last = {
    0,
    sizeof( samp_block_prefix ),
    SAMP_LAST
};

static int              stackSize = 0;


#if defined( __DOS__ ) && !defined( __PHARLAP__ ) && !defined( __DOS4G__ )
void __near WriteMark( const char FAR_PTR *str, far_address where )
#else
void WriteMark( const char FAR_PTR *str, far_address where )
#endif
{
    struct {
        struct samp_block_prefix    pref;
        struct samp_mark            mark;
    }           proto;
    unsigned    size;
    const char  FAR_PTR *ptr;

    SamplerOff++;
    size = 1;
    for( ptr = str; *ptr != '\0'; ++ptr, ++size )
        ;
#if defined(__WINDOWS__)
    proto.pref.tick = WinGetCurrTick();
#else
    proto.pref.tick = CurrTick;
#endif
    proto.pref.length = ( sizeof( proto ) - 1 ) + size;
    proto.pref.kind = SAMP_MARK;
    proto.mark.addr.segment = where.segment;
    proto.mark.addr.offset  = where.offset;
    proto.mark.thread_id = 1;
    Info.d.count[SAMP_MARK].size += proto.pref.length;
    Info.d.count[SAMP_MARK].number++;
    SampWrite( &proto, sizeof( proto ) - 1 );
    SampWrite( str, size );
    SamplerOff--;
}

void WriteCodeLoad( far_address ovl_tbl, const char *name, samp_block_kinds kind )
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
    Info.d.count[kind].size += code->pref.length;
    Info.d.count[kind].number += 1;
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
    Info.d.count[SAMP_ADDR_MAP].size += sizeof( addr );
    Info.d.count[SAMP_ADDR_MAP].number += 1;
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
    while( (tid = NextThread( tid )) != 0 ) {
        if( !CallGraphMode ) {         /* record actual sample only */
            if( SampleIndex > 0 ) {
                size = sizeof( samp_block_prefix ) + sizeof( struct samp_samples )
                            + (SampleIndex - 1) * sizeof( samp_address );
                Samples->pref.length = size;
                if( SampWrite( Samples, size ) == 0 ) {
                    Info.d.count[SAMP_SAMPLES].size += size;
                    Info.d.count[SAMP_SAMPLES].number += 1;
                }
                ResetThread( tid );
            }
        } else {        /* record sample and callgraph information */
            for( i = 0; i < SampleIndex; i++ ) {
                if( SampleCount > 0 ) {
                    /* write sample record */
                    size = SIZE_PREFIX + SIZE_SAMPLE + SampleCount * SIZE_SAMP_ADDR;
                    Samples->pref.length = size;
                    Info.d.count[SAMP_SAMPLES].size += size;
                    Info.d.count[SAMP_SAMPLES].number += 1;
                    SampWrite( Samples, SIZE_PREFIX + SIZE_SAMPLE );
                    for( i = 0, j = 0; i < SampleCount; i++ ) {
                        SampWrite( &Samples->d.sample.sample[j], SIZE_SAMP_ADDR );
                        j += ( Samples->d.sample.sample[j+1].offset >> 16 ) + 2;
                    }
                    /* write callgraph record */
                    size = SIZE_PREFIX + SIZE_CALLGRAPH +      /* prefix stuff */
                           SampleCount * SIZE_CGRAPH_SAMPLE +  /* push/pop info */
                           (SampleIndex - 2*SampleCount) *     /* cgraph samples */
                                                SIZE_SAMP_ADDR;
                    CallGraph->pref.length = size;
                    CallGraph->d.cgraph.number = SampleCount;
                    Info.d.count[SAMP_CALLGRAPH].size += size;
                    Info.d.count[SAMP_CALLGRAPH].number += 1;
                    SampWrite( CallGraph, SIZE_PREFIX + SIZE_CALLGRAPH);
                    for( i = 0, j = 1; i < SampleCount; i++ ) {
                        if( Samples->d.sample.sample[j].segment == 0 ) {
                            xfer.pop_n = (uint_16)Samples->d.sample.sample[j].offset;
                            xfer.push_n = Samples->d.sample.sample[j].offset >> 16;
                            stackSize -= xfer.pop_n;
                            if( stackSize < 0 ) {
                                xfer.pop_n += stackSize;
                            }
                            stackSize += xfer.push_n;
                            SampWrite( &xfer, SIZE_CGRAPH_SAMPLE );
                            SampWrite( &Samples->d.sample.sample[j + 1],
                                                    xfer.push_n * SIZE_SAMP_ADDR );
                            j += xfer.push_n + 2;
                        } else {    /* the callgraph info was not available */
                            xfer.pop_n = CGRAPH_NO_ENTRY;   /* flag this condition for */
                            xfer.push_n = CGRAPH_NO_ENTRY;  /* the profiler. */
                            SampWrite( &xfer, SIZE_CGRAPH_SAMPLE );
                            j += 2;
                        }
                    }
                    ResetThread( tid );
                }
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
        FirstSample = false;
        Comm.push_no++;
    } else if( Comm.push_no != 0 || Comm.pop_no != 0 ) {
        Comm.push_no++;
        Comm.pop_no++;
    }
    Samples->d.sample.sample[SampleIndex].segment = 0;
    Samples->d.sample.sample[SampleIndex].offset = Comm.push_no;
    Samples->d.sample.sample[SampleIndex].offset <<= 16;
    Samples->d.sample.sample[SampleIndex].offset += Comm.pop_no;
    SampleIndex++;

/* then we write the cs:ip of the last known routine */
    if( Comm.push_no ) {
        Samples->d.sample.sample[SampleIndex].offset  = (off)Comm.top_ip;
        Samples->d.sample.sample[SampleIndex].segment = (seg)Comm.top_cs;
        SampleIndex++;
    }

/* finally, record the necessary callgraph information */
    for( i = 0; i < (int)Comm.push_no - 1; i++ ) {
        GetNextAddr();
        Samples->d.sample.sample[SampleIndex].offset = CGraphOff;
        Samples->d.sample.sample[SampleIndex].segment = CGraphSeg;
        SampleIndex++;
    }

/* and reset the communication area to prepare for next sample */
    ResetCommArea();
}


static void AllFull( void )
{
    SampClose();
    OutputMsgNL( ERR_DISK_FULL );
    fatal();
}


void REPORT_TYPE report( void )
{
    StopProg();
    SaveSamples();

    /* write the header on the sample file */
    Info.d.count[SAMP_LAST].number++;
    if( SampWrite( &Last, sizeof( Last ) ) != 0 )
        AllFull();
    if( SampWrite( &Header, sizeof( Header ) ) != 0 )
        AllFull();
#if defined(__WINDOWS__)
    Info.pref.tick = WinGetCurrTick();
#else
    Info.pref.tick = CurrTick;
#endif
    SampSeek( Header.sample_start );
    SampWrite( &Info, sizeof( Info ) );
    SampClose();
    if( LostData )  {
        OutputMsgNL( ERR_SAMPLES_LOST );
    }
    if( FarWriteProblem ) {
        OutputMsgNL( ERR_SAMPLE_TRUNCATE );
        fatal();
    }
#if !defined(__NETWARE__) && !defined(__WINDOWS__)
    MsgFini();
    _exit( 0 );
#endif
}


static const char *skip( const char *ptr )
{
    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    return( ptr );
}


unsigned GetNumber( unsigned min, unsigned max, const char **atstr, unsigned base )
/* handles command line items of the sort "b=23" (up to base 16) */
{
    const char  *scan;
    int         c;
    unsigned    res;
    unsigned    value;

    scan = skip( *atstr );
    if( scan[0] != '=' && scan[0] != '#' ) {
        OutputMsgCharNL( MSG_EXPECTING, scan[0] );
        fatal();
    }
    scan = skip( &scan[1] );
    res = 0;
    for( ;; ) {
        c = tolower( *scan );
        if( !isxdigit( c ) )
            break;
        if( isalpha( c ) ) {
            value = ( c - 'a' ) + 10;
        } else {
            value = c - '0';
        }
        res *= base;
        res += value;
        ++scan;
    }
    if( c != '\0' && c != ' ' && c != '\t' ) {
        OutputMsgNL( MSG_INVALID_CHAR );
        fatal();
    }
    if(( res < min ) || ( res > max )) {
        OutputMsgNL( MSG_OUT_OF_RANGE );
        fatal();
    }
    *atstr = scan;
    return( res );
}

static const char *skip_command( const char *str )
{
    for( ;; ) {
        switch( *str ) {
        case ' ':
    #ifdef __DOS__
        case '/':
        case '-':
    #endif
        case '\t':
        case '\0':
        case '<':
        case '>':
        case '|':
            return( str );
        default:
            str++;
            break;
        }
    }
}

#define CNV_CEIL( size )    ((size * 1024U                            \
        - (sizeof( struct samp_samples ) - sizeof( samp_address )))   \
        / sizeof( samp_address ))


static const char *Parse( const char *line, char arg[], const char **eoc )
{
    const char  *cmd;
    char        *p;
    const char  *ptr;
    int         c, len;

    InitTimerRate();
    SysDefaultOptions();
    Ceiling = CNV_CEIL( DEF_CEIL );
    cmd = line;
    SampName[0] = '\0';
    for( ;; ) {
        cmd = skip( cmd );
#ifdef __UNIX__
        if( *cmd != '-' )
#else
        if( (*cmd != '/') && (*cmd != '-') )
#endif
            break;
        cmd = skip( ++cmd );
        c = *(cmd++);
        c = tolower( c );
        switch( c ) {
#ifndef __WINDOWS__
        case 'b':
            Ceiling = CNV_CEIL( GetNumber( MN_CEIL, MX_CEIL, &cmd, 10 ) );
            break;
#endif
        case 'c':
            CallGraphMode = true;
            break;
        case 'f':
            if( *cmd != '=' && *cmd != '#' ) {
                OutputMsgCharNL( MSG_EXPECTING, *cmd );
                fatal();
            }
            ++cmd;
            p = SampName;
            while( *cmd != '\t' && *cmd != ' ' && *cmd != '\0' ) {
                *p++ = *cmd;
                cmd++;
            }
            *p = '\0';
            break;
        default:
            SysParseOptions( c, &cmd );
            break;
        }
    }
    if( *cmd == '\0' || *cmd == '?' ) {
        Usage();
        fatal();
    }

    Margin = SafeMargin();

    /* scan over command name */
    ptr = skip_command( cmd );
    /* collect program arguments - arg will contain DOS-style command tail,
     * possibly truncated (max 126 usable chars).
     */
    *eoc = ptr;
    len = 0;
    arg[0] = 0;
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
        OutputMsgNL( MSG_SAMPLE_BUFF );
        fatal();
    }
    Samples->pref.kind = SAMP_SAMPLES;
    Samples->d.sample.thread_id = tid;
    if( CallGraphMode ) {       /* allocate callgraph prefix storage */
        CallGraph = my_alloc( sizeof( struct samp_block ) );
        if( CallGraph == NULL ) {
            OutputMsgNL( MSG_CALLGRAPH_BUFF );
            fatal();
        }
        CallGraph->pref.kind = SAMP_CALLGRAPH;
        CallGraph->d.cgraph.thread_id = tid;
    }
    SampleIndex = 0;
    SampleCount = 0;
    LastSampleIndex = 0;
}

int sample_main( char *cmd_line )
{
    char        *arg;
    const char  *cmd;
    char        *tmp_cmd;
    const char  *eoc;

    arg = NULL;
    if( cmd_line != NULL ) {
        arg = malloc( strlen( cmd_line ) + 1 );
    }
    if( arg == NULL ) {
        OutputMsgNL( MSG_SAMPLE_BUFF );
        return( 1 );
    }

    tmp_cmd = cmd_line;
    while( *tmp_cmd != '\0' )
        ++tmp_cmd;
    while( *--tmp_cmd == ' ' || *tmp_cmd == '\t' )
        ;
    *++tmp_cmd = '\0';
    cmd = Parse( cmd_line, arg, &eoc );    /* will set Ceiling, Margin, TimerMult, cmd, and arg */
    GetProg( cmd, eoc - cmd_line );

    AllocSamples( 1 );

    FirstSample = true;
    LostData = false;
    SamplerOff = 0;
    InsiderTime = 0;    /* set non-zero whenever inside an INT 08H */
    FarWriteProblem = false;

    if( !VersionCheck() ) {
        OutputMsgNL( MSG_VERSION );
        return( 1 );
    }

#ifndef __WINDOWS__
    if( SampCreate( SampName ) != 0 ) {
        OutputMsgNL( MSG_SAMPLE_FILE );
        return( 1 );
    }
#endif
    SampWrite( PREFIX_STRING, sizeof( PREFIX_STRING ) );
    Info.pref.length = sizeof( Info );
    Info.pref.kind = SAMP_INFO;
    Info.d.timer_rate = TimerRate();
    /* could do a better job filling in this information */
    memset( &Info.d.config, 0, sizeof( Info.d.config ) );
    Info.d.config.huge_shift    = 12;
#if defined( _M_IX86 )
    Info.d.config.arch          = DIG_ARCH_X86;
#elif defined( __AXP__ )
    Info.d.config.arch          = DIG_ARCH_AXP;
#elif defined( __PPC__ )
    Info.d.config.arch          = DIG_ARCH_PPC;
#else
    #error Machine type not configured
#endif
    /* record get re-written with other information filled in later */
    SampWrite( &Info, sizeof( Info ) );
    CurrTick = 0L;
    /* Some systems need a simple null-terminated string, others need
     * a DOS-style 128-byte array with length in the first byte and CR
     * at the end. We pass both and let the callee pick & choose.
     */
    StartProg( cmd, ExeName, eoc, arg );
    free( arg );
    return( 0 );
}

#if !defined( __WINDOWS__ )
int main( int argc, char **argv )
{
    char        *cmd_line;
    int         rc;

#if !defined( __WATCOMC__ )
    _argv = argv;
    _argc = argc;
#else
    /* unused parameters */ (void)argc; (void)argv;
#endif

    SysInit();
    if( !MsgInit() )
        fatal();

    /* Command line may be several KB large on most OSes */
    cmd_line = malloc( _bgetcmd( NULL, 0 ) + 1 );
    if( cmd_line != NULL ) {
        getcmd( cmd_line );
    }

    rc = sample_main( cmd_line );

    if( cmd_line != NULL ) {
        free( cmd_line );
    }

    MsgFini();
    return( rc );
}
#endif
