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


#ifndef FMEM_H_INCLUDED
#define FMEM_H_INCLUDED

#include "carver.h"

enum { MAX_ELEM_SIZE_INTO_CARVER = 255 };
enum { BIG_ELEMS_HASH_SIZE = 32 };

/* CLUDGE: So that I don't have to implement a separate error mechanism
for fmem, I just use the one that carver has.  This should be changed in
the future! */

void InitFMem(pAllocFunc allocFunc,
              pFreeFunc freeFunc,
              pCarverPrintMsgFunc cPrintMsg,
              CarverMsgType supressMsgs
              );

AllocFunc FAlloc;
FreeFunc  FFree;

enum {
    FMEM_NO_CHECK = CARVER_NO_CHECK,
    FMEM_CHECK = CARVER_CHECK
};
/* NOTE: If checkMem is set to FMEM_CHECK, the memory in carvers
   will be checked for corruption and non-freeness.  However,
   the memory in big elements (with size > MAX_ELEM_SIZE_INTO_CARVER)
   will not be checked.  This feature can be be added in the future,
   by storing the addresses of big elems into a separate carver
   and checking all the non-freed elements of it later.
*/
void FiniFMem(int checkMem);

#endif









