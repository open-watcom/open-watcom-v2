/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Command line arguments scanning routines.
*
****************************************************************************/


#include <ctype.h>
#include <stdlib.h>
#include "bool.h"
#include "cmdscan.h"


typedef struct cmd_scan_ctl {
    char const  *curr_ptr;      // current scan position
    char const  *switch_ptr;    // start of current switch
    bool        unix_mode;      // scan mode
} cmd_scan_ctl;

static cmd_scan_ctl cmd;

char const *CmdScanLineInit(    // INITIALIZE FOR COMMAND SCANNING
    char const *cmd_line )      // - new command line
{                               // RETURN OLD COMMAND-LINE SCAN ADDRESS
    char const *old;

    old = cmd.curr_ptr;
    cmd.curr_ptr = cmd_line;
    cmd.switch_ptr = NULL;
    cmd.unix_mode = false;
    return( old );
}


char const *CmdScanAddr(        // RETURN COMMAND-LINE SCAN ADDRESS
    void )
{
    return( cmd.curr_ptr );
}


int CmdScanChar(                // SCAN THE NEXT CHARACTER
    void )
{
    return( *(unsigned char *)cmd.curr_ptr++ );
}


bool CmdScanBufferEnd(          // TEST IF END OF BUFFER
    void )
{
    return( *cmd.curr_ptr == '\0' );
}


bool CmdScanSwEnd(              // TEST IF END OF SWITCH
    void )
{
    int ch;                     // - current character

    ch = *(unsigned char *)cmd.curr_ptr;
    if( ch == '\0' ) {
        return( true );
    }
    if( isspace( ch ) ) {
        return( true );
    }
    if( CmdScanSwitchChar( ch ) ) {
        return( true );
    }
    return( false );
}


int CmdScanLowerChar(           // SCAN THE NEXT CHARACTER, IN LOWER CASE
    void )
{
    int ch;                     // - character scanned

    ch = CmdScanChar();
    ch = tolower( ch );
    return( ch );
}


int CmdPeekChar(                // PEEK AT NEXT CHARACTER, IN LOWER CASE
    void )
{
    int ch;                     // - character scanned

    ch = CmdScanLowerChar();
    CmdScanUngetChar();
    return( ch );
}


bool CmdRecogLowerChar(         // RECOGNIZE A LOWER CASE CHARACTER
    int recog )                 // - character to be recognized
{
    bool retn;                  // - true ==> got it

    if( recog == CmdScanLowerChar() ) {
        retn = true;
    } else {
        CmdScanUngetChar();
        retn = false;
    }
    return( retn );
}


bool CmdRecogChar(              // RECOGNIZE A CHARACTER
    int recog )                 // - character to be recognized
{
    bool retn;                  // - true ==> got it

    if( recog == CmdScanChar() ) {
        retn = true;
    } else {
        CmdScanUngetChar();
        retn = false;
    }
    return( retn );
}


bool CmdReRecogChar(            // RE-RECOGNIZE LAST CHARACTER
    int recog )                 // - character to be recognized
{
    bool retn;                  // - true ==> got it

    CmdScanUngetChar();
    if( recog == CmdScanChar() ) {
        retn = true;
    } else {
        retn = false;
    }
    return( retn );
}


bool CmdRecogEquals(            // SKIP EQUALCHAR IN COMMAND LINE
    void )
{
    switch( CmdPeekChar() ) {
    case '=':
    case '#':
        CmdScanChar();
        return( true );
        break;
    }
    return( false );
}

bool CmdPathDelim(              // SKIP EQUALCHAR # or ' ' IN COMMAND LINE
    void )
{
    switch( CmdPeekChar() ) {
    case ' ':
        CmdScanSkipWhiteSpace();
        return( true );
    case '=':
    case '#':
        CmdScanChar();
        return( true );
    }
    return( false );
}

void CmdScanSwitchBegin(        // REMEMBER START OF SWITCH
    void )
{
    cmd.switch_ptr = cmd.curr_ptr - 1;
}


#if 0
void CmdScanSwitchBackup(       // BACK UP SCANNER TO START OF SWITCH
    void )
{
    cmd.curr_ptr = cmd.switch_ptr;
}
#endif


size_t CmdScanQuotedString(     // SCAN AN OPTION
    char const **option )       // - addr( option pointer )
{
    char const *str_beg;        // - start of string
    int ch;
    int quote;

    quote = *(unsigned char *)cmd.curr_ptr++;
    *option = str_beg = cmd.curr_ptr;
    while( (ch = *(unsigned char *)cmd.curr_ptr) != '\0' ) {
        cmd.curr_ptr++;
        if( ch == quote ) {
            str_beg++;
            break;
        }
        // '"\\"' means '\', not '\"'
        if( ch == '\\' ) {
            ch = *(unsigned char *)cmd.curr_ptr;
            if( ch == '\\' || ch == '"' ) {
                cmd.curr_ptr++;
            }
        }
    }
    return( cmd.curr_ptr - str_beg );
}

size_t CmdScanOption(           // SCAN AN OPTION
    char const **option,        // - addr( option pointer )
    bool *quoted )              // - addr( quoted )
{
    char const *str_beg;        // - start of string

    if( *cmd.curr_ptr == '"' ) {
        *quoted = true;
        return( CmdScanQuotedString( option ) );
    }
    *option = str_beg = cmd.curr_ptr;
    while( !CmdScanSwEnd() ) {
        cmd.curr_ptr++;
    }
    *quoted = false;
    return( cmd.curr_ptr - str_beg );
}

char const *CmdScanUngetChar(   // UNGET THE LAST CMD SCAN CHARACTER
    void )
{
    return( --cmd.curr_ptr );
}


size_t CmdScanNumber(           // SCAN A NUMBER
    unsigned *pvalue )          // - addr( return value )
{
    char const *str_beg;        // - start of string
    unsigned value;             // - base 10 value

    value = 0;
    str_beg = cmd.curr_ptr;
    for( ; isdigit( *(unsigned char *)cmd.curr_ptr ); cmd.curr_ptr++ ) {
        value *= 10;
        value += *(unsigned char *)cmd.curr_ptr - '0';
    }
    *pvalue = value;
    return( cmd.curr_ptr - str_beg );
}


size_t CmdScanId(               // SCAN AN IDENTIFIER
    char const **option )       // - addr( option pointer )
{
    char const *str_beg;        // - start of string

    *option = str_beg = cmd.curr_ptr;
    while( isalnum( *(unsigned char *)cmd.curr_ptr ) || *(unsigned char *)cmd.curr_ptr == '_' ) {
        cmd.curr_ptr++;
    }
    return( cmd.curr_ptr - str_beg );
}


size_t CmdScanFilename(         // SCAN A FILE NAME
    char const **option,        // - addr( option pointer )
    bool *quoted )              // - addr( quoted )
{
    char const *str_beg;        // - start of string
    int ch;

    *option = str_beg = cmd.curr_ptr;
    if( cmd.unix_mode ) {
        while( *cmd.curr_ptr != '\0' ) {
            cmd.curr_ptr++;
        }
    } else if( *cmd.curr_ptr == '"' ) {
        *quoted = true;
        return( CmdScanQuotedString( option ) );
    } else {
        for( ; (ch = *(unsigned char *)cmd.curr_ptr) != '\0'; cmd.curr_ptr++ ) {
            if( isspace( ch ) ) {
                break;
            }
        }
    }
    *quoted = false;
    return( cmd.curr_ptr - str_beg );
}


void CmdScanSkipWhiteSpace(     // SKIP OVER WHITE SPACES
    void )
{
    while( isspace( *(unsigned char *)cmd.curr_ptr ) ) {
        cmd.curr_ptr++;
    }
}

bool CmdScanSwitchChar( char c )
{
#ifdef __UNIX__
    return( c == '-' );
#else
    return( c == '-' || c == '/' );
#endif
}
