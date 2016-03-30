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
* Description:  F-Code I/O routines.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcdatad.h"
#include "wf77defs.h"
#include "wf77cg.h"
#include "wf77auxd.h"
#include "tmpdefs.h"
#include "rtconst.h"
#include "types.h"
#include "emitobj.h"
#include "fctypes.h"
#include "chain.h"
#include "fccmplx.h"
#include "fcflow.h"
#include "fcjmptab.h"
#include "fcstring.h"
#include "fcsubscr.h"
#include "fctemp.h"
#include "fcrtns.h"
#include "fcstack.h"
#include "cgswitch.h"
#include "cgprotos.h"


static  void            StructIOArrayStruct( sym_id arr );
static  void            StructIOItem( sym_id fd );

static  sym_id          EndEqStmt;
static  sym_id          ErrEqStmt;
static  void            (**IORtnTable)(void);
static  tmp_handle      TmpStructPtr;
static  bool            IOStatSpecified;
static  label_handle    IOSLabel;
static  bool            NmlSpecified;

/* Forward declarations */
void    FCChkIOStmtLabel( void );

static  void    ChrArrayIO( RTCODE rtn, cg_name arr, cg_name num_elts,
                            cg_name elt_size ) {
//====================================================================

    call_handle call;

    call = InitCall( rtn );
    CGAddParm( call, elt_size, TY_INTEGER );
    CGAddParm( call, num_elts, TY_INT_4 );
    CGAddParm( call, arr, TY_POINTER );
    CGDone( CGCall( call ) );
}


static  void    NumArrayIO( RTCODE rtn, cg_name arr, cg_name num_elts,
                            uint typ ) {
//====================================================================

    call_handle call;

    call = InitCall( rtn );
    CGAddParm( call, CGInteger( typ, TY_INTEGER ), TY_INTEGER );
    CGAddParm( call, num_elts, TY_INT_4 );
    CGAddParm( call, arr, TY_POINTER );
    CGDone( CGCall( call ) );
}


static  void    IOCall( RTCODE rtn ) {
//====================================

// Call i/o run-time routine with one argument.

    call_handle handle;

    handle = InitCall( rtn );
    CGAddParm( handle, XPop(), TY_POINTER );
    CGDone( CGCall( handle ) );
}


static  void    IOCallValue( RTCODE rtn ) {
//=========================================

// Call i/o run-time routine with one argument.

    call_handle handle;

    handle = InitCall( rtn );
    CGAddParm( handle, GetTypedValue(), TY_INT_4 );
    CGDone( CGCall( handle ) );
}


static  void    chkIOErr( cg_name io_stat ) {
//===========================================

// Check for i/o errors.

    label_handle        eq_label;

    io_stat = CGUnary( O_POINTS, io_stat, TY_INTEGER );
    if( ( EndEqLabel != 0 ) && ( ErrEqLabel != 0 ) ) {
        eq_label = BENewLabel();
        CG3WayControl( io_stat, GetLabel( EndEqLabel ), eq_label,
                       GetLabel( ErrEqLabel ) );
        CGControl( O_LABEL, NULL, eq_label );
        BEFiniLabel( eq_label );
    } else if( EndEqLabel != 0 ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_LT, io_stat, CGInteger( 0, TY_INTEGER ),
                              TY_INTEGER ),
                   GetLabel( EndEqLabel ) );
    } else if( ErrEqLabel != 0 ) {
        CGControl( O_IF_TRUE,
                   CGCompare( O_NE, io_stat, CGInteger( 0, TY_INTEGER ),
                              TY_INTEGER ),
                   GetLabel( ErrEqLabel ) );
    } else if( IOStatSpecified ) {
        IOSLabel = BENewLabel();
        CGControl( O_IF_TRUE,
                   CGCompare( O_NE, io_stat, CGInteger( 0, TY_INTEGER ),
                              TY_INTEGER ),
                   IOSLabel );
    } else {
        CGDone( io_stat );
    }
}


static  void    StructIO( struct field *fd ) {
//============================================

    sym_id      map;
    sym_id      big_map = NULL;
    unsigned_32 size;

    while( fd != NULL ) {
        if( fd->typ == FT_STRUCTURE ) {
            if( fd->dim_ext != NULL ) {
                StructIOArrayStruct( (sym_id)fd );
            } else {
                StructIO( fd->xt.record->fl.fields );
            }
        } else if( fd->typ == FT_UNION ) {
            size = 0;
            map = fd->xt.sym_record;
            while( map != NULL ) { // find biggest map
                if( map->u.sd.size > size ) {
                    size = map->u.sd.size;        // 91/08/01 DJG
                    big_map = map;
                }
                map = map->u.sd.link;
            }
            StructIO( big_map->u.sd.fl.fields );
        } else {
            StructIOItem( (sym_id)fd );
        }
        fd = &fd->link->u.fd;
    }
}


static  void    IOStatement( RTCODE stmt ) {
//==========================================

    // don't need label generated for IOSTAT unless it's a READ or WRITE
    // statement that is not NAMELIST-directed
    if( ( (stmt != RT_EX_READ) && (stmt != RT_EX_WRITE) ) || NmlSpecified ) {
        IOStatSpecified = FALSE;
    }
    chkIOErr( CGCall( InitCall( stmt ) ) );
}


static  void    Output( RTCODE rtn, cg_type arg_type ) {
//======================================================

// Call runtime routine to output elemental types value.

    call_handle handle;

    handle = InitCall( rtn );
    CGAddParm( handle, XPopValue( arg_type ), PromoteToBaseType( arg_type ) );
    CGDone( CGCall( handle ) );
}


static  void    Input( RTCODE rtn ) {
//===================================

// Common input routine.

    call_handle handle;

    handle = InitCall( rtn );
    CGAddParm( handle, XPop(), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCSetIOCB( void ) {
//===================

// Call runtime routine to set i/o statement in IOCB.
// This must be the first call when processing an I/O statement.

    EndEqLabel = 0;
    ErrEqLabel = 0;
    EndEqStmt = NULL;
    ErrEqStmt = NULL;
    IOStatSpecified = FALSE;
    NmlSpecified = FALSE;
}


void    FCSetUnit( void ) {
//===================

// Call runtime routine to set unit number in IOCB.

    IOCallValue( RT_SET_UNIT );
}


void    FCExRead( void ) {
//==================

// Call runtime routine to set start READ operation.

    IOStatement( RT_EX_READ );
}


void    FCExWrite( void ) {
//===================

// Call runtime routine to set start WRITE operation.

    IOStatement( RT_EX_WRITE );
}


void    FCExOpen( void ) {
//==================

// Call runtime routine to set start OPEN operation.

    IOStatement( RT_EX_OPEN );
}


void    FCExClose( void ) {
//===================

// Call runtime routine to set start CLOSE operation.

    IOStatement( RT_EX_CLOSE );
}


void    FCExBack( void ) {
//==================

// Call runtime routine to set start BACKSPACE operation.

    IOStatement( RT_EX_BACK );
}


void    FCExEndf( void ) {
//==================

// Call runtime routine to set start ENDFILE operation.

    IOStatement( RT_EX_ENDF );
}


void    FCExRew( void ) {
//=================

// Call runtime routine to set start REWIND operation.

    IOStatement( RT_EX_REW );
}


void    FCExInq( void ) {
//=================

// Call runtime routine to set start INQUIRE operation.

    IOStatement( RT_EX_INQ );
}


void    FCOutLOG1( void ) {
//===================

// Call runtime routine to output LOGICAL*1 value.

    Output( RT_OUT_LOG1, TY_UINT_1 );
}


void    FCOutLOG4( void ) {
//===================

// Call runtime routine to output LOGICAL*4 value.

    Output( RT_OUT_LOG4, TY_UINT_4 );
}


void    FCOutINT1( void ) {
//===================

// Call runtime routine to output INTEGER*1 value.

    Output( RT_OUT_INT1, TY_INT_1 );
}


void    FCOutINT2( void ) {
//===================

// Call runtime routine to output INTEGER*2 value.

    Output( RT_OUT_INT2, TY_INT_2 );
}


void    FCOutINT4( void ) {
//===================

// Call runtime routine to output INTEGER*4 value.

    Output( RT_OUT_INT4, TY_INT_4 );
}


void    FCOutREAL( void ) {
//===================

// Call runtime routine to output REAL*4 value.

    Output( RT_OUT_REAL, TY_SINGLE );
}


void    FCOutDBLE( void ) {
//===================

// Call runtime routine to output REAL*8 value.

    Output( RT_OUT_DBLE, TY_DOUBLE );
}


void    FCOutXTND( void ) {
//===================

// Call runtime routine to output REAL*10 value.

    Output( RT_OUT_XTND, TY_LONGDOUBLE );
}


static  void    OutCplx( RTCODE rtn, cg_type typ ) {
//===============================================

// Call runtime routine to input COMPLEX value.

    call_handle handle;
    cg_cmplx    z;

    handle = InitCall( rtn );
    XPopCmplx( &z, typ );
    typ = CmplxBaseType( typ );
    CGAddParm( handle, z.imagpart, typ );
    CGAddParm( handle, z.realpart, typ );
    CGDone( CGCall( handle ) );
}


void    FCOutCPLX( void ) {
//===================

// Call runtime routine to output COMPLEX*8 value.

    OutCplx( RT_OUT_CPLX, TY_COMPLEX );
}


void    FCOutDBCX( void ) {
//===================

// Call runtime routine to output COMPLEX*16 value.

    OutCplx( RT_OUT_DBCX, TY_DCOMPLEX );
}


void    FCOutXTCX( void ) {
//===================

// Call runtime routine to output COMPLEX*20 value.

    OutCplx( RT_OUT_XTCX, TY_XCOMPLEX );
}


void    FCOutCHAR( void ) {
//===================

// Call runtime routine to output CHARACTER*n value.

    call_handle handle;

    handle = InitCall( RT_OUT_CHAR );
    CGAddParm( handle, XPop(), TY_POINTER );
    CGDone( CGCall( handle ) );
}


static  void    IOString( RTCODE rtn ) {
//======================================

    call_handle handle;

    handle = InitCall( rtn );
    CGAddParm( handle, XPop(), TY_INTEGER );
    CGAddParm( handle, XPop(), TY_POINTER );
    CGDone( CGCall( handle ) );
}


static  void    OutString( void ) {
//===========================

// Call runtime routine to output CHARACTER*n value.
// Note: 2 arguments are passed (data pointer and length) as opposed to a
//       pointer to the SCB.

    IOString( RT_OUT_STR );
}


void    FCInpLOG1( void ) {
//===================

// Call runtime routine to input LOGICAL*1 value.

    Input( RT_INP_LOG1 );
}


void    FCInpLOG4( void ) {
//===================

// Call runtime routine to input LOGICAL*4 value.

    Input( RT_INP_LOG4 );
}


void    FCInpINT1( void ) {
//===================

// Call runtime routine to input INTEGER*1 value.

    Input( RT_INP_INT1 );
}


void    FCInpINT2( void ) {
//===================

// Call runtime routine to input INTEGER*2 value.

    Input( RT_INP_INT2 );
}


void    FCInpINT4( void ) {
//===================

// Call runtime routine to input INTEGER*4 value.

    Input( RT_INP_INT4 );
}


void    FCInpREAL( void ) {
//===================

// Call runtime routine to input REAL*4 value.

    Input( RT_INP_REAL );
}


void    FCInpDBLE( void ) {
//===================

// Call runtime routine to input REAL*8 value.

    Input( RT_INP_DBLE );
}


void    FCInpXTND( void ) {
//===================

// Call runtime routine to input REAL*10 value.

    Input( RT_INP_XTND );
}


void    FCInpCPLX( void ) {
//===================

// Call runtime routine to input COMPLEX*8 value.

    Input( RT_INP_CPLX );
}


void    FCInpDBCX( void ) {
//===================

// Call runtime routine to input COMPLEX*16 value.

    Input( RT_INP_DBCX );
}


void    FCInpXTCX( void ) {
//===================

// Call runtime routine to input COMPLEX*20 value.

    Input( RT_INP_XTCX );
}


void    FCInpCHAR( void ) {
//===================

// Call runtime routine to input CHARACTER*n value.

    Input( RT_INP_CHAR );
}


static  void    InpString( void ) {
//===========================

// Call runtime routine to input CHARACTER*n value.
// Note: 2 arguments are passed (data pointer and length) as opposed to a
//       pointer to the SCB.

    IOString( RT_INP_STR );
}


void    FCEndIO( void ) {
//=================

// Call runtime routine to terminate i/o processing.

    CGDone( CGCall( InitCall( RT_ENDIO ) ) );
    FCChkIOStmtLabel();
    if( ( ErrEqLabel == 0 ) && ( EndEqLabel == 0 ) && IOStatSpecified ) {
        CGControl( O_LABEL, NULL, IOSLabel );
        BEFiniLabel( IOSLabel );
    }
}

static  void            (*OutRtn[])(void) = {
    #define ONLY_BASE_TYPES
    #define pick(id,type,dbgtype,cgtype,inpfun,outfun,typnam) outfun,
    #include "ptypdefn.h"
    #undef pick
    #undef ONLY_BASE_TYPES
};

static  void            (*InpRtn[])(void) = {
    #define ONLY_BASE_TYPES
    #define pick(id,type,dbgtype,cgtype,inpfun,outfun,typnam) inpfun,
    #include "ptypdefn.h"
    #undef pick
    #undef ONLY_BASE_TYPES
};

void    FCOutStruct( void ) {
//=====================

// Output a structure.

    IORtnTable = OutRtn;
    TmpStructPtr = MkTmp( XPop(), TY_POINTER );
    StructIO( ((sym_id)GetPtr())->u.sd.fl.fields );
}


static  void    DoStructArrayIO( tmp_handle num_elts, struct field *fieldz ) {
//============================================================================

// Perform structure array i/o.

    label_handle        label;

    label = BENewLabel();
    CGControl( O_LABEL, NULL, label );
    StructIO( fieldz );
    CGControl( O_IF_TRUE,
               CGCompare( O_NE,
                          CGAssign( TmpPtr( num_elts, TY_INT_4 ),
                                    CGBinary( O_MINUS,
                                              TmpVal( num_elts, TY_INT_4 ),
                                              CGInteger( 1, TY_INTEGER ),
                                              TY_INT_4 ),
                                    TY_INT_4 ),
                          CGInteger( 0, TY_INTEGER ), TY_INT_4 ),
               label );
    BEFiniLabel( label );
}


static  void    StructIOArrayStruct( sym_id arr ) {
//=================================================

// Perform structure array i/o on a field.

    tmp_handle          num_elts;

    num_elts = MkTmp( FieldArrayNumElts( arr ), TY_INT_4 );
    DoStructArrayIO( num_elts, arr->u.fd.xt.record->fl.fields );
}


static  void    StructIOItem( sym_id fd ) {
//=========================================

// Perform i/o of structure field.

    RTCODE      rtn;

    if( fd->u.fd.dim_ext == NULL ) {
        XPush( TmpVal( TmpStructPtr, TY_POINTER ) );
        if( fd->u.fd.typ == FT_CHAR ) {
            XPush( CGInteger( fd->u.fd.xt.size, TY_INTEGER ) );
        }
        IORtnTable[ ParmType( fd->u.fd.typ, fd->u.fd.xt.size ) ]();
        CGTrash( CGAssign( TmpPtr( TmpStructPtr, TY_POINTER ),
                           CGBinary( O_PLUS,
                                     TmpVal( TmpStructPtr, TY_POINTER ),
                                     CGInteger( fd->u.fd.xt.size, TY_UINT_4 ),
                                     TY_POINTER ),
                           TY_POINTER ) );
    } else {
        if( IORtnTable == OutRtn ) {
            rtn = RT_PRT_ARRAY;
        } else {
            rtn = RT_INP_ARRAY;
        }
        if( fd->u.fd.typ == FT_CHAR ) {
            ChrArrayIO( rtn + 1, TmpVal( TmpStructPtr, TY_POINTER ),
                        CGInteger( fd->u.fd.dim_ext->num_elts, TY_INT_4 ),
                        CGInteger( fd->u.fd.xt.size, TY_INTEGER ) );
        } else {
            NumArrayIO( rtn, TmpVal( TmpStructPtr, TY_POINTER ),
                        CGInteger( fd->u.fd.dim_ext->num_elts, TY_INT_4 ),
                        ParmType( fd->u.fd.typ, fd->u.fd.xt.size ) );
        }
        CGTrash( CGAssign( TmpPtr( TmpStructPtr, TY_POINTER ),
                           CGBinary( O_PLUS,
                                     TmpVal( TmpStructPtr, TY_POINTER ),
                                     CGInteger( fd->u.fd.xt.size *
                                                fd->u.fd.dim_ext->num_elts,
                                                TY_UINT_4 ),
                                     TY_POINTER ),
                           TY_POINTER ) );
    }
}


static  void    StructArrayIO( void ) {
//===============================

// Perform structure array i/o.

    sym_id              arr;
    tmp_handle          num_elts;

    arr = GetPtr();
    num_elts = MkTmp( ArrayNumElts( arr ), TY_INT_4 );
    TmpStructPtr = MkTmp( SymAddr( arr ), TY_POINTER );
    DoStructArrayIO( num_elts, arr->u.ns.xt.record->fl.fields );
}


void    FCPrtStructArray( void ) {
//==========================

    IORtnTable = OutRtn;
    StructArrayIO();
}


void    FCInpStructArray( void ) {
//==========================

    IORtnTable = InpRtn;
    StructArrayIO();
}


void    FCInpStruct( void ) {
//=====================

// Input a structure.

    IORtnTable = InpRtn;
    TmpStructPtr = MkTmp( XPop(), TY_POINTER );
    StructIO( ((sym_id)GetPtr())->u.sd.fl.fields );
}


void    FCChkIOStmtLabel( void ) {
//==========================

    if( EndEqStmt != NULL ) {
        RefStmtLabel( EndEqStmt );
    }
    if( ErrEqStmt != NULL ) {
        RefStmtLabel( ErrEqStmt );
    }
}


void    FCSetNml( void ) {
//==================

// Set NAMELIST format.

    call_handle handle;
    sym_id      nl;
    grp_entry   *ge;

    NmlSpecified = TRUE;
    handle = InitCall( RT_SET_NML );
    nl = GetPtr();
    ReverseList( (void **)&nl->u.nl.group_list );
    ge = nl->u.nl.group_list;
    while( ge != NULL ) {
        CGAddParm( handle, SymAddr( ge->sym ), TY_POINTER );
        ge = ge->link;
    }
    ReverseList( (void **)&nl->u.nl.group_list );
    CGAddParm( handle, CGBackName( nl->u.nl.address, TY_POINTER ), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCSetFmt( void ) {
//==================

// Set format string from FORMAT statement.

    call_handle handle;

    handle = InitCall( RT_SET_FMT );
    CGAddParm( handle, CGBackName( (back_handle)GetStmtLabel( GetPtr() ), TY_POINTER ), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCPassLabel( void ) {
//=====================

// Pass label to run-time routine.

    call_handle handle;

    handle = InitCall( GetU16() );
    CGAddParm( handle, CGBackName( (back_handle)GetLabel( GetU16() ), TY_POINTER ), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCFmtAssign( void ) {
//=====================

// Set FORMAT string for:
//       ASSIGN 10 TO I
//       PRINT I, ...
// 10    FORMAT( ... )

    call_handle handle;

    handle = InitCall( RT_SET_FMT );
    CGAddParm( handle, CGUnary( O_POINTS, SymAddr( GetPtr() ), TY_POINTER ), TY_POINTER );
    CGDone( CGCall( handle ) );
}


static void    ArrayIO( RTCODE num_array, RTCODE chr_array ) {
//=====================================================

// Output an array.

    sym_id      arr;
    sym_id      field;
    cg_name     addr;
    cg_name     num_elts;
    cg_name     elt_size;

    arr = GetPtr();
    field = GetPtr();
    if( field == NULL ) {
        addr = SymAddr( arr );
        num_elts = ArrayNumElts( arr );
        if( arr->u.ns.u1.s.typ == FT_CHAR ) {
            ChrArrayIO( chr_array, addr, num_elts, ArrayEltSize( arr ) );
        } else {
            NumArrayIO( num_array, addr, num_elts,
                        ParmType( arr->u.ns.u1.s.typ, arr->u.ns.xt.size ) );
        }
    } else { // must be a array field in a structure
        addr = XPop();
        num_elts = FieldArrayNumElts( field );
        if( field->u.fd.typ == FT_CHAR ) {
            elt_size = CGInteger( field->u.fd.xt.size, TY_INTEGER );
            ChrArrayIO( chr_array, addr, num_elts, elt_size );
        } else {
            NumArrayIO( num_array, addr, num_elts,
                        ParmType( field->u.fd.typ, field->u.fd.xt.size ) );
        }
    }
}


void    FCPrtArray( void ) {
//====================

// Output an array.

    ArrayIO( RT_PRT_ARRAY, RT_PRT_CHAR_ARRAY );
}


void    FCInpArray( void ) {
//====================

// Input an array.

    ArrayIO( RT_INP_ARRAY, RT_INP_CHAR_ARRAY );
}


void    FCFmtScan( void ) {
//===================

// Call runtime routine to scan a format specification from a character
// expression.

    call_handle handle;

    handle = InitCall( RT_FMT_SCAN );
    CGAddParm( handle, CGInteger( GetU16(), TY_UNSIGNED ), TY_UNSIGNED );
    CGAddParm( handle, XPop(), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCFmtArrScan( void ) {
//======================

// Call runtime routine to scan a format specification from a character
// array.

    call_handle handle;
    sym_id      sym;

    sym = GetPtr();
    handle = InitCall( RT_FMT_ARR_SCAN );
    CGAddParm( handle, CGInteger( GetU16(), TY_UNSIGNED ), TY_UNSIGNED );
    CGAddParm( handle, ArrayEltSize( sym ), TY_UNSIGNED );
    CGAddParm( handle, ArrayNumElts( sym ), TY_INT_4 );
    CGAddParm( handle, SymAddr( sym ), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCIntlArrSet( void ) {
//======================

// Call runtime routine to set internal file to character array.

    call_handle handle;
    sym_id      sym;
    sym_id      scb;

    sym = GetPtr();
    scb = GetPtr();
    CGTrash( CGAssign( SCBLenAddr( CGFEName( scb, TY_POINTER ) ),
                       ArrayEltSize( sym ), TY_INTEGER ) );
    CGTrash( CGAssign( SCBPtrAddr( CGFEName( scb, TY_POINTER ) ),
                       SymAddr( sym ), TY_POINTER ) );
    handle = InitCall( RT_SET_INTL );
    CGAddParm( handle, ArrayNumElts( sym ), TY_INT_4 );
    CGAddParm( handle, CGFEName( scb, TY_POINTER ), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCSetIntl( void ) {
//===================

// Call runtime routine to set internal file to character item (not array).

    call_handle handle;

    handle = InitCall( RT_SET_INTL );
    CGAddParm( handle, CGInteger( 1, TY_INT_4 ), TY_INT_4 );
    CGAddParm( handle, XPop(), TY_POINTER );
    CGDone( CGCall( handle ) );
}


void    FCSetFile( void ) {
//===================

// Set FILE=.

    IOCall( RT_SET_FILE );
}


void    FCSetCCtrl( void ) {
//====================

// Set CARRIAGECONTROL=.

    IOCall( RT_SET_CCTRL );
}


void    FCSetShare( void ) {
//====================

// Set SHARE=.

    IOCall( RT_SET_SHARE );
}


void    FCSetRecType( void ) {
//======================

// Set RECORDTYPE=.

    IOCall( RT_SET_RECTYPE );
}


void    FCSetAction( void ) {
//=====================

// Set ACTION=.

    IOCall( RT_SET_ACTION );
}


void    FCSetErr( void ) {
//==================

// Set ERROR=.

    sym_id      sn;

    CGDone( CGCall( InitCall( RT_SET_ERR ) ) );
    sn = GetPtr();
    // Don't call RefStmtLabel() for 'sn' yet since we will be referencing
    // the label for error checking after an i/o operation. RefStmtLabel()
    // may call DoneLabel() if this is the last reference to the statement
    // label.
    ErrEqStmt = sn;
    ErrEqLabel = sn->u.st.address;
}


void    FCSetEnd( void ) {
//==================

// Set END=.

    sym_id      sn;

    CGDone( CGCall( InitCall( RT_SET_END ) ) );
    sn = GetPtr();
    // Don't call RefStmtLabel() for 'sn' yet since we will be referencing
    // the label for error checking after an i/o operation. RefStmtLabel()
    // may call DoneLabel() if this is the last reference to the statement
    // label.
    EndEqStmt = sn;
    EndEqLabel = sn->u.st.address;
}


void    FCSetAtEnd( void ) {
//====================

// Set END= for ATEND statement.

    CGDone( CGCall( InitCall( RT_SET_END ) ) );
    EndEqLabel = GetU16();
}


void    FCSetRec( void ) {
//==================

// Set REC=.

    IOCallValue( RT_SET_REC );
}


void    FCSetIOS( void ) {
//==================

// Set IOSTAT=.

    IOCall( RT_SET_IOS );
    IOStatSpecified = TRUE;
}


void    FCSetAcc( void ) {
//==================

// Set ACCESS=.

    IOCall( RT_SET_ACC );
}


void    FCSetBlnk( void ) {
//===================

// Set BLANK=.

    IOCall( RT_SET_BLNK );
}


void    FCSetForm( void ) {
//===================

// Set FORM=.

    IOCall( RT_SET_FORM );
}


void    FCInqBlockSize( void ) {
//========================

// Set BLOCKSIZE= for INQUIRE statement.

    IOCall( RT_INQ_BLOCKSIZE );
}


void    FCSetLen( void ) {
//==================

// Set RECL= for INQUIRE statement.

    IOCall( RT_SET_LEN );
}


void    FCSetStat( void ) {
//===================

// Set STATUS=.

    IOCall( RT_SET_STAT );
}


void    FCSetDir( void ) {
//==================

// Set DIRECT=.

    IOCall( RT_SET_DIR );
}


void    FCSetFmtd( void ) {
//===================

// Set FORMATTED=.

    IOCall( RT_SET_FMTD );
}


void    FCSetName( void ) {
//===================

// Set NAME=.

    IOCall( RT_SET_NAME );
}


void    FCSetSeq( void ) {
//==================

// Set SEQUENTIAL=.

    IOCall( RT_SET_SEQ );
}


void    FCSetUFmtd( void ) {
//====================

// Set UNFORMATTED=.

    IOCall( RT_SET_UFMTD );
}


void    FCSetExst( void ) {
//===================

// Set EXIST=.

    IOCall( RT_SET_EXST );
}


void    FCSetNmd( void ) {
//==================

// Set NAMED=.

    IOCall( RT_SET_NMD );
}


void    FCSetNRec( void ) {
//===================

// Set NEXTREC=.

    IOCall( RT_SET_NREC );
}


void    FCSetNumb( void ) {
//===================

// Set NUMBER=.

    IOCall( RT_SET_NUMB );
}


void    FCSetOpen( void ) {
//===================

// Set OPENED=.

    IOCall( RT_SET_OPEN );
}


void    FCSetBlockSize( void ) {
//========================

// Set BLOCKSIZE= for OPEN statement.

    IOCallValue( RT_SET_BLOCKSIZE );
}


void    FCSetRecl( void ) {
//===================

// Set RECL= for OPEN statement.

    IOCallValue( RT_SET_RECL );
}


void    FCSetNoFmt( void ) {
//====================

// Set "not formatted i/o".

    CGDone( CGCall( InitCall( RT_SET_NOFMT ) ) );
}
