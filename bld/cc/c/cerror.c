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


#include "cvars.h"
#include "iopath.h"
#include <stdarg.h>

#if 0
static char const WngLvls[] = {
#define warn(code,level) level,
#include "cwngs.h"
#undef warn
};
#endif
static void CMsgInfo( cmsg_info *info, msg_codes msgnum, va_list args ){
// fill info
    char        *fname;
    unsigned    line;
    char const  *msgstr;

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
        if( SymLoc != NULL ) {
            fname = SymLoc;
            line = ErrLine;
        } else {
            fname = ErrFName;
            if( SrcFile == NULL ) {
                line = SrcLineCount;
            } else {
                line = TokenLine;
            }
        }
    }
    msgstr = CGetMsgStr( msgnum );
    _vbprintf( info->msgtxt,MAX_MSG_LEN, msgstr, args );
    info->line = line;
    info->fname = fname;
}
static char const *MsgClassPhrase( cmsg_class class )
{
    msg_codes msgcode;
    char const *phrase;

    switch( class ){
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

void FmtCMsg( char *buff, cmsg_info *info ){
// format message with line & file int buff
    int         len;
    char  const *phrase;
    char  const *code_prefix;

    len = 0;
    if( info->line != 0 ) {
        if( info->fname != NULL ) {
            len += _bprintf( &buff[len], MAX_MSG_LEN-len,  "%s(%u): ",
                      info->fname, info->line  );
        }
    }else{
        buff[0] = '\0';
    }
    code_prefix = CGetMsgPrefix( info->msgnum );
    phrase = MsgClassPhrase( info->class );
    len += _bprintf(&buff[len], MAX_MSG_LEN-len, "%s %s%03d: ",
              phrase, code_prefix, info->msgnum );
}

static void OutMsg( cmsg_info  *info ){
// print message to streams
    char        pre[MAX_MSG_LEN]; //actual message text

    if( ErrFile == NULL ) OpenErrFile();
    if( CompFlags.no_conmsg == 0 ){
        ConsErrMsg( info );
    }
    if( ErrFile ) {
        FmtCMsg( pre, info );
        fputs( pre, ErrFile );
        fputs( info->msgtxt, ErrFile );
        fputc( '\n', ErrFile );
        CompFlags.errfile_written = 1;
    }
    SymLoc = NULL;
}

void CErr1( int msgnum )
{
    CErr( msgnum );
}


void CErr2( int msgnum, int p1 )
{
    CErr( msgnum, p1 );
}


void CErr2p( int msgnum, char *p1 )
{
    CErr( msgnum, p1 );
}


void CErr( int msgnum, ... ){
// Output error message
    va_list   args1;
    cmsg_info info;

    if( CompFlags.cpp_output )  return;             /* 29-sep-90 */
#if 0   //shoudn't allow an error to be disabled
    if( MsgDisabled( msgnum ) ) {                       /* 18-jun-92 */
        SymLoc = NULL;                          /* 27-sep-92 */
        return;
    }
#endif
    info.class = CMSG_ERRO;
    va_start( args1, msgnum );
    if( ErrLimit == -1  ||  ErrCount < ErrLimit ) {
        CMsgInfo( &info, msgnum, args1 );
        va_end( args1 );
        OutMsg( &info );
        ++ErrCount;
    } else {
        CMsgInfo( &info, ERR_TOO_MANY_ERRORS, args1 );
        OutMsg( &info );
        va_end( args1 );
        CSuicide();
    }
}


void CWarn1( int level, int msgnum )
{
    CWarn( level, msgnum );
}


void CWarn2( int level, int msgnum, int p1 )
{
    CWarn( level, msgnum, p1 );
}


void CWarn( int level, int msgnum, ... ){
// Out warning message
    va_list     args1;
    cmsg_info   info;

    if( CompFlags.cpp_output )  return;             /* 29-sep-90 */
    if( ! MsgDisabled( msgnum ) ) {                     /* 18-jun-92 */
        if( level <= WngLevel ) {
            info.class = CMSG_WARN;
            va_start( args1, msgnum );
            CMsgInfo( &info, msgnum, args1 );
            va_end( args1 );
            OutMsg( &info );
            ++WngCount;
        }
    }
    SymLoc = NULL;
}


void CInfoMsg( int msgnum, ... )
{
    va_list     args1;
    cmsg_info   info;
    char        pre[MAX_MSG_LEN];

    if( CompFlags.cpp_output )  return;             /* 29-sep-90 */
    if( MsgDisabled( msgnum ) ) {                       /* 18-jun-92 */
        SymLoc = NULL;                          /* 27-sep-92 */
        return;
    }
    info.class = CMSG_INFO;
    va_start( args1, msgnum );
    CMsgInfo( &info, msgnum, args1 );
    va_end( args1 );
    ConsErrMsg( &info );
    if( ErrFile != NULL  &&  WngLevel >= 4 ) {
        FmtCMsg( pre, &info );
        fputs( pre, ErrFile );
        fputs( info.msgtxt, ErrFile );
        fputc( '\n', ErrFile );
        CompFlags.errfile_written = 1;
    }
}

void CNote( int msgnum, ... ){
// Output Note
    va_list   args1;
    char        msgbuf[MAX_MSG_LEN];
    char  const *msgstr;

    va_start( args1, msgnum );
    msgstr = CGetMsgStr( msgnum );
    _vbprintf( msgbuf,MAX_MSG_LEN, msgstr, args1 );
    NoteMsg( msgbuf );
}

void PCHNote( int msgnum, ... ){
// Output Note
    va_list   args1;
    char        msgbuf[MAX_MSG_LEN];
    char  const *msgstr;

    if( !CompFlags.no_pch_warnings ) {
        va_start( args1, msgnum );
        msgstr = CGetMsgStr( msgnum );
        _vbprintf( msgbuf,MAX_MSG_LEN, msgstr, args1 );
        NoteMsg( msgbuf );
    }
}

void SetSymLoc( SYMPTR sym )
{
    FNAMEPTR flist;

    flist = FileIndexToFName( sym->defn_file_index );
    if( CompFlags.ef_switch_used ){
        SymLoc = FNameFullPath( flist );
    }else{
        SymLoc = flist->name;
    }
    ErrLine = sym->d.defn_line;
}


void SetErrLoc( char *fname, unsigned line_num )
{
    SymLoc = fname;
    ErrLine = line_num;
}




void OpenErrFile()
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


void CSuicide()
{
    if( Environment ) {
        longjmp( Environment, 1 );
    }
    MyExit(1);
}

local int MsgDisabled( int msgnum )                     /* 18-jun-92 */
{
    if( MsgFlags != NULL ) {
        if( MsgFlags[ msgnum >> 3 ]  &  (1 << (msgnum & 7)) ) {
            return( 1 );
        }
    }
    return( 0 );
}
#if 0
//doesn't work in general as phases are used in both errror and warnings
static void CMsgSetClass( cmsg_info *info, msg_codes msgnum )
{
    msgtype     kind;
    cmsg_class  class;
    kind = CGetMsgType( msgnum );
    switch( kind ){
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
static void DoMsgInfo(  msg_codes msgnum ){
// fill info
    cmsg_info  sinfo;
    cmsg_info  *info;
    char        pre[MAX_MSG_LEN]; //actual message text
    unsigned    line;

    info = &sinfo;
    info->msgnum = msgnum;
    CMsgSetClass( info, msgnum );
    info->col   = 0;
    line = 0;
    CGetMsg( info->msgtxt, msgnum );
    info->line = line;
    info->fname = NULL;
    FmtCMsg( pre, info );
    printf( "%s%s\n", pre,info->msgtxt );
}
void DumpAllMsg( void ){
#define MSG_DEF( name, group, kind, level, group_index ) DoMsgInfo( name );
    MSG_DEFS
#undef MSG_DEF
}
#endif
