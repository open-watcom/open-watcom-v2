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


#ifndef __CTEXCEPT_H__
#define __CTEXCEPT_H__
//
// CTEXCEPT.H -- compile-time definitions for exceptions
//
// 92/10/30 -- J.W.Welch        -- defined

#include "carve.h"
#include "typesig.h"

typedef struct throw_cnv THROW_CNV;
struct throw_cnv                // THROW_CNV -- a throw conversions
{   THROW_CNV *next;            // - next conversion
    TYPE_SIG *sig;              // - type signature
    target_offset_t offset;     // - offset for conversion
};

typedef struct throw_cnv_ctl THROW_CNV_CTL;
struct throw_cnv_ctl            // THROW_CNV_CTL -- controls throw conversions
{   THROW_CNV *hdr;             // - ring of conversions
    THROW_CNV *cur;             // - current entry
    carve_t carver;             // - carve information
    TOKEN_LOCN err_locn;        // - location, in case of error
    TYPE src_type;              // - source type (most derived)
    target_offset_t offset;     // - offset of current element
    unsigned error_occurred :1; // - TRUE ==> error occurred
};

enum                            // TYPES OF MARKERS ON IC_SECT_xx
{   SECT_NULL           = 0     // - nothing defined
,   SECT_VIRTUAL        = 0x01  // - starting virtuals
,   SECT_DBASE          = 0x02  // - starting direct bases
,   SECT_MEMBER         = 0x04  // - starting members
,   SECT_CODE           = 0x08  // - starting special code
,   SECT_DTOR           = 0x10  // - doing a DTOR
,   SECT_CTOR           = 0x20  // - doing a CTOR
                                // masks:
,   SECT_CTOR_DTOR      = 0x30  // - type of routine being compiled
,   SECT_TYPE           = 0x03  // - type of section being processed
};

// PROTOTYPES:

void ExceptionsCheck(           // DIAGNOSE EXCEPTIONAL USE, WHEN DISABLED
    void )
;
THROBJ ThrowCategory(           // GET THROW-OBJECT CATEGORY FOR A TYPE
    TYPE type )                 // - type
;
target_offset_t ThrowBaseOffset(// GET OFFSET OF BASE
    SCOPE thr_scope,            // - scope of throw
    SCOPE base_scope )          // - scope for base
;
void ThrowCnvFini(              // THROW CONVERSIONS: COMPLETION
    THROW_CNV_CTL *ctl )        // - control area
;
unsigned ThrowCnvInit(          // THROW CONVERSIONS: INITIALIZE
    THROW_CNV_CTL *ctl,         // - control area
    TYPE type )                 // - type thrown
;
TYPE ThrowCnvType(              // THROW CONVERSIONS: GET NEXT TYPE
    THROW_CNV_CTL *ctl,         // - control area
    target_offset_t *offset )   // - addr[ offset ]
;
PTREE ThrowTypeSig(             // GET THROW ARGUMENT FOR TYPE SIGNATURE
    TYPE type,                  // - type to be thrown
    PTREE expr )                // - expression for errors
;
TYPE TypeCanonicalThr(          // GET CANONICAL THROW TYPE
    TYPE type )
;

#endif
