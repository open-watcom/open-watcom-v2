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
#include "linkstd.h"
#include "alloc.h"
#include "command.h"
#include "exeos2.h"
#include "exepe.h"
#include "loados2.h"
#include "loadpe.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "objfree.h"
#include "cmdline.h"
#include "fileio.h"
#include "impexp.h"
#include "objpass1.h"

static bool             GetWlibImports( void );
static bool             getimport( void );
static bool             getexport( void );
static bool             getsegflags( void );

extern bool ProcOS2Import( void )
/*******************************/
{
    return( ProcArgList( &getimport, 0 ) );
}

extern bool ProcOS2Export( void )
/*******************************/
{
    bool    retval;

    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        retval = GetWlibImports();
    } else {
        retval =  ProcArgList( &getexport, 0 );
    }
    return( retval );
}

extern bool ProcAnonExport( void )
/********************************/
{
    bool    retval;

    CmdFlags |= CF_ANON_EXPORT;
    retval = ProcOS2Export();
    CmdFlags &= ~CF_ANON_EXPORT;
    return retval;
}

extern bool ProcOS2Segment( void )
/********************************/
{
    return( ProcArgList( &getsegflags, TOK_INCLUDE_DOT ) );
}

static bool GetWlibImports( void )
/********************************/
/* read in a wlib command file, get the import directives, and treat them
 * as exports (hey man, GO asked for it ...... ) */
{
    char *          fname;
    char *          symname;
    char *          internal;
    f_handle        handle;
    unsigned_16     ordinal;
    entry_export *  exp;

    fname = FileName( Token.this, Token.len, E_LBC, FALSE );
    handle = QOpenR( fname );
    SetCommandFile( handle, fname );
    Token.locked = TRUE;      /* make sure only this file parsed */
    while( GetToken( SEP_SPACE, 0 ) ) {
        if( Token.len <= 2 ) continue;
        if( (Token.this[0] == '+') && (Token.this[1] == '+') ) {
            Token.this += 2;
            Token.len -= 2;
            if( Token.this[0] == '\'' ) {
                Token.thumb = REJECT;
                if( !GetToken( SEP_QUOTE, 0 ) ) {
                    LnkMsg( LOC+LINE+ERR+MSG_BAD_WLIB_IMPORT, NULL );
                    RestoreCmdLine();   /* get rid of this file */
                    return( TRUE );
                }
            }
            symname = tostring();
            internal = NULL;
            if( !GetToken( SEP_DOT_EXT, 0 ) ) {
                LnkMsg( LOC+LINE+ERR+MSG_BAD_WLIB_IMPORT, NULL );
                _LnkFree( symname );
                RestoreCmdLine();       /* get rid of this file */
                return( TRUE );
            }
            ordinal = 0;
            if( GetToken( SEP_DOT_EXT, 0 ) ) {
                if( getatoi( &ordinal ) != ST_IS_ORDINAL ) {
                    if( Token.len > 0 ) {
                        internal = symname;
                        symname = tostring();
                    }
                    if( GetToken( SEP_DOT_EXT, 0 )
                        && getatoi( &ordinal ) != ST_IS_ORDINAL ) {
                        if( GetToken( SEP_DOT_EXT, 0 ) ) {
                            getatoi( &ordinal );
                        }
                    }
                }
            }
            exp = AllocExport( symname, strlen(symname) );
            exp->isanonymous = (CmdFlags & CF_ANON_EXPORT) != 0;
            if( internal != NULL ) {
                exp->sym = RefISymbol( internal );
                _LnkFree( internal );
            } else {
                exp->sym = RefISymbol( symname );
            }
            exp->sym->info |= SYM_DCE_REF;      // make sure it isn't removed
            exp->ordinal = ordinal;
            if( ordinal == 0 ) {
                exp->isresident = TRUE;   // no ord spec'd so must be resident
            }
            AddToExportList( exp );
        }
    }
    Token.locked = FALSE;
    return( TRUE );
}

static bool getimport( void )
/***************************/
{
    length_name         intname;
    length_name         modname;
    length_name         extname;
    unsigned_16         ordinal;
    ord_state           state;

    intname.name = tostring();
    intname.len = strlen( intname.name );
    if( !GetToken( SEP_NO, 0 ) ) {
        _LnkFree( intname.name );
        return( FALSE );
    }
    modname.name = tostring();
    modname.len = strlen( modname.name );
    state = ST_INVALID_ORDINAL;   // assume to extname or ordinal.
    if( GetToken( SEP_PERIOD, TOK_INCLUDE_DOT ) ) {
        state =  getatoi( &ordinal );
        if( state == ST_NOT_ORDINAL ) {
            extname.name = tostring();
            extname.len = strlen( extname.name );
        } else if( state == ST_INVALID_ORDINAL ) {
            LnkMsg( LOC+LINE+MSG_IMPORT_ORD_INVALID + ERR, NULL );
            _LnkFree( intname.name );
            _LnkFree( modname.name );
            return( TRUE );
        }
    }
    if( state == ST_IS_ORDINAL ) {
        HandleImport( &intname, &modname, &intname, ordinal );
    } else {
        if( state == ST_NOT_ORDINAL ) {
            HandleImport( &intname, &modname, &extname, NOT_IMP_BY_ORDINAL );
            _LnkFree( extname.name );
        } else {
            HandleImport( &intname, &modname, &intname, NOT_IMP_BY_ORDINAL );
        }
    }
    _LnkFree( intname.name );
    _LnkFree( modname.name );
    return( TRUE );
}

static bool getexport( void )
/***************************/
{
    entry_export *  exp;
    unsigned_16     value;

    exp = AllocExport( Token.this, Token.len );
    exp->isanonymous = (CmdFlags & CF_ANON_EXPORT) != 0;
    if( GetToken( SEP_PERIOD, TOK_INCLUDE_DOT ) ) {
        if( getatol( &exp->ordinal ) != ST_IS_ORDINAL ) {
            LnkMsg( LOC+LINE+ERR + MSG_EXPORT_ORD_INVALID, NULL );
            _LnkFree( exp );
            GetToken( SEP_EQUALS, TOK_INCLUDE_DOT );
            return( TRUE );
        }
    }
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        exp->sym = SymXOp( ST_CREATE|ST_REFERENCE, Token.this, Token.len );
        if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
            exp->impname = tostring();
        }
    } else {
        exp->sym = RefISymbol( exp->name );
    }
    exp->sym->info |= SYM_DCE_REF;      //make sure it is not removed
    if( exp->ordinal == 0 ) {
        exp->isresident = TRUE;   // no ordinal spec'd so must be kept resident
    }
    exp->next = FmtData.u.os2.exports;    // put in the front of the list for
    FmtData.u.os2.exports = exp;          // now so ProcResidant can get to it.
    while( ProcOne( Exp_Keywords, SEP_NO, FALSE ) ) {}  // handle misc options
    FmtData.u.os2.exports = exp->next;       // take it off the list
    exp->iopl_words = 0;
    if(!(FmtData.type & (MK_WINDOWS|MK_PE)) &&GetToken(SEP_NO,TOK_INCLUDE_DOT)){
        if( getatoi( &value ) == ST_IS_ORDINAL ) {
            if( value > 63 ) {
                LnkMsg( LOC+LINE+MSG_TOO_MANY_IOPL_WORDS+ ERR, NULL );
            } else {
                exp->iopl_words = value;
            }
        } else {
            Token.thumb = REJECT;    // reprocess the token.
        }
    }
    AddToExportList( exp );
    return( TRUE );
}

extern bool ProcExpResident( void )
/*********************************/
{
    FmtData.u.os2.exports->isresident = TRUE;
    return( TRUE );
}

extern bool ProcPrivate( void )
/******************************/
{
    FmtData.u.os2.exports->isprivate = TRUE;
    return( TRUE );
}

extern bool ProcOS2Alignment( void )
/**********************************/
/* process Alignment option */
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals(0) ) return( FALSE );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( FALSE );
    }
    FmtData.u.os2.segment_shift = blog_32( value - 1 ) + 1;     //round up.
    return( TRUE );
}

extern bool ProcObjAlign( void )
/******************************/
/* process ObjAlign option */
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals(0) ) return( FALSE );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( FALSE );
    }                                            /* value not a power of 2 */
    if( value < 512 || value > (256*1024UL*1024) || (value & (value-1)) ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "objalign" );
        value = 64*1024;
    }
    FmtData.objalign = value;
    ChkBase(value);
    return( TRUE );
}

extern bool ProcModName( void )
/*****************************/
{
    if( !HaveEquals(TOK_INCLUDE_DOT) ) return( FALSE );
    FmtData.u.os2.res_module_name = totext();
    return( TRUE );
}

extern bool ProcNewFiles( void )
/******************************/
{
    FmtData.u.os2.flags |= LONG_FILENAMES;
    return( TRUE );
}

extern bool ProcProtMode( void )
/******************************/
{
    FmtData.u.os2.flags |= PROTMODE_ONLY;
    return( TRUE );
}

extern bool ProcOldLibrary( void )
/********************************/
{
    if( !HaveEquals(TOK_INCLUDE_DOT | TOK_IS_FILENAME) ) return( FALSE );
    FmtData.u.os2.old_lib_name = FileName( Token.this, Token.len, E_DLL, FALSE );
    return( TRUE );
}

extern bool ProcOS2HeapSize( void )
/*********************************/
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals(0) ) return( FALSE );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "heapsize" );
    } else {
        FmtData.u.os2.heapsize = value;
    }
    return( TRUE );
}

extern bool ProcDescription( void )
/*********************************/
{
    if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        return( FALSE );
    }
    FmtData.u.os2.description = tostring();
    return( TRUE );
}

extern bool ProcCommitStack( void )
/*********************************/
{
    return( GetLong( &FmtData.u.pe.stackcommit ) );
}

extern bool ProcCommitHeap( void )
/********************************/
{
    return( GetLong( &FmtData.u.pe.heapcommit ) );
}

static bool AddCommit( void )
/***************************/
{
    Token.thumb = REJECT;
    if( ProcOne( CommitKeywords, SEP_NO, FALSE ) == FALSE ) return( FALSE );
    return TRUE;
}

extern bool ProcCommit( void )
/****************************/
// set NT stack commit and heap sizes.
{
    return ProcArgList( AddCommit, TOK_INCLUDE_DOT );
}

extern bool ProcRWRelocCheck( void )
/**********************************/
// check for segment relocations pointing to read/write data segments
{
    FmtData.u.os2.chk_seg_relocs = TRUE;
    return( TRUE );
}

extern bool ProcSelfRelative( void )
/**********************************/
{
    FmtData.u.os2.gen_rel_relocs = TRUE;
    return TRUE;
}

extern bool ProcInternalRelocs( void )
/************************************/
// in case someone wants internal relocs generated.
{
    FmtData.u.os2.gen_int_relocs = TRUE;
    return TRUE;
}

extern bool ProcToggleRelocsFlag( void )
/***************************************/
// Rational wants internal relocs generated, but wants the "no internal relocs"
// flag set
{
    FmtData.u.os2.toggle_relocs = TRUE;
    return TRUE;
}

extern bool ProcPENoRelocs( void )
/*********************************/
{
    LinkState &= ~MAKE_RELOCS;
    return TRUE;
}

extern bool ProcNoStdCall( void )
/*******************************/
{
    FmtData.u.pe.no_stdcall = TRUE;
    return TRUE;
}

extern bool ProcOS2( void )
/*************************/
// process the format os2 or format windows directives
//
{
    Extension = E_LOAD;
    while( ProcOne( SubFormats, SEP_NO, FALSE ) ) {} // NOTE NULL loop
    if( FmtData.type & MK_WINDOWS ) {
        if( ProcOne( WindowsFormatKeywords, SEP_NO, FALSE ) ) {
            ProcOne( WindowsFormatKeywords, SEP_NO, FALSE );
        }
    } else {
        ProcOne( OS2FormatKeywords, SEP_NO, FALSE );
        if( FmtData.type & MK_OS2_LX ) {
            if( FmtData.dll ) {
                FmtData.u.os2.gen_int_relocs = TRUE;
            }
        }
    }
    if( FmtData.type & MK_ONLY_OS2_16 ) {       // if no 32-bit thing specd
        HintFormat( MK_ONLY_OS2_16 );   // make sure 16-bit is what we get
        if( FmtData.dll ) {
            FmtData.u.os2.flags &= ~MULTIPLE_AUTO_DATA;
            FmtData.u.os2.flags |= SINGLE_AUTO_DATA;
        }
    }
    return( TRUE );
}

extern bool ProcPE( void )
/************************/
{
    ProcOne( NTFormatKeywords, SEP_NO, FALSE );
    FmtData.u.pe.heapcommit = 4*1024;   // arbitrary non-zero default.
    FmtData.u.pe.os2.heapsize = 8*1024; // another arbitrary non-zero default
    FmtData.u.pe.stackcommit = PE_DEF_STACK_COMMIT;
    return( TRUE );
}

extern bool ProcWindows( void )
/*****************************/
{
    return( ProcOS2() );
}

extern void ChkBase( offset align )
/*********************************/
// Note: align must be a power of 2
{
    if( FmtData.objalign != NO_BASE_SPEC && FmtData.objalign > align ) {
        align = FmtData.objalign;
    }
    if( FmtData.base != NO_BASE_SPEC &&
                (FmtData.base & (align-1)) != 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_OFFSET_MUST_BE_ALIGNED, "l", align );
        FmtData.base = (FmtData.base + align-1) & ~(align-1);
    }
}

extern void SetOS2Fmt()
/*********************/
// set up the structures needed to be able to process something in OS/2 mode.
{
    if( LinkState & FMT_INITIALIZED ) return;
    LinkState |= FMT_INITIALIZED;
    FmtData.u.os2.flags = MULTIPLE_AUTO_DATA;
    FmtData.u.os2.heapsize = 0;
    FmtData.u.os2.segment_shift = 0;
    FmtData.u.os2.res_module_name = NULL;
    FmtData.u.os2.stub_file_name = NULL;
    FmtData.u.os2.os2_seg_flags = NULL;
    FmtData.u.os2.description = NULL;
    FmtData.u.os2.exports = NULL;
    FmtData.u.os2.old_lib_name = NULL;
    if( FmtData.type & MK_WINDOWS ) {
        FmtData.def_seg_flags |= SEG_PRELOAD;
    }
    Extension = E_LOAD;
    ChkBase(64*1024);
}

extern void FreeOS2Fmt()
/**********************/
{
    _LnkFree( FmtData.u.os2.stub_file_name );
    _LnkFree( FmtData.u.os2.res_module_name );
    _LnkFree( FmtData.u.os2.old_lib_name );
    _LnkFree( FmtData.u.os2.description );
    FreeImpNameTab();
    FreeExportList();
    FreeSegFlags( (seg_flags *) FmtData.u.os2.os2_seg_flags );
}

extern bool ProcLE( void )
/************************/
{
    return( TRUE );
}

extern bool ProcLX( void )
/************************/
{
    return( TRUE );
}

extern bool ProcOS2DLL( void )
/****************************/
{
    FmtData.dll = TRUE;
    Extension = E_DLL;
    if( FmtData.type & MK_WINDOWS ) {
        FmtData.u.os2.flags &= ~MULTIPLE_AUTO_DATA;
        FmtData.u.os2.flags |= SINGLE_AUTO_DATA;
        FmtData.def_seg_flags |= SEG_PURE | SEG_MOVABLE;
    }
    if( ProcOne( Init_Keywords, SEP_NO, FALSE ) ) {
        if( !ProcOne( Term_Keywords, SEP_NO, FALSE ) ) {
            if( FmtData.u.os2.flags & INIT_INSTANCE_FLAG ) {
                FmtData.u.os2.flags |= TERM_INSTANCE_FLAG;
            }
        }
    }
    return( TRUE );
}

extern bool ProcPhysDevice( void )
/********************************/
{
    FmtData.u.os2.flags |= PHYS_DEVICE;
    return( TRUE );
}

extern bool ProcVirtDevice( void )
/********************************/
{
    FmtData.u.os2.flags |= VIRT_DEVICE;
    return( TRUE );
}

extern bool ProcPM( void )
/************************/
{
    FmtData.u.os2.flags |= PM_APPLICATION;
    return( TRUE );
}

extern bool ProcPMCompatible( void )
/**********************************/
{
    FmtData.u.os2.flags |= PM_COMPATIBLE;
    return( TRUE );
}

extern bool ProcPMFullscreen( void )
/**********************************/
{
    FmtData.u.os2.flags |= PM_NOT_COMPATIBLE;
    return( TRUE );
}

extern bool ProcMemory( void )
/****************************/
{
    FmtData.u.os2.flags |= CLEAN_MEMORY;
    return( TRUE );
}

extern bool ProcFont( void )
/**************************/
{
    FmtData.u.os2.flags |= PROPORTIONAL_FONT;
    return( TRUE );
}

extern bool ProcInitGlobal( void )
/********************************/
{
    FmtData.u.os2.flags &= ~INIT_INSTANCE_FLAG;
    return( TRUE );
}

extern bool ProcInitInstance( void )
/**********************************/
{
    FmtData.u.os2.flags |= INIT_INSTANCE_FLAG;
    return( TRUE );
}

extern bool ProcInitThread( void )
/********************************/
{
    FmtData.u.os2.flags |= INIT_THREAD_FLAG;
    return( TRUE );
}

extern bool ProcTermGlobal( void )
/********************************/
{
    FmtData.u.os2.flags &= ~TERM_INSTANCE_FLAG;
    return( TRUE );
}

extern bool ProcTermInstance( void )
/**********************************/
{
    FmtData.u.os2.flags |= TERM_INSTANCE_FLAG;
    return( TRUE );
}

extern bool ProcTermThread( void )
/********************************/
{
    FmtData.u.os2.flags |= TERM_THREAD_FLAG;
    return( TRUE );
}

static bool getsegflags( void )
/*****************************/
{
    os2_seg_flags * entry;

    Token.thumb = REJECT;
    _ChkAlloc( entry, sizeof( os2_seg_flags ) );
    entry->specified = 0;
    entry->flags = FmtData.def_seg_flags;    // default value.
    entry->name = NULL;
    entry->type = SEGFLAG_SEGMENT;
    entry->next = FmtData.u.os2.os2_seg_flags;
    FmtData.u.os2.os2_seg_flags = entry;
    ProcOne( SegDesc, SEP_NO, FALSE );          // look for an optional segdesc
    if( entry->type != SEGFLAG_CODE && entry->type != SEGFLAG_DATA ) {
        if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ){
            FmtData.u.os2.os2_seg_flags = entry->next;
            _LnkFree( entry );
            return( FALSE );
        }
        entry->name = tostring();
    }
    while( ProcOne( SegModel, SEP_NO, FALSE ) ) {}
    return( TRUE );
}

extern bool ProcOS2Class( void )
/******************************/
{
    FmtData.u.os2.os2_seg_flags->type = SEGFLAG_CLASS;
    return( TRUE );
}

extern bool ProcSegType( void )
/*****************************/
{
    if( !ProcOne( SegTypeDesc, SEP_NO, FALSE ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_INVALID_TYPE_DESC, NULL );
    }
    return TRUE;
}

extern bool ProcSegCode( void )
/*****************************/
{
    FmtData.u.os2.os2_seg_flags->type = SEGFLAG_CODE;
    return TRUE;
}

extern bool ProcSegData( void )
/*****************************/
{
    FmtData.u.os2.os2_seg_flags->type = SEGFLAG_DATA;
    return TRUE;
}

extern bool ProcPreload( void )
/*****************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_PRELOAD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_PRELOAD;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_PRELOAD;
    return( TRUE );
}

extern bool ProcLoadoncall( void )
/********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_PRELOAD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_PRELOAD;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_PRELOAD;
    return( TRUE );
}

extern bool ProcIopl( void )
/**************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_IOPL_SPECD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_LEVEL_3;
    FmtData.u.os2.os2_seg_flags->flags |= SEG_LEVEL_2;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_IOPL_SPECD;
    return( TRUE );
}

extern bool ProcNoIopl( void )
/****************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_IOPL_SPECD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_LEVEL_3;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_IOPL_SPECD;
    return( TRUE );
}

extern bool ProcExecuteonly( void )
/*********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_READ_ONLY;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_ONLY;
    return( TRUE );
}

extern bool ProcExecuteread( void )
/*********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_READ_ONLY;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_ONLY;
    return( TRUE );
}

extern bool ProcReadOnly( void )
/******************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_READ_ONLY;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_SPECD;
    return( TRUE );
}

extern bool ProcReadWrite( void )
/******************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_READ_ONLY;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_READ_SPECD;
    return( TRUE );
}

extern bool ProcShared( void )
/****************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_PURE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_PURE;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_PURE;
    return( TRUE );
}

extern bool ProcNonShared( void )
/*******************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_PURE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_PURE;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_PURE;
    return( TRUE );
}

extern bool ProcConforming( void )
/********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_CONFORMING ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_CONFORMING;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_CONFORMING;
    return( TRUE );
}

extern bool ProcNonConforming( void )
/***********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_CONFORMING ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_CONFORMING;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_CONFORMING;
    return( TRUE );
}

extern bool ProcMovable( void )
/*****************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_MOVABLE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_MOVABLE;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_MOVABLE;
    return( TRUE );
}

extern bool ProcFixed( void )
/*****************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_MOVABLE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_MOVABLE;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_MOVABLE;
    return( TRUE );
}

extern bool ProcNonPageable( void )
/*********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_NOPAGE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_NOPAGE;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_NOPAGE;
    return( TRUE );
}

extern bool ProcPageable( void )
/******************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_NOPAGE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_NOPAGE;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_NOPAGE;
    return( TRUE );
}

extern bool ProcDiscardable( void )
/*********************************/
{
    FmtData.u.os2.os2_seg_flags->flags |= SEG_DISCARD;
    return( TRUE );
}

extern bool ProcOS2Dynamic( void )
/********************************/
{
//    FmtData.u.os2.os2_seg_flags->flags |= SEG_DISCARD;
    return( TRUE );
}

extern bool ProcInvalid( void )
/*****************************/
{
    FmtData.u.os2.os2_seg_flags->flags |= SEG_INVALID;
    return( TRUE );
}

extern bool ProcPermanent( void )
/*******************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_RESIDENT ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags |= SEG_RESIDENT;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_RESIDENT;
    return( TRUE );
}

extern bool ProcNonPermanent( void )
/**********************************/
{
    if( FmtData.u.os2.os2_seg_flags->specified & SEG_RESIDENT ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.os2_seg_flags->flags &= ~SEG_RESIDENT;
    FmtData.u.os2.os2_seg_flags->specified |= SEG_RESIDENT;
    return( TRUE );
}

extern bool ProcContiguous( void )
/********************************/
{
    FmtData.u.os2.os2_seg_flags->flags |= SEG_CONTIGUOUS;
    return( TRUE );
}

extern bool ProcSingle( void )
/****************************/
{
    if( CmdFlags & CF_AUTO_SEG_FLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_AUTO_SEG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.flags |= SINGLE_AUTO_DATA;
    FmtData.u.os2.flags &= ~MULTIPLE_AUTO_DATA;
    return( TRUE );
}

extern bool ProcMultiple( void )
/******************************/
{
    if( CmdFlags & CF_AUTO_SEG_FLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_AUTO_SEG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.flags &= ~SINGLE_AUTO_DATA;
    FmtData.u.os2.flags |= MULTIPLE_AUTO_DATA;
    return( TRUE );
}

extern bool ProcNone( void )
/**************************/
{
    if( CmdFlags & CF_AUTO_SEG_FLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_AUTO_SEG_MULT_DEFD, NULL );
    }
    FmtData.u.os2.flags &= ~(SINGLE_AUTO_DATA | MULTIPLE_AUTO_DATA);
    return( TRUE );
}

extern bool ProcRunNative( void )
/*******************************/
{
    FmtData.u.pe.subsystem = PE_SS_NATIVE;
    ParseVersion();
    return( TRUE );
}

extern bool ProcRunWindows( void )
/********************************/
{
    FmtData.u.pe.subsystem = PE_SS_WINDOWS_GUI;
    ParseVersion();
    return( TRUE );
}

extern bool ProcRunConsole( void )
/********************************/
{
    FmtData.u.pe.subsystem = PE_SS_WINDOWS_CHAR;
    ParseVersion();
    return( TRUE );
}

extern bool ProcRunPosix( void )
/******************************/
{
    FmtData.u.pe.subsystem = PE_SS_POSIX_CHAR;
    ParseVersion();
    return( TRUE );
}

extern bool ProcRunOS2( void )
/****************************/
{
    FmtData.u.pe.subsystem = PE_SS_OS2_CHAR;
    ParseVersion();
    return( TRUE );
}


extern bool ProcRunDosstyle( void )
/*********************************/
{
    FmtData.u.pe.subsystem = PE_SS_PL_DOSSTYLE;
    FmtData.u.pe.tnt = TRUE;
    ParseVersion();
    return( TRUE );
}

extern bool ProcTNT( void )
/*************************/
{
    FmtData.u.pe.tnt = TRUE;
    return TRUE;
}

static void ParseVersion( void )
/******************************/
{
    ord_state   retval;

    if( !GetToken( SEP_EQUALS, 0 ) ) return;
    FmtData.u.pe.submajor = 0;
    FmtData.u.pe.subminor = 0;
    retval = getatoi( &FmtData.u.pe.submajor );
    if( retval != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "subsystem" );
        return;
    }
    FmtData.u.pe.sub_specd = TRUE;
    if( !GetToken( SEP_PERIOD, 0 ) ) {  /*if we don't get a minor number*/
       return;                          /* that's OK */
    }
    retval = getatoi( &FmtData.u.pe.subminor );
    if( retval != ST_IS_ORDINAL ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "subsystem" );
    }
}

static bool AddResource( void )
/*****************************/
{
    DoAddResource( tostring() );
    return TRUE;
}

extern bool ProcResource( void )
/******************************/
{
    return ProcArgList( &AddResource, TOK_INCLUDE_DOT | TOK_IS_FILENAME );
}

