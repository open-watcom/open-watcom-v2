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


#ifndef __VFUN_H__
#define __VFUN_H__

// VFUN.H -- virtual functions support
//
// 93/02/05 -- J.W.Welch        -- defined

// prototypes

#define VFUN_BASE               (1)


PTREE AccessVirtualFnAddress(   // GET ADDRESS OF VIRTUAL FUNCTION
    PTREE node,                 // - class pointer
    SEARCH_RESULT *result,      // - access info
    SYMBOL sym )                // - symbol to call
;
void VfnAncestralWalk(          // WALK ANCESTRAL VIRTUAL FUNCTIONS
    SYMBOL sym,                 // - a virtual function
    void (*rtn)(                // - routine called for self, ancestors
        SYMBOL orig,            // - - original virtual function
        SYMBOL vfun,            // - - self or ancestor
        void *data ),           // - - data passed
    void *data )                // - data passed to routine
;
PTREE VfnDecorateCall(          // DECORATE VIRTUAL CALL
    PTREE expr,                 // - call-setup expression
    SYMBOL vfun )               // - virtual function
;
void VfnReference(              // EMIT VIRTUAL FUNCTION REFERENCE
    SYMBOL vfun )               // - a virtual function
;
PTREE VfunSetupCall(            // SETUP CODE FOR VIRTUAL FUNCTION CALL
    PTREE expr )                // - expression for setup
;


#endif
