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

#include "plusplus.h"
#include "preproc.h"
#include "name.h"
#include "memmgr.h"
#include "errdefns.h"
#include "cgdata.h"
#include "pragdefn.h"
#include "pdefn2.h"
#include "asmsym.h"
#include "fnovload.h"
#include "cgswitch.h"
#include "initdefs.h"
#include "cginlibs.h"
#include "cginimps.h"
#include "asmstmt.h"
#include "pcheader.h"


extern  int     GetAliasInfo();
extern  void    AsmInit(int,int,int);

static  hw_reg_set      asmRegsSaved = { HW_D( HW_FULL ) };
static  hw_reg_set      stackParms[] = { HW_D( HW_EMPTY ) };
#if _CPU == 386
static  hw_reg_set      optlinkParms[] = {
    HW_D( HW_FLTS ),
    HW_D( HW_EMPTY )
};
#endif

#define WCPP_ASM     // enable assembler

static void pragmaInitInfo(     // INITIALIZE INFO STRUCTURE
    AUX_INFO *info,             // - structure to be inited
    call_class info_class,      // - class information
    const char *objname )       // - object-name pattern
{
    info->_class = info_class;
    info->objname = strsave( objname );
    info->parms = (hw_reg_set *)vctsave( (char *)stackParms
                                       , sizeof( stackParms ) );
    HW_CAsgn( info->returns, HW_EMPTY );
}

static void pragmaInitOptlink(  // INITIALIZE INFO STRUCTURE
    AUX_INFO *info,             // - structure to be inited
    call_class info_class,      // - class information
    const char *objname )       // - object-name pattern
{
    info->_class = info_class;
    info->objname = strsave( objname );
#if _CPU == 386
    HW_CTurnOn( optlinkParms[0], HW_EAX );
    HW_CTurnOn( optlinkParms[0], HW_ECX );
    HW_CTurnOn( optlinkParms[0], HW_EDX );
    info->parms = (hw_reg_set *)vctsave( (char *)optlinkParms
                                       , sizeof( optlinkParms ) );
#else
    info->parms = (hw_reg_set *)vctsave( (char *)stackParms
                                       , sizeof( stackParms ) );
#endif
    HW_CAsgn( info->returns, HW_FLTS );
}


static void pragmaInit(         // INITIALIZATION FOR PRAGMAS
    INITFINI* defn )            // - definition
{
    call_class call_type;

    defn = defn;
    PragInit();
    call_type = DefaultInfo._class & FAR;
    FortranInfo.objname = strsave( "^" );
    FortranInfo.parms = AuxParmDup( FortranInfo.parms );
    pragmaInitInfo( &CdeclInfo
                  , call_type
                   | CALLER_POPS
                   | NO_FLOAT_REG_RETURNS
                   | NO_STRUCT_REG_RETURNS
                   | ROUTINE_RETURN
#if _CPU == 8086
                   | LOAD_DS_ON_CALL
#endif
                   | SPECIAL_STRUCT_RETURN
                  , "_*" );

    pragmaInitInfo( &PascalInfo
                  , call_type
                   | REVERSE_PARMS
                   | NO_FLOAT_REG_RETURNS
                   | NO_STRUCT_REG_RETURNS
                   | SPECIAL_STRUCT_RETURN
                  , "^" );

    pragmaInitInfo( &SyscallInfo
                  , CALLER_POPS
                   | NO_STRUCT_REG_RETURNS
                   | SPECIAL_STRUCT_RETURN
                  , "*" );

    pragmaInitOptlink( &OptlinkInfo
                  , CALLER_POPS
#ifdef PARMS_STACK_RESERVE
                   | PARMS_STACK_RESERVE
#endif
                   | NO_STRUCT_REG_RETURNS
                   | SPECIAL_STRUCT_RETURN
                  , "*" );

    pragmaInitInfo( &StdcallInfo
                  , call_type
                   | SPECIAL_STRUCT_RETURN
                  , "_*" );

#ifdef __OLD_STDCALL
    pragmaInitInfo( &OldStdcallInfo
                  , call_type
                   | SPECIAL_STRUCT_RETURN
                  , "_*" );
#endif

    #if _CPU == 386
        HW_CTurnOff( CdeclInfo.save, HW_EAX );
        HW_CTurnOff( CdeclInfo.save, HW_ECX );
        HW_CTurnOff( CdeclInfo.save, HW_EDX );
        HW_CAsgn( CdeclInfo.streturn, HW_EAX );

        HW_CTurnOff( PascalInfo.save, HW_EAX );
        HW_CTurnOff( PascalInfo.save, HW_EBX );
        HW_CTurnOff( PascalInfo.save, HW_ECX );
        HW_CTurnOff( PascalInfo.save, HW_EDX );

        HW_CTurnOff( SyscallInfo.save, HW_EAX );
        HW_CTurnOn ( SyscallInfo.save, HW_EBX );
        HW_CTurnOff( SyscallInfo.save, HW_ECX );
        HW_CTurnOff( SyscallInfo.save, HW_EDX );
        HW_CAsgn( SyscallInfo.streturn, HW_EMPTY );

        HW_CTurnOff( OptlinkInfo.save, HW_EAX );
        HW_CTurnOff( OptlinkInfo.save, HW_ECX );
        HW_CTurnOff( OptlinkInfo.save, HW_EDX );
        HW_CAsgn( OptlinkInfo.streturn, HW_EMPTY );

        HW_CTurnOff( StdcallInfo.save, HW_EAX );
        HW_CTurnOff( StdcallInfo.save, HW_ECX );
        HW_CTurnOff( StdcallInfo.save, HW_EDX );

#ifdef __OLD_STDCALL
        HW_CTurnOff( OldStdcallInfo.save, HW_EAX );
        HW_CTurnOff( OldStdcallInfo.save, HW_ECX );
        HW_CTurnOff( OldStdcallInfo.save, HW_EDX );
#endif

        HW_CTurnOff( asmRegsSaved, HW_EAX );
        HW_CTurnOff( asmRegsSaved, HW_EBX );
        HW_CTurnOff( asmRegsSaved, HW_ECX );
        HW_CTurnOff( asmRegsSaved, HW_EDX );
        HW_CTurnOff( asmRegsSaved, HW_ESI );
        HW_CTurnOff( asmRegsSaved, HW_EDI );
    #else
        HW_CTurnOff( CdeclInfo.save, HW_ABCD );
        HW_CTurnOff( CdeclInfo.save, HW_ES );
        HW_CAsgn( CdeclInfo.streturn, HW_AX );

        HW_CTurnOff( PascalInfo.save, HW_ABCD );
        HW_CTurnOff( PascalInfo.save, HW_ES );

        /* roughly like pascal */
        HW_CTurnOff( SyscallInfo.save, HW_ABCD );
        HW_CTurnOff( SyscallInfo.save, HW_ES );

        /* roughly like pascal */
        HW_CTurnOff( OptlinkInfo.save, HW_ABCD );
        HW_CTurnOff( OptlinkInfo.save, HW_ES );

        /* roughly like cdecl */
        HW_CTurnOff( StdcallInfo.save, HW_ABCD );
        HW_CTurnOff( StdcallInfo.save, HW_ES );
        HW_CAsgn( StdcallInfo.streturn, HW_AX );

#ifdef __OLD_STDCALL
        HW_CTurnOff( OldStdcallInfo.save, HW_ABCD );
        HW_CTurnOff( OldStdcallInfo.save, HW_ES );
        HW_CAsgn( OldStdcallInfo.streturn, HW_AX );
#endif

        HW_CTurnOff( asmRegsSaved, HW_ABCD );
        HW_CTurnOff( asmRegsSaved, HW_SI );
        HW_CTurnOff( asmRegsSaved, HW_DI );
        HW_CTurnOff( asmRegsSaved, HW_ES );
    #endif

    #if _CPU == 386
    {
        /*
            These pragmas are used for all far16 function definitions
            depending on whether the function's parms are reversed or not.
            Far16 calls are handled by the code generator w.r.t. what
            calling convention is used.
        */
        Far16CdeclInfo = DefaultInfo;
        pragmaInitInfo( &Far16CdeclInfo
                      , call_type
                       | CALLER_POPS
                       | NO_FLOAT_REG_RETURNS
                       | NO_STRUCT_REG_RETURNS
                       | ROUTINE_RETURN
                       | SPECIAL_STRUCT_RETURN
                       | FAR16_CALL
                      , "_*" );
        HW_CTurnOff( Far16CdeclInfo.save, HW_EAX );
        HW_CTurnOff( Far16CdeclInfo.save, HW_EBX );
        HW_CTurnOff( Far16CdeclInfo.save, HW_ECX );
        HW_CTurnOff( Far16CdeclInfo.save, HW_EDX );
        HW_CAsgn( Far16CdeclInfo.streturn, HW_EAX );

        Far16PascalInfo = DefaultInfo;
        pragmaInitInfo( &Far16PascalInfo
                      , call_type
                       | REVERSE_PARMS
                       | NO_FLOAT_REG_RETURNS
                       | NO_STRUCT_REG_RETURNS
                       | SPECIAL_STRUCT_RETURN
                       | FAR16_CALL
                      , "^" );

        HW_CTurnOff( Far16PascalInfo.save, HW_EAX );
        HW_CTurnOff( Far16PascalInfo.save, HW_EBX );
        HW_CTurnOff( Far16PascalInfo.save, HW_ECX );
        HW_CTurnOff( Far16PascalInfo.save, HW_EDX );
    }
    #endif
}


static void freeInfo( AUX_INFO *info )
{
    if( info->code != NULL ) {
        CMemFree( info->code );
        info->code = NULL;
    }
    if( info->parms != DefaultParms ) {
        CMemFree( info->parms );
        info->parms = NULL;
    }
    if( info->objname != NULL ) {
        CMemFree( info->objname );
        info->objname = NULL;
    }
}


static void pragmaFini(         // FINISH PRAGMAS
    INITFINI* defn )            // - definition
{
    register struct aux_entry *next;
    register void             *junk;

    defn = defn;
    next = AuxList;
    while( next != NULL ) {
        junk = next;
        if( next->info != NULL ) {
            if( next->info->use != 1 ) {
                next->info->use--;
            } else {
                freeInfo( next->info );
#ifndef NDEBUG
                if(( next->info == &DefaultInfo ) ||
                   ( next->info == &CdeclInfo ) ||
                   ( next->info == &PascalInfo ) ||
                   ( next->info == &SyscallInfo ) ||
                   ( next->info == &OptlinkInfo ) ||
                   ( next->info == &StdcallInfo ) ||
                   ( next->info == &Far16CdeclInfo ) ||
                   ( next->info == &Far16PascalInfo ) ||
#ifdef __OLD_STDCALL
                   ( next->info == &OldStdcallInfo ) ||
#endif
                   ( next->info == &FortranInfo ) ) {
                    CFatal( "freeing a static calling convention info" );
                }
#endif
                if( next->info != &DefaultInfo )  CMemFree( next->info );
            }
        }
        next = next->next;
        CMemFree( junk );
    }
    freeInfo( &DefaultInfo );
    freeInfo( &CdeclInfo );
    freeInfo( &PascalInfo );
    freeInfo( &SyscallInfo );
    freeInfo( &OptlinkInfo );
    freeInfo( &StdcallInfo );
    freeInfo( &Far16CdeclInfo );
    freeInfo( &Far16PascalInfo );
#ifdef __OLD_STDCALL
    freeInfo( &OldStdcallInfo );
#endif
    freeInfo( &FortranInfo );
    //CMemFreePtr( &FortranInfo.objname );
    AuxList = NULL;
    CgInfoFreeLibs();
    CgInfoFreeImports();
}


INITDEFN( pragmas, pragmaInit, pragmaFini )


static void assemblerInit(      // INITIALIZATION OF ASSEMBLER
    INITFINI* defn )            // - definition
{
    int         cpu;
    int         fpu;
    int         use32;

    defn = defn;
    if( CompFlags.dll_subsequent ) return;
#if _CPU == 386
    use32 = 1;
    cpu = 3;
#else
    use32 = 0;
    cpu = 0;
#endif
    switch( GET_CPU( CpuSwitches ) ) {
      case CPU_86:        cpu = 0; break;
      case CPU_186:       cpu = 1; break;
      case CPU_286:       cpu = 2; break;
      case CPU_386:       cpu = 3; break;
      case CPU_486:       cpu = 4; break;
      case CPU_586:       cpu = 5; break;
      case CPU_686:       cpu = 6; break;
    }
    switch( GET_FPU( CpuSwitches ) ) {
    case FPU_NONE:      fpu = 0; break;
    default:            fpu = 1; break;
    }
    AsmInit( cpu, fpu, use32 );
}


INITDEFN( assembler, assemblerInit, InitFiniStub )


static boolean GetAliasInfo(
    void )
{
    char buff[256];
    boolean isfar16;
    boolean retn;

    CurrAlias = &DefaultInfo;
    if( CurToken != T_LEFT_PAREN ) {
        retn = TRUE;
    } else {
        NextToken();
        if( ! PragIdCurToken() ) {
            retn = FALSE;
        } else {
            PragCurrAlias();
            strcpy( buff, Buffer );
            NextToken();
            if( CurToken == T_RIGHT_PAREN ) {
                NextToken();
                retn = TRUE;
            } else if( CurToken == T_COMMA ) {
                NextToken();
                if( ! PragIdCurToken() ) {
                    retn = FALSE;
                } else {
                    isfar16 = PragRecog( "far16" );
                    CreateAux( buff );
                    PragCurrAlias();
                    NextToken();
                    if( CurToken == T_RIGHT_PAREN ) {
                        AuxCopy( CurrInfo, CurrAlias );
                        NextToken();
                    }
                    if( isfar16 ) {
                        CurrInfo->flags |= AUX_FLAG_FAR16;
                    }
                    PragEnding( TRUE );
                    retn = FALSE;
                }
            }
        }
    }
    return retn;
}


static void doPragAux(                   // #PRAGMA AUX ...
    void )
{
    struct {
        unsigned f_call   : 1;
        unsigned f_loadds : 1;
        unsigned f_export : 1;
        unsigned f_parm   : 1;
        unsigned f_value  : 1;
        unsigned f_modify : 1;
        unsigned f_frame  : 1;
        unsigned uses_auto: 1;
    } have;

    if( !GetAliasInfo() ) return;
    CurrEntry = NULL;
    if( !PragIdCurToken() ) return;
    SetCurrInfo();
    NextToken();
    AuxCopy( CurrInfo, CurrAlias );
    PragObjNameInfo();
    have.f_call   = 0;
    have.f_loadds = 0;
    have.f_export = 0;
    have.f_parm   = 0;
    have.f_value  = 0;
    have.f_modify = 0;
    have.f_frame = 0;
    have.uses_auto = 0;
    for( ;; ) {
        if( !have.f_call && CurToken == T_EQUAL ) {
            have.uses_auto = GetByteSeq();
            have.f_call = 1;
        } else if( !have.f_call && PragRecog( "far" ) ) {
            CurrInfo->_class |= FAR;
            have.f_call = 1;
        } else if( !have.f_call && PragRecog( "near" ) ) {
            CurrInfo->_class &= ~FAR;
            have.f_call = 1;
        } else if( !have.f_loadds && PragRecog( "loadds" ) ) {
            CurrInfo->_class |= LOAD_DS_ON_ENTRY;
            have.f_loadds = 1;
        } else if( !have.f_export && PragRecog( "export" ) ) {
            CurrInfo->_class |= DLL_EXPORT;
            have.f_export = 1;
        } else if( !have.f_parm && PragRecog( "parm" ) ) {
            GetParmInfo();
            have.f_parm = 1;
        } else if( !have.f_value && PragRecog( "value" ) ) {
            GetRetInfo();
            have.f_value = 1;
        } else if( !have.f_value && PragRecog( "aborts" ) ) {
            CurrInfo->_class |= SUICIDAL;
            have.f_value = 1;
        } else if( !have.f_modify && PragRecog( "modify" ) ) {
            GetSaveInfo();
            have.f_modify = 1;
        } else if( !have.f_frame && PragRecog( "frame" ) ) {
            CurrInfo->_class |= GENERATE_STACK_FRAME;
            have.f_frame = 1;
        } else {
            break;
        }
    }
    if( have.uses_auto ) {
        AsmSysUsesAuto();
    }
    PragEnding( TRUE );
}

void PragAux(                   // #PRAGMA AUX ...
    void )
{
    PPState = PPS_EOL;
    doPragAux();
    PPState = PPS_EOL | PPS_NO_EXPAND;
}


typedef enum
{       FIXWORD_NONE
,       FIXWORD_FLOAT
,       FIXWORD_SEGMENT
,       FIXWORD_OFFSET
,       FIXWORD_RELOFF
} fix_words;


static fix_words FixupKeyword(
    void )
{
    fix_words retn;             // - return

    if( CurToken == T_FLOAT ) {
        NextToken();
        retn = FIXWORD_FLOAT;
    } else if( PragRecog( "seg" ) ) {
        retn = FIXWORD_SEGMENT;
    } else if( PragRecog( "offset" ) ) {
        retn = FIXWORD_OFFSET;
    } else if( PragRecog( "reloff" ) ) {
        retn = FIXWORD_RELOFF;
    } else {
        retn = FIXWORD_NONE;
    }
    return( retn );
}


enum sym_state AsmQueryExternal(
    char *name )
{
#if 1
    SYMBOL sym;
    enum sym_state state;

    state = SYM_UNDEFINED;
    sym = ScopeASMLookup( name );
    if( sym != NULL ) {
        if( SymIsAutomatic( sym ) ) {
            state = SYM_STACK;
        } else {
            state = SYM_EXTERNAL;
        }
    }
    return( state );
#else
    name = name;
    return( SYM_UNDEFINED );
#endif
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

#ifdef WCPP_ASM
static enum sym_type CodePtrType( type_flag flags )
{
    enum sym_type retn;

    if( flags & TF1_FAR ) {
        retn = SYM_FFAR;
    } else if( flags & TF1_NEAR ) {
        retn = SYM_FNEAR;
    } else if( TargetSwitches & BIG_CODE ) {
        retn = SYM_FFAR;
    } else {
        retn = SYM_FNEAR;
    }
    return retn;
}
#endif


static enum sym_type PtrType( type_flag flags )
{
    enum sym_type retn;

    if( flags & ( TF1_FAR | TF1_HUGE ) ) {
        retn = SYM_DFAR;
    } else if( flags & TF1_NEAR ) {
        retn = SYM_DNEAR;
    } else if( TargetSwitches & BIG_DATA ) {
        retn = SYM_DFAR;
    } else {
        retn = SYM_DNEAR;
    }
    return retn;
}


#ifdef WCPP_ASM

#define ENTRY_ERROR             0,
#define ENTRY_BOOL              SYM_INT1,
#define ENTRY_CHAR              SYM_INT1,
#define ENTRY_SCHAR             SYM_INT1,
#define ENTRY_UCHAR             SYM_INT1,
#define ENTRY_WCHAR             SYM_INT2,
#define ENTRY_SSHORT            SYM_INT2,
#define ENTRY_USHORT            SYM_INT2,
#define ENTRY_SINT              SYM_INT,
#define ENTRY_UINT              SYM_INT,
#define ENTRY_SLONG             SYM_INT4,
#define ENTRY_ULONG             SYM_INT4,
#define ENTRY_SLONG64           SYM_INT4,       // SYM_INT8 NYI
#define ENTRY_ULONG64           SYM_INT4,       // SYM_INT8 NYI
#define ENTRY_FLOAT             SYM_FLOAT4,
#define ENTRY_DOUBLE            SYM_FLOAT8,
#define ENTRY_LONG_DOUBLE       SYM_FLOAT8,
#define ENTRY_ENUM              0,
#define ENTRY_POINTER           0,
#define ENTRY_TYPEDEF           0,
#define ENTRY_CLASS             0,
#define ENTRY_BITFIELD          0,
#define ENTRY_FUNCTION          0,
#define ENTRY_ARRAY             0,
#define ENTRY_DOT_DOT_DOT       0,
#define ENTRY_VOID              SYM_INT1,
#define ENTRY_MODIFIER          0,
#define ENTRY_MEMBER_POINTER    0,
#define ENTRY_GENERIC           0,

static enum sym_type AsmDataType[] = {
    #include "type_arr.h"
};
#endif

#ifdef WCPP_ASM

#define ASM_BLOCK       (64)

static enum sym_type AsmType(
    TYPE type )
{
    type_flag mod_flags;

    type = TypeModFlags( type, &mod_flags );
    switch( type->id ) {
      case TYP_POINTER :
        type = TypeModFlags( type->of, &mod_flags );
        if( type->id == TYP_FUNCTION ) {
            return( CodePtrType( mod_flags ) );
        } else {
            return( PtrType( mod_flags ) );
        }
        break;
      case TYP_ARRAY :
      case TYP_BITFIELD :
      case TYP_ENUM :
        return( AsmType( type->of ) );
        break;
      case TYP_CLASS :
        return( SYM_INT1 );
        break;
      case TYP_FUNCTION :
        return( CodePtrType( mod_flags ) );
        break;
      default:
        return( AsmDataType[ type->id ] );
        break;
    }
}
#endif


enum sym_type AsmQueryType( char *name )
{
#if 1
    SYMBOL sym;
    enum sym_type type;

    sym = ScopeASMLookup( name );
    if( sym != NULL ) {
        type = AsmType( sym->sym_type );
    } else {
        type = SYM_INT1;
    }
    return( type );
#else
    name = name;
    return( SYM_INT1 );
#endif
}


static int insertFixups( VBUF *code_buffer, unsigned char *buff, unsigned i )
{
    struct asmfixup     *fix;
    struct asmfixup     *head;
    struct asmfixup     *chk;
    struct asmfixup     *next;
    struct asmfixup     **owner;
    unsigned char       *src;
    unsigned char       *end;
    unsigned char       cg_fix;
    unsigned long       perform_fixups;
    byte_seq            *seq;
    SYMBOL              sym;
    char                *name;
    unsigned            dst;
    unsigned            skip;
    int                 mutate_to_segment;
    boolean             uses_auto;

    uses_auto = FALSE;
    perform_fixups = 0;
    head = FixupHead;
    if( head != NULL ) {
        FixupHead = NULL;
        /* sort the fixup list in increasing fix_loc's */
        for( fix = head; fix != NULL; fix = next ) {
            owner = &FixupHead;
            for( ;; ) {
                chk = *owner;
                if( chk == NULL ) break;
                if( chk->fix_loc > fix->fix_loc ) break;
                owner = &chk->next;
            }
            next = fix->next;
            fix->next = *owner;
            *owner = fix;
        }
        dst = 0;
        src = buff;
        end = src + i;
        fix = FixupHead;
        owner = &FixupHead;
        /* insert fixup escape sequences */
        while( src < end ) {
            /* reserve at least ASM_BLOCK bytes in the buffer */
            VbufReqd( code_buffer, ( (dst+ASM_BLOCK) + (ASM_BLOCK-1) ) & ~(ASM_BLOCK-1) );
            if( fix != NULL && fix->fix_loc == (src - buff) ) {
                name = fix->name;
                if( name != NULL ) {
                    sym = ScopeASMUseSymbol( name, &uses_auto );
                    if( sym == NULL ) {
                        return( 0 );
                    }
                }
                /* insert fixup information */
                skip = 0;
                code_buffer->buf[ dst++ ] = FLOATING_FIXUP_BYTE;
                mutate_to_segment = 0;
                switch( fix->fix_type ) {
                case FIX_SEG:
                    if( name == NULL ) {
                        /* special case for floating point fixup */
                        if( *src != 0x9b ) { /* FWAIT */
                            code_buffer->buf[ dst++ ] = 0x9b;
                        }
                    } else {
                        skip = 2;
                        cg_fix = FIX_SYM_SEGMENT;
                    }
                    break;
                case FIX_RELOFF8:
                    CErr2p( ERR_ASSEMBLER_ERROR, "cannot reach label with short jump" );
                    break;
                case FIX_RELOFF16:
                    skip = 2;
                    cg_fix = FIX_SYM_RELOFF;
                    break;
                case FIX_RELOFF32:
                    skip = 4;
                    cg_fix = FIX_SYM_RELOFF;
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
                    break;
                }
                if( skip != 0 ) {
                    code_buffer->buf[ dst++ ] = cg_fix;
                    *((unsigned long *)&code_buffer->buf[dst]) = (unsigned long)sym;
                    dst += sizeof( long );
                    *((unsigned long *)&code_buffer->buf[dst]) = fix->offset;
                    dst += sizeof( long );
                    src += skip;
                }
                if( mutate_to_segment ) {
                    /*
                        Since the CG escape sequences don't allow for
                        FAR pointer fixups, we have to split them into two.
                        This is done by doing the offset fixup first, then
                        mutating the fixup structure to look like a segment
                        fixup one near pointer size later.
                    */
                    fix->fix_type = FIX_SEG;
                    fix->fix_loc += skip;
                    fix->offset = 0;
                } else {
                    head = fix;
                    fix = fix->next;
                    if( head->external ) {
                        *owner = fix;
                        CMemFree( head->name );
                        CMemFree( head );
                    } else {
                        owner = &head->next;
                    }
                }
            } else {
                if( *src == FLOATING_FIXUP_BYTE ) {
                    code_buffer->buf[ dst++ ] = FLOATING_FIXUP_BYTE;
                }
                code_buffer->buf[ dst++ ] = *src++;
            }
            VbufUsed( code_buffer, dst );
        }
        buff = code_buffer->buf;
        i = dst;
        perform_fixups = DO_FLOATING_FIXUPS;
    }
    seq = CMemAlloc( offsetof( byte_seq, data ) + i );
    seq->length = i | perform_fixups;
    memcpy( seq->data, buff, i );
    CurrInfo->code = seq;
    return( uses_auto );
}


static void AddAFix(
    unsigned i,
    char *name,
    unsigned type,
    unsigned long off )
{
    struct asmfixup        *fix;

    fix = (struct asmfixup *)CMemAlloc( sizeof( *fix ) );
    fix->external = 1;
    fix->fix_loc = i;
    fix->name = name;
    fix->offset = off;
    fix->fix_type = type;
    fix->next = FixupHead;
    FixupHead = fix;
}

// WASM entry points
extern long Address;
extern char *CodeBuffer;
extern void AsmLine( char * );
extern void AsmSymFini( void );

syscode_seq *AuxCodeDup(        // DUPLICATE AUX CODE
    syscode_seq *code )
{
    syscode_seq *new_code;
    byte_seq_len code_length;
    byte_seq_len size;

    if( code == NULL ) {
        return( code );
    }
    code_length = code->length & MAX_BYTE_SEQ_LEN;
    size = offsetof( byte_seq, data ) + code_length;
    new_code = (byte_seq *)vctsave( (char *)code, size );
    return( new_code );
}

void AsmSysCopyCode( void )
/*************************/
{
    byte_seq *code;

    code = CurrInfo->code;
    if( code != NULL && code == CurrAlias->code ) {
        CurrInfo->code = AuxCodeDup( code );
    }
}

void AsmSysParseLine( char *line )
/********************************/
{
    AsmLine( line );
}

void AsmSysSetCodeBuffer( void *buff )
/************************************/
{
    CodeBuffer = buff;
}

uint_32 AsmSysAddress( void )
/***************************/
{
    return Address;
}

boolean AsmSysInsertFixups( VBUF *code )
/**************************************/
{
    boolean uses_auto;
    auto VBUF temp_buffer;

    VbufInit( &temp_buffer );
    VbufReqd( &temp_buffer, code->used );
    uses_auto = insertFixups( &temp_buffer, code->buf, code->used );
    VbufFree( &temp_buffer );
    AsmSymFini();
    return( uses_auto );
}

void *AsmSysCreateAux( char *name )
/*********************************/
{
    CreateAux( name );
    AuxCopy( CurrInfo, &DefaultInfo );
    CurrInfo->use = 1;
    CurrInfo->save = asmRegsSaved;
    CurrEntry->info = CurrInfo;
    return( CurrInfo );
}

void AsmSysUsesAuto( void )
/*************************/
{
    /*
       We want to force the calling routine to set up a [E]BP frame
       for the use of this pragma. This is done by saying the pragma
       modifies the [E]SP register. A kludge, but it works.
    */
    HW_CTurnOff( CurrInfo->save, HW_SP );
    ScopeASMUsesAuto();
}

char const *AsmSysDefineByte( void )
/**********************************/
{
    return( "db" );
}

void AsmSysDone( void )
/*********************/
{
    PragEnding( FALSE );
}

void AsmSysInit( void )
/*********************/
{
    Address = 0;
}

static char *copyCodeLen( char *d, void *v, unsigned len )
{
    char *s = v;
    while( len ) {
        *d++ = *s++;
        --len;
    }
    return( d );
}

void AsmSysPCHWriteCode( AUX_INFO *info )
/***************************************/
{
    SYMBOL sym;
    byte_seq_len code_length;
    byte_seq_len seq_size;
    unsigned fixup;
    byte_seq *code;
    char *d;
    char *s;
    char *c;
    char *p;
    char *tmp_buff;
#ifndef NDEBUG
    auto char buff[8];
#else
    auto char buff[1024];
#endif

    seq_size = 0;
    code = info->code;
    if( code == NULL ) {
        /* no code */
        PCHWriteUInt( seq_size );
        return;
    }
    code_length = code->length & MAX_BYTE_SEQ_LEN;
    seq_size = offsetof( byte_seq, data ) + code_length;
    PCHWriteUInt( seq_size );
    if(( code->length & DO_FLOATING_FIXUPS ) == 0 ) {
        /* code has no relocs */
        PCHWrite( code, seq_size );
        return;
    }
    /* code has relocs */
    tmp_buff = buff;
    if( seq_size > sizeof( buff ) ) {
        tmp_buff = CMemAlloc( seq_size );
    }
    c = code->data;
    s = code->data + code_length;
    d = copyCodeLen( tmp_buff, code, offsetof( byte_seq, data ) );
    for( p = c; p != s; ) {
        if( p[0] == FLOATING_FIXUP_BYTE ) {
            fixup = p[1];
            *d++ = *p++;
            *d++ = *p++;
            if( fixup != FLOATING_FIXUP_BYTE ) {
                switch( fixup ) {
                case FIX_SYM_OFFSET:
                case FIX_SYM_SEGMENT:
                case FIX_SYM_RELOFF:
                    sym = SymbolGetIndex( *((SYMBOL*)p) );
                    d = copyCodeLen( d, &sym, sizeof( sym ) );
                    p += sizeof( SYMBOL );
                    d = copyCodeLen( d, p, sizeof( unsigned long ) );
                    p += sizeof( unsigned long );
                    break;
                default:
                    DbgNever();
                }
            }
        } else {
            *d++ = *p++;
        }
    }
    PCHWrite( tmp_buff, seq_size );
    if( tmp_buff != buff ) {
        CMemFree( tmp_buff );
    }
}

void AsmSysPCHReadCode( AUX_INFO *info )
/**************************************/
{
    char *p;
    char *s;
    byte_seq *code;
    unsigned fixup;
    SYMBOL sym;
    byte_seq_len seq_size;
    byte_seq_len code_length;

    seq_size = PCHReadUInt();
    if( seq_size == 0 ) {
        info->code = NULL;
        return;
    }
    code = CMemAlloc( seq_size );
    info->code = code;
    PCHRead( code, seq_size );
    if(( code->length & DO_FLOATING_FIXUPS ) == 0 ) {
        /* code has no relocs */
        return;
    }
    code_length = code->length & MAX_BYTE_SEQ_LEN;
    p = code->data;
    s = code->data + code_length;
    while( p != s ) {
        if( p[0] == FLOATING_FIXUP_BYTE ) {
            fixup = p[1];
            p += 2;
            if( fixup != FLOATING_FIXUP_BYTE ) {
                switch( fixup ) {
                case FIX_SYM_OFFSET:
                case FIX_SYM_SEGMENT:
                case FIX_SYM_RELOFF:
                    sym = SymbolMapIndex( *((SYMBOL*)p) );
                    copyCodeLen( p, &sym, sizeof( sym ) );
                    p += sizeof( SYMBOL ) + sizeof( unsigned long );
                    break;
                default:
                    DbgNever();
                }
            }
        } else {
            ++p;
        }
    }
}

static int GetByteSeq(
    void )
{
    int i;
    char *name;
    unsigned long offset;
    unsigned fixword;
    int uses_auto;
    auto VBUF code_buffer;

    VbufInit( &code_buffer );
    NextToken();
    i = 0;
    for(;;) {
        /* reserve at least ASM_BLOCK bytes in the buffer */
        VbufReqd( &code_buffer, ( (i+ASM_BLOCK) + (ASM_BLOCK-1) ) & ~(ASM_BLOCK-1) );
        if( CurToken == T_STRING ) {
            Address = i;
            CodeBuffer = code_buffer.buf;
            AsmLine( Buffer );
            i = Address;
            NextToken();
            if( CurToken == T_COMMA )  NextToken();
        } else if( CurToken == T_CONSTANT ) {
            code_buffer.buf[ i++ ] = U32Fetch( Constant64 );
            NextToken();
        } else {
            fixword = FixupKeyword();
            if( fixword == FIXWORD_NONE ) break;
            if( fixword == FIXWORD_FLOAT ) {
                #if _CPU == 8086
                    AddAFix( i, NULL, FIX_SEG, 0 );
                #endif
            } else { /* seg or offset */
                if( !PragIdCurToken() ) {
                    CErr1( ERR_EXPECTING_ID );
                } else {
                    name = strsave( Buffer );
                    offset = 0;
                    NextToken();
                    if( CurToken == T_PLUS ) {
                        NextToken();
                        if( CurToken == T_CONSTANT ) {
                            offset = U32Fetch( Constant64 );
                            NextToken();
                        }
                    } else if( CurToken == T_MINUS ) {
                        NextToken();
                        if( CurToken == T_CONSTANT ) {
                            offset = - U32Fetch( Constant64 );
                            NextToken();
                        }
                    }
                }
                switch( fixword ) {
                  case FIXWORD_RELOFF:
                    #if _CPU == 8086
                        AddAFix( i, name, FIX_RELOFF16, offset );
                        i += 2;
                    #else
                        AddAFix( i, name, FIX_RELOFF32, offset );
                        i += 4;
                    #endif
                    break;
                  case FIXWORD_OFFSET:
                    #if _CPU == 8086
                        AddAFix( i, name, FIX_OFF16, offset );
                        i += 2;
                    #else
                        AddAFix( i, name, FIX_OFF32, offset );
                        i += 4;
                    #endif
                    break;
                  case FIXWORD_SEGMENT:
                    AddAFix( i, name, FIX_SEG, 0 );
                    i += 2;
                    break;
                }
            }
        }
        VbufUsed( &code_buffer, i );
    }
    uses_auto = AsmSysInsertFixups( &code_buffer );
    VbufFree( &code_buffer );
    return( uses_auto );
}


hw_reg_set PragRegName(         // GET REGISTER NAME
    char *str )                 // - register
{
    register int i;
    register char *p;
    hw_reg_set      name;

    if( *str == '_' ) {
        ++str;
    }
    if( *str == '_' ) {
        ++str;
    }
    i = 0;
    p = Registers;
    while( *p != '\0' ) {
        if( stricmp( p, str ) == 0 ) return( RegBits[ i ] );
        i++;
        while( *p++ != '\0' );
    }
    if( strcmp( str, "8087" ) == 0 ) {
        HW_CAsgn( name, HW_FLTS );
    } else {
        HW_CAsgn( name, HW_EMPTY );
    }
    return( name );
}



static void GetParmInfo(
    void )
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
            CurrInfo->_class |= CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_pop && PragRecog( "routine" ) ) {
            CurrInfo->_class &= ~ CALLER_POPS;
            have.f_pop = 1;
        } else if( !have.f_reverse && PragRecog( "reverse" ) ) {
            CurrInfo->_class |= REVERSE_PARMS;
            have.f_reverse = 1;
        } else if( !have.f_nomemory && PragRecog( "nomemory" ) ) {
            CurrInfo->_class |= NO_MEMORY_READ;
            have.f_nomemory = 1;
        } else if( !have.f_loadds && PragRecog( "loadds" ) ) {
            CurrInfo->_class |= LOAD_DS_ON_CALL;
            have.f_loadds = 1;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            PragManyRegSets();
            have.f_list = 1;
        } else {
            break;
        }
    }
}


static void GetRetInfo(
    void )
{
    struct {
        unsigned f_no8087        : 1;
        unsigned f_list          : 1;
        unsigned f_struct        : 1;
    } have;

    have.f_no8087  = 0;
    have.f_list    = 0;
    have.f_struct  = 0;
    CurrInfo->_class &= ~ NO_8087_RETURNS;
    for( ;; ) {
        if( !have.f_no8087 && PragRecog( "no8087" ) ) {
            have.f_no8087 = 1;
            HW_CTurnOff( CurrInfo->returns, HW_FLTS );
            CurrInfo->_class |= NO_8087_RETURNS;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            have.f_list = 1;
            CurrInfo->_class |= SPECIAL_RETURN;
            CurrInfo->returns = PragRegList();
        } else if( !have.f_struct && PragRecog( "struct" ) ) {
            have.f_struct = 1;
            GetSTRetInfo();
        } else {
            break;
        }
    }
}


static void GetSTRetInfo(
    void )
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
            CurrInfo->_class |= NO_FLOAT_REG_RETURNS;
        } else if( !have.f_struct && PragRecog( "struct" ) ) {
            have.f_struct = 1;
            CurrInfo->_class |= NO_STRUCT_REG_RETURNS;
        } else if( !have.f_allocs && PragRecog( "routine" ) ) {
            have.f_allocs = 1;
            CurrInfo->_class |= ROUTINE_RETURN;
        } else if( !have.f_allocs && PragRecog( "caller" ) ) {
            have.f_allocs = 1;
            CurrInfo->_class &= ~ROUTINE_RETURN;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            have.f_list = 1;
            CurrInfo->_class |= SPECIAL_STRUCT_RETURN;
            CurrInfo->streturn = PragRegList();
        } else {
            break;
        }
    }
}


static void GetSaveInfo(
    void )
{
    hw_reg_set      modlist;
    hw_reg_set      default_flt_n_seg;
    hw_reg_set      flt_n_seg;

    struct {
        unsigned f_exact        : 1;
        unsigned f_nomemory     : 1;
        unsigned f_list         : 1;
    } have;

    have.f_exact    = 0;
    have.f_nomemory = 0;
    have.f_list     = 0;
    for( ;; ) {
        if( !have.f_exact && PragRecog( "exact" ) ) {
            CurrInfo->_class |= MODIFY_EXACT;
            have.f_exact = 1;
        } else if( !have.f_nomemory && PragRecog( "nomemory" ) ) {
            CurrInfo->_class |= NO_MEMORY_CHANGED;
            have.f_nomemory = 1;
        } else if( !have.f_list && PragSet() != T_NULL ) {
            modlist = PragRegList();
            have.f_list = 1;
        } else {
            break;
        }
    }
    if( have.f_list ) {
        HW_Asgn( default_flt_n_seg, DefaultInfo.save );
        HW_CTurnOn( CurrInfo->save, HW_FULL );
        if( !have.f_exact && !CompFlags.save_restore_segregs ) {
            HW_CAsgn( flt_n_seg, HW_FLTS );
            HW_CTurnOn( flt_n_seg, HW_SEGS );
            HW_TurnOff( CurrInfo->save, flt_n_seg );
            HW_OnlyOn( default_flt_n_seg, flt_n_seg );
            HW_TurnOn( CurrInfo->save, default_flt_n_seg );
        }
        HW_TurnOff( CurrInfo->save, modlist );
    }
}

static boolean parmSetsIdentical( hw_reg_set *parms1, hw_reg_set *parms2 )
{
    if( parms1 == parms2 ) {
        return( TRUE );
    }
    if( parms1 != NULL && parms2 != NULL ) {
        for(;;) {
            if( HW_Equal( *parms1, *parms2 ) ) {
                if( HW_CEqual( *parms1, HW_EMPTY ) ) {
                    return( TRUE );
                }
                ++parms1;
                ++parms2;
            } else {
                break;
            }
        }
    }
    return( FALSE );
}

// The following defines which flags are to be ignored when checking
// a pragma call classes for equivalence.
//
#define CALL_CLASS_IGNORE ( 0                       \
                          | NO_MEMORY_CHANGED       \
                          | NO_MEMORY_READ          \
                          | MODIFY_EXACT            \
                          | GENERATE_STACK_FRAME    \
                          | EMIT_FUNCTION_NAME      \
                          | GROW_STACK              \
                          | PROLOG_HOOKS            \
                          | EPILOG_HOOKS            \
                          | TOUCH_STACK             \
                          | LOAD_DS_ON_ENTRY        \
                          | DLL_EXPORT              \
                          )

boolean PragmasTypeEquivalent(  // TEST IF TWO PRAGMAS ARE TYPE-EQUIVALENT
    AUX_INFO *inf1,             // - pragma [1]
    AUX_INFO *inf2 )            // - pragma [2]
{
    if( inf1 == NULL ) {
        inf1 = &DefaultInfo;
    }
    if( inf2 == NULL ) {
        inf2 = &DefaultInfo;
    }
    if( inf1 == inf2 ) {
        return TRUE;
    }
    return
           ( ( inf1->_class & ~CALL_CLASS_IGNORE ) ==
             ( inf2->_class & ~CALL_CLASS_IGNORE ) )
        && parmSetsIdentical( inf1->parms, inf2->parms )
        && HW_Equal( inf1->returns, inf2->returns )
        && HW_Equal( inf1->streturn, inf2->streturn )
        && HW_Equal( inf1->save, inf2->save )
        && ( inf1->flags == inf2->flags );
}

boolean PragmaOKForInlines(     // TEST IF PRAGMA IS SUITABLE FOR INLINED FN
    struct aux_info *fnp )      // - pragma
{
    if( fnp->code != NULL ) {
        return FALSE;
    }
    if( ReverseParms( fnp ) ) {
        return FALSE;
    }
    return TRUE;
}

boolean PragmaOKForVariables(   // TEST IF PRAGMA IS SUITABLE FOR A VARIABLE
    AUX_INFO *datap )           // - pragma
{
    AUX_INFO *def_info;

    // only the obj_name can be set for variables everything else is
    // specific to functions except for "__cdecl" and "__syscall"
    if( datap == &CdeclInfo ) {
        return( TRUE );
    }
    if( datap == &SyscallInfo ) {
        return( TRUE );
    }
    def_info = &DefaultInfo;
    if( datap->_class != def_info->_class ) {
        return( FALSE );
    }
    if( datap->code != def_info->code ) {
        return( FALSE );
    }
    if( ! parmSetsIdentical( datap->parms, def_info->parms ) ) {
        return( FALSE );
    }
    if( ! HW_Equal( datap->returns, def_info->returns ) ) {
        return( FALSE );
    }
    if( ! HW_Equal( datap->streturn, def_info->streturn ) ) {
        return( FALSE );
    }
    if( ! HW_Equal( datap->save, def_info->save ) ) {
        return( FALSE );
    }
    if( datap->flags != def_info->flags ) {
        return( FALSE );
    }
    return( TRUE );
}


static boolean okPtrChange(     // TEST IF OK TO CHANGE A PTR IN PRAGMA
    void *oldp,                 // - old ptr
    void *newp,                 // - new ptr
    void *defp )                // - default pointer
{
    return ( oldp == newp ) || ( oldp == defp );
}


static boolean okStrChange(     // TEST IF OK TO CHANGE A STRING IN PRAGMA
    void *oldp,                 // - old ptr
    void *newp,                 // - new ptr
    void *defp )                // - default pointer
{
    if( oldp == NULL || newp == NULL || defp == NULL ) {
        return ( oldp == newp ) || ( oldp == defp );
    }
    return ( 0 == strcmp( oldp, newp ) )
        || ( 0 == strcmp( oldp, defp ) );
}


static boolean okHwChange(      // TEST IF OK TO CHANGE HARDWARE DEFINITION
    hw_reg_set oldr,            // - reg. set, old
    hw_reg_set newr,            // - reg. set, new
    hw_reg_set defr )           // - reg. set, default
{
    return HW_Equal( oldr, newr ) || HW_Equal( oldr, defr );
}


static boolean okParmChange(    // TEST IF OK TO CHANGE PARMS
    hw_reg_set *oldr,           // - addr[ reg. set, old ]
    hw_reg_set *newr,           // - addr[ reg. set, new ]
    hw_reg_set *defr )          // - addr[ reg. set, default ]
{
    return parmSetsIdentical( oldr, newr ) || parmSetsIdentical( oldr, defr );
}


boolean PragmaChangeConsistent( // TEST IF PRAGMA CHANGE IS CONSISTENT
    AUX_INFO *oldp,             // - pragma (old)
    AUX_INFO *newp )            // - pragma (new)
{
    if( oldp == NULL ) {
        oldp = &DefaultInfo;
    }
    if( newp == NULL ) {
        newp = &DefaultInfo;
    }
    if( oldp == newp ) {
        return TRUE;
    }
    return ( ( oldp->_class & newp->_class ) == oldp->_class )
        && ( ( oldp->flags & newp->flags ) == oldp->flags )
        && ( okParmChange( oldp->parms
                         , newp->parms
                         , DefaultInfo.parms ) )
        && ( okHwChange( oldp->returns
                       , newp->returns
                       , DefaultInfo.returns ) )
        && ( okHwChange( oldp->streturn
                       , newp->streturn
                       , DefaultInfo.streturn ) )
        && ( okHwChange( oldp->save
                       , newp->save
                       , DefaultInfo.save ) )
        && ( okStrChange( oldp->objname
                        , newp->objname
                        , DefaultInfo.objname ) )
        && ( okPtrChange( oldp->code
                        , newp->code
                        , DefaultInfo.code ) )
        ;
}
