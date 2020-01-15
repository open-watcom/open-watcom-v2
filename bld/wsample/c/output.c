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
* Description:  Execution sampler message output routines.
*
****************************************************************************/


#ifdef __WINDOWS__
#include "commonui.h"
#endif
#include "sample.h"
#include "smpstuff.h"
#include "sysio.h"
#include "wmsg.h"
#ifdef __WINDOWS__
#include "sampwin.h"
#endif


void OutputMsg( int msg )
{
    Output( GET_MESSAGE( msg ) );
}

void OutputMsgNL( int msg )
{
    OutputMsg( msg );
    OutputNL();
}

void OutputMsgParmNL( int msg, const char FAR_PTR *str )
{
    OutputMsg( msg );
    Output( str );
    OutputNL();
}

void OutputMsgCharNL( int msg, char chr )
{
    OutputMsg( msg );
    SysWrite( 2, &chr, 1 );
    OutputNL();
}
