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


#ifndef __CALLDIAG_H__
#define __CALLDIAG_H__

void CallDiagAmbiguous(         // DIAGNOSE AMBIGUOUS CALL
    PTREE expr,                 // - expression for error
    MSG_NUM msg,                // - message for error
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
void CallDiagNoMatch(           // DIAGNOSE NO MATCHES FOR CALL
    PTREE expr,                 // - call or ctor expression
    MSG_NUM msg_one,            // - message: one function
    MSG_NUM msg_many,           // - message: many functions
    PTREE this_node,            // - this node (or NULL)
    SYMBOL orig,                // - original symbol for overloading
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
void CtorDiagNoMatch(           // DIAGNOSE NO MATCHES FOR CTOR
    PTREE expr,                 // - ctor expression
    MSG_NUM msg_none,           // - message: no CTOR's
    FNOV_DIAG *fnov_diag )      // - diagnosis information
;
void CallDiagnoseRejects(       // DIAGNOSE FUNCTIONS IN REJECT LIST
    PTREE expr,                 // - expression for error
    MSG_NUM msg,                // - message for error
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
;
CNV_RETN AnalyseCtorDiag(       // ANALYSE CONSTRUCTOR
    TYPE type,                  // - type for CTOR
    SYMBOL *ctor,               // - ctor to be filled in
    PTREE *initial,             // - initialization arguments (modified)
    FNOV_DIAG *fnov_diag ) // Don't know how to implement LMW
;
void UdcDiagNoMatch(            // DIAGNOSE NO MATCHES FOR UDC LOOKUP
    PTREE src,                  // - ctor expression
    TYPE tgt_type,              // - target type
    MSG_NUM msg_none,           // - message: no UDC's
    MSG_NUM msg_many,           // - message: many functions
    FNOV_DIAG *fnov_diag )      // - overload diagnosis information
;

#endif // __CALLDIAG_H__
