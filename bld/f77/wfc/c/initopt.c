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


//
// INITOPT   : initialize default options
//

#include "ftnstd.h"
#include "cpopt.h"
#include "global.h"
#include "fcgbls.h"

extern  void            __UseEnglishCharSet(void);

#define INITIAL_OPTS    (OPT_WARN | OPT_ERRFILE | OPT_TERM | OPT_DFLT_LIB | \
                         OPT_REFERENCE | OPT_DESCRIPTOR | OPT_DEPENDENCY)
#if _CPU == 8086
  #define INIT_CG_OPTS  (CGOPT_M_LARGE | CGOPT_ALIGN | CGOPT_DI_DWARF)
  #define INIT_CPU_OPTS (CPUOPT_8086 | CPUOPT_FPI)
#elif _CPU == 386
  #define INIT_CG_OPTS  (CGOPT_M_FLAT | CGOPT_GS_FLOATS | CGOPT_ALIGN| \
                         CGOPT_DI_DWARF)
  #define INIT_CPU_OPTS (CPUOPT_80686 | CPUOPT_FPI | CPUOPT_FP387)
#else
  #define INIT_CG_OPTS  (CGOPT_ALIGN| CGOPT_DI_DWARF|CGOPT_MANGLE)
  #define INIT_CPU_OPTS 0
#endif


void    SetDefaultOpts( void ) {
//========================

// Initialize options.

    Options = INITIAL_OPTS;
    NewOptions = Options;
    CGOpts = INIT_CG_OPTS;
    CPUOpts = INIT_CPU_OPTS;
    DataThreshold = 256;
    ObjName = NULL;
    IncludePath = NULL;
    __UseEnglishCharSet();
}


void    InitOptions( void ) {
//=====================

// Gets called after command line options have been processed
// (NewOptions contains the options).

    Options = NewOptions;
}
