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
* Description:  Execution sampler data and functions.
*
****************************************************************************/


#ifndef global_data
    #define global_data  extern
  #ifdef __WINDOWS__
    #define Info                SharedMemory->Info
    #define SampleIndex         SharedMemory->SampleIndex
    #define SampleCount         SharedMemory->SampleCount
    #define LastSampleIndex     SharedMemory->LastSampleIndex
    #define FarWriteProblem     SharedMemory->FarWriteProblem
    #define CurrTick            SharedMemory->CurrTick
    #define LostData            SharedMemory->LostData
  #endif
#endif

global_data unsigned             InsiderTime;
global_data unsigned             Margin;
global_data unsigned             Ceiling;
global_data unsigned             SamplerOff;

#ifdef __WINDOWS__
global_data shared_data __far    * __near SharedMemory;
#else
global_data info_struct          Info;
global_data unsigned             SampleIndex;
global_data unsigned             SampleCount;
global_data unsigned             LastSampleIndex;
global_data bool                 FarWriteProblem;
global_data unsigned long        CurrTick;
global_data bool                 LostData;
#endif

global_data bool                 CallGraphMode;
global_data bool                 FirstSample;
global_data samp_block           FAR_PTR *Samples;
global_data samp_block           FAR_PTR *CallGraph;
global_data off                  CGraphOff;
global_data seg                  CGraphSeg;

global_data comm_region          Comm;

global_data char                 ExeName[128];
global_data char                 SampName[256];

#undef global_data
