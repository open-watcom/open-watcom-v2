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


#define USAGE_MSG_BASE  500

#ifdef WR_COMPILED
#include "wrcmsg.h"
#else
#define MSG_LANG_SPACING        1000

#define MSG_USE_E_BASE  (USAGE_MSG_BASE + RLE_ENGLISH*MSG_LANG_SPACING)
#define MSG_USE_J_BASE  (USAGE_MSG_BASE + RLE_JAPANESE*MSG_LANG_SPACING)

    #ifdef UNIX
        // JCS - With gcc, angle brackets indicates a system include file so an
        //  rcmsg.h file in this directory will not be found
        #include "rcmsg.h"
    #else
        // DRW - rcmsg is included with angle brackets because when this file is
        //      included by the linker it want to use the rcmsg.h from the include
        //      path not the one in this directory
        #include <rcmsg.h>
    #endif

#endif
#undef ERRITEM

enum {
    INTERR_UNKNOWN_RCSTATUS,
    INTERR_EXE_HAS_MINUS_1_SEGS,
    INTERR_ERR_BUILDING_RES_DIR,
    INTERR_MEM_FREE_FAILED,
    INTERR_MEM_REALLOC_FAILED
};

extern void RcWarning( unsigned errornum, ... );
extern void RcError(unsigned int ,... );
#ifndef __AXP__
#pragma aux RcFatalError aborts;
#endif
extern void RcFatalError( unsigned int, ... );
extern void ErrorInitStatics( void );
