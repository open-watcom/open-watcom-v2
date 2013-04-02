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


#include "standard.h"
#include "coderep.h"
#include "pattern.h"
#include "procdef.h"
#include "cgdefs.h"
#include "model.h"
#include "ocentry.h"
#include "objrep.h"
#include "zoiks.h"
#include "cgaux.h"
#include "typedef.h"
#include "types.h"
#include "dbgstrct.h"
#include "wvdbg.h"
#include "feprotos.h"
#include "cgprotos.h"

extern  seg_id          AskOP(void);
extern  name            *DeAlias(name*);
extern  name            *AllocUserTemp(pointer,type_class_def);
extern  seg_id          SetOP(seg_id);
extern  offset          AskLocation(void);
extern  void            DataInt(short_offset);
#if _TARGET & _TARG_IAPX86
extern  hw_reg_set      Low32Reg(hw_reg_set);
#elif _TARGET & _TARG_80386
extern  hw_reg_set      Low64Reg(hw_reg_set);
#endif
extern  void            DataBytes(unsigned_32,byte*);
extern  void            DoBigBckPtr(back_handle,offset);
extern  type_length     NewBase(name*);
extern  int             ParmsAtPrologue( void ) ;
extern  void            DBLocFini( dbg_loc loc );
extern  void            FEPtr(sym_handle,type_def *,offset);


static    temp_buff     *CurrBuff;



extern  void    BuffStart( temp_buff *temp, uint def ) {
/******************************************************/


    CurrBuff = temp;
    temp->buff[ 1 ] = def;
    temp->index = 2;
    temp->fix_idx = 0;
}



extern  void    BuffEnd( seg_id seg ) {
/*************************************/

    seg_id              old;
    byte                *buff;
    type_def            *ptr_type;
    dbg_patch_handle    *save;
    uint                i;
    uint                last;
    uint                size;

    ptr_type = TypeAddress( TY_LONG_POINTER );
    old = SetOP( seg );
    CurrBuff->buff[ 0 ] = CurrBuff->index;
    buff = CurrBuff->buff;
    last = 0;
    for( i = 0; i < CurrBuff->fix_idx; ++i ) {
        size = CurrBuff->fix[i].pos - last;
        last = CurrBuff->fix[i].pos;
        DataBytes( size, buff );
        buff += size;
        switch( CurrBuff->fix[i].type ) {
        case FIX_SYMBOL:
            FEPtr( CurrBuff->fix[i].p, ptr_type, 0 );
            last += 2 + WORD_SIZE;
            buff += 2 + WORD_SIZE;
            break;
        case FIX_BACKHANDLE:
            DoBigBckPtr( CurrBuff->fix[i].p, *(offset *)buff );
            last += 2 + WORD_SIZE;
            buff += 2 + WORD_SIZE;
            break;
        case FIX_FORWARD:
            save = CurrBuff->fix[i].p;
            save->segment = seg;
            save->offset = AskLocation();
            break;
        }
    }
    DataBytes( CurrBuff->index - last, buff );
    SetOP( old );
}


extern  uint    BuffLoc( void ) {
/*************************/


    return( CurrBuff->index );
}


extern  void    BuffPatch( byte val, uint loc ) {
/***********************************************/

    CurrBuff->buff[  loc  ] = val;
}


extern  void    BuffByte( byte b ) {
/**********************************/

    if( CurrBuff->index < DB_BUFF_SIZE ) {
        CurrBuff->buff[  CurrBuff->index++  ] = b;
        if( CurrBuff->index >= DB_BUFF_SIZE ) {
            _Zoiks( ZOIKS_005 );
        }
    }
}


extern  void    BuffWord( uint w ) {
/**********************************/

    BuffByte( w & 0xff );
    BuffByte( w >> 8 );
}


extern  void    BuffDWord( unsigned_32 w ) {
/******************************************/

    BuffWord( w & 0xffff );
    BuffWord( w >> 16 );
}


extern  void    BuffOffset( offset w )
/************************************/
{
#if _TARG_INTEGER == 32
    BuffDWord( w );
#else
    BuffWord( w );
#endif
}


extern  void    BuffValue( unsigned_32 val, uint class ) {
/********************************************************/


    switch( class ) {
    case 0:
        BuffByte( val );
        break;
    case 1:
        BuffWord( val );
        break;
    case 2:
        BuffWord( val & 0xffff );
        BuffWord( val >> 16 );
        break;
    }
}


extern  void    BuffRelocatable( pointer ptr, fixup_kind type, offset off ) {
/***************************************************************************/

    CurrBuff->fix[CurrBuff->fix_idx].pos = CurrBuff->index;
    CurrBuff->fix[CurrBuff->fix_idx].p = ptr;
    CurrBuff->fix[CurrBuff->fix_idx].type = type;
    if( type == FIX_FORWARD ) {
        BuffWord( 0 );
    } else {
        BuffOffset( off );
        CurrBuff->index += 2;
    }
    CurrBuff->fix_idx++;
}


extern  void    BuffBack( pointer back, int off ) {
/*************************************************/

    BuffRelocatable( back, FIX_BACKHANDLE, off );
}


extern  void    BuffAddr( pointer sym ) {
/***************************************/

    BuffRelocatable( sym, FIX_SYMBOL, 0 );
}


extern  void    BuffForward( dbg_patch_handle *handle ) {
/*******************************************************/

    BuffRelocatable( handle, FIX_FORWARD, 0 );
}


extern  void    BuffWSLString( char *str ) {
/******************************************/

    while( *str != NULLCHAR ) {
        BuffByte( *str );
        ++str;
    }
}


extern  void    BuffString( uint len, char *str ) {
/*************************************************/

    while( len > 0 ) {
        BuffByte( *str );
        ++str;
        --len;
    }
}


extern  void    BuffIndex( uint tipe ) {
/**************************************/

    if( tipe < 0x80 ) {
        BuffByte( tipe );
   } else {
        BuffByte( 0x80 | (tipe >> 8) );
        BuffByte( tipe & 0xff );
    }
}


static    hw_reg_set    HWRegValues[] = {
    HW_D( HW_AL ),  HW_D( HW_AH ),  HW_D( HW_BL ),
    HW_D( HW_BH ),  HW_D( HW_CL ),  HW_D( HW_CH ),
    HW_D( HW_DL ),  HW_D( HW_DH ),
    HW_D( HW_AX ),  HW_D( HW_BX ),  HW_D( HW_CX ),
    HW_D( HW_DX ),  HW_D( HW_SI ),  HW_D( HW_DI ),
    HW_D( HW_BP ),  HW_D( HW_SP ),
    HW_D( HW_CS ),  HW_D( HW_SS ),  HW_D( HW_DS ),  HW_D( HW_ES ),
    HW_D( HW_ST0 ), HW_D( HW_ST1 ), HW_D( HW_ST2 ),
    HW_D( HW_ST3 ), HW_D( HW_ST4 ), HW_D( HW_ST5 ),
    HW_D( HW_ST6 ), HW_D( HW_ST7 ),
    HW_D( HW_EAX ), HW_D( HW_EBX ), HW_D( HW_ECX ),
    HW_D( HW_EDX ), HW_D( HW_ESI ), HW_D( HW_EDI ),
    HW_D( HW_BP ), HW_D( HW_SP ),
    HW_D( HW_FS ),  HW_D( HW_GS )
};

static  uint    RegNibble( hw_reg_set hw_reg ) {
/***********************************************/


    uint        ret;

    ret = REG_AL;
    for(;;) {
        if( HW_Equal( HWRegValues[ret], hw_reg ) ) break;
        ++ret;
        if( ret > REG_LAST ) break;
    }
    return( ret );
}


static  uint    MultiReg( register_name *reg ) {
/***********************************************/

    hw_reg_set  hw_reg;
    hw_reg_set  tmp;

    hw_reg = reg->reg;
#if _TARGET & _TARG_IAPX86
    if( HW_CEqual( hw_reg, HW_ABCD ) ) {
        BuffByte( REG_DX );
        BuffByte( REG_CX );
        BuffByte( REG_BX );
        BuffByte( REG_AX );
        return( 4 );
    }
#endif
#if _TARGET & _TARG_80386
    hw_reg = Low64Reg( hw_reg );
#elif _TARGET & _TARG_IAPX86
    hw_reg = Low32Reg( hw_reg );
#endif
    if( HW_CEqual( hw_reg, HW_EMPTY ) ) {
        BuffByte( RegNibble( reg->reg ) );
        return( 1 );
   } else {
        BuffByte( RegNibble( hw_reg ) );
        tmp = reg->reg;
        HW_TurnOff( tmp, hw_reg );
        BuffByte( RegNibble( tmp ) );
        return( 2 );
    }
}

static  void    DoLocDump( dbg_loc loc ) {
/****************************************/

    long        offset;
    uint        reg;
    uint        patch;

    if( loc->next != NULL ) {
        DoLocDump( loc->next );
    }
    switch( loc->class & 0xf0 ) {
    case LOC_CONSTANT:
        if( loc->class == LOC_MEMORY ) {
            BuffByte( LOC_MEMORY );
            BuffAddr( loc->u.fe_sym );
        } else {
            offset = loc->u.val;
            if( offset >= -128 && offset <= 127 ) {
                BuffByte( LOC_CONST_1 );
                BuffByte( offset );
           } else if( offset >= -32768 && offset <= 32767 ) {
                BuffByte( LOC_CONST_2 );
                BuffWord( offset );
            } else {
                BuffByte( LOC_CONST_4 );
                BuffDWord( offset );
            }
        }
        break;
    case LOC_BP_OFFSET:
        offset = NewBase( loc->u.be_sym );
        if( offset >= -128 && offset <= 127 ) {
            BuffByte( LOC_BP_OFFSET + BP_BYTE );
            BuffByte( offset );
       } else if( offset >= -32768 && offset <= 32767 ) {
            BuffByte( LOC_BP_OFFSET + BP_WORD );
            BuffWord( offset );
        } else {
            BuffByte( LOC_BP_OFFSET + BP_DWORD );
            BuffDWord( offset );
        }
        break;
    case LOC_REG:
        reg = RegNibble( loc->u.be_sym->r.reg );
        if( reg > 15 ) {
            patch = BuffLoc();
            BuffByte( 0 );
            BuffPatch( LOC_MULTI_REG | (MultiReg( &loc->u.be_sym->r )-1), patch );
        } else {
            BuffByte( LOC_REG | reg );
        }
        break;
    case LOC_IND_REG:
        if( HW_CEqual( loc->u.be_sym->r.reg, HW_EMPTY ) ) {
            //NYI: structured return value on the stack. Have to do something
            //       suitable. For now, output a no location.
            BuffByte( 0 );
        } else {
            reg = RegNibble( loc->u.be_sym->r.reg );
            if( reg > REG_LAST ) {
                BuffByte( loc->class + 1 ); /* assumes ..._FAR is one greater*/
                MultiReg( &loc->u.be_sym->r );
            } else {
                BuffByte( loc->class );
                BuffByte( reg );
            }
        }
        break;
    case LOC_OPER:
        BuffByte( loc->class );
        if( loc->class == LOC_OPER+LOP_XCHG ) {
            BuffByte( loc->u.stk );
        }
        break;
    default:
        BuffByte( 0 ); /* No location*/
        break;
    }
}

extern  void    LocDump( dbg_loc loc ) {
/**************************************/

    uint        patch;

    if( loc != NULL ) {
        if( loc->next != NULL ) {
            patch = BuffLoc();
            BuffByte( 0 );
            DoLocDump( loc );
            BuffPatch( LOC_EXPR_IND | (BuffLoc() - patch), patch );
        } else {
            DoLocDump( loc );
        }
    } else {
        BuffByte( LOC_OPER+LOP_NOP );
    }
    DBLocFini( loc );
}
