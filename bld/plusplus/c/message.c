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

#include <setjmp.h>
#include <stdarg.h>

#include "cgfront.h"
#include "stats.h"
#include "memmgr.h"
#include "iosupp.h"
#include "preproc.h"
#include "vbuf.h"
#include "fmtmsg.h"
#include "errdefns.h"
#include "context.h"
#include "ring.h"
#include "srcfile.h"
#include "initdefs.h"
#include "fmtsym.h"
#include "pcheader.h"
#include "cppexit.h"
#include "idedll.h"
#include "macro.h"
#include "intlload.h"
#ifndef NDEBUG
#include "enterdb.h"
#endif

#define RESERVE_MAX                 2048
#define RESERVE_MIN                 256
static unsigned reserveSize;
static void *reserveMem;
static unsigned reserveDepth;
static unsigned internalErrCount;
static unsigned suppressCount;

static FILE *err_file;              // ERROR FILE
static TOKEN_LOCN err_locn;         // error location
static TOKEN_LOCN notes_locn;       // notes location
static unsigned char* orig_err_levs;// original error levels
static boolean errLimitExceeded;    // have exceeded error limit
static IntlData *internationalData; // translated messages

static SUICIDE_CALLBACK *suicideCallbacks;

#define MSG_SCOPE static
#define MSG_MEM
#include "errmsgs1.gh"
#include "msgdecod.c"
#define MSG_CONST
#include "errlevel.gh"
#undef MSG_CONST

#if _CPU == 386
#define INTL_NAME   "wpp386"
#elif _CPU == 8086
#define INTL_NAME   "wppi86"
#elif _CPU == _AXP
#define INTL_NAME   "wppaxp"
#else
#error missing _CPU check
#endif

static char *fileName(          // get name of SRCFILE
    SRCFILE src )               // - source file
{
    char *fname;

    if( CompFlags.error_use_full ) {
        fname = SrcFileFullName( src );
    } else {
        fname = SrcFileName( src );
    }
    return( fname );
}


static void build_file_nesting  // DUMP OUT THE INCLUDE NESTING TRACEBACK
    ( void )
{
    SRCFILE src_file;           // - source file descriptor
    char *fname;
    LINE_NO line;

    if( NULL != err_locn.src_file ) {
        if( SrcFileTraceBackReqd( err_locn.src_file ) ) {
            SrcFileTraceBack( err_locn.src_file );
            CompFlags.log_note_msgs = TRUE;
            if( CompFlags.ew_switch_used ) {
                MsgDisplayArgs( IDEMSGSEV_NOTE_MSG
                              , INF_FILE_LOCATION
                              , fileName( err_locn.src_file ) );
            }
            for( src_file = err_locn.src_file; ; ) {
                src_file = SrcFileIncluded( src_file, &line );
                if( src_file == NULL ) break;
                fname = fileName( src_file );
                MsgDisplayArgs( IDEMSGSEV_NOTE, INF_FILE_NEST, fname, line );
            }
            CompFlags.log_note_msgs = FALSE;
        }
    }
}

static void fmt_inf_hdr         // FORMAT THE INFORMATION HDR
    ( char *hdr_str )
{
    CompFlags.log_note_msgs = TRUE;
    MsgDisplayArgs( IDEMSGSEV_NOTE_MSG
                  , INF_ERR_LOCATION
                  , hdr_str );
    CompFlags.log_note_msgs = FALSE;
}


static void fmt_inf_hdr_switch  // FORMAT THE INFORMATION HDR FOR SWITCH
    ( char *hdr_str             // - header name
    , char *sw_val )            // - switch
{
    CompFlags.log_note_msgs = TRUE;
    MsgDisplayArgs( IDEMSGSEV_NOTE_MSG
                  , INF_SWITCH_LOCATION
                  , hdr_str
                  , sw_val );
    CompFlags.log_note_msgs = FALSE;
}

static void fmt_inf_hdr_sym     // FORMAT THE INFORMATION HDR, WITH SYMBOL
    ( char *hdr_str             // - header name
    , SYMBOL sym )              // - symbol in question
{
    CompFlags.log_note_msgs = TRUE;
    MsgDisplayArgs( IDEMSGSEV_NOTE_MSG
                  , INF_CG_LOCATION
                  , hdr_str
                  , sym );
    CompFlags.log_note_msgs = FALSE;
}

char const *IntlUsageText(      // GET INTERNATIONAL USAGE TEXT
    void )
{
    IntlData *data = internationalData;

    if( data != NULL ) {
        return( data->usage_text );
    }
    return( NULL );
}

static void doDecodeMessage(            // extract message from tables
    char *buff,                 // - buffer
    MSG_NUM msg )               // - message
{
    IntlData *data = internationalData;

    if( data != NULL ) {
        if( msg < data->errors_count ) {
            strcpy( buff, data->errors_text[ msg ] );
            return;
        }
    }
    decodeMsg( buff, msg );
}

static SYMBOL msgBuild(         // BUILD ERROR MESSAGE
    MSG_NUM msgnum,             // - message number
    va_list args,               // - substitution arguments
    VBUF *pbuf )                // - buffer for message
{
    char msgbuf[MAX_MSG+16];    // - contains expanded message

    CBanner();
    doDecodeMessage( msgbuf, msgnum );
    return FormatMsg( pbuf, msgbuf, args );
}


static IDEBool IDEAPI idePrt // PRINT FOR IDE
    ( IDECBHdl hdl              // - handle
    , IDEMsgInfo *info )        // - information
{
    char buffer[512];           // - buffer

    if( err_file == NULL ) {
        ErrFileOpen();
    }
    if( err_file != NULL ) {
        CompFlags.errfile_written = 1;
        IdeMsgFormat( hdl
                    , info
                    , buffer
                    , sizeof( buffer )
                    , &idePrt );
        fputs( buffer, err_file );
        fputc( '\n', err_file );
    }
    return 0;
}


static void ideDisplay          // DISPLAY USING IDE INTERFACE
    ( IDEMsgSeverity severity   // - message severity
    , MSG_NUM msgnum            // - message number
    , char const *msg           // - message
    , TOKEN_LOCN *msg_locn )    // - message location or NULL
{
    IDECallBacks* cbs;          // - pointer to call backs
    IDEMsgInfo inf;             // - message information
    char *fname;                // - file name
    boolean goes_in_err_file;   // - output msg into .err file

    IdeMsgInit( &inf, severity, msg );
    IdeMsgSetMsgNo( &inf, msgnum );
    IdeMsgSetHelp( &inf, "wpperrs.hlp", msgnum + 1 );
    if( CompFlags.ew_switch_used ) {
        IdeMsgSetReadable( &inf );
    }
    if( NULL != msg_locn ) {
        if( msg_locn->src_file != NULL ) {
            fname = fileName( msg_locn->src_file );
            IdeMsgSetSrcFile( &inf, fname );
            IdeMsgSetSrcLine( &inf, msg_locn->line );
            IdeMsgSetSrcColumn( &inf, msg_locn->column );
        }
        notes_locn = *msg_locn;
    }
    goes_in_err_file = FALSE;
    switch( severity ) {
    case IDEMSGSEV_WARNING:
    case IDEMSGSEV_ERROR:
    case IDEMSGSEV_NOTE:
        goes_in_err_file = TRUE;
        break;
    case IDEMSGSEV_BANNER:
    case IDEMSGSEV_DEBUG:
        break;
    case IDEMSGSEV_NOTE_MSG:
        if( CompFlags.log_note_msgs ) {
            goes_in_err_file = TRUE;
        }
        break;
    DbgDefault( "unknown severity" );
    }
    cbs = CompInfo.dll_callbacks;
    if( goes_in_err_file ) {
        if( ! ( CompFlags.eq_switch_used && CompFlags.ide_console_output ) ) {
            (*cbs->PrintWithInfo)( CompInfo.dll_handle, &inf );
        }
        idePrt( CompInfo.dll_handle, &inf );
    } else {
        (*cbs->PrintWithInfo)( CompInfo.dll_handle, &inf );
    }
}


static void setMsgLocation      // SET LOCATION FOR MESSAGE
    ( CTX context )             // - current context
{
    switch( context ) {
      case CTX_FINI :
      case CTX_FUNC_GEN :
      case CTX_CG_FUNC :
      case CTX_CG_OPT :
      case CTX_ENDFILE :
        if( CompFlags.ew_switch_used ) {
      // drops thru
      case CTX_INIT :
      case CTX_CMDLN_ENV :
      case CTX_CMDLN_PGM :
      case CTX_CMDLN_VALID :
            err_locn.src_file = NULL;
            break;
        }
        // drops thru
      case CTX_FORCED_INCS :
      case CTX_SOURCE :
        if( err_locn.src_file == NULL ) {
            if( SrcFilesOpen() ) {
                SrcFileGetTokenLocn( &err_locn );
            } else {
                err_locn.line = SrcLineCount;
                err_locn.column = 0;
                err_locn.src_file = SrcFileCurrent();
            }
        }
        break;
    }
}


void MsgDisplay                 // DISPLAY A MESSAGE
    ( IDEMsgSeverity severity   // - message severity
    , MSG_NUM msgnum            // - message number
    , va_list args )            // - substitution arguments
{

    VBUF buffer;                // - formatting buffer
    SYMBOL sym;                 // - sym requiring location
    TOKEN_LOCN prt_locn;        // - print location
    TOKEN_LOCN *msg_locn;       // - message location
    CTX context;                // - current context
    void *inf;                  // - information about context
    char *inf_prefix;           // - prefix for information
    boolean context_changed;    // - TRUE ==> new context from last time

    context_changed = CtxCurrent( &context, &inf, &inf_prefix );
    setMsgLocation( context );
    prt_locn = err_locn;
    ++reserveDepth;
    VbufInit( &buffer );
    sym = msgBuild( msgnum, args, &buffer );
    switch( severity ) {
      case IDEMSGSEV_ERROR :
      case IDEMSGSEV_WARNING :
        if( CompFlags.ew_switch_used ) {
            switch( context ) {
              case CTX_INIT :
              case CTX_FINI :
              case CTX_CMDLN_VALID :
              case CTX_CG_OPT :
              case CTX_ENDFILE :
                if( context_changed ) {
                    fmt_inf_hdr( inf_prefix );
                }
                break;
              case CTX_CMDLN_ENV :
              case CTX_CMDLN_PGM :
                if( context_changed ) {
                    fmt_inf_hdr_switch( inf_prefix, inf );
                }
                break;
              case CTX_CG_FUNC :
              case CTX_FUNC_GEN :
                if( context_changed ) {
                    fmt_inf_hdr_sym( inf_prefix, inf );
                }
                break;
              case CTX_FORCED_INCS :
              case CTX_SOURCE :
                build_file_nesting();
                break;
              DbgDefault( "Unexpected message context" );
            }
        }
        msg_locn = &prt_locn;
        break;
      case IDEMSGSEV_NOTE :
      case IDEMSGSEV_NOTE_MSG :
        msg_locn = &notes_locn;
        break;
      default :
        msg_locn = NULL;
        break;
    }
    ideDisplay( severity
              , msgnum
              , VbufString( &buffer )
              , msg_locn );
    if( context_changed
     && ! CompFlags.ew_switch_used
     && ( severity == IDEMSGSEV_ERROR
       || severity == IDEMSGSEV_WARNING )
     && ( context == CTX_SOURCE
       || context == CTX_FORCED_INCS )
      ) {
        build_file_nesting();
    }
    VbufFree( &buffer );
    --reserveDepth;
    if( NULL != sym ) {
        notes_locn = sym->locn->tl;
        MsgDisplayArgs( IDEMSGSEV_NOTE
                      , SymIsFunctionTemplateModel( sym )
                            ? INF_TEMPLATE_FN_DECL : INF_SYMBOL_DECLARATION
                      , sym
                      , &sym->locn->tl );
    }
}


void MsgDisplayArgs             // DISPLAY A MESSAGE WITH ARGS
    ( IDEMsgSeverity severity   // - message severity
    , MSG_NUM msgnum            // - message number
    , ... )                     // - arguments
{
    va_list args;

    va_start( args, msgnum );
    MsgDisplay( severity, msgnum, args );
    va_end( args );
}


void MsgDisplayLine             // DISPLAY A BARE LINE
    ( const char *line )        // - the line
{
    ideDisplay( IDEMSGSEV_NOTE_MSG, 0, line, NULL );
}


void MsgDisplayLineArgs         // DISPLAY A BARE LINE, FROM ARGUMENTS
    ( char* seg                 // - the line segments
    , ... )
{
    va_list args;               // - arg list
    char* str;                  // - current segment
    VBUF buffer;                // - buffer

    VbufInit( &buffer );
    va_start( args, seg );
    for( str = seg; str != NULL; str = va_arg( args, char* ) ) {
        VbufConcStr( &buffer, str );
    }
    ideDisplay( IDEMSGSEV_NOTE_MSG, 0, VbufString( &buffer ), NULL );
    va_end( args );
    VbufFree( &buffer );
}


void MsgDisplayBanner           // DISPLAY A BANNER LINE
    ( const char *line )              // - the line
{
    ideDisplay( IDEMSGSEV_BANNER, 0, line, NULL );
}


void AddNoteMessage(            // ADD A NOTE TO A MESSAGE
    MSG_NUM msg_num,            // - message number
    ... )                       // - error message arguments
{
    va_list args;

    va_start( args, msg_num );
    MsgDisplay( IDEMSGSEV_NOTE, msg_num, args );
    va_end( args );
}


static void prtMsg(             // PRINT A MESSAGE
    int warn_level,             // - warning level
    MSG_NUM msgnum,             // - message number
    va_list args,               // - substitution arguments
    unsigned warn_inc )         // - amount to inc WngCount
{
    IDEMsgSeverity severity;    // - message severity

    if( CompFlags.cpp_output )  return;
    if( warn_level == -1 ) {
        err_locn = notes_locn;
        severity = IDEMSGSEV_NOTE;
    } else if( warn_level == 0 ) {
        ++ErrCount;
        severity = IDEMSGSEV_ERROR;
    } else {
        WngCount += warn_inc;
        severity = IDEMSGSEV_WARNING;
    }
    MsgDisplay( severity, msgnum, args );
    if( warn_level != -1 && msgnum != ERR_EXCEEDED_LIMIT ) {
        CtxPostContext();
    }
}


static void fileErase(          // ERASE ERROR FILE
    const char *name )          // - file name
{
    if( name != NULL ) {
        remove( name );
    }
}

static void reserveRelease( void )
{
    void *p;

    if( reserveDepth == 0 ) {
        return;
    }
    p = reserveMem;
    if( p != NULL ) {
        reserveMem = NULL;
        CMemFree( p );
        if( reserveSize > RESERVE_MIN ) {
            reserveSize >>= 1;
            reserveMem = CMemAlloc( reserveSize );
        }
    }
}


void ErrFileOpen( void )        // OPEN ERROR FILE
{
    char *buf;                  // - file name

    if( !CompFlags.errfile_opened && SrcFName != NULL ) {
        // we want to keep retrying until we get a source file name
        CompFlags.errfile_opened = TRUE;
        buf = IoSuppOutFileName( OFT_ERR );
        if( buf != NULL ) {
            fileErase( buf );
            err_file = SrcFileFOpen( buf, SFO_WRITE_TEXT );
            if( err_file != NULL ) {
                IoSuppSetBuffering( err_file, 128 );
            }
        }
    }
}


void ErrFileErase( void )       // ERASE ERROR FILE
{
    if( err_file == NULL ) {
        fileErase( IoSuppOutFileName( OFT_ERR ) );
    }
}


void RegisterSuicideCallback(   // REGISTER A FUNCTION TO BE CALLED BEFORE CSuicide
    SUICIDE_CALLBACK *ctl )     // - call-back control block
{
    RingAppend( &suicideCallbacks, ctl );
}


void CSuicide(                  // COMMIT SUICIDE
    void )
{
    void *env;
    SUICIDE_CALLBACK *ctl;

    CompFlags.compile_failed = TRUE;
    if( Environment ) {
        env = Environment;
        Environment = NULL;
        RingIterBeg( suicideCallbacks, ctl ) {
            ctl->call_back();
        } RingIterEnd( ctl )
        longjmp( env, 1 );
    }
    CppExit(1);
}


void SetErrLoc(                 // SET ERROR LOCATION
    TOKEN_LOCN *locn )          // - error location
{
    if( locn == NULL ) {
        err_locn.src_file = NULL;
    } else {
        err_locn = *locn;
    }
}


static boolean okToPrintMsg     // SEE IF OK TO PRINT MESSAGE
    ( MSG_NUM msgnum            // - message number
    , int *plevel )             // - addr[ level ]
{
    boolean print_err;
    int level;

    print_err = TRUE;
    level = msg_level[ msgnum ] & 0x0F;
    switch( msg_level[ msgnum ] >> 4 ) {
      case MSG_TYPE_INFO :
        level = -1;
        break;
      case MSG_TYPE_ANSIERR :
      case MSG_TYPE_ANSIWARN :
        print_err = ! CompFlags.extensions_enabled;
        break;
      case MSG_TYPE_ANSI :
        if( ! CompFlags.extensions_enabled ) {
            level = 0;
        }
        break;
      case MSG_TYPE_WARNING :
      case MSG_TYPE_ERROR :
        break;
    }
    *plevel = level;
    return( print_err );
}

boolean MsgWillPrint(           // TEST WHETHER A MESSAGE WILL BE SEEN
    MSG_NUM msgnum )            // - message number
{
    int level;                  // - warning level of message

    if( ! okToPrintMsg( msgnum, &level ) ) {
        return( FALSE );
    }
    return( level <= WngLevel );
}

static msg_status_t doError(    // ISSUE ERROR
    MSG_NUM msgnum,             // - message number
    va_list args,               // - varargs
    unsigned warn_inc )         // - amount to inc WngCount (if warning)
{
    msg_status_t retn;          // - message status
    int level;                  // - warning level of message
    struct {
        unsigned print_err : 1; // - TRUE ==> print the message
        unsigned too_many : 1;  // - TRUE ==> too many messages
    } flag;

#ifndef NDEBUG
    fflush(stdout);
    fflush(stderr);
#endif

    retn = MS_NULL;
    if( ! errLimitExceeded ) {
        flag.too_many = TRUE;
        flag.print_err = okToPrintMsg( msgnum, &level );
        if( suppressCount > 0 ) {
            /* suppressed message */
            if( flag.print_err && ( level == 0 ) ) {
                internalErrCount++;
            }
            return MS_NULL;
        } else if( ErrLimit == -1 ) {
            /* unlimited messages */
            flag.too_many = FALSE;
        } else if( ErrCount < ErrLimit ) {
            /* haven't hit the limit */
            flag.too_many = FALSE;
        } else if( msgnum == ERR_EXCEEDED_LIMIT ) {
            /* we hit the limit and we want to diagnose the condition */
            flag.too_many = FALSE;
        } else if( ErrCount == ErrLimit ) {
            /* have hit the limit */
            if( !flag.print_err || level != 0 ) {
                /* this message isn't an error; it's a warning or info */
                flag.too_many = FALSE;
            }
        }
        if( ! flag.too_many ) {
            if( flag.print_err && ( level <= WngLevel ) ) {
                prtMsg( level, msgnum, args, warn_inc );
                retn |= MS_PRINTED;
            }
            if( level != 0 ) {
                /* useful if ANSI requires error but is usually a warning */
                retn |= MS_WARNING;
            }
        } else {
            CErr( ERR_EXCEEDED_LIMIT );
            errLimitExceeded = TRUE;
            CSuicide();
        }
    }
    /* turn off SetErrLoc setting */
    err_locn.src_file = NULL;
    return retn;
}

msg_status_t CErr(              // ISSUE ERROR
    MSG_NUM msgnum,             // - message number
    ... )                       // - parameters for error
{
    va_list args;
    msg_status_t s;

    va_start( args, msgnum );
    s = doError( msgnum, args, 1 );
    va_end( args );
    return( s );
}

msg_status_t CWarnDontCount(    // ISSUE WARNING BUT DON'T COUNT IT
    MSG_NUM msgnum,             // - message number
    ... )                       // - parameters for warning
{
    va_list args;
    msg_status_t s;

    va_start( args, msgnum );
    s = doError( msgnum, args, 0 );
    va_end( args );
    return( s );
}


msg_status_t CErr1(             // ISSUE ERROR (NO PARAMETERS)
    MSG_NUM msgnum )            // - message number
{
    return CErr( msgnum );
}


msg_status_t CErr2(             // ISSUE ERROR (int PARAMETER)
    MSG_NUM msgnum,             // - message number
    int p1 )                    // - parameter
{
    return CErr( msgnum, p1 );
}


msg_status_t CErr2p(            // ISSUE ERROR (char* PARAMETER)
    MSG_NUM msgnum,             // - message number
    void const *p1 )            // - parameter
{
    return CErr( msgnum, p1 );
}

void CFatal(                    // ISSUE ERROR AND COMMIT SUICIDE
    char *msg )                 // - error message
{
    CErr2p( ERR_FATAL_ERROR, msg );
    CSuicide();
}


void InfMsgPtr(                 // INFORMATION MESSAGE, PTR ARG.
    MSG_NUM msgnum,             // - message number
    void const *p1 )            // - extra information
{
    CErr( msgnum, p1 );
}


void InfMsgInt(                 // INFORMATION MESSAGE, INT ARG.
    MSG_NUM msgnum,             // - message number
    int p1 )                    // - extra information
{
    CErr( msgnum, p1 );
}


void CErrSuppress(
    error_state_t *save )
{
    suppressCount++;
    *save = internalErrCount;
}

void CErrSuppressRestore(
    unsigned count )
{
    suppressCount += count;
}

unsigned CErrUnsuppress(
    void )
{
    unsigned val = suppressCount;
    suppressCount = 0;
    return val;
}

boolean CErrSuppressedOccurred(
    error_state_t *previous_state )
{
    suppressCount--;
    if( *previous_state != internalErrCount ) {
        internalErrCount = *previous_state;
        return( TRUE );
    }
    return( FALSE );
}

void CErrCheckpoint(
    error_state_t *save )
{
    *save = ErrCount;
}

boolean CErrOccurred(
    error_state_t *previous_state )
{
    if( *previous_state != ErrCount ) {
        return( TRUE );
    }
    return( FALSE );
}

static boolean warnLevelValidate( // VALIDATE WARNING LEVEL
    int level )                 // - level to be validated
{
    boolean retn;               // - return: TRUE ==> good level

    if( ( level < 0 )
      ||( level > 9 ) ) {
        CErr1( ERR_PRAG_WARNING_BAD_LEVEL );
        retn = FALSE;
    } else {
        retn = TRUE;
    }
    return retn;
}


static void changeLevel(        // EFFECT A LEVEL CHANGE
    int level,                  // - new level
    MSG_NUM msgnum )            // - message number
{
    DbgAssert( msgnum < ARRAY_SIZE( msg_level ) );
    if( NULL == orig_err_levs ) {
        orig_err_levs = CMemAlloc( sizeof( msg_level ) );
        memcpy( orig_err_levs, msg_level, sizeof( msg_level ) );
    }
    msg_level[ msgnum ] = ( msg_level[ msgnum ] & 0xF0 ) + level;
}

void WarnChangeLevel(           // CHANGE WARNING LEVEL FOR A MESSAGE
    int level,                  // - new level
    MSG_NUM msgnum )            // - message number
{
    if( msgnum >= ARRAY_SIZE( msg_level ) ) {
        CErr2( ERR_PRAG_WARNING_BAD_MESSAGE, msgnum );
        return;
    }
    switch( msg_level[ msgnum ] >> 4 ) {
      case MSG_TYPE_ERROR :
      case MSG_TYPE_INFO :
      case MSG_TYPE_ANSIERR :
        CErr2( ERR_PRAG_WARNING_BAD_MESSAGE, msgnum );
        break;
      case MSG_TYPE_WARNING :
      case MSG_TYPE_ANSI :
      case MSG_TYPE_ANSIWARN :
        changeLevel( level, msgnum );
        break;
    }
}

void WarnChangeLevels(          // CHANGE WARNING LEVELS FOR ALL MESSAGES
    int level )                 // - new level
{
    MSG_NUM index;              // - index for number

    if( warnLevelValidate( level ) ) {
        for( index = 0
           ; index < ARRAY_SIZE( msg_level )
           ; ++ index ) {
            switch( msg_level[ index ] >> 4 ) {
              case MSG_TYPE_WARNING :
              case MSG_TYPE_ANSI :
              case MSG_TYPE_ANSIWARN :
                changeLevel( level, index );
                break;
            }
        }
    }
}


void InfClassDecl(              // GENERATE CLASS-DECLARATION NOTE
    TYPE cltype )               // - a class type
{
    TOKEN_LOCN* locn;           // - location for class definition

    cltype = ClassTypeForType( cltype );
    locn = LocnForClass( cltype );
    if( NULL != locn ) {
        CErr( INF_CLASS_DECLARATION
            , cltype->u.c.info->name
            , locn );
    }
}


void InfMacroDecl(              // GENERATE MACRO-DECLARATION NOTE
    void* parm )                // - macro definition
{
    MEPTR mdef;                 // - macro definition

    mdef = parm;
    if( mdef->macro_flags & MFLAG_USER_DEFINED ) {
        CErr( INF_MACRO_DECLARATION, mdef->macro_name, &mdef->defn );
    }
}


void InfSymbolDeclaration(      // GENERATE SYMBOL-DECLARATION NOTE
    SYMBOL sym )                // - a symbol
{
    CErr( SymIsFunctionTemplateModel( sym )
            ? INF_TEMPLATE_FN_DECL : INF_SYMBOL_DECLARATION
        , sym
        , &sym->locn->tl );
}


void InfSymbolAmbiguous(        // GENERATE SYMBOL-AMBIGUITY NOTE
    SYMBOL sym )                // - a symbol
{
    CErr( SymIsFnTemplateMatchable( sym )
            ? INF_TEMPLATE_FN_AMBIGUOUS : INF_FUNC_AMBIGUOUS
        , sym
        , &sym->locn->tl );
}


void InfSymbolRejected(         // GENERATE SYMBOL-REJECTION NOTE
    SYMBOL sym )                // - a symbol
{
    CErr( SymIsFunctionTemplateModel( sym )
            ? INF_TEMPLATE_FN_REJECTED : INF_FUNC_REJECTED
        , sym
        , &sym->locn->tl );
}

static void openDefFile( void )
{
    char *def_fname;

    if( SrcFName != NULL ) {
        def_fname = IoSuppOutFileName( OFT_DEF );
        fileErase( def_fname );
        DefFile = SrcFileFOpen( def_fname, SFO_WRITE_TEXT );
        if( DefFile != NULL ) {
            IoSuppSetBuffering( DefFile, 128 );
        }
    }
}


void DefAddPrototype(           // ADD PROTOTYPE FOR SYMBOL TO .DEF FILE
    SYMBOL fn )                 // - function
{
    VBUF proto;

    if( DefFile == NULL ) {
        openDefFile();
    }
    if( DefFile == NULL ) {
        return;
    }
    if( SymIsFnTemplateMatchable( fn ) ) {
        return;
    }
    if( fn->id == SC_STATIC ) {
        return;
    }
    if( CompFlags.use_base_types ) {
        FormatFnDefn( fn, &proto );
    } else {
        FormatFnDefnWithTypedefs( fn, &proto );
    }
    fprintf( DefFile
           , "//#line \"%s\" %u\n"
           , fileName( fn->locn->tl.src_file )
           , fn->locn->tl.line );
    fprintf( DefFile, "extern %s;\n", VbufString( &proto ) );
    VbufFree( &proto );
}

unsigned ErrPCHVersion(         // PROVIDE A VERSION NUMBER FOR THE ERROR MESSAGES
    void )
{
    return sizeof( msg_level );
}


static void errFileInit(        // INITIALIZE FOR NO ERROR FILE
    INITFINI* defn )            // - definition
{
    defn = defn;
    errLimitExceeded = FALSE;
    err_file = NULL;
    err_locn.src_file = NULL;
    suicideCallbacks = NULL;
    orig_err_levs = NULL;
    reserveSize = RESERVE_MAX;
    reserveMem = CMemAlloc( reserveSize );
    CMemRegisterCleanup( reserveRelease );
    internationalData = LoadInternationalData( INTL_NAME );
}


static void errFileFini(        // CLOSE ERROR FILE
    INITFINI* defn )            // - definition
{
    defn = defn;
    if( IoSuppCloseFile( &err_file ) ) {
        if( ! CompFlags.errfile_written ) {
            fileErase( IoSuppOutFileName( OFT_ERR ) );
        }
    }
    CMemFree( ErrorFileName );
    CMemFreePtr( &reserveMem );
    if( NULL != orig_err_levs ) {
        memcpy( msg_level, orig_err_levs, sizeof( msg_level ) );
        CMemFreePtr( &orig_err_levs );
    }
    FreeInternationalData( internationalData );
}


INITDEFN( error_file, errFileInit, errFileFini )

pch_status PCHReadErrWarnData( void )
{
    char            tmp_buff[sizeof( msg_level )];
    unsigned char   *orig_levels;
    unsigned char   *p;
    unsigned char   *o;
    unsigned char   *stop;

    PCHReadVar( tmp_buff );
    if( NULL != orig_err_levs ) {
        orig_levels = orig_err_levs;
    } else {
        orig_levels = msg_level;
    }
    stop = &tmp_buff[ sizeof( msg_level ) ];
    for( p = tmp_buff, o = orig_levels; p < stop; ++p, ++o ) {
        if( *p != *o ) {
            // reflect a change from the header file into current levels
            changeLevel( *p & 0x0f, p - tmp_buff );
        }
    }
    return( PCHCB_OK );
}

pch_status PCHWriteErrWarnData( void )
{
    // NYI: save snapshot of msg_level before header file is read
    // so that we can write out a msg_level that indicates the
    // changes made by the header file

    PCHWriteVar( msg_level );
    return( PCHCB_OK );
}

pch_status PCHInitErrWarnData( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}

pch_status PCHFiniErrWarnData( boolean writing )
{
    writing = writing;
    return( PCHCB_OK );
}
