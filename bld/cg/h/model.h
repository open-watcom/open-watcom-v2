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


#include "cgswitch.h"

extern  cg_switches     Model, TargetModel, SaveModel;
extern  proc_revision   CGProcessorVersion;

#define _IsModel( mdl )         ( ( Model & (mdl) ) != 0 )
#define _IsntModel( mdl )       ( ( Model & (mdl) ) == 0 )

#define _IsTargetModel( mdl )   ( ( TargetModel & (mdl) ) != 0 )
#define _IsntTargetModel( mdl ) ( ( TargetModel & (mdl) ) == 0 )

#ifdef GET_CPU
    #define _CPULevel( v )  ( GET_CPU( CGProcessorVersion ) >= (v) )
#else
    #define _CPULevel( v )  ( CGProcessorVersion >= (v) )
#endif

#ifdef GET_FPU
    #define _FPULevel( v )  ((GET_FPU(CGProcessorVersion) & ~FPU_EMU) >= (v) )
    #define _IsEmulation()  ((CGProcessorVersion & FPU_EMU) != 0)
#else
    #define _FPULevel( v )  0
#endif
