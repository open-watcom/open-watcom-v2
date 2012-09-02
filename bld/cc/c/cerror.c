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
* Description:  Error and warning message output.
*
****************************************************************************/


#include "cvars.h"
#include "iopath.h"
#include <stdarg.h>

/*
 * Types of post-processing messages (informational notes)
 */
typedef enum {
    POSTLIST_SYMBOL,     /* location of previously defined symbol */
    POSTLIST_TWOTYPES_1, /* type mismatch between two types ( source/target ) - print them */
    POSTLIST_TWOTYPES_2  /* type mismatch between two types ( first/second ) - print them */
} postlist_type;

typedef struct ErrPostList
{
    struct ErrPostList  *next;
    postlist_type       type;

    union {
        /* POSTLIST_SYMBOL */
        struct {
            char        *sym_name;
            char        *sym_file;
            unsigned    sym_line;
        } s;
        /* POSTLIST_TWOTYPES */
        struct {
            TOKEN       opr;
            TYPEPTR     types[ 2 ];
        } o;
    };
} ErrPostList;

static ErrPostList  *PostList;
static unsigned     error_line = 0;
static char         *error_fname = NULL;

#if 0
static char const WngLvls[] = {
#define warn(code,level) level,
#include "cwngs.h"
#undef warn
};
#endif

void OpenErrFile( void )
{
    char        *name;

    if( SrcFName != NULL ) {                        /* 15-dec-88 */
        name = ErrFileName();
        if( name != NULL ) {
            ErrFile = fopen( name, "w" );
            if( ErrFile != NULL ) {
                setvbuf( ErrFile, CPermAlloc( 32 ), _IOFBF, 32 );
            }
        }
    }
}

static int MsgDisabled( int msgnum )
{
    if( MsgFlags != NULL ) {
        if( MsgFlags[ msgnum >> 3 ]  &  (1 << (msgnum & 7)) ) {
            return( 1 );
        }
    }
    return( 0 );
}

// fill cmsg_info struct
static void CMsgInfo( cmsg_info *info, int parmno, msg_codes msgnum, va_list args )
{
    char        *fname;
    unsigned    line;
    char const  *msgstr;
    int         prefix_len;

    info->msgnum = msgnum;
//  CMsgSetClass( info, msgnum );
    info->col   = 0;
    switch( msgnum ) {
    case ERR_INVALID_MEMORY_MODEL:
    case ERR_INVALID_OPTION:
    case ERR_INVALID_OPTIMIZATION:
        /* no location for error message */
        line = 0;
        fname = NULL;
        break;
    default:
        if( error_fname != NULL ) {
            fname = error_fname;
            line = error_line;
        } else {
            fname = FileIndexToCorrectName( TokenLoc.fno );
            line = TokenLoc.line;
        }
    }
    prefix_len = 0;
    if( parmno ) {
        msgstr = CGetMsgStr( PHRASE_PARAM );
        prefix_len = snprintf( info->msgtxt, MAX_MSG_LEN, msgstr, parmno );
        info->msgtxt[prefix_len++] = ' ';
    }
    msgstr = CGetMsgStr( msgnum );
    _vsnprintf( info->msgtxt + prefix_len, MAX_MSG_LEN - prefix_len, msgstr, args );
    info->msgtxt[ MAX_MSG_LEN - 1 ] = '\0';
    info->line = line;
    info->fname = fname;
}

static char const *MsgClassPhrase( cmsg_class class )
{
    msg_codes       msgcode = PHRASE_ERROR;     // just for init.
    char const      *phrase;

    switch( class ) {
    case CMSG_INFO:
        msgcode = PHRASE_NOTE;
        break;
    case CMSG_WARN:
        msgcode = PHRASE_WARNING;
        break;
    case CMSG_ERRO:
        msgcode = PHRASE_ERROR;
        break;
    }
    phrase = CGetMsgStr( msgcode );
    return( phrase );
}

// format message with line & file int buff
void FmtCMsg( char *buff, cmsg_info *info )
{
    int         len;
    char const  *phrase;
    char const  *code_prefix;

    len = 0;
    if( info->line != 0 ) {
        if( info->fname != NULL ) {
            len += _snprintf( &buff[ len ], MAX_MSG_LEN - len, "%s(%u): ", info->fname, info->line );
        }
    } else {
        buff[ 0 ] = '\0';
    }
    code_prefix = CGetMsgPrefix( info->msgnum );
    phrase = MsgClassPhrase( info->class );
    len += _snprintf( &buff[ len ], MAX_MSG_LEN - len, "%s %s%03d: ", phrase, code_prefix, info->msgnum );
}

// print message to streams
static void OutMsg( cmsg_info  *info )
{
    char        pre[ MAX_MSG_LEN ]; //actual message text

    if( ErrFile == NULL )
        OpenErrFile();
    if( CompFlags.no_conmsg == 0 )
        ConsErrMsg( info );
    if( ErrFile != NULL ) {
        FmtCMsg( pre, info );
        fputs( pre, ErrFile );
        fputs( info->msgtxt, ErrFile );
        fputc( '\n', ErrFile );
        CompFlags.errfile_written = 1;
    }
}

static void PrintType( int msg, TYPEPTR typ )
{
    char    *text;

    if( typ == NULL )
        return;

    text = DiagGetTypeName( typ );
    CInfoMsg( msg, text );
    CMemFree( text );
}

static void PrintPostNotes( void )
{
    while( PostList != NULL ) {
        switch( PostList->type ) {
        case POSTLIST_SYMBOL:
            CInfoMsg( INFO_SYMBOL_DECLARATION, PostList->s.sym_name, PostList->s.sym_file, PostList->s.sym_line );
            break;
        case POSTLIST_TWOTYPES_1:
            PrintType( INFO_SRC_CNV_TYPE, PostList->o.types[ 0 ] );
            PrintType( INFO_TGT_CNV_TYPE, PostList->o.types[ 1 ] );
            break;
        case POSTLIST_TWOTYPES_2:
            CInfoMsg( INFO_OPERATOR, Tokens[PostList->o.opr] );
            PrintType( INFO_FIRST_OPND_TYPE, PostList->o.types[ 0 ] );
            PrintType( INFO_SECOND_OPND_TYPE, PostList->o.types[ 1 ] );
            break;
        }
        SetDiagPop();
    }
}

// Output error message
static void CErr( int parmno, int msgnum, ... )
{
    va_list     args1;
    cmsg_info   info;

    if( CompFlags.cpp_output )
        return;
    info.class = CMSG_ERRO;
    va_start( args1, msgnum );
    if( ErrLimit == -1  ||  ErrCount < ErrLimit ) {
        CMsgInfo( &info, parmno, msgnum, args1 );
        OutMsg( &info );
        ++ErrCount;
        PrintPostNotes();
    } else {
        CMsgInfo( &info, 0, ERR_TOO_MANY_ERRORS, args1 );
        OutMsg( &info );
        CSuicide();
    }
}

void CErr1( int msgnum )
{
    CErr( 0, msgnum );
}

void CErr2( int msgnum, int p1 )
{
    CErr( 0, msgnum, p1 );
}

void CErr2p( int msgnum, const char *p1 )
{
    CErr( 0, msgnum, p1 );
}

void CErr3p( int msgnum, const char *p1, const char *p2 )
{
    CErr( 0, msgnum, p1, p2 );
}

void CErr4p( int msgnum, const char *p1, const char *p2, const char *p3 )
{
    CErr( 0, msgnum, p1, p2, p3 );
}

void CErrP1( int parmno, int msgnum )
{
    CErr( parmno, msgnum );
}


// Out warning message
static void CWarn( int parmno, int level, int msgnum, ... )
{
    va_list     args1;
    cmsg_info   info;

    if( CompFlags.cpp_output )
        return;
    if( ! MsgDisabled( msgnum ) ) {
        if( level <= WngLevel ) {
            info.class = CMSG_WARN;
            va_start( args1, msgnum );
            CMsgInfo( &info, parmno, msgnum, args1 );
            OutMsg( &info );
            ++WngCount;
            PrintPostNotes();
        }
    }
}

void CWarn1( int level, int msgnum )
{
    CWarn( 0, level, msgnum );
}

void CWarn2( int level, int msgnum, int p1 )
{
    CWarn( 0, level, msgnum, p1 );
}

void CWarn2p( int level, int msgnum, const char *p1 )
{
    CWarn( 0, level, msgnum, p1 );
}

void CWarnP1( int parmno, int level, int msgnum )
{
    CWarn( parmno, level, msgnum );
}


void CInfoMsg( int msgnum, ... )
{
    va_list     args1;
    cmsg_info   info;

    if( CompFlags.cpp_output )
        return;
    if( MsgDisabled( msgnum ) )
        return;
    info.class = CMSG_INFO;
    va_start( args1, msgnum );
    CMsgInfo( &info, 0, msgnum, args1 );
    OutMsg( &info );
    va_end( args1 );
}


// Output pre-compiled header Note
void PCHNote( int msgnum, ... )
{
    va_list     args1;
    char        msgbuf[ MAX_MSG_LEN ];
    char const  *msgstr;

    if( !CompFlags.no_pch_warnings ) {
        msgstr = CGetMsgStr( msgnum );
        va_start( args1, msgnum );
        _vsnprintf( msgbuf, MAX_MSG_LEN, msgstr, args1 );
        va_end( args1 );
        NoteMsg( msgbuf );
    }
}


void SetErrLoc( source_loc *src_loc )
{
    error_fname = FileIndexToCorrectName( src_loc->fno );
    error_line = src_loc->line;
}


void InitErrLoc( void )
{
    error_fname = NULL;
}


void CSuicide( void )
{
    if( Environment != NULL ) {
        longjmp( *Environment, 1 );
    }
    MyExit(1);
}


#if 0
//doesn't work in general as phases are used in both errror and warnings
static void CMsgSetClass( cmsg_info *info, msg_codes msgnum )
{
    msgtype     kind;
    cmsg_class  class;

    kind = CGetMsgType( msgnum );
    switch( kind ) {
    case msgtype_ERROR:
    case msgtype_ANSIERR:
        class = CMSG_ERRO;
        break;
    case msgtype_WARNING:
    case msgtype_ANSIWARN:
        class = CMSG_WARN;
        break;
    case msgtype_INFO:
    case msgtype_ANSI:
    case msgtype_STYLE:
        class = CMSG_INFO;
        break;
    }
    info->class = class;
}

// fill info
static void DoMsgInfo( msg_codes msgnum )
{
    cmsg_info   sinfo;
    cmsg_info   *info;
    char        pre[ MAX_MSG_LEN ]; //actual message text
    unsigned    line;

    info = &sinfo;
    info->msgnum = msgnum;
    CMsgSetClass( info, msgnum );
    info->col = 0;
    line = 0;
    CGetMsg( info->msgtxt, msgnum );
    info->line = line;
    info->fname = NULL;
    FmtCMsg( pre, info );
    printf( "%s%s\n", pre,info->msgtxt );
}

void DumpAllMsg( void ) {
#define MSG_DEF( name, group, kind, level, group_index ) DoMsgInfo( name );
    MSG_DEFS
#undef MSG_DEF
}
#endif

/*
 * post-processing messages (informational notes)
 */

static struct ErrPostList *NewPostList( postlist_type type )
{
    struct ErrPostList  *np;

    np = CMemAlloc( sizeof( *np ) );
    np->next = PostList;
    np->type = type;
    PostList = np;
    return( np );
}

void SetDiagSymbol( SYMPTR sym, SYM_HANDLE handle )
{
    struct ErrPostList  *np;

    np = NewPostList( POSTLIST_SYMBOL );
    np->s.sym_name = SymName( sym, handle );
    if( np->s.sym_name == NULL )
        np->s.sym_name = "???";
    np->s.sym_file = FileIndexToCorrectName( sym->src_loc.fno );
    np->s.sym_line = sym->src_loc.line;
}

void SetDiagEnum( ENUMPTR ep )
{
    struct ErrPostList  *np;

    np = NewPostList( POSTLIST_SYMBOL );
    np->s.sym_name = ep->name;
    np->s.sym_file = FileIndexToCorrectName( ep->src_loc.fno );
    np->s.sym_line = ep->src_loc.line;
}

void SetDiagMacro( MEPTR mp )
{
    struct ErrPostList  *np;

    np = NewPostList( POSTLIST_SYMBOL );
    np->s.sym_name = mp->macro_name;
    np->s.sym_file = FileIndexToCorrectName( mp->src_loc.fno );
    np->s.sym_line = mp->src_loc.line;
}

void SetDiagType1( TYPEPTR typ_source )
{
    SetDiagType2( NULL, typ_source );
}

void SetDiagType2( TYPEPTR typ_target, TYPEPTR typ_source )
{
    struct ErrPostList  *np;

    np = NewPostList( POSTLIST_TWOTYPES_1 );
    np->o.opr = 0;
    np->o.types[ 0 ] = typ_source;
    np->o.types[ 1 ] = typ_target;
}

void SetDiagType3( TYPEPTR typ_first, TYPEPTR typ_second, TOKEN opr )
{
    struct ErrPostList  *np;

    np = NewPostList( POSTLIST_TWOTYPES_2 );
    np->o.opr = opr;
    np->o.types[ 0 ] = typ_first;
    np->o.types[ 1 ] = typ_second;
}

void SetDiagPop( void )
{
    struct ErrPostList  *np;

    np = PostList;
    if( np != NULL ) {
        PostList = np->next;
        CMemFree( np );
    }
}
