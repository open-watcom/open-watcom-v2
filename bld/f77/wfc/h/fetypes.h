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
* Description:  enumeration for FORTRAN front-end specific cg types
*
****************************************************************************/


    TY_COMPLEX = TY_FIRST_FREE,   // FORTRAN 77 COMPLEX*8
    TY_DCOMPLEX,                 // FORTRAN 77 COMPLEX*16
    TY_XCOMPLEX,                 // FORTRAN 77 COMPLEX*20
    TY_CHAR,                     // FORTRAN 77 CHARACTER*n
    TY_ADV_ENTRY,                // dimension entry in ADV
    TY_ADV_ENTRY_1,              // for 1-D arrays
    TY_ADV_ENTRY_2,              // for 2-D arrays
    TY_ADV_ENTRY_3,              // for 3-D arrays
    TY_ADV_ENTRY_4,              // for 4-D arrays
    TY_ADV_ENTRY_5,              // for 5-D arrays
    TY_ADV_ENTRY_6,              // for 6-D arrays
    TY_ADV_ENTRY_7,              // for 7-D arrays
    TY_ADV_LO,                   // "lo" field in dimension entry
    TY_ADV_HI,                   // "hi" field in dimension entry
    TY_LOCAL_POINTER,            // pointer to data in local data area
    TY_GLOBAL_POINTER,           // pointer to global data (single segment)
    TY_CHAR16,                   // SCB for "far16" support
    TY_CHAR_ALLOCATABLE,         // SCB for allocatable CHARACTER data
    TY_ARR_ALLOCATABLE,          // Allocatable array structure
    TY_ARR_ALLOCATABLE_EXTENDED, // Allocatable extended array structure
    TY_ADV_HI_CV,                // "hi" field in dimension entry for Codeview
    TY_ADV_ENTRY_CV,             // dimension entry in ADV for Codeview
    TY_USER_DEFINED,             // first user defined type
