/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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

#if _CPU == 8086 || _CPU == 386
static char             IF_names[] = "IF@*";
static char             IF_Xnames[] = "IF@X*";
#elif _CPU == _AXP || _CPU == _PPC
static char             IF_names[] = "_IF_*";
static char             IF_Xnames[] = "_IF_X*";
#else
  #error Unknown OS
#endif



static  pass_by         IFArgValue = { NULL, PASS_BY_VALUE };
static  pass_by         IFArgDescriptor = { NULL, PASS_BY_DESCRIPTOR };

static const char       *RegNames[] = {
    #define pick(text,regset) text,
    #include "regsdefn.h"
    #undef pick
};

static hw_reg_set       RegValue[] = {
    #define pick(text,regset) regset,
    #include "regsdefn.h"
    #undef pick
};

#if _CPU == 8086

static  hw_reg_set      RtRtnParms[] =
    { HW_D_5( HW_AX, HW_BX, HW_CX, HW_DX, HW_FLTS ), HW_D( HW_EMPTY ) };

static  hw_reg_set      IFParms[] =
    { HW_D_5( HW_AX, HW_BX, HW_CX, HW_DX, HW_FLTS ), HW_D( HW_EMPTY ) };

static hw_reg_set       WinParms[] =
    { HW_D_4( HW_AX, HW_BX, HW_CX, HW_DX ), HW_D( HW_EMPTY ) };

static aux_info                RtRtnInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_SI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info                RtStopInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED | SUICIDAL | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_SI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info                RtVarInfo = {
        FAR_CALL | CALLER_POPS | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        StackParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_SI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info                CoRtnInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_SI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info               IFXInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        FortranParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        NULL
};

static aux_info                IFInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_names,
        0,
        0,
        &IFArgValue
};

static aux_info                IFCharInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_names,
        0,
        0,
        &IFArgDescriptor
};

static aux_info                IFChar2Info = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        NULL
};

static aux_info                IFVarInfo = {
        FAR_CALL | NO_MEMORY_READ | NO_MEMORY_CHANGED | CALLER_POPS,
        NULL,
        StackParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        &IFArgValue
};

#elif   _CPU == 386

static  hw_reg_set      RtRtnParms[] =
    { HW_D_5( HW_EAX, HW_EBX, HW_ECX, HW_EDX, HW_FLTS ), HW_D( HW_EMPTY ) };

static  hw_reg_set      IFParms[] =
    { HW_D_5( HW_EAX, HW_EBX, HW_ECX, HW_EDX, HW_FLTS ), HW_D( HW_EMPTY ) };

static aux_info                RtRtnInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_ESI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info                RtStopInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | SUICIDAL | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_ESI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info                RtVarInfo = {
        CALLER_POPS | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        StackParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_ESI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info                CoRtnInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_ESI ),
        HW_D( HW_FULL ),
        "RT@*",
        0,
        0,
        NULL
};

static aux_info               IFXInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        FortranParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        NULL
};

static aux_info                IFInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_names,
        0,
        0,
        &IFArgValue
};

static aux_info                IFCharInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_names,
        0,
        0,
        &IFArgDescriptor
};

static aux_info                IFChar2Info = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        NULL
};

static aux_info                IFVarInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | CALLER_POPS,
        NULL,
        StackParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        &IFArgValue
};

#elif _CPU == _AXP || _CPU == _PPC

static  hw_reg_set      RtRtnParms[] =
    { HW_D( HW_EMPTY )};

static  hw_reg_set      IFParms[] =
    { HW_D( HW_EMPTY ) };

static aux_info                RtRtnInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        "__RT_*",
        0,
        0,
        NULL
};

static aux_info                RtStopInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | SUICIDAL | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        "__RT_*",
        0,
        0,
        NULL
};

static aux_info                RtVarInfo = {
        CALLER_POPS | NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        StackParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        "__RT_*",
        0,
        0,
        NULL
};

static aux_info                CoRtnInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | REVERSE_PARMS,
        NULL,
        RtRtnParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        "__RT_*",
        0,
        0,
        NULL
};

static aux_info               IFXInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        FortranParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        NULL
};

static aux_info                IFInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_names,
        0,
        0,
        &IFArgValue
};

static aux_info                IFCharInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_names,
        0,
        0,
        &IFArgDescriptor
};

static aux_info                IFChar2Info = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED,
        NULL,
        IFParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        NULL
};

static aux_info                IFVarInfo = {
        NO_MEMORY_READ | NO_MEMORY_CHANGED | CALLER_POPS,
        NULL,
        StackParms,
        HW_D( HW_EMPTY ),
        HW_D( HW_EMPTY ),
        HW_D( HW_FULL ),
        IF_Xnames,
        0,
        0,
        &IFArgValue
};

#else
  #error Unknown Platform
#endif
