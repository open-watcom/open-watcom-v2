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


#ifndef _TEMPLATE_H
#define _TEMPLATE_H

#define TEMPLATE_MAX_DEPTH              100

typedef struct template_data TEMPLATE_DATA;
struct template_data {
    TEMPLATE_DATA       *next;          // (stack)
    DECL_INFO           *args;          // template arguments
    SCOPE               decl_scope;     // template decl scope
    REWRITE             *defn;          // class template definition
    REWRITE             *member_defn;   // class template member definition
    char                *template_name; // name of the template
    TOKEN_LOCN          locn;           // location of class template id
    error_state_t       errors;         // error state at beginning
    unsigned            all_generic : 1;// all args are generic types
    unsigned            defn_found : 1; // a template defn has been found
    unsigned            member_found :1;// a class template member has been found
    unsigned            defn_added : 1; // class template defn has just been added
};

// these structures are private to TEMPLATE.C but they are exposed
// for debug dump routines

typedef struct class_inst CLASS_INST;   // class template instantiation
PCH_struct class_inst {
    CLASS_INST          *next;          // (ring)
    SCOPE               scope;          // scope containing instantiation
    TOKEN_LOCN          locn;           // location of first instantiation
    unsigned            must_process :1;// must be post-processed
    unsigned            dont_process :1;// should not be post-processed
    unsigned            processed : 1;  // has been post-processed
    unsigned            specific : 1;   // specific instantiation provided
    unsigned            locn_set : 1;   // locn field has been set
    unsigned            free : 1;       // used for precompiled headers
};

typedef struct template_member TEMPLATE_MEMBER; // class template member
PCH_struct template_member {
    TEMPLATE_MEMBER     *next;          // (ring)
    REWRITE             *defn;          // member definition
    char                **arg_names;    // argument names
};

PCH_struct template_info {
    TEMPLATE_INFO       *next;          // (ring)
    REWRITE             *defn;          // template def'n (may be NULL)
    CLASS_INST          *instantiations;// list of current instantiations
    TEMPLATE_MEMBER     *member_defns;  // external member defns
    unsigned            num_args;       // number of template arguments
    TYPE                unbound_type;   // type to use in unbound circumstances
    TYPE                *type_list;     // template argument types
    char                **arg_names;    // argument names
    SYMBOL              sym;            // template symbol
    unsigned            corrupted : 1;  // template def'n contained errors
    unsigned            defn_found : 1; // a template defn has been found
    unsigned            free : 1;       // used for precompiled headers
};

PCH_struct fn_template_defn {
    FN_TEMPLATE_DEFN    *next;          // (ring)
    SYMBOL              sym;            // template function
    REWRITE             *defn;          // always non-NULL
    unsigned            num_args;       // number of template arguments
    char                **arg_names;    // argument names
    TYPE                *type_list;     // template argument types (all generic)
};

typedef enum tc_instantiate {
    TCI_NO_CLASS_DEFN   = 0x01,         // don't process class def'n here
    TCI_NO_DECL_SPEC    = 0x02,         // don't allocate a DECL_SPEC
    TCI_NO_MEMBERS      = 0x04,         // don't process member defns in this module
    TCI_EXPLICIT_FULL   = 0x08,         // process all defns for class template
    TCI_SPECIFIC        = 0x10,         // specialization
    TCI_NULL            = 0x00
} tc_instantiate;

typedef enum tc_directive {
    TCD_EXTERN          = 0x01,         // compile no member fns in this module
    TCD_INSTANTIATE     = 0x02,         // compile all member fns in this module
    TCD_NULL            = 0x00
} tc_directive;

typedef enum tc_fn_control {
    TCF_GEN_FUNCTION    = 0x01,
    TCF_NULL            = 0x00
} tc_fn_control;

extern void TemplateDeclInit( TEMPLATE_DATA *, DECL_INFO * );
extern void TemplateDeclFini( void );
extern void TemplateFunctionCheck( SYMBOL, DECL_INFO * );
extern void TemplateFunctionAttachDefn( DECL_INFO * );
extern unsigned TemplateFunctionGenerate( SYMBOL *, arg_list *, TOKEN_LOCN *, SYMBOL *, boolean );
extern void TemplateClassDeclaration( PTREE );
extern boolean TemplateClassDefinition( PTREE );
extern DECL_SPEC *TemplateClassInstantiation( PTREE, PTREE, tc_instantiate );
extern void TemplateHandleClassMember( DECL_INFO * );
extern void TemplateProcessInstantiations();
extern boolean TemplateMemberCanBeIgnored( void );
extern boolean TemplateVerifyDecl( SYMBOL );
extern void TemplateSpecificDefnStart( char *, PTREE );
extern void TemplateSpecificDefnEnd( void );
extern SCOPE TemplateClassInstScope( TYPE );
extern SCOPE TemplateClassParmScope( TYPE );
extern boolean TemplateParmEqual( SYMBOL, SYMBOL );
extern void TemplateFunctionInstantiate( SYMBOL, SYMBOL, void * );
extern SYMBOL TemplateFunctionTranslate( SYMBOL, SCOPE * );
extern tc_fn_control TemplateFunctionControl( void );
extern TYPE TemplateUnboundInstantiate( TYPE, arg_list *, TOKEN_LOCN * );
extern SYMBOL ClassTemplateLookup( char * );
extern SYMBOL TemplateSymFromClass( TYPE );
extern void TemplateSetDepth( unsigned );
extern boolean TemplateUnboundSame( TYPE, TYPE );
extern void TemplateClassDirective( PTREE, tc_directive );
extern SYMBOL TemplateSetFnMatchable( SYMBOL );
extern void TemplateUsingDecl( SYMBOL, TOKEN_LOCN * );

extern TEMPLATE_INFO *TemplateClassInfoGetIndex( TEMPLATE_INFO * );
extern TEMPLATE_INFO *TemplateClassInfoMapIndex( TEMPLATE_INFO * );
extern FN_TEMPLATE_DEFN *TemplateFunctionInfoGetIndex( FN_TEMPLATE_DEFN * );
extern FN_TEMPLATE_DEFN *TemplateFunctionInfoMapIndex( FN_TEMPLATE_DEFN * );
typedef int  (*AInstSCOPE)( SCOPE scope );
extern int WalkTemplateInst( SYMBOL sym, AInstSCOPE fscope  );

#endif
