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
* Description:  Remote debug server common I/O functions.
*
****************************************************************************/


#include "bool.h"

#define PARMS_MAXLEN    256

extern char     ServUsage[];
extern char     RWBuff[0x400];

extern void     ServError( const char *msg );
extern void     StartupErr( const char *err );
extern void     ServMessage( const char *msg );
extern int      WantUsage( const char *ptr );
extern void     Output( const char *str );
extern void     SayGNiteGracey( int return_code );
extern int      KeyPress( void );
extern int      KeyGet( void );
extern bool     ParseCommandLine( const char *cmdline, char *trapparms, char *servparms, bool *oneshot );
extern bool     Session( void );
