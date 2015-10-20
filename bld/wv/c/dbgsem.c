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
* Description:  Semantic actions for SSL-driven grammars.
*
****************************************************************************/


#include <stdio.h>
#include <limits.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgstk.h"
#include "dbgerr.h"
#include "dipwv.h"
#include "dip.h"
#include "i64.h"
#include "dbgscan.h"
#include "numscan.h"
#include "madinter.h"
#include "dbgexpr4.h"
#include "dbgexpr3.h"
#include "dbgexpr2.h"
#include "dbgexpr.h"
#include "dbgloc.h"
#include "dbgsem.h"

#include "clibext.h"


#define         SSL_VERSION_MAJOR_CURR  0x0100
#define         SSL_VERSION_MINOR_CURR  0x0000

#define         SSL_VERSION_MAJOR_MASK  0xff00
#define         SSL_VERSION_MINOR_MASK  0x00ff

#define         TI_KIND_EXTRACT( ti )   (((ti) >> 12) & 0xf)
#define         TI_MOD_EXTRACT( ti )    (((ti) >>  8) & 0xf)
#define         TI_SIZE_EXTRACT( ti )   ((ti) & 0xff)
#define         TI_CREATE( tk, tm, ts ) (((tk) << 12) | ((tm) << 8) | (ts))

enum {
    SEM_MISC        = 0x00,
    SEM_DO          = 0x20,
    SEM_PUSH_N_POP  = 0x40,
    SEM_STACK       = 0x60,
    SEM_NUM         = 0x80,
    SEM_BITS        = 0xa0,
    SEM_GET         = 0xc0,
    SEM_SELECTOR    = 0x1f,
    SEM_MECHANISM   = 0xe0,
};

extern void             ParseRegSet( bool, location_list *, dip_type_info * );
extern stack_entry      *StkEntry( int );
extern mod_handle       LookupModName( mod_handle, const char *, int );
extern void             AddChar( void );
extern address          GetDotAddr( void );
extern sym_list         *LookupSymList( symbol_source, void *, bool, lookup_item * );
extern void             PurgeSymHandles( void );
extern bool             IsInternalMod( mod_handle );
extern sym_list         *Disambiguate( sym_list * );
extern void             FreeSymHandle( sym_list * );
extern image_entry      *ImagePrimary( void );
extern mod_handle       LookupImageName( const char *, unsigned );


extern stack_entry      *ExprSP;


#define type_bitsII     int

/* SSL Table Variables */
static type_bitsII      Bits;
static unsigned         Num;
static unsigned         EvalSubstring;
static struct {
    lookup_item         li;
    unsigned            multi_module    : 1;
    unsigned            any_image       : 1;
    enum { GET_NAME, GET_OPERATOR, GET_LNUM }   kind;
}                       CurrGet;

#define MAX_SCANSAVE_PTRS 20
static  const char      *CurrScan[MAX_SCANSAVE_PTRS];
        int             ScanSavePtr;


//NYI: begin temp until SSL files can be updated
typedef enum {
    STK_VOID     = 0x0000,
    STK_BOOL     = 0x0001,
    STK_ENUM     = 0x0002,
    STK_CHAR     = 0x0003,
    STK_INT      = 0x0004,
    STK_NEAR_PTR = 0x0005,
    STK_FAR_PTR  = 0x0006,
    STK_ADDR     = 0x0007, /* just a raw address */
    STK_REAL     = 0x0008,
    STK_STRUCT   = 0x0009,
    STK_ARRAY    = 0x000a,
    STK_FUNC     = 0x000b,
    STK_TYPE     = 0x000c, /* have a type name */
    STK_NAME     = 0x000d, /* have a symbol name */
    STK_COMPLEX  = 0x000f,
    STK_STRING   = 0x0010,
    STK_OPERATOR = 0x0011,
    STK_END_PURGE= 0x003f,
#define BASE_TYPE 0x003f
    STK_UNSIGNED = 0x0100, /* meaningful for int & char only */
    STK_LONG     = 0x0200, /*   int (short/long);
                                real (float/double),
                                complex (single/double precision)
                                addresses (286 / 386)
                            */
} stack_class;

static stack_class TypeInfoToClass( dip_type_info *ti )
{
    stack_class         c;

    c = STK_VOID;
    switch( ti->kind ) {
    case TK_INTEGER:
        c = STK_INT;
        if( ti->modifier == TM_UNSIGNED ) c |= STK_UNSIGNED;
        if( ti->size == 4 ) c |= STK_LONG;
        break;
    case TK_POINTER:
        c = STK_FAR_PTR;
        if( ti->size == 6 ) c |= STK_LONG;
        break;
    case TK_ADDRESS:
        c = STK_ADDR;
        if( ti->size == 6 ) c |= STK_LONG;
        break;
    case TK_ARRAY:
        c = STK_ARRAY;
        break;
    case TK_STRING:
        c = STK_STRING;
        break;
    case TK_FUNCTION:
        c = STK_FUNC;
        break;
    }
    return( c );
}

static void ClassToTypeInfo( stack_class c, dip_type_info *ti )
{
    switch( c ) {
    case STK_INT | STK_UNSIGNED:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_UNSIGNED;
        ti->size = 2;
        break;
    case STK_INT | STK_UNSIGNED | STK_LONG:
        ti->kind = TK_INTEGER;
        ti->modifier = TM_UNSIGNED;
        ti->size = 4;
        break;
    case STK_FAR_PTR:
        ti->kind = TK_POINTER;
        ti->modifier = TM_FAR;
        ti->size = 4;
        break;
    case STK_FAR_PTR | STK_LONG:
        ti->kind = TK_POINTER;
        ti->modifier = TM_FAR;
        ti->size = 6;
        break;
    case STK_ADDR:
        ti->kind = TK_ADDRESS;
        ti->modifier = TM_FAR;
        ti->size = 4;
        break;
    case STK_ADDR | STK_LONG:
        ti->kind = TK_ADDRESS;
        ti->modifier = TM_FAR;
        ti->size = 6;
        break;
    case STK_ARRAY:
        ti->kind = TK_ARRAY;
        ti->modifier = TM_NONE;
        ti->size = 0;
        break;
    case STK_FUNC:
        ti->kind = TK_FUNCTION;
        ti->modifier = TM_NONE;
        ti->size = 0;
        break;
    case STK_STRING:
        ti->kind = TK_STRING;
        ti->modifier = TM_NONE;
        ti->size = 0;
        break;
    }
}
#define SSL_CASE_SENSITIVE      0x00000001UL // must be 0x01 - see dbgintr.ssl
#define SSL_SIDE_EFFECT         0x00000002UL // must be 0x02 - see dbgintr.ssl
#define SSL_32_BIT              0x00000004UL // must be 0x04 - see dbgintr.ssl

//NYI: end temp

static void FillInDefaults( dip_type_info *info )
{
    mad_type_info       mti;

    switch( info->kind ) {
    case TK_INTEGER:
        if( info->modifier == TM_NONE ) info->modifier = TM_SIGNED;
        if( info->size == 0 ) {
            if( ModDefault( CodeAddrMod, DK_INT, info ) != DS_OK ) {
                GetMADTypeDefault( MTK_INTEGER, &mti );
                info->size = mti.b.bits / BITS_PER_BYTE;
            }
        }
        break;
    case TK_POINTER:
    case TK_CODE:
    case TK_DATA:
        if( info->modifier == TM_NONE ) {
            if( ModDefault( CodeAddrMod, (info->kind == TK_CODE) ? DK_CODE_PTR : DK_DATA_PTR, info ) != DS_OK ) {
                info->modifier = TM_NONE;
                info->size = 0;
            }
        }
        if( info->modifier == TM_NONE || info->size == 0 ) {
            GetMADTypeDefault( MTK_ADDRESS, &mti );
            if( info->modifier == TM_NONE ) {
                if( mti.a.seg.bits != 0 ) {
                    info->modifier = TM_FAR;
                } else {
                    info->modifier = TM_NEAR;
                }
            }
            if( info->size == 0 ) {
                if( info->modifier == TM_NEAR ) mti.b.bits -= mti.a.seg.bits;
                info->size = mti.b.bits / BITS_PER_BYTE;
            }
        }
        break;
    }
}

static unsigned MechMisc( unsigned select, unsigned parm )
{
    long                value;
    unsigned            result = 0;
    mad_type_info       mti;

    switch( select ) {
    case 0:
        ExprAddrDepth += parm;
        result = ExprAddrDepth;
        break;
    case 1:
        if( _IsOn( SW_EXPR_IS_CALL ) && ExprAddrDepth == 0 ) {
            result = TRUE;
        } else {
            result = FALSE;
        }
        break;
    case 2:
        SkipCount += parm;
        break;
    case 3:
        result = SkipCount;
        break;
    case 4:
        //never called
        break;
    case 5:
        if( ScanSavePtr >= MAX_SCANSAVE_PTRS )
            Error( ERR_INTERNAL, LIT_ENG( ERR_TOO_MANY_SCANSAVE ) );
        CurrScan[ScanSavePtr++] = ScanPos();
        break;
    case 6:
        if( ScanSavePtr <= 0 ) Error( ERR_INTERNAL, LIT_ENG( ERR_TOO_MANY_SCANRESTORE ) );
        ReScan( CurrScan[--ScanSavePtr] );
        break;
    case 7:
        if( ScanSavePtr <= 0 ) Error( ERR_INTERNAL, LIT_ENG( ERR_TOO_MANY_SCANRESTORE ) );
        --ScanSavePtr;
        break;
    case 8:
        if( parm ) {        /* start scan string */
            scan_string = TRUE;
            ReScan( ScanPos() );
        } else {            /* end scan string */
            scan_string = FALSE;
            ReScan( ScanPos() );
        }
        break;
    case 9:
        ReScan( ScanPos() + (int)parm );
        break;
    case 10:
        AddChar();
        break;
    case 11:
        AddCEscapeChar();
        break;
    case 12:
        AddActualChar( '\0' );
        break;
    case 13:
        ScanCCharNum = parm;
        break;
    case 14:
        if( NestedCallLevel == MAX_NESTED_CALL - 1 ) {
            Error( ERR_NONE, LIT_ENG( ERR_TOO_MANY_CALLS ) );
        } else {
            PgmStackUsage[ ++NestedCallLevel ] = 0;
        }
        break;
    case 15:
        RValue( ExprSP );
        ConvertTo( ExprSP, TK_INTEGER, TM_SIGNED, 4 );
        value = U32FetchTrunc( ExprSP->v.uint ) - 1;
        PopEntry();
        if( ExprSP->info.kind == TK_STRING ) {
            if( value < 0 || value >= ExprSP->info.size ) {
                Error( ERR_NONE, LIT_ENG( ERR_BAD_SUBSTRING_INDEX ) );
            }
            LocationAdd( &ExprSP->v.string.loc, value*8 );
            ExprSP->info.size -= value;
            ExprSP->v.string.ss_offset = value;
        } else {
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        }
        break;
    case 16:
        RValue( ExprSP );
        ConvertTo( ExprSP, TK_INTEGER, TM_SIGNED, 4 );
        value = U32FetchTrunc( ExprSP->v.sint ) - 1;
        PopEntry();
        if( ExprSP->info.kind == TK_STRING ) {
            value -= ExprSP->v.string.ss_offset;
            if( value < 0 || value >= ExprSP->info.size ) {
                Error( ERR_NONE, LIT_ENG( ERR_BAD_SUBSTRING_INDEX ) );
            }
            ExprSP->info.size = value;
        } else {
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
        }
        break;
    case 17:
        EvalSubstring = parm;
        if( parm ) ExprSP->v.string.ss_offset = 0;
        break;
    case 18:
        result = EvalSubstring;
        break;
    case 19:
        FreePgmStack( TRUE );
        break;
    case 20:
        switch( parm ) { // nyi - begin temp
        case SSL_CASE_SENSITIVE:
            _SwitchOn( SW_CASE_SENSITIVE );
            break;
        case SSL_SIDE_EFFECT:
            _SwitchOn( SW_SIDE_EFFECT );
            break;
        //case SSL_32_BIT:
        //    _SwitchOn( SW_32_BIT );
        //    break;
        }
        break;
    case 21:
        switch( parm ) {
        case SSL_CASE_SENSITIVE:
            _SwitchOff( SW_CASE_SENSITIVE );
            break;
        case SSL_SIDE_EFFECT:
            _SwitchOff( SW_SIDE_EFFECT );
            break;
        //case SSL_32_BIT:
        //    _SwitchOff( SW_32_BIT );
        //    break;
        }
        break;
    case 22:
        switch( parm ) {
        case SSL_CASE_SENSITIVE:
            result = _IsOn( SW_CASE_SENSITIVE );
            break;
        case SSL_SIDE_EFFECT:
            result = _IsOn( SW_SIDE_EFFECT );
            break;
        case SSL_32_BIT:
            GetMADTypeDefault( MTK_INTEGER, &mti );
            result = (mti.b.bits >= 32);
            break;
        }
        break;
    case 23: // nyi - end temp
        MarkArrayOrder( parm );
        break;
    case 24:
        StartSubscript();
        break;
    case 25:
        AddSubscript();
        break;
    case 26:
        EndSubscript();
        break;
    }
    return( result );
}

static bool UserType( type_handle *th )
{
    unsigned            i;
    unsigned            len;
    sym_info            info;


    //NYI:begin temp
    if( ExprSP->flags & SF_NAME ) {
        static const char       * const TagIds[] = { "struct", "class", "union", "enum", NULL };

        ExprSP->v.li.type = ST_TYPE;
        for( i = 0; TagIds[i] != NULL; ++i ) {
            len = strlen( TagIds[i] );
            if( len == ExprSP->v.li.name.len
                && memcmp( ExprSP->v.li.name.start, TagIds[i], len ) == 0 ) {
                ExprSP->v.li.type = ST_STRUCT_TAG + i;
                ExprSP->v.li.name.start = NamePos();
                ExprSP->v.li.name.len = NameLen();
                Scan();
            }
        }
    }
    //NYI: end temp
    NameResolve( ExprSP, TRUE );
    if( !(ExprSP->flags & SF_SYM) ) return( FALSE );
    if( ExprSP->th == NULL ) return( FALSE );
    SymInfo( ExprSP->v.sh, ExprSP->lc, &info );
    if( info.kind != SK_TYPE ) return( FALSE );
    HDLAssign( type, th, ExprSP->th );
    return( TRUE );
}

static void PushBaseSize( void )
{
    DIPHDL( type, th );
    dip_type_info   ti;

    TypeBase( ExprSP->th, th, NULL, NULL );
    TypeInfo( th, ExprSP->lc, &ti );
    PushNum( ti.size );
}

static void ScaleInt( void )
{
    MoveSP( 1 );
    PushBaseSize();
    MoveSP( -1 );
    DoMul();
}

static void DoPlusScaled( void )
{
    stack_entry         *left;

    left = StkEntry( 1 );
    LRValue( left );
    RValue( ExprSP );
    switch( left->info.kind ) {
    case TK_POINTER:
        ScaleInt();
        break;
    default:
        switch( ExprSP->info.kind ) {
        case TK_POINTER:
            SwapStack( 1 );
            ScaleInt();
            break;
        }
    }
    DoPlus();
}

static void DoMinusScaled( void )
{
    stack_entry *left;

    left = StkEntry( 1 );
    LRValue( left );
    RValue( ExprSP );
    switch( left->info.kind ) {
    case TK_POINTER:
        switch( ExprSP->info.kind ) {
        case TK_POINTER:
            /* Have to check if base type sizes are the same */
            PushBaseSize();
            DupStack();
            SwapStack( 3 );
            PushBaseSize();
            MoveSP( 1 );
            SwapStack( 3 );
            MoveSP( -1 );
            if( !TstEQ( 1 ) ) {
                Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
            }
            PopEntry();
            MoveSP( 1 );
            DoMinus();
            MoveSP( -1 );
            DoDiv();
            return;
        default:
            ScaleInt();
            break;
        }
        break;
    default:
        switch( ExprSP->info.kind ) {
        case TK_POINTER:
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
            break;
        }
    }
    DoMinus();
}

static unsigned MechDo( unsigned select, unsigned parm )
{
    unsigned long       size;
    unsigned            result = 0;
    DIPHDL( type, th );
    dip_type_info       info;
    mad_type_info       mti;

    switch( select ) {
    case 0:
        DoAssign();
        break;
    case 1:
        DoMul();
        break;
    case 2:
        DoDiv();
        break;
    case 3:
        DoMod();
        break;
    case 4:
        DoMinus();
        break;
    case 5:
        DoShift();
        break;
    case 6:
        DoAnd();
        break;
    case 7:
        DoXor();
        break;
    case 8:
        DoOr();
        break;
    case 9:
        DoAddr();
        break;
    case 10:
        ClassToTypeInfo( parm, &info );
        DoPoints( info.kind );
        break;
    case 11:
        DoField();
        break;
    case 12:
        DoCall( Num, parm );
        break;
    case 13:
        DoConvert();
        break;
    case 14:
        DoPlus();
        break;
    case 15:
        MakeAddr();
        break;
    case 16:
        result = TstEQ( parm );
        break;
    case 17:
        result = TstLT( parm );
        break;
    case 18:
        result = TstTrue( parm );
        break;
    case 19:
        result = TstExist( parm );
        break;
    case 20:
        size = ExprSP->info.size;
        PopEntry();
        PushNum( size );
        break;
    case 21:
        TypeBase( ExprSP->th, th, NULL, NULL );
        PopEntry();
        PushType( th );
        break;
    case 22:
        GetMADTypeDefault( MTK_ADDRESS, &mti );
        size = (mti.b.bits - mti.a.seg.bits) / BITS_PER_BYTE;
        if( parm ) {
            size += sizeof( addr_seg );
            TypePointer( ExprSP->th, TM_FAR, size, th );
        } else {
            TypePointer( ExprSP->th, TM_NEAR, size, th );
        }
        PopEntry();
        PushType( th );
        break;
    case 23:
        result = UserType( th );
        if( result ) {
            PopEntry();
            PushType( th );
        }
        break;
    case 24:
        DoMakeComplex();
        break;
    case 25:
        DoStringConcat();
        break;
    case 26:
        DoLConvert();
        break;
    case 27:
        DoPlusScaled();
        break;
    case 28:
        DoMinusScaled();
        break;
    case 29:
        DoPoints( TI_KIND_EXTRACT( parm ) );
        break;
    case 30:
        info.kind = TK_POINTER;
        info.size = TI_SIZE_EXTRACT( parm );
        info.modifier = TI_MOD_EXTRACT( parm );
        FillInDefaults( &info );
        TypePointer( ExprSP->th, info.modifier, info.size, th );
        PopEntry();
        PushType( th );
        break;
    case 31:
        if( parm ) {
            /* file scope */
            if( ExprSP->flags & SF_NAME ) {
                ExprSP->v.li.file_scope = TRUE;
            } else {
                Error( ERR_LOC, LIT_ENG( ERR_WANT_NAME ) );
            }
        } else {
            /* in a namespace */
            DoScope();
        }
        break;
    }
    return( result );
}


struct internal_mod {
    mod_handle          mh;
};

OVL_EXTERN walk_result FindInternalMod( mod_handle mh, void *d )
{
    if( !IsInternalMod( mh ) ) return( WR_CONTINUE );
    ((struct internal_mod *)d)->mh = mh;
    return( WR_STOP );
}

static void BasicType( unsigned basic_type )
{
    struct internal_mod         mod_srch;
    struct imp_type_handle      *internal;
    dip_type_info               info;
    DIPHDL( type, th );

    WalkModList( NO_MOD, FindInternalMod, &mod_srch );
    internal = TypeCreate( th, mod_srch.mh );
    info.kind = TI_KIND_EXTRACT( basic_type );
    info.modifier = TI_MOD_EXTRACT( basic_type );
    info.size = TI_SIZE_EXTRACT( basic_type );
    FillInDefaults( &info );
    internal->t.k = info.kind;
    internal->t.m = info.modifier;
    internal->t.s = info.size;
    internal->ri = NULL;
    PushType( th );
}

static unsigned MechPush_n_Pop( unsigned select, unsigned parm )
{
    location_list       ll;
    dip_type_info       ti;
    unsigned            result;
    const static unsigned       TypeTbl[] = {
        TI_CREATE( TK_VOID,     TM_NONE,         0 ),
        TI_CREATE( TK_INTEGER,  TM_UNSIGNED,     1 ),
        TI_CREATE( TK_INTEGER,  TM_SIGNED,       1 ),
        TI_CREATE( TK_INTEGER,  TM_UNSIGNED,     2 ),
        TI_CREATE( TK_INTEGER,  TM_SIGNED,       2 ),
        TI_CREATE( TK_INTEGER,  TM_UNSIGNED,     4 ),
        TI_CREATE( TK_INTEGER,  TM_SIGNED,       4 ),
        TI_CREATE( TK_REAL,     TM_NONE,         4 ),
        TI_CREATE( TK_REAL,     TM_NONE,         8 ),
        TI_CREATE( TK_COMPLEX,  TM_NONE,         8 ),
        TI_CREATE( TK_COMPLEX,  TM_NONE,        16 ),
    };

    result = FALSE;
    switch( select ) {
    case 0:
        PushInt( parm );
        break;
    case 1:
        PushAddr( GetDotAddr() );
        break;
    case 2:
        ParseRegSet( TRUE, &ll, &ti );
        if( ti.size != 0 ) {
            if( ti.kind == TK_NONE ) Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
            PushLocation( &ll, &ti );
            result = TRUE;
        }
        break;
    case 3:
        if( CurrToken == T_INT_NUM ) {
            PushNum64( IntNumVal() );
            Scan();
            result = TRUE;
        } else if( CurrToken == T_REAL_NUM ) {
            PushRealNum( RealNumVal() );
            Scan();
            result = TRUE;
        } else {
            result = FALSE;
        }
        break;
    case 4:
        BasicType( TypeTbl[ parm ] );
        break;
    case 5:
        DupStack();
        break;
    case 6:
        PopEntry();
        break;
    case 7:
        PushString();
        break;
    case 8:
        /* here because old debuggers will always return FALSE */
        switch( parm & SSL_VERSION_MAJOR_MASK ) {
        case SSL_VERSION_MAJOR_CURR:
            break;
        default:
            return( FALSE );
        }
        #if SSL_VERSION_MINOR_CURR != 0
            if( (parm & SSL_VERSION_MINOR_MASK) > SS_MINOR_VERSION_CURR ) {
                return( FALSE );
            }
        #endif
        result = TRUE;
        break;
    case 9:
        BasicType( parm );
        break;
    }
    return( result );
}

static unsigned MechStack( unsigned select, unsigned parm )
{
    unsigned    result = 0;
    stack_entry *entry;
    sym_info    info;

    switch( select ) {
    case 0:
        SwapStack( parm );
        break;
    case 1:
        MoveSP( parm );
        break;
    case 2:
        entry = StkEntry( parm );
        LValue( entry );
        result = TypeInfoToClass( &entry->info ) & (BASE_TYPE | STK_UNSIGNED);
        break;
    case 3:
        ExprValue( StkEntry( parm ) );
        break;
    case 4:
        LValue( StkEntry( parm ) );
        break;
    case 5:
        RValue( StkEntry( parm ) );
        break;
    case 6:
        LRValue( StkEntry( parm ) );
        break;
    case 7:
        entry = StkEntry( parm );
        LValue( entry );
        result = TI_CREATE( entry->info.kind, TM_NONE, 0 );
        break;
    case 8:
        entry = StkEntry( parm );
        NameResolve( entry, FALSE );
        if( entry->flags & SF_SYM ) {
            SymInfo( entry->v.sh, entry->lc, &info );
            result = info.kind;
        } else {
            result = SK_NONE;
        }
        break;
    case 9:
        entry = StkEntry( parm );
        result = NameResolve( entry, FALSE );
        break;
    case 10:
        entry = StkEntry( parm );
        if( entry->flags & SF_NAME ) {
            result = 0;
        } else if( entry->flags & SF_SYM ) {
            result = 1;
        } else if( entry->flags & SF_LOCATION ) {
            result = 2;
        } else {
            result = 3;
        }
        break;
    }
    return( result );
}

static unsigned MechNum( unsigned select, unsigned parm )
{
    switch( select ) {
    case 0:
        Num = parm;
        break;
    case 1:
        Num += parm;
        break;
    case 2:
        PushInt( Num );
        break;
    case 3:
        /* need to check that top stack entry is an integer value here? */
        Num = U32FetchTrunc( ExprSP->v.uint );
        PopEntry();
        break;
    }
    return( 0 );
}

static unsigned MechBits( unsigned select, unsigned parm )
{
    unsigned result = 0;

    switch( select ) {
    case 0:
        Bits = parm;
        break;
    case 1:
        result = Bits;
        break;
    case 2:
        result = (Bits & parm) != 0;
        Bits |= parm;
        break;
    case 3:
        result = (Bits & parm) == 0;
        Bits &= parm;
        break;
    }
    return( result );
}

typedef struct cue_find {
    cue_handle          *best_ch;
    unsigned long       best_line;
    cue_fileid          id;
    const char          *name;
    unsigned            len;
    unsigned            ambig           : 1;
    unsigned            found_a_file    : 1;
    enum {
        CMP_NONE,
        CMP_INSENSITIVE,
        CMP_SENSITIVE
    }                   match;
} cue_find;

static walk_result FindCue( cue_handle *ch, void *d )
{
    cue_find    *cd = d;
    char        file[FILENAME_MAX];
    unsigned    len;
    unsigned    match;


    len = CueFile( ch, file, sizeof( file ) );
    if( len < cd->len ) return( WR_CONTINUE );
    if( memcmp( &file[len - cd->len], cd->name, cd->len ) == 0 ) {
        match = CMP_SENSITIVE;
    } else if( memicmp( &file[len - cd->len], cd->name, cd->len ) == 0 ) {
        match = CMP_INSENSITIVE;
    } else {
        return( WR_CONTINUE );
    }
    if( match > cd->match ) {
        cd->match = match;
        cd->id = CueFileId( ch );
        cd->ambig = FALSE;
    } else if( match == cd->match ) {
        cd->ambig = TRUE;
    }
    return( WR_CONTINUE );
}

static walk_result FindModCue( mod_handle mod, void *d )
{
    DIPHDL( cue, ch );
    cue_find            *fd = d;
    unsigned long       curr_line;

    fd->id = 0;
    fd->ambig = FALSE;
    fd->match = CMP_NONE;
    if( mod != NO_MOD && fd->len != 0 ) {
        WalkFileList( mod, FindCue, fd );
        if( fd->id == 0 ) return( WR_CONTINUE );
        if( fd->ambig ) {
            Error( ERR_NONE, LIT_ENG( ERR_AMBIG_SRC_FILE ), fd->name, fd->len );
        }
    }
    fd->found_a_file = TRUE;
    switch( LineCue( mod, fd->id, CurrGet.li.name.len, 0, ch ) ) {
    case SR_EXACT:
        HDLAssign( cue, fd->best_ch, ch );
        fd->best_line = CurrGet.li.name.len;
        return( WR_STOP );
    case SR_CLOSEST:
        curr_line = CueLine( ch );
        if( curr_line < CurrGet.li.name.len && curr_line > fd->best_line ) {
            HDLAssign( cue, fd->best_ch, ch );
            fd->best_line = CurrGet.li.name.len;
        }
        break;
    case SR_FAIL:
        return( WR_FAIL );
    }
    return( WR_CONTINUE );
}

static search_result FindACue( cue_handle *ch )
{
    cue_find    data;

    data.found_a_file = FALSE;
    data.best_line = 0;
    data.best_ch = ch;
    data.name = CurrGet.li.source.start;
    data.len = CurrGet.li.source.len;

    if( CurrGet.multi_module ) {
        if( data.len == 0 ) {
            Error( ERR_NONE, LIT_ENG( ERR_WANT_MODULE ) );
        }
        if( WalkModList( CurrGet.li.mod, FindModCue, &data ) == WR_FAIL ) {
            return( SR_FAIL );
        }
    } else {
        if( CurrGet.li.mod == NO_MOD ) CurrGet.li.mod = CodeAddrMod;
        if( FindModCue( CurrGet.li.mod, &data ) == WR_FAIL ) {
            return( SR_FAIL );
        }
    }
    if( !data.found_a_file ) {
        Error( ERR_NONE, LIT_ENG( ERR_NO_SRC_FILE ), data.name, data.len );
    }
    if( data.best_line == 0 ) return( SR_NONE );
    if( data.best_line == CurrGet.li.name.len ) return( SR_EXACT );
    return( SR_CLOSEST );
}

static bool ClosestLineOk = FALSE;

bool SemAllowClosestLine( bool ok )
{
    bool old = ClosestLineOk;
    ClosestLineOk = ok;
    return( old );
}

static unsigned MechGet( unsigned select, unsigned parm )
{
    unsigned    result;
    DIPHDL( cue, ch );
    sym_list    *sym;
    address     addr;
    unsigned    old;
    const char  *save_scan;
    const char  *mod_name;
    unsigned    mod_len;
    unsigned    mod_spec_token;

    result = FALSE;
    switch( select ) {
    case 0: /* init */
        memset( &CurrGet, 0, sizeof( CurrGet ) );
        CurrGet.li.mod = NO_MOD;
        if( _IsOn( SW_CASE_SENSITIVE ) ) {
            CurrGet.li.case_sensitive = TRUE;
        } else {
            CurrGet.li.case_sensitive = FALSE;
        }
        break;
    case 1: /* fini */
        switch( CurrGet.kind ) {
        case GET_NAME:
            PushName( &CurrGet.li );
            break;
        case GET_OPERATOR:
            CurrGet.li.type = ST_OPERATOR;
            PushName( &CurrGet.li );
            break;
        case GET_LNUM:
            switch( FindACue( ch ) ) {
            case SR_EXACT:
                break;
            case SR_CLOSEST:
                if( ClosestLineOk ) break;
            default:
                Error( ERR_NONE, LIT_ENG( ERR_NO_CODE ), CurrGet.li.name.len );
            }
            PushAddr( CueAddr( ch ) );
            break;
        }
        break;
    case 2: /* mod curr */
    case 3: /* mod name lookup */
        //NYI: temporary gunk
        CurrGet.multi_module = TRUE;
        CurrGet.li.mod = NO_MOD;
        save_scan = ScanPos();
        ReScan( "@" );
        mod_spec_token = CurrToken;
        ReScan( save_scan );
        if( CurrToken == T_NAME ) {
            mod_name = NamePos();
            mod_len  = NameLen();
            Scan();
        } else {
            mod_name = NULL;
            mod_len = 0;
        }
        if( CurrToken == mod_spec_token ) {
            if( select == 2 ) {
                CurrGet.li.mod = ImagePrimary()->dip_handle;
                if( CurrGet.li.mod == NO_MOD ) {
                    CurrGet.li.mod = ILL_MOD; /* cause lookup to fail */
                }
            } else {
                CurrGet.li.mod = LookupImageName( CurrGet.li.name.start, CurrGet.li.name.len );
                if( CurrGet.li.mod == NO_MOD ) {
                    #define ANY_IMAGE_NAME      "_anyimage"
                    #define ANY_IMAGE_NAME_LEN  (sizeof(ANY_IMAGE_NAME)-1)
                    if( CurrGet.li.name.len != ANY_IMAGE_NAME_LEN
                      || memicmp( CurrGet.li.name.start, ANY_IMAGE_NAME, ANY_IMAGE_NAME_LEN ) != 0 ) {
                        Error( ERR_NONE, LIT_ENG( ERR_NO_IMAGE ), CurrGet.li.name.start,
                                              CurrGet.li.name.len );
                    } else {
                        CurrGet.any_image = TRUE;
                    }
                }
            }
            CurrGet.li.name.start = mod_name;
            CurrGet.li.name.len   = mod_len;
            Scan();
        } else {
            ReScan( save_scan );
        }
        if( CurrGet.li.name.start != NULL ) {
            CurrGet.li.mod = LookupModName( CurrGet.li.mod, CurrGet.li.name.start, CurrGet.li.name.len );
            if( CurrGet.li.mod == NO_MOD ) {
                Error( ERR_NONE, LIT_ENG( ERR_NO_MODULE ), CurrGet.li.name.start,
                                      CurrGet.li.name.len );
            }
            CurrGet.multi_module = FALSE;
        } else if( !CurrGet.any_image && CurrGet.li.mod == NO_MOD ) {
            CurrGet.li.mod = CodeAddrMod;
            CurrGet.multi_module = FALSE;
        }
        break;
    case 4: /* file scope */
        CurrGet.li.file_scope = TRUE;
        break;
    case 5: /* given scope */
        CurrGet.li.file_scope = FALSE;
        break;
    case 6: /* scope lookup */
        CurrGet.li.scope.start = CurrGet.li.name.start;
        CurrGet.li.scope.len   = CurrGet.li.name.len;
        break;
    case 7: /* get name >>bool */
        if( CurrToken == T_NAME ) {
            CurrGet.kind = GET_NAME;
            CurrGet.li.name.start = NamePos();
            CurrGet.li.name.len   = NameLen();
            Scan();
            result = TRUE;
        }
        break;
    case 8: /* get operator name */
        CurrGet.kind = GET_OPERATOR;
        CurrGet.li.name.start = NamePos();
        CurrGet.li.name.len   = NameLen();
        Scan();
        break;
    case 9: /* get line number >>bool */
        if( CurrToken == T_LEFT_BRACE ) {
            size_t  len;
            /* Get a specfic file name for the module */
            ScanQuote( &CurrGet.li.source.start, &len );
            CurrGet.li.source.len = len;
        }
        if( CurrToken == T_INT_NUM ) {
            unsigned_64         tmp;

            result = TRUE;
            CurrGet.kind = GET_LNUM;
            old = SetCurrRadix( 10 );
            tmp = IntNumVal();
            CurrGet.li.name.len = U32FetchTrunc( tmp );
            Scan();
            SetCurrRadix( old );
        }
        break;
    case 10: /* GetDtorName >>bool */
        if( CurrToken == T_NAME ) {
            CurrGet.kind = GET_NAME;
            CurrGet.li.name.start = NamePos();
            CurrGet.li.name.len   = NameLen();
            CurrGet.li.type = ST_DESTRUCTOR;
            addr = Context.execution;
            sym = LookupSymList( SS_SCOPED, &addr, FALSE, &CurrGet.li );
            if( sym != NULL ) {
                PurgeSymHandles();
                Scan();
                result = TRUE;
            } else {
                CurrGet.li.type = ST_NONE;
            }
        }
        break;
    case 11: /* GetSetNameType(symbol_type) */
        CurrGet.li.type = parm;
        break;
    case 12: /* GetQueryName >>bool */
        CurrGet.kind = GET_NAME;
        addr = Context.execution;
        sym = LookupSymList( SS_SCOPED, &addr, FALSE, &CurrGet.li );
        if( sym != NULL ) {
            PurgeSymHandles();
            result = TRUE;
        } else {
            CurrGet.li.type = ST_NONE;
        }
        break;
    case 13: /* GetAddScope */
        if( CurrGet.li.scope.len == 0 ) {
            CurrGet.li.scope = CurrGet.li.name;
        } else {
            CurrGet.li.scope.len = (CurrGet.li.name.start-CurrGet.li.scope.start)
                                        + CurrGet.li.name.len;
        }
        break;
    }
    return( result );
}


unsigned SSLSemantic( int action, unsigned parm )
{
    unsigned    result = 0;
    unsigned    select;

    select = action & SEM_SELECTOR;
    switch( action & SEM_MECHANISM ) {
    case SEM_MISC:
        result = MechMisc( select, parm );
        break;
    case SEM_DO:
        result = MechDo( select, parm );
        break;
    case SEM_PUSH_N_POP:
        result = MechPush_n_Pop( select, parm );
        break;
    case SEM_STACK:
        result = MechStack( select, parm );
        break;
    case SEM_NUM:
        result = MechNum( select, parm );
        break;
    case SEM_BITS:
        result = MechBits( select, parm );
        break;
    case SEM_GET:
        result = MechGet( select, parm );
        break;
    }
    return( result );
}


int SSLError( unsigned class, unsigned error )
{
    switch( class ) {
    case 1: /* syntax */
        Recog( error ); /* cause error */
        break;
    case 2: /* error stream */
        switch( error ) {
        case 0:
            Error( ERR_NONE, LIT_ENG( ERR_DUPLICATE_TYPE_SPEC ) );
            break;
        case 1:
            Error( ERR_NONE, LIT_ENG( ERR_ILL_TYPE ) );
            break;
        case 2:
            Error( ERR_NONE, LIT_ENG( ERR_ILLEGAL_TYPE_SPEC ) );
            break;
        case 3:
            Error( ERR_LOC, LIT_ENG( ERR_WANT_REG_NAME ) );
            break;
        case 4:
            Error( ERR_LOC, LIT_ENG( ERR_WANT_OPERAND ) );
            break;
        case 5:
            Error( ERR_LOC, LIT_ENG( ERR_WANT_NAME ) );
            break;
        case 6:
            Error( ERR_NONE, LIT_ENG( ERR_BAD_PARSE_VERSION ) );
            break;
        default:
            Error( ERR_INTERNAL, LIT_ENG( ERR_PARSE_FILE ) );
            break;
        }
        break;
    case 3: /* stack overflow */
        Error( ERR_LOC, LIT_ENG( ERR_EXPR_STACK_OVER ) );
        break;
    case 4: /* kill (error in SSL file) */
        Error( ERR_INTERNAL, LIT_ENG( ERR_PARSE_FILE ) );
        break;
    }
    return( 1 );
}


void SSLOutToken( unsigned token )
{
    token = token;
}

unsigned SSLNextToken( void )
{
    Scan();
    return( CurrToken );
}

unsigned SSLCurrToken( void )
{
    return( CurrToken );
}
