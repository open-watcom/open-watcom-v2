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
* Description:  DIG client callback function prototypes.
*
****************************************************************************/


#ifndef DIGCLI_H_INCLUDED
#define DIGCLI_H_INCLUDED

void            *DIGCLIENT DIGCliAlloc( size_t );
void            *DIGCLIENT DIGCliRealloc( void *, size_t );
void            DIGCLIENT DIGCliFree( void * );

dig_fhandle     DIGCLIENT DIGCliOpen( const char *, dig_open );
unsigned long   DIGCLIENT DIGCliSeek( dig_fhandle, unsigned long, dig_seek );
unsigned        DIGCLIENT DIGCliRead( dig_fhandle, void *, unsigned );
unsigned        DIGCLIENT DIGCliWrite( dig_fhandle, const void *, unsigned );
void            DIGCLIENT DIGCliClose( dig_fhandle );
void            DIGCLIENT DIGCliRemove( const char *, dig_open );

unsigned        DIGCLIENT DIGCliMachineData( address, unsigned, dig_elen, const void *, dig_elen, void * );

#endif
