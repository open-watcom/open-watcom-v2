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


#ifndef SYS_RC_INCLUDED
#define SYS_RC_INCLUDED

/****************************************************************************/
/* System independent identifiers                                           */
/****************************************************************************/
#include "w_rc.h"

/****************************************************************************/
/* System dependent identifiers                                             */
/****************************************************************************/
#include "accedit.h"

/****************************************************************************/
/* Menu Identifiers                                                         */
/****************************************************************************/

/****************************************************************************/
/* Accelerator Menu ID's                                                    */
/****************************************************************************/
#define IDM_ACC_CLEAR           100
#define IDM_ACC_UPDATE          101
#define IDM_ACC_OPEN            102
#define IDM_ACC_SAVE            103
#define IDM_ACC_SAVEAS          104
#define IDM_ACC_EXIT            105

#define IDM_ACC_CUT             110
#define IDM_ACC_COPY            111
#define IDM_ACC_PASTE           112
#define IDM_ACC_DELETE          113
#define IDM_ACC_LOAD_SYMBOLS    114
#define IDM_ACC_SYMBOLS         115
#define IDM_ACC_SHOWRIBBON      116

#define IDM_ACC_RENAME          120
#define IDM_ACC_MEM_FLAGS       121
#define IDM_ACC_SAVEINTO        122

#define IDM_ACC_NEWITEM         130
#define IDM_ACC_KEYVALUE        131

#define IDM_ACC_ABOUT           140

#endif
