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
* Description:  Internal debugger trap file OS specific implementation
*               header file.
*
****************************************************************************/


#ifndef _TRPALL_H_INCLUDED
#define _TRPALL_H_INCLUDED

#include "trpcore.h"

#ifdef WANT_FILE_INFO
#include "trpfinfo.h"
#endif

#ifdef WANT_ENV
#include "trpenv.h"
#endif

#ifdef WANT_ASYNC
#include "trpasync.h"
#endif

#ifdef WANT_FILE
#include "trpfile.h"
#endif

#ifdef WANT_OVL
#include "trpovl.h"
#endif

#ifdef WANT_THREAD
#include "trpthrd.h"
#endif

#ifdef WANT_RUN_THREAD
#include "trprtrd.h"
#endif

#ifdef WANT_RFX
#include "trprfx.h"
#endif

#ifdef WANT_CAPABILITIES
#include "trpcapb.h"
#endif

#define BUFF_SIZE       256

#endif
