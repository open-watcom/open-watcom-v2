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


#include "plusplus.h"
#include "typerank.h"

RKD RkdForTypeId(       // GET RKD FOR TYPE ID
    type_id id )        // - id
{
    RKD retn;           // - RKD for id

    switch( id ) {
      case TYP_ERROR :
        retn = RKD_ERROR;
        break;
      case TYP_BOOL :
      case TYP_CHAR :
      case TYP_SCHAR :
      case TYP_UCHAR :
      case TYP_WCHAR :
      case TYP_SSHORT :
      case TYP_USHORT :
      case TYP_SINT :
      case TYP_UINT :
      case TYP_SLONG :
      case TYP_ULONG :
      case TYP_SLONG64 :
      case TYP_ULONG64 :
      case TYP_FLOAT :
      case TYP_DOUBLE :
      case TYP_LONG_DOUBLE :
        retn = RKD_ARITH;
        break;
      case TYP_ENUM :
        retn = RKD_ENUM;
        break;
      case TYP_POINTER :
        retn = RKD_POINTER;
        break;
      case TYP_CLASS :
        retn = RKD_CLASS;
        break;
      case TYP_FUNCTION :
        retn = RKD_FUNCTION;
        break;
      case TYP_DOT_DOT_DOT :
        retn = RKD_ELLIPSIS;
        break;
      case TYP_VOID :
        retn = RKD_VOID;
        break;
      case TYP_MEMBER_POINTER :
        retn = RKD_MEMBPTR;
        break;
      case TYP_GENERIC :
      case TYP_TYPENAME :
        retn = RKD_GENERIC;
        break;
      DbgDefault( "invalid RKD detected" );
    }
    return retn;
}
