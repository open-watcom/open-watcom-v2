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


#include "cgstd.h"
#include "cgdefs.h"
#include "coderep.h"
#include "ocentry.h"
#include "objrep.h"
#include "system.h"
#include "jumps.h"
#include "zoiks.h"
#include "cgauxinf.h"
#include "i86obj.h"
#include "data.h"
#include "utils.h"
#include "objout.h"
#include "namelist.h"
#include "feprotos.h"

extern  void            OutPatch(label_handle,patch_attr);
extern  void            TellKeepLabel(label_handle);
extern  void            OutReloc(segment_id,fix_class,bool);
extern  void            OutImport(cg_sym_handle,fix_class,bool);
extern  void            OutBckImport( const char *name, back_handle bck, fix_class class );
extern  void            CodeBytes(const byte*,uint);
extern  void            CodeLabel(label_handle, unsigned);
extern  void            OutIBytes(byte,offset);
extern  void            OutDataLong(unsigned_32);
extern  void            OutDataShort(unsigned_16);
extern  void            OutDBytes(unsigned,const byte*);
extern  void            SetUpObj(bool);
extern  void            TellObjNewLabel( cg_sym_handle );
extern  void            TellOptimizerByPassed( void );
extern  void            TellByPassOver( void );
extern  void            GenSelEntry(bool);
extern  void            EmptyQueue( void );
extern  bool            UseImportForm( fe_attr );
extern  void            IterBytes( offset len, byte pat );

extern  void            OutLblPatch( label_handle lbl, fix_class class, offset plus );
static  void            DoLblPtr( label_handle lbl, segment_id seg, fix_class class, offset plus );


extern  void    DataAlign( unsigned_32 align ) {
/**********************************************/

    offset      curr_loc;
    uint        modulus;

    curr_loc = AskLocation();
    modulus = curr_loc % align;
    if( modulus != 0 ) {
        if( AskSegBlank( AskOP() ) == FALSE ) {
            IterBytes( (offset)( align - modulus ), 0 );
        } else {
            IncLocation( align - modulus );
        }
    }
}

extern  void    DataBytes( unsigned len, const void *src ) {
/**********************************************************/

    if( len != 0 ){
        TellOptimizerByPassed();
        SetUpObj( TRUE );
        OutDBytes( len, src );
        TellByPassOver();
    }
}


extern  void    DataShort( unsigned_16 val )
/******************************************/
{
    TellOptimizerByPassed();
    SetUpObj( TRUE );
    OutDataShort( val );
    TellByPassOver();
}

extern  void    DataLong( unsigned_32 val )
/*****************************************/
{
    TellOptimizerByPassed();
    SetUpObj( TRUE );
    OutDataLong( val );
    TellByPassOver();
}


extern  void    IterBytes( offset len, byte pat ) {
/***********************************************/

    TellOptimizerByPassed();
    SetUpObj( TRUE );
    OutIBytes( pat, len );
    TellByPassOver();
}


extern  void    DoBigBckPtr( back_handle bck, offset off ) {
/********************************************************/

/* Careful! Make sure a DGLabel has been done first! */

    TellOptimizerByPassed();
    DoLblPtr( bck->lbl, bck->seg, F_PTR, off );
    TellByPassOver();
}

static  void    DoLblPtr( label_handle lbl, segment_id seg, fix_class class, offset plus )
/****************************************************************************************/
{
    SetUpObj( TRUE );
    TellKeepLabel( lbl );
    OutReloc( seg, class, FALSE );
    OutLblPatch( lbl, class, plus );
}


extern  void    DoBigLblPtr( cg_sym_handle sym ) {
/*********************************************/

    TellOptimizerByPassed();
    DoLblPtr( FEBack( sym )->lbl, FESegID( sym ), F_PTR, 0 );
    TellByPassOver();
}



static  void    DoImpPtr( cg_sym_handle sym, fix_class class, offset plus ) {
/***********************************************************************/

    SetUpObj( TRUE );
    OutImport( sym, class, FALSE );
    if( F_CLASS( class ) == F_BASE ) {
        _OutFarSeg( 0 );
    } else {
        _OutFarOff( plus );
        if( F_CLASS( class ) == F_PTR ) {
            _OutFarSeg( 0 );
        }
    }
}

extern  void    BackImpPtr( const char *nm, back_handle bck, offset plus ) {
/************************************************************************/
    fix_class const class = F_OFFSET;

    SetUpObj( TRUE );
    OutBckImport( nm, bck, class );
    if( class == F_BASE ) {
        _OutFarSeg( 0 );
    } else {
        _OutFarOff( plus );
        if( class == F_PTR ) {
            _OutFarSeg( 0 );
        }
    }
}

extern  void    OutLblPatch( label_handle lbl, fix_class class, offset plus )
/***************************************************************************/
{
    offset      val;

    val = AskAddress( lbl );
    if( val == ADDR_UNKNOWN ) {
        OutPatch( lbl, ADD_PATCH | _OFFSET_PATCH );
        val = 0;
    }
    if( F_CLASS( class ) == F_BASE ) {
        _OutFarSeg( 0 );
    } else {
        if( F_CLASS( class ) == F_BIG_OFFSET ) {
            OutDataLong( val + plus );
        } else {
            _OutFarOff( val + plus );
        }
        if( F_CLASS( class ) == F_PTR ) {
            _OutFarSeg( 0 );
        }
    }
}


extern  void    FEPtr( cg_sym_handle sym, type_def *tipe, offset plus ) {
/*******************************************************************/

    fe_attr     attr;
    fix_class   class;

    TellOptimizerByPassed();
    attr = FEAttr( sym );
    if( ( attr & FE_PROC ) && _IsTargetModel( WINDOWS )
      && (*(call_class *)FindAuxInfoSym( sym, CALL_CLASS ) & FAR_CALL) ) {
        class = F_LDR_OFFSET;
    } else {
        class = F_OFFSET;
    }
    if( tipe->length != WORD_SIZE ) {
        class = F_PTR;
    }
    if( (attr & (FE_PROC | FE_DLLIMPORT)) == (FE_PROC | FE_DLLIMPORT) && (tipe->attr & TYPE_CODE) ) {
        class |= F_ALT_DLLIMP;
    }
    if( UseImportForm( attr ) ) {
        DoImpPtr( sym, class, plus );
    } else {
        DoLblPtr( FEBack( sym )->lbl, FESegID( sym ), class, plus );
    }
    TellByPassOver();
}

extern  void    FEPtrBaseOffset( cg_sym_handle sym,  offset plus ) {
/***************************************************************/

    fe_attr     attr;

    TellOptimizerByPassed();
    attr = FEAttr( sym );
    if( UseImportForm( attr ) ) { /* 90-05-22 */
        DoImpPtr( sym, F_PTR, plus );
    } else {
        DoLblPtr( FEBack( sym )->lbl, FESegID( sym ), F_PTR, plus );
    }
    TellByPassOver();
}

extern  void    FEPtrBase( cg_sym_handle sym ) {
/*******************************************/

    fe_attr     attr;

    TellOptimizerByPassed();
    attr = FEAttr( sym );
    if( UseImportForm( attr ) ) {
        DoImpPtr( sym, F_BASE, 0 );
    } else {
        DoLblPtr( FEBack( sym )->lbl, FESegID( sym ), F_BASE, 0 );
    }
    TellByPassOver();
}


extern  void    BackPtr( back_handle bck, segment_id seg, offset plus, type_def *tipe )
/*************************************************************************************/
{
    TellOptimizerByPassed();
    if( tipe->length != WORD_SIZE ) {
        DoLblPtr( bck->lbl, seg, F_PTR, plus );
    } else {
        DoLblPtr( bck->lbl, seg, F_OFFSET, plus );
    }
    TellByPassOver();
}

extern  void    BackBigOffset( back_handle bck, segment_id seg, offset plus )
/***************************************************************************/
{
    TellOptimizerByPassed();
    DoLblPtr( bck->lbl, seg, F_BIG_OFFSET, plus );
    TellByPassOver();
}

extern  void    BackPtrBase( back_handle bck, segment_id seg )
/**********************************************************/
{
    TellOptimizerByPassed();
    DoLblPtr( bck->lbl, seg, F_BASE, 0 );
    TellByPassOver();
}

extern  bool    FPCInCode( void ) {
/*********************************/

    return( _IsTargetModel( CONST_IN_CODE ) || ( _IsTargetModel( FLOATING_DS ) && _IsTargetModel( FLOATING_SS ) ) );
}

static  cg_class ConstDataClass( void ) {
/***************************************/

    if( FPCInCode() ) {
        return( CG_CLB );
    } else {
        return( CG_LBL );
    }
}

extern  name    *GenConstData( byte *buffer, type_class_def class ) {
/*******************************************************************/

    segment_id          old;
    cg_class            cgclass;
    name                *result;
    label_handle        label;
    type_length         size;

    TellOptimizerByPassed();
    cgclass = ConstDataClass();
    size = TypeClassSize[ class ];
    label = AskForLabel( NULL );
    if( cgclass == CG_CLB ) {
        old = SetOP( AskCodeSeg() );
        SetUpObj( TRUE );
        GenSelEntry( TRUE );
        CodeLabel( label, size );
        CodeBytes( buffer, size );
        GenSelEntry( FALSE );
    } else {
        old = SetOP( AskBackSeg() );
        SetUpObj( TRUE );
        DataAlign( size );
        OutLabel( label );
        DataBytes( size, buffer );
    }
    SetOP( old );
    TellByPassOver();
    result = AllocMemory( label, 0, cgclass, class );
    result->v.usage |= USE_IN_ANOTHER_BLOCK;
    return( result );
}

extern  name    *GenFloat( name *cons, type_class_def class ) {
/*************************************************************/

    constant_defn       *defn;
    name                *result;

    defn = GetFloat( cons, class );
    if( defn->label == NULL ) {
        result = GenConstData( (byte *)&defn->value, class );
        defn->label = result->v.symbol;
    } else {
        result = AllocMemory( defn->label, 0, ConstDataClass(), class );
    }
    return( result );
}


extern  void    DataLabel( label_handle lbl ) {
/*********************************************/

    TellObjNewLabel( AskForLblSym( lbl ) );
    TellOptimizerByPassed();
    SetUpObj( TRUE );
    OutLabel( lbl );
    TellByPassOver();
}
