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
* Description:  Argument vector definition and startup.
*
****************************************************************************/


#include "variety.h"
#include "rtinit.h"
#include "widechar.h"
#include <stdlib.h>
#include "initarg.h"

int         __F_NAME(_argc,_wargc);
CHAR_TYPE** __F_NAME(_argv,_wargv);
int         __F_NAME(__argc,__wargc);
CHAR_TYPE** __F_NAME(__argv,__wargv);

#if defined(__386__) || defined(__AXP__) || defined(__PPC__)
AXI( __F_NAME(__Init_Argv,__wInit_Argv), INIT_PRIORITY_RUNTIME )
AYI( __F_NAME(__Fini_Argv,__wFini_Argv), INIT_PRIORITY_LIBRARY )
#else
AXI( __F_NAME(__Init_Argv,__wInit_Argv), 1 )
AYI( __F_NAME(__Fini_Argv,__wFini_Argv), 1 )
#endif
