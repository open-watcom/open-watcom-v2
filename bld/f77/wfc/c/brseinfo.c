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
* Description:  Generate browsing information.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <malloc.h>
#include "ftnstd.h"
#include "errcod.h"
#include "global.h"
#include "cpopt.h"
#include "progsw.h"
#include "brseinfo.h"
#include "sdfile.h"
#include "dw.h"
#include "astype.h"
#include "browscli.h"
#include "fmemmgr.h"
#include "types.h"
#include "ferror.h"

// linked list storage facility
typedef struct sym_list {
    struct sym_list     *link;                  // link
    sym_id              id;                     // symbol entry id
    dw_handle           dbh;                    // dwarf handle
    struct sym_list     *children;              // children of this entry
} sym_list;

typedef dw_handle (*func)( sym_id ste_ptr, dw_handle handle );

extern  int             AllocName(int);
extern  int             MakeName(char *,char *,char *);
extern  char            *SDFName(char *fn);
static void             BIAdd2List(sym_list **,sym_id,dw_handle);
static void             BIWalkList(sym_list **,func,int);
static dw_handle        BIGetAnyType(sym_id);
static dw_handle        BIGetType(sym_id);
static dw_handle        BIGetSPType(sym_id);
static dw_handle        BIGetArrayType(sym_id);
static dw_handle        BIGetStructType(sym_id,dw_handle);
static dw_handle        BILateRefSym(sym_id,dw_handle);
static dw_handle        BIStartStructType(sym_id,int);
static dw_handle        BIGetUnionType(sym_id);
static void             BIOutSP(sym_id);
static void             BISolidifyFunction(sym_id,dw_handle);
static void             BIOutDummies(entry_pt *);
static void             BIOutDeclareSP(sym_id,long);
static void             BIOutSF(sym_id);
static void             BIOutSPDumInfo(sym_id);
static void             BIOutVar(sym_id);
static void             BIOutConst(sym_id);
static void             BIDumpAllEntryPoints(entry_pt *,int);
static dw_handle        BIGetHandle(sym_id);
static void             BISetHandle(sym_id,dw_handle);
static char *           BIMKFullPath(const char *);
static void             BIInitBaseTypes(void);

extern char             BrowseExtn[];

static dw_client        cBIId;
static dw_loc_handle    justJunk;
static char             fullPathName[ PATH_MAX + 1 ];
static dw_handle        subProgTyHandle;
static unsigned_32      currState = 0;
static sym_list         *fixStructs = NULL;
static sym_list         *fixSubParms = NULL;

static dw_handle        baseTypes[LAST_BASE_TYPE + 1];

/* Forward declarations */
static void BIRefSymbol( dw_handle handle );


#define BI_STATE_IN_COMMON_BLOCK        0x00000001
#define BI_STATE_IN_STMT_FUNC           0x00000002
#define BI_STATE_RESOLVED               0x00000004
#define BI_STATE_IN_SCOPE               0x00000008
#define BI_NAMELIST_UNDEFINED           0x00000010

static  bool            BrInitialized;

#if ( _CPU == 8086 )
 #define ARCHITECTURE   sizeof( short )
#else
 #define ARCHITECTURE   sizeof( long )
#endif

#define _GenerateBrInfo()     ((Options & OPT_BROWSE) && \
                                 (BrInitialized) && \
                                 (ProgSw & PS_DONT_GENERATE))

#define _isFundamentalType( typ ) \
                        (((int)typ >= FIRST_BASE_TYPE) && ((int)typ <= LAST_BASE_TYPE))

void    BIInit( void ) {
//================

    dw_init_info        init_dwl;
    dw_cu_info          cu;

    BrInitialized = TRUE;
    if( !_GenerateBrInfo() ) return;
    init_dwl.language = DWLANG_FORTRAN;
    init_dwl.compiler_options = DW_CM_BROWSER | DW_CM_UPPER;
    init_dwl.producer_name = "WATCOM FORTRAN 77";
    if ( !setjmp( init_dwl.exception_handler ) ) {
        CLIInit( &(init_dwl.funcs), MEM_SECTION );
        cBIId = DWInit( &init_dwl );
        justJunk = DWLocFini( cBIId, DWLocInit( cBIId ) );
        cu.source_filename=BIMKFullPath( CurrFile->name );
        cu.directory=".";
        cu.flags = TRUE;
        cu.offset_size = ARCHITECTURE;
        cu.segment_size = 0;
        cu.model = DW_MODEL_NONE;
        cu.inc_list = NULL;
        cu.inc_list_len = 0;
        cu.dbg_pch = 0;
        DWBeginCompileUnit( cBIId, &cu );
        BISetSrcFile();
    } else {
        BrInitialized = FALSE;
        Error( SM_BROWSE_ERROR );
    }
    BIInitBaseTypes();
}

void    BIEnd( void ) {
//===============

    char        fn[MAX_FILE+1];

    if( !_GenerateBrInfo() ) return;
    MakeName( SDFName( SrcName ), BrowseExtn, fn );
    DWEndCompileUnit( cBIId );
    DWLocTrash( cBIId, justJunk );
    DWFini( cBIId );
    CLIDump( fn );
    CLIClear();
}

void    BIStartSubProg( void ) {
//========================
}

void    BIStartSubroutine( void ) {
//===========================

    if( _GenerateBrInfo() ) {
        if ( ( SubProgId->ns.flags & SY_SUBPROG_TYPE ) != SY_BLOCK_DATA ){
            BIOutSrcLine();
            BIOutSP( SubProgId );
        }
        currState |= BI_STATE_IN_SCOPE;
    }
}

void    BIFiniStartOfSubroutine( void ) {
//=================================

    if( _GenerateBrInfo() && (currState & BI_STATE_IN_SCOPE) ) {
        if ( ( SubProgId->ns.flags & SY_SUBPROG_TYPE ) == SY_FUNCTION ){
            BISolidifyFunction( SubProgId, subProgTyHandle );
        }
        if ( ( SubProgId->ns.flags & SY_SUBPROG_TYPE ) != SY_BLOCK_DATA ){
            if ( currState & BI_STATE_IN_SCOPE ) {
                BIOutDummies( Entries );
            }
        }
        BIWalkList( &fixSubParms, &BILateRefSym, TRUE );
        currState |= BI_STATE_RESOLVED;
    }
}

void    BIEndBlockData( void ) {
//==============================

    if( _GenerateBrInfo() ) {
        currState &= ~BI_STATE_IN_COMMON_BLOCK;
        DWEndLexicalBlock ( cBIId );
    }
}

void    BIEndSubProg( void ) {
//======================

    if( _GenerateBrInfo() ) {
        if ( ( SubProgId->ns.flags & SY_SUBPROG_TYPE ) == SY_BLOCK_DATA ) {
            BIEndBlockData();
        } else {
            BIDumpAllEntryPoints( Entries, 0 );
            DWEndSubroutine ( cBIId );
        }
        currState &= ~( BI_STATE_IN_SCOPE | BI_STATE_RESOLVED );
    }
}

void BIResolveUndefTypes( void ) {
//==========================

    if( _GenerateBrInfo() ) {
        BIWalkList( &fixStructs, &BIGetStructType, TRUE );
    }
}

void    BIEndSF( sym_id ste_ptr ) {
//=================================

    if( _GenerateBrInfo() ) {
        DWEndSubroutine ( cBIId );
        BIRefSymbol( BIGetHandle( ste_ptr ) );
        currState &= ~BI_STATE_IN_STMT_FUNC;
    }
}

void    BIStartRBorEP( sym_id ste_ptr ) {
//=======================================

    if( _GenerateBrInfo() ) {
        BIOutSP( ste_ptr );
        if ( ste_ptr->ns.flags & SY_SENTRY ) {
            BIOutDummies( ArgList );
        }
    }
}

void    BIEndRBorEP( void ) {
//===========================

    if( _GenerateBrInfo() ) {
        DWEndSubroutine ( cBIId );
    }
}

void    BIStartComBlock( sym_id ste_ptr ) {
//=========================================

    char        name[MAX_SYMLEN+1];

    if( _GenerateBrInfo() ) {
        memset( name, 0, MAX_SYMLEN+1 );
        DWDeclPos( cBIId, CurrFile->rec, 0 );
        currState |= BI_STATE_IN_COMMON_BLOCK;
        DWIncludeCommonBlock( cBIId, DWBeginCommonBlock( cBIId, justJunk, 0,
            strncpy( name, ste_ptr->ns.name, ste_ptr->ns.name_len ), 0 ) );

    }
}

void    BIEndComBlock( void ) {
//=============================

    if( _GenerateBrInfo() ) {
        currState &= ~BI_STATE_IN_COMMON_BLOCK;
        DWEndCommonBlock ( cBIId );
    }
}

void    BIStartBlockData( sym_id ste_ptr ) {
//==========================================

    char        name[MAX_SYMLEN+1];

    if( _GenerateBrInfo() ) {
        memset( name, 0, MAX_SYMLEN+1 );
        DWDeclPos( cBIId, CurrFile->rec, 0 );
        DWBeginLexicalBlock( cBIId, 0,
                strncpy( name, ste_ptr->ns.name, ste_ptr->ns.name_len ) );
    }
}

void    BIOutComSymbol( sym_id ste_ptr ) {
//========================================

    if( _GenerateBrInfo() ) {
        BIOutVar( ste_ptr );
    }
}

void    BIOutNameList( sym_id ste_ptr ) {
//=======================================

    char        name[33];
    grp_entry   *ge;
    dw_handle   var;

    if( _GenerateBrInfo() ) {
        if( !( ste_ptr->nl.dbh ) ) {
            strncpy( name, ste_ptr->nl.name, ste_ptr->nl.name_len );
            name[ste_ptr->nl.name_len] = 0;
            BIOutSrcLine();
            ge = ste_ptr->nl.group_list;
            while( ge != NULL ) {
                var = BIGetHandle( ge->sym );
                if( !var ) {
                    BIOutSymbol( ge->sym );
                }
                ge = ge->link;
            }
            ste_ptr->nl.dbh = DWNameListBegin( cBIId, name );
            ge = ste_ptr->nl.group_list;
            while( ge != NULL ) {
                DWNameListItem( cBIId, BIGetHandle( ge->sym ) );
                ge = ge->link;
            }
            DWEndNameList( cBIId );
        }
        BIRefSymbol( ste_ptr->nl.dbh );
    }
}

void    BIOutSymbol( sym_id ste_ptr ) {
//=====================================

// define/declare/reference a symbol

    dw_handle   temp;

    if( !_GenerateBrInfo() ) return;
    if (  !( currState & BI_STATE_IN_SCOPE ) ) {
        BISetHandle( ste_ptr, 0 );
        return;
    }
    BIOutSrcLine();
    if ( !( ste_ptr->ns.flags & SY_REFERENCED ) ) {
        if ( ( ste_ptr->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            if((ste_ptr->ns.flags & SY_SUBPROG_TYPE)==SY_STMT_FUNC ) {
                if( ( ASType & AST_ASF ) &&
                        !( currState & BI_STATE_IN_STMT_FUNC ) ) { //if defining s.f.
                    BIOutSF( ste_ptr );
                } else {
                    BIRefSymbol( BIGetHandle( ste_ptr ) );
                }
            } else if((ste_ptr->ns.flags & SY_SUBPROG_TYPE)==SY_REMOTE_BLOCK){
                    BIOutDeclareSP( ste_ptr, 0 );
            } else {
                    if ( !( ste_ptr->ns.flags & SY_PS_ENTRY ) ) {
                        BIOutDeclareSP( ste_ptr, DW_FLAG_GLOBAL );
                    } else {
                        BIRefSymbol( BIGetHandle( ste_ptr ) );
                    }
            }
        } else if ( ( ste_ptr->ns.flags & SY_CLASS ) == SY_VARIABLE ) {
                if ( ste_ptr->ns.flags & SY_SUB_PARM ) {
                    if ( currState & BI_STATE_RESOLVED ) {
                        BIRefSymbol( BIGetHandle( ste_ptr ) );
                    } else {
                        BIAdd2List( &fixSubParms, ste_ptr, CurrFile->rec );
                    }
                } else if ( !( ste_ptr->ns.flags & SY_SPECIAL_PARM ) ) {
                    if ( !( ste_ptr->ns.flags & SY_IN_COMMON ) ||
                       ( currState & BI_STATE_IN_COMMON_BLOCK ) ) {
                        BIOutVar( ste_ptr );
                    } else if ( !( ste_ptr->ns.flags &
                                        ( SY_DATA_INIT | SY_IN_DIMEXPR ) ) ||
                                !( ste_ptr->ns.flags & SY_IN_COMMON ) ) {
                        BIRefSymbol( BIGetHandle( ste_ptr ) );
                    }
                } else if ( currState & BI_STATE_IN_STMT_FUNC ) {
                    BIRefSymbol( BIGetHandle( ste_ptr->ns.si.ms.sym ) );
                }
        } else if ( ( ste_ptr->ns.flags & SY_CLASS ) == SY_PARAMETER ) {
            BIOutConst( ste_ptr );
        }
    } else {
        // Do we need to use the magic symbol when referencing?
        if ( ( currState & BI_STATE_IN_STMT_FUNC ) &&
             ( ste_ptr->ns.flags & SY_SPECIAL_PARM ) ) {
            BIRefSymbol( BIGetHandle( ste_ptr->ns.si.ms.sym ) );
        } else if ( ( ste_ptr->ns.flags & SY_SUB_PARM ) &&
                  ( !( currState & BI_STATE_RESOLVED ) ) ) {
            BIAdd2List( &fixSubParms, ste_ptr, CurrFile->rec );
        } else {
            temp = BIGetHandle( ste_ptr );
            if ( temp ) {
                BIRefSymbol( temp );
            } else {
                // Consider:    data ( x(i), i=1,3 ) ...
                //              do 666 i = 1, 3
                //               ....
                //      666      continue
                // The variable has yet to be declared since it
                // was first referenced before the sub prog definition
                // so we must turn of the reference bit,
                // Dump thhe symbol
                // and set the bit on again
                ste_ptr->ns.flags &= ~SY_REFERENCED;
                BIOutSymbol( ste_ptr );
                ste_ptr->ns.flags |= SY_REFERENCED;
            }
        }
    }

}

void BISetSrcFile( void ) {
//===================

// Set Current Source Line

    char        *name;

    if( _GenerateBrInfo() ) {
        if ( !( ProgSw & PS_FATAL_ERROR ) && CurrFile ) {
            name = BIMKFullPath( CurrFile->name );
            DWSetFile( cBIId, name );
            DWDeclFile( cBIId, name );
            DWLineNum( cBIId, DW_LN_DEFAULT, CurrFile->rec, 0, 0 );
            DWDeclPos( cBIId, CurrFile->rec, 0 );
        }
    }
}

void BIOutSrcLine( void ) {
//===================

// Set Current Source Line

    if( _GenerateBrInfo() ) {
        DWLineNum( cBIId, DW_LN_DEFAULT, SrcRecNum, 0, 0 );
        DWDeclPos( cBIId, CurrFile->rec, 0 );
    }
}


static dw_handle BIGetHandle( sym_id ste_ptr) {
//=============================================

    return( ste_ptr->ns.dbh );
}


static void BISetHandle( sym_id ste_ptr, dw_handle handle ) {
//===========================================================

    ste_ptr->ns.dbh = handle;
}


static void BIRefSymbol( dw_handle handle ) {
//===========================================

    if( _GenerateBrInfo() ) {
        DWReference( cBIId, SrcRecNum, 0, handle );
    }
}


static dw_handle BILateRefSym( sym_id ste_ptr, dw_handle handle ) {
//=================================================================

        unsigned_32     temp = SrcRecNum;

        SrcRecNum = handle;
        BIRefSymbol( BIGetHandle( ste_ptr ) );
        SrcRecNum = temp;
        return( 0 );
}


static void BIOutDummies( entry_pt *dum_lst ) {
//=============================================

    parameter           *curr_parm;

    if ( !dum_lst ) {
        return;
    }
    curr_parm = dum_lst->parms;
    while( curr_parm != NULL ) {
        if ( !( curr_parm->flags & ARG_STMTNO ) ) {
            BIOutSPDumInfo( curr_parm->id );
        }
        curr_parm = curr_parm->link;
    }
}


static void BIDumpAllEntryPoints( entry_pt *dum_lst, int level ) {
//================================================================

    // Close up all entry points for the subprogram.
    // This is done recursively since scope might be a factor
    // in the future

    if ( !dum_lst ) {
        return;
    }
    BIDumpAllEntryPoints( dum_lst->link, level + 1 );
    if ( level ) {
        BIEndRBorEP();
    }
}


static void BIOutSP( sym_id ste_ptr ) {
//=====================================

// Dump the subprogram.
    char        name[MAX_SYMLEN+1];
    uint        flags = 0;
    dw_handle   fret;

    DWDeclPos( cBIId, CurrFile->rec, 0 );
    if ( ( ste_ptr->ns.flags & SY_SUBPROG_TYPE ) == SY_FUNCTION ) {
        if( ste_ptr->ns.flags & SY_SENTRY ) {
            fret = subProgTyHandle;
        } else {
            if ( ste_ptr->ns.typ == FT_STRUCTURE ) {
                fret = BIStartStructType( ste_ptr, FALSE );
            } else {
                fret = DWHandle( cBIId, DW_ST_NONE );
            }
            subProgTyHandle = fret;
        }
    } else {
        fret = BIGetSPType( ste_ptr );
    }
    memset( name, 0, MAX_SYMLEN+1 );
    strncpy( name, ste_ptr->ns.name, ste_ptr->ns.name_len );
    if ( ste_ptr->ns.flags & SY_SENTRY ) {
        fret =    DWBeginEntryPoint( cBIId, fret, justJunk, 0, name, 0, flags );
    }else{
        if ( ( ste_ptr->ns.flags & SY_SUBPROG_TYPE ) == SY_PROGRAM ) {
            flags |= DW_FLAG_MAIN;
        }
        fret =    DWBeginSubroutine( cBIId, 0, fret, justJunk, 0, 0,
                    0, 0, name,
                    0, flags );

    }
    BISetHandle( ste_ptr, fret );
}


static int BIMapType( TYPE typ ) {
//===============================

// Map our type to a DWARF fundamental type

    switch( typ ) {
    case( FT_LOGICAL_1 ):
    case( FT_LOGICAL ):         return( DW_FT_BOOLEAN );
    case( FT_INTEGER_1 ):
    case( FT_INTEGER_2 ):
    case( FT_INTEGER ):         return( DW_FT_SIGNED );
    case( FT_REAL ):
    case( FT_DOUBLE ):
    case( FT_TRUE_EXTENDED ):   return( DW_FT_FLOAT );
    case( FT_COMPLEX ):
    case( FT_DCOMPLEX ):
    case( FT_TRUE_XCOMPLEX ):   return( DW_FT_COMPLEX_FLOAT );
    case( FT_CHAR ):            return( DW_FT_UNSIGNED_CHAR );
    }
    return( 0 );
}


static dw_handle BIMakeFundamental( TYPE typ ) {
//=============================================

// create a new fundamental handle seperate from the one created at birth

    return( DWFundamental(cBIId, TypeKW(typ), BIMapType(typ), TypeSize(typ)) );
}


static void BISolidifyFunction( sym_id ste_ptr, dw_handle handle ) {
//==================================================================

//  solidify the function type;

    if ( ste_ptr->ns.typ != FT_STRUCTURE ) {
        DWHandleSet( cBIId, handle );
    }
    if( _isFundamentalType( ste_ptr->ns.typ ) ) {
        // since we now emit our fundamentals at init time, we must explicitly
        // create another fundemntal handle rather than using the ones created
        // at birth.  This is necessary because we must set next handle emitted
        // to that type
        BIMakeFundamental( ste_ptr->ns.typ );
    } else {
        BIGetSPType( ste_ptr );
    }
}


static void BIOutDeclareSP( sym_id ste_ptr, long flags ) {
//========================================================

// Dump the name of an external or intrinsic function. and its data

    char                name[MAX_SYMLEN+1];
    dw_handle           handle;

    if ( ste_ptr == SubProgId ) return;
    flags |= DW_FLAG_DECLARATION;

    memset( name, 0, MAX_SYMLEN+1 );
    handle = DWBeginSubroutine( cBIId, 0, BIGetSPType( ste_ptr ), justJunk,
                 0, 0, 0, 0, strncpy( name, ste_ptr->ns.name,
                                        ste_ptr->ns.name_len ), 0, flags );
    DWEndSubroutine ( cBIId );
    BISetHandle( ste_ptr, handle );
    BIRefSymbol( handle );
}


static void BIOutSF( sym_id ste_ptr ) {
//=====================================

// Dump the name of a statement function. and its data

    sf_parm             *tmp;

    BIOutSP( ste_ptr );
    tmp = ste_ptr->ns.si.sf.header->parm_list;
    while( tmp ) {
        BIOutSPDumInfo( tmp->actual );
        tmp = tmp->link;
    }
    currState |= BI_STATE_IN_STMT_FUNC;
}


static void BIOutSPDumInfo( sym_id ste_ptr ) {
//============================================

// Dump the name of a subprogram dummy argument.

    dw_handle           handle;
    char                name[MAX_SYMLEN+1];

    memset( name, 0, MAX_SYMLEN+1 );
    handle = DWFormalParameter( cBIId, BIGetAnyType( ste_ptr ), 0, 0,
                                strncpy( name, ste_ptr->ns.name,
                                        ste_ptr->ns.name_len ),
                                DW_DEFAULT_NONE );
    BIRefSymbol( handle );
    BISetHandle( ste_ptr, handle );
}


static void BIOutVar( sym_id ste_ptr ) {
//======================================

// Dump the name of a variable.

    dw_handle           handle;
    char                name[MAX_SYMLEN+1];

    memset( name, 0, MAX_SYMLEN+1 );
    handle = DWVariable(cBIId, BIGetAnyType(ste_ptr), 0, 0, 0,
                        strncpy(name, ste_ptr->ns.name, ste_ptr->ns.name_len ),
                        0, 0 );
    BIRefSymbol( handle );
    BISetHandle( ste_ptr, handle );
}


static void BIOutConst( sym_id ste_ptr ) {
//========================================

// Dump the name of a variable.

    dw_handle           handle;
    char                name[MAX_SYMLEN+1];
    void                *value;

    if ( ste_ptr->ns.typ == FT_CHAR ) {
        value = &(ste_ptr->ns.si.pc.value->lt.value);
    } else {
        value = &(ste_ptr->ns.si.pc.value->cn.value);
    }

    memset( name, 0, MAX_SYMLEN+1 );
    handle = DWConstant(cBIId, BIGetAnyType(ste_ptr), value,
                        ste_ptr->ns.xt.size, 0,
                        strncpy(name, ste_ptr->ns.name, ste_ptr->ns.name_len),
                        0, 0 );
    BIRefSymbol( handle );
    BISetHandle( ste_ptr, handle );
}


static dw_handle        BIGetAnyType( sym_id ste_ptr ) {
//======================================================

// return any and all type

    if ( ste_ptr->ns.flags & SY_SUBSCRIPTED ) {
        return( BIGetArrayType( ste_ptr ) );
    } else {
        return( BIGetType( ste_ptr ) );
    }
}


static dw_handle BIGetSPType( sym_id ste_ptr ) {
//==============================================

    switch ( ste_ptr->ns.flags & SY_SUBPROG_TYPE ) {
    case( SY_SUBROUTINE ) :
    case( SY_REMOTE_BLOCK ):
    case( SY_BLOCK_DATA ) :     return( 0 );
    case( SY_FUNCTION ) :
    case( SY_PROGRAM ) :
    case( SY_STMT_FUNC ) :
    case( SY_FN_OR_SUB ) :
        if ( ( ste_ptr->ns.typ == FT_STRUCTURE ) &&
            !( ste_ptr->ns.xt.record ) ) {
            return( BIStartStructType( ste_ptr, TRUE ) );
        }
        return( BIGetType( ste_ptr ) );
    }
    return( 0 );
}


static dw_handle BIGetType( sym_id ste_ptr ) {
//============================================

// Get the Symbol's NON COMPOUND DWARF TYPE,

    TYPE        typ = ste_ptr->ns.typ;
    dw_handle   ret = 0;

    switch( typ ) {
    case( FT_LOGICAL_1 ):
    case( FT_LOGICAL ):
    case( FT_INTEGER_1 ):
    case( FT_INTEGER_2 ):
    case( FT_INTEGER ):
    case( FT_HEX ):
    case( FT_REAL ):
    case( FT_DOUBLE ):
    case( FT_TRUE_EXTENDED ):
    case( FT_COMPLEX ):
    case( FT_DCOMPLEX ):
    case( FT_TRUE_XCOMPLEX ):
        ret = baseTypes[ typ ];
        break;
    case( FT_CHAR ):
        ret = DWString(cBIId, 0, ste_ptr->ns.xt.size, "", 0, 0);
        break;
    case( FT_UNION ):
        ret = BIGetUnionType( ste_ptr );
        break;
    case( FT_STRUCTURE ):
        ret = BIGetStructType( ste_ptr, 0 );
        break;
    }
    DWDeclPos( cBIId, CurrFile->rec, 0 );
    return( ret );
}


static dw_handle BIGetBaseType( TYPE typ ) {
//=========================================

// Get initialized base type

    DWDeclPos( cBIId, CurrFile->rec, 0 );
    return( baseTypes[ typ ] );
}


static dw_handle BIGetArrayType( sym_id ste_ptr ) {
//=================================================

// Get An array type of a named symbol

    int         dim = _DimCount( ste_ptr->ns.si.va.dim_ext->dim_flags );
    int         x = 0;
    dw_dim_info data;
    intstar4    *sub;
    dw_handle   ret;

    data.index_type = BIGetBaseType( FT_INTEGER );
    sub = &( ste_ptr->ns.si.va.dim_ext->subs_1_lo );

    ret = DWBeginArray( cBIId, BIGetType( ste_ptr ), 0, NULL, 0, 0 );
    for( x = 0; x < dim; x++ ) {
        data.lo_data = *sub++;
        data.hi_data = *sub++;
        DWArrayDimension( cBIId, &data );
    }
    DWDeclPos( cBIId, CurrFile->rec, 0 );
    DWEndArray( cBIId );
    return( ret );
}


static dw_handle BIGetStructType( sym_id ste_ptr, dw_handle handle ) {
//====================================================================

// get a struct type of a non named symbol

    struct field        *fields;
    dw_handle           ret;
    sym_id              data = alloca( sizeof(symbol) + AllocName(MAX_SYMLEN) );
    char                *name;
    char                buffer[MAX_SYMLEN+1];
    long                un = 0;

    if ( handle ) {
        ret = handle;
    } else if ( ste_ptr->ns.xt.record->dbh ) {
        return( ste_ptr->ns.xt.record->dbh );
    } else {
        // consider: record /bar/ function x()
        //                       ....
        // we want to use the handle we used when the function
        // was defined
        if ( ste_ptr->ns.xt.record == SubProgId->ns.xt.record ) {
            ret = subProgTyHandle;
        } else {
            ret = DWStruct( cBIId, DW_ST_STRUCT );
        }
    }
    ste_ptr->ns.xt.record->dbh = ret;
    DWDeclPos( cBIId, CurrFile->rec, 0 );
    memset( buffer, 0, MAX_SYMLEN+1 );
    DWBeginStruct( cBIId, ret, ste_ptr->ns.xt.record->size,
                        strncpy( buffer, ste_ptr->ns.xt.record->name,
                                ste_ptr->ns.xt.record->name_len ),
                        0, 0 );
    fields = ste_ptr->ns.xt.record->fl.fields;
    while( fields ) {
        data->ns.typ = fields->typ;
        data->ns.xt.record = fields->xt.record;
        name = NULL;
        if ( fields->typ == FT_UNION ) {
            data->ns.si.va.dim_ext = NULL;
            data->ns.name_len = 0;
            data->ns.name[0] = 0;
            un++;
            name = data->ns.name;
        } else {
            data->ns.si.va.dim_ext = fields->dim_ext;
            data->ns.name_len = fields->name_len;
            strncpy( data->ns.name, fields->name, fields->name_len );
            data->ns.name[ fields->name_len ] = 0;
            if ( *(data->ns.name) ) {
                name = data->ns.name;
            }
        }
        if ( data->ns.si.va.dim_ext ) {
            DWAddField(cBIId, BIGetArrayType(data), justJunk, name, 0);
        } else {
            DWAddField( cBIId, BIGetType( data ), justJunk, name, 0 );
        }
        fields = &fields->link->fd;
    }
    DWEndStruct( cBIId );
    return( ret );
}


static dw_handle BIGetUnionType( sym_id ste_ptr ) {
//=================================================

// get a union type of a non named symbol

    struct fstruct      *fs;
    dw_handle           ret;
    symbol              data;
    long                max = 0;
    long                map = 0;
    char                buff[12];

    ret = DWStruct( cBIId, DW_ST_UNION );
    fs = ste_ptr->ns.xt.record;
    // find the largest size of map
    while( fs ) {
        if ( fs->size > max ) {
            max = fs->size;
        }
        fs = &fs->link->sd;
    }

    // Start the union declaration
    DWDeclPos( cBIId, CurrFile->rec, 0 );
    DWBeginStruct( cBIId, ret, max, ste_ptr->ns.name, 0, 0 );
    fs = ste_ptr->ns.xt.record;
    data.ns.xt.record = FMemAlloc( sizeof( fstruct) );
    while( fs ) {
        memset( data.ns.xt.record, 0, sizeof( fstruct ) );
        memcpy( data.ns.xt.record, fs, sizeof( fmap ) );
        data.ns.si.va.dim_ext = NULL;
        data.ns.typ = FT_STRUCTURE;
        strcpy( data.ns.name, "MAP" );
        strcat( data.ns.name, itoa( map, buff, 10 ) );
        data.ns.name_len = strlen( data.ns.name );
        strcpy( data.ns.xt.record->name, data.ns.name );
        data.ns.xt.record->name_len = data.ns.name_len;
        map++;
        DWAddField( cBIId, BIGetType( &data ), justJunk, data.ns.name, 0 );
        fs = &fs->link->sd;
    }
    FMemFree( data.ns.xt.record );
    DWEndStruct( cBIId );
    return( ret );
}


static dw_handle BIStartStructType( sym_id ste_ptr, int add ) {
//=============================================================

// start a struct type of a function definition

    dw_handle   ret;

    ret = DWStruct( cBIId, DW_ST_STRUCT );
    if ( add ) {
        BIAdd2List( &fixStructs, ste_ptr, ret );
    }
    return( ret );
}


static void BIAdd2List( sym_list **list, sym_id ste_ptr, dw_handle handle ) {
//===========================================================================

    sym_list    *tmp;

    tmp = FMemAlloc( sizeof( sym_list ) );
    tmp->id = ste_ptr;
    tmp->dbh = handle;
    tmp->link = *list;
    tmp->children = NULL;
    *list = tmp;
}


static void BIWalkList( sym_list **list, func action, int nuke_list ) {
//=====================================================================

    sym_list    *tmp;

    tmp = *list;
    while( tmp ) {
        action( tmp->id, tmp->dbh );
        tmp = tmp->link;
        if ( nuke_list ) {
            FMemFree( *list );
            *list = tmp;
        }
    }
}


static char *BIMKFullPath( const char *path ) {
//=============================================

        return( _fullpath( fullPathName, path, PATH_MAX ) );
}


static void BIInitBaseTypes( void ) {
//===================================

    TYPE    x;

    // assume that LAST_BASE_TYPE is the last fundamental type
    // and types from FIRST_BASE_TYPE to LAST_BASE_TYPE are all fundamental
    // base types
    for( x = FIRST_BASE_TYPE; x <= LAST_BASE_TYPE; x++ ) {
        baseTypes[ x ] = BIMakeFundamental( x );
    }
}
