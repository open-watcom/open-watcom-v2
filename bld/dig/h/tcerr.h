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


#ifndef _TCERR_H_INCLUDED
#define _TCERR_H_INCLUDED
#include "digpck.h"
enum {
    TC_BAD_TRAP_FILE,
    TC_CANT_LOAD_TRAP,
    TC_WRONG_TRAP_VERSION,
    TC_OUT_OF_DOS_MEMORY,
};

extern char     *TrapClientString( unsigned );

#define TC_ERR_BAD_TRAP_FILE            TrapClientString( TC_BAD_TRAP_FILE )
#define TC_ERR_CANT_LOAD_TRAP           TrapClientString( TC_CANT_LOAD_TRAP )
#define TC_ERR_WRONG_TRAP_VERSION       TrapClientString( TC_WRONG_TRAP_VERSION )
#define TC_ERR_OUT_OF_DOS_MEMORY        TrapClientString( TC_OUT_OF_DOS_MEMORY )
#include "digunpck.h"
#endif
