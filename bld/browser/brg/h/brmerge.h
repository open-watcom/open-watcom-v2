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
* Description:  Definitions used throughout the DWARF file merger.
*
****************************************************************************/


#ifndef BRMERGE_H_INCLUDED
#define BRMERGE_H_INCLUDED

// disable "integral value may be truncated" warning
#pragma warning 389 9

#include <wstd.h>
#include <assert.h>

#include "mrdeath.h"
#include "mempool.h"

typedef int_32  fileidx_t;

#if INSTRUMENTS
#include "debuglog.h"
extern DebuggingLog Log;   // log all activity
#endif

#define INSTRUMENTS_NONE            0
#define INSTRUMENTS_STATISTICS      1
#define INSTRUMENTS_PROG_END_LOG    2
#define INSTRUMENTS_FULL_LOGGING    10


template <class Type> class WCValVector;
template <class Type> class WCPtrVector;
template <class Type> class WCValOrderedVector;
template <class Type> class WCPtrOrderedVector;
template <class Type> class WCValSortedVector;
template <class Type> class WCPtrSortedVector;

template <class Type> class WCValHashTable;
template <class Type> class WCPtrHashTable;
template <class Type> class WCValHashSet;
template <class Type> class WCPtrHashSet;
template <class Key, class Type> class WCValHashDict;
template <class Key, class Type> class WCPtrHashDict;

#endif
