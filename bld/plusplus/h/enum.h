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


#ifndef _ENUM_H

typedef struct {
    SYMBOL              sym;            // typedef symbol of current enum
    TYPE                type;           // type of current enum
    TOKEN_LOCN          locn;           // location of typedef symbol
    PTREE               id;             // PTREE of current enum name
    signed_64           next_value;     // next value
    uint_8              index;          // index into enum_ranges table
    type_id             base_id;        // id for base type
    uint_8              next_signed : 1;// TRUE if next_value is signed
    uint_8              has_sign    : 1;// TRUE if at least one value is < 0
    unsigned                        : 0;// alignment
} ENUM_DATA;

extern void InitEnumState( ENUM_DATA *, PTREE );
extern void MakeEnumMember( ENUM_DATA *, PTREE, PTREE );
extern void EnumDefine( ENUM_DATA * );
extern DECL_SPEC *MakeEnumType( ENUM_DATA * );
extern DECL_SPEC *EnumReference( ENUM_DATA * );

#define _ENUM_H
#endif
