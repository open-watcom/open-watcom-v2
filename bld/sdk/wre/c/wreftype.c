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


#include <windows.h>
#include <string.h>
#include "wreglbl.h"
#include "wremem.h"
#include "wreftype.h"
#include "wrdll.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* type definition                                                          */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WResTypeNode *WREFindTypeNodeFromWResID ( WResDir dir, WResID *type )
{
    return( WRFindTypeNodeFromWResID( dir, type ) );
}

WResResNode *WREFindResNodeFromWResID ( WResTypeNode *type, WResID *res )
{
    return( WRFindResNodeFromWResID( type,  res ) );
}

WResLangNode *WREFindLangNodeFromLangType ( WResResNode *rnode,
                                            WResLangType *lang )
{
    return( WRFindLangNodeFromLangType( rnode, lang ) );
}

WResTypeNode *WREFindTypeNode ( WResDir dir, uint_16 type, char *type_name )
{
    return(  WRFindTypeNode( dir, type, type_name ) );
}

WResResNode *WREFindResNode ( WResTypeNode *type, uint_16 res, char *res_name )
{
    return(  WRFindResNode( type, res, res_name ) );
}

