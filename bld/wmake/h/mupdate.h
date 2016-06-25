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
* Description:  mupdate.c interfaces
*
****************************************************************************/


#ifndef _MUPDATE_H
#define _MUPDATE_H  1

#include <time.h>
#include "mtarget.h"
#include "mupdate.h"

extern bool         DoingUpdate;

extern void         UpdateInit( void );
extern void         UpdateFini( void );
extern RET_T        Update( TARGET *targ );
extern RET_T        MakeList( TLIST *list );

/* called by macro.c */
extern char         *GetCurDeps( bool younger, bool IsMacInf );
extern const char   *GetCurTarg( void );
extern const char   *GetFirstDep( void );
extern const char   *GetLastDep( void );
extern void         exPop( void );
extern void         exPush( TARGET *targ, DEPEND *dep, DEPEND *impDep );

extern const char   *MonthNames[];

#endif

