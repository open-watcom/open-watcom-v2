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


// CGINFO.H -- prototypes for code-generator front-end routines
//
// 91/06/19 -- J.W.Welch        -- defined

fe_attr FEAttr(                 // GET SYMBOL ATTRIBUTES
    SYMBOL sym )                // - symbol
;
void *FEAuxInfo(                // REQUEST AUXILLIARY INFORMATION
        SYMBOL sym,             // - symbol
        aux_class request )     // - request
;
back_handle FEBack(             // GET BACK HANDLE FOR A SYMBOL
    SYMBOL sym )                // - the symbol
;
dbg_type FEDbgType(             // GET DEBUG TYPE FOR SYMBOL
    SYMBOL sym )                // - symbol
;
int FELexLevel(                 // GET LEXICAL LEVEL OF SYMBOL
    SYMBOL sym )                // - the symbol
;
void FEMessage(                 // MESSAGES FROM CODE-GENERATOR
    msg_class class,            // - message class
    void *parm )                // - parameter
;
char *FEModuleName(             // RETURN MODULE NAME
    void )
;
char *FEName(                   // RETURN THE SYMBOL'S NAME
    SYMBOL sym )                // - symbol
;
int FEParmType(                 // ARGUMENT PROMOTION ?
    SYMBOL func,
    SYMBOL parm,
    cg_type tipe )
;
segment_id FESegID(             // GET SEGMENT ID FOR SYMBOL
    SYMBOL sym )                // - symbol
;
int FEStackChk(                 // STACK CHECKING ?
    SYMBOL sym )                // - the symbol
;
int FETrue(                     // RETURN TRUE
    void )
;
