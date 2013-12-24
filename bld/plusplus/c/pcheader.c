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

#include "plusplus.h"

#include <stddef.h>
#include <setjmp.h>
#include <limits.h>
#if defined(__UNIX__)
 #include <dirent.h>
#else
 #include <direct.h>
#endif
#include "wio.h"
#include "sopen.h"
#include "memmgr.h"
#include "carve.h"
#include "hfile.h"
#include "preproc.h"
#include "cgdata.h"
#include "stats.h"
#include "initdefs.h"
#include "name.h"
#include "pcheader.h"
#include "cgiobuff.h"
#include "brinfo.h"

typedef struct pch_reloc_info {
    long        start;
    long        stop;
} pch_reloc_info;

static pch_status (*readFunctions[])( void ) = {
#define PCH_EXEC( s, g )        PCHRead##g,
#include "pcregdef.h"
};
static pch_status (*writeFunctions[])( void ) = {
#define PCH_EXEC( s, g )        PCHWrite##g,
#include "pcregdef.h"
};

static pch_status (*initFunctions[])( boolean ) = {
#define PCH_EXEC( s, g )        PCHInit##g,
#include "pcregdef.h"
};
static pch_status (*finiFunctions[])( boolean ) = {
#define PCH_EXEC( s, g )        PCHFini##g,
#include "pcregdef.h"
};
static pch_status (*relocFunctions[])( char *, unsigned ) = {
#define PCH_RELOC( s, g )       PCHReloc##g,
#include "pcregdef.h"
};

ExtraRptCtr( ctr_pch_length );
ExtraRptCtr( ctr_pch_waste );
ExtraRptTable( ctr_pchw_region, PCHRW_MAX + 1, 1 );

static pch_reloc_info relocInfo[ PCHRELOC_MAX ];

static char *pchName;
static NAME pchDebugInfoName;
static int  pchFile;

#ifndef NDEBUG
#define IO_BUFFER_SIZE  1024
#else
#define IO_BUFFER_SIZE  65536
#endif
static int      amountLeft;
static char     *ioBuffer;
static char     *bufferCursor;
//static char     *bufferEnd;
static long     bufferPosition;

#define pch_buff_cur CompInfo.pch_buff_cursor
#define pch_buff_eob CompInfo.pch_buff_end

static jmp_buf  *abortData;

#ifdef NDEBUG

#define PCHTrashAlreadyRead()

void PCHActivate( void )
/**********************/
{
}

#else

static clock_t start_parse;

static void PCHTrashAlreadyRead( void )
/*************************************/
{
    if( ioBuffer != NULL && ioBuffer < pch_buff_cur ) {
        unsigned amt = pch_buff_cur - ioBuffer;
        DbgZapFreed( ioBuffer, amt );
    }
}

void PCHActivate( void )
{
    start_parse = clock();
}

#endif


void PCHSetFileName( char *name )
/*******************************/
{
    CMemFree( pchName );
    pchName = name;
}

char *PCHFileName( void )
{
    if( pchName != NULL ) {
        return( pchName );
    }
    return( PCH_DEFAULT_FILE_NAME );
}

static void fail( void )
{
    longjmp( *abortData, 1 );
}

static void dumpHeader( void )
{
    auto precompiled_header_header header;

    memcpy( header.text_header, PHH_TEXT_HEADER, TEXT_HEADER_SIZE );
    header.signature[0] = PHH_SIGNATURE_0;
    header.signature[1] = PHH_SIGNATURE_1;
    header.signature[2] = PHH_SIGNATURE_2;
    header.signature[3] = PHH_SIGNATURE_3;
    header.major_version = PHH_MAJOR;
    header.minor_version = PHH_MINOR;
    header.target_architecture = PHH_TARG_ARCHITECTURE;
    header.host_system = PHH_HOST_SYSTEM;
    header.host_architecture = PHH_HOST_ARCHITECTURE;
    header.corrupted = PHH_CORRUPTED_YES;
    header.browse_info = 0;
    PCHWriteVar( header );
}

static void setOKHeader( unsigned long brinf_posn )
{
    auto precompiled_header_header header;

    if( lseek( pchFile, 0, SEEK_SET ) != 0 ) {
        fail();
    }
    if( read( pchFile, &header, sizeof( header ) ) != sizeof( header ) ) {
        fail();
    }
    header.browse_info = brinf_posn;
    header.corrupted = PHH_CORRUPTED_NO;
    if( lseek( pchFile, 0, SEEK_SET ) != 0 ) {
        fail();
    }
    if( write( pchFile, &header, sizeof( header ) ) != sizeof( header ) ) {
        fail();
    }
}

static void dumpFileString( char *str )
{
    unsigned len;

    len = strlen( str ) + 1;
    PCHWriteUInt( len );
    PCHWrite( str, len );
}

static char *readFileString( char *buff )
{
    unsigned len;

    // assumes 'buff' is at least _MAX_PATH bytes
    len = PCHReadUInt();
    return( PCHRead( buff, len ) );
}

static void dumpCheckData( char *include_file )
{
    SRCFILE src;
    time_t stamp;
    auto char buff[_MAX_PATH];

    PCHWriteVar( GenSwitches );
    PCHWriteVar( TargetSwitches );
    PCHWriteUInt( ErrPCHVersion() );
    PCHWriteUInt( TYPC_LAST );
    PCHWriteUInt( sizeof( COMP_FLAGS ) );
    PCHWriteVar( CompFlags );
    dumpFileString( WholeFName );
    include_file = IoSuppFullPath( include_file, buff, sizeof( buff ) );
    dumpFileString( include_file );
    getcwd( buff, sizeof( buff ) );
    dumpFileString( buff );
    HFileListStart();
    for( ; ; ) {
        HFileListNext( buff );
        dumpFileString( buff );
        if( buff[0] == '\0' ) break;
    }
    src = SrcFileNotReadOnly( SrcFileWalkInit() );
    for( ; src != NULL; ) {
        if( ! IsSrcFilePrimary( src ) ) {
            dumpFileString( SrcFileName( src ) );
            stamp = SrcFileTimeStamp( src );
            PCHWriteVar( stamp );
        }
        src = SrcFileNotReadOnly( SrcFileWalkNext( src ) );
    }
    buff[0] = '\0';
    dumpFileString( buff );
    PCHDumpMacroCheck();
}

static long cursorWriteFilePosition( void )
{
    long   posn;

    posn = bufferPosition;
    posn += ( bufferCursor - ioBuffer );
    return( posn );
}

static void alignPCH( unsigned i, boolean writing )
{
    unsigned skip;
    char dummy[PCH_ALIGN] = {0};

    if( writing ) {
        skip = - cursorWriteFilePosition();
        skip &= PCH_ALIGN - 1;
        PCHWriteUInt( skip );
        DbgAssert( skip == 0 );
        if( skip != 0 ) {
            PCHWrite( dummy, skip );
        }
    } else {
        skip = PCHReadUInt();
        if( skip != 0 ) {
            PCHRead( dummy, skip );
        }
    }
#ifndef NDEBUG
    if( writing ) {
        unsigned w = -i;
        PCHWriteUInt( w );
    } else {
        unsigned r;
        r = PCHReadUInt();
        if( r != -i ) {
            printf( "index = %u\n", i );
            CFatal( "pre-compiled header read/write out of synch" );
        }
    }
#else
    i = i;
#endif
}

static void execInitFunctions( boolean writing )
{
    int i;
    pch_status ist;

    for( i = 0; i < PCHRW_MAX; ++i ) {
        ist = (initFunctions[i])( writing );
        if( ist != PCHCB_OK ) {
            fail();
        }
        alignPCH( i, writing );
    }
}

static void execFiniFunctions( boolean writing )
{
    int i;
    pch_status fst;

    for( i = PCHRW_MAX - 1; i >= 0; --i ) {
        fst = (finiFunctions[i])( writing );
        if( fst != PCHCB_OK ) {
            fail();
        }
        alignPCH( i, writing );
    }
}

static void execControlFunctions( boolean writing, pch_status (**tbl)( void ) )
{
    unsigned i;
    pch_status st;

    for( i = 0; i < PCHRW_MAX; ++i ) {
        ExtraRptTabSub( ctr_pchw_region, i, 0, (writing!=FALSE)*(ctr_pch_length+(IO_BUFFER_SIZE-amountLeft)) );
        ExtraRptTabSub( ctr_pchw_region, PCHRW_MAX, 0, (writing!=FALSE)*(ctr_pch_length+(IO_BUFFER_SIZE-amountLeft)) );
        st = (tbl[i])();
        ExtraRptTabAdd( ctr_pchw_region, i, 0, (writing!=FALSE)*(ctr_pch_length+(IO_BUFFER_SIZE-amountLeft)) );
        ExtraRptTabAdd( ctr_pchw_region, PCHRW_MAX, 0, (writing!=FALSE)*(ctr_pch_length+(IO_BUFFER_SIZE-amountLeft)) );
        if( st != PCHCB_OK ) {
            fail();
        }
        alignPCH( i, writing );
    }
}

void PCHFlushBuffer( void )
{
    unsigned amount;
    unsigned amt_written;

    if( amountLeft != IO_BUFFER_SIZE ) {
        amount = IO_BUFFER_SIZE - amountLeft;
        amt_written = write( pchFile, ioBuffer, amount );
        if( amt_written == -1 || amt_written != amount ) {
            fail();
        }
        ExtraRptAddtoCtr( ctr_pch_length, amt_written );
#if 1
        bufferPosition += amt_written;
        amountLeft = IO_BUFFER_SIZE;
        bufferCursor = ioBuffer;
#endif
    }
}


#ifndef NDEBUG
void PCHVerifyFile( int handle )    // DEBUG -- verify handle ok
{
    DbgVerify( handle == pchFile, "PCH handle is bad" );
}
#endif


#ifdef OPT_BR
long PCHSeek( long offset, int type )
{
    lseek( pchFile, offset, type );
    return tell( pchFile );
}
#endif


void PCHWrite( void const *p, unsigned size )
/*******************************************/
{
    unsigned aligned_size;
//  int amt_written;

    aligned_size = _pch_align_size( size );
    ExtraRptAddtoCtr( ctr_pch_waste, (aligned_size-size) );
    for( ; aligned_size > amountLeft; ) {
        memcpy( bufferCursor, p, amountLeft );
        p = (char *)p + amountLeft;
        size -= amountLeft;
        aligned_size -= amountLeft;
#if 0
        bufferCursor = ioBuffer;
        amt_written = write( pchFile, ioBuffer, IO_BUFFER_SIZE );
        if( amt_written == -1 || amt_written != IO_BUFFER_SIZE ) {
            fail();
        }
        ExtraRptAddtoCtr( ctr_pch_length, amt_written );
        bufferPosition += IO_BUFFER_SIZE;
        amountLeft = IO_BUFFER_SIZE;
#else
        amountLeft = 0;
        PCHFlushBuffer();
#endif
        DbgAssert( ( aligned_size % PCH_ALIGN ) == 0 );
    }
    memcpy( bufferCursor, p, size );
    bufferCursor += aligned_size;
    amountLeft -= aligned_size;
}

void PCHWriteUnaligned( void const *p, unsigned size )
/****************************************************/
{
#if 0
    wio_ssize_t amt_written;
#endif

    for( ; size > amountLeft; ) {
        memcpy( bufferCursor, p, amountLeft );
        p = (char *)p + amountLeft;
        size -= amountLeft;
#if 0
        bufferCursor = ioBuffer;
        amt_written = write( pchFile, ioBuffer, IO_BUFFER_SIZE );
        if( amt_written == -1 || amt_written != IO_BUFFER_SIZE ) {
            fail();
        }
        ExtraRptAddtoCtr( ctr_pch_length, amt_written );
        bufferPosition += IO_BUFFER_SIZE;
        amountLeft = IO_BUFFER_SIZE;
#else
        amountLeft = 0;
        PCHFlushBuffer();
#endif
    }
    memcpy( bufferCursor, p, size );
    bufferCursor += size;
    amountLeft -= size;
}

void PCHWriteUInt( unsigned v )
/*****************************/
{
    unsigned write_value;

    if( sizeof( unsigned ) <= amountLeft ) {
        DbgAssert( (((unsigned)(pointer_int)bufferCursor) & ( PCH_ALIGN - 1 )) == 0 );
        *((unsigned *)bufferCursor) = v;
        bufferCursor += sizeof( unsigned );
        amountLeft -= sizeof( unsigned );
        return;
    }
    write_value = v;
    PCHWriteVar( write_value );
}

NAME PCHDebugInfoName( void )
/***************************/
{
    return( pchDebugInfoName );
}

void PCHeaderCreate( char *include_file )
/***************************************/
{
    char * volatile pch_fname;  // must be preserved by setjmp()
    int status;
    auto jmp_buf restore_state;
#ifndef NDEBUG
    clock_t start;
    clock_t stop;

    start = clock();
#endif
    if( ErrCount != 0 ) {
        return;
    }
    if( CompFlags.fhr_switch_used ) {
        // treat any .PCH as read-only (do not create one)
        return;
    }
    pch_fname = PCHFileName();
    pchFile = sopen4( pch_fname, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, SH_DENYRW, PMODE_RW );
    if( pchFile == -1 ) {
        CErr2p( ERR_PCH_CREATE_ERROR, pch_fname );
        return;
    }
    bufferPosition = 0;
    ioBuffer = CMemAlloc( IO_BUFFER_SIZE );
    bufferCursor = ioBuffer;
    amountLeft = IO_BUFFER_SIZE;
    abortData = &restore_state;
    status = setjmp( restore_state );
    if( status == 0 ) {
        unsigned long brinf_posn;
        if( CompFlags.pch_debug_info_opt ) {
            pchDebugInfoName = CppPCHDebugInfoName( include_file );
        }
        dumpHeader();
        dumpCheckData( include_file );
        execInitFunctions( TRUE );
        execControlFunctions( TRUE, writeFunctions );
        execFiniFunctions( TRUE );
        brinf_posn = BrinfPch( pchFile );
        PCHFlushBuffer();
        // keep this PCH file
        pch_fname = NULL;
        setOKHeader( brinf_posn );
    } else {
        CErr1( ERR_PCH_WRITE_ERROR );
    }
    abortData = NULL;
    CMemFreePtr( &ioBuffer );
    close( pchFile );
    if( pch_fname != NULL ) {
        // write error occurred; delete PCH file
        remove( pch_fname );
    } else {
        if( CompFlags.pch_debug_info_opt ) {
            CompFlags.pch_debug_info_write = TRUE;
            CompFlags.all_debug_type_names = TRUE;
        }
    }
#ifndef NDEBUG
    stop = clock();
    printf( "%u ticks to parse header\n", ( start - start_parse ) );
    printf( "%u ticks to save pre-compiled header\n", ( stop - start ) );
#endif
}

static boolean headerIsOK( precompiled_header_header const* hp )
{
    if( hp->signature[0] != PHH_SIGNATURE_0 ) {
        return( FALSE );
    }
    if( hp->signature[1] != PHH_SIGNATURE_1 ) {
        return( FALSE );
    }
    if( hp->signature[2] != PHH_SIGNATURE_2 ) {
        return( FALSE );
    }
    if( hp->signature[3] != PHH_SIGNATURE_3 ) {
        return( FALSE );
    }
    if( hp->major_version != PHH_MAJOR ) {
        return( FALSE );
    }
    if( hp->minor_version != PHH_MINOR ) {
        return( FALSE );
    }
    if( hp->target_architecture != PHH_TARG_ARCHITECTURE ) {
        return( FALSE );
    }
    if( hp->host_system != PHH_HOST_SYSTEM ) {
        return( FALSE );
    }
    if( hp->host_architecture != PHH_HOST_ARCHITECTURE ) {
        return( FALSE );
    }
    if( hp->corrupted != PHH_CORRUPTED_NO ) {
        return( FALSE );
    }
    return( TRUE );
}

static boolean sameStamp( char *name, time_t saved )
{
    struct stat statbuf;

    if( CompFlags.pch_min_check ) {
        return( TRUE );
    }
    if( stat( name, &statbuf ) == 0 ) {
        if( statbuf.st_mtime == saved ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

static void pchWarn( unsigned msg )
{
    if( CompFlags.no_pch_warnings ) {
        return;
    }
    if( CompFlags.fhr_switch_used ) {
        return;
    }
    if( CompFlags.fhwe_switch_used ) {
        CWarnDontCount( msg );
    } else {
        CErr1( msg );
    }
}

void PCHWarn2p( unsigned msg, void *p )
/*************************************/
{
    if( CompFlags.no_pch_warnings ) {
        return;
    }
    if( CompFlags.fhr_switch_used ) {
        return;
    }
    if( CompFlags.fhwe_switch_used ) {
        CWarnDontCount( msg, p );
    } else {
        CErr2p( msg, p );
    }
}

static boolean checkCompFlags( COMP_FLAGS *testflags )
{
    #define _VERIFY_FLAG( f ) if( testflags->f ^ CompFlags.f ) return( TRUE );
    _VERIFY_FLAG( signed_char );
    _VERIFY_FLAG( original_enum_setting );
    _VERIFY_FLAG( excs_enabled );
    _VERIFY_FLAG( rtti_enabled );
    _VERIFY_FLAG( zo_switch_used );
    _VERIFY_FLAG( pch_debug_info_opt );
    _VERIFY_FLAG( register_conventions );
    _VERIFY_FLAG( extensions_enabled );
    _VERIFY_FLAG( disable_ialias );
    _VERIFY_FLAG( cpp_ignore_env );
    _VERIFY_FLAG( ignore_default_dirs );
    #undef _VERIFY_FLAG
    return( FALSE );
}

static void transferCompFlags( COMP_FLAGS *testflags )
{
    // flags that can be changed in header files usually go here
    #define _COPY_FLAG( f ) CompFlags.f=0;if( testflags->f )CompFlags.f=1;
    _COPY_FLAG( float_used );
    _COPY_FLAG( external_defn_found );
    _COPY_FLAG( main_has_parms );
    _COPY_FLAG( has_main );
    _COPY_FLAG( has_winmain );
    _COPY_FLAG( has_libmain );
    _COPY_FLAG( has_dllmain );
    _COPY_FLAG( pragma_library );
    _COPY_FLAG( extern_C_defn_found );
    _COPY_FLAG( has_longjmp );
    _COPY_FLAG( dt_method_pragma );
    #undef _COPY_FLAG
}

static boolean stringIsDifferent( const char *from_pch, const char *curr, unsigned msg )
{
    if( ! CompFlags.pch_min_check ) {
        if( FNAMECMPSTR( from_pch, curr ) != 0 ) {
            pchWarn( msg );
            return( TRUE );
        }
    }
    return( FALSE );
}

static void flushUntilNullString( char *buff )
{
    for(;;) {
        if( *readFileString( buff ) == '\0' ) {
            break;
        }
    }
}

static boolean stalePCH( char *include_file )
{
    time_t stamp;
    unsigned long test_gen;
    unsigned long test_target;
    auto char buff1[_MAX_PATH];
    auto char buff2[_MAX_PATH];
    auto COMP_FLAGS testflags;

    PCHReadVar( test_gen );
    PCHReadVar( test_target );
    if( test_gen != GenSwitches || test_target != TargetSwitches ) {
        pchWarn( WARN_PCH_CONTENTS_OPTIONS );
        return( TRUE );
    }
    if( PCHReadUInt() != ErrPCHVersion() ) {
        pchWarn( WARN_PCH_CONTENTS_HEADER_ERROR );
        return( TRUE );
    }
    if( PCHReadUInt() != TYPC_LAST ) {
        pchWarn( WARN_PCH_CONTENTS_HEADER_ERROR );
        return( TRUE );
    }
    if( PCHReadUInt() != sizeof( COMP_FLAGS ) ) {
        pchWarn( WARN_PCH_CONTENTS_HEADER_ERROR );
        return( TRUE );
    }
    PCHReadVar( testflags );
    if( checkCompFlags( &testflags ) ) {
        pchWarn( WARN_PCH_CONTENTS_OPTIONS );
        return( TRUE );
    }
    readFileString( buff1 );
    if( FNAMECMPSTR( buff1, WholeFName ) == 0 ) {
        if( CompFlags.pch_debug_info_opt ) {
            // this source file created the PCH but it is being recompiled
            // so we have to recreate the PCH along with the debug info
            pchWarn( WARN_PCH_DEBUG_OPTIMIZE );
            return( TRUE );
        }
    }
    readFileString( buff1 );
    include_file = IoSuppFullPath( include_file, buff2, sizeof( buff2 ) );
    if( FNAMECMPSTR( buff1, include_file ) != 0 ) {
        pchWarn( WARN_PCH_CONTENTS_INCFILE );
        return( TRUE );
    }
    readFileString( buff1 );
    getcwd( buff2, sizeof( buff2 ) );
    if( stringIsDifferent( buff1, buff2, WARN_PCH_CONTENTS_CWD ) ) {
        return( TRUE );
    }
    if( CompFlags.pch_min_check ) {
        flushUntilNullString( buff2 );
    } else {
        HFileListStart();
        for(;;) {
            HFileListNext( buff1 );
            readFileString( buff2 );
            if( stringIsDifferent( buff1, buff2, WARN_PCH_CONTENTS_INCLUDE ) ) {
                return( TRUE );
            }
            if( buff1[0] == '\0' ) break;
        }
    }
    for( ; *readFileString( buff1 ) != '\0'; ) {
        PCHReadVar( stamp );
        if( ! sameStamp( buff1, stamp ) ) {
            PCHWarn2p( WARN_PCH_CONTENTS_HFILE, buff1 );
            return( TRUE );
        }
    }
    if( ! PCHVerifyMacroCheck() ) {
        return( TRUE );
    }
    transferCompFlags( &testflags );
    return( FALSE );
}

static unsigned readBuffer( unsigned left_check )
{
    unsigned left;

    left = read( pchFile, ioBuffer, IO_BUFFER_SIZE );
    if( left == -1 || left == left_check ) {
        fail();
    }
    return left;
}

static unsigned initialRead( void )
{
    unsigned left;

    DbgAssert( pch_buff_eob == pch_buff_cur );
    left = readBuffer( -1 );
    pch_buff_cur = ioBuffer;
    return( left );
}

pch_absorb PCHeaderAbsorb( char *include_file )
/*********************************************/
{
    pch_absorb ret;
    int status;
    auto jmp_buf restore_state;
#ifndef NDEBUG
    clock_t start;
    clock_t stop;

    start = clock();
#endif
    if( ErrCount != 0 ) {
        return( PCHA_ERRORS_PRESENT );
    }
    if( CompFlags.fhw_switch_used ) {
        return( PCHA_IGNORE );
    }
    pchFile = sopen3( PCHFileName(), O_RDONLY | O_BINARY, SH_DENYWR );
    if( pchFile == -1 ) {
        return( PCHA_NOT_PRESENT );
    }
    ioBuffer = CMemAlloc( IO_BUFFER_SIZE );
    pch_buff_eob = ioBuffer + IO_BUFFER_SIZE;
    pch_buff_cur = pch_buff_eob;
    ret = PCHA_OK;
    abortData = &restore_state;
    status = setjmp( restore_state );
    if( status == 0 ) {
        if( initialRead() == 0 ) {
            ret = PCHA_NOT_PRESENT;
        } else {
            auto precompiled_header_header header;
            unsigned long br_posn;

            PCHReadVar( header );
            br_posn = header.browse_info;
            if( headerIsOK( &header ) ) {
                if( ! stalePCH( include_file ) ) {
                    execInitFunctions( FALSE );
                    execControlFunctions( FALSE, readFunctions );
                    execFiniFunctions( FALSE );
#ifdef OPT_BR
                    if( 0 != br_posn ) {
                        BrinfPchRead();
                    }
#endif
                } else {
                    ret = PCHA_STALE;
                }
            } else {
                pchWarn( WARN_PCH_CONTENTS_HEADER_ERROR );
                ret = PCHA_HEADER;
            }
        }
    } else {
        CErr1( ERR_PCH_READ_ERROR );
    }
    abortData = NULL;
    CMemFreePtr( &ioBuffer );
    close( pchFile );
    if( CompFlags.pch_debug_info_opt && ret == PCHA_OK ) {
        CompFlags.pch_debug_info_read = TRUE;
    }
#ifndef NDEBUG
    stop = clock();
    printf( "%u ticks to load pre-compiled header\n", ( stop - start ) );
#endif
    return( ret );
}

// NOTE: the reading routines are coded to assume that adding a
//       reasonable size to a buffer pointer will never cause a
//       memory wrap-around.

void *PCHRead( void *p, unsigned size )
/*************************************/
{
    unsigned aligned_size;
    unsigned left;
    char *buff_ptr;
    void *retn;

    PCHTrashAlreadyRead();
    retn = p;
    buff_ptr = pch_buff_cur;
    left = pch_buff_eob - buff_ptr;
    aligned_size = _pch_align_size( size );
    for( ; aligned_size > left; ) {
        p = memcpy( p, buff_ptr, left );
        p = (char *)p + left;
        size -= left;
        aligned_size -= left;
        left = readBuffer( 0 );
        buff_ptr = ioBuffer;
        DbgAssert( ( aligned_size % PCH_ALIGN ) == 0 );
    }
    memcpy( p, buff_ptr, size );
    pch_buff_cur = buff_ptr + aligned_size;
    return retn;
}

void *PCHReadUnaligned( void *p, unsigned size )
/**********************************************/
{
    unsigned left;
    char *buff_ptr;
    void *retn;

    PCHTrashAlreadyRead();
    retn = p;
    buff_ptr = pch_buff_cur;
    left = pch_buff_eob - buff_ptr;
    for( ; size > left; ) {
        p = memcpy( p, buff_ptr, left );
        p = (char *)p + left;
        size -= left;
        left = readBuffer( 0 );
        buff_ptr = ioBuffer;
    }
    memcpy( p, buff_ptr, size );
    pch_buff_cur = buff_ptr + size;
    return retn;
}

static unsigned doReadUnsigned( void )
/************************************/
{
    unsigned read_value;
    PCHRead( &read_value, sizeof( read_value ) );
    return( read_value );
}

unsigned PCHReadUInt( void )
/**************************/
{
    unsigned value;
    unsigned *p_value;
    void *buff_ptr;
    void *end;

    PCHTrashAlreadyRead();
    buff_ptr = pch_buff_cur;
    end = (char *)buff_ptr + sizeof( value );
    if( end <= (void *)pch_buff_eob ) {
        DbgAssert( ( ((unsigned)(pointer_int)buff_ptr) % PCH_ALIGN ) == 0 );
        p_value = (unsigned *)buff_ptr;
        pch_buff_cur = end;
        value = *p_value;
        return value;
    }
    return doReadUnsigned();
}

void *PCHReadCVIndexElement( cvinit_t *data )
/*******************************************/
{
    cv_index index;
    void *buff_ptr;
    void *end;

    PCHTrashAlreadyRead();
    buff_ptr = pch_buff_cur;
    end = (char *)buff_ptr + sizeof( cv_index );
    if( end <= (void *)pch_buff_eob ) {
        DbgAssert( ( ((unsigned)(pointer_int)pch_buff_cur) % PCH_ALIGN ) == 0 );
        index = *(cv_index *)pch_buff_cur;
        pch_buff_cur = end;
    } else {
        PCHReadVar( index );
    }
    if( index == PCH_NULL_INDEX ) {
        return( NULL );
    } else {
        return( CarveInitElement( data, index ) );
    }
}

unsigned PCHReadUIntUnaligned( void )
/***********************************/
{
    unsigned value;
    unsigned *p_value;
    void *buff_ptr;
    void *end;

    PCHTrashAlreadyRead();
    buff_ptr = pch_buff_cur;
    end = (char *)buff_ptr + sizeof( value );
    if( end <= (void *)pch_buff_eob ) {
        p_value = (unsigned *)buff_ptr;
        pch_buff_cur = end;
        value = *p_value;
        return value;
    }
    PCHReadUnaligned( &value, sizeof( value ) );
    return value;
}

pch_status PCHReadVerify( void )
/******************************/
{
    char buff[10];

    pchDebugInfoName = NameMapIndex( PCHSetUInt( PCHReadUInt() ) );
    PCHRead( buff, 10 );
    if( memcmp( buff, "WATCOM-PCH", 10 ) != 0 ) {
        return( PCHCB_ERROR );
    }
    return( PCHCB_OK );
}

pch_status PCHWriteVerify( void )
/*******************************/
{
    PCHWriteUInt( PCHGetUInt( NameGetIndex( pchDebugInfoName ) ) );
    PCHWrite( "WATCOM-PCH", 10 );
    return( PCHCB_OK );
}

pch_status PCHInitVerify( boolean writing )
/*****************************************/
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniVerify( boolean writing )
/*****************************************/
{
    writing = writing;
    return( PCHCB_OK );
}

void PCHRelocStart( pch_reloc_index ri )
/**************************************/
{
    relocInfo[ ri ].start = cursorWriteFilePosition();
    DbgVerify( relocInfo[ ri ].start != 0, "PCH reloc cannot be at start of file" );
}

void PCHRelocStop( pch_reloc_index ri )
/*************************************/
{
    relocInfo[ ri ].stop = cursorWriteFilePosition();
    DbgVerify( relocInfo[ ri ].start != 0 && relocInfo[ ri ].stop > relocInfo[ ri ].start, "PCH reloc stop has no matching start" );
}

void PCHPerformReloc( pch_reloc_index ri )
/****************************************/
{
    long        start_position;
    long        stop_position;
    unsigned    reloc_size;
    char        *volatile pch_fname;  // must be preserved by setjmp()
    int         status;
    jmp_buf     restore_state;
#ifndef NDEBUG
    clock_t     start;
    clock_t     stop;

    start = clock();
#endif
    if( ErrCount != 0 ) {
        return;
    }
    start_position = relocInfo[ ri ].start;
    if( start_position == 0 ) {
        return;
    }
    stop_position = relocInfo[ ri ].stop;
    pch_fname = PCHFileName();
    pchFile = sopen3( pch_fname, O_RDWR | O_BINARY | O_EXCL, SH_DENYRW );
    if( pchFile == -1 ) {
        CErr2p( ERR_PCH_OPEN_ERROR, pch_fname );
        return;
    }
    DbgAssert( ( stop_position - start_position ) < UINT_MAX );
    reloc_size = stop_position - start_position;
    ioBuffer = CMemAlloc( reloc_size );
    abortData = &restore_state;
    status = setjmp( restore_state );
    if( status == 0 ) {
        if( lseek( pchFile, start_position, SEEK_SET ) != start_position ) {
            fail();
        }
        if( read( pchFile, ioBuffer, reloc_size ) != reloc_size ) {
            fail();
        }
        if( relocFunctions[ri]( ioBuffer, reloc_size ) == PCHCB_ERROR ) {
            fail();
        }
        if( lseek( pchFile, -(long)reloc_size, SEEK_CUR ) != start_position ) {
            fail();
        }
        if( write( pchFile, ioBuffer, reloc_size ) != reloc_size ) {
            fail();
        }
        // keep this PCH file
        pch_fname = NULL;
    } else {
        CErr1( ERR_PCH_WRITE_ERROR );
    }
    abortData = NULL;
    CMemFreePtr( &ioBuffer );
    close( pchFile );
    if( pch_fname != NULL ) {
        // write error occurred; delete PCH file
        remove( pch_fname );
    }
#ifndef NDEBUG
    stop = clock();
    printf( "%u ticks to relocate pre-compiled header (%u section)\n", ( stop - start ), ri );
#endif
}

#ifndef NDEBUG
static char const * const pchRegionNames[] = {
    #define PCH_EXEC( a, b )    #a ,
    #include "pcregdef.h"
    "Total"
};
#endif

static void pchInit( INITFINI* defn )
{
    pch_reloc_info *cri;

    defn = defn;
    pchName = NULL;
    pchDebugInfoName = NULL;
    for( cri = relocInfo; cri < &relocInfo[ PCHRELOC_MAX ]; ++cri ) {
        cri->start = 0;
    }
#ifndef NDEBUG
    ExtraRptRegisterCtr( &ctr_pch_length, "# bytes in PCH" );
    ExtraRptRegisterCtr( &ctr_pch_waste, "# bytes wasted in PCH for alignment" );
    ExtraRptRegisterTab( "PCH region size table (pcregdef.h)", pchRegionNames, ctr_pchw_region, PCHRW_MAX+1, 1 );
    if( strlen( PHH_TEXT_HEADER ) != TEXT_HEADER_SIZE ) {
        CFatal( "pre-compiled header text is not the correct size!" );
    }
#endif
}

static void pchFini( INITFINI* defn )
{
    defn = defn;
    CMemFreePtr( &pchName );
}

INITDEFN( pchdrs, pchInit, pchFini )
