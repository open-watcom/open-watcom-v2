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


#include "miniproc.h"
#include "debugme.h"
#include <string.h>
#undef POP_UP_SCREEN
#define ConsolePrintf _
#include <conio.h>
#undef ConsolePrintf
#include "nw3to4.h"
#include "trperr.h"


#include <ecb.h>


extern int main( int arg, char **argv ); // defined by user
extern void TrapFini(void);

extern void breakpt(void);
#pragma aux breakpt = 0xcc;

static  BYTE    stack[8192];

struct LoadDefinitionStruct             *MyNLMHandle;
struct ScreenStruct                     *screenID;
struct ScreenStruct                     *debugScreen;
struct ResourceTagStructure             *ScreenTag;
struct ResourceTagStructure             *AllocTag;
struct ResourceTagStructure             *SemaphoreTag;
struct ResourceTagStructure             *ProcessTag;
struct ResourceTagStructure             *TimerTag;
struct ResourceTagStructure             *InterruptTag;
struct ResourceTagStructure             *SocketTag;
struct ResourceTagStructure             *DebugTag;
struct ResourceTagStructure             *BreakTag;
int                                     DebugMode = 0;

static char                             *Command;
#ifdef DEBUG_ME
    debug_classes                       DebugClasses = 0;
#endif

LONG skwHandle, skwoff, skwtmp;


/* We cannot use CLIB.NLM in the debugger because the user might put
   break points in CLIB, and we just couldn't deal with having the
   debugger hit a break point in itself. Ouch! My head hurts just
   thinking about it */

char *getcmd( char *buff )
{
    if( buff == NULL ) return( Command );
    strcpy( buff, Command );
    return( buff );
}

char upper( char ch ) {

    if ( ch >= 'a' && ch <= 'z' ) {
        ch = ch - 'a' + 'A';
    }
    return( ch );
}


char lower( char ch ) {

    if ( ch >= 'A' && ch <= 'Z' ) {
        ch = ch - 'A' + 'a';
    }
    return( ch );
}


int isalpha( char ch ) {

    return( lower( ch ) >= 'a' && lower( ch ) <= 'z' );
}

int strnicmp( const char *a, const char *b, size_t len ) {

    int         diff;

    while( len != 0 ) {
        diff = lower( *a ) - lower( *b );
        if( diff != 0 ) return( diff );
        ++a;
        ++b;
        --len;
    }
    return( 0 );
}

int stricmp( const char *a, const char *b ) {

    int         diff;

    for( ;; ) {
        diff = lower( *a ) - lower( *b );
        if( diff != 0 ) return( diff );
        if( *a == '\0' ) return( 0 );
        ++a;
        ++b;
    }
}

char *strupr( char *str )
{
    char *p = str;

    while( *p != '\0' ) {
        *p = upper( *p );
        ++p;
    }
    return( str );
}

char *strcpy( char *dst, const char *src )
{
    char *p = dst;

    while( *src != '\0' ) {
        *p = *src;
        ++p;
        ++src;
    }
    *p = '\0';
    return( dst );
}

char *strcat( char *dst, const char *src )
{
    char *p = dst;

    while( *p != '\0' ) {
        ++p;
    }
    strcpy( p, src );
    return( dst );
}

size_t strlen( const char *str )
{
    size_t      len;

    len = 0;
    while( *str != '\0' ) {
        ++len;
        ++str;
    }
    return( len );
}

char * memcpy( char *dst, const char *src, size_t len )
{
    char *p = dst;
    while( len != 0 ) {
        *p = *src;
        ++p;
        ++src;
        --len;
    }
    return( dst );
}

int memcmp( char *a, char *b, size_t len )
{
    while( len != 0 ) {
        if( *a - *b != 0 ) return( *a - *b );
        ++a;
        ++b;
        --len;
    }
    return( 0 );
}

void *memset( void *dst, int c, size_t len )
    {

        char *p;
        for( p = dst; len; --len ) {
            *p++ = c;
        }
        return( dst );
    }
char *strchr( const char *s, int c )
    {
        do {
            if( *s == c ) return( (char *)s );
        } while( *s++ != '\0' );
        return( NULL );
    }

int toupper( int c )
    {
        if( c >= 'a'  && c <= 'z' ) {
            c = c - 'a' + 'A';
        }
        return( c );
    }

static char *args[] = { "", 0 };

static void CloseAllScreens()
{
    if( screenID ) CloseScreen( screenID );
    screenID = 0;
#ifdef DEBUG_ME
    if( debugScreen ) CloseScreen( debugScreen );
    debugScreen = 0;
#endif
}


void _Stop( void )
{
    CloseAllScreens();
    TrapFini();
}


int _Check( void )
{
    OutputToScreen( systemConsoleScreen, TRP_NLM_cant_debug_os );
    OutputToScreen( systemConsoleScreen, "\r\n" );
    return( -1 );
}


void    MainHelper()
{
    if( OpenScreen( TRP_The_WATCOM_Debugger, ScreenTag, &screenID ) != 0 ) {
        screenID = 0;
    }
    debugScreen = 0;
#ifdef DEBUG_ME
    if( DebugMode ) {
        if( OpenScreen( TRP_The_WATCOM_Debugger, ScreenTag, &debugScreen ) != 0 ) {
            debugScreen = 0;
        }
    }
#endif
    if( screenID == 0 || ( DebugMode && ( debugScreen == 0 ) ) ) {
        OutputToScreen( systemConsoleScreen, TRP_NLM_no_screen );
        OutputToScreen( systemConsoleScreen, "\r\n" );
    } else {
        EnableInputCursor( screenID );
        main( 1, args );
    }
    _Stop();
    CDestroyProcess( CGetMyProcessID() );
}


#pragma off(unreferenced);
LONG _Prelude(
         struct LoadDefinitionStructure  *NLMHandle,
         struct ScreenStruct            *initializationErrorScreenID,
         BYTE *                           cmdLineP,
         BYTE *                           loadDirectoryPath,
         LONG                             uninitializedDataLength,
         LONG                             NLMfileHandle,
         LONG                           (*readRoutineP)(),
         LONG                             customDataOffset,
         LONG                             customDataSize )
#pragma on(unreferenced);

{

#ifdef __NW30__
    if( FileServerMajorVersionNumber != 3 ) {
        OutputToScreen( systemConsoleScreen, TRP_NLM_no_screen );
        OutputToScreen( systemConsoleScreen, "\r\n" );
        return( -1 );
    }
#endif
#ifdef DEBUG_ME
    if( cmdLineP[0] == '?' || ( cmdLineP[0] == '-' && cmdLineP[1] == 'h' ) ) {
        OutputToScreen( systemConsoleScreen, "Use -d[options]\r\n" );
        OutputToScreen( systemConsoleScreen, "      b = initial break\r\n" );
        OutputToScreen( systemConsoleScreen, "      a = all\r\n" );
        OutputToScreen( systemConsoleScreen, "      t = threads\r\n" );
        OutputToScreen( systemConsoleScreen, "      d = debug regs\r\n" );
        OutputToScreen( systemConsoleScreen, "      e = events\r\n" );
        OutputToScreen( systemConsoleScreen, "      i = IO\r\n" );
        OutputToScreen( systemConsoleScreen, "      i = IPX\r\n" );
        OutputToScreen( systemConsoleScreen, "      m = misc\r\n" );
        OutputToScreen( systemConsoleScreen, "      r = requests\r\n" );
        OutputToScreen( systemConsoleScreen, "      o = errors\r\n" );
        return( -1 );
    }
    if( cmdLineP[0] == '-' && lower( cmdLineP[1] ) == 'd' ) {
        DebugMode = 1;
        cmdLineP += 2;
        while( isalpha( *cmdLineP ) ) {
            switch( lower( *cmdLineP ) ) {
            case 'b':
                breakpt();
                break;
            case 'a':
                DebugClasses = -1;
                break;
            case 'r':
                DebugClasses |= D_REQ;
                break;
            case 'o':
                DebugClasses |= D_ERROR;
                break;
            case 't':
                DebugClasses |= D_THREAD;
                break;
            case 'd':
                DebugClasses |= D_DR;
                break;
            case 'e':
                DebugClasses |= D_EVENT;
                break;
            case 'i':
                DebugClasses |= D_IO;
                break;
            case 'x':
                DebugClasses |= D_IPX;
                break;
            case 'm':
                DebugClasses |= D_MISC;
                break;
            }
            ++cmdLineP;
        }
        while( *cmdLineP == ' ' ) ++cmdLineP;
    }
#endif

// SKW own debug
//    INWDOSCreate("wvideo.dbg", &skwHandle);
//       skwoff=0;

//    OutputToScreen( systemConsoleScreen, "transport time = %8x \r\n", trtime );


    Command = cmdLineP;
    MyNLMHandle = NLMHandle;
    ScreenTag = AllocateResourceTag( MyNLMHandle, "Debug server screens",
                                     ScreenSignature );
    AllocTag = AllocateResourceTag( MyNLMHandle, "Debug server work area",
                                    AllocSignature );
    SemaphoreTag = AllocateResourceTag( MyNLMHandle, "Debug server semaphores",
                                  SemaphoreSignature );
    ProcessTag = AllocateResourceTag( MyNLMHandle, "Debug server processes",
                                   ProcessSignature );
    TimerTag = AllocateResourceTag( MyNLMHandle, "Debugger time out",
                                   TimerSignature );
    InterruptTag = AllocateResourceTag( MyNLMHandle, "Debugger interrupts",
                                   InterruptSignature );
    SocketTag = AllocateResourceTag( MyNLMHandle, "Debugger IPX socket",
                                   SocketSignature );
    DebugTag = AllocateResourceTag( MyNLMHandle, "WVIDEO Debugger",
                                   DebuggerSignature );
    BreakTag = AllocateResourceTag( MyNLMHandle, "WVIDEO Break Points",
                                   BreakpointSignature );
    CMakeProcess( 50, &MainHelper, &stack[ sizeof( stack ) ],
                    sizeof( stack ), "WATCOM Debugger Server", ProcessTag );
    return( 0 );
}

void    __WATCOM_Prelude(void){};
//int     _cstart_;
void __Null_Argv(void){};
void __Init_Argv(void){};

#ifdef DEBUG_ME
void ConsolePrintf( char *format, ... )
   {
//BYTE tmp[160];

// format = format;
   if( DebugMode )
           {
         ActivateScreen( debugScreen );
         OutputToScreenWithPointer( debugScreen, format,
                                    (BYTE *)&format + sizeof( char * ) );
         SetInputToOutputCursorPosition( screenID );

// SKW own debug
//         sprintfWithPointer( tmp, format,
//                             (BYTE *)&format + sizeof( char * ) );
//              LogBuf(tmp);
//      INWsprintf(tmp, format);
/*              _disable();
      INWDOSOpen("wvideo.dbg", &skwHandle);
                _enable();
                _disable();
      INWDOSWrite(skwHandle, skwoff, tmp, strlen(tmp), &skwtmp);
                _enable();
           skwoff += strlen(tmp);
                _disable();
      INWDOSClose(skwHandle);
                _enable();            */


      }
   }
#endif


void WriteStdErr( char *str, int len )
{
    ActivateScreen( systemConsoleScreen );
    while( --len >= 0 ) {
        OutputToScreen( systemConsoleScreen, "%c", *str );
        ++str;
    }
}
