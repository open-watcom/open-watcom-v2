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


#ifndef _CGDATA_H_
#define _CGDATA_H_

#include <stdio.h>
#include "toggle.h"

// CGDATA -- C++ data used during code generation
//
// 91/06/07 -- J.W.Welch        -- defined
// 91/12/12 -- J.W.Welch        -- convert to new codegen switches
// 92/12/29 -- B.J.Stecher      -- QNX support
// 94/10/22 -- B.J.Stecher      -- support for -fr option
// 95/10/04 -- J.W.Welch        -- support for -br option

// DATA:

global  int     OptSize;         /* 100 => make pgm small as possible */

#if _CPU == 386
global  char    CDLL_Name[16];          // C runtime library (-br)
#endif
global  char    DLL_CLIB_Name[16];      // C runtime library (-bd)
global  char    CLIB_Name[16];          // C runtime library

#if _CPU == 386
global  char    WCPPDLL_Name[16];       // C++ runtime library (-br)
#endif
global  char    DLL_WCPPLIB_Name[16];   // C++ runtime library (-bd)
global  char    WCPPLIB_Name[16];       // C++ runtime library

global  char    MATHLIB_Name[16];       // math runtime library
global  char    *EmuLib_Name;           // x87 emulator runtime library

typedef struct prag_stack PRAG_STACK;
struct prag_stack {
    PRAG_STACK  *next;
    unsigned    value;
};

global PRAG_STACK *HeadPacks;
global PRAG_STACK *HeadEnums;
global PRAG_STACK *FreePrags;

global char    *TextSegName;   /* name of the text segment */
global char    *DataSegName;   /* name of the data segment */
global char    *CodeClassName; /* name of the code class */
global char    *ModuleName;    /* name of module */
global char    *ObjectFileName;/* name of object file */
global unsigned PackAmount;    /* current packing alignment for fields */
global unsigned GblPackAmount; /* packing alignment given on command line */

                                     /* switches for code generator         */
global  unsigned long GenSwitches;   /* - generation info.                  */
global  unsigned long TargetSwitches;/* - target dependant generation info. */
global  unsigned long CpuSwitches;   /* - hardware info.                    */
global  char    *GenCodeGroup;  /* pointer to code group name */
global  int     ProEpiDataSize; /* data to be alloc'd for pro/epi hook */

global  int      DataThreshold; /* sizeof(obj) > this ==> separate segment */

global  int     DataPtrSize;
global  int     CodePtrSize;
global  char    Stack87;

global  FILE    *DefFile;       /* output for func prototypes */
global  char    *ErrorFileName;

#endif
