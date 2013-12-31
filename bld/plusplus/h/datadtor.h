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


#ifndef __DATADTOR_H__
#define __DATADTOR_H__

// DATADTOR.H -- interface to control destruction during data initialization


PTREE DataDtorCompArrEl(        // MARK ARRAY ELEMENT AS DTORABLE COMPONENT
    PTREE expr,                 // - expression to be decorated
    target_offset_t index )     // - array index
;
PTREE DataDtorCompBeg(          // START OF DTORABLE OBJECT COMPONENTS
    PTREE expr )                // - expression to be decorated
;
PTREE DataDtorCompClass(        // MARK CLASS OBJECT AS DTORABLE COMPONENT
    PTREE expr,                 // - expression to be decorated
    target_offset_t offset,     // - offset of component
    DTC_KIND dtc_kind )         // - kind of component
;
PTREE DataDtorObjPop(           // COMPLETE DTORABLE OBJECT
    PTREE expr )                // - expression to be decorated
;
PTREE DataDtorObjPush(          // START OF DTORABLE OBJECT
    PTREE expr,                 // - expression to be decorated
    TYPE type,                  // - type of object
    SYMBOL init_sym,            // - symbol being initialized
    target_offset_t offset )    // - offset of object being initialized
;


#endif
