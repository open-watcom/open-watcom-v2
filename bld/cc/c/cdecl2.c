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


#include "cvars.h"
#include "toggle.h"
#include "pragdefn.h"
#include "cgswitch.h"


TYPEPTR *MakeParmList( struct parm_list *, int, int );
struct parm_list *NewParm( TYPEPTR, struct parm_list * );
void    InvDecl( void );
TYPEPTR DeclPart2(TYPEPTR);
TYPEPTR DeclPart3(TYPEPTR);
static void AbsDecl( SYMPTR sym, type_modifiers mod, TYPEPTR typ );

static int ThreadSeg;




void Chk_Struct_Union_Enum( TYPEPTR typ )
{
    while( typ->decl_type == TYPE_TYPEDEF ) {           /* 24-nov-94 */
        if( !(typ->type_flags & TF2_DUMMY_TYPEDEF) ) break;
        typ = typ->object;
    }
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
        if( typ->u.tag->name[0] == '\0' ) {
            InvDecl();
        }
        break;
    case TYPE_ENUM:
        break;
    default:
        InvDecl();
    }
}


local void FlushBadCode()
{
    int         count;

    CErr1( ERR_STMT_MUST_BE_INSIDE_FUNCTION );
    count = 0;
    for(;;) {
        NextToken();
        if( CurToken == T_EOF ) return;
        if( CurToken == T_LEFT_BRACE ) ++count;
        if( CurToken == T_RIGHT_BRACE ) {
            if( count == 0 ) break;
            --count;
        }
    }
    NextToken();
}


static int SCSpecifier()
{
    stg_classes stg_class;

    stg_class = SC_NULL;        /* assume no storage class specified */
    if( TokenClass[ CurToken ] == TC_STG_CLASS ) {
        switch( CurToken ) {
        case T_EXTERN:  stg_class = SC_EXTERN;  break;
        case T_STATIC:  stg_class = SC_STATIC;  break;
        case T___INLINE:stg_class = SC_STATIC;  break;
        case T_TYPEDEF: stg_class = SC_TYPEDEF; break;
        case T_AUTO:    stg_class = SC_AUTO;    break;
        case T_REGISTER:stg_class = SC_REGISTER;break;
        }
        NextToken();
    }
    return( stg_class );
}


void InvDecl()
{
    CErr1( ERR_INVALID_DECLARATOR );
}

#define FUNC_MASK     (~(FLAG_INLINE | FLAG_LOADDS | FLAG_EXPORT | FLAG_LANGUAGES))
local void CmpFuncDecls( SYMPTR new_sym, SYMPTR old_sym )
{
    TYPEPTR     type_new, type_old;

    if( (new_sym->attrib & FUNC_MASK) != (old_sym->attrib & FUNC_MASK)){
        CErr2p( ERR_MODIFIERS_DISAGREE, new_sym->name );
    }

/*      check for conflicting information */
/*      skip over TYPEDEF's   29-aug-89   */
    type_new = new_sym->sym_type;
    while( type_new->decl_type == TYPE_TYPEDEF ) type_new = type_new->object;
    type_old = old_sym->sym_type;
    while( type_old->decl_type == TYPE_TYPEDEF ) type_old = type_old->object;
    //ChkFuncRetType( ret_new, ret_old, new_sym );
    if( ! IdenticalType( type_new->object, type_old->object ) ) {
        TYPEPTR     ret_new, ret_old;

        ret_new = type_new->object;                    /* save return types */
        ret_old = type_old->object;
        // skip over typedef's 18-may-95
        while( ret_new->decl_type == TYPE_TYPEDEF ) ret_new = ret_new->object;
        while( ret_old->decl_type == TYPE_TYPEDEF ) ret_old = ret_old->object;
        /* don't reorder this expression */
        if( old_sym->stg_class != SC_FORWARD ){
            CErr2p( ERR_INCONSISTENT_TYPE, new_sym->name );
        }else if( ret_new->decl_type != TYPE_VOID
               || (old_sym->flags & SYM_TYPE_GIVEN ) ){ //return value used in forward
            CErr2p( ERR_INCONSISTENT_TYPE, new_sym->name );
        }
    }
    if( type_new->u.parms != type_old->u.parms ){
        if( type_new->u.parms == NULL ){
            ChkParmPromotion( type_old->u.parms );
        }else if( type_old->u.parms == NULL ){
            ChkParmPromotion( type_new->u.parms );
        }else{
            ChkParmList( type_new->u.parms, type_old->u.parms );
        }
    }
}

local SYM_HANDLE FuncDecl( SYMPTR sym, stg_classes stg_class )
{
    SYM_HANDLE  sym_handle;
    SYM_HANDLE  old_sym_handle;
    auto SYM_ENTRY old_sym;
    auto struct enum_info ei;
    auto SYM_ENTRY sym_typedef;
    TYPEPTR     old_typ;
    SYM_NAMEPTR sym_name;
    char        *name;
    int         sym_len;

    PrevProtoType = NULL;                               /* 12-may-91 */
    sym->rent = FALSE;   //Assume not override aka re-entrant
    if( CompFlags.rent && (sym->declspec == DECLSPEC_DLLIMPORT) ){
        sym->rent = TRUE;
    }
    if( stg_class == SC_REGISTER  ||
        stg_class == SC_AUTO  ||
        stg_class == SC_TYPEDEF ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_FUNC );
            stg_class = SC_NULL;
    }
    old_sym_handle = SymLook( sym->info.hash_value, sym->name );
    if( old_sym_handle != 0 ) {
        SymGet( &old_sym, old_sym_handle );
    }
    if( old_sym_handle == 0 ) {
        EnumLookup( sym->info.hash_value, sym->name, &ei );/* 22-dec-88 */
        if( ei.level >= 0 ) {       /* if enum was found */
            CErr2p( ERR_SYM_ALREADY_DEFINED, sym->name );
        }
        sym_handle = SymAddL0( sym->info.hash_value, sym );
    } else if( (old_sym.flags & SYM_FUNCTION) == 0 ) {
        CErr2p( ERR_SYM_ALREADY_DEFINED_AS_VAR, sym->name );
//02-jun-89 sym_handle = old_sym_handle;                /* 05-apr-89 */
        sym_handle = SymAddL0( sym->info.hash_value, sym );/* 02-jun-89 */
    } else {
        CmpFuncDecls( sym, &old_sym );
        PrevProtoType = old_sym.sym_type;               /* 12-may-91 */
        if( (old_sym.flags & SYM_DEFINED) == 0 ) {
            if( sym->sym_type->u.parms != NULL  ||      /* 11-jul-89 */
            ( CurToken != T_COMMA &&                    /* 18-jul-89 */
              CurToken != T_SEMI_COLON ) ) {
                old_typ = old_sym.sym_type;
                if( old_typ->decl_type == TYPE_TYPEDEF &&
                    old_typ->object->decl_type == TYPE_FUNCTION ) {
                    SymGet( &sym_typedef, old_typ->u.typedefn );
                    sym_name = SymName( &sym_typedef,
                                        old_typ->u.typedefn );
                    sym_len = far_strlen_plus1( sym_name );
                    name = CMemAlloc( sym_len );
                    far_memcpy( name, sym_name, sym_len );
                    XferPragInfo( name, sym->name );
                    CMemFree( name );
                }
                old_sym.sym_type = sym->sym_type;
                old_sym.d.defn_line = sym->d.defn_line;
                old_sym.defn_file_index = sym->defn_file_index;
            }
        }
        if( (sym->attrib & FLAG_LANGUAGES) !=( old_sym.attrib & FLAG_LANGUAGES)){
            // just inherit old lang flags
            // if new != 0 then it's possible someone saw a different prototype
            if( (sym->attrib & FLAG_LANGUAGES) != 0 ){
                 CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
            }
        }
        if((sym->attrib & FLAG_INLINE) != (old_sym.attrib & FLAG_INLINE) ){
            old_sym.attrib |= FLAG_INLINE; //either is inline
        }
        if( sym->declspec != old_sym.declspec ){
            switch( sym->declspec ){
            case DECLSPEC_DLLIMPORT:
            case DECLSPEC_THREAD:
                CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                break;
            case DECLSPEC_DLLEXPORT:
                if( old_sym.declspec == DECLSPEC_DLLIMPORT ){
                    old_sym.declspec = DECLSPEC_DLLEXPORT;
                }else{
                    CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                }
                break;
            }
        }
        CMemFree( sym->name );
        if( stg_class == SC_NULL && old_sym.stg_class != SC_FORWARD ){   /* 05-jul-89 */
            stg_class = old_sym.stg_class;
        }
        memcpy( sym, &old_sym, sizeof( SYM_ENTRY ) );
        sym_handle = old_sym_handle;
    }
    sym->flags |= SYM_FUNCTION;
    if( (sym->flags & SYM_DEFINED) == 0 ) {
        if( sym->attrib & FLAG_INLINE ){
            sym->flags |= SYM_IGNORE_UNREFERENCE;
            stg_class = SC_STATIC;
        }else if( stg_class == SC_NULL ){
            stg_class = SC_EXTERN;  /* SC_FORWARD; */
        }
        sym->stg_class = stg_class;
    }
    return( sym_handle );
}

TYPEPTR SkipDummyTypedef( TYPEPTR typ )                 /* 25-nov-94 */
{
    while( typ->decl_type == TYPE_TYPEDEF &&
           (typ->type_flags & TF2_DUMMY_TYPEDEF) ) {
        typ = typ->object;
    }
    return( typ );
}

#define QUAL_FLAGS (FLAG_CONST|FLAG_VOLATILE|FLAG_UNALIGNED)
#define ATTRIB_MASK     (~(FLAG_INLINE | FLAG_LOADDS | FLAG_EXPORT | FLAG_LANGUAGES))
local SYM_HANDLE VarDecl( SYMPTR sym, stg_classes stg_class )
{
    int         which;
    TYPEPTR     typ;
    SYM_HANDLE  sym_handle;
    SYM_HANDLE  old_sym_handle;
    SYM_ENTRY   old_sym;
    SYM_ENTRY   sym2;

    if( CompFlags.rent ){
        sym->rent = TRUE; //Assume instance data
    }else{
        sym->rent = FALSE;//Assume non instance data
    }
    if( sym->naked ){          /* 25-jul-95 */
        CErr1( ERR_INVALID_DECLSPEC );
    }
    if( SymLevel == 0 ) {
        if( stg_class == SC_AUTO  ||  stg_class == SC_REGISTER ) {
            CErr1( ERR_INV_STG_CLASS_FOR_GLOBAL );
            stg_class = SC_STATIC;
        } else if( stg_class == SC_NULL ) {
            CompFlags.external_defn_found = 1;
        }
        if( sym->declspec == DECLSPEC_THREAD ) {          /* 25-jul-95 */
            if( !CompFlags.thread_data_present ){
                ThreadSeg = DefThreadSeg();
                CompFlags.thread_data_present = 1;
            }
            sym->u.var.segment = ThreadSeg;
        }
    } else {
        if( stg_class == SC_NULL ) {
            stg_class = SC_AUTO;
        }
        if( stg_class == SC_AUTO  ||  stg_class == SC_REGISTER ) {
            if( sym->attrib & FLAG_LANGUAGES ){
                CErr1( ERR_INVALID_DECLARATOR );
            }
            if( sym->declspec != DECLSPEC_NONE ) {          /* 25-jul-95 */
                CErr1( ERR_INVALID_DECLSPEC );
            }
        }
    }
    if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
        sym->flags |= SYM_IGNORE_UNREFERENCE;   /* 25-apr-91 */
    }
    old_sym_handle = SymLook( sym->info.hash_value, sym->name );
    if( old_sym_handle != 0 ) {                         /* 28-feb-94 */
        SymGet( &old_sym, old_sym_handle );
        if( old_sym.level == SymLevel ) {
            if( old_sym.stg_class == SC_EXTERN  &&  stg_class == SC_EXTERN ) {
                if( ! IdenticalType( old_sym.sym_type, sym->sym_type ) ) {
                    CErr2p( ERR_TYPE_DOES_NOT_AGREE, sym->name );
                }
            }
        }
    }
    if( stg_class == SC_EXTERN ) {              /* 27-oct-88 */
        old_sym_handle = Sym0Look( sym->info.hash_value, sym->name );
    }
    if( old_sym_handle != 0 ) {
        SymGet( &old_sym, old_sym_handle );
        if( old_sym.level == SymLevel           /* 28-mar-88 */
        ||      stg_class == SC_EXTERN ) {              /* 12-dec-88 */
            if( (sym->attrib & ATTRIB_MASK) !=
                (old_sym.attrib & ATTRIB_MASK) ) {
                 CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
            }
            if( (sym->attrib & FLAG_LANGUAGES) !=( old_sym.attrib & FLAG_LANGUAGES)){
                // just inherit old lang flags
                // if new != 0 then it's possible someone saw a different prototype
                if( (sym->attrib & FLAG_LANGUAGES) != 0 ){
                     CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                }
            }
            if( sym->declspec != old_sym.declspec ){
                switch( sym->declspec ){
                case DECLSPEC_DLLIMPORT:
                case DECLSPEC_THREAD:
                    CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                    break;
                case DECLSPEC_DLLEXPORT:
                    if( old_sym.declspec == DECLSPEC_DLLIMPORT ){
                        old_sym.declspec = DECLSPEC_DLLEXPORT;
                    }else{
                        CErr2p( ERR_MODIFIERS_DISAGREE, sym->name );
                    }
                    break;
                }
            }
        }
    }
    if( ( old_sym_handle != 0 )  &&
        ( stg_class == SC_NULL || stg_class == SC_EXTERN ||
          (stg_class == SC_STATIC  &&  SymLevel == 0 ) ) ) {

/*              make sure sym->sym_type same type as old_sym->sym_type */

        which = VerifyType( sym->sym_type, old_sym.sym_type, sym );
        if( which == 0 && old_sym.level == SymLevel ) { /* 06-jul-88 AFS */
            /* new symbol's type supersedes old type */
            old_sym.sym_type = sym->sym_type;
            if( (old_sym.flags & SYM_FUNCTION) ) {
                old_sym = *sym;  // ditch old sym
            }
            SymReplace( &old_sym, old_sym_handle );
        }
        if( stg_class == SC_EXTERN  &&  SymLevel != 0 ) goto new_var;
        CMemFree( sym->name );
        memcpy( sym, &old_sym, sizeof( SYM_ENTRY ) );
        sym_handle = old_sym_handle;
        if( stg_class == SC_NULL  ||  stg_class == SC_STATIC ) {
            if( sym->stg_class == SC_EXTERN ) {
                sym->stg_class = /*SC_NULL*/ stg_class; /* 03-oct-88 */
            } else if( sym->stg_class == SC_STATIC ) {
                if( stg_class == SC_NULL ) {
                    CErrSymName( ERR_STG_CLASS_DISAGREES, sym, sym_handle );
                }
            }
        }
    } else {
        if( stg_class == SC_EXTERN  &&  SymLevel != 0 ) {
            ; /* do nothing  29-jan-93 */
        } else {
            VfyNewSym( sym->info.hash_value, sym->name );
        }
new_var:
        old_sym_handle = 0;
        sym->flags |= SYM_DEFINED;
        typ = SkipDummyTypedef( sym->sym_type );        /* 25-nov-94 */
        if( typ->decl_type == TYPE_TYPEDEF ) {          /* 12-mar-92 */
            SymGet( &sym2, typ->u.typedefn );
            if( sym->u.var.segment == 0  &&  sym2.u.var.segment != 0 ) {
                sym->u.var.segment = sym2.u.var.segment;
            }
            while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        }
        if( typ->decl_type == TYPE_VOID ) {
            CErr2p( ERR_VAR_CANT_BE_VOID, sym->name );
            sym->sym_type = TypeDefault();
        }
        sym->stg_class = stg_class;
        sym_handle = SymAdd( sym->info.hash_value, sym );
    }
    if( sym->u.var.segment == 0  &&     /* 22-oct-92 */
    (stg_class == SC_STATIC ||
     stg_class == SC_NULL   ||
     stg_class == SC_EXTERN) ) {
        if( DefDataSegment != 0 ) {
            sym->u.var.segment = DefDataSegment;
            SymReplace( sym, sym_handle );
        }
    }
    if( CurToken == T_EQUAL ) {
        if( stg_class == SC_EXTERN ) {
            stg_class = SC_STATIC;
            if( SymLevel == 0 ) {
                CompFlags.external_defn_found = 1;
                stg_class = SC_NULL;
            } else {
                CErr1( ERR_CANT_INITIALIZE_EXTERN_VAR );
            }
        }
        sym->stg_class = stg_class;
        NextToken();
        VarDeclEquals( sym, sym_handle );
        sym->flags |=  SYM_ASSIGNED;
    }
    SymReplace( sym, sym_handle );                      /* 06-jul-88 */
    if( old_sym_handle != 0 ) sym_handle = 0;
    return( sym_handle );
}

SYM_HANDLE InitDeclarator( SYMPTR sym,
                           decl_info const * const info,
                           decl_state state )
{
    SYM_HANDLE  sym_handle;
    SYM_HANDLE  old_sym_handle;
    type_modifiers flags;
    TYPEPTR     typ;
    TYPEPTR     otyp;
    auto SYM_ENTRY old_sym;

    if( ParmList != NULL ) {
        FreeParmList();
    }
    memset( sym, 0, sizeof( SYM_ENTRY ) );              /* 02-apr-91 */
    sym->name = "";
    flags = TypeQualifier();                            /* 08-nov-94 */
    if( flags & info->mod ){
       CErr1( ERR_INV_TYPE );
    }
    flags |= info->mod;
    if( info->decl == DECLSPEC_DLLEXPORT ){
        flags |= FLAG_EXPORT; //need to get rid of this
    }
    Declarator( sym, flags, info->typ, state );
    if( sym->name[0] == '\0' ) {
        InvDecl();
        return( 0 );
    }
    typ = sym->sym_type;
    /* skip over typedef's  29-aug-89 */
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( info->stg == SC_TYPEDEF ) {
        if( CompFlags.extensions_enabled ) {            /* 24-mar-91 */
            old_sym_handle = SymLook( sym->info.hash_value, sym->name );
            if( old_sym_handle != 0 ) {
                SymGet( &old_sym, old_sym_handle );
                otyp = old_sym.sym_type;        /* skip typedefs 25-sep-92 */
                while( otyp->decl_type == TYPE_TYPEDEF ) otyp = otyp->object;
                if( old_sym.stg_class == SC_TYPEDEF     &&
                    old_sym.level == SymLevel   &&
                    IdenticalType( typ, otyp ) ) {
                    return( 0 );        /* indicate already in symbol tab */
                }
            }
        }
        VfyNewSym( sym->info.hash_value, sym->name );
        sym->stg_class = info->stg;
        sym_handle = SymAdd( sym->info.hash_value, sym );
    } else {
        sym->declspec = info->decl;
        sym->naked = info->naked;
        if( sym->declspec == DECLSPEC_DLLEXPORT ){ //sync up flags
            sym->attrib |= FLAG_EXPORT; //need to get rid of this
        }else if( sym->attrib & FLAG_EXPORT ){
            if( sym->declspec == DECLSPEC_NONE ){
                sym->declspec = DECLSPEC_DLLEXPORT;
            }else if( sym->declspec  != DECLSPEC_DLLEXPORT ){
                 CErr1( ERR_INVALID_DECLSPEC );
            }
        }
        if( typ->decl_type == TYPE_FUNCTION ) {
            sym_handle = FuncDecl( sym, info->stg );
        } else {
            sym_handle = VarDecl( sym, info->stg );
        }
    }
    return( sym_handle );
}

int DeclList( SYM_HANDLE *sym_head )
{
    decl_state          state;
    SYM_HANDLE          sym_handle;
    SYM_HANDLE          prevsym_handle;
    unsigned            line_num;
    auto SYM_ENTRY      sym;
    auto SYM_ENTRY      prevsym;
    decl_info           info;

    ParmList = NULL;
    prevsym_handle = 0;
    *sym_head = 0;
    for(;;) {
        for(;;) {
            for(;;) {
                while( CurToken == T_SEMI_COLON ) {
                    if( SymLevel == 0 ) {
                        if( !CompFlags.extensions_enabled ) {  /* 28-nov-94 */
                            CErr2p( ERR_EXPECTING_DECL_BUT_FOUND, ";" );
                        }
                    }
                    NextToken();
                }
                if( CurToken == T_EOF ) return( 0 );
                line_num = TokenLine;
                FullDeclSpecifier( &info );
                if( info.stg != SC_NULL  ||  info.typ != NULL )     break;
                if( SymLevel != 0 )  return( 0 );
                break;
            }
            state = DECL_STATE_NONE;
            if( info.typ == NULL ) {
                state |= DECL_STATE_NOTYPE;
                info.typ = TypeDefault();
            }
            if( info.stg == SC_NULL  && (state & DECL_STATE_NOTYPE) ) {
                switch( CurToken ) {
                case T_ID:
                case T_LEFT_PAREN:
                case T___EXPORT:
                case T___LOADDS:
                case T___NEAR:
                case T___FAR:
                case T___HUGE:
                case T___CDECL:
                case T___PASCAL:
                case T___FORTRAN:
                case T__SYSCALL:                        /* 04-jul-91 */
                case T___STDCALL:                       /* 03-feb-95 */
                case T___FASTCALL:
                case T___INTERRUPT:
                case T___SAVEREGS:
                case T_TIMES:                           /* 30-nov-94 */
                    break;
                case T_IF:
                case T_FOR:
                case T_WHILE:
                case T_DO:
                case T_SWITCH:
                case T_BREAK:
                case T_CONTINUE:
                case T_CASE:
                case T_DEFAULT:
                case T_ELSE:
                case T_GOTO:
                case T_RETURN:
                    FlushBadCode();
                    continue;
                default:
                    CErr2p( ERR_EXPECTING_DECL_BUT_FOUND, Tokens[CurToken] );
                    NextToken();
                    break;
                }
            }
            break;
        }
        if( CurToken != T_SEMI_COLON ) {
            if( info.decl == DECLSPEC_DLLIMPORT ) {
                if(!CompFlags.rent ){
                    if( info.stg == SC_NULL ) info.stg = SC_EXTERN;
                }
            }
            for( ;; ) {
                sym_handle = InitDeclarator( &sym, &info, state );
                /* NULL is returned if sym already exists in symbol table */
                if( sym_handle != 0 ) {
                    sym.handle = 0;
                    if( (sym.flags & SYM_FUNCTION) ) {
                        if( !(state & DECL_STATE_NOTYPE ) ) {
                            sym.flags |= SYM_TYPE_GIVEN;
                        }
                    } else if( SymLevel > 0 ) { /* variable */
                        if( prevsym_handle != 0 ) {
                            SymGet( &prevsym, prevsym_handle );
                            prevsym.handle = sym_handle;
                            SymReplace( &prevsym, prevsym_handle );
                        }
                        if( *sym_head == 0 ) {
                            *sym_head = sym_handle;
                        }
                        prevsym_handle = sym_handle;
                    }
                    SymReplace( &sym, sym_handle );
                }
                /* case "int x *p" ==> missing ',' msg already given */
                if( CurToken != T_TIMES ) {
                    if( CurToken != T_COMMA ) break;
                    NextToken();
                }
            }
/*              the following is illegal:
                    typedef double math(double);
                    math sin { ; }
            That's the reason for the check
                    "typ->decl_type != TYPE_FUNCTION"
*/
            if( SymLevel == 0  &&  CurToken != T_SEMI_COLON
                               &&  sym_handle != 0 ) {
                if( sym.sym_type->decl_type == TYPE_FUNCTION
                    &&  sym.sym_type != info.typ ) { /* 21-mar-89 */
                    CurFuncHandle = sym_handle;
                    CurFunc = &CurFuncSym;
                    memcpy( CurFunc, &sym, sizeof( SYM_ENTRY ) );
                    return( 1 );        /* indicate this is a function defn */
                }
            }
            MustRecog( T_SEMI_COLON );
        } else {
            Chk_Struct_Union_Enum( info.typ );
            NextToken();                /* skip over ';' */
        }
    }
// can't get here!      return( 0 );
}


TYPEPTR TypeName()
{
    TYPEPTR     typ;
    decl_info   info;
    SYM_ENTRY   abs_sym;

    TypeSpecifier( &info );
    typ = info.typ;
    if( typ != NULL ) {
         memset( &abs_sym, 0, sizeof( SYM_ENTRY ) );
         abs_sym.name = "";
         AbsDecl( &abs_sym, info.mod, info.typ  );
         typ = abs_sym.sym_type;
    }
    return( typ );
}


local type_modifiers GetModifiers( void )
{
    type_modifiers        modifier;
    int                   hash;

    static type_modifiers const ModifierFlags[] = {
            0,
            FLAG_NEAR,          // TC_NEAR
            FLAG_FAR,           // TC_FAR
#if _CPU == 8086
            FLAG_HUGE,          // TC_HUGE
#else
            FLAG_FAR,            // TC_HUGE      // close
#endif

#if _CPU == 8086
            FLAG_FAR,           // TC_FAR16      // avoid giving error
#else
            FLAG_FAR16,         // TC_FAR16
#endif
            FLAG_INTERRUPT,     // TC_INTERRUPT
            LANG_CDECL,         // TC_CDECL
            LANG_PASCAL,        // TC_PASCAL
            LANG_FORTRAN,       // TC_FORTRAN
            LANG_SYSCALL,       // TC_SYSCALL           /* 04-jul-91 */
            LANG_STDCALL,       // TC_STDCALL
            LANG_FASTCALL,      // TC_FASTCALL
            LANG_OPTLINK,       // TC_OPTLINK
            FLAG_EXPORT,        // TC_EXPORT
            FLAG_LOADDS,        // TC_LOADDS
            FLAG_SAVEREGS,      // TC_SAVEREGS
    };

    modifier = 0;
    for(;;) {
        hash = TokenClass[ CurToken ];
        if( hash >= TC_MODIFIER ) break;        /* 04-jul-90, c/=/>=/ */
        modifier |= ModifierFlags[ hash ];
        NextToken();
    }
    return( modifier );
}

struct mod_info {
    int             segment;
    type_modifiers  modifier;  // const, vol flags
    BASED_KIND       based_kind;
    SYM_HANDLE       based_sym;
};

local TYPEPTR Pointer( TYPEPTR ptr_typ, struct mod_info *info )
{
    type_modifiers  flags;
    SYM_HANDLE      sym_handle;
    auto SYM_ENTRY  sym;

    sym_handle = 0;
    if( ptr_typ != NULL  &&  ptr_typ->decl_type == TYPE_TYPEDEF ) {
     // get segment from typedef TODO should be done sooner
        TYPEPTR typ;
        SYMPTR  symp;

        typ = SkipDummyTypedef( ptr_typ );              /* 25-nov-94 */
        if( typ->decl_type == TYPE_TYPEDEF ) {          /* 15-mar-92 */
            symp = SymGetPtr( typ->u.typedefn );
            if( info->modifier & FLAG_BASED ) {
                info->segment = symp->u.var.segment;
                sym_handle = SegSymHandle( info->segment );
            }
        }
    }
    for(;;) {
        flags = GetModifiers();   // NEAR FAR CDECL stuff
        if( flags & info->modifier ){
            CWarn1(WARN_REPEATED_MODIFIER, ERR_REPEATED_MODIFIER );
        }
        info->modifier |= flags;
        if( CurToken == T___BASED ) {
            bool use_seg;
            use_seg = FALSE;
            NextToken();
            MustRecog( T_LEFT_PAREN );
            info->based_kind = BASED_NONE;
            if( CurToken == T_LEFT_PAREN ) {
                NextToken();
                MustRecog( T___SEGMENT );
                MustRecog( T_RIGHT_PAREN );
                use_seg = TRUE;
            }
            switch( CurToken ) {
            case T_ID:          /* __based(variable) */
                sym_handle = SymLook( HashValue, Buffer );
                if( sym_handle == 0 ) {         /* 10-jan-92 */
                    SymCreate( &sym, Buffer );
                    sym.stg_class = SC_EXTERN; /* indicate extern decl */
                    CErr2p( ERR_UNDECLARED_SYM, Buffer );
                    sym.sym_type = GetType( TYPE_INT );
                    sym_handle = SymAdd( HashValue, &sym );
                }else{
                    TYPEPTR     typ;

                    SymGet( &sym, sym_handle );
                    typ = sym.sym_type;
                    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
                    if( use_seg ){
                        info->based_kind = BASED_VARSEG;
                        if( typ->decl_type != TYPE_POINTER ){
                           CErr1( ERR_SYM_MUST_BE_TYPE_SEGMENT );
                           info->based_kind = BASED_NONE;
                       }
                    }else if( typ->decl_type == TYPE_POINTER ){
                        info->based_kind = BASED_VAR;
                    }else if( (sym.attrib & FLAG_SEGMENT) ) {
                        info->based_kind = BASED_SEGVAR;
                    }else{
                        CErr1( ERR_SYM_MUST_BE_TYPE_SEGMENT );
                        info->based_kind = BASED_NONE;
                    }
                }
                if( ! (sym.flags & SYM_REFERENCED) ) {  /* 10-jan-92 */
                    sym.flags |= SYM_REFERENCED;
                    SymReplace( &sym, sym_handle );
                }
                NextToken();
                break;
            case T_VOID:                /* __based(void)  */
                if( use_seg ){
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                info->based_kind = BASED_VOID;
                NextToken();
                break;
            case T___SEGNAME:   /* __based(__segname("string")) */
                if( use_seg ){
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                info->based_kind = BASED_SEGNAME;
                NextToken();
                MustRecog( T_LEFT_PAREN );
                if( CurToken == T_STRING ) {
                    info->segment = AddSegName( Buffer, "", SEGTYPE_BASED);
                    sym_handle = SegSymHandle( info->segment );
                    SymGet( &sym, sym_handle );
                    sym.u.var.segment = info->segment;
                    SymReplace( &sym, sym_handle );
                    NextToken();
                } else {
                    CErr1( ERR_SEGMENT_NAME_REQUIRED );
                    info->based_kind = BASED_NONE;
                }
                MustRecog( T_RIGHT_PAREN );
                break;
            case T___SELF:              /* __based(__self) */
                info->based_kind = BASED_SELFSEG;
                NextToken();
                break;
            case T_AND:                /* __based((__segment) &var ) */
                if( !use_seg ){
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                NextToken();
                if( CurToken == T_ID ) {
                    sym_handle = SymLook( HashValue, Buffer );
                    if( sym_handle == 0 ) {         /* 10-jan-92 */
                        SymCreate( &sym, Buffer );
                        sym.stg_class = SC_EXTERN; /* indicate extern decl */
                        CErr2p( ERR_UNDECLARED_SYM, Buffer );
                        sym.sym_type = GetType( TYPE_INT );
                        sym_handle = SymAdd( HashValue, &sym );
                    } else {
                        SymGet( &sym, sym_handle );
                    }
                    if( ! (sym.flags & SYM_REFERENCED) ) {  /* 10-jan-92 */
                        sym.flags |= SYM_REFERENCED;
                        SymReplace( &sym, sym_handle );
                    }
                    info->based_kind = BASED_SEGNAME;
                    NextToken();
                }else{
                    CErr1( ERR_INVALID_BASED_DECLARATOR );
                }
                break;
            default:
                CErr1( ERR_INVALID_BASED_DECLARATOR );
                break;
            }
            MustRecog( T_RIGHT_PAREN );
            info->modifier &= ~(FLAG_NEAR|FLAG_FAR|FLAG_HUGE);
            info->modifier = FLAG_NEAR | FLAG_BASED;
        }
        if( CurToken == T_TIMES ) {
            NextToken();
#if _MACHINE == _PC
            if( CurToken == T__SEG16 ) {  // * seg16 binds with * cause IBM dorks
#if _CPU == 386                                         /* 15-nov-91 */
                info->modifier |= FLAG_FAR16;
#else
                info->modifier |= FLAG_FAR;
#endif
                NextToken();
            }
#endif
            flags = info->modifier & ~FLAG_EXPORT;
            ptr_typ = BPtrNode( ptr_typ, flags, info->segment,
                         sym_handle, info->based_kind );
            sym_handle = 0;
            info->segment = 0;  // start over
            info->modifier = TypeQualifier();  // .. * const
            info->based_kind = BASED_NONE;
        } else {
            break;
        }
    }
    return( ptr_typ );
}


local void ParseDeclPart2( TYPEPTR *typep, TYPEPTR typ )
{
    TYPEPTR     decl1;
    TYPEPTR     decl2;

    decl1 = *typep;
    if( decl1 != NULL ) {
        while( decl1->object != NULL ) {
            decl1 = decl1->object;
        }
    }
    decl2 = DeclPart2( typ );
    if( decl1 == NULL ) {
        *typep = decl2;
    } else {
        decl1->object = decl2;
        if( decl1->decl_type == TYPE_POINTER && decl2 != NULL ) {
            AddPtrTypeHash( decl1 );
        }
    }
}


static void AbsDecl( SYMPTR sym, type_modifiers mod, TYPEPTR typ )
{
    auto struct mod_info info;

    info.segment = 0;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = NULL;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( CurToken == T_RIGHT_PAREN ) {
            typ = FuncNode( typ, FLAG_NONE, NULL );
        } else {
            AbsDecl( sym, info.modifier, (TYPEPTR) NULL );
            info.modifier = FLAG_NONE;
            MustRecog( T_RIGHT_PAREN );
            ParseDeclPart2( &sym->sym_type, typ );
        }
    } else {
        sym->attrib = info.modifier;
        sym->u.var.segment = info.segment;              /* 01-dec-91 */
        typ = DeclPart2( typ );
        sym->sym_type = typ;
    }
}

void Declarator( SYMPTR sym, type_modifiers mod, TYPEPTR typ, decl_state state )
{
    TYPEPTR     *type_list;
    TYPEPTR     parm_type;
    auto struct mod_info info;

    info.segment = 0;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = NULL;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        if( state & DECL_STATE_ISPARM ) {
            parm_type = TypeName();                     /* 14-mar-91 */
        } else {
            parm_type = NULL;
        }
        if( parm_type != NULL  ||  CurToken == T_RIGHT_PAREN ) {
            type_list = NULL;
            if( parm_type != NULL ) {
                type_list = MakeParmList( NewParm(parm_type,NULL), 1, 0 );
            }
            typ = FuncNode( typ, FLAG_NONE, type_list );
            typ = PtrNode( typ, 0, SEG_DATA );
            MustRecog( T_RIGHT_PAREN );
        } else {
            if( (state & DECL_STATE_ISPARM) && TokenClass[ CurToken ] == TC_STG_CLASS ) {
                typ = DeclPart3( typ );
            } else {
                Declarator( sym, info.modifier, (TYPEPTR) NULL, state );
                info.modifier = FLAG_NONE;
                MustRecog( T_RIGHT_PAREN );
            }
        }
        ParseDeclPart2( &sym->sym_type, typ );
        typ = sym->sym_type;
    } else {
        if( CurToken == T_ID  ||  CurToken == T_SAVED_ID ) {
            for(;;) {
                if( CurToken == T_ID ) {
                    SymCreate( sym, Buffer );
                    sym->info.hash_value = HashValue;
                    NextToken();
                } else {
                    SymCreate( sym, SavedId );
                    sym->info.hash_value = SavedHash;
                    CurToken = LAToken;
                }
                if( CurToken != T_ID && CurToken != T_TIMES ) break;
                if( state & DECL_STATE_NOTYPE ) {
                    CErr2p( ERR_MISSING_DATA_TYPE, sym->name );
                    if( CurToken == T_TIMES ) { /* "garbage *p" */
                        typ = Pointer( typ, &info );
                    }
                } else if( CurToken == T_TIMES ) {
                    Expecting( "," );
                } else {
                    Expecting( ",' or ';" );
                }
                if( CurToken != T_ID ) break;
                CMemFree( sym->name );
            }
        } else {
            SymCreate( sym, "" );
        }
        sym->attrib = info.modifier;
        sym->u.var.segment = info.segment;              /* 01-dec-91 */
#if 0
        if( modifier & FLAG_INTERRUPT )  sym->flags |= SYM_INTERRUPT_FN;
#endif
        typ = DeclPart2( typ );
        sym->sym_type = typ;
    }
    if( typ != NULL ) {
        if( typ->decl_type == TYPE_FUNCTION ) {         /* 07-jun-94 */
            if( info.segment != 0 ) {           // __based( __segname("X"))
                SetFuncSegment( sym, info.segment );
            }
        }
    }
}


local FIELDPTR FieldCreate( char *name )
{
    FIELDPTR    field;

    field = (FIELDPTR) CPermAlloc( sizeof(FIELD_ENTRY) + strlen( name ) );
    strcpy( field->name, name );
    if( CompFlags.emit_browser_info ) {                 /* 27-oct-94 */
        field->xref = NewXref( NULL );
    }
    return( field );
}


FIELDPTR FieldDecl( TYPEPTR typ, type_modifiers mod, decl_state state )
{
    FIELDPTR    field;
    auto struct mod_info info;

    info.segment = 0;
    info.modifier = mod;
    info.based_kind = BASED_NONE;
    info.based_sym = NULL;
    typ = Pointer( typ, &info );
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        field = FieldDecl( (TYPEPTR) NULL, info.modifier, DECL_STATE_NONE );
        info.modifier = FLAG_NONE;
        MustRecog( T_RIGHT_PAREN );
        ParseDeclPart2( &field->field_type, typ );
    } else {
        if( CurToken == T_ID ) {
            for(;;) {
                field = FieldCreate( Buffer );
                NextToken();
                if( CurToken != T_ID && CurToken != T_TIMES ) break;
                if( state & DECL_STATE_NOTYPE  ) {
                    CErr2p( ERR_MISSING_DATA_TYPE, field->name );
                    if( CurToken == T_TIMES ) { /* "garbage *p" */
                        typ = Pointer( typ, &info );
                    }
                } else if( CurToken == T_TIMES ) {
                    Expecting( "," );
                } else {
                    Expecting( ",' or ';" );
                }
                if( CurToken != T_ID ) break;
            }
        } else {
            field = FieldCreate( "" );
        }
        field->attrib = info.modifier;
        typ = DeclPart2( typ );
        field->field_type = typ;
    }
    return( field );
}


local TYPEPTR ArrayDecl( TYPEPTR typ )
{
    long        dimension;
    TYPEPTR     first_node, next_node, prev_node;

    if( typ != NULL ) {                                 /* 16-mar-90 */
        if( typ->decl_type == TYPE_FUNCTION ) {
            CErr1( ERR_CANT_HAVE_AN_ARRAY_OF_FUNCTIONS );
        }
    }
    first_node = NULL;
    prev_node = NULL;
    while( CurToken == T_LEFT_BRACKET ) {
        NextToken();
        if( CurToken != T_RIGHT_BRACKET ) {
            const_val val;

            if( ConstExprAndType( &val ) ){
                dimension = val.val32;
            }else{
                dimension = 1;
            }
            if( dimension <= 0 ) {
                CErr1( ERR_INVALID_DIMENSION );
                dimension = 1;
            }
        } else {
            if( first_node == NULL ) {
                dimension = 0;
            } else {
                CErr1( ERR_DIMENSION_REQUIRED );
                dimension = 1;
            }
        }
        MustRecog( T_RIGHT_BRACKET );
        next_node = ArrayNode( typ );
        next_node->u.array->dimension = dimension;
        if( first_node == NULL ) {
            first_node = next_node;
        } else {
            prev_node->object = next_node;
        }
        prev_node = next_node;
    }
    return( first_node );
}


static TYPEPTR DeclPart3( TYPEPTR typ )
{
    PARMPTR     parm_list;
    TYPEPTR     *parms;
    TYPEPTR     *FuncProtoType();

    parms = NULL;
    if( CurToken != T_RIGHT_PAREN ) {
        parm_list = ParmList;
        ParmList = NULL;
        parms = FuncProtoType();
        if( parms == NULL  &&  ParmList != NULL ) {
            if( CurToken == T_SEMI_COLON || CurToken == T_COMMA ) {
                /* int f16(i,j); */
                CErr1( ERR_ID_LIST_SHOULD_BE_EMPTY );
            }
        }
        if( parm_list != NULL )  {
            FreeParmList();
            ParmList = parm_list;
        }
    } else {
        NextToken();    /* skip over ')' */
    }
    if( typ != NULL ) {                                 /* 09-apr-90 */
        TYPEPTR typ2;

        typ2 = typ;                                     /* 08-dec-93 */
        while( typ2->decl_type == TYPE_TYPEDEF ) typ2 = typ2->object;
        if( typ2->decl_type == TYPE_ARRAY ) {
            CErr1( ERR_FUNCTION_CANT_RETURN_AN_ARRAY );
        } else if( typ2->decl_type == TYPE_FUNCTION ) {
            CErr1( ERR_FUNCTION_CANT_RETURN_A_FUNCTION );
        }
    }
    typ = FuncNode( typ, FLAG_NONE, parms );
    return( typ );
}


static TYPEPTR DeclPart2( TYPEPTR typ )
{
    if( CurToken == T_LEFT_PAREN ) {
        NextToken();
        typ = DeclPart3( typ );
    }
    for(;;) {
        if( CurToken == T_LEFT_BRACKET ) {
            typ = ArrayDecl( typ );
        }
        if( CurToken != T_LEFT_PAREN ) break;
        NextToken();
        typ = DeclPart3( typ );
    }
    return( typ );
}


local void CheckUniqueName( PARMPTR parm, char *name )  /* 13-apr-89 */
{
    if( name != NULL ) {                                /* 29-oct-91 */
        if( *name != '\0' ) {
            for( ; parm ; parm = parm->next_parm ) {
                if( parm->sym.name != NULL ) {          /* 16-oct-92 */
                    if( strcmp( parm->sym.name, name ) == 0 ) {
                        CErr2p( ERR_SYM_ALREADY_DEFINED, name );
                        parm->sym.flags |= SYM_REFERENCED;
                    }
                }
            }
        }
    }
}


struct parm_list *NewParm( TYPEPTR typ, struct parm_list *prev_parm )
{
    struct parm_list *parm;

    parm = (struct parm_list *)CMemAlloc( sizeof( struct parm_list ) );
    parm->parm_type = typ;
    parm->next_parm = prev_parm;
    return( parm );
}


void AdjParmType( SYMPTR sym )
{
    TYPEPTR     typ;

    typ = sym->sym_type;
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( typ->decl_type == TYPE_FUNCTION ) {
        sym->sym_type = PtrNode( sym->sym_type, sym->attrib, SEG_CODE );
        sym->attrib = FLAG_NONE;
    } else if( typ->decl_type == TYPE_ARRAY ) {
        sym->sym_type = PtrNode( typ->object,
                        FLAG_WAS_ARRAY | sym->attrib,
                       SEG_DATA );
        sym->attrib = FLAG_NONE;
    }
}


local TYPEPTR *GetProtoType( decl_info *first )
{
    PARMPTR     parm;
    PARMPTR     prev_parm;
    PARMPTR     parm_namelist;
    int         parm_count;
    struct parm_list *parmlist;
    decl_state        state;
    declspec_class   declspec;
    stg_classes      stg_class;
    type_modifiers    mod;
    TYPEPTR          typ;
    decl_info        info;

    parm_count = 0;
    parmlist = NULL;
    parm_namelist = NULL;
    info = *first;
    for(;;) {
        SYMPTR           sym; // parm sym

        stg_class = info.stg;
        declspec  = info.decl;
        typ = info.typ;
        mod = info.mod;
        if( stg_class != SC_NULL  &&  stg_class != SC_REGISTER ) {
            CErr1( ERR_INVALID_STG_CLASS_FOR_PARM_PROTO );
        }
        state = DECL_STATE_ISPARM;
        if( typ == NULL ) {
            state |= DECL_STATE_NOTYPE;
            if( stg_class == SC_NULL ) {                /* 30-nov-94 */
                CErr1( ERR_TYPE_REQUIRED_IN_PARM_LIST );
            }
            typ = TypeDefault();
        }
        parm = (PARMPTR) CMemAlloc( sizeof( PARM_ENTRY ) );
        parm->next_parm = NULL;
        sym = &parm->sym;
        memset( sym, 0, sizeof( SYM_ENTRY ) );              /* 02-apr-91 */
        sym->name = "";
        Declarator( sym, mod, typ, state );
        typ = sym->sym_type;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( typ->decl_type == TYPE_VOID ) {
            char buffer[20];
            char *name;
            if( sym->name[0] == '\0' ) {
                strcpy( buffer, "Parm " );
                itoa( parm_count, &buffer[5], 10 );
                name = buffer;
            }else{
                name = sym->name;
            }
            CErr2p( ERR_VAR_CANT_BE_VOID, name );
            sym->sym_type = TypeDefault();
        }
        if( stg_class == SC_NULL ) {
            stg_class = SCSpecifier();                  /* 17-mar-91 */
            if( stg_class == SC_NULL )  stg_class = SC_AUTO;
        }
        sym->stg_class = stg_class;
        AdjParmType( sym );
        parmlist = NewParm( sym->sym_type, parmlist );
        if( parm_count == 0 ) {
            parm_namelist = parm;
        } else {
            CheckUniqueName( parm_namelist, sym->name );
            prev_parm->next_parm = parm;
        }
        if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
            sym->flags |= SYM_REFERENCED;
        }
        ++parm_count;
        if( CurToken == T_RIGHT_PAREN ) break;
        if( CurToken == T_EOF  ||  CurToken == T_LEFT_BRACE )  break;
        MustRecog( T_COMMA );
        if( CurToken == T_DOT_DOT_DOT ) {
            typ = GetType( TYPE_DOT_DOT_DOT );
            parmlist = NewParm( typ, parmlist );
            NextToken();
            break;
        }
        prev_parm = parm;
        FullDeclSpecifier( &info );
    }
    ParmList = parm_namelist;
        /* if void is specified as a parm, it is the only parm allowed */
    return( MakeParmList( parmlist, parm_count, 0 ) );
}


TYPEPTR *MakeParmList( struct parm_list *parm, int parm_count, int reversed )
{
    TYPEPTR     *type_list;
    struct parm_list *next_parm, *prev_parm;
    TYPEPTR     typ;
    int         index;

    type_list = NULL;
    if( parm != NULL ) {
        if( ! reversed ) {
            prev_parm = NULL;
            for(;;) {
                next_parm = parm->next_parm;
                parm->next_parm = prev_parm;
                if( next_parm == NULL ) break;
                prev_parm = parm;
                parm = next_parm;
            }
        }
        parm_count = 0;
        next_parm = parm;
        while( next_parm != NULL ) {
            ++parm_count;
            next_parm = next_parm->next_parm;
        }

        /* try to find an existing parm list that matches, 29-dec-88 */

        index = parm_count;
        if( index > MAX_PARM_LIST_HASH_SIZE ) {
            index = MAX_PARM_LIST_HASH_SIZE;
        }
        for( typ = FuncTypeHead[ index ]; typ; typ = typ->next_type ) {
            type_list = typ->u.parms;
            next_parm = parm;
            for(;;) {
                if( next_parm == NULL ) {
                    if( *type_list != NULL ) break;
                    while( parm != NULL ) {
                        next_parm = parm->next_parm;
                        CMemFree( parm );
                        parm = next_parm;
                    }
                    return( typ->u.parms );
                }
                if( next_parm->parm_type != *type_list ) break;
                next_parm = next_parm->next_parm;
                ++type_list;
            }
        }

        type_list = (TYPEPTR *)CPermAlloc( (parm_count+1)*sizeof(TYPEPTR));
        if( type_list != NULL ) {
            type_list[ parm_count ] = NULL;
            parm_count = 0;
            while( parm != NULL ) {
                type_list[ parm_count ] = parm->parm_type;
                next_parm = parm->next_parm;
                CMemFree( parm );
                parm = next_parm;
                ++parm_count;
            }
        }
    }
    return( type_list );
}

local int VoidType( TYPEPTR typ )                       /* 03-oct-91 */
{
    if( typ != NULL ) {
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( typ->decl_type == TYPE_VOID )  return( 1 );
    }
    return( 0 );
}

local TYPEPTR *FuncProtoType()
{
    TYPEPTR             *type_list;
    TAGPTR              old_taghead;
    decl_info           info;

    if( !CompFlags.extensions_enabled ) ++SymLevel; /* 03-may-89 */
    old_taghead = TagHead;
    FullDeclSpecifier( &info );
    if( info.stg == SC_NULL  &&  info.typ == NULL ) {
        GetFuncParmList();
        if( CurToken == T_RIGHT_PAREN ) {
            NextToken();
        } else {
            Expecting( ")" );                               /* 24-mar-91 */
        }
        type_list = NULL;
    } else {    /* function prototype present */
        if( VoidType( info.typ )  &&  /*27-dec-88*/
            info.stg== SC_NULL  &&  CurToken == T_RIGHT_PAREN ) {
            type_list = VoidParmList;
        } else {
            type_list = GetProtoType(  &info );
        }
        if( CurToken == T_RIGHT_PAREN ) {
            NextToken();
        } else {
            Expecting( ")" );                       /* 24-mar-91 */
        }
        if( CurToken != T_LEFT_BRACE ) {                /* 18-jan-89 */
            if( SymLevel > 1  ||                        /* 03-dec-90 */
            ! CompFlags.extensions_enabled ) {  /* 25-jul-91 */
            /* get rid of any new tags regardless of SymLevel;  23-jul-90 */
                TagHead = old_taghead;  /* get rid of new tags from proto */
                FreeEnums();                    /* 03-may-89 */
            }
        }
    }
    if( !CompFlags.extensions_enabled ) --SymLevel; /* 03-may-89 */
    return( type_list );
}


local void GetFuncParmList()
{
    PARMPTR     parm;
    PARMPTR     newparm;
    PARMPTR     parm_namelist;

    parm_namelist = NULL;
    while( CurToken == T_ID ) { /* scan off func parm list */
        if( parm_namelist == NULL ) {
            parm = (PARMPTR) CMemAlloc( sizeof( PARM_ENTRY ) );
            SymCreate( &parm->sym, Buffer );
            parm_namelist = parm;
        } else {
            newparm = (PARMPTR) CMemAlloc( sizeof( PARM_ENTRY ) );
            SymCreate( &newparm->sym, Buffer );
            CheckUniqueName( parm_namelist, Buffer );
            parm->next_parm = newparm;
            parm = newparm;
        }
        parm->sym.info.hash_value = HashValue;
        if( (Toggles & TOGGLE_UNREFERENCED) == 0 ) {
            parm->sym.flags |= SYM_REFERENCED;
        }
        NextToken();
        if( CurToken == T_RIGHT_PAREN ) break;
        if( CurToken == T_EOF ) break;
        if( CurToken != T_COMMA ) {     /* 04-jan-89 */
            MustRecog( T_COMMA );               /* forces error msg */
            for(;;) {   /* skip until ')' to avoid cascading errors */
                if( CurToken == T_RIGHT_PAREN ) break;
                if( CurToken == T_EOF ) break;
                NextToken();
            }
            break;
        }
        MustRecog( T_COMMA );
        if( CurToken == T_DOT_DOT_DOT ) {
            parm->next_parm = (PARMPTR) CMemAlloc( sizeof( PARM_ENTRY ) );
            parm = parm->next_parm;
            SymCreate( &parm->sym, "" );
            /* set flags so we don't give funny error messages */
            parm->sym.flags |= SYM_TEMP | SYM_ASSIGNED | SYM_REFERENCED;
            NextToken();
            break;
        }
    }
    ParmList = parm_namelist;
}


local void FreeParmList()
{
    PARMPTR     parm;

    for( ; parm = ParmList; ) {
        ParmList = parm->next_parm;
        CMemFree( parm->sym.name );
        CMemFree( parm );
    }
}

#if _CPU == 370
local bool IsIntComp( TYPEPTR ret1 )
    /*
     * what's target compatible between default int as ret type
     * and a later declaration
     */
{
    bool        ret;

    while( ret1->decl_type == TYPE_TYPEDEF ) ret1 = ret1->object;
    switch( ret1->decl_type ) {
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
    case TYPE_USHORT:
    case TYPE_INT:
    case  TYPE_LONG:
        ret = TRUE;
        break;
    default:
       ret = FALSE;
    }
    return( ret );
}
#endif
