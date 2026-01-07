/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Stack F-Code processor.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "wf77cg.h"
#include "tmpdefs.h"
#include "ecflags.h"
#include "cpopt.h"
#include "fltcnv.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cbsize.h"
#include "cnvd2s.h"
#include "tcmplx.h"
#include "fccmplx.h"
#include "fcstring.h"
#include "fcstruct.h"
#include "fcsyms.h"
#include "forcstat.h"
#include "rstmgr.h"
#include "fccall.h"
#include "fcstack.h"
#include "fcjmptab.h"
#include "wf77info.h"
#include "gsegs.h"
#include "cgswitch.h"
#include "cgprotos.h"


static char     *StkPtr;         // F-Code stack pointer

void    InitStack( void )
//=======================
// Initialize stack.
{
    StkPtr = TokenBuff;
}


cg_type ArrayPtrType( sym_id sym )
//================================
{
    if( sym->u.ns.si.va.u.dim_ext->dim_flags & DIM_EXTENDED ) {
#if _CPU == 8086
        if( CGOpts & CGOPT_M_LARGE ) {
            return( TY_HUGE_POINTER );
        } else { // if( CGOpts & CGOPT_M_MEDIUM ) {
            return( TY_LONG_POINTER );
        }
#elif _CPU == 386
        return( TY_LONG_POINTER );
#endif
    }
    return( TY_POINTER );
}


cg_type SymPtrType( sym_id sym )
//==============================
// Get type of pointer required to address given symbol.
{
    cg_type     cgtyp;
    unsigned_32 item_size;
    unsigned_16 flags;

    flags = sym->u.ns.flags;
    if( flags & SY_SUB_PARM ) {
        // subprogram argument
        if( (flags & SY_CLASS) == SY_SUBPROGRAM ) {
            cgtyp = TY_CODE_PTR;
        } else if( flags & SY_SUBSCRIPTED ) {
            cgtyp = ArrayPtrType( sym );
        } else {
            cgtyp = TY_GLOBAL_POINTER;
        }
    } else if( flags & SY_IN_EQUIV ) {
#if _CPU == 8086
        com_eq      *ce_ext;
        signed_32   offset;
        sym_id      leader;

        leader = sym;
        offset = 0;
        for( ;; ) {
            ce_ext = leader->u.ns.si.va.vi.ec_ext;
            if( ce_ext->ec_flags & LEADER )
                break;
            offset += ce_ext->offset;
            leader = ce_ext->link_eqv;
        }
        if( ce_ext->ec_flags & MEMBER_IN_COMMON ) {
            offset += ce_ext->offset;
            if( GetComBlkSize( ce_ext->com_blk ) <= MAX_SEG16_SIZE ) {
                // common block fits in a segment
                cgtyp = TY_GLOBAL_POINTER;
            } else {
                item_size = _SymSize( sym );
                if( flags & SY_SUBSCRIPTED ) {
                    item_size *= sym->u.ns.si.va.u.dim_ext->num_elts;
                }
                if( offset + item_size <= MAX_SEG16_SIZE ) {
                    // object fits in first segment of common block
                    // (common block label is at start of first segment)
                    cgtyp = TY_GLOBAL_POINTER;
                } else {
                    cgtyp = TY_HUGE_POINTER;
                }
            }
        } else {
            if( ce_ext->high - ce_ext->low <= MAX_SEG16_SIZE ) {
                // equivalence set fits in a segment
                cgtyp = TY_GLOBAL_POINTER;
            } else {
                segment_id  leader_segid;

                item_size = _SymSize( sym );
                if( flags & SY_SUBSCRIPTED ) {
                    item_size *= sym->u.ns.si.va.u.dim_ext->num_elts;
                }
                leader_segid = GetGlobalSegId( ce_ext->offset );
                offset += ce_ext->offset;
                if( ( GetGlobalSegId( offset ) == leader_segid )
                  && ( GetGlobalSegId( offset + item_size ) == leader_segid ) ) {
                    // the entire item is in the same segment as the leader
                    cgtyp = TY_GLOBAL_POINTER;
                } else {
                    cgtyp = TY_HUGE_POINTER;
                }
            }
        }
#else
        cgtyp = TY_GLOBAL_POINTER;
#endif
    } else if( flags & SY_IN_COMMON ) {
#if _CPU == 8086
        com_eq      *ce_ext;

        ce_ext = sym->u.ns.si.va.vi.ec_ext;
        if( GetComBlkSize( ce_ext->com_blk ) <= MAX_SEG16_SIZE ) {
            // common block fits in a segment
            cgtyp = TY_GLOBAL_POINTER;
        } else {
            item_size = _SymSize( sym );
            if( flags & SY_SUBSCRIPTED ) {
                item_size *= sym->u.ns.si.va.u.dim_ext->num_elts;
            }
            if( ce_ext->com_blk->u.ns.flags & SY_EQUIVED_NAME ) {
                if( ce_ext->offset + item_size <= MAX_SEG16_SIZE ) {
                    // object fits in first segment of common block
                    // (common block label is at start of first segment)
                    cgtyp = TY_GLOBAL_POINTER;
                } else {
                    cgtyp = TY_HUGE_POINTER;
                }
            } else {
                // each symbol in common block gets a label at the offset into
                // the common block
                if( GetComOffset( ce_ext->offset ) + item_size <= MAX_SEG16_SIZE ) {
                    // object fits in a segment
                    cgtyp = TY_GLOBAL_POINTER;
                } else {
                    cgtyp = TY_HUGE_POINTER;
                }
            }
        }
#else
        cgtyp = TY_GLOBAL_POINTER;
#endif
    } else if( (flags & SY_SUBSCRIPTED)
      && _Allocatable( sym ) ) {
        cgtyp = ArrayPtrType( sym );
    } else if( (flags & SY_SUBSCRIPTED)
      || ( sym->u.ns.u1.s.typ == FT_STRUCTURE ) ) {
        item_size = _SymSize( sym );
        if( flags & SY_SUBSCRIPTED ) {
            item_size *= sym->u.ns.si.va.u.dim_ext->num_elts;
        }
#if _CPU == 8086
        if( item_size > MAX_SEG16_SIZE ) {
            cgtyp = TY_HUGE_POINTER;
        } else if( item_size <= DataThreshold ) {
            cgtyp = TY_LOCAL_POINTER;
        } else {
            cgtyp = TY_GLOBAL_POINTER;
        }
#else
        if( item_size <= DataThreshold ) {
            cgtyp = TY_LOCAL_POINTER;
        } else {
            cgtyp = TY_GLOBAL_POINTER;
        }
#endif
    } else {
        cgtyp = TY_LOCAL_POINTER;
    }
    return( cgtyp );
}


void    XPush( cg_name cgname )
//=============================
// Push a CG-name on the stack.
{
    *(cg_name *)StkPtr = cgname;
    StkPtr += sizeof( cg_name );
}


cg_name SymIndex( sym_id sym, cg_name i )
//=======================================
// Get address of symbol plus an index.
// Merges offset of symbols in common or equivalence with index so that
// we don't get two run-time calls for huge pointer arithmetic.
{
    cg_name     addr;
    cg_type     cgtyp;
    bool        data_reference;

    data_reference = true;
    if( (sym->u.ns.flags & SY_CLASS) == SY_SUBPROGRAM ) {
        if( (sym->u.ns.flags & SY_SUBPROG_TYPE) == SY_STMT_FUNC ) {
            addr = CGFEName( sym, F77ToCGType( sym ) );
        } else {
            addr = CGFEName( sym, TY_CODE_PTR );
            if( sym->u.ns.flags & SY_SUB_PARM ) {
                addr = CGUnary( O_POINTS, addr, TY_CODE_PTR );
            }
            data_reference = false;
        }
    } else if( sym->u.ns.flags & SY_PS_ENTRY ) {
        // it's the shadow symbol for function return value
        if( CommonEntry == NULL ) {
            if( sym->u.ns.u1.s.typ == FT_CHAR ) {
                if( Options & OPT_DESCRIPTOR ) {
                    addr = CGFEName( ReturnValue, F77ToCGType( sym ) );
                    addr = CGUnary( O_POINTS, addr, TY_POINTER );
                } else {
                    addr = SubAltSCB( sym->u.ns.si.ms.sym );
                }
            } else {
                addr = CGFEName( ReturnValue, F77ToCGType( sym ) );
            }
        } else {
            if( (sym->u.ns.u1.s.typ == FT_CHAR)
              && (Options & OPT_DESCRIPTOR) == 0 ) {
                addr = SubAltSCB( CommonEntry );
            } else {
                addr = CGUnary( O_POINTS, CGFEName( ReturnValue, TY_POINTER ),
                                TY_POINTER );
            }
        }
    } else if( sym->u.ns.flags & SY_SUB_PARM ) {
        // subprogram argument
        if( sym->u.ns.flags & SY_SUBSCRIPTED ) {
            cgtyp = ArrayPtrType( sym );
            if( sym->u.ns.u1.s.typ == FT_CHAR ) {
                addr = CGUnary( O_POINTS, CGFEName( sym, cgtyp ), cgtyp );
                if( (sym->u.ns.flags & SY_VALUE_PARM) == 0 ) {
                    if( Options & OPT_DESCRIPTOR ) {
                        addr = SCBPointer( addr );
                    }
                }
            } else {
                addr = CGUnary( O_POINTS, CGFEName( sym, cgtyp ), cgtyp );
            }
        } else {
            cgtyp = TY_POINTER;
            if( sym->u.ns.u1.s.typ == FT_CHAR ) {
                if( SCBRequired( sym ) ) {
                    addr = VarAltSCB( sym );
                } else {
                    addr = CGUnary( O_POINTS, CGFEName( sym, cgtyp ), cgtyp );
                }
            } else if( sym->u.ns.flags & SY_VALUE_PARM ) {
                cgtyp = F77ToCGType( sym );
                if( TypeCmplx( sym->u.ns.u1.s.typ ) ) {
                    cgtyp = CmplxBaseType( cgtyp );
                    addr = CGFEName( sym, cgtyp );
                    XPush( CGUnary( O_POINTS,
                                    CGFEName( FindArgShadow( sym ), cgtyp ),
                                    cgtyp ) );
                    addr = CGUnary( O_POINTS, addr, cgtyp );
                } else {
                    addr = CGFEName( sym, cgtyp );
                }
            } else {
                addr = CGUnary( O_POINTS, CGFEName( sym, cgtyp ), cgtyp );
            }
        }
    } else if( sym->u.ns.flags & SY_IN_EQUIV ) {
        sym_id      leader;
        signed_32   offset;

        leader = sym;
        offset = 0;
        for( ;; ) {
            if( leader->u.ns.si.va.vi.ec_ext->ec_flags & LEADER )
                break;
            offset += leader->u.ns.si.va.vi.ec_ext->offset;
            leader = leader->u.ns.si.va.vi.ec_ext->link_eqv;
        }
        if( leader->u.ns.si.va.vi.ec_ext->ec_flags & MEMBER_IN_COMMON ) {
            addr = CGFEName( leader->u.ns.si.va.vi.ec_ext->com_blk,
                             F77ToCGType( sym ) );
            offset += leader->u.ns.si.va.vi.ec_ext->offset;
        } else {
            sym_id      shadow;

            shadow = FindEqSetShadow( leader );
            if( shadow != NULL ) {
                addr = CGFEName( shadow, shadow->u.ns.si.ms.u.cgtyp );
                offset -= leader->u.ns.si.va.vi.ec_ext->low;
            } else if( (leader->u.ns.u1.s.typ == FT_CHAR)
              && (leader->u.ns.flags & SY_SUBSCRIPTED) == 0 ) {
                addr = CGBackName( leader->u.ns.si.va.u.bck_hdl, F77ToCGType( sym ) );
            } else {
                addr = CGFEName( leader, F77ToCGType( sym ) );
            }
        }
        if( i != NULL ) {
            i = CGBinary( O_PLUS, i, CGInteger( offset, TY_INT_4 ), TY_INT_4 );
        } else {
            i = CGInteger( offset, TY_INT_4 );
        }
        addr = CGBinary( O_PLUS, addr, i, SymPtrType( sym ) );
        if( (sym->u.ns.u1.s.typ == FT_CHAR)
          && (sym->u.ns.flags & SY_SUBSCRIPTED) == 0 ) {
            // tell code generator where storage pointed to by SCB is located
            addr = CGBinary( O_COMMA, addr, CGFEName( sym, F77ToCGType( sym ) ), TY_DEFAULT );
        }
        i = NULL;
    } else if( ( sym->u.ns.u1.s.typ == FT_CHAR )
      && ( (sym->u.ns.flags & SY_SUBSCRIPTED) == 0 ) ) {
        // character variable, address of scb
        addr = CGFEName( sym, F77ToCGType( sym ) );
    } else if( sym->u.ns.flags & SY_IN_COMMON ) {
        com_eq      *ce_ext;

        ce_ext = sym->u.ns.si.va.vi.ec_ext;
        if( i != NULL ) {
            i = CGBinary( O_PLUS, i, CGInteger( ce_ext->offset, TY_INT_4 ), TY_INT_4 );
        } else {
            i = CGInteger( ce_ext->offset, TY_INT_4 );
        }
        addr = CGBinary( O_PLUS, CGFEName( ce_ext->com_blk, F77ToCGType( sym ) ), i, SymPtrType( sym ) );
        i = NULL;
    } else {
        addr = CGFEName( sym, F77ToCGType( sym ) );
        if( (sym->u.ns.flags & SY_SUBSCRIPTED)
          && _Allocatable( sym ) ) {
            addr = CGUnary( O_POINTS, addr, ArrayPtrType( sym ) );
        }
    }
    if( i != NULL ) {
        addr = CGBinary( O_PLUS, addr, i, SymPtrType( sym ) );
    }
    if( (OZOpts & OZOPT_O_VOLATILE)
      && data_reference
      && ( ( sym->u.ns.u1.s.typ >= FT_REAL )
      && ( sym->u.ns.u1.s.typ <= FT_XCOMPLEX ) ) ) {
        addr = CGVolatile( addr );
    } else if( sym->u.ns.u1.s.xflags & SY_VOLATILE ) {
        addr = CGVolatile( addr );
    }
    return( addr );
}


cg_name SymAddr( sym_id sym )
//===========================
{
    return( SymIndex( sym, NULL ) );
}


void    FCPush( void )
//====================
// Process PUSH F-Code.
{
    sym_id      sym;

    sym = GetPtr();
    if( TypeCmplx( sym->u.ns.u1.s.typ ) ) {
        PushComplex( sym );
    } else {
        XPush( SymAddr( sym ) );
    }
}


cg_name SymValue( sym_id sym )
//============================
// Generate value of a symbol.
{
    return( CGUnary( O_POINTS, SymAddr( sym ), F77ToCGType( sym ) ) );
}


void    DXPush( intstar4 val )
//============================
// Push a constant on the stack for DATA statement expressions.
{
    *(intstar4 *)StkPtr = val;
    StkPtr += sizeof( intstar4 );
}


void    SymPush( sym_id val )
//===========================
// Push a symbol table entry on the stack.
{
    *(sym_id *)StkPtr = val;
    StkPtr += sizeof( sym_id );
}


cg_name XPop( void )
//==================
// Pop a CG-name from the stack.
{
    StkPtr -= sizeof( cg_name );
    return( *(cg_name *)StkPtr );
}


cg_name XPopValue( cg_type cgtyp )
//================================
// Pop a CG-name from the stack (its value).
{
    cg_name     opn;

    opn = XPop();
    if( IsCGPointer( CGType( opn ) ) ) {
        opn = CGUnary( O_POINTS, opn, cgtyp );
    }
    return( opn );
}


void    FCPop( void )
//===================
// Process POP F-Code.
{
    sym_id      sym;
    cg_name     dst;
    unsigned_16 typ_info;
    cg_type     dst_cgtyp;
    cg_type     src_cgtyp;
    sym_id      fd;

    sym = GetPtr();
    typ_info = GetU16();
    dst_cgtyp = GetCGTypes1( typ_info );
    src_cgtyp = GetCGTypes2( typ_info );
    if( ( dst_cgtyp == TY_COMPLEX )
      || ( dst_cgtyp == TY_DCOMPLEX )
      || ( dst_cgtyp == TY_XCOMPLEX ) ) {
        CmplxAssign( sym, dst_cgtyp, src_cgtyp );
    } else {
        dst = NULL;
        if( (sym->u.ns.flags & SY_CLASS) == SY_SUBPROGRAM ) {
            // it's a statement function
            if( (OZOpts & OZOPT_O_INLINE) == 0 ) {
                dst = SymAddr( sym );
            }
        } else {
            fd = NULL;
            if( sym->u.ns.u1.s.typ == FT_STRUCTURE ) {
                if( GetU16() ) {
                    // target is a sub-field
                    dst = XPop();
                    if( dst_cgtyp == TY_USER_DEFINED ) {
                        // sub-field is a structure or an array element
                        fd = GetPtr();
                    }
                } else {
                    dst = SymAddr( sym );
                }
            } else if( sym->u.ns.flags & SY_SUBSCRIPTED ) {
                // target is an array element
                dst = XPop();
            } else {
                dst = SymAddr( sym );
            }
            if( dst_cgtyp == TY_USER_DEFINED ) {
                if( fd == NULL ) {
                    dst_cgtyp = sym->u.ns.xt.record->cgtyp;
                } else {
                    dst_cgtyp = fd->u.fd.xt.record->cgtyp;
                }
                XPush( CGAssign( dst, CGUnary( O_POINTS, XPop(), dst_cgtyp ), dst_cgtyp ) );
                return;
            }
        }
        if( (src_cgtyp == TY_COMPLEX)
          || (src_cgtyp == TY_DCOMPLEX)
          || (src_cgtyp == TY_XCOMPLEX) ) {
            Cmplx2Scalar();
            src_cgtyp = CmplxBaseType( src_cgtyp );
        }
        if( ( (sym->u.ns.flags & SY_CLASS) == SY_SUBPROGRAM )
          && (OZOpts & OZOPT_O_INLINE) )
            return;
        XPush( CGAssign( dst, XPopValue( src_cgtyp ), dst_cgtyp ) );
    }
}


cg_name GetTypedValue( void )
//===========================
// Pop a CG-name from the stack (its value).
{
    cg_name     opn;
    cg_type     cgtyp;

    opn = XPop();
    cgtyp = GetCGType( GetU16() );
    if( IsCGPointer( CGType( opn ) ) ) {
        opn = CGUnary( O_POINTS, opn, cgtyp );
    }
    return( opn );
}


cg_name         StkElement( int idx )
//===================================
// Get idx'th stack element.
{
    return(  *(cg_name * )( StkPtr - idx * sizeof( cg_name ) ) );
}


void            PopStkElements( int num )
//=======================================
// Pop stack elements from the stack.
{
    StkPtr -= num * sizeof( cg_name );
}


intstar4        DXPop( void )
//===========================
// Pop a constant from the stack for DATA statement expressions.
{
    StkPtr -= sizeof( intstar4 );
    return( *(intstar4 *)StkPtr );
}


sym_id          SymPop( void )
//============================
// Pop a symbol table entry from the stack.
{
    StkPtr -= sizeof( sym_id );
    return( *(sym_id *)StkPtr );
}


cg_name IntegerConstant( ftn_type *value, size_t size )
//=====================================================
{
    if( size == sizeof( intstar1 ) ) {
        return( CGInteger( value->intstar1, TY_INT_1 ) );
    } else if( size == sizeof( intstar2 ) ) {
        return( CGInteger( value->intstar2, TY_INT_2 ) );
    } else {
        return( CGInteger( value->intstar4, TY_INT_4 ) );
    }
}


void    FCPushConst( void )
//=========================
// Process PUSH_CONST F-Code.
{
    sym_id      sym;
    char        fmt_buff[CONVERSION_BUFFER+1];

    sym = GetPtr();
    switch( sym->u.cn.typ ) {
    case FT_INTEGER_1 :
    case FT_INTEGER_2 :
    case FT_INTEGER :
        XPush( IntegerConstant( &sym->u.cn.value, sym->u.cn.size ) );
        break;
    case FT_LOGICAL_1 :
    case FT_LOGICAL :
        XPush( CGInteger( sym->u.cn.value.logstar4, TY_UINT_1 ) );
        break;
    case FT_REAL :
        CnvS2S( &sym->u.cn.value.single, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_SINGLE ) );
        break;
    case FT_DOUBLE :
        CnvD2S( &sym->u.cn.value.dble, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_DOUBLE ) );
        break;
    case FT_TRUE_EXTENDED :
        CnvX2S( &sym->u.cn.value.extended, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_LONGDOUBLE ) );
        break;
    case FT_COMPLEX :
        PushCmplxConst( sym );
        break;
    case FT_DCOMPLEX :
        PushCmplxConst( sym );
        break;
    case FT_TRUE_XCOMPLEX :
        PushCmplxConst( sym );
        break;
    }
}


void    FCPushLit( void )
//=======================
// Process PUSH_LIT F-Code.
{
    sym_id      sym;

    sym = GetPtr();
    if( sym->u.lt.flags & (LT_SCB_REQUIRED | LT_SCB_TMP_REFERENCE) ) {
        XPush( CGBackName( ConstBack( sym ), TY_CHAR ) );
    }
}
