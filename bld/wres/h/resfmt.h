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
* Description:  Microsoft/IBM resource file format definitions.
*
****************************************************************************/


#ifndef RESFMT_INCLUDED
#define RESFMT_INCLUDED

#include "resutil.h"

#include "resnamor.h"
#include "resdiag.h"
#include "resaccel.h"
#include "resmenu.h"
#include "resstr.h"
#include "resraw.h"
#include "resiccu.h"
#include "resbitmp.h"
#include "resfont.h"
#include "resver.h"

typedef uint_16 ResMemFlags;
#define MEMFLAG_MOVEABLE            0x0010
#define MEMFLAG_PURE                0x0020
#define MEMFLAG_PRELOAD             0x0040
#define MEMFLAG_DISCARDABLE         0x1000
#define MEMFLAG_SEGALIGN            0x8000

#endif
