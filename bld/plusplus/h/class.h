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


#ifndef _CLASS_H

typedef struct vf_hide VF_HIDE;

typedef struct class_data CLASS_DATA;
struct class_data {
    CLASS_DATA      *next;              /* enclosing class' data */
    CLASS_DATA      *inline_data;       /* inlines stored here (non-NULL if defn seen) */
    char            *name;              /* name of class */
    SYMBOL          sym;                /* symbol of current class typedef */
    TYPE            type;               /* type of current class */
    SCOPE           scope;              /* scope of current class */
    CLASSINFO       *info;              /* info part of class type */
    BASE_CLASS      *bases;             /* base classes of current class */
    BASE_CLASS      *base_vbptr;        /* base who's vbptr we are using */
    BASE_CLASS      *base_vfptr;        /* base who's vfptr we are using */
#ifdef OPTIMIZE_EMPTY
    BASE_CLASS      *last_empty;        /* last empty base class (for optimizing) */
#endif
    DECL_INFO       *inlines;           /* ring of pending inline functions */
    DECL_INFO       *defargs;           /* ring of pending default arg exprs */
    void            *fn_pragma;         /* function pragma for member functions */
    TYPE            class_mod_type;     /* type representing mod in class <mod> X */
    TYPE            base_class_with_mod;/* base class with a class mod */
    VF_HIDE         *vf_hide_list;      /* vfns possibly hidden by this class */
    error_state_t   errors;             /* error state at beginning of class */
    target_offset_t start;              /* first offset in class */
    target_offset_t offset;             /* current offset in class */
    target_offset_t boffset;            /* offset of current bitfield */
    target_offset_t max_align;          /* maximum alignment of any field */
    target_offset_t pack_amount;        /* class packing requirement */
    target_offset_t vf_offset;          /* offset of virtual fn ptr field */
    target_offset_t vb_offset;          /* offset of virtual base ptr field */
    uint_16         vf_index;           /* next index for virtual fns */
    uint_16         vb_index;           /* next index for virtual bases */
    type_flag       tflag;              /* TF1_STRUCT or TF1_UNION */
    type_flag       fn_flags;           /* function flags for member functions */
    type_flag       mod_flags;          /* modifier flags for members */
    symbol_flag     perm;               /* current permissions */
    uint_8          bit_offset;         /* next bit offset for a bitfield */
    uint_8          bit_available;      /* available bits in current bitfield */
    uint_8          bit_unit_width;     /* total bits in current bitfield */

    unsigned        bitfield : 1;       /* bitfield in progress */
    unsigned        defined : 1;        /* class has been defined */
    unsigned        local_class : 1;    /* class is being def'd in a function */
    unsigned        nested_class : 1;   /* class is being def'd in a class */
    unsigned        allow_typedef : 1;  /* allow typedef with same name as class */
    unsigned        is_union : 1;       /* current class is a union */
    unsigned        is_explicit : 1;    /* programmer can still define members */
    unsigned        own_vfptr : 1;      /* brand new vfptr */
    unsigned        nameless_OK : 1;    /* we don't need a declarator */
    unsigned        generic : 1;        /* template arg "class T" has been found */
    unsigned        class_template : 1; /* processing a class template */
    unsigned        specific_defn : 1;  /* class T<> {} definition */
    unsigned        zero_array_defd : 1;/* zero size array has been defined */
    unsigned        member_mod_adjust :1;/* class <modifier> <id> was specified */
    unsigned        class_idiom : 1;    /* class C; idiom used */

    unsigned        has_const_copy : 1; /* def'n has const copy ctor decl */
    unsigned        has_nonconst_copy:1;/* def'n has non const copy ctor decl */
    unsigned        has_const_opeq : 1; /* def'n has const op= decl */
    unsigned        has_nonconst_opeq:1;/* def'n has non const op= decl */
    unsigned        has_explicit_opeq:1;/* def'n has explicit op= decl */

                                        /* class has a ... */
    unsigned        a_private : 1;      /* private member */
    unsigned        a_protected : 1;    /* protected member */
    unsigned        a_public : 1;       /* public member */
    unsigned        a_const : 1;        /* const member */
    unsigned        a_reference : 1;    /* reference member */

    unsigned        in_defn : 1;        /* ClassEnd will be called */
};

typedef enum {                  // context of class
    CLINIT_PACKED       = 0x01, // _Packed class required
    CLINIT_TEMPLATE_DECL= 0x02, // class X is inside a template declaration
    CLINIT_NULL         = 0x00
} CLASS_INIT;

typedef enum {                  // context of class name
    CLASS_DEFINITION    = 1,    // class C : ... OR class C { ...
    CLASS_DECLARATION   = 2,    // class C;
    CLASS_GENERIC       = 3,    // template <class T, ...
    CLASS_REFERENCE     = 0     // class X *p; ...
} CLASS_DECL;

typedef enum {                  // state after class name
    CLNAME_CONTINUE     = 1,    // parse can continue
    CLNAME_PROBLEM      = 2,    // parse cannot continue
    CLNAME_NULL         = 0
} CLNAME_STATE;

extern void ClassInit( void );
extern void ClassFini( void );
extern void ClassInitState( type_flag, CLASS_INIT, TYPE );
extern void ClassPermission( symbol_flag );
extern CLNAME_STATE ClassName( PTREE, CLASS_DECL );
extern void ClassSpecificInstantiation( PTREE, CLASS_DECL );
extern void ClassMember( SCOPE, SYMBOL );
extern void ClassBitfield( DECL_SPEC *, PTREE, PTREE );
extern void ClassStart( void );
extern inherit_flag ClassBaseQualifiers( inherit_flag, inherit_flag );
extern BASE_CLASS *ClassBaseSpecifier( inherit_flag, DECL_SPEC * );
extern BASE_CLASS *ClassBaseList( BASE_CLASS *, BASE_CLASS * );
extern void ClassBaseClause( BASE_CLASS * );
extern DECL_SPEC *ClassEnd( void );
extern DECL_SPEC *ClassRefDef( void );
extern void ClassStoreInlineFunc( DECL_INFO * );
extern void ClassStoreDefArg( DECL_INFO * );
extern void ClassAccessDeclaration( PTREE, TOKEN_LOCN * );
extern void ClassAccessTypeDeclaration( DECL_SPEC *, TOKEN_LOCN * );
extern void ClassMakeUniqueName( TYPE, char * );
extern boolean ClassAnonymousUnion( DECL_SPEC * );
extern TYPE ClassTagDefinition( TYPE, char * );
extern PTREE ClassMemInit( SYMBOL, REWRITE * );
extern void ClassPush( CLASS_DATA * );
extern void ClassPop( CLASS_DATA * );
extern void ClassCtorNullBody( SYMBOL );
extern void ClassDtorNullBody( SYMBOL );
extern void ClassAssignNullBody( SYMBOL );
extern boolean ClassNeedsAssign( TYPE, boolean );
extern boolean ClassIsDefaultCtor( SYMBOL, TYPE );
extern boolean ClassIsDefaultCopy( SYMBOL, TYPE );
extern boolean ClassIsDefaultAssign( SYMBOL, TYPE );
extern SYMBOL ClassAddDefaultCtor( SCOPE );
extern SYMBOL ClassAddDefaultCopy( SCOPE );
extern SYMBOL ClassAddDefaultDtor( SCOPE );
extern SYMBOL ClassAddDefaultAssign( SCOPE );
extern boolean ClassCorrupted( TYPE );
extern TYPE ClassUnboundTemplate( char * );
extern void ClassChangingScope( SYMBOL, SCOPE );
extern void ClassDefineRefdDefaults( void );
extern void ClassAddFunctionMods( TYPE );
extern boolean ClassParmIsRef( TYPE );
extern TYPE ClassPreDefined( char *, TOKEN_LOCN * );
extern boolean ClassOKToRewrite( void );

BASE_CLASS *BaseClassGetIndex( BASE_CLASS * );
BASE_CLASS *BaseClassMapIndex( BASE_CLASS * );

#define _CLASS_H
#endif
