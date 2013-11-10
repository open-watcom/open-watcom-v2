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
* Description:  Decode names in DWARF debug info. Language dependent.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <watcom.h>
#include <dwarf.h>
#include "drpriv.h"
#include "drutils.h"
#include "drscope.h"
#include "drdecnam.h"

/*----------------*
 * type definitions
 *----------------*/

/*
 * A simple string type to store a pointer to the string and its length
 */
typedef struct {
    char *         s;   /* the string */
    unsigned short l;   /* length of the string (excluding \0) */
} String;

typedef struct Node_S {
    struct Node_S   *next;
    bool            user_def;   /* TRUE if defined by user */
    dr_handle       entry;      /* if this is user-def'd, the entry */
    dr_sym_type     sym_type;   /* if u_def'd, the type of symbol */
    String          buf;        /* may contain several (non usr-def) words */
} *Node_T;                      /* note - a pointer to Node_S */

/*
 * Put new nodes at front or back?
 */
typedef enum { LIST_HEAD, LIST_TAIL } ListEnd_T;

typedef struct {
    Node_T      head;           /* head */
    Node_T      tail;           /* tail */
    ListEnd_T   end;            /* end to add at */
} List_T;

/*
 * this structure stores the various parts of a name.
 */

typedef struct {
    List_T dec_plg;     /* prolog for the declaration .. this is stored */
                        /* in forward order, not reversed */
    List_T type_plg;    /* prolog of type .. eg extern, const, volatile */
    List_T type_bas;    /* base part of type .. int * */
    List_T type_ptr;    /* pointer part of type *, & */
    List_T type_elg;    /* epilog of type .. const, volatile */
    List_T type_inh;    /* list of base classes */

    List_T var_plg;     /* prolog for variable name .. eg (* */
    List_T var_bas;     /* base of variable name .. foo */
    List_T var_elg;     /* epilog for variable name .. eg ) (param list) */

    /*
     * these are stored in forward order, and the space is put on at
     * the front
     */
    List_T func_plg;    /* plg for function paragmeters "(" */
    List_T func_bas;    /* base of function's parameters */
    List_T func_elg;    /* elg for function parms " )" */
} BrokenName_T;

static BrokenName_T Empty_Broken_Name = {
    { NULL, NULL, LIST_TAIL },     /* dec_plg */
    { NULL, NULL, LIST_HEAD },     /* type_plg */
    { NULL, NULL, LIST_HEAD },     /* type_bas */
    { NULL, NULL, LIST_HEAD },     /* type_ptr */
    { NULL, NULL, LIST_TAIL },     /* type_elg */
    { NULL, NULL, LIST_TAIL },     /* type_inh */
    { NULL, NULL, LIST_HEAD },     /* var_plg */
    { NULL, NULL, LIST_HEAD },     /* var_bas */
    { NULL, NULL, LIST_HEAD },     /* var_elg */
    { NULL, NULL, LIST_TAIL },     /* func_plg */
    { NULL, NULL, LIST_TAIL },     /* func_bas */
    { NULL, NULL, LIST_TAIL }      /* func_elg*/
};

/*
 * this structure holds the current location
 */
typedef struct {
    dr_handle  parent;      /* containing die */
    dr_handle  entry_st;    /* start of the die */
    dr_handle  entry_cr;    /* location in the die */
    dr_handle  abbrev_st;   /* start of the abbrev */
    dr_handle  abbrev_cr;   /* location within the abbrev */
    dr_handle  tag;         /* the tag */
    unsigned_8 child;       /* DW_children_yes if entry has children */
    bool       inParam;     /* decorating parameters? (only used for FORTRAN)*/
} Loc_T;

/*
 * put new symbols to left or right of base type?
 */
typedef enum { TYPE_PLG, TYPE_BAS, TYPE_PTR, TYPE_ELG } TypeSide_T;

typedef struct {
    BrokenName_T    *decname;
    int             firstTime;
} BaseSearchInfo;


/*----------------*
 * static functions
 *----------------*/

static BrokenName_T  BuildList( dr_handle, dr_handle );
static void          BuildCList( BrokenName_T *, Loc_T * );
static void          BuildFortranList( BrokenName_T *, Loc_T * );
static BrokenName_T *DecorateVariable( BrokenName_T *, Loc_T * );
static BrokenName_T *DecorateMember( BrokenName_T *, Loc_T * );
static BrokenName_T *DecorateTypedef( BrokenName_T *, Loc_T * );
static BrokenName_T *DecorateNameSpace( BrokenName_T *, Loc_T * );
static BrokenName_T *DecorateLabel( BrokenName_T *, Loc_T * );
static BrokenName_T *DecorateFunction( BrokenName_T *, Loc_T * );
static List_T        StartFunctionParms( Loc_T * );
static List_T        DecorateParameter( Loc_T * );
static BrokenName_T *DecorateType( BrokenName_T *, Loc_T *, dr_handle );
static void          SwapModifier( BrokenName_T * );
static BrokenName_T *AddPtrModifier( BrokenName_T *, Loc_T * );
static BrokenName_T *DecSubroutineType( BrokenName_T *, Loc_T *, dr_handle );
static void          AddTypeString( BrokenName_T *, String, TypeSide_T);
static BrokenName_T *DecorateCompoundType( BrokenName_T *, Loc_T *,
                                           String, dr_sym_type );
static BrokenName_T *DecorateBases( BrokenName_T *, Loc_T * );
static int           baseHook( dr_sym_type, dr_handle, char *, dr_handle, void * );
static BrokenName_T *DecorateArray( BrokenName_T *, Loc_T * );
static BrokenName_T *DecoratePtrToMember( BrokenName_T *, Loc_T * );

static void ReadBlock( unsigned_8 **buf, dr_handle entry, unsigned len );
static void FORDecVariable( BrokenName_T *, Loc_T * );
static void FORAddConstVal( BrokenName_T * decname, Loc_T * loc, Loc_T * type_loc );
static void FORDecParam( BrokenName_T *, Loc_T * );
static void FORDecMember( BrokenName_T *, Loc_T * );
static void FORDecSubprogram( BrokenName_T *, Loc_T * );
static void FORDecEntryPoint( BrokenName_T *, Loc_T * );
static void FORDecStructure( BrokenName_T *, Loc_T * );
static void FORDecNameList( BrokenName_T *, Loc_T * );
static void FORDecRecord( BrokenName_T *, Loc_T * );
static void FORDecUnion( BrokenName_T *, Loc_T * );
static void FORDecType( BrokenName_T *, Loc_T * );
static void FORDecArray( BrokenName_T *, Loc_T * );
static void FORDecString( BrokenName_T *, Loc_T * );
static void FORDecCommon( BrokenName_T *, Loc_T * );

static String         FormName( BrokenName_T * );
static List_T         FormList( BrokenName_T * );
static void           FillLoc( Loc_T *, dr_handle );
static void           FreeList( List_T );
static void           IterateList( void (*)( void *, char *,
                                             int, dr_handle, dr_sym_type ),
                                   void *, List_T );
static void           ReallocStr( String * );
static void           ListConcat( List_T *, String );
static void           ListAdd( List_T *, Node_T );
static void           EndNode( List_T *, bool, dr_handle, dr_sym_type );
static Node_T         DeleteTail( List_T * );

/*-----------------*
 * static variables
 *-----------------*/

static String const ArrayLeftKwd = { "[", 1 };
static String const ArrayRightKwd = { "]", 1 };
static String const BaseKwd = { " : ", 3 };
static String const BaseSepKwd = { ", ", 2 };
static String const ClassKwd = { " ssalc", 6 };
static String const ConstKwd = { "const ", 6 };
static String const EnumKwd = { " mune", 5 };
static String const ExternKwd = { "extern ", 7 };
static String const FarKwd = { " raf", 4 };
static String const FuncEndKwd = { ")", 1 };
static String const FuncStartKwd = { "(", 1 };
static String const HugeKwd = { " eguh", 5 };
static String const LabelKwd = { " :",  2 };
static String const MemberKwd = { "::", 2 };
static String const NearKwd = { " raen", 5 };
static String const ParmSepKwd = { ", ", 2 };
static String const PtrKwd = { "*", 1 };
static String const RefKwd = { "&", 1 };
static String const SpaceKwd = { " ", 1 };
static String const StructKwd = { " tcurts", 7 };
static String const TypedefKwd = { "typedef ", 8 };
static String const NameSpaceKwd = { "namespace ", 10 };
static String const UnionKwd = { " noinu", 6 };
static String const UnspecParamKwd = { "...", 3 };
static String const VolatileKwd = { "volatile ", 9 };

static String const FORArrayLeftKwd =   { "(", 1 };
static String const FORArrayRightKwd =  { ")", 1 };
static String const FORAllocArrKwd =    { ":", 1 };     /* allocatable array */
static String const FORAssumeArrKwd =   { "*", 1 };    /* assumed-size array */
static String const FORSepKwd =         { ",", 1 };
static String const FORFuncStartKwd =   { "(", 1 };
static String const FORFuncEndKwd =     { ")", 1 };
static String const FORFuncKwd =        { " NOITCNUF", 9 };
static String const FORSubprogramKwd =  { " ENITUORBUS", 11 };
static String const FOREntryPointKwd =  { " YRTNE", 6 };
static String const FORMemberKwd =      { ".", 1 };
static String const FORParmSepKwd =     { ", ", 2 };
static String const FORSlashKwd =       { "/", 1 };
static String const FORStructKwd =      { " ERUTCURTS", 10 };
static String const FORNameListKwd =    { " TSILEMAN", 9 };
static String const FORCommonKwd =      { " NOMMOC", 7 };
static String const FORRecordKwd =      { " DROCER", 7 };
static String const FORUnionKwd =       { "NOINU", 5 };
static String const FORStringKwd =      { "*RETCARAHC", 10 };
static String const FORParamKwd =       { " RETEMARAP", 10 };
static String const FOREqualKwd =       { "=", 1 };

static const char *FORMainProgMatch = "MARGORP NIAM";

static String const * const AddressClasses[] = {
    NULL,       /* ADDR_none */
    &NearKwd,   /* ADDR_near16 */
    &FarKwd,    /* ADDR_far16 */
    &HugeKwd,   /* ADDR_huge16 */
    &NearKwd,   /* ADDR_near32 */
    &FarKwd     /* ADDR_far32 */
};

static const char *LBLFunction =            "Function";
static const char *LBLSubprogram =          "Subprogram";
static const char *LBLTypedef =             "Typedef";
static const char *LBLEnum =                "Enum";
static const char *LBLUnion =               "Union";
static const char *LBLStructure =           "Structure";
static const char *LBLClass =               "Class";
static const char *LBLCommonBlock =         "Common Block";
static const char *LBLVariable =            "Variable";
static const char *LBLParameter =           "Parameter";

extern void DRDecorateLabel( dr_handle die, char *buf )
/*****************************************************/
{
    Loc_T           loc;
    dr_language     lang;
    compunit_info   *compunit;
    const char      *label;
    dr_handle       tmp_abbrev;
    dr_handle       tmp_entry;

    label = NULL;
    FillLoc( &loc, die );

    compunit = DWRFindCompileInfo( die );
    lang = DRGetLanguageAT( compunit->start + COMPILE_UNIT_HDR_SIZE );

    switch( loc.tag ) {
    case DW_TAG_subprogram:
        switch( lang ) {
        case DR_LANG_C:
        case DR_LANG_CPLUSPLUS:
            label = LBLFunction;
            break;
        case DR_LANG_FORTRAN:
            tmp_abbrev = loc.abbrev_cr;
            tmp_entry = loc.entry_cr;
            if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) ) {
                label = LBLFunction;
            } else {
                label = LBLSubprogram;
            }
            break;
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        }
        break;
    case DW_TAG_typedef:
        label = LBLTypedef;
        break;
    case DW_TAG_enumeration_type:
        label = LBLEnum;
        break;
    case DW_TAG_union_type:
        label = LBLUnion;
        break;
    case DW_TAG_structure_type:
        label = LBLStructure;
        break;
    case DW_TAG_class_type:
        label = LBLClass;
        break;
    case DW_TAG_common_block:
        label = LBLCommonBlock;
        break;
    case DW_TAG_formal_parameter:
    case DW_TAG_member:
    case DW_TAG_variable:
        switch( lang ) {
        case DR_LANG_C:
        case DR_LANG_CPLUSPLUS:
            label = LBLVariable;
            break;
        case DR_LANG_FORTRAN:
            tmp_abbrev = loc.abbrev_cr;
            tmp_entry = loc.entry_cr;
            if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_const_value ) ) {
                label = LBLParameter;
            } else {
                label = LBLVariable;
            }
            break;
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
        }
        break;
    default:
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }

    strncpy( buf, label, DRDECLABELLEN );
}

extern char * DRDecoratedName( dr_handle die, dr_handle parent )
/**************************************************************/
{
    BrokenName_T    decstruct;
    char            *retstr;

    decstruct = BuildList( die, parent );
    retstr = ( FormName( &decstruct ) ).s;

    return( retstr );
}

extern void DRDecoratedNameList( void *obj, dr_handle die, dr_handle parent,
                                 void (* cb)( void *, char *,
                                               int, dr_handle,
                                               dr_sym_type ) )
/**************************************************************************/
{
    BrokenName_T    decstruct;
    List_T          list;

    decstruct = BuildList( die, parent );

    list = FormList( &decstruct );
    IterateList( cb, obj, list );
}

static BrokenName_T BuildList( dr_handle die, dr_handle parent )
/**************************************************************/
{
    Loc_T           loc;
    BrokenName_T    decstruct = Empty_Broken_Name;
    compunit_info   *compunit;
    dr_language     lang;

    FillLoc( &loc, die );
    loc.parent = parent;

    compunit = DWRFindCompileInfo( die );
    lang = DRGetLanguageAT( compunit->start + COMPILE_UNIT_HDR_SIZE );

    switch( lang ) {
    case DR_LANG_CPLUSPLUS:
    case DR_LANG_C:
        BuildCList( &decstruct, &loc );
        break;
    case DR_LANG_FORTRAN:
        BuildFortranList( &decstruct, &loc );
        break;
    default:
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    return( decstruct );
}

static void BuildFortranList( BrokenName_T *decname, Loc_T *loc )
/***************************************************************/
{
    switch( loc->tag ) {
    case DW_TAG_formal_parameter:
        FORDecParam( decname, loc );
        break;

    case DW_TAG_variable:
        FORDecVariable( decname, loc );
        break;

    case DW_TAG_member:
        if( loc->parent ) {
            FORDecMember( decname, loc );
        } else {
            FORDecVariable( decname, loc );
        }
        break;

    case DW_TAG_subprogram:
        FORDecSubprogram( decname, loc );
        break;

    case DW_TAG_entry_point:
        FORDecEntryPoint( decname, loc );
        break;

    case DW_TAG_structure_type:
        FORDecStructure( decname, loc );
        break;

    case DW_TAG_namelist:
        FORDecNameList( decname, loc );
        break;

    case DW_TAG_union_type:
        FORDecUnion( decname, loc );
        break;

    case DW_TAG_common_block:
        FORDecCommon( decname, loc );
        break;

    default:
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
}

static void BuildCList( BrokenName_T *decname, Loc_T *loc )
/*********************************************************/
{
    switch( loc->tag ) {
    case DW_TAG_formal_parameter:
    case DW_TAG_variable:
        DecorateVariable( decname, loc );
        break;

    case DW_TAG_member:
        if( loc->parent ) {
            DecorateMember( decname, loc );
        } else {
            DecorateVariable( decname, loc );
        }
        break;

    case DW_TAG_typedef:
        DecorateTypedef( decname, loc );
        break;

    case DW_TAG_label:
        DecorateLabel( decname, loc );
        break;

    case DW_TAG_subprogram:
        DecorateFunction( decname, loc );
        break;

    case DW_TAG_enumeration_type:
        DecorateCompoundType( decname, loc, EnumKwd, DR_SYM_ENUM );
        break;

    case DW_TAG_class_type:
        DecorateCompoundType( decname, loc, ClassKwd, DR_SYM_CLASS );
        DecorateBases( decname, loc );
        break;

    case DW_TAG_structure_type:
        DecorateCompoundType( decname, loc, StructKwd, DR_SYM_CLASS );
        DecorateBases( decname, loc );
        break;

    case DW_TAG_union_type:
        DecorateCompoundType( decname, loc, UnionKwd, DR_SYM_CLASS );
        break;

    case DW_TAG_WATCOM_namespace:
        DecorateNameSpace( decname, loc );
        break;
    default:
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }
}

static void GrabName( dr_handle abbrev, dr_handle entry, String *name )
/*********************************************************************/
// read a name, setting length and reversing
{
    name->s = DWRGetName( abbrev, entry );
    if( name->s == NULL ) {
        name->s = DWRALLOC( 1 );
        *name->s = '\0';
    }
    name->l = strlen( name->s );
    strrev( name->s );
}

static void GetClassName( dr_handle   entry,
                          String      *containing_name )
/******************************************************/
{  //try and skip PCH
    dr_handle   abbrev;
    dr_handle   tmp_entry;
    dw_tagnum   tag;
    char        *name;

    name = NULL;
    while( entry ) {
        tmp_entry = entry;
        abbrev = DWRVMReadULEB128( &tmp_entry );
        abbrev = DWRLookupAbbrev( tmp_entry, abbrev );
        tag = DWRVMReadULEB128( &abbrev );
        abbrev++;           // skip the child pointer.
        name = DWRGetName( abbrev, tmp_entry );
        if( name != NULL ) break;
        if( tag != DW_TAG_typedef ) break;
        entry = DRGetTypeAT( entry );
    }
    if( name == NULL ) {
        name = DWRALLOC( 1 );
        *name = '\0';
    }
    strrev( name );
    containing_name->s = name;
    containing_name->l = strlen( name );
}

static dr_handle SkipPCH( dr_handle entry )
/*****************************************/
{  //try and skip PCH
    dr_handle   abbrev;
    dr_handle   tmp_entry;
    dw_tagnum   tag;
    dw_atnum    attrib;

    while( entry ) {
        tmp_entry = entry;
        abbrev = DWRVMReadULEB128( &tmp_entry );
        abbrev = DWRLookupAbbrev( tmp_entry, abbrev );
        tag = DWRVMReadULEB128( &abbrev );
        if( tag != DW_TAG_typedef ) break;
        abbrev++;           // skip the child pointer.
        for( ;; ) {
            attrib = DWRVMReadULEB128( &abbrev );
            if( attrib == DW_AT_name ) break;
            if( attrib == 0 ) break;
        }
        if( attrib != 0 ) break;
        entry = DRGetTypeAT( entry );
    }
    return( entry );
}

/*
 * decorate the name of a DW_TAG_variable die.
 */

static BrokenName_T *DecorateVariable( BrokenName_T *decname, Loc_T *loc )
/************************************************************************/
{
    String      varname;
    dr_handle   tmp_entry;
    dr_handle   tmp_abbrev;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    GrabName( tmp_abbrev, tmp_entry, &varname );

    EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_VARIABLE );
    ListConcat( &( decname->var_bas ), varname );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( varname.s );

    /* check if external */
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_external )
        == DW_AT_external ) {
        if( DWRReadConstant( tmp_abbrev, tmp_entry ) ) {
            ListConcat( &( decname->dec_plg ), ExternKwd );
        }
    }

    /*
     *check to see if this is a member of a containing die
     */
    tmp_abbrev = loc->abbrev_cr;    /* reset to start of die / abbrev */
    tmp_entry = loc->entry_cr;

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_containing_type )
                           == DW_AT_containing_type  ) {
        dr_handle containing_die;

        containing_die = DWRReadReference( tmp_abbrev, tmp_entry );
        if( containing_die != DW_TAG_padding ) {
            String    containing_name;

            GetClassName( containing_die, &containing_name );
            if( containing_name.s != NULL ) {

                ListConcat( &( decname->var_bas ), MemberKwd );

                EndNode( &( decname->var_bas ), TRUE, containing_die, DR_SYM_CLASS );
                ListConcat( &( decname->var_bas ), containing_name );
                EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

                DWRFREE( containing_name.s );
            }
        }
    }

    /* check for type information */

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) == DW_AT_type ) {
        dr_handle type_die;

        type_die = DWRReadReference( tmp_abbrev, tmp_entry );
        type_die =  SkipPCH( type_die );
        if( type_die != DW_TAG_padding ) {
            Loc_T type_loc;

            FillLoc( &type_loc, type_die );

            DecorateType( decname, &type_loc, DW_TAG_padding );
        }
    }

    return( decname );
}

/*
 * decorate a member of a class or struct -- depends on loc.parent
 */

static BrokenName_T *DecorateMember( BrokenName_T *decname, Loc_T *loc )
/**********************************************************************/
{
    String  tmp_str;

    DecorateVariable( decname, loc );

    ListConcat( &( decname->var_bas ), MemberKwd );
    tmp_str.s = DRGetName( loc->parent );
    if( tmp_str.s == NULL ) {
        tmp_str.s = DWRALLOC( 1 );
        *tmp_str.s = '\0';
    }
    tmp_str.s = strrev( tmp_str.s );
    tmp_str.l = strlen( tmp_str.s );
    EndNode( &( decname->var_bas ), TRUE, loc->parent, DR_SYM_CLASS );
    ListConcat( &( decname->var_bas ), tmp_str );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );
    DWRFREE( tmp_str.s );

    return( decname );
}

/*
 * Decorate a label.
 */
static BrokenName_T *DecorateLabel( BrokenName_T *decname, Loc_T *loc )
/*********************************************************************/
{
    String  lab_name;

    GrabName( loc->abbrev_cr, loc->entry_cr, &lab_name );

    ListConcat( &( decname->var_bas ), LabelKwd );
                                                    /* NYI -- sb label */
    EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_VARIABLE );
    ListConcat( &( decname->var_bas ), lab_name );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( lab_name.s );

    return( decname );
}

/*
 * Decorate a typedef.
 */

static BrokenName_T *DecorateTypedef( BrokenName_T *decname, Loc_T *loc )
/***********************************************************************/
{
    ListConcat( &( decname->dec_plg ), TypedefKwd );

    DecorateVariable( decname, loc );

    return( decname );
}

static BrokenName_T *DecorateNameSpace( BrokenName_T *decname, Loc_T *loc )
/*************************************************************************/
{
    ListConcat( &( decname->dec_plg ), NameSpaceKwd );

    DecorateVariable( decname, loc );

    return( decname );
}
/*
 * decorate a function.
 */
static dw_tagnum GetTag( dr_handle entry )
/****************************************/
{
    dr_handle   abbrev;
    dw_tagnum   tag;

    abbrev = DWRVMReadULEB128( &entry );
    abbrev = DWRLookupAbbrev( entry, abbrev );
    tag = DWRVMReadULEB128( &abbrev );
    return( tag );
}

static void GetContaining( List_T *list, dr_handle of )
/*****************************************************/
{
    scope_trail container;
    scope_entry *curr;

    DRGetScopeList( &container, of );
    curr = container.head;
    while( curr != NULL ) {
        String      containing_name;
        dw_tagnum   tag;

        tag = GetTag( curr->handle );
        switch( tag ){
        case DW_TAG_class_type:
        case DW_TAG_union_type:
        case DW_TAG_structure_type:
        case DW_TAG_WATCOM_namespace:
            GetClassName( curr->handle, &containing_name );
            if( containing_name.s != NULL ) {
                ListConcat( list, MemberKwd );

                EndNode( list, TRUE, curr->handle,
                        DR_SYM_CLASS );
                ListConcat( list, containing_name );
                EndNode( list, FALSE, 0, DR_SYM_NOT_SYM );
                DWRFREE( containing_name.s );
            }
            break;
        default:
            goto done_loop;
        }
        curr = curr->next;
    } done_loop:;
    DREndScopeList( &container );
}

static BrokenName_T *DecorateFunction( BrokenName_T *decname, Loc_T *loc )
/************************************************************************/
{
    String      func_name;
    List_T      parms;
    dr_handle   tmp_abbrev;
    dr_handle   tmp_entry;
    Loc_T       type_loc;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    GrabName( tmp_abbrev, tmp_entry, &func_name );
    if( func_name.s != NULL ) {
        EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_FUNCTION );
        ListConcat( &( decname->var_bas ), func_name );
        EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

        DWRFREE( func_name.s );
    }


    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_external )
        == DW_AT_external ) {
        if( DWRReadConstant( tmp_abbrev, tmp_entry ) ) {
            ListConcat( &( decname->dec_plg ), ExternKwd );
        }
    }

    /* check to see if its a member of a containing entry */
    tmp_abbrev = loc->abbrev_cr;    /* reset */
    tmp_entry = loc->entry_cr;
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_containing_type )
                           ==  DW_AT_containing_type ){
        dr_handle containing_entry;

        containing_entry = DWRReadReference( tmp_abbrev, tmp_entry );
        if( containing_entry != DW_TAG_padding ) {
            GetContaining( &decname->var_bas, containing_entry );
        }
    }

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) == DW_AT_type ) {
        dr_handle type_entry = DWRReadReference( tmp_abbrev, tmp_entry );

        type_entry =  SkipPCH( type_entry );
        if( type_entry != DW_TAG_padding ) {
            FillLoc( &type_loc, type_entry );
            DecorateType( decname, &type_loc, DW_TAG_padding );
        }
    }

    parms = StartFunctionParms( loc );

    if( decname->func_bas.tail == NULL ) {
        decname->func_bas.head = parms.head;
        decname->func_bas.tail = parms.tail;
    } else {
        decname->func_bas.tail->next = parms.head;
        decname->func_bas.tail = parms.tail;
    }

    return( decname );
}

static List_T StartFunctionParms( Loc_T *loc )
/********************************************/
{
    List_T      ret = { NULL, NULL, LIST_TAIL };
    List_T      add_parm;
    bool        first_time = TRUE;
    Loc_T       parm_loc;

    ListConcat( &ret, FuncStartKwd );

    if( loc->child == DW_CHILDREN_yes ) {
        dr_handle tmp_entry;
        dr_handle tmp_abbrev;

        tmp_entry = loc->entry_cr;
        tmp_abbrev = loc->abbrev_cr;
        DWRSkipAttribs( tmp_abbrev, &tmp_entry );

        FillLoc( &parm_loc, tmp_entry );

        while( parm_loc.entry_st != 0 ) {

            add_parm = DecorateParameter( &parm_loc );

            if( add_parm.head != NULL ) {
                if( first_time ) {
                    first_time = FALSE;
                    ListConcat( &ret, SpaceKwd );
                } else {
                    ListConcat( &ret, ParmSepKwd );
                }

                if( ret.tail == NULL ) {
                    ret.head = add_parm.head;
                    ret.tail = add_parm.tail;
                } else {
                    ret.tail->next = add_parm.head;
                    ret.tail = add_parm.tail;
                }
            }
        }
    }

    if( !first_time ) {
        ListConcat( &ret, SpaceKwd );
    }

    ListConcat( &ret, FuncEndKwd );

    return( ret );
}

/*
 * decorate a function's formal parameters.
 * loc->entry_st points to a child entry.  DecorateParameter considers
 * this child -- if it is a parameter, it decorates it and returns a List_T.
 * otherwise, it checks siblings. loc is always updated to point to
 * the next sibling. If there are no more siblings, it sets
 * loc->entry_st to 0.
 */

static List_T DecorateParameter( Loc_T *loc )
/*******************************************/
{
    List_T          ret = { NULL, NULL };
    BrokenName_T    decstruct = Empty_Broken_Name;

    dr_handle       tmp_abbrev;
    dr_handle       tmp_entry;

    tmp_entry = loc->entry_cr;
    tmp_abbrev = loc->abbrev_cr;

    while( loc->tag != DW_TAG_unspecified_parameters &&
             loc->tag != DW_TAG_formal_parameter && loc->abbrev_st != 0 ) {

        if( loc->child ) {
            DWRSkipChildren( &tmp_abbrev, &tmp_entry );
        } else {
            DWRSkipAttribs( tmp_abbrev, &tmp_entry );
        }
        FillLoc( loc, tmp_entry );
        tmp_abbrev = loc->abbrev_cr;
        tmp_entry = loc->entry_cr;
    }

    if( loc->abbrev_st != 0 ) {
        switch( loc->tag ) {
        case DW_TAG_unspecified_parameters:
            ListConcat( &ret, UnspecParamKwd );
            break;

        case DW_TAG_formal_parameter:
            DecorateVariable( &decstruct, loc );
            ret = FormList( &decstruct );
            ret.end = LIST_TAIL;
            EndNode( &ret, FALSE, 0, DR_SYM_NOT_SYM );
            break;

        default:
            DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
        }

        tmp_abbrev = loc->abbrev_cr;
        tmp_entry = loc->entry_cr;
        if( loc->child ) {
            DWRSkipChildren( &tmp_abbrev, &tmp_entry );
        } else {
            DWRSkipAttribs( tmp_abbrev, &tmp_entry );
        }
        FillLoc( loc, tmp_entry );
    }

    return( ret );
}

/*
 * decorate a type.
 * prev_tag contains the
 * previous tag that was handled by the type.  This is needed for the
 * pointer to subroutine type -- it needs to know if it resolves to a pointer
 * to a subroutine, or a subroutine type itself.
 *
 * WARNING -- this changes the contents of loc.
 */

static BrokenName_T *DecorateType( BrokenName_T *decname, Loc_T *loc,
                                   dr_handle prev_tag )
/*******************************************************************/
{
    dr_handle   tmp_entry;
    dr_handle   tmp_abbrev;
    dr_handle   next_die;
    String      typename;
    bool        done = FALSE;   /* true when the end of the type chain is reached */

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    switch( loc->tag ) {
    case DW_TAG_base_type:
    case DW_TAG_typedef:
        GrabName( tmp_abbrev, tmp_entry, &typename );

        ListConcat( &( decname->type_bas ), SpaceKwd );
        if( loc->tag == DW_TAG_typedef ) {
            EndNode( &( decname->type_bas ), TRUE, loc->entry_st, DR_SYM_TYPEDEF );
            ListConcat( &( decname->type_bas ), typename );
            EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );
        } else {
            ListConcat( &( decname->type_bas ), typename );
        }
        if( prev_tag == DW_TAG_const_type || prev_tag == DW_TAG_volatile_type ) {
            SwapModifier( decname );
        }
        DWRFREE( typename.s );
        done = TRUE;

        break;

    case DW_TAG_const_type:
        if( prev_tag != DW_TAG_const_type && prev_tag != DW_TAG_volatile_type ) {
            EndNode( &( decname->type_elg ), FALSE, 0, DR_SYM_NOT_SYM );
        }
        AddTypeString( decname, ConstKwd, TYPE_ELG );
        break;

    case DW_TAG_volatile_type:
        if( prev_tag != DW_TAG_const_type && prev_tag != DW_TAG_volatile_type ) {
            EndNode( &( decname->type_elg ), FALSE, 0, DR_SYM_NOT_SYM );
        }
        AddTypeString( decname, VolatileKwd, TYPE_ELG );
        break;

    case DW_TAG_pointer_type:
        EndNode( &( decname->type_ptr ), FALSE, 0, DR_SYM_NOT_SYM );
        AddTypeString( decname, PtrKwd, TYPE_PTR );
        break;

    case DW_TAG_reference_type:
        EndNode( &( decname->type_ptr ), FALSE, 0, DR_SYM_NOT_SYM );
        AddTypeString( decname, RefKwd, TYPE_PTR );
        break;

    case DW_TAG_ptr_to_member_type:
        DecoratePtrToMember( decname, loc );
        break;

    case DW_TAG_WATCOM_address_class_type:
        if( prev_tag != DW_TAG_pointer_type ) {
            EndNode( &( decname->type_ptr ), FALSE, 0, DR_SYM_NOT_SYM );
        }
        AddPtrModifier( decname, loc );
        break;

    case DW_TAG_enumeration_type:
        DecorateCompoundType( decname, loc, EnumKwd, DR_SYM_ENUM );
        break;

    case DW_TAG_class_type:
        DecorateCompoundType( decname, loc, ClassKwd, DR_SYM_CLASS );
        break;

    case DW_TAG_structure_type:
        DecorateCompoundType( decname, loc, StructKwd, DR_SYM_CLASS );
        break;

    case DW_TAG_union_type:
        DecorateCompoundType( decname, loc, UnionKwd, DR_SYM_CLASS );
        break;

    case DW_TAG_array_type:
        DecorateArray( decname, loc );
        done = TRUE;
        break;

    case DW_TAG_subroutine_type:
        DecSubroutineType( decname, loc, prev_tag );
        break;

    default:
        DWREXCEPT ( DREXCEP_DWARF_LIB_FAIL );
    }

    if( !done ) {
        tmp_abbrev = loc->abbrev_cr;
        tmp_entry = loc->entry_cr;
        if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) == DW_AT_type ) {
            next_die = DWRReadReference( tmp_abbrev, tmp_entry );
            next_die =  SkipPCH( next_die );
            if( next_die != DW_TAG_padding ) {
                if( loc->tag == DW_TAG_WATCOM_address_class_type ) {
                    prev_tag = prev_tag;
                } else {
                    prev_tag = loc->tag;
                }
                FillLoc( loc, next_die );
                DecorateType( decname, loc, prev_tag );
            }
        }
    }

    return( decname );
}

/*
 * take the last node from type_bas and put it at the end of type_plg.
 * this is used for constant and volatile modifiers formed as right
 * associative expressions.
 */
static void SwapModifier( BrokenName_T *decname )
/***********************************************/
{
    Node_T  target;

    target = DeleteTail( &( decname->type_elg ) );

    if( target == NULL ) {
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
    }

    if( decname->type_plg.head == NULL || decname->type_plg.tail == NULL ) {
        decname->type_plg.head = target;
        decname->type_plg.tail = target;
    } else {
        target->next = decname->type_plg.head;
        decname->type_plg.head = target;
    }
    strrev( target->buf.s );
}

static BrokenName_T *AddPtrModifier( BrokenName_T *decname, Loc_T *loc )
/**********************************************************************/
{
    dr_handle   tmp_entry;
    dr_handle   tmp_abbrev;
    unsigned_32 addr_class;
    bool        spaceit = FALSE;

    tmp_entry = loc->entry_cr;
    tmp_abbrev = loc->abbrev_cr;
    if( DWRScanForAttrib( &tmp_abbrev,  &tmp_entry, DW_AT_address_class )
        ==  DW_AT_address_class ) {

        if( decname->type_ptr.head != NULL ) {
            spaceit = TRUE;
        }

        addr_class = DWRReadConstant( tmp_abbrev, tmp_entry );
        ListConcat( &( decname->type_ptr ), *AddressClasses[ addr_class] );

        if( spaceit ) {
            ListConcat( &( decname->type_ptr ), SpaceKwd );
        }
    }

    return( decname );
}

static BrokenName_T *DecSubroutineType( BrokenName_T *decname, Loc_T *loc,
                                         dr_handle prev_tag )
/************************************************************************/
{
    List_T  parms;
    Node_T  target;

    if( prev_tag == DW_TAG_pointer_type ) {
        target = DeleteTail( &( decname->type_ptr ) );
        ListConcat( &( decname->var_plg ), SpaceKwd );
        ListAdd( &( decname->var_plg ), target );
        ListConcat( &( decname->var_plg ), FuncStartKwd );
    } else {
        ListConcat( &( decname->var_plg ), FuncStartKwd );
    }

    ListConcat( &( decname->var_elg ), FuncEndKwd );

    parms = StartFunctionParms( loc );

    if( decname->func_bas.tail == NULL ) {
        decname->func_bas.head = parms.head;
        decname->func_bas.tail = parms.tail;
    } else {
        decname->func_bas.tail->next = parms.head;
        decname->func_bas.tail = parms.tail;
    }

    return( decname );
}

static void AddTypeString( BrokenName_T *dn, String Kwd,
                           TypeSide_T ts)
/******************************************************/
{
    List_T  *list;

    switch( ts ) {
    case TYPE_PLG:
        list = &(dn->type_plg);
        break;

    case TYPE_BAS:
        list = &(dn->type_bas);
        break;

    case TYPE_PTR:
        list = &(dn->type_ptr);
        break;

    case TYPE_ELG:
        list = &(dn->type_elg);
        break;

    default:
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
        list = NULL;
    }

    ListConcat( list, Kwd );
}


/*
 * decorate a union, struct or class.
 */

BrokenName_T *DecorateCompoundType( BrokenName_T *decname, Loc_T *loc,
                                     String Kwd, dr_sym_type symtype )
/********************************************************************/
{
    String  typename;

    GrabName( loc->abbrev_cr, loc->entry_cr, &typename );

    ListConcat( &( decname->type_bas ), SpaceKwd );

    EndNode( &( decname->type_bas ), TRUE, loc->entry_st, symtype );
    ListConcat( &( decname->type_bas ), typename );
    EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    ListConcat( &( decname->type_bas ), Kwd );

    DWRFREE( typename.s );

    return( decname );
}

static int baseHook( dr_sym_type stype, dr_handle base, char *name,
                     dr_handle notused, void *info )
/*****************************************************************/
{
    BaseSearchInfo  *data;
    String          namestr;

    notused = notused;

    namestr.s = name;
    if( namestr.s == NULL ) {
        namestr.s = DWRALLOC( 1 );
        *namestr.s = '\0';
    }
    namestr.l = strlen( namestr.s );

    data = (BaseSearchInfo *) info;
    if( data->firstTime ) {
        data->firstTime = FALSE;
        ListConcat( &( data->decname->type_inh ), BaseKwd );
    } else {
        ListConcat( &( data->decname->type_inh ), BaseSepKwd );
    }
    EndNode( &( data->decname->type_inh ), TRUE, base, stype );
    ListConcat( &( data->decname->type_inh ), namestr );
    EndNode( &( data->decname->type_inh ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( namestr.s );

    return( TRUE );
}

BrokenName_T *DecorateBases( BrokenName_T *decname, Loc_T *loc )
/**************************************************************/
{
    BaseSearchInfo  data;

    data.decname = decname;
    data.firstTime = TRUE;
    DRBaseSearch( loc->entry_st, &data, baseHook );
    return( decname );
}


BrokenName_T *DecoratePtrToMember( BrokenName_T *decname, Loc_T *loc )
/********************************************************************/
{
    dr_handle tmp_abbrev;
    dr_handle tmp_entry;
    dr_handle containing_entry;
    String    containing_name;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    ListConcat( &( decname->var_plg ), SpaceKwd );
    ListConcat( &( decname->var_plg ), PtrKwd );
    ListConcat( &( decname->var_plg ), MemberKwd );

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_containing_type )
        == DW_AT_containing_type ) {

        containing_entry = DWRReadReference( tmp_abbrev, tmp_entry );

        containing_name.s = DRGetName( containing_entry );
        if( containing_name.s == NULL ) {
            containing_name.s = DWRALLOC( 1 );
            *containing_name.s = '\0';
        }

        containing_name.s = strrev( containing_name.s );
        containing_name.l = strlen( containing_name.s );

        EndNode( &( decname->var_plg ), TRUE, containing_entry, DR_SYM_CLASS );
        ListConcat( &( decname->var_plg ), containing_name );
        EndNode( &( decname->var_plg ), FALSE, 0, DR_SYM_NOT_SYM );

        DWRFREE( containing_name.s );
    }

    return( decname );
}

static bool AddArrayIndex( dr_handle abbrev, dr_handle entry, void *data )
/************************************************************************/
// add an array index to the name
{
    unsigned_32 upper_bd;
    unsigned_32 *dataptr;

    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_upper_bound )
        != DW_AT_upper_bound )
        return( FALSE );

    dataptr  = data;
    upper_bd = DWRReadConstant( abbrev, entry );
    *dataptr = upper_bd;
    return( TRUE );
}

static BrokenName_T *DecorateArray( BrokenName_T *decname, Loc_T *loc )
/*********************************************************************/
{
    dr_handle   abbrev;
    dr_handle   entry;
    dr_handle   type_entry;
    Loc_T       type_loc;
    String      tmpStr;
    unsigned_32 upper_bd;

    tmpStr.s = DWRALLOC( 1 );
    *tmpStr.s = '\0';
    tmpStr.l = 0;

    abbrev = loc->abbrev_cr;
    entry = loc->entry_cr;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_type ) != DW_AT_type ) {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    type_entry = DWRReadReference( abbrev, entry );
    type_entry =  SkipPCH( type_entry );
    FillLoc( &type_loc, type_entry );
    DecorateType( decname, &type_loc, DW_TAG_padding );

    abbrev = loc->abbrev_cr;
    entry = loc->entry_cr;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_count ) == DW_AT_count ) {
        upper_bd = DWRReadConstant( abbrev, entry );
    } else {
        DWRSkipRest( abbrev, &entry );
        DWRAllChildren( entry, AddArrayIndex, &upper_bd );
    }
    if( upper_bd != 0 ) {
        String      bounds;
        char        buf[ 15 ];      /* "[ 2147483647 ]\0" */
        char        indx[ 12 ];     /* "2147483647" */

        buf[ 0 ] = '\0';
        strncat( buf, ArrayLeftKwd.s, ArrayLeftKwd.l );
        ltoa( upper_bd, indx, 10 );
        strcat( buf, indx );
        strncat( buf, ArrayRightKwd.s, ArrayRightKwd.l );

        bounds.s = buf;
        bounds.l = strlen( buf );
        tmpStr.l += bounds.l;
        ReallocStr( &tmpStr );
        strncat( tmpStr.s, bounds.s, bounds.l );
    }
    strrev( tmpStr.s );

    ListConcat( &(decname->var_elg), tmpStr );

    DWRFREE( tmpStr.s );

    return( decname );
}

static void FORDecVariable( BrokenName_T *decname, Loc_T *loc )
/*************************************************************/
{
    String      varname;
    dr_handle   tmp_entry;
    dr_handle   tmp_abbrev;
    bool        inParam;
    dr_handle   type_die;
    Loc_T       type_loc;

    inParam = loc->inParam;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    GrabName( tmp_abbrev, tmp_entry, &varname );

    EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_VARIABLE );
    ListConcat( &( decname->var_bas ), varname );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( varname.s );

    /*
     *check to see if this is a member of a containing die
     */
    tmp_abbrev = loc->abbrev_cr;    /* reset to start of die / abbrev */
    tmp_entry = loc->entry_cr;

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_containing_type )
                           == DW_AT_containing_type  ){
        dr_handle containing_die;

        containing_die = DWRReadReference( tmp_abbrev, tmp_entry );
        if( containing_die != DW_TAG_padding ) {
            String    containing_name;

            tmp_entry = containing_die;
            tmp_abbrev = DWRGetAbbrev( &tmp_entry );

            GrabName( tmp_abbrev, tmp_entry, &containing_name );
            if( containing_name.s != NULL ) {

                ListConcat( &( decname->var_bas ), MemberKwd );

                EndNode( &( decname->var_bas ), TRUE, containing_die, DR_SYM_CLASS );
                ListConcat( &( decname->var_bas ), containing_name );
                EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

                DWRFREE( containing_name.s );
            }
        }
    }

    /* check for type information */

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type )
        == DW_AT_type ) {

        type_die = DWRReadReference( tmp_abbrev, tmp_entry );
        type_die =  SkipPCH( type_die );
        if( type_die != DW_TAG_padding ) {
            FillLoc( &type_loc, type_die );
            type_loc.inParam = inParam;

            FORDecType( decname, &type_loc );
        }

        FORAddConstVal( decname, loc, &type_loc );
    }
}

static void FORAddConstVal( BrokenName_T *decname, Loc_T *loc, Loc_T *type_loc )
/******************************************************************************/
/* if a variable is constant, decorate as a parameter */
{
    dr_handle   tmp_entry;
    dr_handle   tmp_abbrev;
    unsigned_8  *buf;
    unsigned    form;
    unsigned    len;
    char        *charBuf;
#ifdef __WATCOMC__
    char        numBuf1[ 32 ];  // hold text for number
    char        numBuf2[ 32 ];  // hold text for number
#endif
    String      value;
    unsigned_32 encoding;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_const_value )
        == DW_AT_const_value ) {
        ListConcat( &( decname->type_plg ), FORParamKwd );

        form = DWRVMReadULEB128( &tmp_abbrev );
        if( form == DW_FORM_indirect ) {
            form = DWRVMReadULEB128( &tmp_entry );
        }

        switch( form ) {
        case DW_FORM_block1:
            len = DWRVMReadByte( tmp_entry );
            tmp_entry += sizeof(unsigned_8);
            ReadBlock( &buf, tmp_entry, len );
            break;
        case DW_FORM_block2:
            len = DWRVMReadWord( tmp_entry );
            tmp_entry += sizeof(unsigned_16);
            ReadBlock( &buf, tmp_entry, len );
            break;
        case DW_FORM_block4:
            len = DWRVMReadWord( tmp_entry );
            tmp_entry += sizeof(unsigned_32);
            ReadBlock( &buf, tmp_entry, len );
            break;
        case DW_FORM_block:
            len = DWRVMReadULEB128( &tmp_entry );
            ReadBlock( &buf, tmp_entry, len );
            break;
        default:
            DWREXCEPT( DREXCEP_BAD_DBG_INFO );
            len = 0;
        }

        if( type_loc->tag == DW_TAG_string_type ) {
            charBuf = DWRALLOC( len + 3 );      // 2 * "'" + \0
            if( charBuf == NULL ) {
                DWREXCEPT( DREXCEP_OUT_OF_MMEM );
            }
            charBuf[ 0 ] = '\'';
            strncat( charBuf + 1, (char *)buf, len );
            strcat( charBuf, "'" );
        } else {
            charBuf = DWRALLOC( 64 );
            if( charBuf == NULL ) {
                DWREXCEPT( DREXCEP_OUT_OF_MMEM );
            }

            tmp_abbrev = type_loc->abbrev_cr;
            tmp_entry = type_loc->entry_cr;
            if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_encoding ) == DW_AT_encoding ) {
                encoding = DWRReadConstant( tmp_abbrev, tmp_entry );
            } else {
                DWREXCEPT( DREXCEP_BAD_DBG_INFO );
                encoding = 0;
            }

            switch( encoding ) {
            case DW_ATE_address:
                // NYI
                break;
            case DW_ATE_boolean:
                if( *buf ) {
                    strcpy( charBuf, ".TRUE." );
                } else {
                    strcpy( charBuf, ".FALSE." );
                }
                break;
            case DW_ATE_complex_float:
                switch( len ) {
                case (2 * sizeof(float)):
#ifdef __WATCOMC__
                    gcvt( *(float *)buf, 5, numBuf1 );
                    gcvt( *((float *)buf + 1), 5, numBuf2 );
#else
                    sprintf( charBuf, "(%.5g,%.5g)", *(float *)buf, *((float *)buf + 1) );
#endif
                    break;
                case (2 * sizeof(double)):
#ifdef __WATCOMC__
                    gcvt( *(double *)buf, 5, numBuf1 );
                    gcvt( *((double *)buf + 1), 5, numBuf2 );
#else
                    sprintf( charBuf, "(%.5g,%.5g)", *(double *)buf, *((double *)buf + 1) );
#endif
                    break;
                default:
                    DWREXCEPT( DREXCEP_BAD_DBG_INFO );
                }
#ifdef __WATCOMC__
                strcpy( charBuf, "(" );
                strcat( charBuf, numBuf1 );
                strcat( charBuf, "," );
                strcat( charBuf, numBuf2 );
                strcat( charBuf, ")" );
#endif
                break;
            case DW_ATE_float:
                switch( len ) {
                case sizeof(float):
#ifdef __WATCOMC__
                    gcvt( *(float *)buf, 5, charBuf );
#else
                    sprintf( charBuf, "%.5g", *(float *)buf );
#endif
                    break;
                case sizeof(double):
#ifdef __WATCOMC__
                    gcvt( *(double *)buf, 5, charBuf );
#else
                    sprintf( charBuf, "%.5g", *(double *)buf );
#endif
                    break;
                default:
                    DWREXCEPT( DREXCEP_BAD_DBG_INFO );
                }
                break;
            case DW_ATE_signed:
                switch( len ) {
                case sizeof(signed_32):
                    ltoa( *(signed_32 *)buf, charBuf, 10 );
                    break;
                case sizeof(signed_16):
                    ltoa( *(signed_16 *)buf, charBuf, 10 );
                    break;
                case sizeof(signed_8):
                    ltoa( *(signed_8 *)buf, charBuf, 10 );
                    break;
                default:
                    DWREXCEPT( DREXCEP_BAD_DBG_INFO );
                }
                break;
            case DW_ATE_unsigned:
                switch( len ) {
                case sizeof(unsigned_32):
                    ultoa( *(unsigned_32 *)buf, charBuf, 10 );
                    break;
                case sizeof(unsigned_16):
                    ultoa( *(unsigned_16 *)buf, charBuf, 10 );
                    break;
                case sizeof(unsigned_8):
                    ultoa( *(unsigned_8 *)buf, charBuf, 10 );
                    break;
                default:
                    DWREXCEPT( DREXCEP_BAD_DBG_INFO );
                }
                break;
            case DW_ATE_unsigned_char:
            case DW_ATE_signed_char:
                charBuf[ 0 ] = '\'';
                charBuf[ 1 ] = *(unsigned_8 *)buf;
                charBuf[ 2 ] = '\'';
                charBuf[ 3 ] = '\'';
                break;
            default:
                DWREXCEPT( DREXCEP_BAD_DBG_INFO );
            }
        }

        value.s = strrev( charBuf );
        value.l = strlen( charBuf );
        ListConcat( &(decname->var_elg), value );
        ListConcat( &(decname->var_elg), FOREqualKwd );

        DWRFREE( buf );
        DWRFREE( charBuf );
    }
}

static void ReadBlock( unsigned_8 **buf, dr_handle entry, unsigned len )
/**********************************************************************/
{
    unsigned_8  *mbuf;

    *buf = DWRALLOC( len );
    if( *buf == NULL ) {
        DWREXCEPT( DREXCEP_OUT_OF_MMEM );
    }

    mbuf = *buf;
    DWRVMRead( entry, mbuf, len );
}

static void FORDecParam( BrokenName_T *decname, Loc_T *loc )
/**********************************************************/
{
    loc->inParam = TRUE;
    FORDecVariable( decname, loc );
}

static void FORDecMember( BrokenName_T *decname, Loc_T *loc )
/***********************************************************/
{
    String    tmp_str;

    FORDecVariable( decname, loc );

    ListConcat( &( decname->var_bas ), FORMemberKwd );
    tmp_str.s = DRGetName( loc->parent );
    if( tmp_str.s == NULL ) {
        tmp_str.s = DWRALLOC( 1 );
        *tmp_str.s = '\0';
    }
    tmp_str.s = strrev( tmp_str.s );
    tmp_str.l = strlen( tmp_str.s );
    EndNode( &( decname->var_bas ), TRUE, loc->parent, DR_SYM_CLASS );
    ListConcat( &( decname->var_bas ), tmp_str );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );
    DWRFREE( tmp_str.s );
}

static int FORAddParam( dr_handle entry, int index, void *data )
/**************************************************************/
// add an array index to the name
{
    List_T      *list;
    List_T      add;
    Loc_T       loc;

    BrokenName_T  decstruct = Empty_Broken_Name;

    index = index;
    list = (List_T *)data;

    FillLoc( &loc, entry );

    FORDecParam( &decstruct, &loc );
    add = FormList( &decstruct );

    if( list->head == NULL ) {
        list->head = add.head;
        list->tail = add.tail;
    } else {
        EndNode( list, FALSE, 0, DR_SYM_NOT_SYM );
        ListConcat( list, FORParmSepKwd );
        list->tail->next = add.head;
        list->tail = add.tail;
    }

    return( TRUE );
}

static void FORDecSubprogram( BrokenName_T *decname, Loc_T *loc )
/***************************************************************/
{
    static unsigned_16 WalkTags[] = { DW_TAG_formal_parameter, 0 };
    static DRWLKBLK     WalkFns[] = { &FORAddParam, NULL };

    String      func_name;
    List_T      parms = { NULL, NULL, LIST_TAIL };
    dr_handle   tmp_abbrev;
    dr_handle   tmp_entry;
    Loc_T       type_loc;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    GrabName( tmp_abbrev, tmp_entry, &func_name );

    if( strcmp( func_name.s, FORMainProgMatch ) == 0 ) {
        EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_FUNCTION );
        ListConcat( &( decname->var_bas ), func_name );
        EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

        DWRFREE( func_name.s );

        return;
    }

    EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_FUNCTION );
    ListConcat( &( decname->var_bas ), func_name );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( func_name.s );

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) == DW_AT_type ) {
        dr_handle type_entry = DWRReadReference( tmp_abbrev, tmp_entry );

        type_entry =  SkipPCH( type_entry );
        if( type_entry != DW_TAG_padding ) {
            FillLoc( &type_loc, type_entry );
            FORDecType( decname, &type_loc );
        }
        ListConcat( &decname->var_plg, FORFuncKwd );
    } else {
        ListConcat( &decname->var_plg, FORSubprogramKwd );
    }

    DWRWalkChildren( loc->entry_st, WalkTags, WalkFns, &parms );

    if( parms.head != NULL ) {
        ListConcat( &decname->func_plg, FORFuncStartKwd );
        ListConcat( &decname->func_elg, FORFuncEndKwd );
        decname->func_bas.head = parms.head;
        decname->func_bas.tail = parms.tail;
    }
}

static void FORDecEntryPoint( BrokenName_T *decname, Loc_T *loc )
/***************************************************************/
{
    String      func_name;
    List_T      parms = { NULL, NULL, LIST_TAIL };
    dr_handle   tmp_abbrev;
    dr_handle   tmp_entry;
    unsigned_16 WalkTags[] = { DW_TAG_formal_parameter, 0 };
    DRWLKBLK    WalkFns[] = { &FORAddParam, NULL };

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    GrabName( tmp_abbrev, tmp_entry, &func_name );

    EndNode( &( decname->var_bas ), TRUE, loc->entry_st, DR_SYM_FUNCTION );
    ListConcat( &( decname->var_bas ), func_name );
    EndNode( &( decname->var_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( func_name.s );

    ListConcat( &decname->var_plg, FOREntryPointKwd );

    DWRSkipRest( tmp_abbrev, &tmp_entry );
    DWRWalkChildren( loc->entry_st, WalkTags, WalkFns, &parms );

    if( parms.head != NULL ) {
        ListConcat( &decname->func_plg, FORFuncStartKwd );
        ListConcat( &decname->func_elg, FORFuncEndKwd );
        decname->func_bas.head = parms.head;
        decname->func_bas.tail = parms.tail;
    }
}

static void FORDecStructure( BrokenName_T *decname, Loc_T *loc )
/**************************************************************/
{
    String  strucName;

    GrabName( loc->abbrev_cr, loc->entry_cr, &strucName );

    ListConcat( &( decname->type_bas ), SpaceKwd );
    ListConcat( &( decname->type_bas ), FORSlashKwd );

    EndNode( &( decname->type_bas ), TRUE, loc->entry_st, DR_SYM_CLASS );
    ListConcat( &( decname->type_bas ), strucName );
    EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    ListConcat( &( decname->type_bas ), FORSlashKwd );
    ListConcat( &( decname->type_bas ), FORStructKwd );

    DWRFREE( strucName.s);
}

static int FORAddNameListItem( dr_handle entry, int index, void *data )
/*********************************************************************/
{
    BrokenName_T    *decname = (BrokenName_T *)data;
    String          itemName;
    Loc_T           loc;
    dr_handle       mod = entry;
    dr_handle       abbrev;
    dr_handle       item;

    index = index;
    abbrev = DWRGetAbbrev( &mod );
    if( DWRScanForAttrib( &abbrev, &mod, DW_AT_namelist_item ) != DW_AT_namelist_item ) {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    item = DWRReadReference( abbrev, mod );

    FillLoc( &loc, item );

    if( decname->type_elg.head != NULL ) {
        ListConcat( &( decname->type_elg ), FORSepKwd );
    }

    GrabName( loc.abbrev_cr, loc.entry_cr, &itemName );
    strrev( itemName.s );

    EndNode( &( decname->type_elg ), TRUE, loc.entry_st, DR_SYM_VARIABLE );
    ListConcat( &( decname->type_elg ), itemName );
    EndNode( &( decname->type_elg ), FALSE, 0, DR_SYM_NOT_SYM );

    DWRFREE( itemName.s );

    return( TRUE );
}

static void FORDecNameList( BrokenName_T *decname, Loc_T *loc )
/*************************************************************/
{
    static unsigned_16 WalkTags[] = { DW_TAG_namelist_item, 0 };
    static DRWLKBLK     WalkFns[] = { &FORAddNameListItem, NULL };

    String strucName;

    GrabName( loc->abbrev_cr, loc->entry_cr, &strucName );

    ListConcat( &( decname->type_bas ), SpaceKwd );
    ListConcat( &( decname->type_bas ), FORSlashKwd );

    EndNode( &( decname->type_bas ), TRUE, loc->entry_st, DR_SYM_CLASS );
    ListConcat( &( decname->type_bas ), strucName );
    EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    ListConcat( &( decname->type_bas ), FORSlashKwd );
    ListConcat( &( decname->type_bas ), FORNameListKwd );

    DWRFREE( strucName.s);

    DWRWalkChildren( loc->entry_st, WalkTags, WalkFns, decname );
}

static void FORDecRecord( BrokenName_T *decname, Loc_T *loc )
/***********************************************************/
{
    String  strucName;

    GrabName( loc->abbrev_cr, loc->entry_cr, &strucName );

    ListConcat( &( decname->type_bas ), SpaceKwd );
    ListConcat( &( decname->type_bas ), FORSlashKwd );

    EndNode( &( decname->type_bas ), TRUE, loc->entry_st, DR_SYM_CLASS );
    ListConcat( &( decname->type_bas ), strucName );
    EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    ListConcat( &( decname->type_bas ), FORSlashKwd );
    ListConcat( &( decname->type_bas ), FORRecordKwd );

    DWRFREE( strucName.s);
}

static void FORDecUnion( BrokenName_T *decname, Loc_T *loc )
/**********************************************************/
{
    ListConcat( &( decname->type_bas ), SpaceKwd );

    EndNode( &( decname->type_bas ), TRUE, loc->entry_st, DR_SYM_CLASS );
    ListConcat( &( decname->type_bas ), FORUnionKwd );
    EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );
}

typedef struct FORArrIndexInf_S {
    String  *bounds;
    bool    inParam;
} FORArrIndexInf;

static bool FORAddArrayIndex( dr_handle abbrev, dr_handle entry, void *data )
/***************************************************************************/
// add an array index to the name
{
    dr_handle       tmp_abbrev;
    dr_handle       tmp_entry;
    signed_32       upper_bd;
    signed_32       lower_bd;
    bool            inParam;
    String          *bounds;
    String const    *add;
    char            buf[ 64 ];      /* "-4294967295:4294967295\0" */

    bounds = ((FORArrIndexInf *)data)->bounds;
    inParam = ((FORArrIndexInf *)data)->inParam;

    tmp_abbrev = abbrev;
    tmp_entry = entry;
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_lower_bound )
                != DW_AT_lower_bound ) {
        lower_bd = 1;   // default for FORTRAN
    } else {
        lower_bd = (signed_32)DWRReadConstant( tmp_abbrev, tmp_entry );
    }

    tmp_abbrev = abbrev;
    tmp_entry = entry;
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_upper_bound )
                != DW_AT_upper_bound ) {
        return( TRUE ); // not a subrange type
    } else {
        upper_bd = (signed_32)DWRReadConstant( tmp_abbrev, tmp_entry );
    }

    /* if empty, add "(" */
    if( bounds->l == 0 ) {
        add = &FORArrayLeftKwd;
    } else {
        add = &FORSepKwd;
    }
    bounds->l += add->l;
    ReallocStr( bounds );
    strncat( bounds->s, add->s, add->l );

    buf[ 0 ] = '\0';

    if( upper_bd < lower_bd ) {
        if( inParam ) {
            add = &FORAssumeArrKwd;
        } else {
            add = &FORAllocArrKwd;
        }
        bounds->l += add->l;
        ReallocStr( bounds );
        strncat( bounds->s, add->s, add->l );
    } else {
        if( lower_bd != 1 ) {
            ltoa( lower_bd, buf, 10 );
            bounds->l += strlen( buf ) + 1;
            ReallocStr( bounds );
            strcat( bounds->s, buf );
            strcat( bounds->s, ":" );
        }
        ltoa( upper_bd, buf, 10 );
        bounds->l += strlen( buf );
        ReallocStr( bounds );
        strcat( bounds->s, buf );
    }

    return( TRUE );
}

static void FORDecArray( BrokenName_T *decname, Loc_T *loc )
/**********************************************************/
{
    dr_handle       abbrev;
    dr_handle       entry;
    dr_handle       type_entry;
    Loc_T           type_loc;
    String          tmpStr;
    FORArrIndexInf  data;

    data.bounds = &tmpStr;
    data.inParam = loc->inParam;

    tmpStr.s = DWRALLOC( 1 );
    *tmpStr.s = '\0';
    tmpStr.l = 0;

    abbrev = loc->abbrev_cr;
    entry = loc->entry_cr;
    if( DWRScanForAttrib( &abbrev, &entry, DW_AT_type ) != DW_AT_type ) {
        DWREXCEPT( DREXCEP_BAD_DBG_INFO );
    }
    type_entry = DWRReadReference( abbrev, entry );
    type_entry =  SkipPCH( type_entry );
    FillLoc( &type_loc, type_entry );
    FORDecType( decname, &type_loc );

    DWRSkipRest( abbrev, &entry );
    DWRAllChildren( entry, FORAddArrayIndex, &data );

    tmpStr.l += FORArrayRightKwd.l;
    ReallocStr( &tmpStr );
    strncat( tmpStr.s, FORArrayRightKwd.s, FORArrayRightKwd.l );

    strrev( tmpStr.s );

    ListConcat( &(decname->var_elg), tmpStr );

    DWRFREE( tmpStr.s );
}

static void FORDecType( BrokenName_T *decname, Loc_T *loc )
/*********************************************************/
{
    dr_handle   tmp_entry;
    dr_handle   tmp_abbrev;
    dr_handle   next_die;
    String      typename;
    bool        done = FALSE;   /* true when the end of the type chain is reached */

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;

    switch( loc->tag ) {
    case DW_TAG_string_type:
        FORDecString( decname, loc );
        break;

    case DW_TAG_base_type:
        GrabName( tmp_abbrev, tmp_entry, &typename );

        ListConcat( &( decname->type_bas ), SpaceKwd );
        ListConcat( &( decname->type_bas ), typename );
        DWRFREE( typename.s );
        done = TRUE;

        break;

    case DW_TAG_structure_type:
        FORDecRecord( decname, loc );
        break;

    case DW_TAG_common_block:
        FORDecCommon( decname, loc );
        break;

    case DW_TAG_union_type:
        FORDecUnion( decname, loc );
        break;

    case DW_TAG_array_type:
        FORDecArray( decname, loc );
        done = TRUE;
        break;

    default:
        DWREXCEPT ( DREXCEP_DWARF_LIB_FAIL );
    }

    if( !done ) {
        tmp_abbrev = loc->abbrev_cr;
        tmp_entry = loc->entry_cr;
        if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_type ) == DW_AT_type ) {
            next_die = DWRReadReference( tmp_abbrev, tmp_entry );

            next_die =  SkipPCH( next_die );
            if( next_die != DW_TAG_padding ) {
                FillLoc( loc, next_die );
                FORDecType( decname, loc );
            }
        }
    }
}

static void FORDecString( BrokenName_T *decname, Loc_T *loc )
/***********************************************************/
{
    uint_32     len = 0;
    dr_handle   tmp_abbrev;
    dr_handle   tmp_entry;
    char        buf[ 64 ];  // "(2147483647)"
    char        size[ 62 ]; // "2147483647"
    String      sizeExpr;

    tmp_abbrev = loc->abbrev_cr;
    tmp_entry = loc->entry_cr;
    if( DWRScanForAttrib( &tmp_abbrev, &tmp_entry, DW_AT_byte_size )
        == DW_AT_byte_size ) {
        len = DWRReadConstant( tmp_abbrev, tmp_entry );
    }

    strcpy( buf, "(" );
    if( len ) {
        ltoa( len, size, 10 );
    } else {
        strcpy( size, "*" );
    }
    strcat( buf, size );
    strcat( buf, ")" );

    sizeExpr.s = strrev( buf );
    sizeExpr.l = strlen( buf );

    ListConcat( &( decname->type_bas ), SpaceKwd );
    ListConcat( &( decname->type_bas ), sizeExpr );
    ListConcat( &( decname->type_bas ), FORStringKwd );

}

static void FORDecCommon( BrokenName_T *decname, Loc_T *loc )
/***********************************************************/
{
    String commonName;

    GrabName( loc->abbrev_cr, loc->entry_cr, &commonName );

    ListConcat( &( decname->type_bas ), SpaceKwd );
    ListConcat( &( decname->type_bas ), FORSlashKwd );

    EndNode( &( decname->type_bas ), TRUE, loc->entry_st, DR_SYM_CLASS );
    ListConcat( &( decname->type_bas ), commonName );
    EndNode( &( decname->type_bas ), FALSE, 0, DR_SYM_NOT_SYM );

    ListConcat( &( decname->type_bas ), FORSlashKwd );
    ListConcat( &( decname->type_bas ), FORCommonKwd );

    DWRFREE( commonName.s);
}

/*
 * form a name from a BrokenName_T structure.
 * the new name is malloc'ed, and all other strings in the struct are freed
 */
static String FormName( BrokenName_T *decname )
/*********************************************/
{
    String name = { NULL, 0 };
    List_T list;
    Node_T curr;

    list = FormList( decname );
    for( curr = list.head; curr != NULL; curr = curr->next ) {
        name.l += curr->buf.l;
    }

    ReallocStr( &name );

    for( curr = list.head; curr != NULL; curr = curr->next ) {
        strncat( name.s, curr->buf.s, curr->buf.l );
        DWRFREE( curr->buf.s );
    }

    FreeList( list );
    return( name );
}

/*
 * take two lists and tack on on the end of the other, doing strrev as
 * appropriate
 */

static List_T *ListTack( List_T *addto, List_T *add )
/***************************************************/
{
    Node_T curr = NULL;
    if( add->tail != NULL ) {
        if( add->end == LIST_HEAD ) {
            for( curr = add->head; curr != NULL; curr = curr->next ) {
                if( curr->buf.s != NULL ) {
                    curr->buf.s = strrev( curr->buf.s );
                }
            }
        }

        if( addto->tail == NULL ) {
            addto->head = add->head;
            addto->tail = add->tail;
        } else {
            addto->tail->next = add->head;
            addto->tail = add->tail;
        }
    }
    return( addto );
}

/*
 * take a BrokenName_T structure, and build it into a single linked list
 */

static List_T FormList( BrokenName_T *decname )
/*********************************************/
{
    List_T list = { NULL, NULL, LIST_HEAD };

    ListTack( &list, &( decname->dec_plg ));

    ListTack( &list, &( decname->type_plg ));
    ListTack( &list, &( decname->type_bas ));
    ListTack( &list, &( decname->type_ptr ));

    /* goofy code to get a space after a pointer ... */

    if( decname->type_ptr.tail != NULL ) {
        list.tail->next = DWRALLOC( sizeof( *(list.tail->next) ) );
        if( list.tail->next == NULL ) {
            DWREXCEPT( DREXCEP_OUT_OF_MMEM );
        }

        list.tail->next->next = NULL;
        list.tail->next->user_def = FALSE;
        list.tail->next->entry = 0;
        list.tail->next->buf.l = SpaceKwd.l;
        list.tail->next->buf.s = NULL;
        ReallocStr( &( list.tail->next->buf ) );
        strcpy( list.tail->next->buf.s, SpaceKwd.s );
        list.tail = list.tail->next;
    }

    ListTack( &list, &( decname->type_elg ));
    ListTack( &list, &( decname->type_inh ));

    ListTack( &list, &( decname->var_plg ));
    ListTack( &list, &( decname->var_bas ));
    ListTack( &list, &( decname->var_elg ));

    ListTack( &list, &( decname->func_plg ));
    ListTack( &list, &( decname->func_bas ));
    ListTack( &list, &( decname->func_elg ));

    return( list );
}

/*
 * fill in the location structure for a given entry
 */

static void FillLoc( Loc_T *loc, dr_handle die )
/**********************************************/
{
    dr_handle abbrev_idx;

    loc->entry_st = die;
    loc->entry_cr = die;
    abbrev_idx = DWRVMReadULEB128( &loc->entry_cr );

    if( abbrev_idx != 0 ) {
        loc->abbrev_st = DWRLookupAbbrev( loc->entry_cr, abbrev_idx );
        loc->abbrev_cr = loc->abbrev_st;
        loc->tag = DWRVMReadULEB128( &( loc->abbrev_cr ) );
        loc->child = DWRVMReadByte( loc->abbrev_cr );
        loc->abbrev_cr += sizeof( unsigned_8 );    /* skip child byte */
        loc->inParam = FALSE;
    } else {
        loc->entry_st = 0;
        loc->entry_cr = 0;
        loc->abbrev_st = 0;
        loc->abbrev_cr = 0;
        loc->tag = 0;
        loc->child = 0;
        loc->inParam = FALSE;
    }
}

/*
 * for each node in a list, call the callback function.
 */

static void IterateList( void (* cb)( void *, char *, int,
                                      dr_handle, dr_sym_type ),
                         void *obj, List_T list )
/*************************************************************/
{
    Node_T curr;

    for( curr = list.head; curr != NULL; curr = curr->next ) {
        if( curr->buf.s ) {
            cb( obj, curr->buf.s, curr->user_def,
                curr->entry, curr->sym_type );
        }
    }
    FreeList( list );
    cb( obj, NULL, 0, (dr_handle) 0, DR_SYM_NOT_SYM );
}

/*
 * free all the allocated memory in the list - leave strings for client
 */

static void FreeList( List_T list )
/*********************************/
{
    Node_T curr;
    Node_T tmp;

    for( curr = list.head; curr != NULL; ) {
        tmp = curr;

        curr = curr->next;

        DWRFREE( tmp );
    }
}

/*
 * safe string reallocation -- causes exception on memory fail, and
 * makes newly allocated strings empty with a '\0'.  Zero-length strings
 * are represented by allocated memory of length 1.
 */

static void ReallocStr( String *str )
/***********************************/
{
    if( str->s == NULL ) {
        str->s = DWRALLOC( str->l + 1 );
        if( str->s == NULL ) {
            DWREXCEPT( DREXCEP_OUT_OF_MMEM );
        }

        *str->s = '\0';
    } else {
        if( strlen( str->s ) != str->l ) {
            str->s = DWRREALLOC( str->s, str->l + 1 );
            if( str->s == NULL ) {
                DWREXCEPT( DREXCEP_OUT_OF_MMEM );
            }
        }
    }
}

/*
 * Add a string onto the end of the tail node of a given list.  The
 * node's string is reallocated.  If a NULL list is used, a node is allocated
 * and a string allocated to it.
 */
static void ListConcat( List_T *list, String str )
/************************************************/
{
    String * strptr;
    Node_T newnode;

    if( str.s == NULL ) {
        return;
    }

    if( list->head == NULL || list->tail == NULL ) {

        newnode = DWRALLOC( sizeof( *newnode ) );
        if( newnode == NULL ) {
            DWREXCEPT( DREXCEP_OUT_OF_MMEM );
        }

        newnode->next = NULL;
        newnode->user_def = FALSE;
        newnode->entry = 0;
        newnode->buf.l = 0;
        newnode->buf.s = 0;

        list->head = list->tail = newnode;
    }

    switch( list->end ) {
    case LIST_HEAD:
        strptr = &( list->head->buf );
        break;

    case LIST_TAIL:
        strptr = &( list->tail->buf );
        break;

    default:
        DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
        strptr = NULL;
    }

    strptr->l += str.l;
    ReallocStr( strptr );
    strncat( strptr->s, str.s, str.l );
}

/*
 * add a node onto the appropriate end of a list
 */
static void ListAdd( List_T *list, Node_T node )
/**********************************************/
{
        switch( list->end ) {
        case LIST_HEAD:
            node->next = list->head;
            list->head = node;
            if( list->tail == NULL ) {
                list->tail = list->head;
            }
            break;

        case LIST_TAIL:
            if( list->tail != NULL ) {
                list->tail->next = node;
                list->tail = node;
            } else {
                list->tail = node;
                list->head = list->tail;
            }
            break;

        default:
            DWREXCEPT( DREXCEP_DWARF_LIB_FAIL );
        }
}

/*
 * Finish the current node and allocate a new one.  u_def is true if the
 * new node is user defined.
 */
static void EndNode( List_T * list, bool u_def,
                     dr_handle entry, dr_sym_type st )
/****************************************************/
{
    Node_T newnode;

    /* try to prevent unecessary allocations if this node is unused
     */

    if( list->end == LIST_HEAD && list->head != NULL && list->head->buf.s == NULL ) {
        list->head->user_def = u_def;
        list->head->entry = entry;
        list->head->sym_type = st;
    } else if( list->end == LIST_TAIL && list->tail != NULL && list->tail->buf.s == NULL ) {
        list->tail->user_def = u_def;
        list->tail->entry = entry;
        list->tail->sym_type = st;
    } else {
        newnode = DWRALLOC( sizeof( *newnode ) );
        if( newnode == NULL ) {
            DWREXCEPT( DREXCEP_OUT_OF_MMEM );
        }

        newnode->next = NULL;
        newnode->user_def = u_def;
        newnode->entry = entry;
        newnode->sym_type = st;
        newnode->buf.l = 0;
        newnode->buf.s = 0;

        ListAdd( list, newnode );
    }
}

/*
 * remove the node from the tail of a list, and return it.  This correctly
 * patches the tail of the list to be NULL.
 */

static Node_T  DeleteTail( List_T *list )
/***************************************/
{
    Node_T  target;
    Node_T  currnode;

    target = list->tail;
    if( target == NULL ) {
        return( NULL );
    }

    if( list->head == target ) {
        list->head = NULL;
        list->tail = NULL;
    } else {
        for( currnode = list->head; currnode != NULL;
             currnode = currnode->next ) {

          if( currnode->next == target ) break;

        }
        currnode->next = NULL;
        list->tail = currnode;
    }

    return( target );
}
