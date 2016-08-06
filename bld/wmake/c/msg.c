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
* Description:  Message formatting and output for wmake.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include "make.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"


STATIC const char   *logName;
STATIC int          logFH;

typedef union msg_arg {
    UINT16      ui16;
    UINT32      ui32;
    int         i;
    char        *cp;
    char FAR    *cfp;
} MSG_ARG;

STATIC MSG_ARG  ArgValue[2];
STATIC bool     USEARGVALUE = false;    /* set to non_zero if ArgValue is used */

#define pick( num, string ) static const char FAR __literal_ ## num [] = { string };
#include "_msg.h"
#undef pick

STATIC const char FAR * const msgText[] = {
    #define pick( num, string ) __literal_ ## num,
    #include "_msg.h"
    #undef pick
};

STATIC void reOrder( va_list args, char *paratype )
/*************************************************/
{
    int         i;

    for( i = 1; i >= 0 && *paratype != NULLCHAR; --i ) {
        switch( *paratype++ ) {
        case 'D':
        case 'd':
        case 'x':
            ArgValue[i].ui16 = (UINT16)va_arg( args, unsigned );
            break;
        case 'C':
        case 'c':
        case 'M':
            ArgValue[i].i = (UINT16)va_arg( args, unsigned );
            break;
        case 'E':
        case 's':
        case '1':
        case '2':
            ArgValue[i].cp = va_arg( args, char * );
            break;
        case 'F':
            ArgValue[i].cfp = va_arg( args, char FAR * );
            break;
        case 'l':
            ArgValue[i].ui32 = va_arg( args, UINT32 );
            break;
        }
    }
}


STATIC void positnArg( va_list args, UINT16 size )
/*************************************************
 * the reordered parameter are passed to FmtStr as a union of 4 bytes.
 * so we have to take two more bytes out for int, char *, etc, when we use
 * va_arg().
 */
{
    if( USEARGVALUE && ( size < (UINT16)sizeof( MSG_ARG ) ) ) {
        size = (UINT16)va_arg( args, unsigned );    // used for side efect
    }
}


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
 *  %C  : 'safe' character ie: if(!isprint) then do %x
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
                positnArg( args, (UINT16)sizeof( UINT16 ) );
                break;
            case 'C' :
                ch = va_arg( args, int );
                positnArg( args, (UINT16)sizeof( int ) );
                if( isprint( ch ) ) {
                    *dest++ = ch;
                } else {
                    dest = strHex( strApp( dest, "0x" ), (UINT16)ch );
                }
                break;
            case 'E' :
                *dest++ = '(';
                dest = strApp( dest, va_arg( args, char * ) );
                positnArg( args, (UINT16)sizeof( char * ) );
                *dest++ = ')';
                break;
            case 'F' :
                dest = farStrApp( dest, va_arg( args, char FAR * ) );
                positnArg( args, (UINT16)sizeof( char FAR * ) );
                break;
            case 'L' :
                *dest = NULLCHAR;
                return( dest - buff );
                break;
            case 'M' :
                MsgGet( va_arg( args, int ), msgbuff );
                positnArg( args, (UINT16)sizeof( int ) );
                dest = strApp( dest, msgbuff );
                break;
            case 'Z' :
#if defined( __DOS__ )
                MsgGet( SYS_ERR_0 + errno, msgbuff );
#else
                strcpy( msgbuff, strerror( errno ) );
#endif
                dest = strApp( dest, msgbuff );
                break;
            case 'c' :
                *dest++ = va_arg( args, int );
                positnArg( args, (UINT16)sizeof( int ) );
                break;
            case 'd' :
                dest = strDec( dest, (UINT16)va_arg( args, unsigned ) );
                positnArg( args, (UINT16)sizeof( UINT16 ) );
                break;
#ifdef CACHE_STATS
            case 'l' :
                dest = strDecL( dest, va_arg( args, UINT32 ) );
                positnArg( args, (UINT16)sizeof( UINT32 ) );
                break;
#endif
            case 's' :
            case '1' :
            case '2' :
                dest = strApp( dest, va_arg( args, char * ) );
                positnArg( args, (UINT16)sizeof( char * ) );
                break;
            case 'u' :
                dest = strDec5( dest, (UINT16)va_arg( args, unsigned ) );
                positnArg( args, (UINT16)sizeof( UINT16 ) );
                break;
            case 'x' :
                dest = strHex( dest, (UINT16)va_arg( args, unsigned ) );
                positnArg( args, (UINT16)sizeof( UINT16 ) );
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


size_t FmtStr( char *buff, const char *fmt, ... )
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

STATIC void writeOutput( unsigned class, int fh, const char *buff, size_t len )
/*****************************************************************************/
{
    if( class != INF ) {
        if( logFH != -1 ) {
#ifdef _WIN64
            write( logFH, buff, (unsigned)len );
#else
            write( logFH, buff, len );
#endif
        }
    }
#ifdef _WIN64
    write( fh, buff, (unsigned)len );
#else
    write( fh, buff, len );
#endif
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
    int             fh;
    char            wefchar = 'F';    /* W, E, or F */
    char            *str;
    char            msgbuff[MAX_RESOURCE_SIZE];
    char            *paratype;

    if( !Glob.debug && (num & DBG) ) {
        return;
    }

    len = 0;

    if( (num & LOC) && GetFileLine( &fname, &fline ) == RET_SUCCESS ) {
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
        fh = STDOUT_FILENO;
    } else {
        fh = STDERR_FILENO;
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

    Header();

    /*
     * print the leader to our message, if any... do this now because it
     * may contain a long filename, and we don't want to overwrite the stack
     * with the doFmtStr() substitution.
     */
    if( len > 0 ) {
        writeOutput( class, fh, buff, len );
    }

    va_start( args, num );
    if( num & PRNTSTR ) {       /* print a big string */
        str = va_arg( args, char * );
        writeOutput( class, fh, str, strlen( str ) );
        len = 0;
    } else {                    /* print a formatted string */
        if( (num & NUM_MSK) >= MSG_SPECIAL_BASE ) {
            len = doFmtStr( buff, msgText[(num & NUM_MSK) - MSG_SPECIAL_BASE], args );
        } else if( MsgReOrder( num & NUM_MSK, msgbuff, &paratype ) ) {
            USEARGVALUE = true;
            reOrder( args, paratype ); /* reposition the parameters */
            len = FmtStr( buff, msgbuff, ArgValue[0], ArgValue[1] );
            USEARGVALUE = false;
        } else {
            len = doFmtStr( buff, msgbuff, args );
        }
    }
    va_end( args );
    if( (num & NEOL) == 0 ) {
        buff[len++] = EOL;
    }
    writeOutput( class, fh, buff, len );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif

#if !defined( NDEBUG )
void massert( const char *expr, const char *file, int line )
{
    PrtMsg( FTL | ASSERTION_FAILED, expr, file, line );
    ExitFatal();
}
#endif

void Usage( void )
/****************/
{
    char        msgbuff[MAX_RESOURCE_SIZE];
    int         i;

    for( i = MSG_USAGE_BASE;; i++ ) {
        MsgGet( i, msgbuff );
        if( ( msgbuff[0] == '.' ) && ( msgbuff[1] == NULLCHAR ) ) {
            break;
        }
        PrtMsg( INF | PRNTSTR, msgbuff );
    }
    ExitOK();
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

    if( read( STDIN_FILENO, buf, LINE_BUFF ) == 0 ) {
        return( false );
    }

    return( toupper( buf[0] ) == YES_CHAR );
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
    logFH = -1;
    if( name != NULL ) {
        logFH = open( logName, O_WRONLY | O_APPEND | O_CREAT | O_TEXT, PMODE_RW );
    }
}


void LogFini( void )
/*************************/
{
    if( logFH != -1 ) {
        close( logFH );
    }
}
