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
#include "hostsys.h"
#include "cgdefs.h"
#include "model.h"
#include "cfloat.h"
#include "cgaux.h"
#include "cgdll.h"
#include "cgprotos.h"
#include "feprotos.h"

struct  cg_interface OptCGTable = {
    #define  CGAPIDEF(a,b,c) a,
    #include "cgfuntab.h"
    #undef   CGAPIDEF
};

cg_interface    *CGFuncTable = &OptCGTable;
fe_interface    *FEFuncTable;

cg_interface *_CGDLLEXPORT BEDLLInit( fe_interface *func_table )
/**************************************************************/
{
    FEFuncTable = func_table;
    return( CGFuncTable );
}

void _CGDLLEXPORT BEDLLFini( cg_interface *func_table )
/*****************************************************/
{
    // nothing yet
}

int __stdcall LibMain( unsigned_32 hdll, unsigned_32 reason, void *reserved )
{
    return( 1 );
}
