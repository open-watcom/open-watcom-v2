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
* Description:  Dump internal optimizer data.
*
****************************************************************************/


#include "optwif.h"
#include "dump.h"
#include "feprotos.h"

extern  void            DumpNL();
extern  void            DumpPtr(pointer);
extern  void            Dump8h(unsigned_32);
extern  void            DumpInt(int);
extern  void            DumpByte(byte);
extern  char            *AskRTName( rt_class );

static  void            DoData( oc_entry *instr );
static  void            DoLabel( oc_handle *instr );
static  void            DoRef( oc_handle *instr );

static char * CNames[] = {
    #define pick_class(x) #x ,
    #include "occlasss.h"
    #undef pick_class
    ""
};

#if _TARGET & _TARG_INTEL
static char * Conds[] = {
    "jo   ",
    "jno  ",
    "jb   ",
    "jae  ",
    "je   ",
    "jne  ",
    "jbe  ",
    "ja   ",
    "js   ",
    "jns  ",
    "jp   ",
    "jnp  ",
    "jl   ",
    "jge  ",
    "jle  ",
    "jg   ",
    "jmp  ",
    ""
};

static  char    *CondName( oc_jcond *oc ) {
/*****************************************/

    return( Conds[ oc->cond ] );
}
#else
static char * Conds[] = {
    "je   ",
    "jne  ",
    "je   ",
    "jne  ",
    "jg   ",
    "jle  ",
    "jl   ",
    "jge  ",
    ""
};

static  char    *CondName( oc_jcond *oc ) {
/*****************************************/

    return( Conds[ oc->cond - FIRST_CONDITION ] );
}
#endif

static  bool    LblName( code_lbl *lbl ) {
/*****************************************/


    if( ValidLbl( lbl ) == FALSE ) return( FALSE );
    DumpLiteral( "L" );
    DumpPtr( lbl );
    if( lbl->lbl.sym == NULL ) return( TRUE );
    DumpLiteral( "(" );
    if( AskIfRTLabel( lbl ) ) {
        DumpXString( AskRTName( (rt_class)(pointer_int)lbl->lbl.sym ) );
    } else if( AskIfCommonLabel( lbl ) ) {
        DumpLiteral( "Common import => [" );
        DumpInt( (int)(pointer_int)lbl->lbl.sym );
        DumpLiteral( "] " );
    } else {
        DumpXString( FEName( lbl->lbl.sym ) );
    }
    DumpLiteral( ")" );
    return( TRUE );
}


static  void    CheckAttr( oc_class cl ) {
/****************************************/

    if( cl & ATTR_FAR ) {
        DumpLiteral( "far " );
    }
    if( cl & ATTR_SHORT ) {
        DumpLiteral( "short " );
    }
    if( cl & ATTR_POP ) {
        DumpLiteral( "popping " );
    }
    if( cl & ATTR_FLOAT ) {
        DumpLiteral( "floating " );
    }
}


static  void    DoInfo( any_oc *oc ) {
/**************************************/

    switch( oc->oc_header.class & INFO_MASK ) {
    case INFO_LINE:
        DumpLiteral( "LINE " );
        DumpInt( oc->oc_linenum.line );
        if( oc->oc_linenum.label_line ) {
            DumpLiteral( " (Label)" );
        }
        break;
    case INFO_LDONE:
        DumpLiteral( "LDONE " );
        LblName( oc->oc_handle.handle );
        break;
    case INFO_DEAD_JMP:
        DumpLiteral( "DEAD  " );
        DumpLiteral( "jmp  " );
        DoRef( &(oc->oc_handle) );
        break;
    case INFO_DBG_RTN_BEG:
        DumpLiteral( "RTN BEGIN " );
        DumpPtr( oc->oc_debug.ptr );
        break;
    case INFO_DBG_BLK_BEG:
        DumpLiteral( "BLOCK BEGIN " );
        DumpPtr( oc->oc_debug.ptr );
        break;
    case INFO_DBG_PRO_END:
        DumpLiteral( "PROLOG END " );
        DumpPtr( oc->oc_debug.ptr );
        break;
    case INFO_DBG_EPI_BEG:
        DumpLiteral( "EPILOG BEGIN " );
        DumpPtr( oc->oc_debug.ptr );
        break;
    case INFO_DBG_BLK_END:
        DumpLiteral( "BLOCK END " );
        DumpPtr( oc->oc_debug.ptr );
        break;
    case INFO_DBG_RTN_END:
        DumpLiteral( "RTN END " );
        DumpPtr( oc->oc_debug.ptr );
        break;
    case INFO_SELECT:
        DumpLiteral( "SELECT TABLE " );
        if( oc->oc_select.starts ) {
            DumpLiteral( "STARTS" );
        } else {
            DumpLiteral( "ENDS" );
        }
        break;
    default:
        DumpLiteral( "*** unknown info ***" );
        break;
    }
}


extern  void    DumpOc( ins_entry *ins ) {
/****************************************/

    DumpPtr( ins );
    DumpLiteral( " " );
    DumpString(  CNames[ _Class( ins ) ] );
    DumpLiteral( " " );
    if( _Class( ins ) != OC_INFO ) {
        CheckAttr( ins->oc.oc_header.class );
    }
    switch( _Class( ins ) ) {
    case OC_INFO:
        DoInfo ( &ins->oc );
        break;
    case OC_CODE:
        DoData ( &ins->oc.oc_entry );
        break;
    case OC_DATA:
        DoData ( &ins->oc.oc_entry );
        break;
    case OC_IDATA:
        DoData( &ins->oc.oc_entry );
        break;
    case OC_BDATA:
        DoData ( &ins->oc.oc_entry );
        break;
    case OC_LABEL:
        DoLabel( &ins->oc.oc_handle );
        break;
    case OC_LREF:
        DumpLiteral( "dw   " );
        DoRef  ( &ins->oc.oc_handle );
        break;
    case OC_CALL:
        DumpLiteral( "call " );
        DoRef  ( &ins->oc.oc_handle );
        break;
    case OC_CALLI:
        DoData ( &ins->oc.oc_entry );
        break;
    case OC_JCOND:
        DumpString( CondName( &ins->oc.oc_jcond ) );
        DoRef( &ins->oc.oc_handle );
        break;
    case OC_JCONDI:
        DoData( &ins->oc.oc_entry );
        break;
    case OC_JMP:
        DumpLiteral( "jmp  " );
        DoRef  ( &ins->oc.oc_handle );
        break;
    case OC_JMPI:
        DoData ( &ins->oc.oc_entry );
        break;
    case OC_RET:
        DumpInt( ins->oc.oc_ret.pops );
        DumpNL();
        break;
#if _TARGET & _TARG_RISC
    case OC_RCODE:
        DumpPtr( (pointer)(pointer_int)ins->oc.oc_rins.opcode );
        if( _HasReloc( &ins->oc.oc_rins ) ) {
            DumpLiteral( " [ " );
            LblName( ins->oc.oc_rins.sym );
            DumpLiteral( "," );
            DumpInt( ins->oc.oc_rins.reloc );
            DumpLiteral( " ] " );
        }
        break;
#endif
    default:
        DumpLiteral( "*** unknown class ***" );
        break;
    }
    DumpNL();
}


static  void    DoData( oc_entry *instr ) {
/*****************************************/

    uint        len;

    for( len = 0; len < instr->hdr.reclen - offsetof( oc_entry, data ); ++len ) {
        DumpByte( instr->data[ len ] );
        DumpLiteral( " " );
    }
}


static  void    DoLabel( oc_handle *instr ) {
/*******************************************/

    code_lbl    *lbl;

    lbl = instr->handle;
    DumpLiteral( "align=<" );
    DumpByte( instr->hdr.objlen + 1 );
    DumpLiteral( "> " );
    for(;;) {
        if( LblName( lbl ) == FALSE ) break;
        lbl = lbl->alias;
        if( lbl == NULL ) break;
        DumpLiteral( " " );
    }
#if _TARGET & _TARG_RISC
    if( instr->line != 0 ) {
        DumpLiteral( "line=<" );
        DumpInt( instr->line );
        DumpLiteral( "> " );
    }
#endif
}


static  void    DoRef( oc_handle *instr ) {
/*****************************************/

    LblName( instr->handle );
}


extern  void    DumpLbl( code_lbl *lbl ) {
/****************************************/

    ins_entry   *ref;

    if( _ValidLbl( lbl ) == FALSE ) return;
    if( lbl->lbl.sym != NULL ) {
        DumpLiteral( "(" );
        DumpXString( FEName( lbl->lbl.sym ) );
        DumpLiteral( ") " );
    }
    DumpLiteral( "addr==" );
    Dump8h( lbl->lbl.address );
    DumpLiteral( ", patch==" );
    DumpPtr( lbl->lbl.patch );
    DumpLiteral( " " );
    if( _TstStatus( lbl, CODELABEL ) ) {
        DumpLiteral( "CODE " );
    }
    if( _TstStatus( lbl, KEEPLABEL ) ) {
        DumpLiteral( "KEEP " );
    }
    if( _TstStatus( lbl, DYINGLABEL ) ) {
        DumpLiteral( "DYING " );
    }
    if( _TstStatus( lbl, SHORTREACH ) ) {
        DumpLiteral( "S-REACH " );
    }
    if( _TstStatus( lbl, CONDEMNED ) ) {
        DumpLiteral( "CONDEMNED " );
    }
    if( _TstStatus( lbl, RUNTIME ) ) {
        DumpLiteral( "RT " );
    }
    if( _TstStatus( lbl, REDIRECTION ) ) {
        DumpLiteral( "REDIR " );
    }
    if( _TstStatus( lbl, UNIQUE ) ) {
        DumpLiteral( "UNIQUE " );
    }
    DumpNL();
    if( lbl->ins != NULL ) {
        DumpLiteral( "ins==" );
        DumpPtr( lbl->ins );
        DumpLiteral( " " );
    }
    if( lbl->redirect != NULL ) {
        DumpLiteral( "redir==" );
        DumpPtr( lbl->redirect );
        DumpLiteral( " " );
    }
    ref = lbl->refs;
    if( ref != NULL ) {
        DumpLiteral( "ref==" );
        for(;;) {
            DumpPtr( ref );
            DumpLiteral( "  " );
            ref = _LblRef( ref );
            if( ref == NULL ) break;
        }
    }
    DumpNL();
}


extern  void    DownOpt( ins_entry *instr, uint num ) {
/*****************************************************/

    DumpLiteral( "--------<Queue>-------" );
    DumpNL();
    for(;;) {
        if( instr == NULL ) break;
        if( num == 0 ) break;
        DumpOc( instr );
        --num;
        instr = instr->ins.next;
    }
}


extern  void    UpOpt( ins_entry *ins, uint last ) {
/**************************************************/

    uint        size;

    size = last;
    for(;;) {
        if( size == 0 ) break;
        ins = ins->ins.prev;
        if( ins == NULL ) break;
        --size;
    }
    DownOpt( ins, last + 1 );
}


extern  void    DumpOpt( void ) {
/*******************************/

    DownOpt( FirstIns, -1 );
}
