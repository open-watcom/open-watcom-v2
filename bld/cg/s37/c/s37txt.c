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


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "cgstd.h"
#include "cg.h"
#include "cgaux.h"
#include "bckdef.h"
#include "targsys.h"
#include "system.h"
#include "offset.h"
#include "s37bead.h"
#include "obj370.h"
#include "model.h"
#include "zoiks.h"



typedef int handle;
extern    handle        ObjFile;


extern  pointer         FEAuxInfo(pointer*,aux_class);
extern  int             Length( const char* );
extern  void            PLBlip();
extern void CloseStream(handle );
extern void PutObjRec(int ,void *,uint );


typedef struct src_fi {
    handle fi;
    int    curr_line;
    char  *curr;
    char   buff[80+1];
} src_fi;


extern src_fi *CGSrcOpen(src_fi *,char *);
extern int CGSrcGet(src_fi *,char *,int ,int );
extern void CGSrcClose(src_fi *);
typedef struct asm_fi  asm_fi;
extern asm_fi *AsmOpen(char *,int ,bool );
extern void PutLine(asm_fi *,char *,int );
extern void AsmClose(asm_fi *);
extern void PutStream(handle ,byte *,uint );
extern int OpenObj(char *);
extern void CloseObj(void);

extern hwop_info const HWOpTable[];
extern int const HWOpICL[];
static  char const BRMnem[16][4]={
            "NOP",
            "BO",
            "BH",
            "",
            "BL",
            "",
            "",
            "BNE",
            "BE",
            "",
            "",
            "BNL",
            "",
            "BNH",
            "BNO",
            "B"
};

static const rec_tag tag_ESD = { PUNCH_12_2_9, { 0xC5,0xE2,0xC4 } };
static const rec_tag tag_TXT = { PUNCH_12_2_9, { 0xE3,0xE7,0xE3 } };
static const rec_tag tag_RLD = { PUNCH_12_2_9, { 0xD9,0xD3,0xC4 } };
static const rec_tag tag_END = { PUNCH_12_2_9, { 0xC5,0xD5,0xC4 } };


typedef enum {
    LST_NONE = 0x00,
    LST_ASM  = 0x01,
    LST_SRC  = 0x02,
    LST_OBJ  = 0x04
} lst_opts;

typedef struct asm_obj {
    char    label[8];
    char    sep1[1];
    char    code[5];
    char    sep2[1];
    char    ops[24];
    char    sep3[1];
    char    com[31];
    char    cont[1];
    char    id[8];
}asm_obj;

typedef struct obj_area {
    char  c[32];
}obj_area;

typedef struct use_info   {
    offset     base_disp;
    char       base_reg;
}use_info;

typedef struct txt_obj {
    offset      address;
    short int   csect;
    short int   rem;
    char       *curr;
    txt_record  rec;
    use_info    using;
    lst_opts    opts;
    src_fi     *srcfi;
    asm_fi     *asmfi;
    asm_obj    *asm;
    obj_area   *ocodes;
    int         size;
    char        line[122];
} txt_obj;

#define TXT_MAX 56

typedef struct rld_obj {
    short int   csect;
    short int   rem;
    char        *curr;
    rld_record  rec;
} rld_obj;

#define RLD_MAX 56

#include "s37txt.def"
/* file types */
#if _HOSTOS & _CMS
    #define TXT_SUFFIX  " text"
    #define ASM_SUFFIX  " assemble"
    #define LST_SUFFIX  " lst"
#else
    #define TXT_SUFFIX  ".tex"
    #define ASM_SUFFIX  ".asm"
    #define LST_SUFFIX  ".lst"
#endif
extern void GenTXT( bead_xsym *entry, txtseg_rec *rec ) {
/*** generate ESD TXT RLD END for segment *************/
    char datname[20];
    if( rec->txtseg == TXT_DATA ) { /* abit crude for now */
        CloseObj();
        strcpy( datname, rec->csect->sym->name );
        strcat( datname, TXT_SUFFIX );
        ObjFile = OpenObj( datname );
    }
   EsdRecs( rec );
   TxtRecs( entry, rec );
   RldRecs( rec );
   EndRecs( entry, rec );
}

static void ESDInitRec ( esd_record *rec ) {
/*************************************/

    memset( rec, 0x40, sizeof( esd_record ) );
    rec->tag= tag_ESD;
}


static void    EsdRecs( txtseg_rec *rec ) {
/*** set psdid esdid's write ESDS, look for cross seg refs    ***/
/*** also give local labels a name if a listing is being made ***/
    esd_record    esds;
    esd_item      *curr_item;
    int           pesdid;
    int           csectid;
    int           rec_set;
    offset        sym_off;
    bead_def      *bead;
    int            refno;

    ESDInitRec( &esds );
    pesdid = 1;
    rec_set = false;
    csectid = 1;
    bead = rec->first;
    curr_item = &esds.items[0];
    if( _IsModel( ASM_OUTPUT ) || _IsModel( ASM_LISTING ) ) {
        refno = 1;
    }else{
        refno = 0;
    }
    while( bead ) {
        if( bead->class == BEAD_XSYM ) {
            if( curr_item >= &esds.items[3] ) {
                Stick16( &esds.size,(char*)curr_item - (char*)&esds.items[0] );
                PutObjRec( ObjFile, &esds, 80 );
                ESDInitRec( &esds );
                curr_item = &esds.items[0];
                rec_set = false;
            }
            memset( curr_item->name, 0x40, 8 );
            TXTSym( curr_item->name, ((bead_xsym *)bead)->sym, true );
            switch( ((bead_xsym *)bead)->class ) {
            case XSYM_CSECT:
                ((bead_xsym *)bead)->id = pesdid;
                curr_item->type = ESDT_SD;
                Stick24( &curr_item->addr, bead->address );
                Stick24( &curr_item->extra, rec->location );
                curr_item->align = ((bead_xsym*)bead)->armode;
                if( !rec_set ) {
                    Stick16( &esds.esdid, pesdid );
                    rec_set = true;
                }
                csectid = pesdid;
                pesdid++;
                break;
            case XSYM_EXTRN:
                /* in case of more than one extern */
                ((bead_xsym *)bead)->sym->def = bead;
                ((bead_xsym *)bead)->id = pesdid;
                curr_item->type = ESDT_ER;
                if( !rec_set ) {
                    Stick16( &esds.esdid, pesdid );
                    rec_set = true;
                }
                pesdid++;
                break;
            case XSYM_ENTRY:
                ((bead_xsym *)bead)->id = csectid;
                curr_item->type = ESDT_LD;
                sym_off = ((bead_xsym *)bead)->sym->def->address;
                Stick24( &curr_item->addr, sym_off );
                Stick24( &curr_item->extra, csectid );
                break;
            }
           curr_item++;
        }else if( bead->class == BEAD_ADDR ) {
            FixRef( rec, (bead_addr*)bead );
        }else if( refno > 0 && bead->class == BEAD_LABEL ){
        /** if a local sym doesn't have a name give it one ***/
            hw_sym *sym;

            sym = ((bead_label *)bead)->sym;
            if( sym->name[0] == '\0' ){
                sym->name[0] = '#';
                itoa( refno, &sym->name[1], 10 );
                ++refno;
            }
        }
        bead = bead->next;
    }
    if( curr_item > &esds.items[0] ) {
        Stick16( &esds.size, (char *)curr_item - (char*)&esds.items[0] );
        PutObjRec( ObjFile, &esds, 80 );
    }
}

static void FixRef( txtseg_rec *rec, bead_addr *bead ) {
/*** change a ref to a sym in another CSECT to CSECT+offset ***/
    hw_sym *sym;

    sym = bead->ref;
    if( !(sym->defflag & rec->txtseg)  ) {
        bead->val += sym->def->address;
        bead->ref = rec->other->sym;
    }
}

static void InitTxtObj( txt_obj *obj ) {
/*************************************/

    memset( &obj->rec, 0x40, sizeof( obj->rec ) );
    obj->rec.tag = tag_TXT;
    obj->rem = TXT_MAX;
    obj->address = 0;
    obj->curr = obj->rec.info;
    obj->opts = LST_NONE;
    obj->using.base_reg = 0;
    obj->using.base_disp = -1;
}

static void TxtFlush( txt_obj *obj ) {
/*************************************/

    int size;

    size = TXT_MAX - obj->rem;
    if( size != 0 ) {
        Stick16( &obj->rec.esdid, obj->csect );
        Stick24( obj->rec.faddr, obj->address );
        Stick16( obj->rec.size, size );
        PutObjRec( ObjFile, &obj->rec, 80 );
        obj->rem = TXT_MAX;
        obj->curr = obj->rec.info;
        obj->address += size;
        memset( obj->rec.info, 0x40, sizeof( obj->rec.info ) );
    }
}

static void PutAsm( txt_obj *obj ) {
/*************************************/
    PutLine( obj->asmfi, obj->line, obj->size );
    memset( &obj->line, ' ',  obj->size  );
}


static void TxtOut( txt_obj *obj, offset address, char *dat, int len ) {
/*****************************************************/
    int size;

    size = TXT_MAX- obj->rem;
    if( obj->address+size != address ) { /* do an ORG */
        TxtFlush( obj );
        obj->address = address;
    }
    while( len > 0 ) {
        if( obj->rem == 0 ) {
            TxtFlush( obj );
        }
        *obj->curr++ = *dat++;
        obj->rem--;
        len--;
    }
}

static void TxtRecs( bead_xsym *entry, txtseg_rec *rec ) {
/*****************************************/

    txt_obj     txt;
    src_fi      src_entry;
    char        datname[20];

    InitTxtObj( &txt );
    if( _IsModel( ASM_OUTPUT ) ) {
        txt.opts = LST_ASM;
        if( _IsModel( ASM_SOURCE ) ) {
            txt.srcfi = CGSrcOpen( &src_entry, FEAuxInfo( NULL, SOURCE_NAME ) );
            txt.opts |= LST_SRC;
        }else{
            txt.srcfi = NULL;
        }
        memset( &txt.line, ' ', sizeof( txt.line ) );
        if( _IsModel( ASM_LISTING ) ) {
            txt.opts |= LST_OBJ;
            txt.asm = (asm_obj *)&txt.line[sizeof(obj_area)+1];
            txt.ocodes = (obj_area *)&txt.line[0];
            txt.size = 121;
        } else {
            txt.asm = (asm_obj *)&txt.line[0];
            txt.size = 80;
        }
        if( rec->txtseg == TXT_DATA ) { /* abit crude for now */
            strcpy( datname, rec->csect->sym->name );
            strcat( datname, txt.opts & LST_OBJ ? LST_SUFFIX:ASM_SUFFIX );
            txt.asmfi = AsmOpen( datname, txt.size, txt.opts & LST_OBJ );
        } else {
            txt.asmfi = AsmOpen( FEAuxInfo( NULL, ASM_NAME ), txt.size,
                                                     txt.opts & LST_OBJ );
        }
    }
    PutBeads( &txt, rec->first );
    if( txt.opts & LST_SRC ) {
        CGSrcClose( txt.srcfi );
    }
    if( txt.opts & LST_ASM ) {
        fmt_str( txt.asm->code, "END" );
        if( entry != NULL ) {
           OutSym( txt.asm->ops, entry->sym, true );
        }
        PutAsm( &txt );
        AsmClose( txt.asmfi );
    }
    TxtFlush( &txt );
}

static void PutBeads( txt_obj *obj, bead_def *bead ){
/************************/
    while( bead != NULL ) {
        PLBlip();
        switch( bead->class ) {
        case BEAD_HWINS:
        case BEAD_BR:
            DmpHwIns( obj, (any_bead_hwins*)bead );
            break;
        case BEAD_LABEL:
            DmpLabel( obj, (bead_label*)bead );
            break;
        case BEAD_DATA:
            DmpData( obj, (bead_data*)bead );
            break;
        case BEAD_BINDEX:
            DmpBindex( obj, (bead_bindex*)bead );
            break;
        case BEAD_ADDR:
            DmpAddr( obj, (bead_addr*)bead );
            break;
        case BEAD_INT:
            DmpInt( obj, (bead_int*)bead );
            break;
        case BEAD_FLT:
            DmpFlt( obj, (bead_flt*)bead );
            break;
        case BEAD_DISP:
            DmpDisp( obj, (bead_disp*)bead );
            break;
        case BEAD_XSYM:
            DmpXsym( obj, (bead_xsym*)bead );
            break;
        case BEAD_USING:
            DmpUsing( obj, (bead_using*)bead );
            break;
        case BEAD_DROP:
            DmpDrop( obj, (bead_drop*)bead );
            break;
        case BEAD_LTORG: /* returns next bead */
            bead = DmpLtorg( obj, (bead_ltorg*)bead );
            break;
        case BEAD_STARTPROC: /* internal queues */
            DmpMsg( obj, "STARTPROC" );
            break;
        case BEAD_EPILOGUE:
            DmpMsg( obj, "EPILOGUE" );
            break;
        case BEAD_ENDPROC:
            DmpMsg( obj, "ENDPROC" );
            break;
        case BEAD_QUEUE:
            DmpQueue( obj, (bead_queue*)bead );
            break;
        case BEAD_SEG:
           DmpSeg( obj, (bead_seg*)bead );
           break;
        case BEAD_STORE:
            DmpStore( obj, (bead_store*)bead );
            break;
        default:
            Zoiks( ZOIKS_061 );
        }
        if( ( obj->opts & LST_ASM )&& (bead->class != BEAD_QUEUE) ){
            PutAsm( obj );
        }
        bead = bead->next;
    }
}

static  short int GetDispSX(  txt_obj *obj, hwins_op_sx *sx ) {
/*********************************************/

    long int disp;
    bead_def *bead;
    offset    base_disp;

    disp = sx->disp;
    if( sx->ref != NULL ) {
        if( sx->ref->entry.class == REF_LIT ) { /* lit ref */
            bead = sx->ref->lit.def;
            base_disp = obj->using.base_disp;
            sx->b = obj->using.base_reg;
        } else if( sx->ref->entry.class == REF_SYM) {/* sym ref */
            bead = sx->ref->sym.sym->def;
            base_disp = obj->using.base_disp;
            sx->b = obj->using.base_reg;
        } else if( sx->ref->entry.class == REF_DISP ) {/* sym ref */
            bead = sx->ref->disp.base->def;
            base_disp = bead->address;
            bead = sx->ref->disp.sym->def;
        }
        disp += bead->address-base_disp;
    }
    if( disp > 4095 || disp < 0 ) {
        if( obj->opts & LST_ASM ) {
            fmt_str( obj->asm->com, "ADDRESS ERROR" );
        }
        Zoiks( ZOIKS_062 ); /* addressability error */
    }
    return( disp );

}

static  offset GetAddrSX( hwins_op_sx *sx ) {
/*** Get addr of sx from start of CSECT   ****/

    bead_def *bead;
    offset    ref_addr;

    ref_addr = sx->disp;
    if( sx->ref != NULL ) {
        if( sx->ref->entry.class == REF_LIT ) { /* lit ref */
            bead = sx->ref->lit.def;
            ref_addr = bead->address;
        } else if( sx->ref->entry.class == REF_SYM) {/* sym ref */
            bead = sx->ref->sym.sym->def;
            ref_addr = bead->address;
        }
    }
    return( ref_addr );

}

static void DmpHwIns( txt_obj *obj, any_bead_hwins *bead  ) {
/****************************************/

    hwins_class    hwclass;
    hwins_opcode   opcode;
    int            inslen;
    char           out[6];


    char           inscode;

    enum ON_flags{
         ON_none = 0,
         ON_b2 = 0x1,
         ON_n1 = 0x2,
         ON_n2 = 0x4,
         ON_d1 = 0x8,
         ON_d2 = 0x10
    }flags;

    char           b2;
    char           n1;
    char           n2;
    hwins_op_sx    *d1;
    hwins_op_sx    *d2;

    hwclass = bead->ins.class;
    opcode = bead->ins.opcode;
    inslen = HWOpICL[hwclass];
    inscode   = HWOpTable[opcode].inscode;

    flags = ON_none;
    switch( hwclass ) {
    case HWINS_RR:
        n1 = bead->rr.r1;
        n2 = bead->rr.r2;
        flags = ON_n1 + ON_n2;
        break;
    case HWINS_RX:
        n1 = bead->rx.r1;
        n2 =  bead->rx.s2.a;
        d1 =  &bead->rx.s2;
        flags = ON_n1 + ON_n2 + ON_d1;
        break;
    case HWINS_RS1:
        n1 = bead->rs1.r1;
        n2 = bead->rs1.r3;
        d1 = &bead->rs1.s2;
        flags = ON_n1 + ON_n2 + ON_d1;
        break;
    case HWINS_RS2:
        n1 = bead->rs2.r1;
        d1 = &bead->rs2.s2;
        flags = ON_n1 + ON_d1;
        break;
    case HWINS_SI:
        b2 = bead->si.i2;
        d1 = &bead->si.s1;
        flags = ON_b2 + ON_d1;
        break;
    case HWINS_S:
        d1 = &bead->s.s2;
        flags = ON_d1;
        break;
    case HWINS_SS1:
        b2 = bead->ss1.s1.a-1;
        d1 = &bead->ss1.s1;
        d2 = &bead->ss1.s2;
        flags = ON_b2 + ON_d1 + ON_d2;
        break;
    case HWINS_SS2:
        n1 = bead->ss2.s1.b-1;
        n2 = bead->ss2.s2.a-1;
        d1 = &bead->ss2.s1;
        d2 = &bead->ss2.s2;
        flags = ON_n1 + ON_n2 + ON_d1 + ON_d2;
        break;
    case HWINS_SSP:
        n1 = bead->ssp.s1.b-1;
        n2 = bead->ssp.s2.a;
        d1 = &bead->ssp.s1;
        d2 = &bead->ssp.s2;
        flags = ON_n1 + ON_n2 + ON_d1 + ON_d2;
        break;
    default:
        Zoiks( ZOIKS_063 );
    }
    out[0] = inscode;
    memset( &out[1], 0, 5 );
    if( flags & ON_b2 ) {
        out[1] = b2;
    }
    if( flags & ON_n1 ) {
        out[1] = n1<<4;
    }
    if( flags & ON_n2 ) {
        out[1] |= n2;
    }
    if( flags & ON_d1 ) {
        Stick16( &out[2], GetDispSX( obj, d1 ) );
        out[2] |= d1->b << 4;
    }
    if( flags & ON_d2 ) {
        Stick16( &out[4], GetDispSX( obj, d2 ) );
        out[4] |= d2->b << 4;
    }
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->ins.common.address, 5 );
            fmt_lxstr( &obj->ocodes->c[6], &out[0], 2 );
            if( inslen > 2 ) {
                fmt_lxstr( &obj->ocodes->c[11], &out[2], 2 );
            }
            if( inslen > 4 ) {
                fmt_lxstr( &obj->ocodes->c[16], &out[4], 2 );
            }
            if( flags & ON_d1 ) {
                fmt_hex( &obj->ocodes->c[21], GetAddrSX( d1 ), 5);
            }
            if( flags & ON_d2 ) {
                fmt_hex( &obj->ocodes->c[27], GetAddrSX( d2 ), 5 );
            }
        }
        AsmHwIns( obj, bead );
    }
    TxtOut( obj, bead->ins.common.address, out, inslen );
}

static char *DmpLitRef( char *cur, ref_lit *lit ) {
/***************************************/
    bead_def  *bead;

    bead = lit->def;
    *cur++ = '=';
    switch( bead->class ) {
    case BEAD_DATA:
        cur = PrtData( cur, (bead_data *)bead );
        break;
    case BEAD_ADDR:
        cur = PrtAddr( cur, (bead_addr *)bead );
        break;
    case BEAD_INT:
        cur = PrtInt( cur,  (bead_int *)bead );
        break;
    case BEAD_FLT:
        cur = PrtFlt( cur,  (bead_flt *)bead );
        break;
    }
    return( cur );
}

static  char *DmpSX( char *cur,  hwins_op_sx *sx, int a ) {
/*********************************************/

    hw_sym *sym;
    hw_sym *base;

    if( sx->ref == NULL ) {
        cur = fmt_dec( cur, sx->disp );
        *cur++ = '(';
        if( a ) {
            cur = fmt_dec( cur, sx->a );
            *cur++ = ',';
        }
        cur = fmt_dec( cur, sx->b );
        *cur++ = ')';
    } else {
        if( sx->ref->entry.class == REF_LIT ) { /* lit ref */
            cur = DmpLitRef( cur, sx->ref );
        } else if( sx->ref->entry.class == REF_SYM) {/* sym ref */
            sym = (hw_sym *)sx->ref->sym.sym;
            cur = OutSym( cur, sym, true );
            if( sx->disp != 0 ) {
                *cur++ = '+';
                cur = fmt_dec( cur, sx->disp );
            }
        } else if( sx->ref->entry.class == REF_DISP) {/* explicit base */
            sym = (hw_sym *)sx->ref->disp.sym;
            base = (hw_sym *)sx->ref->disp.base;
            cur = OutSym( cur, sym, true );
            if( sx->disp < 0 ) {
                cur = fmt_dec( cur, sx->disp );
            }else if( sx->disp > 0 ) {
                *cur++ = '+';
                cur = fmt_dec( cur, sx->disp );
            }
            *cur++ = '-';
            cur = OutSym( cur, base, true );
            *cur++ = '(';
            *cur++ = ',';
            cur = fmt_dec( cur, sx->b );
            *cur++ = ')';
        }
    }
    return( cur );
}

static void AsmHwIns( txt_obj *obj, any_bead_hwins *bead  ) {
/********** dump assemble mnemonics******/
    hwins_class    hwclass;
    hwins_opcode   opcode;
    char          *cur;
    char const    *brmnem;

    enum asm_op { /* asm format of fields    */
        ASM_NULL, /* field not used          */
        ASM_R,    /* field is reg            */
        ASM_S,    /* field is disp(base)     */
        ASM_SX,   /* field is disp( l|r,base)*/
        ASM_I     /* field is immediate      */
    };

    struct { /* assembler field  */
        enum asm_op class;
        union {
            hwins_op_sx  *sx;
            char          r;
            char          i;
        }op;
    } f[3], *cur_f;
    int fcount;  /* number of operands     */


    hwclass = bead->ins.class;
    opcode = bead->ins.opcode;
    switch( hwclass ) {
    case HWINS_RR:
        f[0].class = ASM_R;
        f[0].op.r = bead->rr.r1;
        f[1].class = ASM_R;
        f[1].op.r = bead->rr.r2;
        fcount = 2;
        break;
    case HWINS_RX:
        f[0].class = ASM_R;
        f[0].op.r = bead->rx.r1;
        f[1].class = ASM_SX;
        f[1].op.sx = &bead->rx.s2;
        fcount = 2;
        break;
    case HWINS_RS1:
        f[0].class = ASM_R;
        f[0].op.r = bead->rs1.r1;
        f[1].class = ASM_R;
        f[1].op.r = bead->rs1.r3;
        f[2].class = ASM_S;
        f[2].op.sx = &bead->rs1.s2;
        fcount = 3;
        break;
    case HWINS_RS2:
        f[0].class = ASM_R;
        f[0].op.r = bead->rs2.r1;
        f[1].class = ASM_S;
        f[1].op.sx = &bead->rs2.s2;
        fcount = 2;
        break;
    case HWINS_SI:
        f[0].class = ASM_S;
        f[0].op.sx = &bead->si.s1;
        f[1].class = ASM_I;
        f[1].op.i =  bead->si.i2;
        fcount = 2;
        break;
    case HWINS_S:
        f[0].class = ASM_S;
        f[0].op.sx = &bead->s.s2;
        fcount = 1;
        break;
    case HWINS_SS1:
        f[0].class = ASM_SX;
        f[0].op.sx = &bead->ss1.s1;
        f[1].class = ASM_S;
        f[1].op.sx = &bead->ss1.s2;
        fcount = 2;
        break;
    case HWINS_SS2:
        f[0].class = ASM_SX;
        f[0].op.sx = &bead->ss2.s1;
        f[1].class = ASM_SX;
        f[1].op.sx = &bead->ss2.s2;
        fcount = 2;
        break;
    case HWINS_SSP:
        f[0].class = ASM_SX;
        f[0].op.sx = &bead->ssp.s1;
        f[1].class = ASM_S;
        f[1].op.sx = &bead->ssp.s2;
        f[2].class = ASM_I;
        f[2].op.i = bead->ssp.s2.a;
        fcount = 3;
        break;
    default:
        Zoiks( ZOIKS_063 );
    }
    cur_f = f;
    if( opcode == HWOP_BCR || opcode == HWOP_BC ) {
        brmnem = BRMnem[ f[0].op.r ];
        if( *brmnem != '\0' ) {
            cur = fmt_str( obj->asm->code, brmnem );
            if( opcode == HWOP_BCR ) {
                *cur = 'R';
            }
            ++cur_f;
            --fcount;
        } else {
            fmt_str( obj->asm->code, HWOpTable[opcode].name );
        }
    } else {
        fmt_str( obj->asm->code, HWOpTable[opcode].name );
    }
    cur = obj->asm->ops;
    for(;;) {
        switch( cur_f->class ) {
        case ASM_R:
            cur = fmt_dec( cur, cur_f->op.r );
            break;
        case ASM_S:
            cur = DmpSX( cur, cur_f->op.sx, false );
            break;
        case ASM_SX:
            cur = DmpSX( cur, cur_f->op.sx, true );
            break;
        case ASM_I:
             cur = fmt_dec( cur, cur_f->op.i );
            break;
        }
        if( --fcount == 0 )break;
        *cur++ = ',';
        ++cur_f;
    }
}

static void DmpLabel( txt_obj *obj, bead_label *bead ) {
/****************************************/
    hw_sym *sym;
    char *code;
    char const cd[] = "*XH?F???D";
    char align;

    sym = bead->sym;
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->common.address, 5 );
        }
        OutSym( obj->asm->label, sym, true );
        align = bead->align;
        switch( bead->class ) {
        case LBL_EQU:
            code = "EQU";
            break;
        case LBL_DS:
            code = "DS";
            break;
        case LBL_DC:
            code = "DC";
            break;
        }
        fmt_str( obj->asm->code, code );
        obj->asm->ops[0] = '0';
        obj->asm->ops[1] = cd[align];
    }
}

static void DmpBindex( txt_obj *obj, bead_bindex *bead  ) {
/****************************************/

    if( obj->opts & LST_ASM ) {
        obj->asm->code[0] = '*';
        fmt_str( obj->asm->ops, "SWITCH" );
    }
    if( bead->size != 0 ) {
        Zoiks( ZOIKS_064 ); /* unexpanded macro */
    }
}

static void DmpData( txt_obj *obj, bead_data *bead ) {
/**************************************/

    int count;
    offset addr;

    addr = bead->common.address;
    for( count = bead->rep; count > 0; --count ) {
        TxtOut( obj, addr, bead->value, bead->length );
        addr+= bead->length;
    }
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->common.address, 5 );
            count = bead->length;
            if( count > 7 ) {
                count = 7;
            }
            fmt_lxstr( &obj->ocodes->c[6], bead->value, count );
        }
        fmt_str( obj->asm->code,"DC" );
        PrtData( obj->asm->ops, bead );
    }
}

static void DmpStore( txt_obj *obj, bead_store *bead ) {
/**************************************/

    if( obj->opts & LST_ASM ) {
        fmt_str( obj->asm->code,"DS" );
        obj->asm->ops[0] = 'X';
        obj->asm->ops[1] = 'L';
        fmt_dec( &obj->asm->ops[2], bead->length );
    }
}

static void DmpAddr( txt_obj *obj, bead_addr *bead ) {
/**************************************/

    byte out[4];
    offset num_part;
    bead_def *def;

    num_part = 0;
    if( bead->ref->def != NULL ) {
        def = bead->ref->def;
        if( def->class != BEAD_XSYM ) {
             num_part = def->address;
        }
    }
    num_part += bead->val;
    Stick32( out, num_part );
    TxtOut( obj, bead->common.address, out, 4 );
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->common.address, 5 );
            fmt_hex( &obj->ocodes->c[6], num_part, 8 );
        }
        fmt_str( obj->asm->code,"DC" );
        PrtAddr( obj->asm->ops, bead );
    }
}

static void DmpInt( txt_obj *obj, bead_int *bead ) {
/**************************************/

    byte out[4];
    offset num_part;

    num_part = bead->value;
    switch( bead->size ) {
    case 1 :
        out[0] = num_part&0xff;
        break;
    case 2 :
        Stick16( out, num_part );
        break;
    case 3 :
        Stick24( out, num_part );
        break;
    case 4 :
        Stick32( out, num_part );
        break;
    }
    TxtOut( obj, bead->common.address, out, bead->size );
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->common.address, 5 );
            fmt_hex( &obj->ocodes->c[6], num_part, bead->size*2 );
        }
        fmt_str( obj->asm->code,"DC" );
        PrtInt( obj->asm->ops, bead );
    }
}

static void DmpFlt( txt_obj *obj, bead_flt *bead ) {
/**************************************/

    byte out[8];

    fmt_double( out, bead->value );
    TxtOut( obj, bead->common.address, out, bead->size );
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->common.address, 5 );
            fmt_lxstr( &obj->ocodes->c[6], out, bead->size );
        }
        fmt_str( obj->asm->code,"DC" );
        PrtFlt( obj->asm->ops, bead );
    }
}

static void DmpDisp( txt_obj *obj, bead_disp *bead ) {
/**************************************/

    byte out[4];
    offset num_part;
    bead_def *def;

    num_part = 0;
    def = bead->ref->def;
    num_part = def->address;
    num_part += bead->val;
    def = bead->base->def;
    if( bead->op_len & DISP_SUB ) {
        num_part -= def->address;
    } else {
        num_part += def->address;
    }
    switch( bead->op_len & DISP_LEN ) {
    case 1 :
        out[0] = num_part&0xff;
        break;
    case 2 :
        Stick16( out, num_part );
        break;
    case 3 :
        Stick24( out, num_part );
        break;
    case 4 :
        Stick32( out, num_part );
        break;
    }
    TxtOut( obj, bead->common.address, out, bead->op_len & DISP_LEN );
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[0], bead->common.address, 5 );
            fmt_hex( &obj->ocodes->c[6], num_part,(bead->op_len&DISP_LEN)*2 );
        }
        fmt_str( obj->asm->code,"DC" );
        PrtDisp( obj->asm->ops, bead );
    }
}

static char *PrtData( char *cur, bead_data *bead ) {
/************** print data bead *******/
    char *data;

    cur = fmt_dec( cur, bead->rep );
    *cur++ = 'X';
    *cur++ = '\'';
    data = bead->value;
    cur = fmt_lxstr( cur, bead->value, bead->length );
    *cur++ = '\'';
    return( cur );
}

static char *PrtAddr( char *cur, bead_addr *bead ) {
/**************************************/
    hw_sym *ref;

    *cur++ = 'A';
    *cur++ = '(';
    ref = bead->ref;
    cur = OutSym( cur, ref, true );
    if( bead->val != 0 ) {
        *cur++ = '+';
        cur = fmt_dec( cur, bead->val );
    }
    ref = bead->rel;
    if( ref != NULL ){
        *cur++ = '-';
        cur = OutSym( cur, ref, true );
    }
    *cur++ = ')';
    return( cur );
}

static char *PrtInt( char *cur, bead_int *bead ) {
/**************************************/

    offset num_part;

    num_part = bead->value;
    switch( bead->size ) {
    case 1 :
        *cur++ = 'X';
        *cur++ = '\'';
        cur = fmt_hex( cur, num_part, 2 );
        *cur++ = '\'';
        break;
    case 2 :
        *cur++ = 'H';
        *cur++ = '\'';
        cur = fmt_dec( cur, num_part );
        *cur++ = '\'';
        break;
    case 3 :
        *cur++ = 'X';
        *cur++ = 'L';
        *cur++ = '3';
        *cur++ = '(';
        cur = fmt_dec( cur, num_part );
        *cur++ = ')';
        break;
    case 4 :
        *cur++ = 'F';
        *cur++ = '\'';
        cur = fmt_dec( cur, num_part );
        *cur++ = '\'';
        break;
    }
    return( cur );
}

static char *PrtFlt( char *cur, bead_flt *bead ) {
/**************************************/

    double num_part;
    char   num_str[20];

    num_part = bead->value;
    if( bead->size == 4 ) {
        *cur++ = 'E';
    } else {
        *cur++ = 'D';
    }
    *cur++ = '\'';
    sprintf( num_str, "%E", num_part );
    cur = fmt_str( cur, num_str );
    *cur++ = '\'';
    return( cur );
}

static char *PrtDisp( char *cur,  bead_disp *bead ) {
/**************************************/

    *cur++ = 'A';
    *cur++ = 'L';
    cur = fmt_dec( cur, bead->op_len & DISP_LEN );
    *cur++ = '(';
    cur = OutSym( cur, bead->ref, true );
    if( bead->val != 0 ) {
        *cur++ = '+';
        cur = fmt_dec( cur, bead->val );
    }
    *cur++ = bead->op_len & DISP_SUB ? '-' : '+';
    cur = OutSym( cur, bead->base, true );
    *cur++ = ')';
    return( cur );
}

static void DmpXsym( txt_obj *obj, bead_xsym *bead ) {
/*********************************/
    char *code;
    char *id_area;
    if( bead->class == XSYM_CSECT ) {
        TxtFlush( obj );
        obj->csect = bead->id;
        obj->address = bead->common.address;
    }
    if( _IsntModel( ASM_OUTPUT ) ) {
        return;
    }
    if( obj->opts & LST_ASM ) {
        switch( bead->class ) {
        case XSYM_CSECT:
            code = "CSECT";
            id_area = obj->asm->label;
            break;
        case XSYM_EXTRN:
            code = "EXTRN";
            id_area = obj->asm->ops;
            break;
        case XSYM_ENTRY:
            code =  "ENTRY ";
            id_area = obj->asm->ops;
            break;
        }
        OutSym( id_area, bead->sym, true );
        fmt_str( obj->asm->code, code );
    }

}

static void DmpUsing( txt_obj *obj, bead_using *bead ) {
/*********************************/
    char *cur;
    bead_def *using_start;

    if( bead->sym ) {
        using_start = bead->sym->def;
    } else {
        using_start = (bead_def* )bead;
    }
    obj->using.base_reg  = bead->reg;
    obj->using.base_disp = using_start->address;
    if( obj->opts & LST_ASM ) {
        if( obj->opts & LST_OBJ ) {
            fmt_hex( &obj->ocodes->c[27], using_start->address, 5 );
        }
        fmt_str( obj->asm->code, "USING" );
        cur = obj->asm->ops;
        if( bead->sym ) {
            cur = OutSym( cur, bead->sym, true );
        } else {
            *cur++ = '*';
        }
        *cur++ = ',';
        fmt_dec( cur, bead->reg );
    }

}

static void DmpDrop( txt_obj *obj, bead_drop *bead ) {
/*********************************/

    obj->using.base_reg  = 0;
    obj->using.base_disp = -1;
    if( obj->opts & LST_ASM ) {
        fmt_str( obj->asm->code, "DROP" );
        fmt_dec( obj->asm->ops, bead->reg );
    }

}

static bead_def *DmpLtorg( txt_obj *obj, bead_ltorg *bead ) {
/*********************************/
    bead_def *current, *done;
    lst_opts  curr_opts;

    current = (bead_def *)bead;
    if( obj->opts & LST_ASM ) {
        fmt_str( obj->asm->code, "LTORG" );
        curr_opts = obj->opts;
        if( !(obj->opts & LST_OBJ) ) {
            obj->opts &= ~LST_ASM;
        }
        current = current->next; /*skip ltorg */
        done = bead->end->next; /* end is last in pool */
        while( current != done ){
            if(  obj->opts & LST_ASM ){
                PutAsm( obj );
            }
            switch( current->class ){
            case BEAD_ADDR:
                DmpAddr( obj, (bead_addr*)current );
                break;
            case BEAD_INT:
                DmpInt( obj, (bead_int*)current );
                break;
            case BEAD_FLT:
                DmpFlt( obj, (bead_flt*)current );
                break;
            default:
                Zoiks( ZOIKS_061 );
            }
            current = current->next;
            if(  obj->opts & LST_ASM ){
                obj->asm->sep1[0] = '=';
            }
        }
        current = bead->end; /* main loop gets next */
        obj->opts = curr_opts;
    }
    return( current );
}

static bool FormatQueue( txt_obj *obj, unsigned num ) {
/******************************************************************/

    if( CGSrcGet( obj->srcfi, &obj->line[7], num, 73 ) ){
        obj->line[0] = '*';
        fmt_rdec( &obj->line[1], num );
        obj->line[6] = ':';
        return( true );
    }else{
        return( false);
    }
}

static void DmpQueue( txt_obj *obj, bead_queue *bead ) {
/****************************************************************/

    bead_queue  *next_queue;
    unsigned    last_line;
    unsigned    i;

    if( !( obj->opts & LST_ASM ) ){
        return;
    }
    if( !( obj->opts & LST_SRC ) ){
        obj->asm->label[0] = '*';
        fmt_str( obj->asm->code, "LINE" );
        fmt_dec( obj->asm->ops, bead->num );
        PutAsm( obj );
        return;
    }
    next_queue = (bead_queue *)bead->common.next;
    for(;;){
        if( next_queue == NULL ) {
            last_line = bead->num;
            break;
        }
        if( next_queue->common.class == BEAD_EPILOGUE) {
            last_line = bead->num;
            break;
        }
        if( next_queue->common.class == BEAD_QUEUE ) {
            last_line = next_queue->num - 1;
            break;
        }
        next_queue = (bead_queue *)next_queue->common.next;
    }
    for( i = bead->num; i <= last_line; ++i ) {
        if( !FormatQueue( obj, i ) ) break;
        PutAsm( obj );
    }
    return;
}


static void DmpSeg( txt_obj *obj, bead_seg *bead ) {
/*********************************/
    char *cur;
    char const cd[] = "*XH?F???D";

    if( obj->opts & LST_ASM ) {
        cur = fmt_str( obj->asm->code, "DS" );
        obj->asm->ops[0] = '0';
        obj->asm->ops[1] = cd[ bead->align ];
        cur =  fmt_str( &obj->asm->ops[12], "SEG " );
        cur =  fmt_str( cur, bead->str );
        ++cur;
        fmt_dec( cur, bead->id );
    }

}

static void DmpMsg( txt_obj *obj, char *msg ) {
/*********************************/

    if( obj->opts & LST_ASM ) {
        obj->asm->label[0] = '*';
        fmt_str( obj->asm->code, msg );
    }

}
static void InitRldObj ( rld_obj *obj ) {
/*************************************/

    memset( &obj->rec, 0x40, sizeof( obj->rec ) );
    obj->rec.tag = tag_RLD;
    obj->rem = RLD_MAX;
    obj->curr = obj->rec.info;
}

static void RldFlush( rld_obj *obj ) {
/*************************************/

    int size;

    size = RLD_MAX - obj->rem;
    if( size != 0 ) {
        Stick16( obj->rec.size, size );
        PutObjRec( ObjFile, &obj->rec, 80 );
        obj->rem = RLD_MAX;
        obj->curr = obj->rec.info;
        memset( obj->rec.info, 0x40, sizeof( obj->rec.info ) );
    }
}


static void RldOut( rld_obj *obj, offset addr,
                    short rel_esdid, char flag ) {
/*****************************************************/
    rld_entry *pos;

    if( obj->rem < sizeof( *pos ) ) {
        RldFlush( obj );
    }
    pos = (rld_entry *)obj->curr;
    Stick16( &pos->relesdid, rel_esdid );
    Stick16( &pos->posesdid, obj->csect );
    Stick24( pos->faddr.addr, addr );
    pos->faddr.flag = flag;
    obj->curr += sizeof( *pos );
    obj->rem -= sizeof( *pos );
}

static void RldRecs(  txtseg_rec *rec ) {
    rld_obj     rld;
    bead_def   *bead;

    bead = rec->first;
    InitRldObj( &rld );
    while( bead != NULL ) {
        switch( bead->class ) {
        case BEAD_ADDR:
            RldAddr( &rld, (bead_addr*)bead );
            break;
        case BEAD_XSYM:
           RldXsym( &rld, (bead_xsym*)bead );
           break;
        }
        bead = bead->next;
    }
    RldFlush( &rld );
}

static short GetRelEsdid( rld_obj *obj, hw_sym  *sym ){
/**** get the relesdid************/
    short      rel_esdid;
    bead_xsym *ref;

    if( sym->def != NULL ) {
        if( sym->def->class == BEAD_XSYM ) {
             ref= (bead_xsym *)sym->def;
             rel_esdid = ref->id;
        } else {
            rel_esdid = obj->csect;
        }
    } else {
        rel_esdid = obj->csect;
    }
    return( rel_esdid );
}

static void RldAddr( rld_obj *obj, bead_addr *bead ) {
/**************************************/
    short    rel_esdid;
    char     flag;

    rel_esdid = GetRelEsdid( obj, bead->ref );
    flag = _RLD_FLAG( RLD_AY, 4, RLD_ADD );
    RldOut( obj, bead->common.address, rel_esdid, flag );
    if( bead->rel != NULL ) {
        rel_esdid = GetRelEsdid( obj, bead->rel );
        flag = _RLD_FLAG( RLD_AY, 4, RLD_SUB );
        RldOut( obj, bead->common.address, rel_esdid, flag );
    }
}

static void RldXsym( rld_obj *obj, bead_xsym *bead ) {
/*********************************/

    if( bead->class == XSYM_CSECT ) {
        obj->csect = bead->id;
    }

}

static void  EndRecs(  bead_xsym *entry, txtseg_rec *rec ) {
/*** Generate end card with optional entry point **********/
    bead_def *bead;
    end_record end;

    rec = NULL;
    memset( &end, 0x40, sizeof( end ) );
    if( entry != NULL ) {
        bead = entry->sym->def;
        Stick16( &end.esdid, entry->id );
        Stick24( &end.entpt, bead->address );
    }
    end.tag = tag_END;
    end._12 = 0xF1;
    end.pgmid[0] = 0xE6;
    end.pgmid[1] = 0xC3;
    end.pgmid[2] = 0xC3;
    PutObjRec( ObjFile, &end, 80 );
}

static void Stick32( char *in, offset num ) {
/** format 32 bit into 4byte number ********/
    in[0] = num>>24 & 0xff;
    in[1] = num>>16 & 0xff;
    in[2] = num>>8  & 0xff;
    in[3] = num     & 0xff;
}

static void Stick24( char *in, offset num ) {
/** format 32 bit into 3byte number ********/
    in[0] = num>>16 & 0xff;
    in[1] = num>>8  & 0xff;
    in[2] = num     & 0xff;
}

static void Stick16( char *in, short num ) {
/** format 16 bit into 2byte number ********/
    in[0] = num>>8  & 0xff;
    in[1] = num     & 0xff;
}

static char *OutSym( char *to, const hw_sym *sym, int flag ) {
/**** Put a symbol out in a list or ASM file*******/
    char const *from;
    char curr;

    from = sym->name;
    if( *from == '\0' ){
        Zoiks( ZOIKS_065 ); /* unamed symbol */
    }
    curr = *from;
    while( curr != '\0' ) {
        if( flag ) {
            curr = curr == '_' ? '$' :  toupper( curr );
        }
        *to++ = curr;
        curr = *++from;
   }
   return( to );
}

static char *TXTSym( char *to, const hw_sym *sym, int flag ) {
/*Put symbol out in 370 TEXT file ************/
  #if( ' ' == 0x20 ) /*ASCII HOST ? */
    extern char const atoetbl[];
  #endif
    char const *from;
    char curr;

    from = sym->name;
    if( *from == '\0' ){
        Zoiks( ZOIKS_065 ); /* unamed symbol */
    }
    curr = *from;
    while( curr != '\0' ) {
        if( flag ) {
            curr = curr == '_' ? '$' :  toupper( curr );
        }
      #if( ' ' == 0x20 ) /*ASCII HOST ? */
        *to++ =  atoetbl[curr];
      #else
        *to++ = curr;
      #endif
        curr = *++from;
   }
   return( to );
}

static char *fmt_str( char *to, const char *from ) {
/*** char copy from to***************************/
    while( *from != '\0' ) {
        *to = *from;
        from++;
        to++;
   }
   return( to );
}

static char *fmt_lxstr( char *to, char *from, int len ) {
/*** copy len hexchar from to ***************r*********/
//  static char const con[16] = "0123456789ABCDEF";
    auto char const con[16] = "0123456789ABCDEF";

    for( ;len > 0;len--) {
        to[0] = con[ *from >> 4  ];
        to[1] = con[ *from & 0x0f ];
        to += 2;
        from++;
   }
   return( to );
}

static char *fmt_dec( char *to, offset num ) {
/*** convert num to dec and  copy **************/
    char buff[33];
    char *from;

    from = buff;
    ltoa( num, from, 10 );
    while( *from != '\0' ) {
        *to = *from;
        from++;
        to++;
   }
   return( to );
}

static char *fmt_rdec( char *to, offset num ) {
/*** convert num to dec right justified **************/
    char buff[33];
    char *from;
    int  len;

    from = buff;
    ltoa( num, from, 10 );
    len = Length( from );
    len = 5-len;
    if( len < 0 ) {
        len = 5;
    }
    to = &to[len];
    while( *from != '\0' ) {
        *to = *from;
        from++;
        to++;
   }
   return( to );
}

static char *fmt_hex( char *to, offset num, int len ) {
/*** convert num to hex and copy len char*********/
    char buff[4];
    char hex[8];
    char *from;

    from = buff;
    Stick32( from, num );
    fmt_lxstr( hex, buff, 4 );
    from = &hex[8-len];
    for( ;len > 0;len--) {
        *to++ = *from++;
   }
   return( to );
}


#if  _HOST == _IBM_370
static void fmt_double( char *to, double num ) {
/*** convert num to IBM370 DOUBLE and  copy *******/
    *(double *)to = num;
}
#else
static void fmt_double( char *to, double num ) {
/*** convert num to IBM370 DOUBLE and  copy *******/
    union { double num; unsigned long i[2]; } image;
    int exp;
    int shift;
    unsigned long high;
    unsigned long low;
    image.num = num;
    high = image.i[1];
    low  = image.i[0];
    if( high == 0 && low == 0 ){ /* if  true 0 */
        exp = 0;
    }else{
        high >>= 20;    /* get sign / exp */
        high &= 0x000007ff; /* get rid of sign */
        exp = high;
        exp -= 1023;       /* un-excess exp    */
        shift = exp % 4 ;  /*  rem from base 16 */
        exp   = exp / 4;   /* turn into base 16 */
        if( shift < 0 ){   /* negative pow2 exponent */
            exp += 64;
            shift+= 4;
        }else{
            exp += 65;        /* excess 64         */
        }
        if( exp < 0 ){
            printf( "underflow" );
        }else if( exp > 127 ){
            printf( "overflow" );
        }
        high = image.i[1];  /* get high halve again */
        if( high & 0x80000000 ){  /* set sign bit */
            exp |= 0x80;
        }
        low = image.i[0];
        high &= 0x000fffff; /*knock off exponent */
        high |= 0x00100000; /*dink in MSB        */
        /*normalize base 16 and leave 8 bit exp/sign free */
        high <<= shift;
        high |= low >>( 32-shift);
        low <<= shift;
    }
    Stick32( to, high );
    to[0] = exp;
    Stick32( &to[4], low );
}
#endif
