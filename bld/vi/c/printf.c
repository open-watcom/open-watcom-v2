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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "const.h"
#include "control.h"
#ifdef __WIN__
#include "winvi.h"
#endif

void Lead( char c, int num, char *buff )
{
    int len,diff,i;

    len = strlen( buff );
    diff = num-len;
    if( diff <= 0 ) {
        return;
    }
    for( i=len;i>=0;i-- ) {
        buff[diff+i] = buff[i];
    }
    for( i=0;i<diff;i++ ) {
        buff[i] = c;
    }
}

static FILE *cFile;
static char *cStr;
static void barfChar( char ch )
{
    if( cFile == NULL ) {
        *cStr++ = ch;
    } else {
        fputc( ch, cFile );
    }
}

static void basePrintf( const char *in, va_list al )
{
    char        cin;
    int         i,j;
    long        l;
    char        buff[MAX_STR],*tmp;

    cin = *in;
    while( cin ) {
        if( cin == '%' ) {
            in++;
            switch( *in ) {
            case '%':
                barfChar( '%' );
                break;
            case 'c':
                barfChar( va_arg( al, char ) );
                break;
            case 'u':
                i = va_arg( al, int );
                utoa( i, buff, 10 );
                goto copyloop1;
            case 'd':
                i = va_arg( al, int );
                itoa( (unsigned int) i, buff, 10 );
                goto copyloop1;
                break;
            case 'U':
                l = va_arg( al, long );
                ultoa( (unsigned long) l, buff, 10 );
                goto copyloop1;
                break;
            case 'l':
                l = va_arg( al, long );
                ltoa( (long) l, buff, 10 );
                goto copyloop1;
                break;
            case 's':
                tmp = va_arg( al, char * );
                goto copyloop2;
                break;
            #ifdef DBG
                case 'W':
                    #ifdef __386__
                        i = va_arg( al, int );
                        itoa( j, buff, 16 );
                        Lead( '0', 8, buff );
                        goto copyloop1;
                    #else
                        i = va_arg( al, int );
                        j = va_arg( al, int );
                        itoa( j, buff, 16 );
                        Lead( '0', 4, buff );
                        buff[4] = ':';
                        itoa( i, &buff[5], 16 );
                        Lead( '0', 4, &buff[5] );
                        goto copyloop1;
                    #endif
            #endif
            case 'Z':   /* %02x */
                i = va_arg( al, int );
                itoa( i, buff, 16 );
                Lead( '0', 2, buff );
                goto copyloop1;
            case 'D':   /* %02d */
                i = va_arg( al, int );
                itoa( i, buff, 10 );
                Lead( '0', 2, buff );
                goto copyloop1;
                break;
            case 'L':   /* %8ld */
                l = va_arg( al, long );
                ltoa( l, buff, 10 );
                j = 8;
                goto copyloop;
                break;
            case 'M':   /* %5ld */
                l = va_arg( al, long );
                ltoa( l, buff, 10 );
                j = 5;
                goto copyloop;
                break;
            case 'N':   /* %6ld */
                l = va_arg( al, long );
                ltoa( l, buff, 10 );
                j = 6;
                goto copyloop;
                break;
            case 'O':   /* %6d */
                l = va_arg( al, int );
                itoa( l, buff, 10 );
                j = 6;
                goto copyloop;
                break;
            case 'S':   /* %-12s */
                tmp = va_arg( al, char * );
                strcpy( buff, tmp );
                j = 12;
                goto copyloopa;
            case 'X':   /* %-15s */
                tmp = va_arg( al, char * );
                strcpy( buff, tmp );
                j = 15;
                goto copyloopa;
            case 'Y':   /* %-32s */
                tmp = va_arg( al, char * );
                strcpy( buff, tmp );
                j = 32;
copyloopa:
                {
                int k,l;
                    l = strlen( buff );
                    k = j-l;
                    if( k > 0 ) {
                        tmp = &buff[l];
                        for( i=0;i<k;i++ ) {
                            tmp[i] = ' ';
                        }
                        tmp[k] = 0;
                    }
                }
                goto copyloop1;
copyloop:           Lead( ' ', j, buff );
copyloop1:          tmp = buff;
copyloop2:          while( *tmp ) {
                        barfChar( *tmp++ );
                    }
                break;
            }
        } else {
            barfChar( cin );
        }
        in++;
        cin = *in;
    }
    if( cFile == NULL ) {
        *cStr++ = 0;
    }
}

void MyPrintf( const char *str, ... )
{
    va_list     al;

#ifdef __WIN__
    char        tmp[MAX_STR];
    va_start( al, str );
    cFile = NULL;
    cStr = tmp;
    basePrintf( str, al );
    MessageBox( (HWND) NULL, tmp, EditorName, MB_OK | MB_TASKMODAL );
#else
    va_start( al, str );
    cFile = stdout;
    cStr = NULL;
    basePrintf( str, al );
#endif

}
void MySprintf( char *out, const char *str, ... )
{
    va_list     al;

    va_start( al, str );
    cFile = NULL;
    cStr = out;
    basePrintf( str, al );
}

void MyVSprintf( char *out, const char *str, va_list al )
{
    cFile = NULL;
    cStr = out;
    basePrintf( str, al );
}

void MyVPrintf( const char *str, va_list al )
{
    cFile = stdout;
    basePrintf( str, al );
}

void MyFprintf( FILE *fp, const char *str, ... )
{
    va_list al;

    va_start( al, str );
    cFile = fp;
    basePrintf( str, al );
}
