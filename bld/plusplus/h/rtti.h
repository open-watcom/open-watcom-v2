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


#ifndef _RTTI_H
#define _RTTI_H

typedef struct _rtti_adjust_class RTTI_CLASS;
typedef struct _rtti_typeid RTTI_TYPEID;
typedef struct _rtti_adjust_vfptr RTTI_VFPTR;

struct _rtti_adjust_class {
    RTTI_CLASS          *next;
    RTTI_VFPTR          *vfptrs;        // list of all vfptr adjustors
    TYPE                class_type;     // host class type
    SYMBOL              sym;            // symbol to reference from vftable
    target_offset_t     offset;         // current offset from start of symbol
    unsigned            done : 1;       // all vfptrs have been registered
    unsigned            gen : 1;        // must be generated
    unsigned            cg_gen : 1;     // has been generated
    unsigned            free : 1;       // used for PCH
    unsigned            too_big : 1;    // cannot fit in segment
};

struct _rtti_typeid {
    RTTI_TYPEID         *next;
    TYPE                type;           // type for typeid
    SYMBOL              sym;            // symbol to reference
    unsigned            free : 1;       // used for PCH
};

struct _rtti_adjust_vfptr {
    RTTI_VFPTR          *next;
    unsigned            control;        // RA_* control mask
    target_offset_t     delta;          // delta for adjustor
    target_offset_t     offset;         // offset from start of symbol
    unsigned            free : 1;       // used for PCH
};

#define RttiAdjustSize()        ( 2 * TARGET_UINT )
#define RttiClassSize()         ( TARGET_UINT + CgDataPtrSize() )
#define RttiLeapSize()          ( 2 * TARGET_SHORT + 1 * TARGET_UINT + CgDataPtrSize() )

extern SYMBOL RttiBuild( SCOPE, CLASS_TABLE *, target_offset_t * );
extern void RttiDone( SCOPE );
extern void RttiRef( SYMBOL );
extern void RttiWalk( void (*)( RTTI_CLASS * ) );

// len is strlen( raw_name )
extern unsigned TypeidSize( unsigned len );
#define TypeidRawNameOffset()   ( CgDataPtrSize() )

extern SYMBOL TypeidAccess( TYPE );
extern SYMBOL TypeidICAccess( TYPE );
extern void TypeidRef( SYMBOL );
extern void TypeidWalk( void (*)( RTTI_TYPEID * ) );

#endif
