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
// REGS         : register information
//

#include "ftnstd.h"
#include "wf77aux.h"

hw_reg_set              StackParms[] = { HW_D( HW_EMPTY ) };

#if _CPU == 8086 || _CPU == 386
  char                  IF_names[] = "IF@*";
  char                  IF_Xnames[] = "IF@X*";
#elif _CPU == _AXP || _CPU == _PPC
  char                  IF_names[] = "_IF_*";
  char                  IF_Xnames[] = "_IF_X*";
#else
  #error Unknown OS
#endif



static  pass_by         IFArgValue = { NULL, PASS_BY_VALUE };
static  pass_by         IFArgDescriptor = { NULL, PASS_BY_DESCRIPTOR };

#ifdef pick
#undef pick
#endif
#define pick(text,regset) text,

char    *RegNames[] = {
#include "regsdefn.h"
};

#undef pick
#define pick(text,regset) regset,

hw_reg_set  RegValue[] = {
#include "regsdefn.h"
};

byte            MaxReg = { sizeof( RegNames ) / sizeof( RegNames[0] ) - 1 };

#if _CPU == 8086

static  hw_reg_set      RtRtnParms[] =
    { HW_D_5( HW_AX, HW_BX, HW_CX, HW_DX, HW_FLTS ), HW_D( HW_EMPTY ) };

static  hw_reg_set      FortranParms[] =
    { HW_D_4( HW_AX, HW_BX, HW_CX, HW_DX ), HW_D( HW_EMPTY ) };

static  hw_reg_set      IFParms[] =
    { HW_D_5( HW_AX, HW_BX, HW_CX, HW_DX, HW_FLTS ), HW_D( HW_EMPTY ) };

hw_reg_set              WinParms[] =
    { HW_D_4( HW_AX, HW_BX, HW_CX, HW_DX ), HW_D( HW_EMPTY ) };

aux_info                RtRtnInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_SI ),
        "RT@*",
        NULL,
        0 };

aux_info                RtStopInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED | SUICIDAL | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_SI ),
        "RT@*",
        NULL,
        0 };

aux_info                RtVarInfo = {
        NULL,
        FAR | CALLER_POPS | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        StackParms,
        NULL,
        HW_D( HW_SI ),
        "RT@*",
        NULL,
        0 };

aux_info                CoRtnInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_SI ),
        "RT@*",
        NULL,
        0 };

aux_info               IFXInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        FortranParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        NULL,
        0 };

aux_info                IFInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_names,
        &IFArgValue,
        0 };

aux_info                IFCharInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_names,
        &IFArgDescriptor,
        0 };

aux_info                IFChar2Info = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        NULL,
        0 };

aux_info                IFVarInfo = {
        NULL,
        FAR | NO_MEMORY_READ | NO_MEMORY_CHANGED | CALLER_POPS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        StackParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        &IFArgValue,
        0 };

aux_info                DefaultInfo = {
        NULL,
        FAR,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        FortranParms,
        NULL,
        HW_D( HW_SI ),
        "^",
        NULL,
        0 };

#elif   _CPU == 386

static  hw_reg_set      RtRtnParms[] =
    { HW_D_5( HW_EAX, HW_EBX, HW_ECX, HW_EDX, HW_FLTS ), HW_D( HW_EMPTY ) };

static  hw_reg_set      FortranParms[] =
    { HW_D_4( HW_EAX, HW_EBX, HW_ECX, HW_EDX ), HW_D( HW_EMPTY ) };

static  hw_reg_set      IFParms[] =
    { HW_D_5( HW_EAX, HW_EBX, HW_ECX, HW_EDX, HW_FLTS ), HW_D( HW_EMPTY ) };

aux_info                RtRtnInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_ESI ),
        "RT@*",
        NULL,
        0 };

aux_info                RtStopInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | SUICIDAL | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_ESI ),
        "RT@*",
        NULL,
        0 };

aux_info                RtVarInfo = {
        NULL,
        CALLER_POPS | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        StackParms,
        NULL,
        HW_D( HW_ESI ),
        "RT@*",
        NULL,
        0 };

aux_info                CoRtnInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_ESI ),
        "RT@*",
        NULL,
        0 };

aux_info               IFXInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        FortranParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        NULL,
        0 };

aux_info                IFInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_names,
        &IFArgValue,
        0 };

aux_info                IFCharInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_names,
        &IFArgDescriptor,
        0 };

aux_info                IFChar2Info = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        NULL,
        0 };

aux_info                IFVarInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | CALLER_POPS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        StackParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        &IFArgValue,
        0 };

aux_info                DefaultInfo = {
        NULL,
        0,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        FortranParms,
        NULL,
        HW_D( HW_ESI ),
        "^",
        NULL,
        0 };

#elif _CPU == _AXP || _CPU == _PPC

static  hw_reg_set      RtRtnParms[] =
    { HW_D( HW_EMPTY )};

static  hw_reg_set      FortranParms[] =
    { HW_D( HW_EMPTY ) };

static  hw_reg_set      IFParms[] =
    { HW_D( HW_EMPTY ) };

aux_info                RtRtnInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_EMPTY ),
        "__RT_*",
        NULL,
        0 };

aux_info                RtStopInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | SUICIDAL | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_EMPTY ),
        "__RT_*",
        NULL,
        0 };

aux_info                RtVarInfo = {
        NULL,
        CALLER_POPS | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        StackParms,
        NULL,
        HW_D( HW_EMPTY ),
        "__RT_*",
        NULL,
        0 };

aux_info                CoRtnInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        RtRtnParms,
        NULL,
        HW_D( HW_EMPTY ),
        "__RT_*",
        NULL,
        0 };

aux_info               IFXInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        FortranParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        NULL,
        0 };

aux_info                IFInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_names,
        &IFArgValue,
        0 };

aux_info                IFCharInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_names,
        &IFArgDescriptor,
        0 };

aux_info                IFChar2Info = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        IFParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        NULL,
        0 };

aux_info                IFVarInfo = {
        NULL,
        NO_MEMORY_READ | NO_MEMORY_CHANGED | CALLER_POPS,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        StackParms,
        NULL,
        HW_D( HW_EMPTY ),
        &IF_Xnames,
        &IFArgValue,
        0 };

aux_info                DefaultInfo = {
        NULL,
        0,
        HW_D( HW_FULL ),
        HW_D( HW_EMPTY ),
        FortranParms,
        NULL,
        HW_D( HW_EMPTY ),
        "^",
        NULL,
        0 };

#else
  #error Unknown Platform
#endif
