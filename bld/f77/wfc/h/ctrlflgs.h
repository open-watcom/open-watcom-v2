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


#define CF_BAD_DO_ENDING        0x0001
#define CF_NOT_SIMPLE_STMT      0x0002
#define CF_NOT_EXECUTABLE       0x0004
#define CF_BAD_BRANCH_OBJECT    0x0008
#define CF_NEED_SET_LINE        0x0010
#define CF_SPECIFICATION        0x0020
#define CF_SUBPROGRAM           0x0040
#define CF_BLOCK_DATA           0x0080
#define CF_DBUG_BEFORE_LINE     0x0100
#define CF_DBUG_AFTER_LINE      0x0200
#define CF_IMPLICIT_LABEL       0x0400
#define CF_NOT_IN_STRUCTURE     0x0800
#define CF_NOT_IN_UNION         0x1000
#define CF_NOT_IN_MAP           0x2000

#define CtrlFlgOn( __bits )     ( CtrlFlgs & (__bits) )
