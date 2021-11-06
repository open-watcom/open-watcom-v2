/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Intel architecture reduce function prototype declarations
*
****************************************************************************/


extern instruction      *rFLIPSIGN( instruction * );
extern instruction      *rTEMP2CONST( instruction * );
extern instruction      *rSAVEFACE( instruction * );
extern instruction      *rMULSAVEFACE( instruction * );
extern instruction      *rDOLONGPUSH( instruction * );
extern instruction      *rOP1CMEM( instruction * );
extern instruction      *rOP2CMEM( instruction * );
extern instruction      *rFSCONSCMP( instruction * );
extern instruction      *rHIGHLOWMOVE( instruction * );
extern instruction      *rMAKECYPMUL( instruction * );
extern instruction      *rMAKESTRCMP( instruction * );
extern instruction      *rMAKESTRMOVE( instruction * );
extern instruction      *rMOVELOW( instruction * );
extern instruction      *rMOVOP1MEM( instruction * );
extern instruction      *rOP2CL( instruction * );
extern instruction      *rOP2CX( instruction * );
extern instruction      *rSPLITUNARY( instruction * );
extern instruction      *rMULREGISTER( instruction * );
extern instruction      *rDIVREGISTER( instruction * );
extern instruction      *rCPSUB( instruction * );
extern instruction      *rPTSUB( instruction * );
extern instruction      *rU_TEST( instruction * );
extern instruction      *rEXTPT( instruction * );
extern instruction      *rMAYBSTRMOVE( instruction * );
extern instruction      *rSEG_SEG( instruction * );
extern instruction      *rCHPPT( instruction * );
extern instruction      *rMOVRESMEM( instruction * );
extern instruction      *rMAKEU4CONS( instruction * );
extern instruction      *rEXT_PUSHC( instruction * );
extern instruction      *rCMPCP( instruction * );
extern instruction      *rMOVPTI8( instruction * );
extern instruction      *rMOVI8PT( instruction * );
