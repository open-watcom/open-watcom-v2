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


/*------------------------------HOST------------------------------------*/

typedef struct vecnode {
    struct vecnode      *next;
    symbol *            entry;
} vecnode;

/*----------------------------------------------------------------------*/

#include "ovltab.h"

#define DefOvlClass     "CODE"
#define OVL_CL_LEN      ( sizeof( DefOvlClass ) - 1 )
#define OvlMgrClass     "_OVLCODE"
#define OVL_MGR_CL_LEN  ( sizeof( OvlMgrClass ) - 1 )

#define _OvltabName     "__OVLTAB__"
#define OVLTAB_NAMELEN  ( sizeof( _OvltabName ) - 1 )
#define _OvltabEndName  "__OVLTABEND__"
#define _OvlVecStartName "__OVLSTARTVEC__"
#define _OvlVecEndName  "__OVLENDVEC__"
#define _LongOvlldr     "__LOVLLDR__"
#define _ShortOvlldr    "__SOVLLDR__"
#define _LongOvlInitRtn "__LOVLINIT__"
#define _ShortOvlInitRtn "__SOVLINIT__"
#define _DynamicOvlldr   "__NOVLLDR__"
#define _DynamicInitRtn  "__NOVLINIT__"

#define _OvlfnameLength strlen( OvlFName )
