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
* Description:  Stack F-Code processor.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "wf77defs.h"
#include "cg.h"
#include "tmpdefs.h"
#include "ecflags.h"
#include "cpopt.h"
#include "fltcnv.h"
#include "emitobj.h"
#include "fctypes.h"
#include "cgswitch.h"
#include "cgprotos.h"


extern  pointer         ConstBack(sym_id);
extern  void            CnvS2S(float *,char *);
extern  void            CnvD2S(double *,char *);
extern  void            CnvX2S(extended *,char *);
extern  cg_name         StructRef(cg_name,int);
extern  segment_id      GetGlobalSeg(unsigned_32);
extern  bool            TypeCmplx(TYPE);
extern  intstar4        GetComBlkSize(sym_id);
extern  seg_offset      GetComOffset(unsigned_32);
extern  cg_name         SCBPointer(cg_name);
extern  void            CmplxAssign(sym_id,cg_type,cg_type);
extern  void            PushCmplxConst(sym_id);
extern  void            PushComplex(sym_id);
extern  void            Cmplx2Scalar( void );
extern  cg_type         CmplxBaseType(cg_type);
extern  cg_name         VarAltSCB(sym_id);
extern  cg_name         SubAltSCB(sym_id);
extern  bool            SCBRequired(sym_id);
extern  sym_id          FindEqSetShadow(sym_id);
extern  sym_id          FindArgShadow(sym_id);
extern  bool            ForceStatic(unsigned_16);


void    InitStack( void ) {
//===================

// Initialize stack.

    StkPtr = &TokenBuff;
}


cg_type ArrayPtrType( sym_id sym ) {
//==================================

    if( sym->ns.si.va.dim_ext->dim_flags & DIM_EXTENDED ) {
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


cg_type SymPtrType( sym_id sym ) {
//================================

// Get type of pointer required to address given symbol.

    sym_id      leader;
    cg_type     p_type;
    signed_32   offset;
    com_eq      *ce_ext;
    unsigned_32 item_size;
    segment_id  leader_seg;
    unsigned_16 flags;

    flags = sym->ns.flags;
    if( flags & SY_SUB_PARM ) {
        // subprogram argument
        if( ( flags & SY_CLASS ) == SY_SUBPROGRAM ) {
            p_type = TY_CODE_PTR;
        } else if( flags & SY_SUBSCRIPTED ) {
            p_type = ArrayPtrType( sym );
        } else {
            p_type = TY_GLOBAL_POINTER;
        }
    } else if( flags & SY_IN_EQUIV ) {
        leader = sym;
        offset = 0;
        for(;;) {
            ce_ext = leader->ns.si.va.vi.ec_ext;
            if( ce_ext->ec_flags & LEADER ) break;
            offset += ce_ext->offset;
            leader = ce_ext->link_eqv;
        }
        if( ce_ext->ec_flags & MEMBER_IN_COMMON ) {
            offset += ce_ext->offset;
            if( GetComBlkSize( ce_ext->com_blk ) <= MaxSegSize ) {
                // common block fits in a segment
                p_type = TY_GLOBAL_POINTER;
            } else {
                item_size = _SymSize( sym );
                if( flags & SY_SUBSCRIPTED ) {
                    item_size *= sym->ns.si.va.dim_ext->num_elts;
                }
                if( offset + item_size <= MaxSegSize ) {
                    // object fits in first segment of common block
                    // (common block label is at start of first segment)
                    p_type = TY_GLOBAL_POINTER;
                } else {
                    p_type = TY_HUGE_POINTER;
                }
            }
        } else {
            if( ce_ext->high - ce_ext->low <= MaxSegSize ) {
                // equivalence set fits in a segment
                p_type = TY_GLOBAL_POINTER;
            } else {
                item_size = _SymSize( sym );
                if( flags & SY_SUBSCRIPTED ) {
                    item_size *= sym->ns.si.va.dim_ext->num_elts;
                }
                leader_seg = GetGlobalSeg( ce_ext->offset );
                offset += ce_ext->offset;
                if( ( GetGlobalSeg( offset ) == leader_seg ) &&
                    ( GetGlobalSeg( offset + item_size ) == leader_seg ) ) {
                    // the entire item is in the same segment as the leader
                    p_type = TY_GLOBAL_POINTER;
                } else {
                    p_type = TY_HUGE_POINTER;
                }
            }
        }
    } else if( flags & SY_IN_COMMON ) {
        ce_ext = sym->ns.si.va.vi.ec_ext;
        if( GetComBlkSize( ce_ext->com_blk ) <= MaxSegSize ) {
            // common block fits in a segment
            p_type = TY_GLOBAL_POINTER;
        } else {
            item_size = _SymSize( sym );
            if( flags & SY_SUBSCRIPTED ) {
                item_size *= sym->ns.si.va.dim_ext->num_elts;
            }
            if( ce_ext->com_blk->ns.flags & SY_EQUIVED_NAME ) {
                if( ce_ext->offset + item_size <= MaxSegSize ) {
                    // object fits in first segment of common block
                    // (common block label is at start of first segment)
                    p_type = TY_GLOBAL_POINTER;
                } else {
                    p_type = TY_HUGE_POINTER;
                }
            } else {
                // each symbol in common block gets a label at the offset into
                // the common block
                if( GetComOffset( ce_ext->offset ) + item_size <= MaxSegSize ) {
                    // object fits in a segment
                    p_type = TY_GLOBAL_POINTER;
                } else {
                    p_type = TY_HUGE_POINTER;
                }
            }
        }
    } else if( ( flags & SY_SUBSCRIPTED ) && _Allocatable( sym ) ) {
        p_type = ArrayPtrType( sym );
    } else if( ( flags & SY_SUBSCRIPTED ) || ( sym->ns.typ == FT_STRUCTURE ) ) {
        item_size = _SymSize( sym );
        if( flags & SY_SUBSCRIPTED ) {
            item_size *= sym->ns.si.va.dim_ext->num_elts;
        }
        if( item_size > MaxSegSize ) {
            p_type = TY_HUGE_POINTER;
        } else if( item_size <= DataThreshold ) {
            p_type = TY_LOCAL_POINTER;
        } else {
            p_type = TY_GLOBAL_POINTER;
        }
    } else {
        p_type = TY_LOCAL_POINTER;
    }
    return( p_type );
}


void    XPush( cg_name cgname ) {
//===============================

// Push a CG-name on the stack.

    *(cg_name *)StkPtr = cgname;
    StkPtr = (char *)StkPtr + sizeof( cg_name );
}


cg_name SymIndex( sym_id sym, cg_name i ) {
//=========================================

// Get address of symbol plus an index.
// Merges offset of symbols in common or equivalence with index so that
// we don't get two run-time calls for huge pointer arithmetic.

    sym_id      leader;
    cg_name     addr;
    signed_32   offset;
    com_eq      *ce_ext;
    cg_type     p_type;
    bool        data_reference;

    data_reference = TRUE;
    if( ( sym->ns.flags & SY_CLASS ) == SY_SUBPROGRAM ) {
        if( ( sym->ns.flags & SY_SUBPROG_TYPE ) == SY_STMT_FUNC ) {
            addr = CGFEName( sym, F772CGType( sym ) );
        } else {
            addr = CGFEName( sym, TY_CODE_PTR );
            if( sym->ns.flags & SY_SUB_PARM ) {
                addr = CGUnary( O_POINTS, addr, TY_CODE_PTR );
            }
            data_reference = FALSE;
        }
    } else if( sym->ns.flags & SY_PS_ENTRY ) {
        // it's the shadow symbol for function return value
        if( CommonEntry == NULL ) {
            if( sym->ns.typ == FT_CHAR ) {
                if( Options & OPT_DESCRIPTOR ) {
                    addr = CGFEName( ReturnValue, F772CGType( sym ) );
                    addr = CGUnary( O_POINTS, addr, TY_POINTER );
                } else {
                    addr = SubAltSCB( sym->ns.si.ms.sym );
                }
            } else {
                addr = CGFEName( ReturnValue, F772CGType( sym ) );
            }
        } else {
            if( (sym->ns.typ == FT_CHAR) && !(Options & OPT_DESCRIPTOR) ) {
                addr = SubAltSCB( CommonEntry );
            } else {
                addr = CGUnary( O_POINTS, CGFEName( ReturnValue, TY_POINTER ),
                                TY_POINTER );
            }
        }
    } else if( sym->ns.flags & SY_SUB_PARM ) {
        // subprogram argument
        if( sym->ns.flags & SY_SUBSCRIPTED ) {
            p_type = ArrayPtrType( sym );
            if( sym->ns.typ == FT_CHAR ) {
                addr = CGUnary( O_POINTS, CGFEName( sym, p_type ), p_type );
                if( !(sym->ns.flags & SY_VALUE_PARM) ) {
                    if( Options & OPT_DESCRIPTOR ) {
                        addr = SCBPointer( addr );
                    }
                }
            } else {
                addr = CGUnary( O_POINTS, CGFEName( sym, p_type ), p_type );
            }
        } else {
            p_type = TY_POINTER;
            if( sym->ns.typ == FT_CHAR ) {
                if( SCBRequired( sym ) ) {
                    addr = VarAltSCB( sym );
                } else {
                    addr = CGUnary( O_POINTS, CGFEName( sym, p_type ), p_type );
                }
            } else if( sym->ns.flags & SY_VALUE_PARM ) {
                p_type = F772CGType( sym );
                if( TypeCmplx( sym->ns.typ ) ) {
                    p_type = CmplxBaseType( p_type );
                    addr = CGFEName( sym, p_type );
                    XPush( CGUnary( O_POINTS,
                                    CGFEName( FindArgShadow( sym ), p_type ),
                                    p_type ) );
                    addr = CGUnary( O_POINTS, addr, p_type );
                } else {
                    addr = CGFEName( sym, p_type );
                }
            } else {
                addr = CGUnary( O_POINTS, CGFEName( sym, p_type ), p_type );
            }
        }
    } else if( sym->ns.flags & SY_IN_EQUIV ) {
        leader = sym;
        offset = 0;
        for(;;) {
            if( leader->ns.si.va.vi.ec_ext->ec_flags & LEADER ) break;
            offset += leader->ns.si.va.vi.ec_ext->offset;
            leader = leader->ns.si.va.vi.ec_ext->link_eqv;
        }
        if( leader->ns.si.va.vi.ec_ext->ec_flags & MEMBER_IN_COMMON ) {
            addr = CGFEName( leader->ns.si.va.vi.ec_ext->com_blk,
                             F772CGType( sym ) );
            offset += leader->ns.si.va.vi.ec_ext->offset;
        } else {
            sym_id      shadow;

            shadow = FindEqSetShadow( leader );
            if( shadow != NULL ) {
                addr = CGFEName( shadow, shadow->ns.si.ms.cg_typ );
                offset -= leader->ns.si.va.vi.ec_ext->low;
            } else if( (leader->ns.typ == FT_CHAR) &&
                       !(leader->ns.flags & SY_SUBSCRIPTED) ) {
                addr = CGBackName( leader->ns.si.va.bck_hdl, F772CGType( sym ) );
            } else {
                addr = CGFEName( leader, F772CGType( sym ) );
            }
        }
        if( i != NULL ) {
            i = CGBinary( O_PLUS, i, CGInteger( offset, TY_INT_4 ), TY_INT_4 );
        } else {
            i = CGInteger( offset, TY_INT_4 );
        }
        addr = CGBinary( O_PLUS, addr, i, SymPtrType( sym ) );
        if( (sym->ns.typ == FT_CHAR) && !(sym->ns.flags & SY_SUBSCRIPTED) ) {
            // tell code generator where storage pointed to by SCB is located
            addr = CGBinary( O_COMMA, addr,
                             CGFEName( sym, F772CGType( sym ) ), TY_DEFAULT );
        }
        i = NULL;
    } else if( ( sym->ns.typ == FT_CHAR ) &&
               ( ( sym->ns.flags & SY_SUBSCRIPTED ) == 0 ) ) {
        // character variable, address of scb
        addr = CGFEName( sym, F772CGType( sym ) );
    } else if( sym->ns.flags & SY_IN_COMMON ) {
        ce_ext = sym->ns.si.va.vi.ec_ext;
        if( i != NULL ) {
            i = CGBinary( O_PLUS, i, CGInteger( ce_ext->offset, TY_INT_4 ),
                          TY_INT_4 );
        } else {
            i = CGInteger( ce_ext->offset, TY_INT_4 );
        }
        addr = CGBinary( O_PLUS, CGFEName( ce_ext->com_blk, F772CGType( sym ) ),
                         i, SymPtrType( sym ) );
        i = NULL;
    } else {
        addr = CGFEName( sym, F772CGType( sym ) );
        if( ( sym->ns.flags & SY_SUBSCRIPTED ) && _Allocatable( sym ) ) {
            addr = CGUnary( O_POINTS, addr, ArrayPtrType( sym ) );
        }
    }
    if( i != NULL ) {
        addr = CGBinary( O_PLUS, addr, i, SymPtrType( sym ) );
    }
    if( ( OZOpts & OZOPT_O_VOLATILE ) && data_reference &&
        ( ( sym->ns.typ >= FT_REAL ) && ( sym->ns.typ <= FT_XCOMPLEX ) ) ) {
        addr = CGVolatile( addr );
    } else if( sym->ns.xflags & SY_VOLATILE ) {
        addr = CGVolatile( addr );
    }
    return( addr );
}


cg_name SymAddr( sym_id sym ) {
//=============================

    return( SymIndex( sym, NULL ) );
}


void    FCPush( void ) {
//================

// Process PUSH F-Code.

    sym_id      sym;

    sym = GetPtr();
    if( TypeCmplx( sym->ns.typ ) ) {
        PushComplex( sym );
    } else {
        XPush( SymAddr( sym ) );
    }
}


cg_name SymValue( sym_id sym ) {
//==============================

// Generate value of a symbol.

    return( CGUnary( O_POINTS, SymAddr( sym ), F772CGType( sym ) ) );
}


char *  StackBuffer( int * len ) {
//================================

// Return a pointer to a buffer on the stack.  This is a very temporary buffer.

    *len = TOKLEN - ((char *)StkPtr - TokenBuff);
    return( StkPtr );
}


void    DXPush( intstar4 val ) {
//==============================

// Push a constant on the stack for DATA statement expressions.

    *(intstar4 *)StkPtr = val;
    StkPtr = (char *)StkPtr + sizeof( intstar4 );
}


void    SymPush( sym_id val ) {
//=============================

// Push a symbol table entry on the stack.

    *(sym_id *)StkPtr = val;
    StkPtr = (char *)StkPtr + sizeof( sym_id );
}


cg_name XPop( void ) {
//==============

// Pop a CG-name from the stack.

    StkPtr = (char *)StkPtr - sizeof( cg_name );
    return( *(cg_name *)StkPtr );
}


cg_name XPopValue( cg_type typ ) {
//================================

// Pop a CG-name from the stack (its value).

    cg_name     opn;

    opn = XPop();
    if( TypePointer( CGType( opn ) ) ) {
        opn = CGUnary( O_POINTS, opn, typ );
    }
    return( opn );
}


void    FCPop( void ) {
//===============

// Process POP F-Code.

    sym_id      sym;
    cg_name     dst;
    unsigned_16 typ_info;
    cg_type     dst_typ;
    cg_type     src_typ;
    sym_id      fd;

    sym = GetPtr();
    typ_info = GetU16();
    dst_typ = GetType1( typ_info );
    src_typ = GetType2( typ_info );
    if( ( dst_typ == TY_COMPLEX ) || ( dst_typ == TY_DCOMPLEX )
        || ( dst_typ == TY_XCOMPLEX ) ) {
        CmplxAssign( sym, dst_typ, src_typ );
    } else {
        if( (sym->ns.flags & SY_CLASS) == SY_SUBPROGRAM ) {
            // it's a statement function
            if( !(OZOpts & OZOPT_O_INLINE) ) {
                dst = SymAddr( sym );
            }
        } else {
            fd = NULL;
            if( sym->ns.typ == FT_STRUCTURE ) {
                if( GetU16() ) {
                    // target is a sub-field
                    dst = XPop();
                    if( dst_typ == TY_USER_DEFINED ) {
                        // sub-field is a structure or an array element
                        fd = GetPtr();
                    }
                } else {
                    dst = SymAddr( sym );
                }
            } else if( sym->ns.flags & SY_SUBSCRIPTED ) {
                // target is an array element
                dst = XPop();
            } else {
                dst = SymAddr( sym );
            }
            if( dst_typ == TY_USER_DEFINED ) {
                if( fd == NULL ) {
                    dst_typ = sym->ns.xt.record->cg_typ;
                } else {
                    dst_typ = fd->fd.xt.record->cg_typ;
                }
                XPush( CGAssign( dst, CGUnary( O_POINTS, XPop(), dst_typ ),
                                 dst_typ ) );
                return;
            }
        }
        if( (src_typ == TY_COMPLEX) || (src_typ == TY_DCOMPLEX)
            || (src_typ == TY_XCOMPLEX) ) {
            Cmplx2Scalar();
            src_typ = CmplxBaseType( src_typ );
        }
        if( ((sym->ns.flags & SY_CLASS) == SY_SUBPROGRAM) &&
            (OZOpts & OZOPT_O_INLINE ) ) return;
        XPush( CGAssign( dst, XPopValue( src_typ ), dst_typ ) );
    }
}


cg_name GetTypedValue( void ) {
//=======================

// Pop a CG-name from the stack (its value).

    cg_name     opn;
    cg_type     typ;

    opn = XPop();
    typ = GetType( GetU16() );
    if( TypePointer( CGType( opn ) ) ) {
        opn = CGUnary( O_POINTS, opn, typ );
    }
    return( opn );
}


cg_name         StkElement( int idx ) {
//=====================================

// Get idx'th stack element.

    return(  *(cg_name * )((char *)StkPtr - idx * sizeof( cg_name )) );
}


void            PopStkElements( int num ) {
//=========================================

// Pop stack elements from the stack.

    StkPtr = (char *)StkPtr - num * sizeof( cg_name );
}


intstar4        DXPop( void ) {
//=======================

// Pop a constant from the stack for DATA statement expressions.

    StkPtr = (char *)StkPtr - sizeof( intstar4 );
    return( *(intstar4 *)StkPtr );
}


sym_id          SymPop( void ) {
//========================

// Pop a symbol table entry from the stack.

    StkPtr = (char *)StkPtr - sizeof( sym_id );
    return( *(sym_id *)StkPtr );
}


cg_name IntegerConstant( ftn_type *value, uint size ) {
//===================================================

    if( size == sizeof( intstar1 ) ) {
        return( CGInteger( value->intstar1, TY_INT_1 ) );
    } else if( size == sizeof( intstar2 ) ) {
        return( CGInteger( value->intstar2, TY_INT_2 ) );
    } else {
        return( CGInteger( value->intstar4, TY_INT_4 ) );
    }
}


void    FCPushConst( void ) {
//=====================

// Process PUSH_CONST F-Code.

    sym_id      sym;
    char        fmt_buff[CONVERSION_BUFFER+1];

    sym = GetPtr();
    switch( sym->cn.typ ) {
    case FT_INTEGER_1 :
    case FT_INTEGER_2 :
    case FT_INTEGER :
        XPush( IntegerConstant( &sym->cn.value, sym->cn.size ) );
        break;
    case FT_LOGICAL_1 :
    case FT_LOGICAL :
        XPush( CGInteger( sym->cn.value.logstar4, TY_UINT_1 ) );
        break;
    case FT_REAL :
        CnvS2S( &sym->cn.value.single, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_SINGLE ) );
        break;
    case FT_DOUBLE :
        CnvD2S( &sym->cn.value.dble, fmt_buff );
        XPush( CGFloat( fmt_buff, TY_DOUBLE ) );
        break;
    case FT_TRUE_EXTENDED :
        CnvX2S( &sym->cn.value.extended, fmt_buff );
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


void    FCPushLit( void ) {
//===================

// Process PUSH_LIT F-Code.

    sym_id      sym;

    sym = GetPtr();
    if( sym->lt.flags & (LT_SCB_REQUIRED | LT_SCB_TMP_REFERENCE) ) {
        XPush( CGBackName( ConstBack( sym ), TY_CHAR ) );
    }
}
