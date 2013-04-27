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


#ifndef _DLLTOOL_H
#define _DLLTOOL_H

#include <windows.h>
#include "idedll.h"


/*
 * Defines for specifying which tool to use.
 */
#define DLLTOOL_WLIB    0


/*
 * Define some types.
 */
typedef IDEBool (IDEAPI *IDEMsgInfo2Fn)( IDECBHdl hdl, IDEMsgInfo2 *info );

typedef struct {
    IDEPrintMsgFn       printmessage;
    IDEPrintMsgFn       printmessageCRLF;
    IDEMsgInfo2Fn       printwithinfo2;
    IDEMsgInfoFn        printwithinfo;
    void *              cookie;             /* passed to callback routines */
} DllToolCallbacks;


/*
 * Function prototypes.
 */
extern void *   InitDllTool( int whichtool,
                             const DllToolCallbacks *callbacks );
extern void     FiniDllTool( void *tool );
extern int      RunDllTool( void *tool, const char *cmdline );


#endif
