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


#include <ctype.h>
#include <time.h>
#include <string.h>

#include "dmpobj.h"

typedef unsigned short  DOSDATE_T;

enum {
    TIME_SEC_B  = 0,
    TIME_SEC_F  = 0x001f,
    TIME_MIN_B  = 5,
    TIME_MIN_F  = 0x07e0,
    TIME_HOUR_B = 11,
    TIME_HOUR_F = 0xf800
};

enum {
    DATE_DAY_B  = 0,
    DATE_DAY_F  = 0x001f,
    DATE_MON_B  = 5,
    DATE_MON_F  = 0x01e0,
    DATE_YEAR_B = 9,
    DATE_YEAR_F = 0xfe00
};

static time_t d2t( DOSDATE_T date, DOSDATE_T time )
/*************************************************/
{
    struct tm tmbuf;

    tmbuf.tm_year = ( ( date & DATE_YEAR_F ) >> DATE_YEAR_B ) + 80;
    tmbuf.tm_mon  = ( ( date & DATE_MON_F ) >> DATE_MON_B ) - 1;
    tmbuf.tm_mday = ( date & DATE_DAY_F ) >> DATE_DAY_B;

    tmbuf.tm_hour = ( time & TIME_HOUR_F ) >> TIME_HOUR_B;
    tmbuf.tm_min  = ( time & TIME_MIN_F ) >> TIME_MIN_B;
    tmbuf.tm_sec  = ( ( time & TIME_SEC_F ) >> TIME_SEC_B ) * 2;

    tmbuf.tm_isdst= -1;

    return( mktime( &tmbuf ) );
}

void ProcEndRec( void )
/*********************/
{
    byte    typ;

    typ = GetByte();
    switch( typ & 0x3 ) {
    case 0: Output( INDENT "end of overlay" CRLF );     break;
    case 1: Output( INDENT "end of block" CRLF );       break;
    default:
        Output( BAILOUT "Unknown ENDREC type (%b)" CRLF, typ );
        longjmp( BailOutJmp, 1 );
    }
}

void ProcTHeadr( void )
/*********************/
{
    Segindex    = 0;
    Nameindex   = 0;
    Importindex = 0;
    Libindex    = 0;
    GetName();
    Output( INDENT "%N" CRLF );
}

void ProcLHeadr( void )
/*********************/
{
    GetName();
    Output( INDENT "%N" CRLF );
}

void ProcRHeadr( void )
/*********************/
{
    GetName();
    Output( INDENT "%N" CRLF );
    OutputData( (unsigned_32)RecOffset(), 0L );
}

static void doProcModel( void ) {

    byte        tmp;

    if( EndRec() ) return;
    Output( INDENT "Processor     : 80" );
    tmp = GetByte();
    if( tmp != '0' ) {
        Output( "%c", tmp );
    }
    Output( "86" CRLF );
    if( EndRec() ) return;
    Output( INDENT "Memory Model  : " );
    tmp = GetByte();
    switch( tolower( tmp ) ) {
    case 's':   Output( "Small" CRLF );     break;
    case 'm':   Output( "Medium" CRLF );    break;
    case 'c':   Output( "Compact" CRLF );   break;
    case 'l':   Output( "Large" CRLF );     break;
    case 'h':   Output( "Huge" CRLF );      break;
    case 'f':   Output( "Flat" CRLF );      break;
    default:
        Output( "Unknown(%c)" CRLF, tmp );
        break;
    }
    if( EndRec() ) return;
    tmp = GetByte();
    Output( INDENT "Optimized     : %s" CRLF, ( tmp == 'O' ) ? "Yes" : "No" );
    if( EndRec() ) return;
    Output( INDENT "Floating point: " );
    tmp = GetByte();
    switch( tmp ) {
    case 'e':   Output( "Emulated inline" CRLF );   break;
    case 'c':   Output( "Emulator calls" CRLF );    break;
    case 'p':   Output( "80x87 inline code" CRLF ); break;
    default:
        Output( "Unknown(%c)" CRLF, tmp );
        break;
    }
}

static void doWeakLazyExtern( void )
{
    byte default_resolution;
    byte extern_idx;

    for(;;) {
        if( EndRec() ) break;
        extern_idx = GetIndex();
        default_resolution = GetIndex();
        Output( INDENT INDENT "EI(%u) default: EI(%u)\n", extern_idx, default_resolution );
    }
}

static void doDependency( void ) {

    byte        len;
    DOSDATE_T   dos_date;
    DOSDATE_T   dos_time;
    time_t      t;
    char        *p;
    auto char buff[80];

    if( EndRec() ) {
        Output( INDENT "Last Dependency Record" CRLF );
        return;
    }
    dos_time = GetByte();
    dos_time |= GetByte() << 8;
    dos_date = GetByte();
    dos_date |= GetByte() << 8;
    Output( INDENT "File: " );
    buff[1] = '\0';
    for( len = GetByte(); len != 0; --len ) {
        buff[0] = GetByte();
        Output( buff );
    }
    Output( CRLF );
    t = d2t( dos_date, dos_time );
    p = ctime( &t );
    Output( INDENT "Time Stamp: " );
    Output( p );        // has embedded '\n'
}

static int doDisasmDirective( void ) {

    byte        ddir;
    unsigned_32 off1;
    unsigned_32 off2;
    unsigned_16 idx;
    unsigned_16 nidx;

    ddir = GetByte();
    switch( ddir ) {
    case DDIR_SCAN_TABLE_32:
    case DDIR_SCAN_TABLE:
        idx = GetIndex();
        if( idx == 0 ) {
            /* scan table in a COMDAT */
            nidx = GetIndex();
        }
        if( ddir == DDIR_SCAN_TABLE ) {
            off1 = GetUInt();
            off2 = GetUInt();
        } else {
            off1 = GetLInt();
            off2 = GetLInt();
        }
        Output( INDENT "Scan Table: " );
        if( idx != 0 ) {
            Output( "SI(%u)", idx );
        } else {
            Output( "COMDAT(%u)", nidx );
        }
        Output( "  begin=%X  end+1=%X" CRLF, off1, off2 );
        return( 1 );
    }
    BackupByte();
    return( 0 );
}

static void doVirtualConditional( void )
{
    unsigned idx;
    unsigned def_idx;

    idx = GetIndex();
    def_idx = GetIndex();
    Output( INDENT "EI(%u) default: EI(%u)\n", idx, def_idx );
    while( ! EndRec() ) {
        idx = GetIndex();
        Output( INDENT INDENT "conditional: LI(%u)\n", idx );
    }
}

static int doLinkerDirective( void ) {

    byte        ldir;
    byte        major;
    byte        minor;
    unsigned    e1;
    unsigned    s1;

    ldir = GetByte();
    switch( ldir ) {
    case LDIR_SOURCE_LANGUAGE:
        major = GetByte();
        minor = GetByte();
        Output( INDENT "WATCOM DBI: Version %u.%u  Language %R"
                CRLF, major, minor );
        return( 1 );
    case LDIR_DEFAULT_LIBRARY:
        major = GetByte();
        Output( INDENT "Default Library: Priority %u \"%R\"" CRLF,
            major );
        return( 1 );
    case LDIR_OPT_FAR_CALLS:
        Output( INDENT "Optimize Far Calls: SI(%u)" CRLF, GetIndex() );
        return( 1 );
    case LDIR_OPT_UNSAFE:
        Output( INDENT "Far Call Optimization Unsafe (Last FIXUPP)"
            CRLF );
        return( 1 );
    case LDIR_VF_TABLE_DEF:
        Output( INDENT "Virtual Function Conditional"
            CRLF );
        doVirtualConditional();
        return( 1 );
    case LDIR_VF_PURE_DEF:
        Output( INDENT "Pure Virtual Function Conditional"
            CRLF );
        doVirtualConditional();
        return( 1 );
    case LDIR_VF_REFERENCE:
        e1 = GetIndex();
        s1 = GetIndex();
        if( s1 == 0 ) {
            s1 = GetIndex();
            Output( INDENT "Virtual Function Reference EI(%u) CI(%u)"
                CRLF, e1, s1 );
        } else {
            Output( INDENT "Virtual Function Reference EI(%u) SI(%u)"
                CRLF, e1, s1 );
        }
        return( 1 );
    }
    BackupByte();
    return( 0 );
}

static int doEasyOmf( byte c_bits ) {

    if( c_bits == 0x80 && memcmp( RecPtr, EASY_OMF_SIGNATURE, 5 ) == 0 ) {
        Output( INDENT "---- PharLap 80386 object deck ----" CRLF );
        IsPharLap = TRUE;
        IsIntel = FALSE;
        return( 1 );
    }
    return( 0 );
}

static int doMSOmf( void ) {

    Output( INDENT "---- Microsoft extensions present ----" CRLF );
    IsIntel = FALSE;
    return( 1 );
}

void ProcComent( void )
/*********************/
{
    byte        c_bits;
    byte        c_class;
    int         dont_print;

    c_bits = GetByte();
    c_class = GetByte();
    Output( INDENT "bits %b, class %b" CRLF, c_bits, c_class );
    dont_print = 0;
    if( InterpretComent ) {
        switch( c_class ) {
        case CMT_LZEXT:
            Output( INDENT "Lazy extern (will search libs):" CRLF );
            doWeakLazyExtern();
            dont_print = 1;
            break;
        case CMT_WKEXT:
            Output( INDENT "Weak extern (won't search libs):" CRLF );
            doWeakLazyExtern();
            dont_print = 1;
            break;
        case CMT_DEPENDENCY:
            Output( INDENT "Borland Dependency:" CRLF );
            doDependency();
            dont_print = 1;
            break;
        case CMT_LANGUAGE_TRANS:
            Output( INDENT "Language Translator:" CRLF );
            break;
        case CMT_WAT_PROC_MODEL:
            Output( INDENT "WATCOM Compile parameters:" CRLF );
            doProcModel();
            dont_print = 1;
            break;
        case CMT_MS_PROC_MODEL:
            Output( INDENT "Microsoft Compile parameters:" CRLF );
            doProcModel();
            dont_print = 1;
            break;
        case CMT_DOSSEG:
            Output( INDENT "DOSSEG Segment Ordering" CRLF );
            break;
        case CMT_DEFAULT_LIBRARY:
            Output( INDENT "Default Library: \"%R\"" CRLF );
            dont_print = 1;
            break;
        case CMT_DLL_ENTRY:
            Output( INDENT "DLL Entry" CRLF );
            break;
        case CMT_MS_OMF:
            dont_print = doMSOmf();
            break;
        case CMT_MS_END_PASS_1:
            Output( INDENT "End of Linker Pass 1" CRLF );
            break;
        case CMT_EASY_OMF:
            dont_print = doEasyOmf( c_bits );
            break;
        case CMT_DISASM_DIRECTIVE:
            dont_print = doDisasmDirective();
            break;
        case CMT_LINKER_DIRECTIVE:
            dont_print = doLinkerDirective();
            break;
        }
    } else {
        /* we have to interpret these in order to handle the object file
           properly */
        switch( c_class ) {
        case CMT_EASY_OMF:
            dont_print = doEasyOmf( c_bits );
            break;
        case CMT_MS_OMF:
            dont_print = doMSOmf();
            break;
        }
    }
    if( ! ( dont_print || EndRec() ) ) {
        OutputData( 0L, 0L );
    }
}

void ProcNames( unsigned_16 *index )
/**********************************/
{
    while( ! EndRec() ) {
       GetName();
       Output( INDENT "%u - %N" CRLF, ++(*index) );
    }
}

void ProcExtNames( void )
/***********************/
{
    while( ! EndRec() ) {
       GetName();
       Output( INDENT "%u - %N Type:%u" CRLF, ++Importindex, GetIndex() );
    }
}


void ProcComExtDef() {
/********************/
    unsigned    name;
    unsigned    typ;

    while( ! EndRec() ) {
        name = GetIndex();
        typ = GetIndex();
        Output(INDENT "%u - LNAME:%u Type:%u" CRLF, ++Importindex, name, typ);
    }
}

static const char *oldAlign[] = {
/*ALIGN_ABS     */  "ABS",
/*ALIGN_BYTE    */  "BYTE",
/*ALIGN_WORD    */  "WORD",
/*ALIGN_PARA    */  "PARA",
/*ALIGN_PAGE    */  "PAGE",
/*ALIGN_UNABS   */  "UNABS",
/*ALIGN_LTRELOC */  "LTL"
};

static const char *newAlign[] = {
/*ALIGN_ABS     */  "ABS",
/*ALIGN_BYTE    */  "BYTE",
/*ALIGN_WORD    */  "WORD",
/*ALIGN_PARA    */  "PARA",
/*ALIGN_PAGE    */  "PAGE",
/*ALIGN_DWORD   */  "DWORD",
/*ALIGN_4KPAGE  */  "PAGE4K"
};

static const char *segComb[] = {
/*COMB_INVALID  */  "PRIVATE",
/*COMB_ABOVEALL */  "ABOVEALL",
/*COMB_ADDOFF   */  "PUBLIC",
/*COMB_BAD      */  "***bad***",
/*COMB_FOUR     */  "TYPE 4",
/*COMB_STACK    */  "STACK",
/*COMB_COMMON   */  "COMMON",
/*COMB_ALIGNTOP */  "ALIGNTOP"
};

void ProcSegDefs( void )
/**********************/
{
    byte        acbp;
    byte        align;
    unsigned_32 length;
    byte        comb;
    byte        phar_attr;
    const char  *phar_access;
    unsigned_16 abs_frame;
    unsigned_32 abs_offset;
    byte        ltl_dat;
    unsigned_16 ltl_len;
    unsigned_16 ltl_offset;
    unsigned_16 seg;
    unsigned_16 class;
    unsigned_16 ovl;
    int         use32;

    ++Segindex;
    acbp = GetByte();
    align = acbp >> 5;
    if( align >= ALIGN_4KPAGE ) {
        Output( BAILOUT "Unknown align, acbp=%b" CRLF, acbp );
        longjmp( BailOutJmp, 1 );
    }
    comb = ( acbp >> 2 ) & 7;
    switch( align ) {
    case ALIGN_UNABS:
        if( !IsIntel ) break;
            /* FALL THROUGH */
    case ALIGN_ABS:
        abs_frame = GetUInt();
             /* note MS386 has a 16bit quantity here */
        abs_offset = ( IsPharLap ) ? GetLInt() : GetUInt();
        break;
    case ALIGN_LTRELOC:
        if( IsIntel ) {
            ltl_dat = GetByte();
            ltl_len = GetUInt();
            ltl_offset = GetUInt();
        }
        break;
    }
    length = GetEither();
    if( acbp & 2 ) {
        length = 0x10000;   /* FIXME - should handle 4G segments */
    }
    if( !IsIntel || align != ALIGN_UNABS ) {
        seg = GetIndex();
        class = GetIndex();
        ovl = GetIndex();
    }
    phar_access = NULL;
    if( IsPharLap && EndRec() == FALSE ) {
        phar_attr = GetByte();
        use32 = phar_attr & EASY_USE32_FIELD;
        phar_attr &= EASY_PROTECT_FIELD;
        switch( phar_attr ) {
        case EASY_READ_ONLY:    phar_access = " RO";    break;
        case EASY_EXEC_ONLY:    phar_access = " EO";    break;
        case EASY_EXEC_READ:    phar_access = " ER";    break;
        case EASY_READ_WRITE:   phar_access = " RW";    break;
        }
    } else if( IsPharLap || ( !IsIntel && ( acbp & 1 ) ) ) {
        use32 = 1;
    } else {
        use32 = 0;
    }
    if( !IsIntel || align != ALIGN_UNABS ) {
        Output( INDENT "%u: Seg:%u Class:%u Ovl:%u %s %s %s%s Length %X" CRLF,
            Segindex, seg, class, ovl,
            ( IsIntel ) ? oldAlign[align] : newAlign[align],
            segComb[ comb ],
            ( use32 ) ? "USE32" : "USE16",
            ( phar_access != NULL ) ? phar_access : "",
            length
        );
    } else {
        Output( INDENT "%u: Unnamed Absolute %s %s USE16 Length %X" CRLF,
            Segindex, oldAlign[ ALIGN_UNABS ], segComb[ comb ], length );
    }
    if( align == ALIGN_ABS || ( IsIntel && align == ALIGN_UNABS ) ) {
        Output( INDENT "Frame: %x Offset: %X" CRLF, abs_frame, abs_offset );
    }
    if( IsIntel && align == ALIGN_LTRELOC ) {
        Output( INDENT "LTL Data:%b Len:%x Offset:%x" CRLF,
                ltl_dat, ltl_len, ltl_offset );
    }
}

static void getBase( int indent )
{
    unsigned_16 group;
    unsigned_16 seg;

    group = GetIndex();
    seg = GetIndex();
    if( indent ) Output( INDENT );
    if( group == 0 && seg == 0 ) {
        Output( "Frame: %x", GetUInt() );
    } else {
        Output( "Group: %u, Seg: %u", group, seg );
    }
    if( indent ) Output( CRLF );
}

void ProcLocSyms( void )
/**********************/
{
    ProcPubDefs();
}

void ProcPubDefs( void )
/**********************/
{
    unsigned_32 puboff;

    getBase( TRUE );
    while( ! EndRec() ) {
        GetName();
        puboff = GetEither();
        Output( INDENT "%X - %N Type:%u" CRLF, puboff, GetIndex() );
    }
}

void ProcComDef( void )
/*********************/
{
    unsigned_16 type;
    byte        seg;
    unsigned_32 num;
    unsigned_32 size;

    while( ! EndRec() ) {
        GetName();
        type = GetIndex();  /* type index */
        seg = GetByte();   /* data seg type */
        if( seg == COMDEF_FAR ) {
            num = GetVariable();
            size = GetVariable();
            Output( INDENT "%u - %N Type %u, FAR, Num:%X, Size:%X" CRLF,
                ++Importindex, type, num, size );
        } else if( seg == COMDEF_NEAR ) {
            size = GetVariable();
            Output( INDENT "%u - %N Type %u, NEAR, Size:%X" CRLF,
                ++Importindex, type, size );
        } else {
            Output( INDENT "%u - %N Type %u" BAILOUT "Unknown seg(%b)" CRLF,
                ++Importindex, type, seg );
            longjmp( BailOutJmp, 1 );
        }
    }
}


static void DoLinNums()
/*********************/
{
    unsigned_16 line_num;
    unsigned_32 offset;
    unsigned    count;

    if( Descriptions ) {
        Output( INDENT "Number pairs are \"line#:offset\"" CRLF );
    }
    if( ! EndRec() ) {
        count = 0;
        do {
            line_num = GetUInt();
            offset = GetEither();
            switch( count % 4 ) {
            case 0:
                Output( INDENT "%5:%X", line_num, offset );
                break;
            case 3:
                Output( "  %5:%X" CRLF, line_num, offset );
                break;
            default:
                Output( "  %5:%X", line_num, offset );
                break;
            }
            ++count;
        } while( ! EndRec() );
        if( count % 4 != 0 ) {  /* if count % 4 == 0 then CRLF already output */
            Output( CRLF );
        }
    }
}


void ProcLinNums( void )
/**********************/
{
    getBase( TRUE );
    DoLinNums();
}


void ProcLineSym()
/****************/
{
    unsigned    flag;
    unsigned    sym;

    flag = GetByte();
    sym = GetIndex();
    Output( INDENT "sym:%u", sym );
    if( flag & 1 ) {
        Output( " - continued" );
    }
    Output( CRLF );
    DoLinNums();
}


static void idBlock( unsigned indent, unsigned_16 first_block_offset )
{
    unsigned_32 rpt_count;
    unsigned_16 blk_count;
    byte        data_count;

    rpt_count = IsMS386 ? GetLInt() : GetUInt();
    Output( "%>Repeated %X times" CRLF, indent, rpt_count );
    blk_count = GetUInt();
    if( blk_count == 0 ) {
        data_count = GetByte();
        OutputData( (unsigned_32)( RecOffset() - first_block_offset ),
            (uint_32)data_count );
#if 0
        Output( "%>", indent );
        for( num = 0; num < data_count; ++num ) {
            Output( "%b ", GetByte() );
            if( ( num & 0x7 ) == 7 ) {
                Output( CRLF "%>", indent );
            }
        }
        if( ( data_count & 0x7 ) != 7 ) {
            Output( CRLF );
        }
#endif
    } else {
        while( blk_count != 0 ) {
            idBlock( indent + (INDWIDTH/2), first_block_offset );
            --blk_count;
        }
    }
}

static unsigned_32 begData( void )
{
    unsigned_16 seg;
    unsigned_32 offset;

    seg = GetIndex();
    offset = GetEither();
    Output( INDENT "Seg index:%u offset:%X" CRLF, seg, offset );
    return( offset );
}


static void DoLidata()
/********************/
{
    unsigned_16 first_block_offset;

    first_block_offset = RecOffset();
    while( ! EndRec() ) {
        idBlock( INDWIDTH, first_block_offset );
    }
}


void ProcLidata( void )
/*********************/
{
    begData();
    DoLidata();
}

void ProcLedata( void )
/*********************/
{
    unsigned_32 offset;

    offset = begData();
    OutputData( offset, 0L );
}

static void doFrame( byte frame )
{
    switch( frame ) {
    case F_SEG:     Output( "SI(%u)%<", GetIndex(), 8 );     break;
    case F_GRP:     Output( "GI(%u)%<", GetIndex(), 8 );     break;
    case F_EXT:     Output( "EI(%u)%<", GetIndex(), 8 );     break;
    case F_ABS:     Output( "%x%<", GetUInt(), 8 );          break;
    case F_LOC:     Output( "LOCATION" );                    break;
    case F_TARG:    Output( "TARGET  " );                    break;
    case F_NONE:    Output( "NONE    " );                    break;
    default:
        Output( BAILOUT "Unknown frame(%b)" CRLF, frame );
        longjmp( BailOutJmp, 1 );
    }
}

static void doTarget( byte target )
{
    switch( target & 0x03 ) {
    case T_SEGWD:   Output( "SI(%u)", GetIndex() );         break;
    case T_GRPWD:   Output( "GI(%u)", GetIndex() );         break;
    case T_EXTWD:   Output( "EI(%u)", GetIndex() );         break;
    case T_ABSWD:   Output( "%x", GetUInt() );              break;
    }
}

static void threadFixup( byte typ )
{
    byte    num;
    byte    frame;

    num = typ & 0x03;
    frame = ( typ >> 2 ) & 0x07;
    if( ( typ & TRDDAT_DBIT ) != 0 ) {
        Output( INDENT "Thread: FRAME  %u: ", num );
        doFrame( frame );
    } else {
        Output( INDENT "Thread: TARGET %u: ", num );
        doTarget( frame );
    }
    Output( CRLF );
}

static void explicitFixup( byte typ )
{
    byte        loc;
    unsigned_16 offset;
    byte        frame;

    offset = ( ( typ & 0x03 ) << 8 ) + GetByte();
    Output( INDENT "%x %s", offset, ( typ & FIXDAT_MBIT ) ? "Seg " : "Self" );
    loc = ( ( typ >> 2 ) & 0x0f );
    if( IsPharLap ) {
        if( loc > LOC_BASE_OFFSET_32 ) {
            Output( BAILOUT "Unknown loc, type = %b" CRLF, typ );
            longjmp( BailOutJmp, 1 );
        } else if( loc == LOC_MS_LINK_OFFSET ) {
            loc = LOC_MS_OFFSET_32;
        }
    } else if( IsIntel ) {
        if( loc > LOC_MS_LINK_OFFSET ) {
            Output( BAILOUT "Unknown loc, type = %b" CRLF, typ );
            longjmp( BailOutJmp, 1 );
        }
    }
    switch( loc ) {
    case LOC_OFFSET_LO:         Output( "  LOBYTE     " );   break;
    case LOC_OFFSET:            Output( "  OFFSET     " );   break;
    case LOC_BASE:              Output( "  BASE       " );   break;
    case LOC_BASE_OFFSET:       Output( "  POINTER    " );   break;
    case LOC_OFFSET_HI:         Output( "  HIBYTE     " );   break;
    case LOC_MS_LINK_OFFSET:    Output( "  LROFFSET   " );   break;
    case LOC_BASE_OFFSET_32:    /* fall through */
    case LOC_MS_BASE_OFFSET_32: Output( "  POINTER386 " );   break;
    case LOC_MS_OFFSET_32:      Output( "  OFFSET386  " );   break;
    case LOC_MS_LINK_OFFSET_32: Output( "  LROFFSET386" );   break;
    default:
        Output( BAILOUT "Unknown loc, type = %b" CRLF, typ );
        longjmp( BailOutJmp, 1 );
    }
    typ = GetByte();
    frame = ( typ >> 4 ) & 0x07;
    Output( "  Frame: " );
    if( typ & FIXDAT_FTHREAD ) {
        Output( "THREAD %u", frame );
    } else {
        doFrame( frame );
    }
    loc = typ & 0x03;
    Output( "  Target: " );
    if( typ & FIXDAT_TTHREAD ) {
        Output( "THREAD %u", loc & 0x03 );
    } else {
        doTarget( loc );
    }
    if( typ & 0x04 ) {
        Output( CRLF );
    } else {
        Output( ",%X" CRLF, GetEither() );
    }
}

void ProcFixup( void )
/********************/
{
    byte    typ;

    if( Descriptions ) {
        Output( INDENT "Locat Mode    Method     "
            "  Fixup Frame    Fixup Target(,displacement)" CRLF );
        Output( INDENT "----- ----  -----------  "
            "---------------  ---------------------------" CRLF );
    }
    while( ! EndRec() ) {
        typ = GetByte();
        if( typ & FIXUPP_FIXUP ) {
            explicitFixup( typ );
        } else {
            threadFixup( typ );
        }
    }
}

void ProcModEnd( void )
/*********************/
{
    byte        mod_type;
    byte        typ;
    byte        frame;
    byte        loc;

    mod_type = GetByte();
    Output( INDENT "mod type:%smain module %s" CRLF,
        ( mod_type & 0x2 ) ? "":"non-",
        ( mod_type & 0x1 ) ? "with start address" : ""
    );
    if( mod_type & 0x1 && !EndRec() ) {
        typ = GetByte();
        frame = ( typ >> 4 ) & 0x07;
        Output( INDENT "Frame: " );
        if( typ & FIXDAT_FTHREAD ) {
            Output( "THREAD %u", frame );
        } else {
            doFrame( frame );
        }
        loc = typ & 0x03;
        Output( "  Target: " );
        if( typ & FIXDAT_TTHREAD ) {
            Output( "THREAD %u", loc & 0x03 );
        } else {
            doTarget( loc );
        }
        if( typ & 0x04 ) {
            Output( CRLF );
        } else {
            Output( ",%X" CRLF, GetEither() );
        }
    }
}

static void dmpBytes( int num )
{
    while( num-- > 0 ) {
        Output( " %b", GetByte() );
    }
}

void ProcGrpDef( void )
/*********************/
{
    byte        grptype;

    Output( INDENT "name: %u" CRLF, GetIndex() );
    while( !EndRec() ) {
        grptype = GetByte();
        Output( INDENT "member: " );
        switch( grptype ) {
            case GRP_SEGIDX:
                Output( "seg %u", GetIndex() );
                break;
            case GRP_EXTIDX:
                Output( "ext %u", GetIndex() );
                break;
            case GRP_FULLNAME:
                {
                    unsigned_16 seg;
                    unsigned_16 class;
                    unsigned_16 ovl;

                    seg = GetIndex();
                    class = GetIndex();
                    ovl = GetIndex();
                    Output( "seg %u, class %u, ovl %u", seg, class, ovl );
                }
                break;
            case GRP_LTLDATA:
                Output( "%b", grptype );
                dmpBytes( 6 );
                break;
            case GRP_ADDR:
                Output( "%b", grptype );
                dmpBytes( 4 );
                break;
            default:
                Output( BAILOUT "Unknown group type(%b)" CRLF, grptype );
                longjmp( BailOutJmp, 1 );
                break;
        }
        Output( CRLF );
    }
}

static unsigned_32 libDictOffs;
static unsigned_16 libDictSize;

void ProcLibHeader( void ) {

    byte                flags;

    Output( INDENT "Page size        : %u" CRLF, RecLen + 4 );
    libDictOffs = GetLInt();
    libDictSize = GetUInt();
    flags = GetByte();
    Output( INDENT "Dictionary Offset: %X" CRLF, libDictOffs );
    Output( INDENT "Dictionary Size  : %x" CRLF, libDictSize );
    Output( INDENT "Case-Sensitive   : %s" CRLF,
        ( flags & 1 ) ? "Yes" : "No" );
    OutputData( 0, 0 );
}

static void doBucket( int bucket ) {
    byte                bucket_value;

    bucket_value = GetByte();
    if( bucket_value == 0 ) {
        Output( "%b:     ", bucket );
    } else {
        Output( "%b:%x", bucket, 2*bucket_value );
    }
}

void ProcLibTrailer( FILE *fp ) {

    unsigned_16         dict_block;
    int                 i;
    int                 free;

    if( fseek( fp, libDictOffs, SEEK_SET ) != 0 ) {
        return;
    }
    ResizeBuff( 512 );
    for( dict_block = 0; dict_block < libDictSize; ++dict_block ) {
        if( fread( RecBuff, 512, 1, fp ) == 0 ) {
             break;
        }
        RecPtr = RecBuff;
        RecLen = 512;
        Output( CRLF "Dictionary Block %x:" CRLF
            INDENT "Buckets:" CRLF INDENT, dict_block );
        for( i = 0; i < 36; i += 6 ) {
            doBucket( i ); Output( "  " );
            doBucket( i + 1 ); Output( "  " );
            doBucket( i + 2 ); Output( "  " );
            doBucket( i + 3 ); Output( "  " );
            doBucket( i + 4 ); Output( "  " );
            doBucket( i + 5 ); Output( CRLF INDENT );
        }
        doBucket( 36 );
        free = GetByte() * 2;
        Output( CRLF INDENT "Free Byte: %x" CRLF INDENT "Entries:" CRLF,
            free );
        while( RecOffset() < free ) {
            i = RecOffset();
            GetName();
            Output( INDENT "%x: %N Page %x" CRLF, i, GetUInt() );
            if( RecOffset() & 1 ) {
                GetByte();
            }
        }
    }
}


static char *PatchType() {
/************************/
    char                *fix;

    switch( GetByte() ) {
    case 0: fix = "8-bit lobyte";       break;
    case 1: fix = "16-bit offset";      break;
    case 2: fix = "32-bit offset";      break;
    default: fix = "*** ill fix type ***"; break;
    }
    return( fix );
}


static void DoBackPat() {
/***********************/
    unsigned_32         off;
    unsigned_32         val;

    while( !EndRec() ) {
        off = GetEither();
        val = GetEither();
        Output( INDENT "    offset:%X - value:%X" CRLF, off, val );
    }
}


void ProcBackPat() {
/******************/
    unsigned            seg;
    char                *fix;

    seg = GetIndex();
    fix = PatchType();
    Output( INDENT "Segment:%u - %s" CRLF, seg, fix );
    DoBackPat();
}


void ProcNameBackPat() {
/**********************/
    unsigned            sym;
    char                *fix;

    fix = PatchType();
    sym = GetIndex();
    Output( INDENT "Symbol:%u - %s" CRLF, sym, fix );
    DoBackPat();
}


void ProcComDat() {
/*****************/
    unsigned    flag;
    unsigned    attr;
    unsigned    sel;
    unsigned    alloc;
    unsigned    align;
    unsigned_32 offset;
    unsigned    typ;
    unsigned    sym;
    static char *ComAlign[] = {
        "SEGDEF", "BYTE", "WORD", "PARA", "4K", "DWORD"
    };
    static char *ComSel[] = {
        "NO MATCH", "ANY", "SAME SIZE", "EXACT MATCH"
    };
    static char *ComAlloc[] = {
        "EXPLICIT", "FAR CODE", "FAR DATA", "CODE32", "DATA32"
    };

    flag = GetByte();
    attr = GetByte();
    sel = (attr >> 4) & 0xf;
    alloc = attr & 0xf;
    if( sel > 3 || alloc > 4 ) {
        Output( BAILOUT "Unknown attr => %b" CRLF, attr );
        longjmp( BailOutJmp, 1 );
    }
    align = GetByte();
    if( align > 5 ) {
        Output( BAILOUT "Unknown align => %b" CRLF, align );
        longjmp( BailOutJmp, 1 );
    }
    offset = GetEither();
    typ = GetIndex();
    Output( INDENT "align:%s, select:%s, alloc:%s",
            ComAlign[align], ComSel[sel], ComAlloc[alloc] );
    if( alloc == 0 ) { /* explict allocation */
        Output( " ==> " );
        getBase( FALSE );
    }
    Output( CRLF );
    sym = GetIndex();
    Output( INDENT "sym:%u, typ:%u, offset:%X", sym, typ, offset );
    if( flag & 0x1 ) Output( " continued" );
    if( flag & 0x4 ) Output( " local" );
    if( flag & 0x2 ) Output( " iterated" );
    else             Output( " enumerated" );
    Output( CRLF );
    if( flag & 0x2 ) {
        DoLidata();
    } else {
        OutputData( offset, 0L );
    }
}


void ProcAlias() {
/****************/

    GetName();
    Output( INDENT "alias = <%N> "  );
    GetName();
    Output( "substitue = <%N>" CRLF );
}


void ProcVerNum() {
/*****************/

    GetName();
    Output( INDENT "***** TIS compliant OMF *****" CRLF );
    Output( INDENT INDENT "Version <%N>" CRLF );
}


void ProcVendExt() {
/******************/

    OutputData( 0L, 0L );
}
