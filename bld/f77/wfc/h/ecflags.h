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


// Common/Equivalence flags:
// ==========================

#define IN_EQUIV_SET            0x0001  // in equivalence set
#define LEADER                  0x0002  // leader of equivalence set
#define EQUIV_SET_ALLOC         0x0004  // equivalence set allocated
#define LAST_IN_COMMON          0x0008  // last name in common block
#define MEMBER_IN_COMMON        0x0010  // member of equivalence set in common
#define ES_TYPE                 0x0060  // define equivalence set type bits
#define ES_NO_TYPE              0x0000  // - equivalence set has no type
#define ES_NOT_CHAR             0x0020  // - equivalence set is not TY_CHAR
#define ES_CHAR                 0x0040  // - equivalence set is TY_CHAR
#define ES_MIXED                0x0060  // - equivalence set is of mixed type
#define MEMBER_INITIALIZED      0x0080  // member of equivalence set initialized
#define HAVE_COMMON_OFFSET      0x0100  // offset of leader in common calculated
#define EQUIV_SET_LABELED       0x0200  // label assigned to equivalence set
