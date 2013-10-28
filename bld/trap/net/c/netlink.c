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
  #if defined( _M_I86 )
    #define _FAR    __far
  #else
    #define _FAR
  #endif
#elif defined( __NT__ )
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include "bool.h"
  #ifdef __WATCOMC__
    /* Don't actually include nb30.h because of conflicting definitions. */
    #pragma library( "netapi32.lib" )
  #endif
    #define _FAR
#else
    #include "tinyio.h"
    #define _FAR    __far
#endif
#include "trptypes.h"
#include "trperr.h"
#include "packet.h"

#if defined( __OS2__ )
    #include "wnetbios.h"
  #if defined( __386__ )
    unsigned short _System (*NetBiosSubmit)( unsigned short, unsigned short, NCB * );
  #else
    int __pascal (_FAR *NetBiosSubmit)( int, int, NCB _FAR * );
  #endif
    #define NetBIOS( x ) (NetBiosSubmit)( 0, 0, (x) )

    extern byte __pascal _FAR NetBiosOpen( char _FAR *, char _FAR *, unsigned, int _FAR *);

#elif defined( __NT__ )
    #include <nb30.h>

    #define NET_INVALID_CMD 0x7F

    UCHAR APIENTRY Netbios( NCB * );
    #define NetBIOS( x )    Netbios( (x) )

#elif defined( __WINDOWS__ )
    #include "wnetbios.h"

    #define NetBIOS( x ) NetBIOSCall( x )

    extern byte _FAR NetBIOSCall( NCB _FAR * );
    #pragma aux NetBIOSCall "^" parm [es bx] value [al];

#else
    #include "wnetbios.h"

    #define NET_BIOS_INT    0x5c
    extern byte NetBIOS( NCB _FAR * );
    #pragma aux NetBIOS = 0xcd NET_BIOS_INT parm [es bx] value [al];

    extern tiny_dos_version GetTrueDOSVersion( void );
    #pragma aux GetTrueDOSVersion = \
        "mov    ax,3306h"               \
        "int    21h"                    \
        value [bx] modify exact [ax bx dx];

#endif

NCB             NetCtlBlk;
byte            LanaNum;
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
byte GetLanaNum( void )
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

#ifdef __NT__
bool Terminate( void )
{
    // a sideways dive to terminate the link (with failure)
    return( FALSE );
}
#endif

trap_retval RemoteGet( byte *rec, trap_elen len )
{
    NetCtlBlk.ncb_buffer = rec;
    NetCtlBlk.ncb_length = len;
    NetCtlBlk.ncb_command = NCBRECV;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.ncb_length );
}

trap_retval RemotePut( byte *rec, trap_elen len )
{
    NetCtlBlk.ncb_buffer = rec;
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
            return( TRUE );
        PostListen();
    }
#else
    NetCtlBlk.ncb_command = NCBCALL;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ncb_retcode == NRC_GOODRET )
        return( TRUE );
#endif
    return( FALSE );
}

void RemoteDisco( void )
{
    NetCtlBlk.ncb_command = NCBHANGUP;
    NetBIOS( &NetCtlBlk );
#ifdef SERVER
    PostListen();
#endif
}

char            DefLinkName[] = "NetLink";
static char     NotThere[] = TRP_ERR_NetBIOS_is_not_running ;

char *RemoteLink( char *name, bool server )
{
    unsigned    i;

    server = server;
    if( name == NULL || *name == '\0' )
        name = DefLinkName;
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
        if( !is_nt )
            SkipEnum = 1;
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
    for( i = 1; i < NCBNAMSZ; ++i ) {
        NetCtlBlk.ncb_name[i] = (*name != '\0') ? *name++ : ' ';
    }
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
