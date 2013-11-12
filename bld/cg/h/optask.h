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
* Description:  Prototypes for optask.c
*
****************************************************************************/


extern  code_lbl        *AskForNewLabel( void );
extern  code_lbl        *AskRTLabel( cg_sym_handle sym );
extern  code_lbl        *AskForLabel( cg_sym_handle sym );
extern  bool            AskIfReachedLabel( code_lbl *lbl );
extern  bool            AskIfRTLabel( code_lbl *lbl );
extern  bool            AskIfUniqueLabel( code_lbl *lbl );
extern  bool            AskIfCommonLabel( code_lbl *lbl );
extern  offset          AskAddress( code_lbl *lbl );
extern  pointer         AskLblPatch( code_lbl *lbl );
extern  cg_sym_handle   AskForLblSym( code_lbl *lbl );
extern  code_lbl        *AskForSymLabel( pointer hdl, cg_class class );
