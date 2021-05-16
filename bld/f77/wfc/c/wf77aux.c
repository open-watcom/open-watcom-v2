/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Auxiliary information processing.
*
****************************************************************************/


#include "ftnstd.h"
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "global.h"
#include "fcgbls.h"
#include "wf77auxd.h"
#include "wf77aux.h"
#include "errcod.h"
#include "cpopt.h"
#include "progsw.h"
#include "fmemmgr.h"
#include "ferror.h"
#include "inout.h"
#include "cspawn.h"
#include "asmstmt.h"
#include "chain.h"
#include "data.h"
#include "kwlookup.h"
#include "symtab.h"
#include "auxlook.h"
#include "option.h"
#include "cioconst.h"

#include "clibext.h"


#define MAX_REG_SETS    16
#define MAXIMUM_BYTESEQ 127

#if _INTEL_CPU
#elif _CPU == _AXP || _CPU == _PPC
  #define AsmSymFini    AsmFini
#endif

#define AddDefaultLibConst(l,p) AddDefaultLib(l, sizeof( l ) - 1, p)


default_lib             *DefaultLibs;
dep_info                *DependencyInfo;

aux_info                *AuxInfo;
aux_info                FortranInfo;
aux_info                ProgramInfo;

static  aux_info        *CurrAux;
static  const char      *TokStart;
static  const char      *TokEnd;
static  aux_info        *AliasInfo;
static  char            SymName[MAX_SYMLEN];
static  size_t          SymLen;

#if _INTEL_CPU
static  arr_info        *ArrayInfo;
#endif

#if _CPU == 386
static      char    __Syscall[] = { "aux __syscall \"*\""
                                "parm caller []"
                                "value struct struct caller []"
                                "modify [eax ecx edx]" };
static      char    __Cdecl[] =   { "aux __cdecl \"_*\""
                                "parm caller loadds []"
                                "value struct float struct routine [eax]"
                                "modify [eax ebx ecx edx]" };
static      char    __Pascal[] =  { "aux __pascal \"^\""
                                "parm reverse routine []"
                                "value struct float struct caller []"
                                "modify [eax ebx ecx edx]" };
static      char    __Stdcall[] = { "aux __stdcall \"_*#\""
                                "parm routine []"
                                "value struct []"
                                "modify [eax ecx edx]" };
#elif _CPU == 8086
static      char    __Pascal[] =  { "aux __pascal \"^\""
                                "parm routine reverse []"
                                "value struct float struct caller []"
                                "modify [ax bx cx dx]" };
static      char    __Cdecl[] =   { "aux __cdecl \"_*\""
                                "parm caller []"
                                "value struct float struct routine [ax]"
                                "modify [ax bx cx dx]" };
#endif


#include "regs.c"


void InitAuxInfo( void )
//======================
{
#if _INTEL_CPU
    int         cpu;
    int         fpu;
  #if _CPU == 8086
    bool        fpu_emu;
  #endif

    cpu = 0;
  #if _CPU == 8086
    if( CPUOpts & CPUOPT_80186 )
        cpu = 1;
    if( CPUOpts & CPUOPT_80286 )
        cpu = 2;
  #endif
    if( CPUOpts & CPUOPT_80386 )
        cpu = 3;
    if( CPUOpts & CPUOPT_80486 )
        cpu = 4;
    if( CPUOpts & CPUOPT_80586 )
        cpu = 5;
    if( CPUOpts & CPUOPT_80686 )
        cpu = 6;

    if( CPUOpts & CPUOPT_FP287 ) {
        fpu = 2;
    } else if( CPUOpts & CPUOPT_FP387 ) {
        fpu = 3;
    } else if( CPUOpts & CPUOPT_FP5 ) {
        fpu = 3;
    } else if( CPUOpts & CPUOPT_FP6 ) {
        fpu = 3;
    } else if( CPUOpts & (CPUOPT_FPI | CPUOPT_FPI87) ) {
        // no level specified; use default
  #if _CPU == 8086
        fpu = 1;
  #else
        fpu = 3;
  #endif
    } else {
        fpu = 0;
    }
  #if _CPU == 8086
    fpu_emu = false;
    if( CPUOpts & CPUOPT_FPI ) {
        fpu_emu = true;
    }
    if( CPUOpts & CPUOPT_FPI87 ) {
        fpu_emu = false;
    }
    AsmInit( 0, cpu, fpu, fpu_emu );
  #else
    AsmInit( 1, cpu, fpu, false );
  #endif
#elif _CPU == _AXP || _CPU == _PPC
    AsmInit();
#endif

    DefaultLibs = NULL;
    AuxInfo = NULL;
    DependencyInfo = NULL;
#if _INTEL_CPU
  #if _CPU == 8086
    // Change auxiliary information for calls to run-time routines to match
    // the options used to compile the run-time routines
    if( CGOpts & CGOPT_M_LARGE ) {
        if( (CGOpts & CGOPT_WINDOWS) == 0 ) {
            HW_CTurnOff( IFXInfo.save, HW_DS );
            HW_CTurnOff( RtRtnInfo.save, HW_DS );
            HW_CTurnOff( RtStopInfo.save, HW_DS );
            HW_CTurnOff( RtVarInfo.save, HW_DS );
            HW_CTurnOff( CoRtnInfo.save, HW_DS );
            HW_CTurnOff( IFInfo.save, HW_DS );
            HW_CTurnOff( IFCharInfo.save, HW_DS );
            HW_CTurnOff( IFChar2Info.save, HW_DS );
            HW_CTurnOff( IFVarInfo.save, HW_DS );
        }
    }
    HW_CTurnOff( IFXInfo.save, HW_ES );
    HW_CTurnOff( RtRtnInfo.save, HW_ES );
    HW_CTurnOff( RtStopInfo.save, HW_ES );
    HW_CTurnOff( RtVarInfo.save, HW_ES );
    HW_CTurnOff( CoRtnInfo.save, HW_ES );
    HW_CTurnOff( IFInfo.save, HW_ES );
    HW_CTurnOff( IFCharInfo.save, HW_ES );
    HW_CTurnOff( IFChar2Info.save, HW_ES );
    HW_CTurnOff( IFVarInfo.save, HW_ES );
  #endif

    if( (CGOpts & CGOPT_SEG_REGS) == 0 ) {
        if( _FloatingDS( CGOpts ) ) {
            HW_CTurnOff( DefaultInfo.save, HW_DS );
        }
        if( _FloatingES( CGOpts ) ) {
            HW_CTurnOff( DefaultInfo.save, HW_ES );
        }
  #if _CPU == 8086
        if( CPUOpts & (CPUOPT_80386 | CPUOPT_80486 | CPUOPT_80586 | CPUOPT_80686) )
  #endif
        {
            if( _FloatingFS( CGOpts ) ) {
                HW_CTurnOff( DefaultInfo.save, HW_FS );
            }
            if( _FloatingGS( CGOpts ) ) {
                HW_CTurnOff( DefaultInfo.save, HW_GS );
            }
        }
    }
    if( OZOpts & OZOPT_O_FRAME ) {
        DefaultInfo.cclass |= GENERATE_STACK_FRAME;
    }
  #if _CPU != 8086
    if( CGOpts & CGOPT_STK_ARGS ) {
        DefaultInfo.cclass |= CALLER_POPS | NO_8087_RETURNS;
        DefaultInfo.parms = StackParms;
        HW_CTurnOff( DefaultInfo.save, HW_EAX );
        HW_CTurnOff( DefaultInfo.save, HW_EDX );
        HW_CTurnOff( DefaultInfo.save, HW_ECX );
        HW_CTurnOff( DefaultInfo.save, HW_FLTS );

        IFXInfo.cclass |= CALLER_POPS | NO_8087_RETURNS;
        IFXInfo.parms = StackParms;

        HW_CTurnOff( IFXInfo.save, HW_FLTS );
        HW_CTurnOff( RtRtnInfo.save, HW_FLTS );
        HW_CTurnOff( RtStopInfo.save, HW_FLTS );
        HW_CTurnOff( RtVarInfo.save, HW_FLTS );
        HW_CTurnOff( CoRtnInfo.save, HW_FLTS );
        HW_CTurnOff( IFInfo.save, HW_FLTS );
        HW_CTurnOff( IFCharInfo.save, HW_FLTS );
        HW_CTurnOff( IFChar2Info.save, HW_FLTS );
        HW_CTurnOff( IFVarInfo.save, HW_FLTS );
    }
  #endif
  #if _CPU == 8086
    if( CGOpts & CGOPT_WINDOWS ) {
        DefaultInfo.parms = WinParms;
        IFXInfo.parms = WinParms;
    }
  #endif
#endif

    FortranInfo = DefaultInfo;
    ProgramInfo = DefaultInfo;
#if _INTEL_CPU
  #if _CPU != 8086
    DoPragma( __Syscall );
    DoPragma( __Stdcall );
  #endif
    DoPragma( __Pascal );
    DoPragma( __Cdecl );
#endif
}


static void FreeArgList( aux_info *aux )
//======================================
{
    FreeChain( &aux->arg_info );
}


static void FreeAuxElements( aux_info *aux )
//==========================================
{
    FreeArgList( aux );
    if( aux->parms != DefaultInfo.parms ) {
        FMemFree( aux->parms );
        aux->parms = DefaultInfo.parms;
    }
    if( aux->code != DefaultInfo.code ) {
        FMemFree( aux->code );
        aux->code = DefaultInfo.code;
    }
    if( aux->objname != DefaultInfo.objname ) {
        FMemFree( aux->objname );
        aux->objname = DefaultInfo.objname;
    }
}


static void FreeAuxEntry( aux_info *aux )
//=======================================
{
    FreeAuxElements( aux );
    FMemFree( aux );
}


void FiniAuxInfo( void )
//======================
{
    void        *next;

    while( AuxInfo != NULL ) {
        next = AuxInfo->link;
        FreeAuxEntry( AuxInfo );
        AuxInfo = next;
    }
    FreeAuxElements( &FortranInfo );
    FreeChain( &DefaultLibs );
    // in case of fatal error, FiniAuxInfo() is called
    // from TDPurge()
#if _INTEL_CPU
    FreeChain( &ArrayInfo );
#endif
    FreeChain( &DependencyInfo );
    AsmSymFini();
}


void SubAuxInit( void )
//=====================
{
#if _INTEL_CPU
// Initialize aux information for a subprogram.

    ArrayInfo = NULL;
#endif
}


void SubAuxFini( void )
//=====================
{
#if _INTEL_CPU
// Finalize aux information for a subprogram.

    arr_info    *next;
    sym_id      arr;

    while( ArrayInfo != NULL ) {
        next = ArrayInfo->link;
        arr = SymFind( ArrayInfo->arr, ArrayInfo->len );
        if( ( arr != NULL ) && ( arr->u.ns.flags & SY_SUBSCRIPTED ) &&
            ( arr->u.ns.u1.s.typ != FT_CHAR ) &&
            ( ( arr->u.ns.flags & SY_SUB_PARM ) || _Allocatable( arr ) ) ) {
            arr->u.ns.si.va.u.dim_ext->dim_flags |= DIM_EXTENDED;
        }
        FMemFree( ArrayInfo );
        ArrayInfo = next;
    }
#endif
}

#if _INTEL_CPU
static bool cmp_ucased( const char *p1, const char *p2, uint len )
{
    uint    i;

    for( i = 0; i < len; i++ ) {
        if( *p1++ != toupper( *p2++ ) ) {
            return( false );
        }
    }
    return( true );
}

static void AddArrayInfo( const char *arr_name, uint arr_len )
//============================================================
// Process aux information for an array.
{
    arr_info    **arr;
    arr_info    *new_arr;
    uint        i;

    for( arr = &ArrayInfo; *arr != NULL; arr = &(*arr)->link ) {
        if( (*arr)->len != arr_len )
            continue;
        if( cmp_ucased( (*arr)->arr, arr_name, arr_len ) ) {
            return;
        }
    }
    new_arr = FMemAlloc( sizeof( arr_info ) + arr_len );
    new_arr->link = NULL;
    for( i = 0; i < arr_len; i++ ) {
        new_arr->arr[i] = toupper( arr_name[i] );
    }
    new_arr->arr[arr_len] = NULLCHAR;
    new_arr->len = arr_len;
    *arr = new_arr;
}
#endif

void AddDependencyInfo( source_t *fi )
//====================================
// Add dependency information for an included file.
{
    char        *p;
    dep_info    **dep;
    dep_info    *new_dep;
    struct stat stat_info;
    char        buff[_MAX_PATH];

    p = _fullpath( buff, fi->name, _MAX_PATH );
    if( p != NULL ) {
        for( dep = &DependencyInfo; *dep != NULL; dep = &(*dep)->link ) {
            if( strcmp( (*dep)->fn, p ) == 0 ) {
                return;
            }
        }
        if( stat( p, &stat_info ) != -1 ) {
            new_dep = FMemAlloc( sizeof( dep_info ) + strlen( p ) );
            new_dep->link = NULL;
            strcpy( new_dep->fn, p );
            new_dep->time_stamp = stat_info.st_mtime;
            *dep = new_dep;
        }
    }
}


static void AddDefaultLib( const char *lib_ptr, int lib_len, char priority )
//==========================================================================
{
    default_lib         **lib;
    default_lib         *new_lib;

    if( (Options & OPT_DFLT_LIB) == 0 )
        return;
    if( ( *lib_ptr == '"' ) && ( lib_ptr[lib_len - 1] == '"' ) ) {
        lib_len -= 2;
        ++lib_ptr;
    }
    for( lib = &DefaultLibs; *lib != NULL; lib = &(*lib)->link ) {
        if( strlen( &(*lib)->lib[1] ) != lib_len )
            continue;
        if( memcmp( &(*lib)->lib[1], lib_ptr, lib_len ) == 0 ) {
            return;
        }
    }
    new_lib = FMemAlloc( sizeof( default_lib ) + lib_len );
    new_lib->link = NULL;
    new_lib->lib[0] = priority;
    memcpy( &new_lib->lib[1], lib_ptr, lib_len );
    new_lib->lib[1+lib_len] = NULLCHAR;
    *lib = new_lib;
}


void DefaultLibInfo( void )
//=========================
{
#if _CPU == 386
    if( CGOpts & CGOPT_STK_ARGS ) {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flibs", '1' );
            AddDefaultLibConst( "math3s", '1' );
        } else {
            AddDefaultLibConst( "flib7s", '1' );
            AddDefaultLibConst( "math387s", '1' );
        }
        AddDefaultLibConst( "clib3s", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresfs", '1' );
        }
    } else {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flib", '1' );
            AddDefaultLibConst( "math3r", '1' );
        } else {
            AddDefaultLibConst( "flib7", '1' );
            AddDefaultLibConst( "math387r", '1' );
        }
        AddDefaultLibConst( "clib3r", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresf", '1' );
        }
    }
    if( CPUOpts & CPUOPT_FPI ) {
        AddDefaultLibConst( "emu387", '1' );
    } else if( CPUOpts & CPUOPT_FPI87 ) {
        AddDefaultLibConst( "noemu387", '1' );
    }
#elif _CPU == 8086
    if( CGOpts & CGOPT_M_MEDIUM ) {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flibm", '1' );
            AddDefaultLibConst( "mathm", '1' );
        } else {
            AddDefaultLibConst( "flib7m", '1' );
            AddDefaultLibConst( "math87m", '1' );
        }
        AddDefaultLibConst( "clibm", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresm", '1' );
        }
    } else {
        if( CPUOpts & CPUOPT_FPC ) {
            AddDefaultLibConst( "flibl", '1' );
            AddDefaultLibConst( "mathl", '1' );
        } else {
            AddDefaultLibConst( "flib7l", '1' );
            AddDefaultLibConst( "math87l", '1' );
        }
        AddDefaultLibConst( "clibl", '1' );
        if( Options & OPT_RESOURCES ) {
            AddDefaultLibConst( "wresl", '1' );
        }
    }
    if( CPUOpts & CPUOPT_FPI ) {
        AddDefaultLibConst( "emu87", '1' );
    } else if( CPUOpts & CPUOPT_FPI87 ) {
        AddDefaultLibConst( "noemu87", '1' );
    }
#elif _CPU == _AXP
    AddDefaultLibConst( "flib", '1' );
    AddDefaultLibConst( "math", '1' );
    AddDefaultLibConst( "clib", '1' );
    if( Options & OPT_RESOURCES ) {
        AddDefaultLibConst( "wresaxp", '1' );
    }
#elif _CPU == _PPC
    AddDefaultLibConst( "flib", '1' );
    AddDefaultLibConst( "math", '1' );
    AddDefaultLibConst( "clib", '1' );
    if( Options & OPT_RESOURCES ) {
        AddDefaultLibConst( "wresppc", '1' );
    }
#endif
}


aux_info *NewAuxEntry( const char *name, size_t name_len )
//========================================================
{
    aux_info    *aux;

    aux = FMemAlloc( sizeof( aux_info ) + name_len );
    aux->sym_len = name_len;
    memcpy( aux->sym_name, name, name_len );
    aux->sym_name[name_len] = NULLCHAR;
    aux->link = AuxInfo;
    aux->parms = DefaultInfo.parms;
    aux->code = DefaultInfo.code;
    aux->objname = DefaultInfo.objname;
    aux->arg_info = NULL;
    AuxInfo = aux;
    return( aux );
}


static bool CurrToken( const char *tok )
//======================================
{
    const char  *ptr;

    ptr = TokStart;
    for(;;) {
        if( ptr == TokEnd )
            break;
        if( toupper( *ptr ) != *tok )
            break;
        ptr++;
        tok++;
    }
    if( ( ptr == TokEnd ) && ( *tok == '\0' ) )
        return( true );
    return( false );
}


static void ScanToken( void )
//===========================
{
    const char  *ptr;
    bool        found_token;
    bool        first;

    ptr = TokEnd;
    ptr = SkipBlanks( ptr );
    TokStart = ptr;
    first = true;
    found_token = false;
    for(;;) {
        switch( *ptr ) {
        case ' ' :
        case '\t' :
        case '\0' :
            found_token = true;
            break;
        case '"' :
            if( first ) {
                for(;;) {
                    ++ptr;
                    if( *ptr == '\0' )
                        break;
                    if( *ptr == '"' ) {
                        ++ptr;
                        break;
                    }
                }
            }
            found_token = true;
            break;
        case '[' :
        case ']' :
        case '(' :
        case ')' :
        case ',' :
        case '=' :
        case '*' :
            if( first ) {
                ++ptr;
            }
            found_token = true;
            break;
        case '\\' :
            if( first ) {
                ReadSrc();
                ptr = SrcBuff;
                if( ( *ptr == '*' ) || ( *ptr == 'C' ) || ( *ptr == 'c' ) ) {
                    ++ptr;
                } else {
                    Error( PR_BAD_CONTINUATION );
                    CSuicide();
                }
                ptr = SkipBlanks( ptr );
                TokStart = ptr;
            } else {
                found_token = true;
            }
            break;
        default :
            first = false;
            ptr++;
        }
        if( found_token ) {
            break;
        }
    }
    TokEnd = ptr;
}

static bool RecToken( const char *tok )
//=====================================
{
    if( CurrToken( tok ) ) {
        ScanToken();
        return( true );
    }
    return( false );
}

static void ReqToken( const char *tok )
//=====================================
{
    size_t      len;
    char        buffer[ERR_BUFF_SIZE + 1];

    if( !RecToken( tok ) ) {
        len = TokEnd - TokStart;
        memcpy( buffer, TokStart, len );
        buffer[len] = NULLCHAR;
        Error( PR_BAD_SYNTAX, tok, buffer );
        CSuicide();
    }
}


static void ScanFnToken( void )
//=============================
{
    const char  *ptr;
    bool        found_token;
    bool        first;

    ptr = TokEnd;
    ptr = SkipBlanks( ptr );
    TokStart = ptr;
    first = true;
    for( found_token = false; !found_token; ) {
        switch( *ptr ) {
        case ' ' :
        case '\t' :
        case '\0' :
            found_token = true;
            break;
        case '"' :
            if( first ) {
                for(;;) {
                    ++ptr;
                    if( *ptr == '\0' )
                        break;
                    if( *ptr == '"' ) {
                        ++ptr;
                        break;
                    }
                }
            }
            found_token = true;
            break;
        default :
            first = false;
            ptr++;
        }
    }
    TokEnd = ptr;
}


static bool RecFnToken( const char *tok )
//=======================================
{
    if( CurrToken( tok ) ) {
        ScanFnToken();
        return( true );
    }
    return( false );
}



static void SymbolId( void )
//==========================
{
    const char  *ptr;

    ptr = TokStart;
    if( ( isalpha( *ptr ) == 0 ) && ( *ptr != '$' ) && ( *ptr != '_' ) ) {
        Error( PR_SYMBOL_NAME );
        CSuicide();
    }
    for(;;) {
        ptr++;
        if( ptr == TokEnd )
            break;
        if( ( isalnum( *ptr ) == 0 ) && ( *ptr != '$' ) && ( *ptr != '_' ) ) {
            Error( PR_SYMBOL_NAME );
            CSuicide();
        }
    }
}


static void AliasName( void )
//===========================
{
    aux_info    *alias;

    SymbolId();
    alias = AuxLookupName( TokStart, TokEnd - TokStart );
    if( alias != NULL ) {
        AliasInfo = alias;
    }
    ScanToken();
}


static void SymbolName( void )
//============================
{
    if( RecToken( "DEFAULT" ) ) {
        SymLen = 0;
    } else {
        SymbolId();
        SymLen = TokEnd - TokStart;
        if( SymLen > MAX_SYMLEN ) {
            SymLen = MAX_SYMLEN;
        }
        memcpy( SymName, TokStart, SymLen );
        ScanToken();
    }
}


static void ObjectName( void )
//============================
{
    int         obj_len;
    char        *name;

    if( *TokStart != '"' )
        return;
    if( TokStart == TokEnd - 1 )
        CSuicide();
    if( *(TokEnd - 1) != '"' )
        CSuicide();
    obj_len = TokEnd - TokStart - 2;
    name = FMemAlloc( obj_len + 1 );
    if( CurrAux->objname != DefaultInfo.objname ) {
        FMemFree( CurrAux->objname );
    }
    memcpy( name, TokStart + 1, obj_len );
    name[obj_len] = NULLCHAR;
    CurrAux->objname = name;
    ScanToken();
}


static void DupParmInfo( aux_info *dst, aux_info *src )
//=====================================================
{
    hw_reg_set  *new_reg_set;
    hw_reg_set  *reg_set;
    int         size;

    reg_set = src->parms;
    size = 0;
    while( !HW_CEqual( reg_set[size], HW_EMPTY ) ) {
        ++size;
    }
    ++size;
    new_reg_set = FMemAlloc( size * sizeof( hw_reg_set ) );
    memcpy( new_reg_set, reg_set, size * sizeof( hw_reg_set ) );
    dst->parms = new_reg_set;
}


static void DupCallBytes( aux_info *dst, aux_info *src )
//======================================================
{
    byte_seq        *new_seq;
    byte_seq_len    seq_len;

    seq_len = src->code->length;
    new_seq = FMemAlloc( offsetof( byte_seq, data ) + seq_len );
    memcpy( new_seq->data, src->code->data, seq_len );
    dst->code = new_seq;
    dst->code->length = src->code->length;

#if _INTEL_CPU

    dst->code->relocs = src->code->relocs;

#elif _CPU == _AXP || _CPU == _PPC

    byte_seq_reloc      **lnk;
    byte_seq_reloc      *new;
    byte_seq_reloc      *head;
    byte_seq_reloc      *reloc;

    head = NULL;
    lnk = &head;
    for( reloc = src->code->relocs; reloc; reloc = reloc->next ) {
        new = FMemAlloc( sizeof( byte_seq_reloc ) );
        new->off = reloc->off;
        new->type = reloc->type;
        new->sym = reloc->sym;
        new->next = NULL;
        *lnk = new;
        lnk = &new->next;
    }
    dst->code->relocs = head;

#endif
}


static void DupObjectName( aux_info *dst, aux_info *src )
//=======================================================
{
    char        *new_name;

    new_name = FMemAlloc( strlen( src->objname ) + 1 );
    strcpy( new_name, src->objname );
    dst->objname = new_name;
}


static void DupArgInfo( aux_info *dst, aux_info *src )
//====================================================
{
    pass_by     *new_arg;
    pass_by     *args;
    pass_by     **curr_arg;

    args = src->arg_info;
    curr_arg = &dst->arg_info;
    while( args != NULL ) {
        new_arg = FMemAlloc( sizeof( pass_by ) );
        new_arg->info = args->info;
        new_arg->link = NULL;
        *curr_arg = new_arg;
        curr_arg = &new_arg->link;
        args = args->link;
    }
}


void CopyAuxInfo( aux_info *dst, aux_info *src )
//==============================================
{
    if( dst != src ) {
        dst->cclass = src->cclass;
        dst->save = src->save;
        dst->returns = src->returns;
        dst->streturn = src->streturn;
        if( src->parms != DefaultInfo.parms ) {
            DupParmInfo( dst, src );
        }
        if( src->code != DefaultInfo.code ) {
            DupCallBytes( dst, src );
        }
        if( src->objname != DefaultInfo.objname ) {
            DupObjectName( dst, src );
        }
        DupArgInfo( dst, src );
    }
}


static void ProcessAlias( void )
//==============================
{
    if( SymLen == 0 ) { // "DEFAULT"
        CurrAux = AliasInfo;
    } else {
        CurrAux = AuxLookupName( SymName, SymLen );
        if( CurrAux != AliasInfo ) { // Consider: c$pragma aux (sp) sp
            if( CurrAux == NULL ) {
                CurrAux = NewAuxEntry( SymName, SymLen );
            } else {
                FreeAuxElements( CurrAux );
            }
        }
        CopyAuxInfo( CurrAux, AliasInfo );
    }
}


void *AsmQuerySymbol( const char *name )
//======================================
{
    return( (void *)name );
}


enum sym_state AsmQueryState( void *handle )
//==========================================
{
    /* unused parameters */ (void)handle;

    return( SYM_UNDEFINED );
}


#if _INTEL_CPU
enum sym_type AsmQueryType( void *handle )
//========================================
{
    /* unused parameters */ (void)handle;

    return( SYM_INT1 );
}


static void InsertFixups( unsigned char *buff, size_t len )
//=========================================================
{
                        // additional slop in buffer to simplify the code
    unsigned char       temp[MAXIMUM_BYTESEQ + 2];
    struct asmfixup     *fix;
    struct asmfixup     *head;
    struct asmfixup     *chk;
    struct asmfixup     *next;
    struct asmfixup     **owner;
    byte                *dst;
    byte                *src;
    byte                *end;
    byte_seq            *seq;
    bool                perform_fixups;
    char                *name;

    perform_fixups = false;
    head = FixupHead;
    if( head != NULL ) {
        FixupHead = NULL;
        // sort the fixup list in increasing fixup_loc's
        for( fix = head; fix != NULL; fix = next ) {
            owner = &FixupHead;
            for( ;; ) {
                chk = *owner;
                if( chk == NULL )
                    break;
                if( chk->fixup_loc > fix->fixup_loc )
                    break;
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
        // insert fixup escape sequences
        while( src < end ) {
            if( (fix != NULL) && (fix->fixup_loc == (src - buff)) ) {
                name = fix->name;
                // insert fixup information
                *dst++ = FLOATING_FIXUP_BYTE;
                // only expect a 'fixup_type' of FIX_SEG
                switch( fix->fixup_type ) {
                case FIX_FPPATCH:
                    *dst++ = fix->u_fppatch;
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
                        break;
                    }
                default:
                    Error( PR_SYMBOL_UNRESOLVED, name );
                    return;
                }
                head = fix;
                fix = fix->next;
                if( head->external ) {
                    *owner = fix;
                    if( head->name != NULL ) {
                        FMemFree( head->name );
                    }
                    FMemFree( head );
                } else {
                    owner = &head->next;
                }
            } else {
                if( *src == FLOATING_FIXUP_BYTE ) {
                    *dst++ = FLOATING_FIXUP_BYTE;
                }
                *dst++ = *src++;
            }
            if( dst > &temp[MAXIMUM_BYTESEQ] ) {
                Error( PR_BYTE_SEQ_LIMIT );
                CSuicide();
            }
        }
        buff = temp;
        len = dst - temp;
        perform_fixups = true;
    }
    seq = FMemAlloc( offsetof( byte_seq, data ) + len );
    seq->relocs = perform_fixups;
    seq->length = len;
    memcpy( &seq->data, buff, len );
    if( CurrAux->code != DefaultInfo.code ) {
        FMemFree( CurrAux->code );
    }
    CurrAux->code = seq;
}

#elif _CPU == _AXP || _CPU == _PPC

uint_32 AsmQuerySPOffsetOf( void *handle )
//========================================
{
    handle = handle;
    return( 0 );
}


static void InsertFixups( unsigned char *buff, size_t len )
//=========================================================
{
    byte_seq            *seq;
    asmreloc            *reloc;
    byte_seq_reloc      *head;
    byte_seq_reloc      *new;
    byte_seq_reloc      **lnk;

    head = NULL;
    lnk = &head;
    for( reloc = AsmRelocs; reloc; reloc = reloc->next ) {
        new = FMemAlloc( sizeof( byte_seq_reloc ) );
        new->off = reloc->offset;
        new->type = reloc->type;
        new->sym = (void *)SymFind( reloc->name, strlen( reloc->name ) );
        new->next = NULL;
        *lnk = new;
        lnk = &new->next;
    }

    seq = FMemAlloc( offsetof( byte_seq, data ) + len );
    seq->relocs = head;
    seq->length = len;
    memcpy( &seq->data, buff, len );
    if( CurrAux->code != DefaultInfo.code ) {
        FMemFree( CurrAux->code );
    }
    CurrAux->code = seq;
}
#endif

#if _CPU == 8086

static void AddAFix( size_t i, char *name, unsigned type, unsigned off )
//========================================================================
{
    struct asmfixup     *fix;

    fix = FMemAlloc( sizeof( *fix ) );
    fix->external = true;
    fix->fixup_loc = i;
    fix->name = name;
    fix->u_offset = off;
    fix->fixup_type = type;
    fix->next = FixupHead;
    FixupHead = fix;
}

#endif


static void GetByteSeq( void )
//============================
{
    size_t          seq_len;
    size_t          len;
    const char      *ptr;
    byte            buff[MAXIMUM_BYTESEQ + 32]; // extra for assembler
#if _CPU == 8086
    bool            use_fpu_emu = false;
#endif

    seq_len = 0;
#if _INTEL_CPU
    AsmSaveCPUInfo();
#endif
    for(;;) {
        if( *TokStart == '"' ) {
            if( TokStart == TokEnd - 1 )
                CSuicide();
            if( *(TokEnd - 1) != '"' )
                CSuicide();
            *(char *)(TokEnd - 1) = NULLCHAR;
            AsmCodeAddress = (byte_seq_len)seq_len;
            AsmCodeLimit = MAXIMUM_BYTESEQ;
            AsmCodeBuffer = buff;
#if _INTEL_CPU
  #if _CPU == 8086
            AsmLine( TokStart + 1, use_fpu_emu );
            use_fpu_emu = false;
  #else
            AsmLine( TokStart + 1, false );
  #endif
#else
            AsmLine( TokStart + 1 );
#endif
            if( AsmCodeAddress <= MAXIMUM_BYTESEQ ) {
                seq_len = AsmCodeAddress;
            } else {
                Error( PR_BYTE_SEQ_LIMIT );
                CSuicide();
            }
            ScanToken();
        } else if( RecToken( "FLOAT" ) ) {
#if _CPU == 8086
            if( CPUOpts & CPUOPT_FPI ) {
                use_fpu_emu = true;
            }
#endif
        } else {
#if _CPU == 8086
            if( use_fpu_emu ) {
                AddAFix( seq_len, NULL, FIX_SEG, 0 );
                use_fpu_emu = false;
            }
#endif
            ptr = TokStart;
            if( ( *ptr != 'Z' ) && ( *ptr != 'z' ) )
                break;
            ++ptr;
            len = MkHexConst( ptr, NULL, TokEnd - TokStart - 1 );
            if( len == 0 ) {
                Error( PR_BAD_BYTE_SEQ );
                CSuicide();
            }
            if( seq_len + len > MAXIMUM_BYTESEQ ) {
                Error( PR_BYTE_SEQ_LIMIT );
                CSuicide();
            }
            MkHexConst( ptr, (char *)( buff + seq_len ), TokEnd - TokStart - 1 );
            seq_len += len;
            ScanToken();
        }
    }
    InsertFixups( buff, seq_len );
    AsmSymFini();
#if _INTEL_CPU
    AsmRestoreCPUInfo();
#endif
}


#if _INTEL_CPU

#define REGNAME_MAX_LEN     4

static hw_reg_set RegSet( void )
//==============================
{
    hw_reg_set  reg_set;
    int         reg;
    char        regname_buf[REGNAME_MAX_LEN];
    int         i;

    HW_CAsgn( reg_set, HW_EMPTY );
    for(;;) {
        if( TokEnd - TokStart > REGNAME_MAX_LEN )
            break;
        for( i = 0; i < TokEnd - TokStart; i++ ) {
            regname_buf[i] = toupper( TokStart[i] );
        }
        reg = KwLookUp( RegNames, sizeof( RegNames ) / sizeof( RegNames[0] ) - 1, regname_buf, TokEnd - TokStart, true );
        if( reg == 0 )
            break;
        HW_TurnOn( reg_set, RegValue[reg] );
        ScanToken();
    }
    ReqToken( "]" );
    return( reg_set );
}


static hw_reg_set *RegSets( void )
//================================
{
    hw_reg_set  reg_sets[MAX_REG_SETS];
    hw_reg_set  *regs;
    int         num_sets;

    num_sets = 0;
    while( RecToken( "[" ) ) {
        reg_sets[num_sets] = RegSet();
        if( ( num_sets < MAX_REG_SETS ) &&
            !HW_CEqual( reg_sets[num_sets], HW_EMPTY ) ) {
            ++num_sets;
        }
    }
    HW_CAsgn( reg_sets[num_sets], HW_EMPTY );
    regs = FMemAlloc( ( num_sets + 1 ) * sizeof( hw_reg_set ) );
    memcpy( regs, reg_sets, ( num_sets + 1 ) * sizeof( hw_reg_set ) );
    return( regs );
}
#endif


static void GetArgList( void )
//============================
{
    pass_by     *arg;
    pass_by     **curr_arg;
    pass_info   arg_pass_info;

    FreeArgList( CurrAux );
    if( RecToken( ")" ) )
        return;
    curr_arg = &CurrAux->arg_info;
    for(;;) {
        arg_pass_info = 0;
        if( RecToken( "VALUE" ) ) {
            arg_pass_info |= PASS_BY_VALUE;
            if( RecToken( "*" ) ) {
                if( RecToken( "1" ) ) {
                    arg_pass_info |= ARG_SIZE_1;
                } else if( RecToken( "2" ) ) {
                    arg_pass_info |= ARG_SIZE_2;
                } else if( RecToken( "4" ) ) {
                    arg_pass_info |= ARG_SIZE_4;
                } else if( RecToken( "8" ) ) {
                    arg_pass_info |= ARG_SIZE_8;
                } else {
                    Error( PR_BAD_PARM_SIZE );
                    CSuicide();
                }
#if _INTEL_CPU
            } else if( RecToken( "FAR" ) ) {
                arg_pass_info |= ARG_FAR;
    #if ( _CPU == 8086 )
                arg_pass_info |= ARG_SIZE_2;
    #else
                arg_pass_info |= ARG_SIZE_4;
    #endif
            } else if( RecToken( "NEAR" ) ) {
                arg_pass_info |= ARG_NEAR;
    #if ( _CPU == 8086 )
                arg_pass_info |= ARG_SIZE_2;
    #else
                arg_pass_info |= ARG_SIZE_4;
    #endif
            } else {
    #if ( _CPU == 8086 )
                arg_pass_info |= ARG_SIZE_2;
    #else
                arg_pass_info |= ARG_SIZE_4;
    #endif
#endif
            }
        } else if( RecToken( "REFERENCE" ) ) {
            arg_pass_info |= PASS_BY_REFERENCE;
#if _INTEL_CPU
            if( RecToken( "FAR" ) ) {
                arg_pass_info |= ARG_FAR;
            } else if( RecToken( "NEAR" ) ) {
                arg_pass_info |= ARG_NEAR;
            }
#endif
            if( RecToken( "DESCRIPTOR" ) ) {
                arg_pass_info |= PASS_BY_DESCRIPTOR;
            } else if( RecToken( "NODESCRIPTOR" ) ) {
                arg_pass_info |= PASS_BY_NODESCRIPTOR;
            }
        } else if( RecToken( "DATA_REFERENCE" ) ) {
            arg_pass_info |= PASS_BY_DATA | PASS_BY_REFERENCE;
#if _INTEL_CPU
            if( RecToken( "FAR" ) ) {
                arg_pass_info |= ARG_FAR;
            } else if( RecToken( "NEAR" ) ) {
                arg_pass_info |= ARG_NEAR;
            }
#endif
        } else {
            Error( PR_BAD_PARM_ATTR );
            CSuicide();
        }
        arg = FMemAlloc( sizeof( pass_by ) );
        arg->link = NULL;
        arg->info = arg_pass_info;
        *curr_arg = arg;
        curr_arg = &arg->link;
        if( !RecToken( "," ) ) {
            break;
        }
    }
    ReqToken( ")" );
}


#if _INTEL_CPU
static void GetSTRetInfo( void )
//==============================
{
    struct {
        boolbit f_float        : 1;
        boolbit f_struct       : 1;
        boolbit f_allocs       : 1;
        boolbit f_list         : 1;
    } have;

    have.f_float  = false;
    have.f_struct = false;
    have.f_allocs = false;
    have.f_list   = false;
    for(;;) {
        if( !have.f_float && RecToken( "FLOAT" ) ) {
            CurrAux->cclass |= NO_FLOAT_REG_RETURNS;
            have.f_float = true;
        } else if( !have.f_struct && RecToken( "STRUCT" ) ) {
            CurrAux->cclass |= NO_STRUCT_REG_RETURNS;
            have.f_struct = true;
        } else if( !have.f_allocs && RecToken( "ROUTINE" ) ) {
            CurrAux->cclass |= ROUTINE_RETURN;
            have.f_allocs = true;
        } else if( !have.f_allocs && RecToken( "CALLER" ) ) {
            CurrAux->cclass &= ~ROUTINE_RETURN;
            have.f_allocs = true;
        } else if( !have.f_list && RecToken( "[" ) ) {
            CurrAux->cclass |= SPECIAL_STRUCT_RETURN;
            CurrAux->streturn = RegSet();
            have.f_list = true;
        } else {
            break;
        }
    }
}


static void GetRetInfo( void )
//============================
{
    struct {
        boolbit f_no8087        : 1;
        boolbit f_list          : 1;
        boolbit f_struct        : 1;
    } have;

    have.f_no8087  = false;
    have.f_list    = false;
    have.f_struct  = false;
    // "3s" default is NO_8087_RETURNS - turn off NO_8087_RETURNS
    // flag so that "3s" model programs can use 387 pragmas
    CurrAux->cclass &= ~NO_8087_RETURNS;
    for(;;) {
        if( !have.f_no8087 && RecToken( "NO8087" ) ) {
            CurrAux->cclass |= NO_8087_RETURNS;
            HW_CTurnOff( CurrAux->returns, HW_FLTS );
            have.f_no8087 = true;
        } else if( !have.f_list && RecToken( "[" ) ) {
            CurrAux->cclass |= SPECIAL_RETURN;
            CurrAux->returns = RegSet();
            have.f_list = true;
        } else if( !have.f_struct && RecToken( "STRUCT" ) ) {
            GetSTRetInfo();
            have.f_struct = true;
        } else {
            break;
        }
    }
}


static void GetSaveInfo( void )
//=============================
{
    hw_reg_set  modlist;
    hw_reg_set  default_flt_n_seg;
    hw_reg_set  flt_n_seg;

    struct {
        boolbit f_exact        : 1;
        boolbit f_nomemory     : 1;
        boolbit f_list         : 1;
    } have;

    have.f_exact    = false;
    have.f_nomemory = false;
    have.f_list     = false;
    for(;;) {
        if( !have.f_exact && RecToken( "EXACT" ) ) {
            CurrAux->cclass |= MODIFY_EXACT;
            have.f_exact = true;
        } else if( !have.f_nomemory && RecToken( "NOMEMORY" ) ) {
            CurrAux->cclass |= NO_MEMORY_CHANGED;
            have.f_nomemory = true;
        } else if( !have.f_list && RecToken( "[" ) ) {
            modlist = RegSet();
            have.f_list = true;
        } else {
            break;
        }
    }
    if( have.f_list ) {
        HW_Asgn( default_flt_n_seg, DefaultInfo.save );
        HW_CTurnOn( CurrAux->save, HW_FULL );
        if( !have.f_exact && (CGOpts & CGOPT_SEG_REGS) == 0 ) {
            HW_CAsgn( flt_n_seg, HW_FLTS );
            HW_CTurnOn( flt_n_seg, HW_SEGS );
            HW_TurnOff( CurrAux->save, flt_n_seg );
            HW_OnlyOn( default_flt_n_seg, flt_n_seg );
            HW_TurnOn( CurrAux->save, default_flt_n_seg );
        }
        HW_TurnOff( CurrAux->save, modlist );
    }
}
#endif


static void GetParmInfo( void )
//=============================
// Collect argument information.
{
    struct {
#if _INTEL_CPU
        boolbit f_pop           : 1;
        boolbit f_reverse       : 1;
        boolbit f_loadds        : 1;
        boolbit f_nomemory      : 1;
        boolbit f_list          : 1;
#endif
        boolbit f_args          : 1;
    } have;

#if _INTEL_CPU
    have.f_pop           = false;
    have.f_reverse       = false;
    have.f_loadds        = false;
    have.f_nomemory      = false;
    have.f_list          = false;
#endif
    have.f_args          = false;
    for(;;) {
        if( !have.f_args && RecToken( "(" ) ) {
            GetArgList();
            have.f_args = true;
#if _INTEL_CPU
        } else if( !have.f_pop && RecToken( "CALLER" ) ) {
            CurrAux->cclass |= CALLER_POPS;
            have.f_pop = true;
        } else if( !have.f_pop && RecToken( "ROUTINE" ) ) {
            CurrAux->cclass &= ~CALLER_POPS;
            have.f_pop = true;
        } else if( !have.f_reverse && RecToken( "REVERSE" ) ) {
            // arguments are processed in reverse order by default
            CurrAux->cclass |= REVERSE_PARMS;
            have.f_reverse = true;
        } else if( !have.f_nomemory && RecToken( "NOMEMORY" ) ) {
            CurrAux->cclass |= NO_MEMORY_READ;
            have.f_nomemory = true;
        } else if( !have.f_loadds && RecToken( "LOADDS" ) ) {
            CurrAux->cclass |= LOAD_DS_ON_CALL;
            have.f_loadds = true;
        } else if( !have.f_list && CurrToken( "[" ) ) {
            if( CurrAux->parms != DefaultInfo.parms ) {
                FMemFree( CurrAux->parms );
            }
            CurrAux->parms = RegSets();
            have.f_list = true;
#endif
        } else {
            break;
        }
    }
}


static void     Pragma( void )
//============================
// Process a pragma.
{
#if _INTEL_CPU
    const char  *arr;
    uint        arr_len;
#endif

    struct {
        boolbit f_far    : 1;
        boolbit f_far16  : 1;
        boolbit f_loadds : 1;
        boolbit f_export : 1;
        boolbit f_parm   : 1;
        boolbit f_value  : 1;
        boolbit f_modify : 1;
    } have;

    if( RecFnToken( "LIBRARY" ) ) {
        if( RecFnToken( "\0" ) ) {
            DefaultLibInfo();
        } else {
            while( !RecFnToken( "\0" ) ) {
                AddDefaultLib( TokStart, TokEnd - TokStart, '9' );
                ScanFnToken();
            }
        }
#if _INTEL_CPU
    } else if( RecToken( "ARRAY" ) ) {
        SymbolId();
        arr = TokStart;
        arr_len = TokEnd - TokStart;
        ScanToken();
        if( RecToken( "FAR" ) ) {
            if( _SmallDataModel( CGOpts ) ) {
                AddArrayInfo( arr, arr_len );
            }
  #if _CPU == 8086
        } else if( RecToken( "HUGE" ) ) {
            if( CGOpts & CGOPT_M_LARGE ) {
                AddArrayInfo( arr, arr_len );
            }
  #endif
        }
#endif
    } else {
        AliasInfo = &FortranInfo;
        if( RecToken( "LINKAGE" ) ) {
            ReqToken( "(" );
            SymbolName();
            ReqToken( "," );
            AliasName();
            ProcessAlias();
            ReqToken( ")" );
        } else {
            ReqToken( "AUX" );
            if( RecToken( "(" ) ) {
                AliasName();
                ReqToken( ")" );
            }
            SymbolName();
            ProcessAlias();
            ObjectName();

            have.f_far    = false;
            have.f_loadds = false;
            have.f_export = false;
            have.f_value  = false;
            have.f_modify = false;
            have.f_parm   = false;
            for( ;; ) {
                if( !have.f_parm && RecToken( "PARM" ) ) {
                    GetParmInfo();
                    have.f_parm = true;
                } else if( !have.f_far && RecToken( "=" ) ) {
                    GetByteSeq();
#if _INTEL_CPU
                    have.f_far = true;
                } else if( !have.f_far && RecToken( "FAR" ) ) {
                    CurrAux->cclass |= FAR_CALL;
                    have.f_far = true;
#if _CPU == 386
                } else if( !have.f_far16 && RecToken( "FAR16" ) ) {
                    CurrAux->cclass |= FAR16_CALL;
                    have.f_far16 = true;
#endif
                } else if( !have.f_far && RecToken( "NEAR" ) ) {
                    CurrAux->cclass &= ~FAR_CALL;
                    have.f_far = true;
                } else if( !have.f_loadds && RecToken( "LOADDS" ) ) {
                    CurrAux->cclass |= LOAD_DS_ON_ENTRY;
                    have.f_loadds = true;
#endif
                } else if( !have.f_export && RecToken( "EXPORT" ) ) {
                    CurrAux->cclass |= DLL_EXPORT;
                    have.f_export = true;
#if _INTEL_CPU
                } else if( !have.f_value && RecToken( "VALUE" ) ) {
                    GetRetInfo();
                    have.f_value = true;
#endif
                } else if( !have.f_value && RecToken( "ABORTS" ) ) {
                    CurrAux->cclass |= SUICIDAL;
                    have.f_value = true;
#if _INTEL_CPU
                } else if( !have.f_modify && RecToken( "MODIFY" ) ) {
                    GetSaveInfo();
                    have.f_modify = true;
#endif
                } else {
                    break;
                }
            }
        }
    }
}


void DoPragma( const char *ptr )
//==============================
{
    int         status;

    TokStart = ptr;
    TokEnd = ptr;
    ScanToken();
    for(;;) {
        status = CSpawn( &Pragma );
        if( status != 0 ) {
            if( ProgSw & PS_FATAL_ERROR ) {
                CSuicide();
            }
            AsmSymFini();
            break;
        }
        if( RecToken( "\0" ) ) {
            break;
        }
    }
}


void ProcPragma( const char *ptr )
//================================
{
    // don't process auxiliary pragma's until pass 2
    if( ProgSw & PS_DONT_GENERATE )
        return;
    DoPragma( ptr );
}
