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


#include "standard.h"
#include "coderep.h"
#include "procdef.h"
#include "zoiks.h"

extern proc_def *CurrProc;

extern void MemtoBaseTemp()
{
}

extern void CharsAndShortsToInts()
{
}

extern void FixCallIns( instruction * ins )
{
    ins = ins;
}
extern void FixMemBases()
{
}

extern hw_reg_set ScratchReg() {
    _Zoiks( ZOIKS_091 );
    return( HW_SI );
}

extern hw_reg_set ParmRegConflicts( hw_reg_set r ) {

    return( r );
}

extern type_length TempLocation( name *temp ) {

    return( temp->t.location + CurrProc->locals.size );
}

extern  hw_reg_set      VarargsHomePtr() {

    _Zoiks( ZOIKS_078 );
    return( HW_EAX );
}

