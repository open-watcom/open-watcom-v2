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


#ifndef _CONPOOL_H_
#define _CONPOOL_H_

// typedef in SYMTYPE.H

struct pool_con {               // POOL_CON -- pool entry
    void        *next;          // - next in ring
 union {                        // - one of:
  struct {                      // - - floating-point constant
    unsigned    len;            // - - - # bytes (includes '\0')
    char        *fp_constant;   // - - - floating value in the pool
  } s;
    signed_64   int64_constant; // - - int-64 constant
 };
    unsigned    free : 1;       // - used for precompiled headers
    unsigned    flt  : 1;       // - floating constant
    unsigned    i64  : 1;       // - int-64 constant
};

// PROTOTYPES

POOL_CON *ConPoolFloatAdd(          // ADD AN ITEM TO THE CONSTANTS POOL
    PTREE node )                // - node for floating constant
;
POOL_CON *ConPoolInt64Add       // ADD AN INT-64 CONSTANT
    ( signed_64 con )           // - new constant
;

POOL_CON *ConstantPoolGetIndex( POOL_CON * );
POOL_CON *ConstantPoolMapIndex( POOL_CON * );

#endif
