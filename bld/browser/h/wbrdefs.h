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


#ifndef __WBRDEFS_H__
#define __WBRDEFS_H__

//#include <wsample.h>

#include "wstd.h"

typedef unsigned char uchar;
typedef unsigned long ulong;

void wbrCleanup();

class WWindow;
class Browse;

extern WWindow* topWindow; //defined in browse.cpp
extern Browse * browseTop; //defined in browse.cpp

class WObject;

#define methodOf(o,m) (cb)(&o::m)

#define MAX_DISP_NAME_LEN 80    // for demangling

inline int maxInt( int a, int b ) { return (a > b) ? a : b; }
inline int minInt( int a, int b ) { return (a < b) ? a : b; }

#endif // __WBRDEFS_H__
