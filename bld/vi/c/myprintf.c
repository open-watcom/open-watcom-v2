/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  A variety of printf-like functions for vi
*
****************************************************************************/


#include "vi.h"
#include "bprintf.h"
#include "myprintf.h"


// out_char_str() - called from BasePrintf() - writes to cStr
static char *cStr;

static void out_char_str( char ch )
{
    *cStr++ = ch;
}

// out_char_file() - called from BasePrintf() - writes to cFile
static FILE *cFile;

static void out_char_file( char ch )
{
    fputc( ch, cFile );
}

void MyPrintf( const char *str, ... )
{
    va_list     args;
#ifdef __WIN__
    char        tmp[MAX_STR];

    cStr = tmp;
    va_start( args, str );
    BasePrintf( str, out_char_str, args );
    va_end( args );
    *cStr++ = '\0';
    MessageBox( NO_WINDOW, tmp, EditorName, MB_OK | MB_TASKMODAL );
#else
    cFile = stdout;
    va_start( args, str );
    BasePrintf( str, out_char_file, args );
    va_end( args );
#endif
}

void MySprintf( char *out, const char *str, ... )
// sprintf++ functionality
{
    va_list     args;

    cStr = out;
    va_start( args, str );
    BasePrintf( str, out_char_str, args );
    va_end( args );
    *cStr++ = '\0';
}

void MyVSprintf( char *out, const char *str, va_list args )
// vsprintf++ functionality
{
    cStr = out;
    BasePrintf( str, out_char_str, args );
    *cStr++ = '\0';
}

void MyVPrintf( const char *str, va_list args )
// vprintf++ functionality
{
    cFile = stdout;
    BasePrintf( str, out_char_file, args );
}

void MyFprintf( FILE *fp, const char *str, ... )
// vfprintf++ functionality
{
    va_list args;

    cFile = fp;
    va_start( args, str );
    BasePrintf( str, out_char_file, args );
    va_end( args );
}
