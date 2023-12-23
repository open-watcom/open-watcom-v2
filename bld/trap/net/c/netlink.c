/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  NetBIOS remote trap file core.
*
****************************************************************************/


#include <string.h>
#include <dos.h>
#if defined( __OS2__ )
    #define INCL_DOSMODULEMGR
    #include <os2.h>
  #if defined( _M_I86 )
    #define _FAR    __far
  #else
    #define _FAR
  #endif
#elif defined( __NT__ )
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include "bool.h"
    #define _FAR
#elif defined( __WINDOWS__ )
    #include <windows.h>
    #define _FAR    __far
#else
    #include "tinyio.h"
    #define _FAR    __far
#endif
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"
#if defined( __NT__ )
    #include <nb30.h>
#else
    #include "wnetbios.h"
#endif

#define DEFAULT_LINK_NAME   "NetLink"

#if defined( __OS2__ )
    #define NetBIOS( x ) (NetBiosSubmit)( 0, 0, (x) )
#elif defined( __NT__ )
    #define NET_INVALID_CMD 0x7F
    #define NetBIOS( x )    Netbios( (x) )
#elif defined( __WINDOWS__ )
    #define NetBIOS( x ) NetBIOSCall( x )
#else
    #define NET_BIOS_INT    0x5c
#endif

#if defined( __OS2__ )

    extern byte __pascal _FAR NetBiosOpen( char _FAR *, char _FAR *, unsigned, int _FAR *);

#elif defined( __NT__ )

    extern UCHAR APIENTRY Netbios( NCB * );

#elif defined( __WINDOWS__ )

    extern byte _FAR NetBIOSCall( NCB __far * );
    #pragma aux NetBIOSCall "^" __parm [__es __bx] __value [__al]

#else

    extern byte NetBIOS( NCB __far * );
    #pragma aux NetBIOS = 0xcd NET_BIOS_INT __parm [__es __bx] __value [__al]

    extern tiny_dos_version GetTrueDOSVersion( void );
    #pragma aux GetTrueDOSVersion = \
            "mov  ax,3306h"         \
            "int  21h"              \
        __parm              [] \
        __value             [__bx] \
        __modify __exact    [__ax __bx __dx]

#endif

NCB             NetCtlBlk;
byte            LanaNum;
int             SkipEnum;

static char     LinkName[NCBNAMSZ + 1];

#if defined( __OS2__ )
  #if defined( __386__ )
static unsigned short _System (*NetBiosSubmit)( unsigned short, unsigned short, NCB * );
  #else
static int __pascal (_FAR *NetBiosSubmit)( int, int, NCB _FAR * );
  #endif
#endif

/* On traditional NetBIOS 3.0 implementations, only lana numbers 0 and 1 are
 * valid, and lana 0 is almost guaranteed to be the one we want. On Windows NT,
 * things aren't so simple and there's a good chance lana number 0 cannot
 * be used. NT provides an ENUM command which provides a list of the available
 * lana numbers.
 *
 * Of course Microsoft had to go and make people's lives more complicated by
 * requiring DOS applications to use lana enumeration when running under NT
 * but having their own DOS client blow up when the ENUM NCB is submitted on
 * plain DOS. Sigh. We detect running under NT and behave accordingly.
 */
static byte GetLanaNum( void )
{
    LANA_ENUM   l_enum;

    if( !SkipEnum ) {
        memset( &NetCtlBlk, 0, sizeof( NetCtlBlk ) );
        NetCtlBlk.ncb_buffer = (byte _FAR *)&l_enum;
        NetCtlBlk.ncb_length = sizeof( l_enum );
        NetCtlBlk.ncb_command = NCBENUM;
        NetBIOS( &NetCtlBlk );
        if( (NetCtlBlk.ncb_retcode == NRC_GOODRET) && l_enum.length ) {
            return( l_enum.lana[0] );
        }
    }
    return( 0 );    /* Default if not running on NT. */
}

trap_retval RemoteGet( void *data, trap_elen len )
{
    NetCtlBlk.ncb_buffer = data;
    NetCtlBlk.ncb_length = len;
    NetCtlBlk.ncb_command = NCBRECV;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.ncb_length );
}

trap_retval RemotePut( void *data, trap_elen len )
{
    NetCtlBlk.ncb_buffer = data;
    NetCtlBlk.ncb_length = len;
    NetCtlBlk.ncb_command = NCBSEND;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.ncb_length );
}

static char PostListen( void )
{
    NetCtlBlk.ncb_command = NCBLISTEN | ASYNCH;
    return( NetBIOS( &NetCtlBlk ) == 0 );
}

bool RemoteConnect( void )
{
#ifdef SERVER
    if( NetCtlBlk.ncb_cmd_cplt != 0xff ) {
        if( NetCtlBlk.ncb_retcode == NRC_GOODRET )
            return( true );
        PostListen();
    }
#else
    NetCtlBlk.ncb_command = NCBCALL;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ncb_retcode == NRC_GOODRET )
        return( true );
#endif
    return( false );
}

void RemoteDisco( void )
{
    NetCtlBlk.ncb_command = NCBHANGUP;
    NetBIOS( &NetCtlBlk );
#ifdef SERVER
    PostListen();
#endif
}

static char     NotThere[] = TRP_ERR_NetBIOS_is_not_running ;

#ifdef SERVER
#ifdef TRAPGUI
const char *RemoteLinkGet( char *parms, size_t len )
{
    int     i;

    /* unused parameters */ (void)len;

    strcpy( parms, LinkName + 1 );
    for( i = NCBNAMSZ - 1; i > 0 && parms[i] == ' '; i-- ) {
        parms[i] = '\0';
    }
    return( NULL );
}
#endif
#endif

const char *RemoteLinkSet( const char *parms )
{
    int     i;

    if( *parms == '\0' ) {
        parms = DEFAULT_LINK_NAME;
    }
    *LinkName = ' ';
    for( i = 1; i < NCBNAMSZ; ++i ) {
        LinkName[i] = ( *parms != '\0' ) ? *parms++ : ' ';
    }
    LinkName[i] = '\0';
    return( NULL );
}

const char *RemoteLink( const char *parms, bool server )
{
    /* unused parameters */ (void)server;

    if( parms != NULL ) {
        parms = RemoteLinkSet( parms );
        if( parms != NULL ) {
            return( parms );
        }
    }
#if defined(__OS2__)
  #if defined(__386__)
    {
        HMODULE hmod;

        if( DosLoadModule( NULL, 0, "NETAPI32", &hmod ) != 0 ) {
            return( NotThere );
        }
        if( DosQueryProcAddr( hmod, 0, "NetBios32Submit", (PFN*)&NetBiosSubmit ) != 0 ) {
            return( NotThere );
        }
    }
  #else
    {
        HMODULE hmod;

        if( DosLoadModule( NULL, 0, "NETAPI", &hmod ) != 0 ) {
            return( NotThere );
        }
        if( DosGetProcAddr( hmod, "NETBIOSSUBMIT", &NetBiosSubmit ) != 0 ) {
            return( NotThere );
        }
    }
  #endif
#elif !defined( __WINDOWS__ ) && !defined( __NT__ )
    {
        byte                *net_bios;
        tiny_dos_version    dos_ver;
        int                 is_nt = 0;

        net_bios = (void *)TinyGetVect( NET_BIOS_INT );
        if( net_bios == 0 || *net_bios == 0xcf ) {
            return( NotThere );
        }

        /* Check if running under NT; based on MS KB 100290. */
        dos_ver = GetTrueDOSVersion();
        if( dos_ver.major == 5 && dos_ver.minor == 50 )
            is_nt = 1;
        if( !is_nt ) {
            SkipEnum = 1;
        }
    }
#endif
    NetCtlBlk.ncb_command = NET_INVALID_CMD;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ncb_retcode != NRC_ILLCMD ) {
        return( NotThere );
    }

    LanaNum = GetLanaNum();

    /* NetBIOS reset is required on NT, but kills connections on DOS. */
#ifdef __NT__
    memset( &NetCtlBlk, 0, sizeof( NetCtlBlk ) );
    NetCtlBlk.ncb_command = NCBRESET;
    NetCtlBlk.ncb_lana_num = LanaNum;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ncb_retcode != NRC_GOODRET ) {
        return( NotThere );
    }
#endif

    memset( &NetCtlBlk, 0, sizeof( NetCtlBlk ) );
    memcpy( NetCtlBlk.ncb_name, LinkName, NCBNAMSZ );
    NetCtlBlk.ncb_name[0] = ( server ) ? 'S' : 'C';
    NetCtlBlk.ncb_command = NCBADDNAME;
    NetCtlBlk.ncb_lana_num = LanaNum;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ncb_retcode == NRC_DUPNAME || NetCtlBlk.ncb_retcode == NRC_INUSE ) {
        return( TRP_ERR_server_name_already_in_use );
    } else if( NetCtlBlk.ncb_retcode != NRC_GOODRET ) {
        return( TRP_ERR_NetBIOS_name_add_failed );
    }
    memcpy( NetCtlBlk.ncb_callname, NetCtlBlk.ncb_name, NCBNAMSZ );
    NetCtlBlk.ncb_callname[0] = ( !server ) ? 'S' : 'C';
    if( server ) {
        if( !PostListen() ) {
            return( TRP_ERR_can_not_start_server );
        }
    }
    return( NULL );
}


void RemoteUnLink( void )
{
    NCB     cancel;

    if( NetCtlBlk.ncb_cmd_cplt == 0xff ) {
        cancel.ncb_command = NCBCANCEL;
        cancel.ncb_buffer = (byte _FAR *)&NetCtlBlk;
        cancel.ncb_lana_num = LanaNum;
        cancel.ncb_post = 0;
        NetBIOS( &cancel );
    }
    NetCtlBlk.ncb_command = NCBDELNAME;
    NetBIOS( &NetCtlBlk );
}
