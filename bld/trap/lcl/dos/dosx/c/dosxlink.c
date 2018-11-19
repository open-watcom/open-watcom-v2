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
* Description:  DOS protected mode "remote link" to real mode.
*
****************************************************************************/


#define DEBUG_TRAP

#include <setjmp.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <i86.h>
#include "cpuglob.h"
#include "trptypes.h"
#include "packet.h"
#include "trperr.h"
#include "dosxlink.h"
#ifdef SERVER
    #include "dosxrmod.h"
  #ifdef PHARLAP
    #include "pharlap.h"
    #include "dxproto.h"
  #endif
#else
    #include "dosenv.h"
    #include "dosxfork.h"
    #include "tinyio.h"
    #include "trapdbg.h"
    #include "dbgpsp.h"
  #if defined(PHARLAP)
    #include "exedos.h"
    #include "exeos2.h"
    #include "exepe.h"
  #endif
#endif


#define LINK_SIGNATURE      0xdeb0deb0L

#if defined(DOS4G) || defined(CAUSEWAY)
    #define LINK_VECTOR     0x06
#else
    #define LINK_VECTOR     0x01
#endif

#define LOW( c )        ((c) | 0x20)        /*Quick and dirty lower casing*/

#ifdef SERVER

    #define _DBG( s )
    #define _DBG_ExitFunc( s )

    #define LINK(i)             GetDosLong( link + sizeof( long ) * i )

  #if defined(CAUSEWAY)
    extern unsigned short   GetZeroSel( void );
    #pragma aux GetZeroSel = \
        "mov ax,0ff00h" \
        "int 31h" \
        modify [bx ecx dx edi esi es] value [ax];
  #endif

#else

  #if defined(ACAD)
    #define EXTENDER_NAMES  "ACAD.EXE\0"
  #elif defined(PHARLAP)
    #define EXTENDER_NAMES  "TNT.EXE\0" "RUN386.EXE\0"
  #elif defined(DOS4G)
    #define EXTENDER_NAMES  "DOS4GW.EXE\0" "4GWPRO.EXE\0" "DOS4G.EXE\0" "DOS4GX.EXE\0"
  #elif defined(CAUSEWAY)
    #define EXTENDER_NAMES  "CWSTUB.EXE\0"
  #else
    #error Extender and helper names not defined
  #endif

  #ifdef USE_FILENAME_VERSION
    #define QUOTED(x)       # x
    #define STRX(x)         QUOTED(x)

    #if defined(ACAD)
      #define HELPNAME      ""
    #elif defined(PHARLAP)
      #define HELPNAME      "PLSHEL" STRX( USE_FILENAME_VERSION ) ".EXP"
      #define HELPNAME_DS   "PEDHEL" STRX( USE_FILENAME_VERSION ) ".EXP"
      #define HELPNAME_NS   "PENHEL" STRX( USE_FILENAME_VERSION ) ".EXP"   /* not supported yet */
    #elif defined(DOS4G)
      #define HELPNAME      "RSIHEL" STRX( USE_FILENAME_VERSION ) ".EXP"
    #elif defined(CAUSEWAY)
      #define HELPNAME      "CWHELP" STRX( USE_FILENAME_VERSION ) ".EXE"
    #endif
  #else
    #if defined(ACAD)
      #define HELPNAME      ""
    #elif defined(PHARLAP)
      #define HELPNAME      "PLSHELP.EXP"
      #define HELPNAME_DS   "PEDHELP.EXP"
      #define HELPNAME_NS   "PENHELP.EXP"   /* not supported yet */
    #elif defined(DOS4G)
      #define HELPNAME      "RSIHELP.EXP"
    #elif defined(CAUSEWAY)
      #define HELPNAME      "CWHELP.EXE"
    #endif
  #endif

    #define MK_LINEAR(p)    ( ( (long)FP_SEG( (void __far *)(p) ) << 4 ) + FP_OFF( (void __far *)(p) ) )

    #define LINK(i)             link[i]

    extern void doskludge( void );
    #pragma aux doskludge = \
        "mov  ax,2a00h" \
        "sub  sp,50h" \
        "int  21h" \
        parm caller [ax] modify [sp cx dx];

#endif

typedef struct RMBuff {
    unsigned long   ptr;
    unsigned long   len;
} RMBuff;

#ifdef SERVER

  #if defined(CAUSEWAY)
    int                     XVersion;
  #else
    char                    XVersion;
  #endif

  #if !defined(DOS4G)
    static unsigned short   Meg1;
  #endif

    static unsigned long    RMProcAddr;
    static RMBuff           __far *RMBuffPtr;

#else

    char                BackFromFork;

    static jmp_buf      RealModeState;
    static jmp_buf      ProtModeState;
    static RMBuff       Buff;
    static short        OldPSP;
    static char         BeenToProtMode;

#endif

trap_retval RemoteGet( void *data, trap_elen len )
{
    trap_elen       received;
#ifdef SERVER
    unsigned long   buff;

    _DBG(("Remote Get Calling real mode - %8.8lx %8.8lx\n", RMProcAddr, RMBuffPtr->ptr));
    CallRealMode( RMProcAddr );
    _DBG(("Remote Get Back from real mode\n"));
    buff = RMBuffPtr->ptr;
    received = RMBuffPtr->len;
    len = received;
    _DBG(("Remote Geting %d bytes\n",len));
    while( len != 0 ) {
        *(char *)data = GetDosByte( buff );
        data = (char *)data + 1;
        ++buff;
        --len;
    }
    _DBG(("Remote Get Done\n"));
#else
    _DBG_EnterFunc( "RemoteGet()" );
    len = len;
    Buff.ptr = MK_LINEAR( data );
    BackToProtMode();
    received = Buff.len;
    _DBG_DumpBytes( data, received );
    _DBG_ExitFunc( "RemoteGet()" );
#endif
    return( received );
}

trap_retval RemotePut( void *data, trap_elen len )
{
#ifdef SERVER
    unsigned long   buff;

    _DBG(("Remote Put - %8.8lx %8.8lx\n", RMProcAddr, RMBuffPtr->ptr));
    RMBuffPtr->len = len;
    _DBG(("Remote Put %d bytes\n",len));
    buff = RMBuffPtr->ptr;
    while( len != 0 ) {
        PutDosByte( buff, *(char *)data );
        data = (char *)data + 1;
        ++buff;
        --len;
    }
    _DBG(("Remote Put Calling real mode\n"));
    CallRealMode( RMProcAddr );
    _DBG(("Remote Put Back from real mode\n"));
#else
    _DBG_EnterFunc( "RemotePut()" );
    _DBG_DumpBytes( data, len );
    Buff.len = len;
    Buff.ptr = MK_LINEAR( data );
    BackToProtMode();
    _DBG_ExitFunc( "RemotePut()" );
#endif
    return( len );
}

#ifdef SERVER

#if 0
char __far *GetScreenPointer( void )
{
#if defined( ACAD )
    return( MK_FP( Meg1, 0xB0000 ) );
#elif defined(CAUSEWAY)
    return( MK_FP( Meg1, 0xB0000 ) );
#elif defined(PHARLAP)
    return( MK_FP( 0x1C, 0 ) );
#elif defined( DOS4G )
    return( MK_FP( 0x50, 0 ) );
#endif
}
#endif

#else

void BackToProtMode( void )
{
    if( setjmp( RealModeState ) == 0 ) {
        _DBG_Writeln( "ENTERING PROTECTED MODE" );
        longjmp( ProtModeState, 0 );
    }
    _DBG_Writeln( "RETURNED FROM PROTECTED MODE" );
}

void __far BackFromProtMode( void )
{
    BeenToProtMode = 1;
    if( setjmp( ProtModeState ) == 0 ) {
        OldPSP = GetPSP();
        SetPSP( DbgPSP() );
        longjmp( RealModeState, 0 );
    }
    SetPSP( OldPSP );
}

static char *CopyStr( const char __far *src, char *dst )
{
    while( *dst = *src ) {
        dst++;
        src++;
    }
    return( dst );
}

static char *SearchPath( const char __far *env, const char *file, char *buff, char **pendname )
{
    char        *endname;
    char        *name;
    tiny_ret_t  rc;
    char        save[20];
    unsigned    len;
    char        *ptr;

    if( env == NULL ) {
        CopyStr( ";", buff );
    } else {
        CopyStr( ";", CopyStr( env, CopyStr( ".;", buff ) ) );
    }
    name = buff;
    len = strlen( file );
    while( *name ) {
        endname = name;
        while( *endname != ';' )
            ++endname;
        memcpy( save, endname, len + 2 );
        ptr = endname;
        if( name != ptr && ptr[-1]!=':' && ptr[-1]!='/' && ptr[-1]!='\\' ) {
            *ptr++ = '\\';
        }
        memcpy( ptr, file, len + 1 );
        rc = TinyOpen( name, 0 );
        if( TINY_OK( rc ) ) {
            TinyClose( TINY_INFO( rc ) );
            break;
        }
        memcpy( endname, save, len + 2 );
        name = endname + 1;
    }
    *pendname = endname;
    return( name );
}

static char *CheckPath( const char __far *path, char *fullpath, char **endname )
{
    const char  *namep;
    char        *name;

    for( namep = EXTENDER_NAMES; *namep != '\0'; ) {
        name = SearchPath( path, namep, fullpath, endname );
        if( *name != '\0' )
            return( name );
        while( *namep++ != '\0' ) {}    // skip to next extender name
    }
    return( NULL );
}

static char *FindExtender( char *fullpath, char **endname )
{
#if defined(DOS4G)
    char        *name;
    const char  __far *d4gname;
    unsigned    len;

    d4gname = DOSEnvFind( "DOS4GPATH" );
    if( d4gname != NULL ) {
_DBG_Write("Got DOS4GPATH -<");
_DBG_Write(d4gname);
_DBG_Writeln(">");
        len = _fstrlen( d4gname );
        if( len > 4 ) {
            const char __far *ext = d4gname + len - 4;
            if( ext[0] == '.'
                && LOW( ext[1] ) == 'e'
                && LOW( ext[2] ) == 'x'
                && LOW( ext[3] ) == 'e' ) {
_DBG_Writeln( "is exe\r\n" );
                *endname = CopyStr( d4gname, fullpath );
                return( fullpath );
            }
        }
        name = CheckPath( d4gname, fullpath, endname );
        if( name != NULL ) {
_DBG_Writeln( "found in path\r\n" );
            return( name );
        }
    }
#endif
    return( CheckPath( DOSEnvFind( "PATH" ), fullpath, endname ) );
}

#if defined(PHARLAP)

static const char *GetHelpName( const char *exe_name )
{
    /*
      if executable is:
          PE format, subsystem PE_SS_PL_DOSSTYLE (0x42) then PEDHELP.EXP
      or:
          PE format, other subsystems then PENHELP.EXP
      otherwise:
          PLSHELP.EXP
    */
    tiny_ret_t          rc;
    tiny_handle_t       handle;
    unsigned_32         off;
    union {
        dos_exe_header  dos;
        pe_header       pe;
    }   head;

    rc = TinyOpen( exe_name, 0 );
    if( TINY_OK( rc ) ) {
        handle = TINY_INFO( rc );
        TinyRead( handle, &head.dos, sizeof( head.dos ) );
        if( head.dos.signature != DOS_SIGNATURE ) {
            TinyClose( handle );
        } else {
            TinySeek( handle, OS2_NE_OFFSET, SEEK_SET );
            TinyRead( handle, &off, sizeof( off ) );
            TinySeek( handle, off, SEEK_SET );
            TinyRead( handle, &head.pe, sizeof( head.pe ) );
            TinyClose( handle );
            switch( head.pe.signature ) {
            case PE_SIGNATURE:
            case PL_SIGNATURE:
                if( head.pe.subsystem == PE_SS_PL_DOSSTYLE ) {
                    _DBG_Writeln( "Want PEDHELP" );
                    return( HELPNAME_DS );
                }
                _DBG_Writeln( "Want PENHELP" );
                return( HELPNAME_NS );
            }
        }
    }
    _DBG_Writeln( "Want PLSHELP" );
    return( HELPNAME );
}
#endif
#endif

const char *RemoteLink( const char *parms, bool server )
{
#ifdef SERVER
    unsigned long       link;
  #if defined(ACAD)
    {
        XVersion = 2;
        if( GetCS() & 3 ) {
            Meg1 = 0x37;
        } else {
            Meg1 = 0x60;
        }
    }
  #elif defined(PHARLAP)
    {
        CONFIG_INF              config;
        static unsigned char    buff[256];

        _dx_config_inf(&config, buff );
        XVersion = config.c_major;
        if( XVersion >= 3 ) {
            Meg1 = config.c_dos_sel;
        } else {
            Meg1 = 0x60;
        }
    }
  #elif defined(CAUSEWAY)
    Meg1 = GetZeroSel();
  #endif
    parms = parms;
    link = GetDosLong( LINK_VECTOR * 4 );
    if( link >= (1024UL * 1024UL) || LINK( 0 ) != LINK_SIGNATURE ) {
        return( TRP_ERR_not_from_command );
    }
    RMBuffPtr = RMLinToPM( LINK( 1 ), 0 );
    RMProcAddr = LINK( 2 );
    PutDosLong( LINK_VECTOR * 4, LINK( 3 ) );
#else
    static char     fullpath[256];              /* static because ss != ds */
    static char     buff[256];
    static char     *endname;
    char            *name;
    char            *buffp;
    char            *endparm;
    void            __far *link[4];
    void            __far * __far *link_ptr;
    unsigned        len;
  #if defined(PHARLAP)
    const char      *exe_name;
  #endif

    _DBG_EnterFunc( "RemoteLink()" );
    BackFromFork = 0;
    link_ptr = (void __far *)(LINK_VECTOR * 4);
    LINK( 3 ) = *link_ptr;
    LINK( 2 ) = MK_FP( GetCS(), (unsigned )BackFromProtMode );
    LINK( 1 ) = (void __far *)MK_LINEAR( &Buff );
    LINK( 0 ) = (void __far *)LINK_SIGNATURE;
    *link_ptr = (void __far *)MK_LINEAR( &link );
    // parms has following format
    // "trap parameters string"+"\0"+"command line string"+"\0"
    _DBG_Write( "Parms: " );
    _DBG_NoTabWriteln( parms );
    while( *parms == ' ' )
        ++parms;
    if( *parms == '`' ) {
        ++parms;
        buffp = buff;
        while( *parms != '\0' ) {
            if( *parms == '`' ) {
                ++parms;
                break;
            }
            *buffp++ = *parms++;
        }
        *buffp = '\0';
    }
    while( *parms == ' ' )
        ++parms;
    if( setjmp( RealModeState ) == 0 ) {
        name = FindExtender( fullpath, &endname );
        if( name == NULL ) {
            _DBG_ExitFunc( "RemoteLink(), unable to find extender" );
            return( TRP_ERR_no_extender );
        }
        _DBG_Write( "Extender name: " );
        _DBG_NoTabWriteln( name );
        while( *endname++ != '\0' ) {}      // skip after extender name + '\0'
  #if defined(ACAD)
        buffp = buff;
        *buffp = '\0';
  #else
        {
            static char     *endhelp;
            const char      *help_name;

    #if defined(PHARLAP)
            exe_name = parms;
            while( *exe_name++ != '\0' ) {} // skip to command line
            help_name = GetHelpName( exe_name );
    #else
            help_name = HELPNAME;
    #endif
            buffp = SearchPath( DOSEnvFind( "PATH" ), help_name, buff, &endhelp );
            if( *buffp == '\0' ) {
                _DBG_ExitFunc( "RemoteLink(), unable to find extender help file" );
                return( TRP_ERR_no_extender );
            }
        }
  #endif
        _DBG_Write( "Extender help name: " );
        _DBG_NoTabWriteln( buffp );
        endparm = CopyStr( parms, endname + 1 );     // reserve length byte
        endparm = CopyStr( buffp, CopyStr( " ", endparm ) );
  #if defined(PHARLAP)
        endparm = CopyStr( " ", endparm );
        endparm = CopyStr( exe_name, endparm );     // add extra executable name
  #endif
        len = endparm - ( endname + 1 );
        if( len > 126 )
            len = 126;
        *endname = len;       // setup length byte
        endparm = endname + len + 1;
        endparm[0] = '\r';
        endparm[1] = '\0';
        _DBG_Write( "Extender Cmd line: " );
        _DBG_NoTabWriteln( endname + 1 );
        _DBG_Writeln( "calling _fork() to start extender/debugee" );
        if( _fork( name, endname ) != 0 ) {
            _DBG_ExitFunc( "RemoteLink(), unable to start extender" );
            return( TRP_ERR_cant_start_extender );
        }
    } else if( BackFromFork || !BeenToProtMode ) {
        _DBG_ExitFunc( "RemoteLink(), extender could not start extender help file" );
        return( TRP_ERR_cant_start_extender );
    }
#endif
    server = server;
    _DBG_ExitFunc( "RemoteLink()" );
    return( NULL );
}

void RemoteUnLink( void )
{
#ifdef SERVER
    CallRealMode( RMProcAddr );
#else
    _DBG_EnterFunc( "RemoteUnLink()" );
    if( setjmp( RealModeState ) == 0 ) {
        doskludge(); /* Ask brian. I don't feel like writing a book */
        SetPSP( OldPSP );
        longjmp( ProtModeState, 0 );
    }
    _DBG_ExitFunc( "RemoteUnLink()" );
#endif
}

bool RemoteConnect( void )
{
    return( true );
}

void RemoteDisco( void )
{
}
