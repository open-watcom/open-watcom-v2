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
#include "cpuglob.h"
#include <string.h>
#undef POP_UP_SCREEN
#define ConsolePrintf _
#include <conio.h>
#undef ConsolePrintf
#include "nw3to4.h"
#include "trperr.h"

#if !defined ( __NW50__ )
#error This file is a LibC startup module and must be using __NW50__
#endif

/*
//  New LibC object file will call init and deinit
*/
extern int      __init_environment(
    void *  reserved
    );
extern int     __deinit_environment(
    void *  reserved
    );

/*
//  But we still need to hook prelude so we can catch the commands line
*/
extern LONG _LibCPrelude(
    struct LoadDefinitionStructure *    NLMHandle,
    struct ScreenStruct            *    initializationErrorScreenID,
    BYTE *                              cmdLineP,
    BYTE *                              loadDirectoryPath,
    LONG                                uninitializedDataLength,
    LONG                                NLMfileHandle,
    LONG                                (*readRoutineP)(),
    LONG                                customDataOffset,
    LONG                                customDataSize
    );

extern int main( int arg, char **argv ); // defined by user
extern void TrapFini(void);

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

/*
 *  This HAS to be in the _DATA segment and not _BSS as the LibC prelude
 *  code will reset it to NULL and we need it set when we get into 
 *  __init_environment
 */
static char                             *Command = NULL;
#ifdef DEBUG_ME
    debug_classes                       DebugClasses = 0;
#endif

/* We cannot use CLIB.NLM in the debugger because the user might put
   break points in CLIB, and we just couldn't deal with having the
   debugger hit a break point in itself. Ouch! My head hurts just
   thinking about it */

char *getcmd( char *buff )
{
    if( buff == NULL )
        return( Command );
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

int isdigit( char ch ) {

    return(  ch  >= '0' && ch  <= '9' );
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

void * memcpy( void *_dst, const void * _src, size_t len )
{
    char *dst = _dst;
    const char *src = _src;
    char *p = dst;

    while( len != 0 ) {
        *p = *src;
        ++p;
        ++src;
        --len;
    }
    return( dst );
}

int memcmp( const void *a_in, const void *b_in, size_t len )
{
    const char *a = a_in;
    const char *b = b_in;

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

#ifdef DEBUG_ME
static char *args[] = { "", 0 };
#endif

static void CloseAllScreens()
{
    if( screenID ) CloseScreen( screenID );
    screenID = 0;
#ifdef DEBUG_ME
    if( debugScreen ) CloseScreen( debugScreen );
    debugScreen = 0;
#endif
}


#if 0
void _Stop( void )
{
    CloseAllScreens();
    TrapFini();
}
#endif

#if 0
int _Check( void )
{
    OutputToScreen( systemConsoleScreen, TRP_NLM_cant_debug_os );
    OutputToScreen( systemConsoleScreen, "\r\n" );
    return( -1 );
}
#endif

/*
//  Could have used -zls switch on newer OW compilers.
*/
void    __WATCOM_Prelude(void){};

#ifdef DEBUG_ME
void ConsolePrintf( char *format, ... )
{
    if( DebugMode ){
        ActivateScreen( debugScreen );
        OutputToScreenWithPointer(
            debugScreen,
            format,
            (BYTE *)&format + sizeof( char * )
            );
         SetInputToOutputCursorPosition( screenID );
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

LONG _PreludeHook(
         struct LoadDefinitionStructure  *NLMHandle,
         struct ScreenStruct            *initializationErrorScreenID,
         BYTE *                           cmdLineP,
         BYTE *                           loadDirectoryPath,
         LONG                             uninitializedDataLength,
         LONG                             NLMfileHandle,
         LONG                           (*readRoutineP)(),
         LONG                             customDataOffset,
         LONG                             customDataSize )
{
    /*
    //  We have hooked prelude primarily so we can store a copy of the command
    //  line after LibC has memset our globals!
    */
    LONG status;
    
    MyNLMHandle = NLMHandle;
    Command = (char *)cmdLineP;
    
    status = _LibCPrelude(
        NLMHandle,
        initializationErrorScreenID,
        cmdLineP,
        loadDirectoryPath,
        uninitializedDataLength,
        NLMfileHandle,
        readRoutineP,
        customDataOffset,
        customDataSize
        );

    MyNLMHandle = NLMHandle;
    Command = (char *)cmdLineP;
    return status;
}

int      __init_environment(void *  reserved){

#ifdef DEBUG_ME
    char    *cmdLineP = Command;
    if( cmdLineP[0] == '?' || ( cmdLineP[0] == '-' && cmdLineP[1] == 'h' ) ) {
        OutputToScreen( systemConsoleScreen, "Use -d[options]\r\n" );
        OutputToScreen( systemConsoleScreen, "  options are:\r\n" );
        OutputToScreen( systemConsoleScreen, "      b = initial break\r\n" );
        OutputToScreen( systemConsoleScreen, "      a = all\r\n" );
        OutputToScreen( systemConsoleScreen, "      t = threads\r\n" );
        OutputToScreen( systemConsoleScreen, "      d = debug regs\r\n" );
        OutputToScreen( systemConsoleScreen, "      e = events\r\n" );
        OutputToScreen( systemConsoleScreen, "      i = IO\r\n" );
        OutputToScreen( systemConsoleScreen, "      x = network events\r\n" );
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
                BreakPoint();
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
                DebugClasses |= D_NET;
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

    ScreenTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"Debug server screens",
        ScreenSignature );
    AllocTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"Debug server work area",
        AllocSignature );
    SemaphoreTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"Debug server semaphores",
        SemaphoreSignature );
    ProcessTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"Debug server processes",
        ProcessSignature );
    TimerTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"Debugger time out",
        TimerSignature );
    InterruptTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"Debugger interrupts",
        InterruptSignature );
    DebugTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"WVIDEO Debugger",
        DebuggerSignature );
    BreakTag = AllocateResourceTag(
        MyNLMHandle,
        (BYTE *)"WVIDEO Break Points",
        BreakpointSignature );

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
    }
    return 0;
}

extern int     __deinit_environment(void *  reserved){
    CloseAllScreens();
    TrapFini();
    return 0;
}
