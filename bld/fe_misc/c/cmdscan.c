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
* Description:  Command line arguments scanning routines.
*
****************************************************************************/


#include <ctype.h>
#include <stdlib.h>

#include "cmdscan.h"

#ifndef TRUE
#   define TRUE 1
#   define FALSE 0
#endif

static char const *cmd_scanner; // current scan position
static char const *cmd_switch;  // start of current switch

void CmdScanInit(               // INITIALIZE FOR COMMAND SCANNING
    char const *cmd_line )      // - command line
{
    cmd_scanner = cmd_line;
}


char const *CmdScanAddr(        // RETURN COMMAND-LINE SCAN ADDRESS
    void )
{
    return( cmd_scanner );
}


int CmdScanChar(                // SCAN THE NEXT CHARACTER
    void )
{
    return( *cmd_scanner++ );
}


int CmdScanBufferEnd(           // TEST IF END OF BUFFER
    void )
{
    return( *cmd_scanner == '\0' );
}


int CmdScanSwEnd(               // TEST IF END OF SWITCH
    void )
{
    int ch;                     // - current character

    ch = *cmd_scanner;
    if( ch == '\0' ) {
        return( TRUE );
    }
    if( isspace( ch ) ) {
        return( TRUE );
    }
    if( ch == _SWITCH_CHAR1 || ch == _SWITCH_CHAR2 ) {
        return( TRUE );
    }
    return( FALSE );
}


static int cmdFileChar(         // TEST IF A FILENAME CHARACTER
    void )
{
    char c = *cmd_scanner;

    if( c == _SWITCH_CHAR1 || c == _SWITCH_CHAR2 ) {
        return( TRUE );
    }
    return !CmdScanSwEnd();
}


int CmdDelimitChar(             // TEST IF SWITCH-DELIMITING CHARACTER
    void )
{
    int retn;                   // - return: TRUE ==> is a delimiter
    char ch;                    // - next character

    if( ! cmdFileChar() ) {
        retn = TRUE;
    } else {
        ch = *cmd_scanner;
        if( ch == _SWITCH_CHAR1 || ch == _SWITCH_CHAR2 ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    }
    return( retn );
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


int CmdRecogLowerChar(          // RECOGNIZE A LOWER CASE CHARACTER
    int recog )                 // - character to be recognized
{
    int retn;                   // - TRUE ==> got it

    if( recog == CmdScanLowerChar() ) {
        retn = TRUE;
    } else {
        CmdScanUngetChar();
        retn = FALSE;
    }
    return( retn );
}


int CmdRecogChar(               // RECOGNIZE A CHARACTER
    int recog )                 // - character to be recognized
{
    int retn;                   // - TRUE ==> got it

    if( recog == CmdScanChar() ) {
        retn = TRUE;
    } else {
        CmdScanUngetChar();
        retn = FALSE;
    }
    return( retn );
}


int CmdRecogEquals(             // SKIP EQUALCHAR IN COMMAND LINE
    void )
{
    switch( CmdPeekChar() ) {
    case '=':
    case '#':
        CmdScanChar();
        return( TRUE );
        break;
    }
    return( FALSE );
}

int CmdPathDelim(             // SKIP EQUALCHAR # or ' ' IN COMMAND LINE
    void )
{
    switch( CmdPeekChar() ) {
    case ' ':
        CmdScanWhiteSpace();
        CmdScanUngetChar();
        return( TRUE );
    case '=':
    case '#':
        CmdScanChar();
        return( TRUE );
    }
    return( FALSE );
}

void CmdScanSwitchBegin(        // REMEMBER START OF SWITCH
    void )
{
    cmd_switch = cmd_scanner - 1;
}


#if 0
void CmdScanSwitchBackup(       // BACK UP SCANNER TO START OF SWITCH
    void )
{
    cmd_scanner = cmd_switch;
}
#endif


size_t CmdScanOption(           // SCAN AN OPTION
    char const **option )       // - addr( option pointer )
{
    char const *str;            // - scan position

    str = cmd_scanner;
    *option = str;
    for( ; ; ++str ) {
        int ch;
        ch = *str;
        if( ch == '\0' ) break;
        if( ch == _SWITCH_CHAR1 ) break;
        if( ch == _SWITCH_CHAR2 ) break;
        if( isspace( ch ) ) break;
    }
    return( str - cmd_scanner );
}


char const *CmdScanUngetChar(   // UNGET THE LAST CMD SCAN CHARACTER
    void )
{
    return( --cmd_scanner );
}


size_t CmdScanNumber(           // SCAN A NUMBER
    unsigned *pvalue )          // - addr( return value )
{
    char const *p;              // - scan position
    char const *str_beg;        // - start of string
    unsigned value;             // - base 10 value

    str_beg = CmdScanAddr();
    value = 0;
    for( p = str_beg; isdigit(*p); ++p ) {
        value *= 10;
        value += *p - '0';
    }
    cmd_scanner = p;
    *pvalue = value;
    return( p - str_beg );
}


size_t CmdScanId(               // SCAN AN IDENTIFIER
    char const **option )       // - addr( option pointer )
{
    char const *p;              // - scan position
    char const *str_beg;        // - start of string

    p = CmdScanUngetChar();
    *option = p;
    str_beg = p;
    while( isalnum( *p ) || *p == '_' ) {
        ++p;
    }
    cmd_scanner = p;
    return( p - str_beg );
}


size_t CmdScanFilename(         // SCAN A FILE NAME
    char const **option )       // - addr( option pointer )
{
    char const *str_beg;        // - start of string
    char const *p;              // - pointer into string
    size_t len;                 // - length to return

    str_beg = cmd_scanner;
    *option = str_beg;
    if( *cmd_scanner == '"' ) {
        for( p = cmd_scanner + 1; *p; ++p ) {
            if( *p == '"' ) {
                ++p;
                break;
            }
            // '"\\"' means '\', not '\"'
            if( p[0] == '\\' && p[1] == '\\' ) {
                ++p;
            } else if( p[0] == '\\' && p[1] == '"' ) {
                ++p;
            }
        }
        cmd_scanner = p;
    } else {
        for( ; cmdFileChar(); ++ cmd_scanner );
    }
    len = cmd_scanner - str_beg;
    return( len );
}


int CmdScanWhiteSpace(          // SCAN OVER WHITE SPACE
    void )
{
    int c;                      // - next character

    for( ; ; ) {
        c = CmdScanLowerChar();
        if( c == '\n' ) continue;
        if( c == '\r' ) continue;
        if( isspace( c ) ) continue;
        break;
    }
    return c;
}
