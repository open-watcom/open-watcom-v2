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
* Description:  NetBIOS remote trap file core.
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#include <dos.h>
#if defined( __OS2__ )
#define INCL_DOSMODULEMGR
#include <os2.h>
    #if defined( __386__ )
    #define far
    #endif
#elif defined( __NT__ )
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    /* Don't actually include nb30.h because of conflicting definitions. */
    #pragma library( "netapi32.lib" )
#else
#include "tinyio.h"
#endif
#include "wnetbios.h"
#include "trperr.h"
#include "packet.h"

#if defined( __OS2__ )
    #if defined( __386__ )
    unsigned short _System (*NetBiosSubmit)( unsigned short, unsigned short, NCB * );
    #else
        int pascal (far *NetBiosSubmit)( int, int, NCB far * );
    #endif
#define NetBIOS( x ) (NetBiosSubmit)( 0, 0, (x) )

extern unsigned char pascal far NetBiosOpen( char far *, char far *,
                                             unsigned, int far *);

#elif defined( __NT__ )

UCHAR APIENTRY Netbios( NCB * );
#define NetBIOS( x )    Netbios( (x) )

#elif defined( __WINDOWS__ )

#define NetBIOS( x ) NetBIOSCall( x )

extern unsigned char far NetBIOSCall( NCB far * );
#pragma aux NetBIOSCall "^" parm [es bx] value [al];

#else

#define NET_BIOS_INT    0x5c
extern unsigned char NetBIOS( NCB far * );
#pragma aux NetBIOS = 0xcd NET_BIOS_INT parm [es bx] value [al];

extern tiny_dos_version GetTrueDOSVersion( void );
#pragma aux GetTrueDOSVersion = \
    "mov    ax,3306h"               \
    "int    21h"                    \
    value [bx] modify exact [ax bx dx];

#endif

NCB             NetCtlBlk;
unsigned char   LanaNum;
int             SkipEnum;

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
unsigned char GetLanaNum( void )
{
    LANA_ENUM   l_enum;

    if( !SkipEnum ) {
        memset( &NetCtlBlk, 0, sizeof( NetCtlBlk ) );
        NetCtlBlk.buff = &l_enum;
        NetCtlBlk.length = sizeof( l_enum );
        NetCtlBlk.cmd = NET_ENUM;
        NetBIOS( &NetCtlBlk );
        if( (NetCtlBlk.ret_code == NET_RC_OK) && l_enum.length ) {
            return( l_enum.lana[0] );
        }
    }
    return( 0 );    /* Default if not running on NT. */
}

unsigned RemoteGet( char *rec, unsigned len )
{
    NetCtlBlk.buff = rec;
    NetCtlBlk.length = len;
    NetCtlBlk.cmd = NET_RECEIVE;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.length );
}

unsigned RemotePut( char *rec, unsigned len )
{
    NetCtlBlk.buff = rec;
    NetCtlBlk.length = len;
    NetCtlBlk.cmd = NET_SEND;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.length );
}

static char PostListen( void )
{
    NetCtlBlk.cmd = NET_LISTEN | NET_NOWAIT;
    return( NetBIOS( &NetCtlBlk ) == 0 );
}

char RemoteConnect( void )
{
#ifdef SERVER
    if( NetCtlBlk.cmd_cmplt != 0xff ) {
        if( NetCtlBlk.ret_code == NET_RC_OK ) return( 1 );
        PostListen();
    }
#else
    NetCtlBlk.cmd = NET_CALL;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code == NET_RC_OK ) return( 1 );
#endif
    return( 0 );
}

void RemoteDisco( void )
{
    NetCtlBlk.cmd = NET_HANG_UP;
    NetBIOS( &NetCtlBlk );
#ifdef SERVER
    PostListen();
#endif
}

char            DefLinkName[] = "NetLink";
static char     NotThere[] = TRP_ERR_NetBIOS_is_not_running ;

char *RemoteLink( char *name, char server )
{
    unsigned    i;

    server = server;
    if( name == NULL || *name == '\0' ) name = DefLinkName;
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
        unsigned    char    *net_bios;
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
        if( !is_nt )
            SkipEnum = 1;
    }
#endif
    NetCtlBlk.cmd = NET_INVALID_CMD;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code != NET_RC_ILLCMD ) {
        return( NotThere );
    }

    LanaNum = GetLanaNum();

    /* NetBIOS reset is required on NT, but kills connections on DOS. */
#ifdef __NT__
    memset( &NetCtlBlk, 0, sizeof( NetCtlBlk ) );
    NetCtlBlk.cmd = NET_RESET;
    NetCtlBlk.lana_num = LanaNum;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code != NET_RC_OK ) {
        return( NotThere );
    }
#endif

    memset( &NetCtlBlk, 0, sizeof( NetCtlBlk ) );
    for( i = 1; i < MAX_NAME_LEN; ++i ) {
        NetCtlBlk.name[i] = (*name != '\0') ? *name++ : ' ';
    }
    NetCtlBlk.name[0] = server ? 'S' : 'C';
    NetCtlBlk.cmd = NET_ADD_NAME;
    NetCtlBlk.lana_num = LanaNum;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code == NET_RC_DUPNAME || NetCtlBlk.ret_code == NET_RC_INUSE ) {
        return( TRP_ERR_server_name_already_in_use );
    } else if( NetCtlBlk.ret_code != NET_RC_OK ) {
        return( TRP_ERR_NetBIOS_name_add_failed ); 
    }
    memcpy( NetCtlBlk.call_name, NetCtlBlk.name, MAX_NAME_LEN );
    NetCtlBlk.call_name[0] = !server ? 'S' : 'C';
    if( server ) {
        if( !PostListen() ) return( TRP_ERR_can_not_start_server );
    }
    return( NULL );
}


void RemoteUnLink( void )
{
    NCB     cancel;

    if( NetCtlBlk.cmd_cmplt == 0xff ) {
        cancel.cmd = NET_CANCEL;
        cancel.buff = &NetCtlBlk;
        cancel.lana_num = LanaNum;
        cancel.post = 0;
        NetBIOS( &cancel );
    }
    NetCtlBlk.cmd = NET_DELETE_NAME;
    NetBIOS( &NetCtlBlk );
}
