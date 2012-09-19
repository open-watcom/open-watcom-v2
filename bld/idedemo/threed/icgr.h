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


#ifndef CGR_ICGR_H
#define CGR_ICGR_H
/*
Description:
------------
    This is the chart graphics include file for internal use only.

*/

/* set some switches that are used in windows.h */
//#define       WINVER  0x0300          // make sure we don't use any 3.1 stuff
#ifdef PLAT_OS2
#define INCL_PM
#define INCL_DOSMODULEMGR
#include <os2.h>
#else
#include <windows.h>
#endif
/* include the memory tracker from the mem library */
#ifndef PROD
#define  TRACK_MEM
#endif
#include "mem.h"

#include "mytypes.h"
#include "wpi.h"

#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "watdlg.h"


/* These routines are used only in the dbg_??? routines */
#include <stdio.h>

#ifdef CGR_GLOBAL
#define cgrglobal
#else
#define cgrglobal extern
#endif

typedef struct w3coord {
    float               xcoord;
    float               ycoord;
    float               zcoord;
} w3coord;

#include "cgrchart.h"
#include "cgrerr.h"
#include "rowscale.h"
#include "line.h"
#include "winworld.h"
#include "cgrids.h"
#include "textpos.h"
#include "text.h"
#include "pipe.h"

#include "global.h"
#include "const.h"

#include "icgrdef.h"            // internal definitions

#endif
