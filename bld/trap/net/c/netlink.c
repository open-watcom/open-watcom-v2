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


#include <stddef.h>
#include <string.h>
#include <dos.h>
#if defined( __OS2__ )
#define INCL_DOSMODULEMGR
#include <os2.h>
#else
#include "tinyio.h"
#endif
#include "netbios.h"
#include "trperr.h"
#include "packet.h"

#if defined( __OS2__ )

int pascal (far *NetBiosSubmit)( int, int, NCB far * );
#define NetBIOS( x ) (NetBiosSubmit)( 0, 0, (x) )

extern unsigned char pascal far NetBiosOpen( char far *, char far *,
                                             unsigned, int far *);

#elif defined( __WINDOWS__ )
#define NetBIOS( x ) NetBIOSCall( x )

extern unsigned char far NetBIOSCall( NCB far * );
#pragma aux NetBIOSCall "^" parm [es bx] value [al];

#else

#define NET_BIOS_INT    0x5c
extern unsigned char NetBIOS( NCB far * );
#pragma aux NetBIOS = 0xcd NET_BIOS_INT parm [es bx] value [al];

#endif

NCB         NetCtlBlk;


unsigned RemoteGet( void far *rec, unsigned len )
{
    NetCtlBlk.buff = rec;
    NetCtlBlk.length = len;
    NetCtlBlk.cmd = NET_RECEIVE;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.length );
}

unsigned RemotePut( void far *rec, unsigned len )
{
    NetCtlBlk.buff = rec;
    NetCtlBlk.length = len;
    NetCtlBlk.cmd = NET_SEND;
    NetBIOS( &NetCtlBlk );
    return( NetCtlBlk.length );
}

static char PostListen()
{
    NetCtlBlk.cmd = NET_LISTEN | NET_NOWAIT;
    return( NetBIOS( &NetCtlBlk ) == 0 );
}

char RemoteConnect( void )
{
#ifdef SERVER
    if( NetCtlBlk.cmd_cmplt != 0xff ) {
        if( NetCtlBlk.ret_code == 0 ) return( 1 );
        PostListen();
    }
#else
    NetCtlBlk.cmd = NET_CALL;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code == 0 ) return( 1 );
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

char *RemoteLink( char far *name, char server )
{
    unsigned    i;

    server = server;
    if( name == NULL || *name == '\0' ) name = DefLinkName;
    for( i = 1; i < MAX_NAME_LEN; ++i ) {
        NetCtlBlk.name[i] = (*name != '\0') ? *name++ : ' ';
    }
    NetCtlBlk.name[0] = server ? 'S' : 'C';
#ifdef __OS2__
    {
        HMODULE hmod;

        if( DosLoadModule( NULL, 0, "NETAPI", &hmod ) != 0 ) {
            return( NotThere );
        }
        if( DosGetProcAddr( hmod, "NETBIOSSUBMIT", &NetBiosSubmit ) != 0 ) {
            return( NotThere );
        }
    }
#elif !defined( __WINDOWS__ )
    {
    unsigned    char far *net_bios;

        net_bios = (void far *)TinyGetVect( NET_BIOS_INT );
        if( net_bios == 0 || *net_bios == 0xcf ) {
            return( NotThere );
        }
    }
#endif
    NetCtlBlk.cmd = NET_INVALID_CMD;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code == 0 ) {
        return( NotThere );
    }
    NetCtlBlk.cmd = NET_ADD_NAME;
    NetBIOS( &NetCtlBlk );
    if( NetCtlBlk.ret_code != 0 ) {
        return( TRP_ERR_server_name_already_in_use );
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
        cancel.lana_num = 0;
        cancel.post = 0;
        NetBIOS( &cancel );
    }
    NetCtlBlk.cmd = NET_DELETE_NAME;
    NetBIOS( &NetCtlBlk );
}
