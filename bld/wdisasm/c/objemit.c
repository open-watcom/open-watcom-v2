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
#include <string.h>
#include <ctype.h>
#include "disasm.h"
#include "wdismsg.h"


static  char            *RetFMacro[] = {
        "            MACRO   POP_COUNT",
    "                LOCAL   DUMMY",
    "                DUMMY   PROC FAR",
    "                RET     POP_COUNT",
    "                DUMMY   ENDP",
    "                ENDM",
    NULL
};


static char *DOSAlignTxt[] = {
    "SEGMENT", /* used for COMDAT's */
    "BYTE",
    "WORD",
    "PARA",
    "PAGE",
    "DWORD",
    "PAGE4K",
};

static char *UnixAlignTxt[] = {
    "section", /* used for COMDAT's */
    "1",
    "2",
    "16",
    "256",
    "4",
    "4096",
};


static char *MatchTxt[] = {
    "NONE",
    "ANY",
    "SIZE",
    "EXACT",
};

static char *AllocTxt[] = {
    NULL,
    "FAR CODE",
    "FAR DATA",
    "CODE32",
    "DATA32",
};


/*
 * Static function prototypes
 */
static  void            EmitSegAlign( void );
static  void            EmitSegCombine( void );
static  void            EmitOrg( uint_32 origin );
static  void            EmitRetFMacro( void );


void  EmitModule()
/****************/

{
    char                *name;
    import_sym          *imp;
    char                sp_buf[ _MAX_PATH2 ];
    char                *root_name;

    if( ( Options & FORM_ASSEMBLER ) == 0 ) {
        EmitNL();
        DoEmit( MSG_MODULE );
        EmitLine( Mod->name );
        if( Mod->main ) {
            EmitLine( "MAIN MODULE " );
        }
        if( Mod->start != NULL ) {
            DoEmit( MSG_START_ADDR );
            name = GetFixName( Mod->start );
            if( name == NULL ) {
                name = NewName( Mod->start );
            }
            Emit( name );
            EmitNL();
        }
    } else {
        _splitpath2( Mod->name, sp_buf, NULL, NULL, &root_name, NULL );
        if( DO_UNIX ) {
            EmitBlanks( LABEL_LEN );
            EmitSpaced( ".file", OPCODE_LEN );
            Emit( "\"" );
            Emit( root_name );
            Emit( "\"" );
            EmitNL();
        } else {
            if( RetFarUsed ) {
                EmitRetFMacro();
            }
            if( Is32BitObj ) EmitLine( ".386p" );
            EmitBlanks( LABEL_LEN );
            EmitSpaced( "NAME", OPCODE_LEN );
            EmitLine( root_name );
        }
        if( ( Options & FORM_DO_WTK )  &&  HaveWtk() ) {
            Emit( "INCLUDE" );
            EmitBlanks( LABEL_LEN - 7 );
            EmitLine( "67MACROS.EQU" );
        }
    }
    for( imp = Mod->imports; imp != NULL; imp = imp->next_imp ) {
        switch( imp->class ) {
        case TYPE_IMPORT:
            if( !DO_UNIX
                && ( Options & FORM_ASSEMBLER )
                && imp->public
                && !imp->exported
                && strcmp( imp->name, WTLBASEStr ) != 0 ) {
                EmitBlanks( LABEL_LEN );
                EmitSpaced( "EXTRN", OPCODE_LEN );
                EmitSym( imp->name, 0 );
                Emit( ":BYTE" );        // so that output would assemble
                EmitNL();
            }
            break;
        case TYPE_COMDEF:
            if( DO_UNIX ) {
                if( imp->public ) {
                    EmitBlanks( LABEL_LEN );
                    EmitSpaced( ".globl", OPCODE_LEN );
                    EmitLine( imp->name );
                }
                EmitBlanks( LABEL_LEN );
                EmitSpaced( ".lcomm", OPCODE_LEN );
                EmitSym( imp->name, 0 );
                Emit( ", " );
            } else {
                if( imp->public ) {
                    EmitBlanks( LABEL_LEN );
                    EmitSpaced( "COMM", OPCODE_LEN );
                } else {
                    if( Options & FORM_ASSEMBLER ) {
                        DoEmitError( MSG_LCL_COMM_WONT_ASM );
                    }
                    EmitBlanks( LABEL_LEN );
                    EmitSpaced( "LCOMM", OPCODE_LEN );
                }
                if( imp->far_common ) {
                    Emit( "FAR  " );
                } else {
                    Emit( "NEAR " );
                }
                EmitSym( imp->name, 0 );
                Emit( ":BYTE:" );
            }
            EmitHex( imp->u.size );
            EmitNL();
            break;
        }
        FreeSymTranslations();
    }
}


void  EmitEndMod()
/****************/

{
    EmitSrc();
    if( Options & FORM_ASSEMBLER ) {
        if( !DO_UNIX ) {
            EmitBlanks( LABEL_LEN );
            EmitLine( "END" );
        }
    } else {
        DumpSymList();
    }
}


void  EmitGroup( group *grp )
/***************************/

{
    handle              *entry;
    segment             *seg;
    bool                found;

    if( grp->name != NULL ) {
        entry = grp->list;
        found = false;
        while( entry != NULL ) {
            seg = (segment *)entry->data;
            if( !found ) {
                found = true;
                if( Options & FORM_ASSEMBLER ) {
                    EmitSpaced( grp->name, LABEL_LEN );
                    EmitSpaced( "GROUP", OPCODE_LEN );
                } else {
                    DoEmit( MSG_GROUP );
                    Emit( "'" );
                    Emit( grp->name );
                    Emit( "' " );
                }
            } else {
                Emit( "," );
            }
            Emit( seg->name );
            entry = entry->next_hndl;
        }
        if( found ) EmitNL();
    }
}


void  EmitSegment()
/*****************/

{
    export_sym          *exp;
    char                numbuff[ 16 ];
    group               *grp;

    if( Segment->name != NULL ) {
        if( Options & FORM_ASSEMBLER ) {
            if( DO_UNIX ) {
                if( !Segment->data_seg ) {
                    EmitLine( ".text" );
                } else if( stricmp( Segment->name, "_BSS" ) == 0 ) {
                    EmitLine( ".bss" );
                } else {
                    EmitLine( ".data" );
                }
                if( Segment->class == TYPE_SEGMENT ) {
                    EmitBlanks( LABEL_LEN );
                    Emit( ".align " );
                    EmitLine( UnixAlignTxt[ _SegAlign( Segment ) ] );
                } else {
                    //NYI: don't know what to do with comdat's
                    EmitNL();
                }
            } else {
                if( Segment->class == TYPE_COMDAT ) {
                    if( Options & FORM_ASSEMBLER ) {
                        DoEmitError( MSG_COMDAT_WONT_ASM );
                    }
                    EmitSym( Segment->name, LABEL_LEN );
                    EmitSpaced( "COMDAT", OPCODE_LEN );
                } else {
                    EmitSpaced( Segment->name, LABEL_LEN );
                    EmitSpaced( "SEGMENT", OPCODE_LEN );
                }
                EmitSegAlign();
                EmitSegCombine();
                EmitSegUse();
                EmitSegAccess();
                if( Segment->class == TYPE_SEGMENT ) {
                    Emit( "'" );
                    Emit( Segment->u.seg.class_name );
                    Emit( "'" );
                }
                EmitNL();
                if( !Segment->data_seg ) {
                    EmitBlanks( LABEL_LEN );
                    Emit( "ASSUME  CS:" );
                    grp = Segment->grouped;
                    if( grp != NULL ) {
                        Emit( grp->name );
                    } else {
                        EmitSegName( Segment, 0 );
                    }
                    EmitLine( ",DS:DGROUP,SS:DGROUP" );
                }
            }
            exp = Segment->exports;
            while( exp != NULL ) {
                if( exp->public ) {
                    EmitBlanks( LABEL_LEN );
                    if( DO_UNIX ) {
                        EmitSpaced( ".globl", OPCODE_LEN );
                    } else {
                        EmitSpaced( "PUBLIC", OPCODE_LEN );
                    }
                    EmitSym( exp->name, 0 );
                    EmitNL();
                }
                exp = exp->next_exp;
            }
        } else {
            EmitNL();
            if( Segment->class == TYPE_COMDAT ) {
                DoEmit( MSG_COMDAT );
            } else {
                DoEmit( MSG_SEGMENT );
            }
            EmitSegName( Segment, 0 );
            Emit( " " );
            EmitSegAlign();
            EmitSegUse();
            EmitSegAccess();
            if( _Size64K( Segment ) ) {
                strcpy( numbuff, "10000" );
            } else {
                EmitAddr( Segment->size, WORD_SIZE, numbuff );
            }
            SubStrEmit( MSG_BYTES, numbuff, 's' );
            if( Segment->offset != BAD_OFFSET ) {
                DoEmit( MSG_LOCATION );
                EmitAddr( Segment->address, WORD_SIZE, NULL );
                IToHS( numbuff, Segment->offset, 1 );
                numbuff[1] = NULLCHAR;
                Emit( numbuff );
            }
            EmitNL();
        }
    }
}


static  void  EmitSegAlign( void )
/********************************/

{
    unsigned    align;

    align = (Segment->class == TYPE_COMDAT)
                        ? _ComdatAlign( Segment ) : _SegAlign( Segment );
    Emit( DOSAlignTxt[ align ] );
    Emit( " " );
}


static  void  EmitSegUse( void )
/******************************/

{
    if( Segment->class == TYPE_COMDAT ) {
        Emit( MatchTxt[ _ComdatMatch( Segment ) ] );
        Emit( " " );
        if( Segment->u.com.grp != NULL ) {
            Emit( "'" );
            Emit( Segment->u.com.grp->name );
            if( Segment->u.com.seg != NULL ) {
                Emit( ":" );
                Emit( Segment->u.com.seg->name );
            }
            Emit( "'" );
        } else if( Segment->u.com.seg != NULL ) {
            Emit( "'" );
            Emit( Segment->u.com.seg->name );
            Emit( "'" );
        } else {
            Emit( AllocTxt[ _ComdatAlloc( Segment ) ] );
        }
    } else if( Segment->use_32 ) {
        Emit( "USE32" );
    } else if( Is32BitObj ) {
        Emit( "USE16" );
    }
    Emit( " " );
}


static void EmitSegAccess( void )
/*******************************/
{
    if( IsPharLap && Segment->access_valid ) {
        switch( Segment->access_attr ) {
        case EASY_READ_ONLY:    Emit( "RO " );  break;
        case EASY_EXEC_ONLY:    Emit( "EO " );  break;
        case EASY_EXEC_READ:    Emit( "ER " );  break;
        case EASY_READ_WRITE:   Emit( "RW " );  break;
        }
    }
}


static  void  EmitSegCombine( void )
/**********************************/

{
    char                combine;

    if( Segment->class == TYPE_COMDAT ) return;
    combine = _SegCombine( Segment );
    switch( combine ) {
    case 0:
        if( ( Options & FORM_ASSEMBLER ) == 0 ) {
            Emit( "PRIVATE " );
        }
        break;
    case 2:
        Emit( "PUBLIC " );
        break;
    case 5:
        Emit( "STACK " );
        break;
    case 6:
        Emit( "COMMON " );
        break;
    }
}


void  EmitEndSeg()
/****************/

{
    if( !DO_UNIX ) {
        if( Segment->name != NULL ) {
            if( Options & FORM_ASSEMBLER ) {
                if( Segment->start != 0 ) {
                    EmitOrg( Segment->start );
                }
                EmitSegName( Segment, LABEL_LEN );
                EmitLine( "ENDS" );
                EmitNL();
            } else {
                DumpImpList();
                EmitDashes( LINE_LEN );
            }
        }
    } else {
        EmitNL();
    }
}


void  EmitLabel( char *lbl, uint_32 origin )
/******************************************/

{
    if( DO_UNIX ) {
        EmitSpaced( ".bss", LABEL_LEN );
        EmitSym( lbl, 0 );
        EmitSpaced( ", ", OPCODE_LEN - strlen( lbl ) );
        EmitOrg( origin );
    } else {
        EmitOrg( origin );
        EmitSym( lbl, LABEL_LEN );
        if( Segment->data_seg ) {
            EmitLine( "LABEL   BYTE" );
        } else {
            EmitLine( "LABEL   NEAR" );
        }
    }
}

void EmitHex( uint_32 val )
/*************************/
{
    char                numbuff[ 16 ];

    DoIToHS( numbuff, val, WORD_SIZE );
    Emit( numbuff );
}


static void EmitOrg( uint_32 origin )
/***********************************/

{
    if( !DO_UNIX ) {
        EmitBlanks( LABEL_LEN );
        EmitSpaced( "ORG", OPCODE_LEN );
    }
    EmitHex( origin );
    EmitNL();
}


void  DoEmit( int msg_num )
/***************/
{
    char        msg_buff[MAX_RESOURCE_SIZE];

    MsgGet( msg_num, msg_buff );
    Emit( msg_buff );
}


void  Emit( str )
/***************/

    char                *str;
{
    if( ( Pass == 2 ) && ( str != NULL ) ) {
        PutString( str );
    }
}


void  EmitNL()
/************/

{
    if( Pass == 2 ) {
        FPutEnd( Output );
    }
}


void  EmitLine( ptr )
/*******************/

    char                *ptr;
{
    Emit( ptr );
    EmitNL();
}


void  EmitAddr( uint_32 addr, int len, char *straddr )
/*************************************/
{
    char                buff[ 16 ];

    Emit( " " );
    IToHS( buff, addr, len );
    buff[ len ] = NULLCHAR;
    if( straddr == NULL ){
        Emit( buff );
    } else {
        strcpy( straddr, buff );
    }
}


void  EmitDashes( int length )
/****************************/
{
    int                 i;

    i = 0;
    while( ++i <= length ) {
        Emit( "-" );
    }
    EmitNL();
}


void  EmitBlanks( int blanks )
/****************************/
{
    do {
        Emit( " " );
    } while( --blanks > 0 );
}


void  DoEmitSpaced( int msgnum, int spacing )
/******************************************/
{
    char        rc_buff[MAX_RESOURCE_SIZE];

    MsgGet( msgnum, rc_buff );
    EmitSpaced( rc_buff, spacing );
}


int  EmitSpaced( char *str, int spacing )
/***************************************/
{
    int                 length;
    int                 over;

    if( str == NULL ) {
        length = 0;
    } else {
        length = strlen( str );
        Emit( str );
    }
    over = (length >= spacing);
    do {
        Emit( " " );
    } while( ++length < spacing );
    return( over );
}

int EmitSym( char *sym, unsigned spacing )
{
    return( EmitSpaced( FormSym( sym ), spacing ) );
}

void EmitSegName( segment *seg, unsigned spacing )
{
    if( seg->class == TYPE_COMDAT ) {
        EmitSym( seg->name, spacing );
    } else {
        EmitSpaced( seg->name, spacing );
    }
}


void  DoEmitError( int msg_num )
/**************************/
{
    char        msg_buff[MAX_RESOURCE_SIZE];

    MsgGet( msg_num, msg_buff );
    EmitError( msg_buff );
}


void  EmitError( char *msg )
/**************************/
{
    if( Options & FORM_ASSEMBLER ) {
        Emit( "; " );
    }
    DoEmit( MSG_ERROR );
    EmitLine( msg );
    ++ErrCount;
}


void  EmitLoc()
/*************/

{
    if( ( Options & FORM_ASSEMBLER ) == 0 ) {
        EmitAddr( InsAddr, 4, NULL );
        Emit( "  " );
    }
}


void  EmitBytes()
/***************/

{
    int                 len, curr_len;
    char                DataBuff[ 3 ];

    if( ( Options & FORM_ASSEMBLER ) == 0 ) {
        len = 0;
        curr_len = 0;
        if( DataBytes != 0 ) {
            for( ;; ) {
                IToHS( DataBuff, DataString[ len ], 2 );
                DataBuff[ 2 ] = NULLCHAR;
                Emit( DataBuff );
                Emit( " " );
                ++len;
                ++curr_len;
                if( len >= DataBytes ) break;
                if( curr_len == DataLen ) {
                    EmitNL();
                    Emit( "       " );
                    curr_len = 0;
                }
            }
        }
        while( ++curr_len <= DataLen ) {
            Emit( "   " );
        }
    }
}


void  EmitSrc()
/*************/

{
    if( Source != NULL && Pass == 2 ) {
        FindSrcLine( GetOffset() );
    }
}


void  InitAsm()
/*************/

{
    ErrCount = 0;
    InitBuffs();
    if( Segment->data_seg ) {
        DataLen = 8;
    } else {
        DataLen = 6;
    }
}


static  void  InitBuffs()
/***********************/

{
    NameBuff[ 0 ] = NULLCHAR;
}


void  FiniAsm()
/*************/

{
    char                numbuff[ 16 ];

    if( ( Options & FORM_ASSEMBLER ) == 0 && Pass != 1 ) {
        EmitNL();
        if( ErrCount == 0 ) {
            DoEmit( MSG_NO_DISASM_ERR );
        } else {
            itoa( ErrCount, numbuff, 10 );
            SubStrEmit( MSG_DISASM_ERR, numbuff, 's' );
        }
        EmitNL();
        EmitNL();
    }
}


void  EmitDups()
/**************/

{
    int                 i;
    char                numbuff[ 16 ];

    itoa( Repeats - 1, numbuff, 10 );
    if( Options & FORM_ASSEMBLER ) {
        EmitBlanks( LABEL_LEN );
        Emit( DBstring() );
        Emit( numbuff );
        Emit( " DUP(" );
        i = 0;
        do {
            if( i > 0 ) {
                Emit( "," );
            }
            DoIToHS( numbuff, PrevString[ i ], 2 );
            Emit( numbuff );
        } while( ++i != MAX_DATA_LEN );
        EmitLine( ")" );
    } else {
        SubStrEmit( MSG_ABOVE_LN_REPEAT, numbuff, 's' );
        EmitNL();
    }
    Repeats = 0;
}


static void EmitRetFMacro( void )
/*******************************/

{
    char                **str;
    char                tmp_buff[ 8 ];

    strcpy( tmp_buff, "retf" );
    if( Options & FORM_NAME_UPPER ) {
        ZapUpper( tmp_buff );
    }
    Emit( tmp_buff );
    str = RetFMacro;
    while( *str != NULL ) {
        EmitLine( *str );
        ++str;
    }
}


static void SubStrEmit( int msgnum, char *para, char specifier )
/**************************************************************/
{
    char        msg_buff[MAX_RESOURCE_SIZE];

    MsgGet( msgnum, msg_buff );
    MsgSubStr( msg_buff, para, specifier );
    Emit( msg_buff );
}
