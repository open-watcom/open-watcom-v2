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


#include "plusplus.h"

#include <stddef.h>

#include "cgfront.h"
#include "errdefns.h"
#include "ptree.h"
#include "name.h"
#include "defarg.h"
#include "ring.h"
#include "conpool.h"

#define stgClassInSet( s, m )   ((( 1 << (s)->id ) & (m) ) != 0 )

#define SCM_AUTOMATIC   \
        ( 1 << SC_AUTO )        | \
        ( 1 << SC_REGISTER )

#define SCM_CONSTANT            \
        ( 1 << SC_AUTO )        | \
        ( 1 << SC_REGISTER )    | \
        ( 1 << SC_NULL )        | \
        ( 1 << SC_STATIC )      | \
        ( 1 << SC_EXTERN )

#define SCM_NOT_DATA_OR_FUNC    \
        ( 1 << SC_TYPEDEF )     | \
        ( 1 << SC_ACCESS )

#define symIsDataFunction( s )  ( \
        (( (s)->flag & SF_ERROR ) == 0 ) && \
        ( ! stgClassInSet( sym, SCM_NOT_DATA_OR_FUNC ) ) \
        )

#define symIsFuncSym( s ) ( FunctionDeclarationType( (s)->sym_type ) != NULL )
#define symIsDataSym( s ) ( FunctionDeclarationType( (s)->sym_type ) == NULL )

#define symGetScope( sym, scope ) \
    { \
        SYMBOL_NAME sname = sym->name; \
        if( sname != NULL ) { \
            scope = sname->containing; \
        } else { \
            scope = NULL; \
        } \
    }


SCOPE SymScope(                 // GET SCOPE FOR SYMBOL
    SYMBOL sym )                // - the symbol
{
    SCOPE scope;                // - SCOPE for "sym"

    symGetScope( sym, scope );
    return( scope );
}


TYPE SymClass(                  // GET TYPE FOR CLASS CONTAINING A SYMBOL
    SYMBOL sym )                // - the symbol
{
    SCOPE scope;                // - SCOPE for "sym"
    SYMBOL templ_sym;

    sym = SymDefaultBase( SymDeAlias( sym ) );

    templ_sym = SymIsFunctionTemplateInst( sym );
    if( templ_sym != NULL ) {
        sym = templ_sym;
    }

    symGetScope( sym, scope );
    if( scope == NULL ) {
        return( NULL );
    }
    return( ScopeClass( scope ) );
}


static SCOPE symClassScope(     // GET SCOPE FOR CLASS CONTAINING SYMBOL
    SYMBOL sym )
{
    SCOPE scope;                // - SCOPE for "sym"
    SYMBOL templ_sym;

    sym = SymDefaultBase( SymDeAlias( sym ) );

    templ_sym = SymIsFunctionTemplateInst( sym );
    if( templ_sym != NULL ) {
        sym = templ_sym;
    }

    symGetScope( sym, scope );
    if( scope && ScopeType( scope, SCOPE_CLASS ) ) {
        return( scope );
    }
    return( NULL );
}


boolean SymIsData(              // TEST IF SYMBOL IS DATA
    SYMBOL sym )                // - the symbol
{
    return symIsDataFunction( sym ) && symIsDataSym( sym );
}


boolean SymIsFunction(          // TEST IF SYMBOL IS A FUNCTION
    SYMBOL sym )                // - the function
{
    return symIsDataFunction( sym ) && symIsFuncSym( sym );
}


boolean SymIsHugeData(          // TEST IF SYMBOL IS HUGE DATA
    SYMBOL sym )                // - the symbol
{
    type_flag flag;

    if( SymIsData( sym ) ) {
        TypeModFlags( sym->sym_type, &flag );
        return(( flag & TF1_HUGE ) != 0 );
    }
    return( FALSE );
}


boolean SymIsInMem(             // TEST IF SYMBOL SHOULD STAY IN MEMORY
    SYMBOL sym )                // - symbol
{
    type_flag flag;

    TypeModFlags( sym->sym_type, &flag );
    return(( flag & TF1_STAY_MEMORY ) != 0 );
}


boolean SymIsClassMember(       // TEST IF SYMBOL IS MEMBER OF A CLASS
    SYMBOL sym )                // - the symbol
{
    return symIsDataFunction( sym ) && symClassScope( sym ) != NULL;
}

boolean SymIsNameSpaceMember(    // TEST IF SYMBOL IS MEMBER OF NAMESPACE
    SYMBOL sym )                // - the symbol
{
    boolean     ret;
    SCOPE       scope;

    ret = FALSE;
    scope = SymScope( sym );
    if( ScopeId( scope ) == SCOPE_FILE ) {
        NAME_SPACE *ns;

        ns = scope->owner.ns;
        if( !ns->s.global_fs  ){
            ret = TRUE;
        }
    }
    return( ret );
}

boolean SymIsClassDefinition(   // TEST IF SYMBOL IS TYPEDEF FOR A CLASS
    SYMBOL sym )                // - the symbol
{
    TYPE type;                  // - type of class

    if( !SymIsTypedef( sym ) ) {
        return( FALSE );
    }
    type = StructType( sym->sym_type );
    if( type == NULL ) {
        return( FALSE );
    }
    if( !type->u.c.info->defined ) {
        return( FALSE );
    }
    if( sym->name->name != type->u.c.info->name ) {
        return( FALSE );
    }
    if( sym->name->containing != type->u.c.scope->enclosing ) {
        return( FALSE );
    }
    return( TRUE );
}


boolean SymIsInjectedTypedef(   // TEST IF SYMBOL IS INJECTED TYPEDEF
    SYMBOL sym )                // - the symbol
{
    TYPE type;                  // - type of class

    if( !SymIsTypedef( sym ) ) {
        return( FALSE );
    }
    type = StructType( sym->sym_type );
    if( type == NULL ) {
        return( FALSE );
    }
    if( !type->u.c.info->defined ) {
        return( FALSE );
    }
    if( sym->name->name != type->u.c.info->name ) {
        return( FALSE );
    }
    if( sym->name->containing != type->u.c.scope ) {
        return( FALSE );
    }
    return( TRUE );
}


boolean SymIsEnumDefinition(    // TEST IF SYMBOL IS AN ENUMERATION
    SYMBOL sym )                // - the symbol
{
    TYPE type;                  // - type of symbol

    if( !SymIsTypedef( sym ) ) {
        return( FALSE );
    }
    type = EnumType( sym->sym_type );
    if( type == NULL ) {
        return( FALSE );
    }
    if( type->u.t.sym != sym ) {
        return( FALSE );
    }
    return( TRUE );
}


#ifndef NDEBUG
SYMBOL SymDeAlias(              // REDUCE TO NON-ALIASED SYMBOL
    SYMBOL sym )                // - the symbol
{
    if( SymIsAlias( sym ) ) {
        sym = sym->u.alias;
    }
    DbgAssert( ! SymIsAlias( sym ) );
    return( sym );
}
#endif


boolean SymIsAutomatic(         // TEST IF SYMBOL IS AUTOMATIC VARIABLE
    SYMBOL sym )                // - the symbol
{
    sym = SymDeAlias( sym );
    return( stgClassInSet( sym, SCM_AUTOMATIC ) );
}


SYMBOL SymDefaultBase(          // REMOVE DEFAULT ARGUMENTS TO GET BASE SYMBOL
    SYMBOL sym )                // - the symbol
{
    while( SymIsDefArg( sym ) ) {
        sym = sym->thread;
    }
    return sym;
}


boolean SymIsStatic(     // DETERMINE IF SYMBOL IS STATIC
    SYMBOL sym )                // - the symbol
{
    symbol_class id;

    id = SymDefaultBase( sym )->id;
    return ( SC_STATIC == id ) || ( SC_STATIC_FUNCTION_TEMPLATE == id );
}


boolean SymIsStaticMember(      // TEST IF SYMBOL IS STATIC MEMBER
    SYMBOL sym )                // - the symbol
{
    return( SymIsClassMember( sym ) && SymIsStatic( sym ) );
}


boolean SymIsStaticDataMember(  // TEST IF SYMBOL IS STATIC DATA MEMBER
    SYMBOL sym )                // - the symbol
{
    return( SymIsStaticMember( sym ) && symIsDataSym( sym ) );
}


boolean SymIsThisMember(        // TEST IF SYMBOL IS DATA/FUNCTION MEMBER
    SYMBOL sym )                // - the symbol
{
    return SymIsClassMember( sym )
        && ! SymIsStatic( sym )
        && ( sym->id != SC_ENUM );
}


boolean SymIsThisDataMember(    // TEST IF SYMBOL IS THIS DATA MEMBER
    SYMBOL sym )                // - the symbol
{
    return SymIsThisMember( sym ) && symIsDataSym( sym );
}


boolean SymIsFuncMember(        // TEST IF SYMBOL IS A MEMBER FUNCTION
    SYMBOL sym )                // - the symbol
{
    return( SymIsClassMember( sym ) && symIsFuncSym( sym ) );
}


boolean SymIsStaticFuncMember(  // TEST IF SYMBOL IS A STATIC MEMBER FUNC.
    SYMBOL sym )                // - the symbol
{
    return( SymIsFuncMember( sym ) && SymIsStatic( sym ) );
}


boolean SymIsThisFuncMember(    // TEST IF SYMBOL IS A THIS MEMBER FUNC.
    SYMBOL sym )                // - the symbol
{
    return SymIsThisMember( sym ) && symIsFuncSym( sym );
}


boolean SymIsStaticData(        // TEST IF SYMBOL IS STATIC DATA ELEMENT
    SYMBOL sym )                // - the symbol
{
    SCOPE scope;                // - scope for symbol
    boolean retn;               // - TRUE ==> symbol is static data element

    retn = FALSE;
    if( SymIsData( sym ) ) {
        symGetScope( sym, scope );
        if( scope != NULL ) {
            switch( ScopeId( scope ) ) {
              case SCOPE_BLOCK :
              case SCOPE_FILE :
                if( sym->id == SC_STATIC ) {
                    retn = TRUE;
                }
                break;
            }
        }
    }
    return( retn );
}


CLASSINFO *SymClassInfo(        // GET CLASSINFO FOR SYMBOL
    SYMBOL sym )                // - the symbol
{
    return TypeClassInfo( SymClass( sym ) );
}


boolean SymRequiresDtoring(     // TEST IF SYMBOL REQUIRES DTOR-ING
    SYMBOL sym )                // - the symbol
{
    boolean retn;               // - TRUE ==> DTOR required

    if( SymIsData( sym ) ) {
        retn = TypeRequiresDtoring( sym->sym_type );
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymRequiresCtoring(     // TEST IF SYMBOL REQUIRES DTOR-ING
    SYMBOL sym )                // - the symbol
{
    boolean retn;               // - TRUE ==> DTOR required

    if( SymIsData( sym ) ) {
        retn = TypeRequiresCtoring( sym->sym_type );
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymClassCorrupted(      // TEST IF SYMBOL'S CLASS CORRUPTED
    SYMBOL sym )                // - the symbol
{
    TYPE type;                  // - scope's class type

    type = SymClass( sym );
    if( type != NULL ) {
        if( TypeDefined( type ) && ! type->u.c.info->corrupted ) {
            return( FALSE );
        }
    }
    return( TRUE );
}


arg_list *SymFuncArgList(       // GET FUNCTION ARG. LIST
    SYMBOL func )               // - a function SYMBOL
{
    return( TypeArgList( func->sym_type ) );
}


static SYMBOL symAllocate(      // ALOOCATE A NEW SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags )         // - symbol flags
{
    SYMBOL sym = AllocSymbol();
    sym->id = id;
    sym->sym_type = type;
    sym->flag = flags;
    return sym;
}


SYMBOL SymMakeDummy(            // MAKE A DUMMY SYMBOL
    TYPE type,                  // - type of the symbol
    char **name )               // - gets filled in with the name
{
    *name = NameDummy();
    return symAllocate( type, 0, 0 );
}


boolean SymIsTemporary(         // DETERMINE IF INTERNAL SYMBOL
    SYMBOL sym )                // - the symbol
{
    boolean retn;               // - TRUE ==> symbol is internal symbol

    if( ! SymIsData( sym ) ) {
        retn = FALSE;
    } else if( SymIsAnError( sym ) ) {
        retn = TRUE;
    } else if( sym->name->name[0] == NAME_DUMMY_PREFIX_0 ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymIsGenedFunc(         // DETERMINE IF SYMBOL IS A GENERATED FUNC.
    SYMBOL sym )                // - the symbol
{
    boolean retn;               // - TRUE ==> symbol is internal symbol

    if( ! SymIsFunction( sym ) ) {
        retn = FALSE;
    } else if( SymIsAnError( sym ) ) {
        retn = TRUE;
    } else if( sym->name->name[0] == NAME_DUMMY_PREFIX_0 ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymIsClass(             // DETERMINE IF SYMBOL IS A CLASS OBJECT
    SYMBOL sym )                // - the symbol
{
    return( SymUnmodifiedType( sym )->id == TYP_CLASS );
}


boolean SymIsClassArray(        // DETERMINE IF SYMBOL IS ARRAY OF CLASS ITEMS
    SYMBOL sym )                // - the symbol
{
    TYPE array_type;            // - array type
    TYPE base_type;             // - base type, when array
    boolean retn;               // - TRUE ==> is array of class objects

    array_type = ArrayType( sym->sym_type );
    if( NULL == array_type ) {
        retn = FALSE;
    } else {
        base_type = ArrayBaseType( array_type );
        if( NULL == StructType( base_type ) ) {
            retn = FALSE;
        } else {
            retn = TRUE;
        }
    }
    return retn;
}


SYMBOL SymIsAnonymous(          // TEST IF SYMBOL IS AN ANONYMOUS UNION MEMBER
    SYMBOL sym )                // - the symbol
{
    SYMBOL ret_sym;
    SCOPE scope;

    ret_sym = NULL;
    if( SymIsData( sym ) ) {
        symGetScope( sym, scope );
        if( scope != NULL ) {
            switch( ScopeId( scope ) ) {
            case SCOPE_FILE:
            case SCOPE_BLOCK:
                if( SymIsAnonymousMember( sym ) ) {
                    ret_sym = sym->u.alias;
                }
                break;
            }
        }
    }
    return( ret_sym );
}


SYMBOL SymFunctionReturn(       // GET SYMBOL FOR RETURN
    void )
{
    SYMBOL retn;                // - return symbol
    SEARCH_RESULT *result;      // - search result
    SCOPE fun_scope;            // - scope for current function

    result = ScopeFindNaked( GetCurrScope()
                           , CppSpecialName( SPECIAL_RETURN_VALUE ) );
    if( result == NULL ) {
        retn = NULL;
    } else {
        fun_scope = ScopeFunctionScopeInProgress();
        if( ( result->scope == fun_scope )
          ||( result->scope->enclosing == fun_scope ) ) {
            retn = result->sym_name->name_syms;
            retn->flag |= SF_REFERENCED;
        } else {
            retn = NULL;
        }
        ScopeFreeResult( result );
    }
    return( retn );
}


SYMBOL SymAllocReturn(          // ALLOCATE A RETURN SYMBOL
    SCOPE scope,                // - scope for symbol
    TYPE type )                 // - symbol's type
{
    return SymCreate( type
                    , SC_AUTO
                    , SF_REFERENCED
                    , CppSpecialName( SPECIAL_RETURN_VALUE )
                    , scope );
}


boolean SymIsArgument(          // TEST IF SYMBOL IS AN ARGUMENT
    SYMBOL sym )                // - symbol
{
    SCOPE scope;                // - symbol scope
    boolean retn;               // - return

    symGetScope( sym, scope );
    if( scope == NULL ) {
        retn = FALSE;
    } else if( ScopeId( scope ) == SCOPE_FUNCTION ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymIsThreaded(          // TEST IF SYMBOL IS A __declspec(thread)
    SYMBOL sym )                // - symbol
{
    type_flag flags;            // - flags

    TypeModFlags( sym->sym_type, &flags );
    return( ( flags & TF1_THREAD ) != 0 );
}


boolean SymIsDllImport(         // TEST IF SYMBOL IS A __declspec(dllimport)
    SYMBOL sym )                // - symbol
{
    type_flag flags;            // - flags

    TypeModFlags( sym->sym_type, &flags );
    return( ( flags & TF1_DLLIMPORT ) != 0 );
}


boolean SymIsConstant(          // TEST IF SYMBOL IS A CONSTANT
    SYMBOL sym )                // - symbol
{
    boolean retn;               // - return
    TYPE type;                  // - unmodified type
    type_flag flags;            // - flags

    if( SymIsEnumeration( sym ) ) {
        retn = TRUE;
    } else if( SymIsData( sym ) ) {
        type = TypeModFlags( sym->sym_type, &flags );
        if(( flags & TF1_CONST ) && stgClassInSet( sym, SCM_CONSTANT )
          &&( NULL != IntegralType( type ) ) ) {
            retn = TRUE;
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    return retn;
}


static boolean funTypeFlagged(  // TEST IF ANY FLAGS ON IN FUNCTION TYPE
    SYMBOL fun,                 // - possible function symbol
    type_flag flags )           // - flags to be tested
{
    boolean retn;               // - FALSE ==> not inlined
    TYPE type;                  // - unmodified type for function

    type = FunctionDeclarationType( fun->sym_type );
    if( type == NULL ) {
        retn = FALSE;
    } else {
        retn = ( type->flag & flags ) != 0;
    }
    return retn;
}


boolean SymIsInline(            // TEST IF FUNCTION IS INLINE
    SYMBOL fun )                // - function symbol
{
    return funTypeFlagged( fun, TF1_INLINE );
}


boolean SymIsVirtual(           // TEST IF FUNCTION IS VIRTUAL
    SYMBOL fun )                // - function symbol
{
    return funTypeFlagged( fun, TF1_VIRTUAL );
}


boolean SymIsPure(              // TEST IF A PURE VIRTUAL FUNCTION
    SYMBOL fun )                // - a function
{
    return funTypeFlagged( fun, TF1_PURE );
}


scf_mask SymComdatFunInfo(      // TEST IF FUNCTION IS IN COMDAT SEGMENT
    SYMBOL sym )                // - function symbol
{
    SCOPE scope;
    TYPE fn_type;
    type_flag flag;

    symGetScope( sym, scope );
    if( ScopeEnclosedInUnnamedNameSpace( scope ) ) {
        return( SCF_NULL );
    }
    fn_type = TypeModFlags( sym->sym_type, &flag );
    DbgAssert( fn_type->id == TYP_FUNCTION );
    if( fn_type->flag & TF1_INLINE ) {
        scf_mask inline_fn = SCF_COMDAT;

        if( flag & TF1_DLLEXPORT ) {
            // linker will complain about multiple exports
            return( inline_fn );
        }
        if( SymIsThunk( sym ) || SymIsCtorOrDtor( sym ) ) {
            // typesigs depend on public symbols since the typesig
            // depends on accumulating its contents from multiple
            // modules (since it is always COMDAT) (link14,link15)
            return( inline_fn );
        }
        if( SymIsMustGen( sym ) ) {
            // inline fn must go out so make it COMDAT in case
            // it is an externed template member fn (link11)
            return( inline_fn );
        }
        if( CompFlags.static_inline_fns ) {
            inline_fn |= SCF_STATIC;
        }
        return( inline_fn );
    }
    if( flag & TF1_COMMON ) {
        return( SCF_COMDAT );
    }
    if( SymIsFunctionTemplateInst( sym ) != NULL ) {
        /* function template instantiations */
        return( SCF_COMDAT );
    }
    if( SymIsClassTemplateMember( sym ) ) {
        /* member function template instantiations */
        return( SCF_COMDAT );
    }
    return( SCF_NULL );
}


boolean SymIsComdatData(        // TEST IF DATA SYMBOL IS COMDAT
    SYMBOL sym )                // - data symbol
{
    TYPE var_type;
    type_flag flag;
    SCOPE scope;
    SYMBOL fn_sym;

    symGetScope( sym, scope );
    if( !scope || ScopeEnclosedInUnnamedNameSpace( scope ) ) {
        return( FALSE );
    }
    var_type = TypeModFlags( sym->sym_type, &flag );
    DbgAssert( var_type->id != TYP_FUNCTION );
    if( flag & TF1_COMMON ) {
        return( TRUE );
    }
    if( SymIsClassTemplateMember( sym ) ) {
        /* class template static member instantiations */
        return( TRUE );
    }
    /* check for local static data in a function */
    if( ScopeId( scope ) == SCOPE_BLOCK && SymIsStaticData( sym ) ) {
        /* local static data is COMMON if the host fn is COMMON */
        fn_sym = ScopeFunctionScope( scope );
        if( SymIsComdatFun( fn_sym ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


boolean SymCDtorExtraParm(      // does the CTOR/DTOR need the extra int arg?
    SYMBOL sym )                // - the symbol
{
    boolean retn;               // - return value

    if( SymIsCtor( sym ) ) {
        retn = TypeRequiresCtorParm( SymClass( sym ) );
    } else if( SymIsDtor( sym ) ) {
        retn = TRUE;
    } else {
        retn = FALSE;
    }
    return retn;
}


boolean SymIsModuleDtorable(    // TEST IF SYMBOL IS MODULE-DTORABLE
    SYMBOL sym )                // - a DTORable symbol
{
    SCOPE scope;                // - symbol scope
    boolean retn;               // - TRUE ==> module dtorable

    /*
        three cases:

            (1) file scope variables (but not temporaries)
            (2) class static members
            (3) function static variables
    */
    symGetScope( sym, scope );
    if( ( sym->name->name[0] != NAME_DUMMY_PREFIX_0 )
     && ( ScopeId( scope ) == SCOPE_FILE ) ) {
        retn = TRUE;
    } else {
        retn = ( sym->id == SC_STATIC );
    }
    return retn;
}


boolean SymIsConversionToType(  // SYMBOL IS operator T()?
    SYMBOL sym,                 // - symbol to check
    TYPE type )                 // - type T
{
    TYPE fn_type;

    fn_type = FunctionDeclarationType( sym->sym_type );
    if( fn_type != NULL ) {
        if( TypesIdentical( type, fn_type->of ) ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


static boolean symHasFuncName(  // DETERMINE IF NAMED FUNCTION
    SYMBOL func,                // - function symbol
    char *name )                // - name used
{
    boolean retn;               // - return: TRUE ==> is named function

    if( SymIsFunction( func ) ) {
        retn = ( name == func->name->name );
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymIsCtor(              // TEST IF SYMBOL IS A CTOR FUNCTION
    SYMBOL ctor )               // - potential CTOR function
{
    return symHasFuncName( ctor, CppConstructorName() );
}


boolean SymIsDtor(              // TEST IF SYMBOL IS A DTOR FUNCTION
    SYMBOL dtor )               // - potential DTOR function
{
    return symHasFuncName( dtor, CppDestructorName() );
}


boolean SymIsCtorOrDtor(        // TEST IF SYMBOL IS CTOR OR DTOR
    SYMBOL func )               // - hopefully, one of them
{
    boolean retn;               // - return: TRUE ==> is named function

    if( SymIsFunction( func ) ) {
        char const *name = func->name->name;
        retn = ( name == CppConstructorName() )
             ||( name == CppDestructorName()  );
    } else {
        retn = FALSE;
    }
    return( retn );
}


boolean SymIsAssign(            // TEST IF SYMBOL IS "operator ="
    SYMBOL op_eq )              // - potential op= function
{
    return symHasFuncName( op_eq, CppOperatorName( CO_EQUAL ) );
}


boolean SymIsOpDel(             // TEST IF SYMBOL IS "operator delete"
    SYMBOL op_del )             // - potential operator delete
{
    return symHasFuncName( op_del, CppOperatorName( CO_DELETE ) );
}


boolean SymIsOpDelar(           // TEST IF SYMBOL IS "operator delete[]"
    SYMBOL op_del )             // - potential operator delete[]
{
    return symHasFuncName( op_del, CppOperatorName( CO_DELETE_ARRAY ) );
}


boolean SymIsUDC(               // TEST IF SYMBOL IS USER-DEFINED CONVERSION
    SYMBOL udc )                // - symbol in question
{
    return symHasFuncName( udc, CppConversionName() );
}


boolean SymIsThunkDtor(         // TEST IF DTOR ADDRESSABILITY THUNK
    SYMBOL sym )                // - function symbol
{
    return symHasFuncName( sym, CppSpecialName( SPECIAL_DTOR_THUNK ) );
}


boolean SymIsThunkCtorDflt(     // TEST IF DEFAULT CTOR ADDRESSABILITY THUNK
    SYMBOL sym )                // - function symbol
{
    return symHasFuncName( sym, CppSpecialName( SPECIAL_CTOR_THUNK ) );
}


boolean SymIsThunkCtorCopy(     // TEST IF COPY CTOR ADDRESSABILITY THUNK
    SYMBOL sym )                // - function symbol
{
    return symHasFuncName( sym, CppSpecialName( SPECIAL_COPY_THUNK ) );
}


boolean SymIsClassTemplateMember(// TEST IF SYMBOL IS MEMBER OF CLASS TEMPLATE
    SYMBOL sym )                // - the symbol    \\ i.e., a class instantiation
{
    SCOPE host_scope;
    TYPE host_class;

    host_scope = symClassScope( sym );
    if( host_scope != NULL ) {
        /* properly handle members of nested classes within a class template */
        host_class = ScopeClass( ScopeHostClass( host_scope ) );
        if(( host_class->flag & TF1_INSTANTIATION ) != 0 ) {
            return( TRUE );
        }
    }
    return( FALSE );
}


SYMBOL SymIsFunctionTemplateInst(// TEST IF SYMBOL WAS GENERATED FROM A FUNCTION
    SYMBOL sym )                // - symbol                     \\     TEMPLATE
{
    SCOPE scope;

    if( SymIsFnTemplateMatchable( sym ) ) {
        symGetScope( sym, scope );
        if( ScopeType( scope, SCOPE_FILE )
         || ScopeType( scope, SCOPE_TEMPLATE_INST ) ) {
            /* get function template sym */
            sym = sym->u.alias;
            if( sym != NULL ) {
                return( sym );
            }
        }
    }
    return( NULL );
}


boolean SymIsRegularStaticFunc( // TEST IF SYMBOL IF NON-MEMBER STATIC FUNC.
    SYMBOL sym )                // - the symbol
{
    return ( sym->id == SC_STATIC )
        && SymIsFunction( sym )
        && ! SymIsClassMember( sym );
}


void SymLocnCopy(               // COPY LOCATION TO SYMBOL FROM ANOTHER SYMBOL
    SYMBOL dest,                // - 'to' symbol
    SYMBOL src )                // - 'from' symbol
{
    if( NULL != dest->locn ) {
        SymbolLocnFree( dest->locn );
    }
    dest->locn = src->locn;
    src->locn = NULL;
}


boolean SymIsExtern(            // SYMBOL IS DEFINED OUTSIDE THIS MODULE
    SYMBOL sym )                // - the symbol
{
    boolean retn;

    retn = FALSE;
    if( ! SymIsInitialized( sym ) ) {
        if( sym->id != SC_EXTERN ) {
            if( SymIsStaticDataMember( sym ) ) {
                /* uninitialized static data members are external */
                retn = TRUE;
            }
        } else {
            retn = TRUE;
        }
    }
#ifndef NDEBUG
    if( retn && ! CompFlags.parsing_finished ) {
        // symbol still has a chance of being initialized!
        CFatal( "SymIsExtern return value is not accurate" );
    }
#endif
    return( retn );
}


static boolean symIsAnonymousStructUnion( SYMBOL sym )
{
    TYPE type;

    type = StructType( sym->sym_type );
    if( type == NULL ) {
        return( FALSE );
    }
    if( ! type->u.c.info->anonymous ) {
        return( FALSE );
    }
    return( TRUE );
}


boolean SymIsNextInitializableMember(// SYMBOL CAN BE BRACE INITIALIZED
    SYMBOL *prev,               // - previous initializable member
    SYMBOL sym )                // - the member
{
    SYMBOL save_prev;

    /*
       Most exits want to cancel the previous prev
       because any anon union will be finished when
       we bump into one of these symbols. We want to
       prevent any problems with the following case:
       e.g.,
        struct S {
            union {
                int a;
                char b;
            };
            union {
                char c;
                int d;
            };
        } x = { 1, 'a' };
    */
    save_prev = *prev;
    *prev = NULL;
    if( ! SymIsData( sym ) ) {
        return( FALSE );
    }
    if( SymIsTypedef( sym ) ) {
        return( FALSE );
    }
    if( SymIsEnumeration( sym ) ) {
        return( FALSE );
    }
    if( SymIsStaticMember( sym ) ) {
        return( FALSE );
    }
    if( symIsAnonymousStructUnion( sym ) ) {
        return( FALSE );
    }
    if( save_prev != NULL ) {
        if( SymIsAnonymousMember( save_prev ) ) {
            if( SymIsAnonymousMember( sym ) ) {
                if( save_prev->u.offset >= sym->u.offset ) {
                    // we've already initialized the anonymous union
                    // so skip the rest
                    *prev = save_prev;
                    return( FALSE );
                }
            }
        }
    }
    return( TRUE );
}


boolean SymIsThunk(             // DETERMINE IF FUNCTION IS THUNK
    SYMBOL func )               // - function which is possible thunk
{
    boolean retn;               // - TRUE ==> is thunk
    char* name;                 // - function name

    if( func != NULL ) {
        if( ( func->name != NULL )
          &&( func->name->name != NULL ) ) {
            name = func->name->name;
            if( ( name == CppSpecialName( SPECIAL_DTOR_THUNK     ) )
              ||( name == CppSpecialName( SPECIAL_CTOR_THUNK     ) )
              ||( name == CppSpecialName( SPECIAL_OP_DEL_THUNK   ) )
              ||( name == CppSpecialName( SPECIAL_OP_DELAR_THUNK ) )
              ||( name == CppSpecialName( SPECIAL_COPY_THUNK     ) )
              ) {
                retn = TRUE;
            } else {
                retn = FALSE;
            }
        } else {
            retn = FALSE;
        }
    } else {
        retn = FALSE;
    }
    // we should be able to use SF_ADDR_THUNK; so we test this...
    DbgAssert( func == NULL || ((func->flag & SF_ADDR_THUNK)!=0) == (retn!=0) );
    return retn;
}


boolean SymIsVft(               // TEST IF SYMBOL IS VFT SYMBOL
    SYMBOL sym )                // - symbol
{
    boolean retn;               // - TRUE ==> is VFT symbol
    SYMBOL_NAME sname;          // - symbol-name entry
    char* name;                 // - name for symbol

    sname = sym->name;
    if( sname == NULL ) {
        retn = FALSE;
    } else {
        name = sname->name;
        if( name == NULL ) {
            retn = FALSE;
        } else {
            retn = IsVftName( name );
        }
    }
    return retn;
}


SYMBOL SymMarkRefed(            // MARK SYMBOL AS REFERENCED
    SYMBOL sym )                // - the symbol
{
    SYMBOL base;                // - original symbol, minus default args
    SYMBOL orig;                // - original symbol, when thunk

    base = sym;
    if( SymIsFunction( base ) ) {
        while( base->id == SC_DEFAULT ) {
            base = base->thread;
        }
        if( SymIsThunk( base ) ) {
            orig = base->u.thunk_calls;
            orig->flag |= SF_REFERENCED;
        }
    }
    base->flag |= SF_REFERENCED;
    // callers depend on original symbol coming back
    return( sym );
}


SYMBOL SymCreate(               // CREATE NEW SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name,                 // - symbol name
    SCOPE scope )               // - scope for insertion
{
    SYMBOL sym;                 // - created symbol
    SYMBOL check;               // - inserted symbol

    DbgVerify( id != SC_ACCESS, "SymCreate -- misused" );
    sym = symAllocate( type, id, flags );
    check = ScopeInsert( scope, sym, name );
    if( check != sym ) {
        check = NULL;
    }
    return check;
}


SYMBOL SymCreateAtLocn(         // CREATE NEW SYMBOL AT LOCATION
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name,                 // - symbol name
    SCOPE scope,                // - scope for insertion
    TOKEN_LOCN* locn )          // - location
{
    SYMBOL sym;                 // - created symbol
    SYMBOL check;               // - inserted symbol

    sym = symAllocate( type, id, flags );
    SymbolLocnDefine( locn, sym );
    check = ScopeInsert( scope, sym, name );
    // some call sites require that 'check'
    // be returned as non-NULL even if
    // 'check' is different than 'sym' (template.c)
    return( check );
}


SYMBOL SymCreateCurrScope(      // CREATE NEW CURR-SCOPE SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name )                // - symbol name
{
    return SymCreate( type, id, flags, name, GetCurrScope() );
}


SYMBOL SymCreateFileScope(      // CREATE NEW FILE-SCOPE SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name )                // - symbol name
{
    return SymCreate( type, id, flags, name, GetFileScope() );
}


SYMBOL SymCreateTempScope(      // CREATE NEW TEMP-SCOPE SYMBOL
    TYPE type,                  // - symbol type
    symbol_class id,            // - symbol class
    symbol_flag flags,          // - symbol flags
    char* name )                // - symbol name
{
    return SymCreate( type, id, flags, name, ScopeForTemps() );
}


SYMBOL SymDeriveThrowBits(      // DERIVE SF_.._LONGJUMP BITS FROM SOURCE
    SYMBOL tgt,                 // - target symbol
    SYMBOL src )                // - source symbol
{
    symbol_flag src_flag        // - source flags
        = SymThrowFlags( src );

#ifndef NDEBUG
    if( src_flag & SF_LONGJUMP ) {
        DbgVerify( !( tgt->flag & SF_NO_LONGJUMP )
                 , "SymDeriveThrowBits -- target has SF_NO_LONGJUMP" );
    } else if( src_flag & SF_NO_LONGJUMP ) {
        DbgVerify( !( tgt->flag & SF_LONGJUMP )
                 , "SymDeriveThrowBits -- target has SF_LONGJUMP" );
    }
#endif

    tgt->flag |= src_flag;
    return tgt;
}


symbol_flag SymThrowFlags(      // GET SYMBOL'S THROW BITS
    SYMBOL sym )                // - the symbol
{
    symbol_flag flags;          // - symbol's flags

    flags = ( sym->flag & SF_LONGJUMP )
          | ( sym->flag & SF_NO_LONGJUMP );
    DbgVerify( flags != ( SF_LONGJUMP | SF_NO_LONGJUMP )
             , "SymThrowFlags -- both throw bits computed" );
    return flags;
}


void SymSetNvReferenced(        // SET REFERENCED IF NOT VIRTUAL FUNCTION
    SYMBOL sym )                // - the symbol
{
    if( ! SymIsVirtual( sym ) ) {
        SymMarkRefed( sym );
    }
}


SYMBOL SymReloc                 // RELOCATE SYMBOL
    ( SYMBOL src,               // - source symbol
      RELOC_LIST *reloc_list )  // - reloc list
{
    RELOC_LIST *curr;

    RingIterBeg( reloc_list, curr ) {
        if( src == curr->orig ) {
            return curr->dest;
        }
    } RingIterEnd( curr )
    return src;
}


SYMBOL SymForClass              // GET SYMBOL FOR CLASS IF IT EXISTS
    ( TYPE cltype )             // - a type
{
    SYMBOL retn;                // - NULL or TYPEDEF for class

    cltype = StructType( cltype );
    if( NULL == cltype ) {
        retn = NULL;
    } else {
        char* name = cltype->u.c.info->name;
        if( NULL == name ) {
            retn = NULL;
        } else {
            SEARCH_RESULT* result;
            result = ScopeFindNaked( cltype->u.c.scope->enclosing, name );
            if( NULL == result ) {
                retn = NULL;
            } else {
                retn = result->sym_name->name_type;
                if( ! SymIsTypedef( retn ) ) {
                    retn = NULL;
                }
                ScopeFreeResult( result );
            }
        }
    }
    return retn;
}


TOKEN_LOCN* LocnForClass        // GET DEFINITION LOCATION, IF POSSIBLE
    ( TYPE cltype )             // - a type
{
    TOKEN_LOCN* retn;           // - return location
    SYMBOL sym;                 // - typedef for class

    sym = SymForClass( cltype );
    if( NULL == sym ) {
        retn = NULL;
    } else {
        retn = &sym->locn->tl;
    }
    return retn;
}

SYMBOL SymMakeAlias(            // DECLARE AN ALIAS IN CURRSCOPE
    SYMBOL aliasee,             // - sym to be aliased
    TOKEN_LOCN *locn )          // - location for symbol
{
    SYMBOL sym;
    SYMBOL check;

    aliasee = SymDeAlias( aliasee );
    sym = symAllocate( aliasee->sym_type, aliasee->id, aliasee->flag | SF_REFERENCED );
    sym->flag |= SF_ALIAS;
    sym->u.alias = aliasee;
    SymbolLocnDefine( locn, sym );
    check = ScopeInsert( GetCurrScope(), sym, aliasee->name->name );
    if( check != sym ) {
        check = NULL;
    }
    return( check );
}


#ifdef OPT_BR
boolean SymIsBrowsable          // TEST IF SYMBOL CAN BE BROWSED
    ( SYMBOL sym )              // - the symbol
{
    SCOPE scope;
    symGetScope( sym, scope );
    return ! SymIsTemporary( sym )
        && ! SymIsThunk( sym )
        && ! SymIsVft( sym )
        && GetInternalScope() != scope
        ;
}
#endif


SYMBOL SymBindConstant              // BIND A CONSTANT TO A SYMBOL
    ( SYMBOL sym                    // - the symbol
    , signed_64 con )               // - the constant
{
    if( NULL != sym ) {
        if( NULL == Integral64Type( sym->sym_type ) ) {
            sym->u.sval = con.u._32[ I64LO32 ];
        } else {
            sym->u.pval = ConPoolInt64Add( con );
            sym->flag |= SF_CONSTANT_INT64;
        }
    }
    return sym;
}


SYMBOL SymConstantValue             // GET CONSTANT VALUE FOR SYMBOL
    ( SYMBOL sym                    // - the symbol
    , INT_CONSTANT* pval )          // - addr[ value ]
{
    pval->type = sym->sym_type;
    if( sym->flag & SF_CONSTANT_INT64 ) {
        pval->value = sym->u.pval->int64_constant;
    } else if( sym->flag & SF_ENUM_UINT ) {
        Int64FromU32( sym->u.sval, &pval->value );
    } else {
        Int64From32( sym->sym_type, sym->u.sval, &pval->value );
    }
    return sym;
}

SYMBOL SymDefArgBase(               // GET DEFARG BASE SYMBOL
    SYMBOL sym )                    // - the symbol
{
    sym = sym;
    while( SymIsDefArg( sym ) ) {
        sym = sym->thread;
    }

    return sym;
}
