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


#include <string.h>
#include <stdlib.h>
#include "disasm.h"
#include "wdismsg.h"

#ifdef __PCODE__
extern int              SwitchCount;
#endif

/*
 * Static function prototypes
 */
static  void                DecodeData( void );
static  void                DecodeCode( void );
static  uint_32             GetNextFix( void );
static  uint_32             GetNextLabel( void );
static  scan_table          *InScanTable( void );


void  Obj2Asm( segment *seg )
/***************************/
{
    if( !seg->dumped ) {
        Segment = seg;
        if( seg->last_export == NULL ) seg->last_export = seg->exports;
        if( seg->last_fix == NULL )    seg->last_fix    = seg->fixes;
        EmitSegment();
        InitSegAccess();
        InitAsm();
        if( !EndOfSegment() ) {
            if( Segment->data_seg ) {
                DecodeData();
            } else {
                DecodeCode();
            }
        }
        if( EndOfSegment() ) {
            ForceLabels();
            FiniAsm();
            Segment->dumped = TRUE;
        }
        EmitEndSeg();
    }
}


void FlipToSeg( segment *seg )
/****************************/
{
    unsigned    old_err;
    unsigned    old_datalen;
    segment     *old_seg;

    old_seg = Segment;
    old_err = ErrCount;
    old_datalen = DataLen;
    Obj2Asm( seg );
    Segment = old_seg;
    ErrCount = old_err;
    DataLen = old_datalen;
}


static void ForceLabels()
/***********************/
{
    char        rc_buff[MAX_RESOURCE_SIZE];
    uint_32     size;
    export_sym  *exp;
    export_sym  *curr;

    if( (Options & FORM_ASSEMBLER) && (Pass == 2) ) {
        curr = Segment->exports;
        if( DO_UNIX ) {
            if( curr != NULL && curr->address != 0 ) {
                EmitSpaced( ".zero", LABEL_LEN+OPCODE_LEN );
                EmitHex( curr->address );
                EmitNL();
            }
        }
        while( curr != NULL ) { /* emit labels in BSS seg or inside instr's */
            if( !curr->dumped ) {
                if( !Segment->data_seg ) {
                    MsgGet( MSG_ORG_MAY_NOT_ASSEM, rc_buff );
                    MsgSubStr( rc_buff, curr->name, 's' );
        /*  The following simulates a call to Error( RecNumber, FALSE ) */
                    PutText( MSG_ASTERISK_1 );
                    PutText( MSG_WARNING );
                    PutString( rc_buff );
                    PutText( MSG_ASTERISK_2 );
                    FPutEnd( stdout );
                }
                if( DO_UNIX ) {
                    exp = curr;
                    for( ;; ) {
                        exp = exp->next_exp;
                        if( exp == NULL ) {
                            size = Segment->size - curr->address;
                            break;
                        }
                        if( !exp->dumped ) {
                            size = exp->address - curr->address;
                            break;
                        }
                    }
                    EmitLabel( curr->name, size );
                } else {
                    EmitLabel( curr->name, curr->address );
                }
            }
            curr = curr->next_exp;
        }
    }
}


static char *MagicFixNames[] = {
        "FIWRQQ",
        "FIDRQQ",
        "FIERQQ",
        "FISRQQ",
        "FJSRQQ",
        "FICRQQ",
        "FJCRQQ",
        "FIARQQ",
        "FJARQQ",
        NULL
};


static  int  Is87Fix( fixup *fix )
/********************************/

{
    char        *fix_name;
    char        **table;

    if( _Class( fix->target ) != TYPE_IMPORT ) return( FALSE );
    fix_name = _Name( fix->target );
    for( table = MagicFixNames; *table != NULL; ++table ) {
        if( strcmp( fix_name, *table ) == 0 ) return( TRUE );
    }
    return( FALSE );
}


static void SwallowFix( fixup * fix )
/***********************************/

{
    switch( _Class( fix ) ) {
    case CL_LONG_POINTER: /* BASE + 32-bit offset */
        GetDataWord();
        /* no break! */
    case CL_LONG:
    case CL_POINTER:
        GetDataWord();
        break;
    }
    GetDataWord();
}


static uint_32 SizeDataInCode( uint_32 size )
/*******************************************/
{
    uint                curr_size; /* no need for uint_32 since max 4 */
    export_sym          *label;

    /* never print more than 4 or 8 bytes at a time */
    if( Options & FORM_ASSEMBLER ) {
        if( size > 8 ) {
            size = 8;
        }
    } else {
        if( size > 4 ) {
            size = 4;
        }
    }
    /* only print bytes up to the next label */
    label = Segment->last_export;
    for( ;; ) {
        if( label == NULL ) break;
        if( label->address != InsAddr ) break;
        /* not interested in possible label(s) at beginning of data */
        label = label->next_exp;
    }
    if( label != NULL && label->address - InsAddr < size ) {
        size = label->address - InsAddr;
    }
    /* At this point size > 0.  Otherwise we are in big trouble */
    /* only print bytes up to the next fixup */
    curr_size = 0;
    for(;;) {
        GetDataByte();
        ++curr_size;
        if( curr_size == size ) break;
        if( FindFixup( InsAddr+curr_size, Segment ) ) break;
    }
    return( curr_size );
}


static  void  DecodeCode()
/************************/

{
    fixup               *fix;
    char                is_data;
    char                in_pcode;
    scan_table          *table;
    instruction         curr_ins;           /* not used */

    in_pcode = 0;
    for(;;) {
        EmitSrc();
        if( EndOfSegment() ) break;
        InsAddr = GetOffset();
        DataBytes = 0;
        is_data = FALSE;
        fix = FindFixup( InsAddr, Segment );
        if( fix != NULL && Is87Fix( fix ) ) {
            fix = NULL;
        }
        if( fix != NULL ) {
            SwallowFix( fix );
            FindLabel( BAD_OFFSET, fix->imp_address, fix->target );
        } else {
            table = InScanTable();
            if( table != NULL ) {
#ifdef __PCODE__
                if( in_pcode != 1 ) {
                    if( SwitchCount == 0 && PeekDataByte() == 0x9A ) {
                        fix = FindFixup( InsAddr+1, Segment );
                        if( fix != NULL ) {
                            if( !(Options & FORM_ASSEMBLER) )  in_pcode = 1;
                            goto do_code;
                        }
                    }
                }
                if( in_pcode >= 2 ) {
                    DoPcode();
                    in_pcode = 3;
                } else {
                    if( in_pcode == 1 ) {
                        if( GetDataByte() == 0xFF ) {
                            GetDataByte();
                            GetDataByte();
                        }
                        in_pcode = 2;
                    } else {
                        SizeDataInCode( table->ends - InsAddr );
                    }
                }
#else
                SizeDataInCode( table->ends - InsAddr );
#endif
                is_data = TRUE;
            } else {
#ifdef __PCODE__
                in_pcode = 0;
do_code:
#endif
                DoCode( &curr_ins, Segment->use_32 );
            }
            fix = NULL;
        }
        if( Pass == 2 ) {
            if( !is_data && fix == NULL && CurrIns.pref & PREF_FWAIT ) {
                if( CurrIns.opcode == I_NOP ) {
                    CurrIns.opcode = I_WAIT;
                } else if( !(CurrIns.pref & FP_INS) ) {
                    EmitWait();  /* emit WAIT instruction */
                }
            }
            FormatLine( fix, GetLabel(), is_data, in_pcode );
        }
        FreeSymTranslations();
    }
}


static  void  GetBytes( uint size )
/*********************************/
{
    while( size != 0 ) {
        GetDataByte();
        --size;
    }
}


static  void  DecodeData()
/************************/

{
    fixup               *fix;
    char                *label;
    uint_32             next_fix;
    uint_32             next_label;
    uint_32             next_stop;
    uint_32             curr_size;

    Repeats = 0;
    next_fix = GetNextFix();
    next_label = GetNextLabel();
    do {
        InsAddr = GetOffset();
        DataBytes = 0;
        fix = NULL;
        label = NULL;
        if( next_label == InsAddr ) {
            label = FormSym( Segment->last_export->name );
            Segment->last_export->dumped = TRUE;
            Segment->last_export = Segment->last_export->next_exp;
            next_label = GetNextLabel();
        }
        if( next_fix == InsAddr ) {
            fix = Segment->last_fix;
            SwallowFix( fix );
            curr_size = DataBytes;
            FindLabel( BAD_OFFSET, fix->imp_address, fix->target );
            Segment->last_fix = Segment->last_fix->next_fix;
            next_fix = GetNextFix();
            EmitData( label, fix );
        } else {
            next_stop = Segment->size;
            if( next_label < next_stop ) next_stop = next_label;
            if( next_fix   < next_stop ) next_stop = next_fix;
            curr_size = next_stop - InsAddr;
            if( curr_size > MAX_DATA_LEN ) curr_size = MAX_DATA_LEN;
            GetBytes( curr_size );
            EmitData( label, fix );
        }
        FreeSymTranslations();
    } while( !EndOfSegment() );
    if( Pass == 2 && Repeats > 1 ) {
        EmitRepeats();
    }
}


static  void  EmitData( char *label, fixup *fix )
/***********************************************/
{
    if( Pass != 2 ) {
        return;
    }
    if( Repeats != 0 ) {
        if( fix == NULL && label == NULL && DataBytes == MAX_DATA_LEN &&
               memcmp( PrevString, DataString, MAX_DATA_LEN ) == 0
#ifdef O2A
                && !DO_UNIX
#endif
               ) {
            ++Repeats;
            return;
        } else if( Repeats > 1 ) {
            EmitRepeats();
        }
    }
    FormatLine( fix, label, TRUE, 0 );
    if( DataBytes == MAX_DATA_LEN ) {     /* may repeat */
        memcpy( PrevString, DataString, MAX_DATA_LEN );
        Repeats = 1;
    } else {
        Repeats = 0;
    }
}


static  void  EmitRepeats()
/*************************/

{
    char                temp_bytes[ MAX_DATA_LEN ];
    int                 curr_bytes;

    if( Repeats == 2 ) {        /* line repeats only 1 time */
        InsAddr -= MAX_DATA_LEN;
        memcpy( temp_bytes, DataString, MAX_DATA_LEN );
        memcpy( DataString, PrevString, MAX_DATA_LEN );
        Repeats = 0;
        curr_bytes = DataBytes;
        DataBytes = MAX_DATA_LEN;
        FormatLine( NULL, NULL, TRUE, 0 );
        DataBytes = curr_bytes;
        InsAddr += MAX_DATA_LEN;
        memcpy( DataString, temp_bytes, MAX_DATA_LEN );
    } else {
        EmitDups();
    }
}


static  int  EmitWait()
/*********************/

{
    int                 old_size;
    int                 old_num_oper;
    int                 old_opcode;
    int                 i;

    if( CurrIns.opcode == I_NULL ) {
        CurrIns.opcode = I_WAIT;
    } else {
        old_size = CurrIns.ins_size;
        old_num_oper = CurrIns.num_oper;
        old_opcode = CurrIns.opcode;
        CurrIns.ins_size = 1;
        CurrIns.num_oper = 0;
        CurrIns.opcode = I_WAIT;
        DataBytes = 1;
        FormatLine( NULL, GetLabel(), FALSE, 0 );
        ++InsAddr;
        CurrIns.pref &= ~PREF_FWAIT;
        CurrIns.ins_size = old_size - 1;
        CurrIns.num_oper = old_num_oper;
        CurrIns.opcode = old_opcode;
        i = old_num_oper;
        while( --i >= 0 ) {
            --(CurrIns.op[ i ].offset);
        }
        DataBytes = CurrIns.ins_size;
        for( i = 0; i < DataBytes; ++i ) {
            DataString[ i ] = DataString[ i + 1 ];
        }
    }
    return( CurrIns.ins_size );
}


static  scan_table  *InScanTable()
/********************************/

{
    scan_table          *table;

    table = Segment->scan_tabs;
    while( table != NULL ) {
        if( InsAddr >= table->starts && InsAddr < table->ends ) {   /* open at end */
            break;
        }
        table = table->next;
    }
    return( table );
}


static  uint_32 GetNextFix()
/**************************/

{
    if( Segment->last_fix != NULL ) {
        return( Segment->last_fix->address );
    } else {
        return( ~(uint_32)0 );
    }
}


static  void  SkipHiddenLabels()
/******************************/

{
    export_sym      *exp;

    while( Segment->last_export != NULL ) {
        exp = Segment->last_export;
        if( !exp->hidden ) return;
        exp->dumped = TRUE;     //  pretend that it's dumped
        Segment->last_export = exp->next_exp;
    }
}


static  uint_32 GetNextLabel()
/****************************/

{
    if( UseORL ) SkipHiddenLabels();
    if( Segment->last_export != NULL ) {
        return( Segment->last_export->address );
    } else {
        return( ~(uint_32)0 );
    }
}


static  bool    LabelIn( bool exact )
/***********************************/

{
    export_sym      *exp;

    exp = Segment->last_export;
    while( exp != NULL && exp->dumped ) {
        exp = exp->next_exp;
    }
    if( exp == NULL ) return( FALSE );
    if( exp->address >= InsAddr+DataBytes ) return( FALSE );
    if( exact ) return( exp->address == InsAddr );
    return( exp->address > InsAddr );
}


extern  bool    LabelInInstr()
/****************************/
{
    return( LabelIn( FALSE ) );
}


extern  bool    LabelOnInstr()
/****************************/
{
    return( LabelIn( TRUE ) );
}


extern char  *GetLabel( void )
/****************************/

{
    export_sym          *sym;

    if( UseORL ) SkipHiddenLabels();
    while( Segment->last_export != NULL ) {
        sym = Segment->last_export;
        if( sym->address > InsAddr ) {
            return( NULL );
        }
        Segment->last_export = sym->next_exp;
        if( UseORL ) SkipHiddenLabels();
        if( sym->address == InsAddr ) {
            sym->dumped = TRUE;     /* label has been used */
            return( FormSym( sym->name ) );
        }
    }
    return( NULL );
}
