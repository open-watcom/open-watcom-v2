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
* Description:  Reductions used by the PowerPC cg (see ppctable.c).
*
****************************************************************************/


_R_( R_SWAPOPS,         rSWAPOPS ),
_R_( R_MOVOP1TEMP,      rMOVOP1TEMP ),
_R_( R_MOVOP2TEMP,      rMOVOP2TEMP ),
_R_( R_MOVOP1REG,       rOP1REG ),
_R_( R_MOVOP2REG,       rOP2REG ),
_R_( R_MOVRESREG,       rMOVRESREG ),
_R_( R_DOCVT,           rDOCVT ),
_R_( R_MOVRESTEMP,      rMOVRESTEMP ),
_R_( R_PUSHTOMOV,       rPUSHTOMOV ),
_R_( R_POPTOMOV,        rPOPTOMOV ),
_R_( R_FORCEOP1MEM,     rOP1MEM ),
_R_( R_FORCEOP2MEM,     rOP2MEM ),
_R_( R_CONSTLOAD,       rCONSTLOAD ),
_R_( R_SWAPCMP,         rSWAPCMP ),
_R_( R_SIMPCMP,         rSIMPCMP ),
_R_( R_DOSET,           rDOSET ),
_R_( R_FORCEOP1CMEM,    rOP1CMEM ),
_R_( R_FORCEOP2CMEM,    rOP2CMEM ),
_R_( R_MAKECALL,        rMAKECALL ),
_R_( R_DOTEST,          rDOTEST ),
_R_( R_CHANGETYPE,      rCHANGETYPE ),
_R_( R_MOVEXX,          rMOVEXX ),
_R_( R_FORCERESMEM,     rFORCERESMEM ),
_R_( R_MOVEINDEX,       rMOVEINDEX ),
_R_( R_BIN2INT,         rBIN2INT ),
_R_( R_SHR,             rSHR ),
_R_( R_MOVEXX_4,        rMOVEXX_4 ),
_R_( R_MOD2DIV,         rMOD2DIV ),
_R_( R_SPLITMOVE,       rSPLITMOVE ),
_R_( R_SPLITOP,         rSPLITOP ),
_R_( R_SPLITCMP,        rSPLITCMP ),
_R_( R_SPLITNEG,        rSPLITNEG ),
_R_( R_SPLITUNARY,      rSPLITUNARY ),
_R_( R_CLRHI_4,         rCLRHI_4 ),
_R_( R_SEX_4TO8,        rSEX_4TO8 ),
_R_( R_MOVELOW,         rMOVELOW ),
