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


#ifndef RCGLOBAL_INCLUDED
#define RCGLOBAL_INCLUDED

#ifndef RCEXTERN
#define RCEXTERN extern
#endif

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "watcom.h"
#include "pass2.h"
#include "param.h"
#include "rcio.h"
#include "phandle.h"

/****** constants describing global data ******/
/* this is the size of the text input buffer for pass 1 and the data trasfer */
/* buffer for pass 2 */
#if defined( _STANDALONE_ ) || defined( __NT__ )
#define IO_BUFFER_SIZE  0x8000      /* 32k */
#else
#define IO_BUFFER_SIZE  0x4000      /* 16 */
#endif

/****** uninitialized global data ******/
RCEXTERN struct RCParams     CmdLineParms;
RCEXTERN RcResFileID         CurrResFile;
RCEXTERN RcPass2Info         Pass2Info;
RCEXTERN char               *NewIncludeDirs;
RCEXTERN char                CharSetLen[256];
RCEXTERN HANDLE_INFO         Instance;
RCEXTERN bool                StopInvoked;

/****** initialized global data ******/
extern int                  ErrorHasOccured;

#undef RCEXTERN
#endif
