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


#ifndef __TYPESIG_H__
#define __TYPESIG_H__

// TYPESIG.H -- type-signature information
//
// 92/11/27 -- J.W.Welch        -- extracted from CGBACK.C

typedef struct type_sig TYPE_SIG;
PCH_struct type_sig {           // TYPE_SIG -- type signature
    TYPE_SIG*   next;           // - next in ring (hdr: type_sigs)
    TYPE_SIG*   base;           // - base entry (T*,T&)
    TYPE        type;           // - type being signatured
    SYMBOL      sym;            // - NULL or symbol for signature
    SYMBOL      dtor;           // - destructor
    SYMBOL      default_ctor;   // - null constructor
    SYMBOL      copy_ctor;      // - copy constructor
    unsigned    cgref : 1;      // - TRUE ==> ref'ed during code generation
    unsigned    cggen : 1;      // - TRUE ==> gen'ed during code generation
    unsigned    free : 1;       // used for precompiled headers
};

typedef enum                    // TYPE_SIG_ACCESS
{   TSA_GEN             = 0x01  // - signature must exist
,   TSA_FILL_OUT        = 0x02  // - fill out, without checking access
,   TSA_COPY_CTOR       = 0x04  // - copy constructor
,   TSA_DEFAULT_CTOR    = 0x08  // - null constructor
,   TSA_DTOR            = 0x10  // - destructor
,   TSA_NO_REF          = 0x20  // - don't set referenced bit in symbol
,   TSA_INDIRECT_ACCESS = 0x40  // - indirect access
,   TSA_INDIRECT_GEN    = 0x80  // - indirect generation
,   TSA_NO_ACCESS       = 0x100 // - no need to check access
,   TSA_INDIRECT = TSA_INDIRECT_ACCESS + TSA_INDIRECT_GEN
} TYPE_SIG_ACCESS;

// PROTOTYPES:

PTREE NodeTypeSig               // MAKE NODE FOR TYPE-SIG ADDRESS
    ( TYPE_SIG* sig )           // - type signature
;
PTREE NodeTypeSigArg            // MAKE ARGUMENT NODE FOR TYPE-SIG ADDRESS
    ( TYPE_SIG* sig )           // - type signature
;
TYPE_SIG *TypeSigFind(          // FIND TYPE SIGNATURE
    TYPE_SIG_ACCESS acc,        // - access type
    TYPE type,                  // - type for signature
    TOKEN_LOCN* err_locn,       // - error location for access errors
    bool *error_occurred )      // - to set error indication
;
void TypeSigSymOffset(          // GET SYMBOL,OFFSET FOR TYPE-SIG REFERENCE
    TYPE_SIG* sig,              // - type signature
    SYMBOL* a_sym,              // - addr[ symbol ]
    target_offset_t* a_offset ) // - addr[ offset ]
;
void TypeSigReferenced(         // EMIT CODE NOTING A TYPE-SIGNATURE REFERENCE
    TYPE_SIG *sig )             // - type signature
;
void TypeSigWalk(               // WALK THRU TYPE SIGNATURES
    void (*walker)              // - walking routine
        ( TYPE_SIG *sig ) )     // - - type signature
;

TYPE_SIG *TypeSigGetIndex( TYPE_SIG * );
TYPE_SIG *TypeSigMapIndex( TYPE_SIG * );

#endif
