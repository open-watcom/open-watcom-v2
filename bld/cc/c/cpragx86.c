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
* Description:  Pragmas specific to x86 targets.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "asmstmt.h"

static  hw_reg_set      AsmRegsSaved = HW_D( HW_FULL );
static  int             AsmFuncNum;
static  aux_info        AuxInfo;

#if _CPU == 386
static  hw_reg_set      STOSBParms[] = {
    HW_D( HW_EAX ), HW_D( HW_EDX ), HW_D( HW_ECX ),
    HW_D( HW_EMPTY )
};
#endif

static struct {
    unsigned    f_near : 1;
    unsigned    f_routine_pops : 1;
    unsigned    f_caller_return : 1;
    unsigned    f_8087_returns : 1;
} AuxInfoFlg;

static void pragmaAuxInfoInit( void )
/***********************************/
{
#if _CPU == 386
    HW_CTurnOff( AsmRegsSaved, HW_EAX );
    HW_CTurnOff( AsmRegsSaved, HW_EBX );
    HW_CTurnOff( AsmRegsSaved, HW_ECX );
    HW_CTurnOff( AsmRegsSaved, HW_EDX );
    HW_CTurnOff( AsmRegsSaved, HW_ESI );
    HW_CTurnOff( AsmRegsSaved, HW_EDI );
#else
    HW_CTurnOff( AsmRegsSaved, HW_ABCD );
    HW_CTurnOff( AsmRegsSaved, HW_SI );
    HW_CTurnOff( AsmRegsSaved, HW_DI );
    HW_CTurnOff( AsmRegsSaved, HW_ES );
#endif

#if _CPU == 386
    /* these are internal, and will never be pointed to by
       an aux_entry, so we don't have to worry about them
       or their fields being freed */

    STOSBInfo = WatcallInfo;
    STOSBInfo.cclass = NO_FLOAT_REG_RETURNS |
                       NO_STRUCT_REG_RETURNS |
                       SPECIAL_STRUCT_RETURN;
    STOSBInfo.parms = STOSBParms;
    STOSBInfo.objname = "*";
#endif
}

void PragmaInit( void )
/*********************/
{
    int         cpu;
    int         fpu;

    pragmaAuxInfoInit();

    AsmFuncNum = 0;
    switch( GET_CPU( ProcRevision ) ) {
    case CPU_86:    cpu = 0; break;
    case CPU_186:   cpu = 1; break;
    case CPU_286:   cpu = 2; break;
    case CPU_386:   cpu = 3; break;
    case CPU_486:   cpu = 4; break;
    case CPU_586:   cpu = 5; break;
    case CPU_686:   cpu = 6; break;
#if _CPU == 8086
    default:        cpu = 0; break;
#else
    default:        cpu = 3; break;
#endif
    }
    switch( GET_FPU_LEVEL( ProcRevision ) ) {
    case FPU_NONE:  fpu = 0; break;
    case FPU_87:    fpu = 1; break;
//    case FPU_287:   fpu = 2; break;
    case FPU_387:   fpu = 3; break;
    case FPU_586:   fpu = 3; break;
    case FPU_686:   fpu = 3; break;
#if _CPU == 8086
    default:        fpu = 1; break;
#else
    default:        fpu = 3; break;
#endif
    }

#if _CPU == 8086
    AsmInit( 0, cpu, fpu, GET_FPU_EMU( ProcRevision ) );
#else
    AsmInit( 1, cpu, fpu, FALSE );
#endif
}


void PragmaFini( void )
/*********************/
{
}


static void InitAuxInfo( void )
/*****************************/
{
    CurrAlias   = NULL;
    CurrInfo    = NULL;
    CurrEntry   = NULL;

    memset( &AuxInfo, 0, sizeof( AuxInfo ) );

    AuxInfoFlg.f_near           = 0;
    AuxInfoFlg.f_routine_pops   = 0;
    AuxInfoFlg.f_caller_return  = 0;
    AuxInfoFlg.f_8087_returns   = 0;
}

static void CopyAuxInfo( void )
/*****************************/
{
    hw_reg_set      default_flt_n_seg;
    hw_reg_set      flt_n_seg;

    if( CurrEntry == NULL ) {
        // Redefining a built-in calling convention
    } else {
        CurrInfo = (aux_info *)CMemAlloc( sizeof( aux_info ) );
        *CurrInfo = *CurrAlias;
    }
    if( AuxInfo.code != NULL ) {
        CurrInfo->code = AuxInfo.code;
    }
    if( AuxInfoFlg.f_near ) {
        CurrInfo->cclass &= ~FAR_CALL;
    }
    if( AuxInfoFlg.f_routine_pops ) {
        CurrInfo->cclass &= ~CALLER_POPS;
    }
    if( AuxInfoFlg.f_caller_return ) {
        CurrInfo->cclass &= ~ROUTINE_RETURN;
    }
    if( AuxInfoFlg.f_8087_returns ) {
        CurrInfo->cclass &= ~NO_8087_RETURNS;
    }
    CurrInfo->cclass |= AuxInfo.cclass;
    CurrInfo->flags |= AuxInfo.flags;
    if( AuxInfo.objname != NULL )
        CurrInfo->objname = AuxInfo.objname;
    if( AuxInfo.cclass & SPECIAL_RETURN )
        CurrInfo->returns = AuxInfo.returns;
    if( AuxInfo.cclass & SPECIAL_STRUCT_RETURN )
        CurrInfo->streturn = AuxInfo.streturn;
    if( AuxInfo.parms != NULL )
        CurrInfo->parms = AuxInfo.parms;

    if( !HW_CEqual( AuxInfo.save, HW_EMPTY ) ) {
        HW_CTurnOn( CurrInfo->save, HW_FULL );
        if( !( AuxInfo.cclass & MODIFY_EXACT ) && !CompFlags.save_restore_segregs ) {
            HW_Asgn( default_flt_n_seg, WatcallInfo.save );
            HW_CAsgn( flt_n_seg, HW_FLTS );
            HW_CTurnOn( flt_n_seg, HW_SEGS );
            HW_TurnOff( CurrInfo->save, flt_n_seg );
            HW_OnlyOn( default_flt_n_seg, flt_n_seg );
            HW_TurnOn( CurrInfo->save, default_flt_n_seg );
        }
        HW_TurnOff( CurrInfo->save, AuxInfo.save );
    }
}

static void AdvanceToken( void )
/******************************/
{
    CMemFree( SavedId );
    SavedId = NULL;
    CurToken = LAToken;
}

static int GetAliasInfo( void )
/*****************************/
{
    int     isfar16;

    if( CurToken != T_LEFT_PAREN )          // #pragma aux symbol .....
        return( IS_ID_OR_KEYWORD( CurToken ) );
    NextToken();
    if( !IS_ID_OR_KEYWORD( CurToken ) )     // error
        return( 0 );
    LookAhead();
    if( LAToken == T_RIGHT_PAREN ) {        // #pragma aux (alias) symbol .....
        PragCurrAlias( SavedId );
        AdvanceToken();
        NextToken();
        return( IS_ID_OR_KEYWORD( CurToken ) );
    } else if( LAToken == T_COMMA ) {       // #pragma aux (symbol, alias)
        HashValue = SavedHash;
        SetCurrInfo( SavedId );
        AdvanceToken();
        NextToken();
        if( !IS_ID_OR_KEYWORD( CurToken ) ) // error
            return( 0 );
        isfar16 = PragRecog( "far16" );
        if( IS_ID_OR_KEYWORD( CurToken ) ) {
            PragCurrAlias( Buffer );
            NextToken();
        }
        if( CurToken == T_RIGHT_PAREN )
            NextToken();
        if( isfar16 )
            AuxInfo.flags |= AUX_FLAG_FAR16;
        CopyAuxInfo();
        PragEnding();
        return( 0 ); /* process no more! */
    } else {                                // error
        AdvanceToken();
        return( 0 ); // shut up the compiler
    }
}

typedef enum {
    FIXWORD_NONE,
    FIXWORD_FLOAT,
    FIXWORD_SEGMENT,
    FIXWORD_OFFSET,
    FIXWORD_RELOFF
} fix_words;

local fix_words FixupKeyword( void )
/**********************************/
{
    if( CurToken == T_FLOAT ) {
        NextToken();                    /* 20-jun-92 */
        return( FIXWORD_FLOAT );
    }
    if( PragRecog( "seg" ) )    return( FIXWORD_SEGMENT );
    if( PragRecog( "offset" ) ) return( FIXWORD_OFFSET );
    if( PragRecog( "reloff" ) ) return( FIXWORD_RELOFF );
    return( FIXWORD_NONE );
}


void *AsmQuerySymbol( char *name )
/********************************/
{
    return( SymLook( CalcHash( name, strlen( name ) ), name ) );
}

enum sym_state AsmQueryState( void *handle )
/******************************************/
{
    SYM_HANDLE  sym_handle = (SYM_HANDLE)handle;
    SYM_ENTRY   sym;

    if( sym_handle == 0 )
        return( SYM_UNDEFINED );
    SymGet( &sym, sym_handle );
    if( !(sym.flags & SYM_REFERENCED) ) {
        sym.flags |= SYM_REFERENCED;
        SymReplace( &sym, sym_handle );
    }
    switch( sym.attribs.stg_class ) {
    case SC_AUTO:
    case SC_REGISTER:
        return( SYM_STACK );
    }
    return( SYM_EXTERNAL );
}

#if _CPU == 8086
    #define SYM_INT     SYM_INT2
    #define SYM_FFAR    SYM_FAR2
    #define SYM_FNEAR   SYM_NEAR2
    #define SYM_DFAR    SYM_INT4
    #define SYM_DNEAR   SYM_INT2
#else
    #define SYM_INT     SYM_INT4
    #define SYM_FFAR    SYM_FAR4
    #define SYM_FNEAR   SYM_NEAR2
    #define SYM_DFAR    SYM_INT6
    #define SYM_DNEAR   SYM_INT4
#endif

static int AsmCodePtrType( type_modifiers flags )
/***********************************************/
{
    if( flags & FLAG_FAR ) {
        return( SYM_FFAR );
    } else if( flags & FLAG_NEAR ) {
        return( SYM_FNEAR );
    } else if( TargetSwitches & BIG_CODE ) {
        return( SYM_FFAR );
    } else {
        return( SYM_FNEAR );
    }
}


static int AsmPtrType( TYPEPTR typ, type_modifiers flags )
/********************************************************/
{

    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_FUNCTION ) {
        return( AsmCodePtrType( flags ) );
    } else if( flags & (FLAG_FAR|FLAG_HUGE) ) {
        return( SYM_DFAR );
    } else if( flags & FLAG_NEAR ) {
        return( SYM_DNEAR );
    } else if( TargetSwitches & BIG_DATA ) {
        return( SYM_DFAR );
    } else {
        return( SYM_DNEAR );
    }
}

/* matches enum DataType in ctypes.h */
static enum sym_type AsmDataType[] = {
    #define pick1(enum,cgtype,x86asmtype,name,size) x86asmtype,
    #include "cdatatyp.h"
    #undef  pick1
};

local int AsmType( TYPEPTR typ, type_modifiers flags )
/****************************************************/
{
    SKIP_TYPEDEFS( typ );
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
        return( SYM_INT1 );
    case TYPE_ARRAY:
        return( AsmType( typ->object, flags ) );
    case TYPE_FIELD:
    case TYPE_UFIELD:
        return( AsmDataType[typ->u.f.field_type] );
    case TYPE_FUNCTION:
        return( AsmCodePtrType( flags ) );
    case TYPE_POINTER:
        return( AsmPtrType( typ->object, typ->u.p.decl_flags ) );
    case TYPE_ENUM:
        typ = typ->object;
        /* fall through */
    default:
        return( AsmDataType[typ->decl_type] );
    }
}

enum sym_type AsmQueryType( void *handle )
/****************************************/
{
    SYM_HANDLE  sym_handle = (SYM_HANDLE)handle;
    SYM_ENTRY   sym;

    if( sym_handle == 0 )
        return( SYM_INT1 );
    SymGet( &sym, sym_handle );
    return( AsmType( sym.sym_type, sym.mods ) );
}

static bool InsertFixups( unsigned char *buff, byte_seq_len len, byte_seq **code )
/********************************************************************************/
{
                        /* additional slop in buffer to simplify the code */
    unsigned char       temp[MAXIMUM_BYTESEQ + 1 + 2 + sizeof( BYTE_SEQ_SYM ) + sizeof( BYTE_SEQ_OFF )];
    struct asmfixup     *fix;
    struct asmfixup     *head;
    struct asmfixup     *chk;
    struct asmfixup     *next;
    struct asmfixup     **owner;
    unsigned char       *dst;
    unsigned char       *src;
    unsigned char       *end;
    byte_seq            *seq;
    bool                perform_fixups;
    unsigned char       cg_fix;
    SYM_HANDLE          sym_handle;
    SYM_ENTRY           sym;
    char                *name;
    unsigned            skip;
    int                 mutate_to_segment;
    bool                uses_auto;
#if _CPU == 8086
    int                 fixup_padding;
#endif

    sym_handle = 0;
    uses_auto = FALSE;
    perform_fixups = FALSE;
    head = FixupHead;
    if( head != NULL ) {
        FixupHead = NULL;
        /* sort the fixup list in increasing fixup_loc's */
        for( fix = head; fix != NULL; fix = next ) {
            owner = &FixupHead;
            for( ;; ) {
                chk = *owner;
                if( chk == NULL ) break;
                if( chk->fixup_loc > fix->fixup_loc ) break;
                owner = &chk->next;
            }
            next = fix->next;
            fix->next = *owner;
            *owner = fix;
        }
        dst = temp;
        src = buff;
        end = src + len;
        fix = FixupHead;
        owner = &FixupHead;
        /* insert fixup escape sequences */
        while( src < end ) {
            if( fix != NULL && fix->fixup_loc == (src - buff) ) {
                name = fix->name;
                if( name != NULL ) {
                    sym_handle = SymLook( CalcHash( name, strlen( name ) ), name );
                    if( sym_handle == 0 ) {
                        CErr2p( ERR_UNDECLARED_SYM, name );
                        return( 0 );
                    }
                    SymGet( &sym, sym_handle );
                    sym.flags |= SYM_REFERENCED | SYM_ADDR_TAKEN;
                    switch( sym.attribs.stg_class ) {
                    case SC_REGISTER:
                    case SC_AUTO:
                        sym.flags |= SYM_USED_IN_PRAGMA;
                        CurFuncNode->op.u2.func.flags &= ~FUNC_OK_TO_INLINE;
                        uses_auto = TRUE;
                        break;
                    }
                    SymReplace( &sym, sym_handle );
                }
                /* insert fixup information */
                skip = 0;
                *dst++ = FLOATING_FIXUP_BYTE;
                mutate_to_segment = 0;
                cg_fix = 0;
#if _CPU == 8086
                fixup_padding = 0;
#endif
                switch( fix->fixup_type ) {
                case FIX_FPPATCH:
                    *dst++ = fix->offset;
                    break;
                case FIX_SEG:
                    if( name == NULL ) {
                        // special case for floating point fixup
                        if( ( src[0] == 0x90 ) && ( src[1] == 0x9B ) ) {
                           // inline assembler FWAIT instruction 0x90, 0x9b
                            *dst++ = FIX_FPP_WAIT;
                        } else if( src[0] == 0x9b && (src[1] & 0xd8) == 0xd8 ) {
                           // FWAIT as first byte and FPU instruction opcode as second byte
                            *dst++ = FIX_FPP_NORMAL;
                        } else if( src[0] == 0x9b && (src[2] & 0xd8) == 0xd8 ) {
                           // FWAIT as first byte and FPU instruction opcode as third byte
                           // second byte should be segment override prefix
                            switch( src[1] ) {
                            case PREFIX_ES: *dst++ = FIX_FPP_ES;    break;
                            case PREFIX_CS: *dst++ = FIX_FPP_CS;    break;
                            case PREFIX_SS: *dst++ = FIX_FPP_SS;    break;
                            case PREFIX_DS: *dst++ = FIX_FPP_DS;    break;
                            case PREFIX_GS: *dst++ = FIX_FPP_GS;    break;
                            case PREFIX_FS: *dst++ = FIX_FPP_FS;    break;
                            default: --dst; break;  // skip FP patch
                            }
                        } else {
                            // skip FP patch
                            --dst;
                        }
                    } else {
                        skip = 2;
                        cg_fix = FIX_SYM_SEGMENT;
                    }
                    break;
                case FIX_RELOFF16:
                    skip = 2;
                    cg_fix = FIX_SYM_RELOFF;
                    break;
                case FIX_RELOFF32:
                    skip = 4;
                    cg_fix = FIX_SYM_RELOFF;
#if _CPU == 8086
                    fixup_padding = 1;
#endif
                    break;
                case FIX_PTR16:
                    mutate_to_segment = 1;
                    /* fall through */
                case FIX_OFF16:
                    skip = 2;
                    cg_fix = FIX_SYM_OFFSET;
                    break;
                case FIX_PTR32:
                    mutate_to_segment = 1;
                    /* fall through */
                case FIX_OFF32:
                    skip = 4;
                    cg_fix = FIX_SYM_OFFSET;
#if _CPU == 8086
                    fixup_padding = 1;
#endif
                    break;
                default:
                    CErr2p( ERR_UNREACHABLE_SYM, name );
                    break;
                }
                if( skip != 0 ) {
                    *dst++ = cg_fix;
                    *(BYTE_SEQ_SYM *)dst = sym_handle;
                    dst += sizeof( BYTE_SEQ_SYM );
                    *((BYTE_SEQ_OFF *)dst) = fix->offset;
                    dst += sizeof( BYTE_SEQ_OFF );
                    src += skip;
                }
#if _CPU == 8086
                if( fixup_padding ) {
                    // add offset fixup padding to 32-bit
                    // cg create only 16-bit offset fixup
                    *dst++ = 0;
                    *dst++ = 0;
                    //
                }
#endif
                if( mutate_to_segment ) {
                    /*
                        Since the CG escape sequences don't allow for
                        FAR pointer fixups, we have to split them into two.
                        This is done by doing the offset fixup first, then
                        mutating the fixup structure to look like a segment
                        fixup one near pointer size later.
                    */
                    fix->fixup_type = FIX_SEG;
                    fix->fixup_loc += skip;
                    fix->offset = 0;
                } else {
                    head = fix;
                    fix = fix->next;
                    if( head->external ) {
                        *owner = fix;
                        if( head->name != NULL ) {
                            CMemFree( head->name );
                        }
                        CMemFree( head );
                    } else {
                        owner = &head->next;
                    }
                }
            } else {
                if( *src == FLOATING_FIXUP_BYTE ) {
                    *dst++ = FLOATING_FIXUP_BYTE;
                }
                *dst++ = *src++;
            }
            if( dst > &temp[MAXIMUM_BYTESEQ] ) {
                CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                return( 0 );
            }
        }
        buff = temp;
        len = dst - temp;
        perform_fixups = TRUE;
    }
    seq = (byte_seq *)CMemAlloc( offsetof( byte_seq, data ) + len );
    seq->relocs = perform_fixups;
    seq->length = len;
    memcpy( &seq->data[0], buff, len );
    *code = seq;
    return( uses_auto );
}


local void AddAFix( unsigned loc, char *name, unsigned type, unsigned off )
/*************************************************************************/
{
    struct asmfixup     *fix;

    fix = (struct asmfixup *)CMemAlloc( sizeof( *fix ) );
    fix->external = 1;
    fix->fixup_loc = loc;
    fix->name = name;
    fix->offset = off;
    fix->fixup_type = type;
    fix->next = FixupHead;
    FixupHead = fix;
}

local void FreeAsmFixups( void )
/******************************/
{
    struct asmfixup     *fix;

    for( ; (fix = FixupHead) != NULL; ) {
        FixupHead = fix->next;
        CMemFree( fix );
    }
}


void AsmSysLine( char *buff )
/***************************/
{
#if _CPU == 8086
    AsmLine( buff, GET_FPU_EMU( ProcRevision ) );
#else
    AsmLine( buff, FALSE );
#endif
}

local bool GetByteSeq( byte_seq **code )
/**************************************/
{
    unsigned char       buff[MAXIMUM_BYTESEQ + 32];
    char                *name;
    unsigned            offset;
    fix_words           fixword;
    bool                uses_auto;
    char                too_many_bytes;
#if _CPU == 8086
    bool                use_fpu_emu = FALSE;
#endif

    AsmSysInit( buff );
    PPCTL_ENABLE_MACROS();
    NextToken();
    too_many_bytes = 0;
    uses_auto = FALSE;
    offset = 0;
    name = NULL;
    for( ;; ) {
        if( CurToken == T_STRING ) {    /* 06-sep-91 */
#if _CPU == 8086
            AsmLine( Buffer, use_fpu_emu );
            use_fpu_emu = FALSE;
#else
            AsmLine( Buffer, FALSE );
#endif
            NextToken();
            if( CurToken == T_COMMA ) {
                NextToken();
            }
        } else if( CurToken == T_CONSTANT ) {
#if _CPU == 8086
            if( use_fpu_emu ) {
                AddAFix( AsmCodeAddress, NULL, FIX_SEG, 0 );
                use_fpu_emu = FALSE;
            }
#endif
            AsmCodeBuffer[AsmCodeAddress++] = Constant;
            NextToken();
        } else {
#if _CPU == 8086
            use_fpu_emu = FALSE;
#endif
            fixword = FixupKeyword();
            if( fixword == FIXWORD_NONE )
                break;
            if( fixword == FIXWORD_FLOAT ) {
#if _CPU == 8086
                if( GET_FPU_EMU( ProcRevision ) ) {
                    use_fpu_emu = TRUE;
                }
#endif
            } else { /* seg or offset */
                if( !IS_ID_OR_KEYWORD( CurToken ) ) {
                    CErr1( ERR_EXPECTING_ID );
                } else {
                    name = CStrSave( Buffer );
                    NextToken();
                    if( CurToken == T_PLUS ) {
                        NextToken();
                        if( CurToken == T_CONSTANT ) {
                            offset = Constant;
                            NextToken();
                        }
                    } else if( CurToken == T_MINUS ) {
                        NextToken();
                        if( CurToken == T_CONSTANT ) {
                            offset = -Constant;
                            NextToken();
                        }
                    }
                }
                switch( fixword ) {
                case FIXWORD_RELOFF:
#if _CPU == 8086
                    AddAFix( AsmCodeAddress, name, FIX_RELOFF16, offset );
                    AsmCodeAddress += 2;
#else
                    AddAFix( AsmCodeAddress, name, FIX_RELOFF32, offset );
                    AsmCodeAddress += 4;
#endif
                    break;
                case FIXWORD_OFFSET:
#if _CPU == 8086
                    AddAFix( AsmCodeAddress, name, FIX_OFF16, offset );
                    AsmCodeAddress += 2;
#else
                    AddAFix( AsmCodeAddress, name, FIX_OFF32, offset );
                    AsmCodeAddress += 4;
#endif
                    break;
                case FIXWORD_SEGMENT:
                    AddAFix( AsmCodeAddress, name, FIX_SEG, 0 );
                    AsmCodeAddress += 2;
                    break;
                }
            }
        }
        if( AsmCodeAddress > MAXIMUM_BYTESEQ ) {
            if( ! too_many_bytes ) {
                CErr1( ERR_TOO_MANY_BYTES_IN_PRAGMA );
                too_many_bytes = 1;
            }
            AsmCodeAddress = 0;          // reset index to we don't overrun buffer
        }
    }
    PPCTL_DISABLE_MACROS();
    if( too_many_bytes ) {
        uses_auto = FALSE;
    } else {
        uses_auto = InsertFixups( buff, AsmCodeAddress, code );
    }
    FreeAsmFixups();
    AsmSysFini();
    return( uses_auto );
}


hw_reg_set PragRegName( char *str )
/*********************************/
{
    int         index;
    char        *p;
    hw_reg_set  name;

    if( *str == '\0' ) {
        HW_CAsgn( name, HW_EMPTY );
        return( name );
    }
    if( *str == '_' ) {
        ++str;
        if( *str == '_' ) {
            ++str;
        }
    }
    index = 0;
    p = Registers;
    while( *p != '\0' ) {
        if( stricmp( p, str ) == 0 )
            return( RegBits[index] );
        index++;
        while( *p++ != '\0' ) {
            ;
        }
    }
    if( strcmp( str, "8087" ) == 0 ) {
        HW_CAsgn( name, HW_FLTS );
    } else {
        CErr2p( ERR_BAD_REGISTER_NAME, str );
        HW_CAsgn( name, HW_EMPTY );
    }
    return( name );
}



local void GetParmInfo( void )
/****************************/
{
    struct {
        unsigned f_pop           : 1;
        unsigned f_reverse       : 1;
        unsigned f_loadds        : 1;
        unsigned f_nomemory      : 1;
        unsigned f_list          : 1;
    } have;

    have.f_pop           = 0;
    have.f_reverse       = 0;
    have.f_loadds        = 0;
    have.f_nomemory      = 0;
    have.f_list          = 0;
    for( ;; ) {
        if( !have.f_pop && PragRecog( "caller" ) ) {
            AuxInfo.cclass |= CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_pop && PragRecog( "routine" ) ) {
            AuxInfo.cclass &= ~ CALLER_POPS;
            AuxInfoFlg.f_routine_pops = 1;
            have.f_pop = 1;
        } else if( !have.f_reverse && PragRecog( "reverse" ) ) {
            AuxInfo.cclass |= REVERSE_PARMS;
            have.f_reverse = 1;
        } else if( !have.f_nomemory && PragRecog( "nomemory" ) ) {
            AuxInfo.cclass |= NO_MEMORY_READ;
            have.f_nomemory = 1;
        } else if( !have.f_loadds && PragRecog( "loadds" ) ) {
            AuxInfo.cclass |= LOAD_DS_ON_CALL;
            have.f_loadds = 1;
        } else if( !have.f_list && PragRegSet() != T_NULL ) {
            AuxInfo.parms = PragManyRegSets();
            have.f_list = 1;
        } else {
            break;
        }
    }
}


local void GetSTRetInfo( void )
/*****************************/
{
    struct {
        unsigned f_float        : 1;
        unsigned f_struct       : 1;
        unsigned f_allocs       : 1;
        unsigned f_list         : 1;
    } have;

    have.f_float  = 0;
    have.f_struct = 0;
    have.f_allocs = 0;
    have.f_list   = 0;
    for( ;; ) {
        if( !have.f_float && PragRecog( "float" ) ) {
            have.f_float = 1;
            AuxInfo.cclass |= NO_FLOAT_REG_RETURNS;
        } else if( !have.f_struct && PragRecog( "struct" ) ) {
            have.f_struct = 1;
            AuxInfo.cclass |= NO_STRUCT_REG_RETURNS;
        } else if( !have.f_allocs && PragRecog( "routine" ) ) {
            have.f_allocs = 1;
            AuxInfo.cclass |= ROUTINE_RETURN;
        } else if( !have.f_allocs && PragRecog( "caller" ) ) {
            have.f_allocs = 1;
            AuxInfo.cclass &= ~ROUTINE_RETURN;
            AuxInfoFlg.f_caller_return = 1;
        } else if( !have.f_list && PragRegSet() != T_NULL ) {
            have.f_list = 1;
            AuxInfo.cclass |= SPECIAL_STRUCT_RETURN;
            AuxInfo.streturn = PragRegList();
        } else {
            break;
        }
    }
}


local void GetRetInfo( void )
/***************************/
{
    struct {
        unsigned f_no8087        : 1;
        unsigned f_list          : 1;
        unsigned f_struct        : 1;
    } have;

    have.f_no8087  = 0;
    have.f_list    = 0;
    have.f_struct  = 0;
    AuxInfo.cclass &= ~ NO_8087_RETURNS;               /* 29-mar-90 */
    AuxInfoFlg.f_8087_returns = 1;
    for( ;; ) {
        if( !have.f_no8087 && PragRecog( "no8087" ) ) {
            have.f_no8087 = 1;
            HW_CTurnOff( AuxInfo.returns, HW_FLTS );
            AuxInfo.cclass |= NO_8087_RETURNS;
        } else if( !have.f_list && PragRegSet() != T_NULL ) {
            have.f_list = 1;
            AuxInfo.cclass |= SPECIAL_RETURN;
            AuxInfo.returns = PragRegList();
        } else if( !have.f_struct && PragRecog( "struct" ) ) {
            have.f_struct = 1;
            GetSTRetInfo();
        } else {
            break;
        }
    }
}


local void GetSaveInfo( void )
/****************************/
{
    struct {
        unsigned    f_exact     : 1;
        unsigned    f_nomemory  : 1;
        unsigned    f_list      : 1;
    } have;

    have.f_exact    = 0;
    have.f_nomemory = 0;
    have.f_list     = 0;
    for( ;; ) {
        if( !have.f_exact && PragRecog( "exact" ) ) {
            AuxInfo.cclass |= MODIFY_EXACT;
            have.f_exact = 1;
        } else if( !have.f_nomemory && PragRecog( "nomemory" ) ) {
            AuxInfo.cclass |= NO_MEMORY_CHANGED;
            have.f_nomemory = 1;
        } else if( !have.f_list && PragRegSet() != T_NULL ) {
            HW_TurnOn( AuxInfo.save, PragRegList() );
            have.f_list = 1;
        } else {
            break;
        }
    }
}

void PragAux( void )
/******************/
{
    struct {
        unsigned    f_call   : 1;
        unsigned    f_loadds : 1;
        unsigned    f_rdosdev: 1;
        unsigned    f_export : 1;
        unsigned    f_parm   : 1;
        unsigned    f_value  : 1;
        unsigned    f_modify : 1;
        unsigned    f_frame  : 1;
        unsigned    uses_auto: 1;
    } have;

    InitAuxInfo();
    if( GetAliasInfo() ) {
        SetCurrInfo( Buffer );
        NextToken();
        PragObjNameInfo( &AuxInfo.objname );
        have.f_call   = 0;
        have.f_loadds = 0;
        have.f_rdosdev = 0;
        have.f_export = 0;
        have.f_parm   = 0;
        have.f_value  = 0;
        have.f_modify = 0;
        have.f_frame = 0;
        have.uses_auto = 0; /* BBB - Jan 26, 1994 */
        for( ;; ) {
            if( !have.f_call && CurToken == T_EQUAL ) {
                have.uses_auto = GetByteSeq( &AuxInfo.code );
                have.f_call = 1;
            } else if( !have.f_call && PragRecog( "far" ) ) {
                AuxInfo.cclass |= FAR_CALL;
                have.f_call = 1;
            } else if( !have.f_call && PragRecog( "near" ) ) {
                AuxInfo.cclass &= ~FAR_CALL;
                AuxInfoFlg.f_near = 1;
                have.f_call = 1;
            } else if( !have.f_loadds && PragRecog( "loadds" ) ) {
                AuxInfo.cclass |= LOAD_DS_ON_ENTRY;
                have.f_loadds = 1;
            } else if( !have.f_rdosdev && PragRecog( "rdosdev" ) ) {
                AuxInfo.cclass |= LOAD_RDOSDEV_ON_ENTRY;
                have.f_rdosdev = 1;
            } else if( !have.f_export && PragRecog( "export" ) ) {
                AuxInfo.cclass |= DLL_EXPORT;
                have.f_export = 1;
            } else if( !have.f_parm && PragRecog( "parm" ) ) {
                GetParmInfo();
                have.f_parm = 1;
            } else if( !have.f_value && PragRecog( "value" ) ) {
                GetRetInfo();
                have.f_value = 1;
            } else if( !have.f_value && PragRecog( "aborts" ) ) {
                AuxInfo.cclass |= SUICIDAL;
                have.f_value = 1;
            } else if( !have.f_modify && PragRecog( "modify" ) ) {
                GetSaveInfo();
                have.f_modify = 1;
            } else if( !have.f_frame && PragRecog( "frame" ) ) {
                AuxInfo.cclass |= GENERATE_STACK_FRAME;
                have.f_frame = 1;
            } else {
                break;
            }
        }
        if( have.uses_auto ) {
            /*
               We want to force the calling routine to set up a [E]BP frame
               for the use of this pragma. This is done by saying the pragma
               modifies the [E]SP register. A kludge, but it works.
            */
            HW_CTurnOn( AuxInfo.save, HW_SP );
        }
        CopyAuxInfo();
        PragEnding();
    }
}

void AsmSysInit( unsigned char *buf )
/***********************************/
{
    AsmCodeBuffer = buf;
    AsmCodeAddress = 0;
    AsmSaveCPUInfo();
}

void AsmSysFini( void )
/*********************/
{
    AsmSymFini();
    AsmRestoreCPUInfo();
}

void AsmSysMakeInlineAsmFunc( int code_ovrflw )
/*********************************************/
{
    int                 code_length;
    SYM_HANDLE          sym_handle;
    TREEPTR             tree;
    bool                uses_auto;
    char                name[8];

    code_ovrflw = code_ovrflw;
    code_length = AsmCodeAddress;
    if( code_length != 0 ) {
        sprintf( name, "F.%d", AsmFuncNum );
        ++AsmFuncNum;
        CreateAux( name );
        CurrInfo = (aux_info *)CMemAlloc( sizeof( aux_info ) );
        *CurrInfo = WatcallInfo;
        CurrInfo->use = 1;
        CurrInfo->save = AsmRegsSaved;  // indicate no registers saved
        uses_auto = InsertFixups( AsmCodeBuffer, code_length, &CurrInfo->code );
        if( uses_auto ) {
            /*
               We want to force the calling routine to set up a [E]BP frame
               for the use of this pragma. This is done by saying the pragma
               modifies the [E]SP register. A kludge, but it works.
            */
            HW_CTurnOff( CurrInfo->save, HW_SP );
        }
        CurrEntry->info = CurrInfo;
        CurrEntry->next = AuxList;
        AuxList = CurrEntry;
        CurrEntry = NULL;
        sym_handle = MakeFunction( CStrSave( name ),
                        FuncNode( GetType( TYPE_VOID ), 0, NULL ) );
        tree = LeafNode( OPR_FUNCNAME );
        tree->op.u2.sym_handle = sym_handle;
        tree = ExprNode( tree, OPR_CALL, NULL );
        tree->u.expr_type = GetType( TYPE_VOID );
        AddStmt( tree );
    }
}

char const *AsmSysDefineByte( void )
/**********************************/
{
    return( "db " );
}
