/****************************************************************************
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
* Description:  Message formatting and output for wmake.
*
****************************************************************************/


#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "make.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"


typedef union msg_arg {
    UINT16      ui16;
    UINT32      ui32;
    int         i;
    char        *cp;
    char FAR    *cfp;
} MSG_ARG;

STATIC const char   *logName = NULL;
STATIC FILE         *logFP = NULL;

#define pick( num, string ) static const char FAR __literal_ ## num [] = { string };
#include "_msg.h"
#undef pick

STATIC const char FAR * const msgText[] = {
    #define pick( num, string ) __literal_ ## num,
    #include "_msg.h"
    #undef pick
};

/*
 *  These str routines are support routines for doFmtStr.  They return a
 *  pointer to where the null-terminator for dest should be.  (Not all of
 *  them null terminate their result).
 */

STATIC char *strApp( char *dest, const char *src )
/************************************************/
{
    assert( dest != NULL && src != NULL );

    while( (*dest = *src) != NULLCHAR ) {
        ++dest;
        ++src;
    }
    return( dest );
}


STATIC char *strDec( char *dest, UINT16 num )
/*******************************************/
{
    char    *orig;
    char    *str;
    div_t   res;

    assert( dest != NULL );

    orig = dest;
    str = dest + 5;
    res.quot = num;
    do {
        ++dest;
        res = div( res.quot, 10 );
        *--str = (char)( '0' + res.rem );
    } while( res.quot != 0 );

    while( dest >= orig ) {
        *orig++ = *str++;
    }

    return( dest );
}


#ifdef CACHE_STATS
STATIC char *strDecL( char *dest, UINT32 num )
/********************************************/
{
    char    *orig;
    char    *str;
    ldiv_t  res;

    assert( dest != NULL );

    orig = dest;
    str = dest + 10;
    res.quot = num;
    do {
        ++dest;
        res = ldiv( res.quot, 10 );
        *--str = (char)( '0' + res.rem );
    } while( res.quot != 0 );

    while( dest >= orig ) {
        *orig++ = *str++;
    }

    return( dest );
}
#endif


STATIC char *strHex( char *dest, UINT16 num )
/*******************************************/
{
    int     digits;
    char    *str;

    assert( dest != NULL );

    digits = (num > 0xff) ? 4 : 2;

    dest += digits;
    str = dest;
    while( digits > 0 ) {
        *--str = "0123456789abcdef"[num & 0x0f];
        num >>= 4;
        --digits;
    }

    return( dest );
}


#ifdef USE_FAR
STATIC char *farStrApp( char *dest, const char FAR *fstr )
/********************************************************/
{
    assert( dest != NULL && fstr != NULL );

    while( (*dest = *fstr) != NULLCHAR ) {
        ++dest;
        ++fstr;
    }
    return( dest );
}
#else
#   define farStrApp( n, f )  strApp( n, f )
#endif


STATIC char *strDec2( char *dest, UINT16 num )
/********************************************/
{
    div_t   res;

    assert( dest != NULL );

    res = div( num % 100, 10 );
    *dest++ = (char)( '0' + res.quot );
    *dest++ = (char)( '0' + res.rem );
    return( dest );
}

STATIC char *strDec5( char *dest, UINT16 num )
/********************************************/
{
    div_t   res;
    char    *temp;

    assert( dest != NULL );

    res.quot = num;
    for( temp = dest + 4; temp >= dest; --temp ) {
        res = div( res.quot, 10 );
        *temp = (char)( '0' + res.rem );
    }
    return( dest + 5 );
}


STATIC size_t doFmtStr( char *buff, const char FAR *src, va_list args )
/**********************************************************************
 * quick vsprintf routine
 * assumptions - format string does not end in '%'
 *             - only use of '%' is as follows:
 *  %D  : decimal with leading 0 modulo 100 ie: %02d
 *  %C  : 'safe' character ie: if(!cisprint) then do %x
 *  %E  : envoloped string ie: "(%s)"
 *  %F  : far string
 *  %L  : long string ( the process exits here to let another function to
          print the long string. Trailing part must be printed
          using other calls. see printMac()@macros. )
 *  %M  : a message number
 *  %Z  : strerror( errno ) - print a system error
 *  %c  : character
 *  %d  : decimal
 *  %l  : long decimal  ( only if -dCACHE_STATS )
 *  %s  : string
 *  %1  : string
 *  %2  : string
 *  %x  : hex number (2 or 4 digits)
 *  %anything else : %anything else
 */
{
    char        ch;
    char        *dest;
    char        msgbuff[MAX_RESOURCE_SIZE];

    assert( buff != NULL && src != NULL );

    dest = buff;
    for( ;; ) {
        ch = *src++;
        if( ch == NULLCHAR ) {
            break;
        }
        if( ch != '%' ) {
            *dest++ = ch;
        } else {
            ch = *src++;
            switch( ch ) {
                /* case statements are sorted in ascii order */
            case 'D' :
                dest = strDec2( dest, (UINT16)va_arg( args, unsigned ) );
                break;
            case 'C' :
                ch = (char)va_arg( args, int );
                if( cisprint( ch ) ) {
                    *dest++ = ch;
                } else {
                    dest = strHex( strApp( dest, "0x" ), (UINT16)ch );
                }
                break;
            case 'E' :
                *dest++ = '(';
                dest = strApp( dest, va_arg( args, char * ) );
                *dest++ = ')';
                break;
            case 'F' :
                dest = farStrApp( dest, va_arg( args, char FAR * ) );
                break;
            case 'L' :
                *dest = NULLCHAR;
                return( dest - buff );
                break;
            case 'M' :
                MsgGet( va_arg( args, int ), msgbuff );
                dest = strApp( dest, msgbuff );
                break;
            case 'Z' :
                dest = strApp( dest, strerror( errno ) );
                break;
            case 'c' :
                *dest++ = (char)va_arg( args, int );
                break;
            case 'd' :
                dest = strDec( dest, (UINT16)va_arg( args, unsigned ) );
                break;
#ifdef CACHE_STATS
            case 'l' :
                dest = strDecL( dest, va_arg( args, UINT32 ) );
                break;
#endif
            case 's' :
            case '1' :
            case '2' :
                dest = strApp( dest, va_arg( args, char * ) );
                break;
            case 'u' :
                dest = strDec5( dest, (UINT16)va_arg( args, unsigned ) );
                break;
            case 'x' :
                dest = strHex( dest, (UINT16)va_arg( args, unsigned ) );
                break;
            default :
                *dest++ = ch;
                break;
            }
        }
    }
    *dest = NULLCHAR;
    return( dest - buff );
}


size_t FmtStr( char *buff, const char FAR *fmt, ... )
/*******************************************************
 * quick sprintf routine... see doFmtStr
 */
{
    va_list     args;
    size_t      len;

    assert( buff != NULL && fmt != NULL );

    va_start( args, fmt );
    len = doFmtStr( buff, fmt, args );
    va_end( args );
    return( len );
}

static void writeOutput( unsigned class, FILE *fp, const char *buff, size_t len )
/*******************************************************************************/
{
    if( class != INF ) {
        if( logFP != NULL ) {
            fwrite( buff, 1, len, logFP );
        }
    }
    fwrite( buff, 1, len, fp );
}

static void printBanner( char *buff )
/***********************************/
{
    size_t  len;

    Glob.headerout = true;      /* so we don't print more than once */
    len = FmtStr( buff, msgText[BANNER - MSG_SPECIAL_BASE] );
    buff[len++] = '\n';
    fwrite( buff, 1, len, stdout );
    fflush( stdout );
}

void PrintBanner( void )
/**********************/
{
    char        buff[1024];

    printBanner( buff );
}

#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
void PrtMsg( enum MsgClass num, ... )
/*******************************************
 * report a message with various format options
 */
{
    va_list         args;
    char            buff[1024];
    enum MsgClass   pref = M_ERROR;
    size_t          len;
    unsigned        class;
    const char      *fname;
    UINT16          fline;
    FILE            *fp;
    char            wefchar = 'F';    /* W, E, or F */
    char            *str;
    char            msgbuff[MAX_RESOURCE_SIZE];

    if( !Glob.debug && (num & DBG) ) {
        return;
    }

    if( !Glob.noheader && !Glob.headerout ) {
        printBanner( buff );
    }

    len = 0;

    if( (num & LOC) && GetFileLine( &fname, &fline ) ) {
        if( fname != NULL ) {
            len += FmtStr( &buff[len], "%s", fname );
        }
        if( fline != 0 ) {
            len += FmtStr( &buff[len], "(%d)", fline );
        }
        if( len > 0 ) {
            len += FmtStr( &buff[len], ": " );
        }
    }

    class = num & CLASS_MSK;
    if( class == INF ) {
        fp = stdout;
    } else {
        fp = stderr;
        switch( class ) {
        case WRN:
            wefchar = 'W';
            pref = M_WARNING;
            break;
        case ERR:
            Glob.erroryet = true;
            wefchar = 'E';
            pref = M_ERROR;
            break;
        case FTL:
            Glob.erroryet = true;
            wefchar = 'F';
            pref = M_ERROR;
            break;
        }
        if( (num & PRNTSTR) == 0 ) {
            len += FmtStr( &buff[len], "%M(%c%D): ", pref, wefchar, num & NUM_MSK );
        }
    }

    /*
     * print the leader to our message, if any... do this now because it
     * may contain a long filename, and we don't want to overwrite the stack
     * with the doFmtStr() substitution.
     */
    if( len > 0 ) {
        writeOutput( class, fp, buff, len );
    }

    va_start( args, num );
    if( num & PRNTSTR ) {       /* print a big string */
        str = va_arg( args, char * );
        writeOutput( class, fp, str, strlen( str ) );
        len = 0;
    } else if( (num & NUM_MSK) >= MSG_SPECIAL_BASE ) {  /* print a formatted string */
        len = doFmtStr( buff, msgText[(num & NUM_MSK) - MSG_SPECIAL_BASE], args );
    } else {
        MsgGet( num & NUM_MSK, msgbuff );
        len = doFmtStr( buff, msgbuff, args );
    }
    va_end( args );
    if( (num & NEOL) == 0 ) {
        buff[len++] = '\n';
    }
    writeOutput( class, fp, buff, len );
    fflush( fp );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif

#if !defined( NDEBUG )
void massert( const char *expr, const char *file, int line )
{
    PrtMsg( FTL | ASSERTION_FAILED, expr, file, line );
    ExitFatal();
    // never return
}
#endif

enum {
    MSG_USAGE_COUNT = 0
    #define pick(num,eng,jap)   + 1
    #include "usage.gh"
    #undef pick
};

void Usage( void )
/****************/
{
    char        msgbuff[MAX_RESOURCE_SIZE];
    int         i;

    for( i = MSG_USAGE_BASE; i < MSG_USAGE_BASE + MSG_USAGE_COUNT; i++ ) {
        MsgGet( i, msgbuff );
        PrtMsg( INF | PRNTSTR, msgbuff );
    }
    ExitOK();
    // never return
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
bool GetYes( enum MsgClass querymsg )
/************************************
 * ask question, and return true if user responds 'y', else false
 * You should phrase the question such that the default action is the least
 * damaging (ie: the 'no' action is least damaging).
 */
{
    char    buf[LINE_BUFF];

    PrtMsg( INF | NEOL | STRING_YES_NO, querymsg );

    if( fread( buf, 1, LINE_BUFF, stdin ) == 0 ) {
        return( false );
    }

    return( ctoupper( buf[0] ) == YES_CHAR );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif


void LogInit( const char *name )
/**************************************
 * assumes name points to static memory
 */
{
    logName = name;
    logFP = NULL;
    if( logName != NULL ) {
        logFP = fopen( logName, "a" );
    }
}


void LogFini( void )
/*************************/
{
    if( logFP != NULL ) {
        fclose( logFP );
        logFP = NULL;
    }
}
