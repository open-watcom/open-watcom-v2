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


#include <setjmp.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "packet.h"
#include "trperr.h"

#if defined( ACAD )
    #undef PHARLAP /* just in case */
#endif

#ifdef SERVER

    #define _DBG( s )
    #define _DBG_ExitFunc( s )

    #ifdef PHARLAP
        #include "pharlap.h"
        #include "dxproto.h"
    #endif

    extern long         GetDosLong( long linear_addr );
    extern char         GetDosByte( long linear_addr );
    extern void         PutDosByte( long linear_addr, char );
    extern void         PutDosLong( long linear_addr, long );
    extern void         CallRealMode( long dos_addr );
    long                RMProcAddr;
    long                RMBuffPtr;
    long                RMBuffLen;
    char                XVersion;
    short               Meg1;

#else

    #include "tinyio.h"
    #include "trapdbg2.h"
    #include <i86.h>

    extern unsigned short MyCS( void );

    #pragma aux MyCS   = 0x8c 0xc8 value [ ax ];

    #define MK_LINEAR( p )    ( ( (long)FP_SEG( (void far *)(p) ) << 4 ) + FP_OFF( (void far *)(p) ) )

    char                Server;
    jmp_buf             RealModeState;
    jmp_buf             ProtModeState;
    struct {
        long            ptr;
        long            len;
    }                   Buff;
    char                BackFromFork;
    short               OldPSP;
    char                BeenToProtMode;

    extern short        DbgPSP();
    extern short        GetPSP();
    extern void         SetPSP( short );
    extern int          _fork(char *,char *);
    extern short        DbgPSP();

#endif



unsigned RemoteGet( char *rec, unsigned len )
{
    unsigned received;
#ifdef SERVER
    long buff;

    _DBG(("Remote Get Calling real mode - %8.8lx %8.8lx\n", RMProcAddr, RMBuffPtr));
    CallRealMode( RMProcAddr );
    _DBG(("Remote Get Back from real mode\n"));
    buff = GetDosLong( RMBuffPtr );
    received = GetDosLong( RMBuffLen );
    len = received;
    _DBG(("Remote Geting %d bytes\n",len));
    while( len != 0 ) {
        *rec = GetDosByte( buff );
        ++buff;
        ++rec;
        --len;
    }
    _DBG(("Remote Get Done\n"));
#else
    _DBG_EnterFunc( "RemoteGet()" );
    len = len;
    Buff.ptr = MK_LINEAR( rec );
    BackToProtMode();
    received = Buff.len;
    _DBG_DumpBytes( rec, received );
    _DBG_ExitFunc( "RemoteGet()" );
#endif
    return( received );
}

unsigned RemotePut( char *snd, unsigned len )
{
#ifdef SERVER
    long buff;

    _DBG(("Remote Put - %8.8lx %8.8lx\n", RMProcAddr, RMBuffPtr));
    PutDosLong( RMBuffLen, len );
    _DBG(("Remote Put %d bytes\n",len));
    buff = GetDosLong( RMBuffPtr );
    while( len != 0 ) {
        PutDosByte( buff, *snd );
        ++buff;
        ++snd;
        --len;
    }
    _DBG(("Remote Put Calling real mode\n"));
    CallRealMode( RMProcAddr );
    _DBG(("Remote Put Back from real mode\n"));
#else
    _DBG_EnterFunc( "RemotePut()" );
    _DBG_DumpBytes( snd, len );
    Buff.len = len;
    Buff.ptr = MK_LINEAR( snd );
    BackToProtMode();
    _DBG_ExitFunc( "RemotePut()" );
#endif
    return( len );
}

#ifndef SERVER
BackToProtMode()
{
    if( setjmp( RealModeState ) == 0 ) {
        _DBG_Writeln( "ENTERING PROTECTED MODE" );
        longjmp( ProtModeState, 0 );
    }
    _DBG_Writeln( "RETURNED FROM PROTECTED MODE" );
}


void far BackFromProtMode()
{
    BeenToProtMode = 1;
    if( setjmp( ProtModeState ) == 0 ) {
        OldPSP = GetPSP();
        SetPSP( DbgPSP() );
        longjmp( RealModeState, 0 );
    }
    SetPSP( OldPSP );
}
#endif

char RemoteConnect( void )
{
    return( 1 );
}

void RemoteDisco( void )
{
}


#ifndef SERVER
char *CopyStr( char *src, char *dst )
{
    while( *dst = *src ) {
        dst++;
        src++;
    }
    return( dst );
}

static char *FindEnv( char *name )
{
    char        far *env;
    unsigned    len;

    len = strlen( name );
    env = MK_FP( *((unsigned far *)MK_FP( DbgPSP(), 0x2c )), 0 );
    while( *env ) {
        if( memcmp( env, name, len ) == 0 ) {
            return( env + len );
        }
        while( *env ) ++env;
        ++env;
    }
    return( NULL );
}

static char *SearchPath( char far *env, char *file, char *buff, char **pendname )
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
        while( *endname != ';' ) ++endname;
        memcpy( save, endname, len + 2 );
        ptr = endname;
        if( name != ptr && ptr[-1]!=':' && ptr[-1]!='/' && ptr[-1]!='\\' ) {
            *ptr++ = '\\';
        }
        memcpy( ptr, file, len + 1 );
        rc = TinyOpen( name, 0 );
        if( rc > 0 ) {
            TinyClose( (tiny_handle_t)rc );
            break;
        }
        memcpy( endname, save, len + 2 );
        name = endname + 1;
    }
    *pendname = endname;
    return( name );
}

#if defined(ACAD)
    #define NAME        "ACAD.EXE"
    #define HELPNAME    ""
#elif defined(PHARLAP)
    #if defined(_NEC_PC)
        // Different run386 for NEC PC-9801
        #define NAME        "NRUN386.EXE"
    #elif defined(_FMR_PC)
        // Different run386 for Fujitsu FM-R
        #define NAME        "RUN386F.EXE"
    #else
        #define NAME        "TNT.EXE"
        #define OTHER_NAMES "RUN386.EXE\0"
    #endif
    #define HELPNAME    "PLSHELP.EXP"
    #define HELPNAME_DS "PEDHELP.EXP"
    #define HELPNAME_NS "PENHELP.EXP"   /* not supported yet */
#elif defined(DOS4G)
    #define NAME        "DOS4GW.EXE"
    #define OTHER_NAMES "4GWPRO.EXE\0DOS4G.EXE\0DOS4GX.EXE\0"
    #define HELPNAME    "RSIHELP.EXP"
#else
    #error Extender and helper names not defined
#endif

#ifndef OTHER_NAMES
    #define OTHER_NAMES
#endif

static char *CheckPath( char *path, char *fullpath, char **endname )
{
    char        *namep;
    char        *name;

    namep = NAME "\0" OTHER_NAMES;
    for( ;; ) {
        name = SearchPath( path, namep, fullpath, endname );
        if( *name != '\0' ) return( name );
        if( *namep == '\0' ) return( NULL );
        namep += strlen( namep ) + 1;
    }
}

#define LOW( c )        ((c)|0x20)      /*Quick and dirty lower casing*/

static char *FindExtender( char *fullpath, char **endname )
{
    #if defined(DOS4G)
        char    *name;
        char    *d4gname;
        unsigned len;

        d4gname = FindEnv( "DOS4GPATH=" );
        if( d4gname != NULL ) {
_DBG_Write("Got DOS4GPATH -<");
_DBG_Write(d4gname);
_DBG_Writeln(">");
            len = strlen( d4gname );
            if( len > 4 ) {
                name = d4gname + len - 4;
                if( name[0] == '.'
                    && LOW(name[1]) == 'e'
                    && LOW(name[2]) == 'x'
                    && LOW(name[3]) == 'e' ) {
_DBG_Writeln( "is exe\r\n" );
                    CopyStr( d4gname, fullpath );
                    *endname = &fullpath[strlen(fullpath)];
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
    return( CheckPath( FindEnv( "PATH=" ), fullpath, endname ) );
}

#if defined(PHARLAP)

#include "exedos.h"
#include "exeos2.h"
#include "exepe.h"

static char *GetHelpName( char *exe_name )
{
    /*
      if executable is:
          PE format, subsystem PE_SS_PL_DOSSTYLE (0x42) then PEDHELP.EXP
      or:
          PE format, other subsystems then PENHELP.EXP
      otherwise:
          PLSHELP.EXP
    */
    tiny_ret_t          ret;
    int                 handle;
    unsigned_32         off;
    union {
        dos_exe_header  dos;
        pe_header       pe;
    }   head;

    handle = -1;
    ret = TinyOpen( exe_name, 0 );
    if( TINY_ERROR( ret ) ) goto exp;
    handle = TINY_INFO( ret );
    TinyRead( handle, &head.dos, sizeof( head.dos ) );
    if( head.dos.signature != DOS_SIGNATURE ) goto exp;
    TinySeek( handle, OS2_NE_OFFSET, SEEK_SET );
    TinyRead( handle, &off, sizeof( off ) );
    TinySeek( handle, off, SEEK_SET );
    TinyRead( handle, &head.pe, sizeof( head.pe ) );
    TinyClose( handle );
    handle = -1;
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
exp:
    if( handle != -1 ) TinyClose( handle );
    _DBG_Writeln( "Want PLSHELP" );
    return( HELPNAME );
}
#endif
#endif

#define LINK_SIGNATURE 0xdeb0deb0L

#if defined(DOS4G)
#define LINK_VECTOR     0x06
#else
#define LINK_VECTOR     0x01
#endif

#ifdef SERVER
#if defined(ACAD)
void InitMeg1()
{
    extern short GetCS();
    if( Meg1 == 0 ) {
        if( GetCS() & 3 ) {
            Meg1 = 0x37;
        } else {
            Meg1 = 0x60;
        }
    }
}

#endif
#endif
char *RemoteLink( char *parm, char server )
{

    #ifdef SERVER
                unsigned long           link;
        #if defined(ACAD)
            {
                XVersion = 2;
                InitMeg1();
            }
        #elif defined(PHARLAP)
        {
            CONFIG_INF                  config;
            static char                 buff[256];

            _dx_config_inf(&config, buff );
            XVersion = config.c_major;
            if( XVersion >= 3 ) {
                Meg1 = config.c_dos_sel;
            } else {
                Meg1 = 0x60;
            }
        }
        #endif
        link = GetDosLong( LINK_VECTOR*4 );
        if( link >= (1024UL*1024UL) || GetDosLong(link) != LINK_SIGNATURE ) {
            return( TRP_ERR_not_from_command );
        }
        RMBuffPtr = GetDosLong( link + 4 );
        RMProcAddr = GetDosLong( link + 8 );
        PutDosLong( LINK_VECTOR*4, GetDosLong( link + 12 ) );
        RMBuffLen = RMBuffPtr + sizeof( long );
    #else
        static char     fullpath[256];              /* static because ss != ds */
        static char     buff[256];
        static char     *endname;
        char            *name;
        char            *buffp;
        char            *endparm;
        void            far *link[4];
        void            far * far * link_ptr;

        _DBG_EnterFunc( "RemoteLink()" );
        BackFromFork = 0;
        link_ptr = (void far *)(LINK_VECTOR*4);
        link[ 3 ] = *link_ptr;
        link[ 2 ] = MK_FP( MyCS(), (unsigned )BackFromProtMode );
        link[ 1 ] = (void far *)MK_LINEAR( &Buff );
        link[ 0 ] = (void far *)LINK_SIGNATURE;
        *link_ptr = (void far *)MK_LINEAR( &link );
        if( parm == NULL ) parm = "\0\0";
        while( *parm == ' ' ) ++parm;
        if( *parm == '`' ) {
            buffp = buff;
            ++parm;
            for( ;; ) {
                *buffp = *parm;
                ++buffp;
                if( *parm == '\0' ) break;
                ++parm;
                if( parm[-1] == '`' ) break;
            }
            *buffp = '\0';
        }
        while( *parm == ' ' ) ++parm;
        if( setjmp( RealModeState ) == 0 ) {
            name = FindExtender( fullpath, &endname );
            if( name == NULL ) {
                _DBG_ExitFunc( "RemoteLink(), unable to find extender" );
                return( TRP_ERR_no_extender );
            }
            _DBG_Write( "Extender name: " );
            _DBG_NoTabWriteln( name );
            endname += strlen( endname )+1;
            #if defined(ACAD)
                buffp = buff;
                buff[ 0 ] = '\0';
            #else
            {
                static char     *endhelp;
                char            *help_name;

#if defined(PHARLAP)
                help_name = GetHelpName( parm + strlen( parm ) + 1 );
#else
                help_name = HELPNAME;
#endif
                buffp = SearchPath( FindEnv( "PATH=" ), help_name, buff, &endhelp );
                if( !*buffp ) {
                    _DBG_ExitFunc( "RemoteLink(), unable to find extender "
                                   "help file" );
                    return( TRP_ERR_no_extender );
                }
            }
            #endif
            _DBG_Write( "Extender help name: " );
            _DBG_NoTabWriteln( buffp );
            endparm = CopyStr( parm, endname+1 );
            endparm = CopyStr( buffp, CopyStr( " ", endparm ) );
            #if defined(PHARLAP)
                endparm = CopyStr( " ", endparm );
                endparm = CopyStr( parm + strlen( parm ) + 1, endparm );
            #endif
            *endname = endparm - endname - 1;
            *endparm = '\r';
            endparm[ 1 ] = '\0';
            _DBG_Write( "Extender Cmd line: " );
            _DBG_NoTabWriteln( endname );
            _DBG_Writeln( "calling _fork() to start extender/debugee" );
            if( _fork( name, endname ) != 0 ) {
                _DBG_ExitFunc( "RemoteLink(), unable to start extender" );
                return( TRP_ERR_cant_start_extender );
            }
        } else if( BackFromFork || !BeenToProtMode ) {
            _DBG_ExitFunc( "RemoteLink(), extender could not start extender "
                           "help file" );
            return( TRP_ERR_cant_start_extender );
        }
    #endif
    parm = parm;
    server = server;
    _DBG_ExitFunc( "RemoteLink()" );
    return( 0 );
}

#pragma aux doskludge = 0xB8 0x00 0x2a  /* mov  ax,2a00H */ \
                        0x83 0xec 0x50 /* sub   sp,50H */ \
                        0xcd 0x21      /* int   21H */ \
                        parm caller [ ax ] \
                        modify [ sp cx dx ] \
                        ;
extern void doskludge( void );

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
