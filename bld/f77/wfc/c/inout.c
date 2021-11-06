/***************************************************************************
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
* Description:  Input/output for listing, errors and source files.
*
****************************************************************************/


#include "ftnstd.h"
#include <time.h>
#include "progsw.h"
#include "cpopt.h"
#include "errcod.h"
#include "global.h"
#include "cioconst.h"
#include "csetinfo.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "comio.h"
#include "inout.h"
#include "banner.h"
#include "charset.h"
#include "mkname.h"
#include "filescan.h"
#include "sdcio.h"
#include "libsupp.h"
#include "wf77prag.h"
#include "errutil.h"
#include "sdfile.h"
#include "brseinfo.h"

#include "clibext.h"


#if _CPU == 8086
    #define _Banner "FORTRAN 77 x86 16-bit Optimizing Compiler"
#elif _CPU == 386
    #define _Banner "FORTRAN 77 x86 32-bit Optimizing Compiler"
#elif _CPU == _AXP
    #define _Banner "FORTRAN 77 Alpha AXP Optimizing Compiler"
#elif _CPU == _PPC
    #define _Banner "FORTRAN 77 PowerPC Optimizing Compiler"
#else
    #error Unknown System
#endif

static char             ErrExtn[] = { "err" };
static char             LstExtn[] = { "lst" };

static char             *ListBuff;      // listing file buffer
static file_handle      ListFile;       // file pointer for the listing file
static size_t           ListCursor;     // offset into "ListBuff"

static byte             ListCount;      // # of lines printed to listing file
static byte             ListFlag;       // flag for listing file

static char             *ErrBuff;       // error file buffer
static file_handle      ErrFile;        // file pointer for the error file
static size_t           ErrCursor;      // offset into "ErrBuff"

static char             *TermBuff;      // terminal file buffer
static file_handle      TermFile;       // file pointer for terminal
static size_t           TermCursor;     // offset into "TermBuff"


//========================================================================
//
//  Initialization routines
//
//========================================================================


void    InitComIO( void ) {
//===================

    TermCursor = 0;
    ErrCursor  = 0;
    ListCursor = 0;
    TermBuff = FMemAlloc( TERM_BUFF_SIZE );
    ErrBuff = NULL;
    ListBuff = NULL;
    CurrFile = NULL;
    ErrFile = NULL;
    ListFile = NULL;
    ListFlag = 0;
    ListCount = 0;
    SDInitIO();
    TermFile = FStdOut;
}


void    FiniComIO( void ) {
//===================

    FMemFree( TermBuff );
}


//========================================================================
//
//  Source file routines
//
//========================================================================


static  void    Erase( char *extn ) {
//===================================

    char        buffer[_MAX_PATH];

    MakeName( SDFName( SrcName ), extn, buffer );
    SDScratch( buffer );
}


void    OpenSrc( void ) {
//=======================

    file_handle fp;
    char        err_msg[ERR_BUFF_SIZE+1];
    char        bld_name[_MAX_PATH];
    bool        erase_err;

    erase_err = ErrFile == NULL;
    MakeName( SrcName, SrcExtn, bld_name );
    fp = SDOpenText( bld_name, "rt" );
    if( fp != NULL ) {
        SrcInclude( bld_name );
        CurrFile->fileptr = fp;
    } else {
        SDError( NULL, err_msg, sizeof( err_msg ) );
        InfoError( SM_OPENING_FILE, bld_name, err_msg );
    }
    if( erase_err ) {
        CloseErr();
        Erase( ErrExtn );
    }
}


void    IOPurge( void ) {
//=======================

// make sure all the input files are closed

    while( CurrFile != NULL ) {
        Conclude();
    }
}


static size_t SrcRead( void )
//===========================
{
    size_t      len;
    file_handle fp;
    char        msg[81];

    fp = CurrFile->fileptr;
    len = SDReadText( fp, SrcBuff, SRCLEN );
    if( SDEof( fp ) ) {
        ProgSw |= PS_INC_EOF;
    } else if( SDError( fp, msg, sizeof( msg ) ) ) {
        InfoError( SM_IO_READ_ERR, CurrFile->name, msg );
        ProgSw |= PS_INC_EOF;
    }
    return( len );
}


void    ReadSrc( void ) {
//=======================

    size_t      len;

    // If we are loading source as a result of an undefined
    // subprogram (as opposed to using an C$INCLUDE option),
    // then indicate EOF since the main source file may have
    // the C$DATA option in it in which case "CurrFile" will
    // not be NULL after calling "Conclude()".
    if( CurrFile->flags & INC_DATA_OPTION ) {
        ProgSw |= PS_SOURCE_EOF;
    } else {
        len = SrcRead();
        if( ProgSw & PS_INC_EOF ) {
            CurrFile->flags |= CONC_PENDING;
            if( CurrFile->link == NULL ) {
                ProgSw |= PS_SOURCE_EOF;
            }
        } else {
            CurrFile->rec++;
            SrcBuff[ len ] = NULLCHAR;
        }
    }
}


static bool AlreadyOpen( const char *name )
//=========================================
{
    source_t    *src;

    src = CurrFile;
    for( ;; ) {
        if( src == NULL )
            return( false );
        if( strcmp( name, src->name ) == 0 )
            break;
        src = src->link;
    }
    InfoError( CO_ALREADY_OPEN, name );
    return( true );
}


void    Include( const char *inc_name )
//=====================================
{
    file_handle fp;
    char        bld_name[_MAX_PATH];
    char        err_msg[ERR_BUFF_SIZE+1];

    CopyMaxStr( inc_name, bld_name, _MAX_PATH - 1 );
    MakeName( bld_name, SDSplitSrcExtn( bld_name ), bld_name );
    if( AlreadyOpen( inc_name ) )
        return;
    if( AlreadyOpen( bld_name ) )
        return;
    // try file called <include_name>.FOR.
    fp = SDOpenText( bld_name, "rt" );
    if( fp != NULL ) {
        SrcInclude( bld_name );
        CurrFile->fileptr = fp;
    } else {
        // get error message before next i/o
        SDError( NULL, err_msg, sizeof( err_msg ) );
        // try library
        fp = IncSearch( inc_name );
        if( fp != NULL ) {
            // SrcInclude( inc_name ) now done in LIBSUPP
            CurrFile->fileptr = fp;
            CurrFile->flags |= INC_LIB_MEMBER;
        } else {
            // could not open include file
            InfoError( SM_OPENING_FILE, bld_name, err_msg );
        }
    }
    // clear RetCode so that we don't get "file not found" returned
    // because we could not open include file
    RetCode = _SUCCESSFUL;
    AddDependencyInfo( CurrFile );
}


bool    SetLst( bool new ) {
//==========================

    bool        old;

    old = ( (ListFlag & LF_QUIET) == 0 );
    if( new ) {
        ListFlag &= ~LF_QUIET;
    } else {
        ListFlag |= LF_QUIET;
    }
    return( old );
}


void    SrcInclude( const char *name )
//====================================
{
    source_t    *src;

    src = FMemAlloc( sizeof( source_t ) );
    src->name = FMemAlloc( strlen( name ) + 1 );
    strcpy( src->name, name );
    src->rec = 0;
    src->link = CurrFile;
    src->options = NewOptions;
    src->flags = 0;
    if( CurrFile != NULL ) {
        NewOptions = Options;
        if( (Options & OPT_INCLIST) == 0 ) {
            SetLst( false );
        }
    }
    CurrFile = src;
    if( CurrFile->link ) {
        // tell the browser which file we are going into (not for the main
        // source file since we have not yet initialized the dwarf library)
        BISetSrcFile();
    }
}


void    Conclude( void ) {
//========================

    source_t    *old;

    old = CurrFile;
    CurrFile = CurrFile->link;
    if( CurrFile != NULL ) {
        NewOptions = old->options;
        Options = NewOptions;
        if( ( ( CurrFile->link == NULL ) && (Options & OPT_LIST) ) ||
              (Options & OPT_INCLIST) ) {
            SetLst( true );
        } else {
            SetLst( false );
        }
    }
    SDClose( old->fileptr );
    FMemFree( old->name );
    FMemFree( old );
    ProgSw &= ~PS_INC_EOF;
    BISetSrcFile();             // tell browser which file we return to
}


//========================================================================
//
//  Error file routines
//
//========================================================================


void    OpenErr( void ) {
//=======================

    char        buffer[_MAX_PATH];
    char        errmsg[81];

    if( (Options & OPT_ERRFILE) && ( (ProgSw & PS_ERR_OPEN_TRIED) == 0 ) ) {
        ProgSw |= PS_ERR_OPEN_TRIED;
        MakeName( SDFName( SrcName ), ErrExtn, buffer );
        ErrFile = SDOpenText( buffer, "wt" );
        if( SDError( ErrFile, errmsg, sizeof( errmsg ) ) ) {
            InfoError( SM_OPENING_FILE, &buffer, &errmsg );
        }
        if( ErrFile != NULL ) {
            ErrCursor = 0;
            if( ErrBuff == NULL ) {
                ErrBuff = FMemAlloc( ERR_BUFF_SIZE );
            }
        }
    }
}


void    CompErr( uint msg ) {
//===========================

    InfoError( msg );
}


void    PrintErr( const char *string )
//====================================
{
    JustErr( string );
    PrtLst( string );
}


static  bool    ErrToTerm( void ) {
//=================================

    if( (Options & OPT_TERM) == 0 )
        return( false );
    if( (Options & OPT_TYPE) && ( ListFile != NULL ) )
        return( false );
    return( true );
}


static  void    ChkErrErr( void ) {
//=================================

    char        msg[81];
    char        fnbuff[_MAX_PATH];

    if( SDError( ErrFile, msg, sizeof( msg ) ) ) {
        CloseErr();
        Options |= OPT_TERM;
        TermCursor = 0;
        MakeName( SDFName( SrcName ), ErrExtn, fnbuff );
        InfoError( SM_IO_WRITE_ERR, fnbuff, msg );
    }
}


static  void    ErrNL( void ) {
//=============================

    if( ErrFile != NULL ) {
        SDWriteTextNL( ErrFile, ErrBuff, ErrCursor );
        ChkErrErr();
    }
    ErrCursor = 0;
}


void    PrtErrNL( void ) {
//========================

    if( ErrToTerm() ) {
        TOutNL( "" );
    }
    ErrNL();
    PrtLstNL( "" );
}


static  void    SendBuff( const char *str, char *buff, size_t buff_size, size_t *cursor,
                          file_handle fp, void (*err_rtn)( void ) ) {
//==========================================================================

    size_t  len;

    for( ; fp != NULL; ) {
        len = buff_size - 1 - *cursor;
        len = CharSetInfo.extract_text( str, len );
        len = CopyMaxStr( str, &buff[*cursor], len );
        *cursor += len;
        str += len;
        if( *str == NULLCHAR )
            break;
        SDWriteTextNL( fp, buff, *cursor );
        err_rtn();
        *cursor = 0;
    }
}


static  void    ErrOut( const char *string )
//==========================================
{
    if( ErrFile != NULL ) {
        SendBuff( string, ErrBuff, ERR_BUFF_SIZE, &ErrCursor, ErrFile, &ChkErrErr );
    }
}


void    JustErr( const char *string )
//===================================
{
    if( ErrToTerm() ) {
        TOut( string );
    }
    ErrOut( string );
}


void    ChkErrFile( void ) {
//==========================

// Make sure error file is opened.

    if( ErrFile == NULL ) {
        OpenErr();
    }
}


void    CloseErr( void ) {
//========================

    if( ErrFile == NULL )
        return;
    SDClose( ErrFile );
    ErrFile = NULL;
    if( ErrBuff == NULL )
        return;
    FMemFree( ErrBuff );
    ErrBuff = NULL;
}


//========================================================================
//
//  Terminal output routines
//
//========================================================================


static  void    ChkTermErr( void ) {
//==================================
}


void    TOutNL( const char *string )
//==================================
{
    TOut( string );
    SDWriteTextNL( TermFile, TermBuff, TermCursor );
    TermCursor = 0;
}


void    TOut( const char *string )
//================================
{
    SendBuff( string, TermBuff, TERM_BUFF_SIZE, &TermCursor, TermFile, &ChkTermErr );
}


//========================================================================
//
//  Listing file routines
//
//========================================================================


static  void    OpenListingFile( bool reopen ) {
//==============================================

    char        errmsg[81];
    char        name[_MAX_PATH];

    /* unused parameters */ (void)reopen;

    if( (Options & OPT_LIST) == 0 ) {
        // no listing file
        // ignore other listing file options
    } else {
        GetLstName( name );
        ListFile = SDOpenText( name, "wb" );
        if( SDError( ListFile, errmsg, sizeof( errmsg ) ) ) {
            InfoError( SM_OPENING_FILE, name, errmsg );
        } else {
            ListFlag |= LF_CC_NOLF;
            ListBuff = FMemAlloc( LIST_BUFF_SIZE + 1 );
            if( ListBuff == NULL ) {
                CloseLst();
                InfoError( MO_DYNAMIC_OUT );
            }
        }
    }
}


void    OpenLst( void ) {
//=======================

    OpenListingFile( false );
}


void    ReOpenLst( void ) {
//=========================

    OpenListingFile( true );
}


void    ChkPntLst( void ) {
//=========================

    if( ListFlag & LF_QUIET ) {
        ListFlag &= ~LF_STMT_LISTED;
    } else {
        ListFlag |= LF_STMT_LISTED;
    }
}


bool    WasStmtListed( void ) {
//=============================

    return( (ListFlag & LF_STMT_LISTED) != 0 );
}


void    TOutBanner( void ) {
//==========================

#if defined( _BETAVER )
    TOutNL( banner1w1( _Banner ) );
    TOutNL( banner1w2( _WFC_VERSION_ ) );
#else
    TOutNL( banner1w( _Banner, _WFC_VERSION_ ) );
#endif
    TOutNL( banner2 );
    TOutNL( banner2a( 1984 ) );
    TOutNL( banner3 );
    TOutNL( banner3a );
}

#define MAX_TIME_STR    (4+1+2+1+2+1+2+1+2+1+2)

void    PrtBanner( void ) {
//=========================

    char        banner[LIST_BUFF_SIZE + 1];
    time_t      time_of_day;
    struct tm   *t;

#if defined( _BETAVER )
    strcpy( banner, banner1w1( _Banner ) );
    PrtLstNL( banner );
    strcpy( banner, banner1w2( _WFC_VERSION_ ) );
#else
    strcpy( banner, banner1w( _Banner, _WFC_VERSION_ ) );
#endif
    time_of_day = time( NULL );
    t = localtime( &time_of_day );
    MsgFormat( "  %4d1/%2d2/%2d3 %2d4:%2d5:%2d6", banner + strlen( banner ),
               1900 + t->tm_year, t->tm_mon + 1, t->tm_mday,
               t->tm_hour, t->tm_min, t->tm_sec );
    PrtLstNL( banner );
    PrtLstNL( banner2 );
    PrtLstNL( banner2a( 1984 ) );
    PrtLstNL( banner3 );
    PrtLstNL( banner3a );
}


void    GetLstName( char *buffer ) {
//==================================

    if( Options & OPT_TYPE ) {
        strcpy( buffer, SDTermOut );
#if !defined( __UNIX__ )
    // On the VAX, /PRINT means to generate a disk file "xxx.LIS"
    //             and set the spooling bit
    // On QNX, there is no /PRINT option
    } else if( Options & OPT_PRINT ) {
        strcpy( buffer, SDPrtName );
#endif
    } else {
        MakeName( SDFName( SrcName ), LstExtn, buffer );
    }
}


void    PrtLstNL( const char *string )
//====================================
{
    ListFlag |= LF_NEW_LINE;
    PrtLst( string );
    ListFlag &= LF_OFF;
}


static  void    ChkLstErr( void ) {
//=================================

    char        msg[81];
    char        fnbuff[_MAX_PATH];

    if( SDError( ListFile, msg, sizeof( msg ) ) ) {
        CloseLst();
        Options |= OPT_TERM;
        TermCursor = 0;
        GetLstName( fnbuff );
        InfoError( SM_IO_WRITE_ERR, fnbuff, msg );
    }
}


static  void    SendRec( void ) {
//===============================

    if( ListFile != NULL ) {
        SDWriteCCChar( ListFile, *ListBuff, (ListFlag & LF_CC_NOLF) );
        SDWrite( ListFile, ListBuff + 1, ListCursor - 1 );
        SDWrite( ListFile, "\r", 1 );
        ChkLstErr();
        ListFlag &= ~LF_CC_NOLF;
    }
    ListFlag &= LF_OFF;
    ListCursor = 0;
}


static  void    SetCtrlSeq( void ) {
//==================================

    char        *ctrlseq;

    ++ListCount;
    if( ListCount >= LinesPerPage - LF_PAGE_BOUNDARY ) {
        ListFlag |= LF_PAGE_FLAG;
    }
    if( ListFlag & LF_PAGE_FLAG ) {
        ListCount = 0;
        if( Options & OPT_TYPE ) {
            ctrlseq = SkipCtrlSeq;
        } else {
            ctrlseq = FFCtrlSeq;
        }
    } else if( ListFlag & LF_SKIP_FLAG ) {
        ctrlseq = SkipCtrlSeq;
    } else {
        ctrlseq = NormalCtrlSeq;
    }
    ListCursor = CopyMaxStr( ctrlseq, ListBuff, LIST_BUFF_SIZE );
}


static  void    PutLst( const char *string )
//==========================================
{
    size_t      len;
    bool        newline;

    newline = (ListFlag & LF_NEW_LINE);
    for( ; ListFile != NULL; ) {
        if( ListCursor == 0 ) {
            SetCtrlSeq();
        }
        len = LIST_BUFF_SIZE - ListCursor - 1; // -1 for NULLCHAR
        len = CharSetInfo.extract_text( string, len );
        len = CopyMaxStr( string, &ListBuff[ ListCursor ], len );
        ListCursor += len;
        string += len;
        if( *string == NULLCHAR )
            break;
        SendRec();
    }
    if( newline ) {
        SendRec();
    }
}


void    PrtLst( const char *string )
//==================================
{
    if( ListFlag & LF_QUIET )
        return;
    if( ListFile == NULL )
        return;
    PutLst( string );
}


void    CloseLst( void ) {
//========================

    if( ListFile == NULL )
        return;
    SDWriteCCChar( ListFile, ' ', (ListFlag & LF_CC_NOLF) );
    SDClose( ListFile );
    ListFile = NULL;
    if( ListBuff == NULL )
        return;
    FMemFree( ListBuff );
    ListBuff = NULL;
}


void    LFEndSrc( void ) {
//==================

    if( ListFile == NULL )
        return;
    ListFlag &= LF_OFF;
    ListCursor = 0;
    SetCtrlSeq();
    SendRec();
}


void    LFNewPage( void ) {
//=========================

    ListFlag |= LF_PAGE_FLAG;
}


void    LFSkip( void ) {
//======================

    ListFlag |= LF_SKIP_FLAG;
    if( (ListFlag & LF_QUIET) == 0 ) {
        ++ListCount;
    }
}
