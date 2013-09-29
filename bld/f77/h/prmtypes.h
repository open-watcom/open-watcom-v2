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
* Description:  Run and compile-time constants indicating parameter type
*               and attributes
*
****************************************************************************/

/**************** IMPORTANT NOTE *******************************

  data in files:  ptypes.inc, ptypes.h and ptypdefn.h 
  
  must corespond each to other

****************************************************************/

//
// Run and compile-time consts indicating parameter type and attributes
// NOTE: These constants should stay in the following order.
//       ( routines rely on them being sequential )
//

enum PARAM_TYPES {
    #define pick(id,type,dbgtype,cgtype,typnam) id,
    #include "ptypdefn.h"
    #undef pick

    VAR_LEN_CHAR = 0x80
};

typedef enum PARAM_TYPES PTYPE;
