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


#include <wcqueue.h>
#include <wcdefs.h>
#include <wclist.h>
#include <wclbase.h>
#include <wclcom.h>
#include <wclibase.h>
#include <wcstack.h>
#include <wclistit.h>
#include <wcvbase.h>
#include <wcvector.h>
#include <wchash.h>
#include <wchbase.h>
#include <wcexcept.h>
#include <wchiter.h>

class link : public WCSLink {
};

typedef WCValSList<int> a;
typedef WCValDList<int> b;
typedef WCPtrSList<int> c;
typedef WCPtrDList<int> d;
typedef WCIsvSList<link> e;
typedef WCIsvDList<link> f;

typedef WCValSListIter<int> g;
typedef WCValDListIter<int> h;
typedef WCPtrSListIter<int> i;
typedef WCPtrDListIter<int> j;
typedef WCIsvSListIter<link> k;
typedef WCIsvDListIter<link> l;

typedef WCValVector<int> m;
typedef WCPtrVector<int> n;
typedef WCValOrderedVector<int> o;
typedef WCPtrOrderedVector<int> p;
typedef WCValSortedVector<int> q;
typedef WCPtrSortedVector<int> s;

typedef WCStack<int,WCValSList<int> > t;
typedef WCQueue<int,WCValSList<int> > u;

typedef WCValHashTable<int> v;
typedef WCPtrHashTable<int> w;
typedef WCValHashSet<int> x;
typedef WCPtrHashSet<int> y;

typedef WCValHashTableIter<int> z;
typedef WCPtrHashTableIter<int> aa;
typedef WCValHashSetIter<int> bb;
typedef WCPtrHashSetIter<int> cc;
