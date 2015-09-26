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


#include <stdio.h>
#include "ctags.h"
//#include "misc.h"

#define VBUFF_SIZE      4096

static char         vBuff[VBUFF_SIZE];
static long         currentLineNumber;
static long         currentLinePos;
static FILE         *inputFile;
static const char   *fileName;
static char         lineBuffer[MAX_LINE + 2];

/*
 * StartFile - start a new file
 */
void StartFile( const char *fname )
{
    inputFile = fopen( fname, "r" );
    if( inputFile == NULL ) {
        ErrorMsgExit( "Could not open file %s\n", fname );
    }
    setvbuf( inputFile, vBuff, _IOFBF, VBUFF_SIZE );
    currentLineNumber = 1L;
    currentLinePos = ftell( inputFile );
    fileName = fname;

} /* StartFile */

/*
 * EndFile - done with current file
 */
void EndFile( void )
{
    if( inputFile != NULL ) {
        fclose( inputFile );
        inputFile = NULL;
    }

} /* EndFile */

/*
 * NewFileLine - set to the position of a new line
 */
void NewFileLine( void )
{
    currentLineNumber++;
    currentLinePos = ftell( inputFile );

} /* NewFileLine */

static char prevChar;

/*
 * GetChar - get a char from the input file
 */
int GetChar( void )
{
    int pc;
    if( prevChar > 0 ) {
        pc = prevChar;
        prevChar = 0;
        return( pc );
    }
    return( fgetc( inputFile ) );

} /* GetChar */

/*
 * UnGetChar - put a character "back" into the file stream
 */
void UnGetChar( int ch )
{
    prevChar = ch;

} /* UnGetChar */

/*
 * RecordCurrentLineData - add a new tag
 */
void RecordCurrentLineData( void )
{
    long        curr_pos;
    char        *ptr;
    int         cnt;
    bool        done = false;
    int         ch;

    curr_pos = ftell( inputFile );
    fseek( inputFile, currentLinePos, SEEK_SET );

    ptr = lineBuffer;
    *ptr++ = '^';
    cnt = 1;
    while( !done ) {
        ch = GetChar();
        switch( ch ) {
        case EOF:
            done = true;
            break;
        case '/':
            if( cnt == MAX_LINE - 2 ) {
                done = true;
                break;
            }
            *ptr++ = '\\';
            *ptr++ = ch;
            cnt++;
            break;
        case '\\':
            if( cnt == MAX_LINE - 2 ) {
                done = true;
                break;
            }
            *ptr++ = '\\';
            *ptr++ = '\\'; // need two, because of regular expressions
            cnt++;
            break;
        case '\n':
            *ptr++ = '$';
            done = true;
            break;
        default:
            *ptr++ = ch;
        }
        cnt++;
        if( cnt >= MAX_LINE - 1 ) {
            done = true;
        }
    }
    *ptr = 0;
    fseek( inputFile, curr_pos, SEEK_SET );

} /* RecordCurrentLineData */

/*
 * GetCurrentLineDataPtr - return pointer to current line data
 */
char *GetCurrentLineDataPtr( void )
{
    return( lineBuffer );

} /* GetCurrentLineDataPtr */

/*
 * GetCurrentFileName - return pointer to current file name
 */
const char *GetCurrentFileName( void )
{
    return( fileName );

} /* GetCurrentFileName */

/*
 * GetString - get a string from the input file.
 */
bool GetString( char *buff, int maxbuff )
{
    currentLineNumber++;
    currentLinePos = ftell( inputFile );
    if( fgets( buff, maxbuff, inputFile ) == NULL ) {
        return( false );
    }
    return( true );

} /* GetString */
