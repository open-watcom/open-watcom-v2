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


#include <intl.h>
#include <os.h>
#include <debug.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "service.h"
#include "servmgr.h"
#include "instlmgr.h"

/* The old headers don't define these */

#if !defined(Ustrlwr)
   #define Ustrlwr strlwr
#endif
#if !defined(Ustrstr)
   #define Ustrstr strstr
#endif

static short    MyPortAddress;

short PortTest[] =
{
        0x3bc, 0x378, 0x278
};

short PortAddress[3] =
{
        0,0,0
};

int PortTaken[3] =
{
        0,0,0
};

#pragma aux myinp = 0xec /* in  al,dx */ parm   routine [ dx ];
extern char myinp( short );
char inp( short port ) {
    return( myinp( port ) );
}

#pragma aux myoutp = 0xee /* out        dx,al */ parm   routine [ dx ] [ ax ];
extern void myoutp( short, char );
extern void outp( short port, char val ) {
    myoutp( port, val );
}


char GetPortValue( int i, char value )
{
    int         j;
    char        c;

    Dbg(Debugf( U_L("Checking '%4.4x' with %2.2x"), PortTest[ i ], value );)
    outp( PortTest[ i ], value );
    for( j = 100; j != 0; j-- );
    c = inp( PortTest[ i ] );
    Dbg(Debugf( U_L("Got back %2.2x from '%4.4x'"), c, PortTest[ i ] );)
    return( c );
}


STATUS  InitCommunications( OBJECT self )
{
    int                 i;
    int                 portnum;
    STATUS              s;
    U16                 n;
    LIST                list;
    LIST_ENTRY          le;
    IM_GET_SET_NAME     name;
    CHAR                buff[80];

    self=self;

    portnum = 0;
    for( i = 0; i < 3; ++i ) {
        if( GetPortValue( i, 0x55 ) != GetPortValue( i, 0xaa ) ) {
            PortAddress[ portnum++ ] = PortTest[ i ];
            Dbg(Debugf( U_L("Found port at '%4.4x'"), PortTest[ i ] );)
        }
    }
    if( PortAddress[0] == 0 ) return( stsFailed );

    s = ObjectCall( msgIMGetList, theHighSpeedPacketHandlers, &list );
    if( s == stsOK ) {
        ObjectCall( msgListNumItems, list, &n );
        for( le.position = 0; le.position < n ; ++le.position ) {

            ObjectCall( msgListGetItem, list, &le );

            name.handle = le.item;
            name.pName = buff;
            ObjectCall( msgIMGetName, theHighSpeedPacketHandlers, &name );

            Ustrlwr( buff );
            Dbg(Debugf( U_L("Found packet handler '%s'"), buff );)
            if( Ustrstr( buff, U_L("lpt1") ) ) {
                PortTaken[0] = TRUE;
            } else if( Ustrstr( buff, U_L("lpt2") ) ) {
                PortTaken[1] = TRUE;
            } else if( Ustrstr( buff, U_L("lpt3") ) ) {
                PortTaken[2] = TRUE;
            }
        }
    }
    for( i = 0; i < 3; ++i ) {
        if( !PortTaken[i] && PortAddress[i] != 0 ) {
            MyPortAddress = PortAddress[i];
            return( stsOK );
        }
    }
    Debugf( U_L("Cannot obtain a parallel port") );
    return( stsFailed );
}


char *InitSys()
{
    return( 0 );
}

void FiniSys()
{
}

unsigned long Ticks()
{
    return( OSPowerUpTime() / 100 );
}


int NumPrinters()
{
    return( 1 ); /* NYI */
}


unsigned PrnAddress( int printer )
{
    printer=printer;
    return( MyPortAddress );
}

#pragma off(unreferenced);
void FreePorts( unsigned first, unsigned last )
#pragma on(unreferenced);
{
}

#pragma off(unreferenced);
unsigned AccessPorts( unsigned first, unsigned last )
#pragma on(unreferenced);
{
    return( 1 );
}
