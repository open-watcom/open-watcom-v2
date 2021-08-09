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
* Description:  Processing of linker options for OS/2 and Windows formats.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include "linkstd.h"
#include "alloc.h"
#include "walloca.h"
#include "cmdutils.h"
#include "cmdos2.h"
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
#include "cmdall.h"


#ifdef _OS2

void SetOS2Fmt( void )
/*********************
 * set up the structures needed to be able to process something in OS/2 mode.
 */
{
    if( LinkState & LS_FMT_INITIALIZED )
        return;
    Extension = E_LOAD;
    LinkState |= LS_FMT_INITIALIZED;
    FmtData.u.os2fam.flags = MULTIPLE_AUTO_DATA;
    FmtData.u.os2fam.heapsize = 0;
    FmtData.u.os2fam.segment_shift = 0;
    FmtData.u.os2fam.module_name = NULL;
    FmtData.u.os2fam.stub_file_name = NULL;
    FmtData.u.os2fam.seg_flags = NULL;
    FmtData.description = NULL;
    FmtData.u.os2fam.exports = NULL;
    FmtData.u.os2fam.old_lib_name = NULL;
    if( FmtData.type & MK_WINDOWS ) {
        FmtData.def_seg_flags |= SEG_PRELOAD;
    }
    ChkBase( _64KB );
}

void FreeOS2Fmt( void )
/*********************/
{
    _LnkFree( FmtData.u.os2fam.stub_file_name );
    _LnkFree( FmtData.u.os2fam.module_name );
    _LnkFree( FmtData.u.os2fam.old_lib_name );
    _LnkFree( FmtData.description );
    FreeImpNameTab();
    FreeExportList();
    FreeSegFlags( (xxx_seg_flags *)FmtData.u.os2fam.seg_flags );
}

static entry_export *ProcWlibDLLImportEntry( void )
/*************************************************/
{
    unsigned_16     ordinal;
    entry_export    *exp;
    length_name     symname;
    length_name     internal;

    DUPSTR_STACK( symname.name, Token.this, Token.len );
    symname.len = Token.len;
    if( !GetToken( SEP_DOT_EXT, TOK_NORMAL ) ) {
        return( NULL );
    }
    internal.len = 0;
    internal.name = NULL;
    ordinal = 0;
    if( GetToken( SEP_DOT_EXT, TOK_NORMAL ) ) {
        if( getatoi( &ordinal ) != ST_IS_ORDINAL ) {
            if( Token.len > 0 ) {
                internal = symname;
                DUPSTR_STACK( symname.name, Token.this, Token.len );
                symname.len = Token.len;
            }
            if( GetToken( SEP_DOT_EXT, TOK_NORMAL ) && getatoi( &ordinal ) != ST_IS_ORDINAL ) {
                if( GetToken( SEP_DOT_EXT, TOK_NORMAL ) ) {
                    getatoi( &ordinal );
                }
            }
        }
    }
    exp = AllocExport( symname.name, symname.len );
    if( CmdFlags & CF_ANON_EXPORT )
        exp->isanonymous = true;
    if( internal.name != NULL ) {
        exp->sym = SymOp( ST_CREATE_REFERENCE, internal.name, internal.len );
    } else {
        exp->sym = SymOp( ST_CREATE_REFERENCE, symname.name, symname.len );
    }
    exp->sym->info |= SYM_DCE_REF;      // make sure it isn't removed
    exp->ordinal = ordinal;
    if( ordinal == 0 ) {
        exp->isresident = true;   // no ord spec'd so must be resident
    }
    return( exp );
}

static bool GetWlibImports( void )
/*********************************
 * read in a wlib command file, get the import directives, and treat them
 * as exports (hey man, GO asked for it ...... )
 */
{
    char            *fname;
    f_handle        handle;
    entry_export    *exp;

    fname = FileName( Token.this, Token.len, E_LBC, false );
    handle = QOpenR( fname );
    SetCommandFile( handle, fname );
    _LnkFree( fname );
    Token.locked = true;      /* make sure only this file parsed */
    while( GetToken( SEP_SPACE, TOK_NORMAL ) ) {
        if( Token.len <= 2 )
            continue;
        if( (Token.this[0] == '+') && (Token.this[1] == '+') ) {
            Token.this += 2;
            Token.len -= 2;
            if( Token.this[0] == '\'' ) {
                Token.thumb = true;
                if( !GetToken( SEP_QUOTE, TOK_NORMAL ) ) {
                    LnkMsg( LOC+LINE+ERR+MSG_BAD_WLIB_IMPORT, NULL );
                    RestoreCmdLine();   /* get rid of this file */
                    return( true );
                }
            }
            exp = ProcWlibDLLImportEntry();
            if( exp == NULL ) {
                LnkMsg( LOC+LINE+ERR+MSG_BAD_WLIB_IMPORT, NULL );
                RestoreCmdLine();       /* get rid of this file */
                return( true );
            }
            AddToExportList( exp );
        }
    }
    Token.locked = false;
    return( true );
}


/****************************************************************
 * "OPtion" Directive
 ****************************************************************/

static bool ProcNoStub( void )
/****************************/
{
    FmtData.u.os2fam.no_stub = true;
    return( true );
}

static bool ProcSingle( void )
/****************************/
{
    if( CmdFlags & CF_AUTO_SEG_FLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_AUTO_SEG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.flags |= SINGLE_AUTO_DATA;
    FmtData.u.os2fam.flags &= ~MULTIPLE_AUTO_DATA;
    return( true );
}

static bool ProcMultiple( void )
/******************************/
{
    if( CmdFlags & CF_AUTO_SEG_FLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_AUTO_SEG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.flags &= ~SINGLE_AUTO_DATA;
    FmtData.u.os2fam.flags |= MULTIPLE_AUTO_DATA;
    return( true );
}

static bool ProcNone( void )
/**************************/
{
    if( CmdFlags & CF_AUTO_SEG_FLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_AUTO_SEG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.flags &= ~(SINGLE_AUTO_DATA | MULTIPLE_AUTO_DATA);
    return( true );
}

static bool ProcOldLibrary( void )
/********************************/
{
    if( !HaveEquals(TOK_INCLUDE_DOT | TOK_IS_FILENAME) )
        return( false );
    FmtData.u.os2fam.old_lib_name = FileName( Token.this, Token.len, E_DLL, false );
    return( true );
}

static bool ProcModName( void )
/*****************************/
{
    if( !HaveEquals( TOK_INCLUDE_DOT ) )
        return( false );
    FmtData.u.os2fam.module_name = totext();
    return( true );
}

static bool ProcNewFiles( void )
/******************************/
{
    FmtData.u.os2fam.flags |= LONG_FILENAMES;
    return( true );
}

static bool ProcProtMode( void )
/******************************/
{
    FmtData.u.os2fam.flags |= PROTMODE_ONLY;
    return( true );
}

static bool ProcNoStdCall( void )
/*******************************/
{
    FmtData.u.pe.no_stdcall = true;
    return( true );
}

static bool ProcRWRelocCheck( void )
/***********************************
 * check for segment relocations pointing to read/write data segments
 */
{
    FmtData.u.os2fam.chk_seg_relocs = true;
    return( true );
}

static bool ProcSelfRelative( void )
/**********************************/
{
    FmtData.u.os2fam.gen_rel_relocs = true;
    return( true );
}

static bool ProcInternalRelocs( void )
/*************************************
 * in case someone wants internal relocs generated.
 */
{
    FmtData.u.os2fam.gen_int_relocs = true;
    return( true );
}

static bool ProcToggleRelocsFlag( void )
/***************************************
 * Rational wants internal relocs generated, but wants
 * the "no internal relocs" flag set
 */
{
    FmtData.u.os2fam.toggle_relocs = true;
    return( true );
}

static bool ProcLinkVersion( void )
/*********************************/
{
    version_state   result;
    version_block   vb;

    /* set required limits, 0 = no limit */
    vb.major = 255;
    vb.minor = 255;
    vb.revision = 0;
    vb.message = "LINKVERSION";
    result = GetGenVersion( &vb, GENVER_MAJOR | GENVER_MINOR, false );
    if( result != GENVER_ERROR ) {
        FmtData.u.pe.lnk_specd = true;
        FmtData.u.pe.linkmajor = vb.major;
        FmtData.u.pe.linkminor = vb.minor;
        return( true );
    }
    return( false );
}

static bool ProcOsVersion( void )
/*******************************/
{
    version_state   result;
    version_block   vb;

    /* set required limits, 0 = no limit */
    vb.major = 0;
    vb.minor = 99;          /* from old default of 100 max */
    vb.revision = 0;
    vb.message = "OSVERSION";
    result = GetGenVersion( &vb, GENVER_MAJOR | GENVER_MINOR, false );
    if( result != GENVER_ERROR ) {
        FmtData.u.pe.osv_specd = true;
        FmtData.u.pe.osmajor = vb.major;
        FmtData.u.pe.osminor = vb.minor;
        return( true );
    }
    return( false );    /* error has occurred */
}

static bool ProcChecksum( void )
/******************************/
{
    FmtData.u.pe.checksumfile = true;
    return( true );
}

static bool ProcLargeAddressAware( void )
/***************************************/
{
    FmtData.u.pe.largeaddressaware = true;
    FmtData.u.pe.nolargeaddressaware = false;
    return( true );
}

static bool ProcNoLargeAddressAware( void )
/*****************************************/
{
    FmtData.u.pe.nolargeaddressaware = true;
    FmtData.u.pe.largeaddressaware = false;
    return( true );
}

static bool ProcMixed1632( void )
/********************************
 * Sometimes it's useful to mix 16-bit and 32-bit code/data into one segment
 * specially for OS/2 Device Drivers
 */
{
    LinkFlags &= ~LF_FAR_CALLS_FLAG ; // must be turned off for mixed code
    FmtData.u.os2fam.mixed1632 = true;
    return( true );
}

static parse_entry  MainOptions[] = {
    "NOSTUB",               ProcNoStub,                 MK_OS2 | MK_PE | MK_WIN_VXD,        0,
    "ONEautodata",          ProcSingle,                 MK_OS2,                             CF_AUTO_SEG_FLAG,
    "MANYautodata",         ProcMultiple,               MK_OS2,                             CF_AUTO_SEG_FLAG,
    "NOAutodata",           ProcNone,                   MK_OS2_16BIT,                       CF_AUTO_SEG_FLAG,
    "OLDlibrary",           ProcOldLibrary,             MK_OS2 | MK_PE,                     0,
    "MODName",              ProcModName,                MK_OS2 | MK_PE | MK_WIN_VXD,        0,
    "NEWFiles",             ProcNewFiles,               MK_ONLY_OS2_16,                     0,
    "PROTmode",             ProcProtMode,               MK_OS2_16BIT,                       0,
    "NOSTDCall",            ProcNoStdCall,              MK_PE,                              0,
    "RWReloccheck",         ProcRWRelocCheck,           MK_WINDOWS,                         0,
    "SELFrelative",         ProcSelfRelative,           MK_OS2_LX,                          0,
    "INTernalrelocs",       ProcInternalRelocs,         MK_OS2_LX,                          0,
    "TOGglerelocsflag",     ProcToggleRelocsFlag,       MK_OS2_LX,                          0,
    "LINKVersion",          ProcLinkVersion,            MK_PE,                              0,
    "OSVersion",            ProcOsVersion,              MK_PE,                              0,
    "CHECKSUM",             ProcChecksum,               MK_PE,                              0,
    "LARGEaddressaware",    ProcLargeAddressAware,      MK_PE,                              0,
    "NOLARGEaddressaware",  ProcNoLargeAddressAware,    MK_PE,                              0,
    "MIXed1632",            ProcMixed1632,              MK_OS2_FLAT,                        0,
    NULL
};

bool ProcOS2Options( void )
/*************************/
{
    return( ProcOne( MainOptions, SEP_NO ) );
}

bool ProcOS2NoRelocs( void )
/**************************/
{
    LinkState &= ~LS_MAKE_RELOCS;
    return( true );
}

bool ProcOS2Alignment( void )
/****************************
 * process Alignment option
 */
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        return( false );
    }
    FmtData.u.os2fam.segment_shift = log2_32( value - 1 ) + 1;     //round up.
    return( true );
}

bool ProcOS2HeapSize( void )
/**************************/
{
    ord_state           ret;
    unsigned_32         value;

    if( !HaveEquals( TOK_NORMAL ) )
        return( false );
    ret = getatol( &value );
    if( ret != ST_IS_ORDINAL || value == 0 ) {
        LnkMsg( LOC+LINE+WRN+MSG_VALUE_INCORRECT, "s", "HEAPSIZE" );
    } else {
        FmtData.u.os2fam.heapsize = value;
    }
    return( true );
}


/****************************************************************
 * "RESource" Directive
 ****************************************************************/

static bool AddResource( void )
/*****************************/
{
    char    *str;

    str = tostring();
    DoAddResource( str );
    _LnkFree( str );
    return( true );
}

bool ProcResource( void )
/***********************/
{
    return( ProcArgList( AddResource, TOK_INCLUDE_DOT | TOK_IS_FILENAME ) );
}


/****************************************************************
 * "IMPort" Directive
 ****************************************************************/

static bool getimport( void )
/***************************/
{
    length_name         intname;
    length_name         modname;
    length_name         extname;
    unsigned_16         ordinal;
    ord_state           state;

    DUPSTR_STACK( intname.name, Token.this, Token.len );
    intname.len = Token.len;
    if( !GetToken( SEP_NO, TOK_NORMAL ) ) {
        return( false );
    }
    DUPSTR_STACK( modname.name, Token.this, Token.len );
    modname.len = Token.len;
    ordinal = 0;
    state = ST_INVALID_ORDINAL;   // assume to extname or ordinal.
    if( GetToken( SEP_PERIOD, TOK_INCLUDE_DOT ) ) {
        state =  getatoi( &ordinal );
        if( state == ST_NOT_ORDINAL ) {
            DUPSTR_STACK( extname.name, Token.this, Token.len );
            extname.len = Token.len;
        } else if( state == ST_INVALID_ORDINAL ) {
            LnkMsg( LOC+LINE+MSG_IMPORT_ORD_INVALID + ERR, NULL );
            return( true );
        }
    }
    if( state == ST_IS_ORDINAL ) {
        HandleImport( &intname, &modname, &intname, ordinal );
    } else {
        if( state == ST_NOT_ORDINAL ) {
            HandleImport( &intname, &modname, &extname, NOT_IMP_BY_ORDINAL );
        } else {
            HandleImport( &intname, &modname, &intname, NOT_IMP_BY_ORDINAL );
        }
    }
    return( true );
}

bool ProcOS2Import( void )
/************************/
{
    return( ProcArgList( getimport, TOK_NORMAL ) );
}


/****************************************************************
 * "EXPort" Directive
 ****************************************************************/

static bool ProcExpResident( void )
/*********************************/
{
    FmtData.u.os2fam.exports->isresident = true;
    return( true );
}

static bool ProcPrivate( void )
/*****************************/
{
    FmtData.u.os2fam.exports->isprivate = true;
    return( true );
}

static parse_entry  Exp_Keywords[] = {
    "RESident",     ProcExpResident,    MK_OS2, 0,
    "PRIVATE",      ProcPrivate,        MK_OS2 | MK_PE, 0,
    NULL
};

static bool getexport( void )
/***************************/
{
    entry_export    *exp;
    unsigned_16     val16;
    unsigned_32     val32;

    exp = AllocExport( Token.this, Token.len );
    if( CmdFlags & CF_ANON_EXPORT )
        exp->isanonymous = true;
    if( GetToken( SEP_PERIOD, TOK_INCLUDE_DOT ) ) {
        if( getatol( &val32 ) != ST_IS_ORDINAL ) {
            LnkMsg( LOC+LINE+ERR + MSG_EXPORT_ORD_INVALID, NULL );
            _LnkFree( exp );
            GetToken( SEP_EQUALS, TOK_INCLUDE_DOT );
            return( true );
        }
        exp->ordinal = val32;
    }
    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        exp->sym = SymOp( ST_CREATE_REFERENCE, Token.this, Token.len );
        if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
            exp->impname = tostring();
        }
    } else {
        exp->sym = RefISymbol( exp->name.u.ptr );
    }
    exp->sym->info |= SYM_DCE_REF;          //make sure it is not removed
    if( exp->ordinal == 0 ) {
        exp->isresident = true;             // no ordinal spec'd so must be kept resident
    }
    exp->next = FmtData.u.os2fam.exports;   // put in the front of the list for
    FmtData.u.os2fam.exports = exp;         // now so ProcResidant can get to it.
    while( ProcOne( Exp_Keywords, SEP_NO ) ) {
        // handle misc options
    }
    FmtData.u.os2fam.exports = exp->next;   // take it off the list
    exp->iopl_words = 0;
    if( (FmtData.type & (MK_WINDOWS | MK_PE)) == 0 && GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
        if( getatoi( &val16 ) == ST_IS_ORDINAL ) {
            if( val16 > 63 ) {
                LnkMsg( LOC+LINE+MSG_TOO_MANY_IOPL_WORDS+ ERR, NULL );
            } else {
                if( val16 & 1 ) {
                    LnkMsg( LOC+LINE+WRN+MSG_IOPL_BYTES_ODD, NULL );
                }
                // The linker takes bytes as input!
                exp->iopl_words = val16 / 2;
            }
        } else {
            Token.thumb = true;     // reprocess the token.
        }
    }
    AddToExportList( exp );
    return( true );
}

bool ProcOS2Export( void )
/************************/
{
    bool    retval;

    if( GetToken( SEP_EQUALS, TOK_INCLUDE_DOT ) ) {
        retval = GetWlibImports();
    } else {
        retval = ProcArgList( getexport, TOK_NORMAL );
    }
    return( retval );
}


/****************************************************************
 * "ANONymousexport" Directive
 ****************************************************************/

bool ProcAnonExport( void )
/*************************/
{
    bool    retval;

    CmdFlags |= CF_ANON_EXPORT;
    retval = ProcOS2Export();
    CmdFlags &= ~CF_ANON_EXPORT;
    return( retval );
}


/****************************************************************
 * "SEGment" Directive
 ****************************************************************/

static bool ProcOS2Class( void )
/******************************/
{
    FmtData.u.os2fam.seg_flags->type = SEGFLAG_CLASS;
    return( true );
}

static bool ProcSegCode( void )
/*****************************/
{
    FmtData.u.os2fam.seg_flags->type = SEGFLAG_CODE;
    return( true );
}

static bool ProcSegData( void )
/*****************************/
{
    FmtData.u.os2fam.seg_flags->type = SEGFLAG_DATA;
    return( true );
}

static parse_entry  SegTypeDesc[] = {
    "CODE",         ProcSegCode,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "DATA",         ProcSegData,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    NULL
};

static bool ProcSegType( void )
/*****************************/
{
    if( !ProcOne( SegTypeDesc, SEP_NO ) ) {
        LnkMsg( LOC+LINE+WRN+MSG_INVALID_TYPE_DESC, NULL );
    }
    return( true );
}

static bool ProcPreload( void )
/*****************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_PRELOAD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_PRELOAD;
    FmtData.u.os2fam.seg_flags->specified |= SEG_PRELOAD;
    return( true );
}

static bool ProcLoadoncall( void )
/********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_PRELOAD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_PRELOAD;
    FmtData.u.os2fam.seg_flags->specified |= SEG_PRELOAD;
    return( true );
}

static bool ProcIopl( void )
/**************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_IOPL_SPECD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_LEVEL_3;
    FmtData.u.os2fam.seg_flags->flags |= SEG_LEVEL_2;
    FmtData.u.os2fam.seg_flags->specified |= SEG_IOPL_SPECD;
    return( true );
}

static bool ProcNoIopl( void )
/****************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_IOPL_SPECD ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_LEVEL_3;
    FmtData.u.os2fam.seg_flags->specified |= SEG_IOPL_SPECD;
    return( true );
}

static bool ProcExecuteonly( void )
/*********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_READ_ONLY;
    FmtData.u.os2fam.seg_flags->specified |= SEG_READ_ONLY;
    return( true );
}

static bool ProcExecuteread( void )
/*********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_READ_ONLY;
    FmtData.u.os2fam.seg_flags->specified |= SEG_READ_ONLY;
    return( true );
}

static bool ProcShared( void )
/****************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_PURE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_PURE;
    FmtData.u.os2fam.seg_flags->specified |= SEG_PURE;
    return( true );
}

static bool ProcNonShared( void )
/*******************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_PURE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_PURE;
    FmtData.u.os2fam.seg_flags->specified |= SEG_PURE;
    return( true );
}

static bool ProcReadOnly( void )
/******************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_READ_ONLY;
    FmtData.u.os2fam.seg_flags->specified |= SEG_READ_SPECD;
    return( true );
}

static bool ProcReadWrite( void )
/******************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_RFLAG ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_READ_ONLY;
    FmtData.u.os2fam.seg_flags->specified |= SEG_READ_SPECD;
    return( true );
}

static bool ProcConforming( void )
/********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_CONFORMING ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_CONFORMING;
    FmtData.u.os2fam.seg_flags->specified |= SEG_CONFORMING;
    return( true );
}

static bool ProcNonConforming( void )
/***********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_CONFORMING ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_CONFORMING;
    FmtData.u.os2fam.seg_flags->specified |= SEG_CONFORMING;
    return( true );
}

static bool ProcMovable( void )
/*****************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_MOVABLE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_MOVABLE;
    FmtData.u.os2fam.seg_flags->specified |= SEG_MOVABLE;
    return( true );
}

static bool ProcFixed( void )
/*****************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_MOVABLE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_MOVABLE;
    FmtData.u.os2fam.seg_flags->specified |= SEG_MOVABLE;
    return( true );
}

static bool ProcDiscardable( void )
/*********************************/
{
    FmtData.u.os2fam.seg_flags->flags |= SEG_DISCARD;
    return( true );
}

static bool ProcNonDiscardable( void )
/*********************************/
{
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_DISCARD;
    return( true );
}

static bool ProcInvalid( void )
/*****************************/
{
    FmtData.u.os2fam.seg_flags->flags |= SEG_INVALID;
    return( true );
}

static bool ProcContiguous( void )
/********************************/
{
    FmtData.u.os2fam.seg_flags->flags |= SEG_CONTIGUOUS;
    return( true );
}

static bool ProcOS2Dynamic( void )
/********************************/
{
//    FmtData.u.os2fam.seg_flags->flags |= SEG_DISCARD;
    return( true );
}

static bool ProcPermanent( void )
/*******************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_RESIDENT ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_RESIDENT;
    FmtData.u.os2fam.seg_flags->specified |= SEG_RESIDENT;
    return( true );
}

static bool ProcNonPermanent( void )
/**********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_RESIDENT ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_RESIDENT;
    FmtData.u.os2fam.seg_flags->specified |= SEG_RESIDENT;
    return( true );
}

static bool ProcPageable( void )
/******************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_NOPAGE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags &= ~SEG_NOPAGE;
    FmtData.u.os2fam.seg_flags->specified |= SEG_NOPAGE;
    return( true );
}

static bool ProcNonPageable( void )
/*********************************/
{
    if( FmtData.u.os2fam.seg_flags->specified & SEG_NOPAGE ) {
        LnkMsg( LOC+LINE+WRN+MSG_SEG_FLAG_MULT_DEFD, NULL );
    }
    FmtData.u.os2fam.seg_flags->flags |= SEG_NOPAGE;
    FmtData.u.os2fam.seg_flags->specified |= SEG_NOPAGE;
    return( true );
}

static parse_entry  SegDesc[] = {
    "Class",            ProcOS2Class,       MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "TYpe",             ProcSegType,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    NULL
};

static parse_entry  SegModel[] = {
    "PReload",          ProcPreload,        MK_OS2 | MK_WIN_VXD, 0,
    "LOadoncall",       ProcLoadoncall,     MK_OS2 | MK_WIN_VXD, 0,
    "Iopl",             ProcIopl,           MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "NOIopl",           ProcNoIopl,         MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "EXECUTEOnly",      ProcExecuteonly,    MK_OS2, 0,
    "EXECUTERead",      ProcExecuteread,    MK_OS2, 0,
    "SHared",           ProcShared,         MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "NONShared",        ProcNonShared,      MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "READOnly",         ProcReadOnly,       MK_OS2, 0,
    "READWrite",        ProcReadWrite,      MK_OS2, 0,
    "CONforming",       ProcConforming,     MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "NONConforming",    ProcNonConforming,  MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "MOVeable",         ProcMovable,        MK_OS2_16BIT, 0,
    "FIXed",            ProcFixed,          MK_WINDOWS, 0,
    "DIScardable",      ProcDiscardable,    MK_WINDOWS | MK_WIN_VXD, 0,
    "NONDiscardable",   ProcNonDiscardable, MK_WIN_VXD, 0,
    "INValid",          ProcInvalid,        MK_OS2_LE | MK_OS2_LX, 0,
    "RESident",         ProcPermanent,      MK_OS2_LE | MK_OS2_LX | MK_WIN_VXD, 0,
    "CONTiguous",       ProcContiguous,     MK_OS2_LE | MK_OS2_LX, 0,
    "DYNamic",          ProcOS2Dynamic,     MK_OS2_LE | MK_OS2_LX, 0,
    "PERManent",        ProcPermanent,      MK_OS2_LE | MK_OS2_LX, 0,
    "NONPERManent",     ProcNonPermanent,   MK_OS2_LE | MK_OS2_LX, 0,
    "PAGEable",         ProcPageable,       MK_PE, 0,
    "NONPageable",      ProcNonPageable,    MK_PE, 0,
    NULL
};

static bool getsegflags( void )
/*****************************/
{
    os2_seg_flags   *entry;

    Token.thumb = true;
    _ChkAlloc( entry, sizeof( os2_seg_flags ) );
    entry->specified = 0;
    entry->flags = FmtData.def_seg_flags;   // default value.
    entry->name = NULL;
    entry->type = SEGFLAG_SEGMENT;
    entry->next = FmtData.u.os2fam.seg_flags;
    FmtData.u.os2fam.seg_flags = entry;
    ProcOne( SegDesc, SEP_NO );             // look for an optional segdesc
    if( entry->type != SEGFLAG_CODE && entry->type != SEGFLAG_DATA ) {
        if( !GetToken( SEP_NO, TOK_INCLUDE_DOT ) ) {
            FmtData.u.os2fam.seg_flags = entry->next;
            _LnkFree( entry );
            return( false );
        }
        entry->name = tostring();
    }
    while( ProcOne( SegModel, SEP_NO ) ) {
    }
    return( true );
}

bool ProcOS2Segment( void )
/*************************/
{
    return( ProcArgList( getsegflags, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "COMmit" Directive
 ****************************************************************/

static bool ProcCommitStack( void )
/*********************************/
{
    return( GetLong( &FmtData.u.pe.stackcommit ) );
}

static bool ProcCommitHeap( void )
/********************************/
{
    return( GetLong( &FmtData.u.pe.heapcommit ) );
}

static parse_entry  CommitKeywords[] = {
    "STack",        ProcCommitStack,    MK_PE, 0,
    "Heap",         ProcCommitHeap,     MK_PE, 0,
    NULL
};

static bool AddCommit( void )
/***************************/
{
    Token.thumb = true;
    return( ProcOne( CommitKeywords, SEP_NO ) );
}

bool ProcCommit( void )
/**********************
 * set NT stack and heap commit sizes.
 */
{
    return( ProcArgList( AddCommit, TOK_INCLUDE_DOT ) );
}


/****************************************************************
 * "RUntime" Directive
 ****************************************************************/

static void GetSubsystemVersion( void )
/*************************************/
{
    version_state   result;
    version_block   vb;

    /* set required limits, 0 = no limit */
    vb.major = 0;
    vb.minor = 0;
    vb.revision = 0;
    vb.message = "SUBSYSTEM";
    result = GetGenVersion( &vb, GENVER_MAJOR | GENVER_MINOR, false );
    if( result != GENVER_ERROR ) {
        FmtData.u.pe.sub_specd = true;
        FmtData.u.pe.submajor = vb.major;
        FmtData.u.pe.subminor = vb.minor;
    }
}

static bool ProcRunNative( void )
/*******************************/
{
    FmtData.u.pe.subsystem = PE_SS_NATIVE;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunWindows( void )
/********************************/
{
    FmtData.u.pe.subsystem = PE_SS_WINDOWS_GUI;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunConsole( void )
/********************************/
{
    FmtData.u.pe.subsystem = PE_SS_WINDOWS_CHAR;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunPosix( void )
/******************************/
{
    FmtData.u.pe.subsystem = PE_SS_POSIX_CHAR;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunOS2( void )
/****************************/
{
    FmtData.u.pe.subsystem = PE_SS_OS2_CHAR;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunDosstyle( void )
/*********************************/
{
    FmtData.u.pe.subsystem = PE_SS_PL_DOSSTYLE;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunRDOS( void )
/*****************************/
{
    FmtData.u.pe.subsystem = PE_SS_RDOS;
    GetSubsystemVersion();
    return( true );
}

static bool ProcRunEFIBoot( void )
/********************************/
{
    Extension = E_EFI;
    FmtData.u.pe.subsystem = PE_SS_EFI_BOOT;
    GetSubsystemVersion();
    return( true );
}

static parse_entry  RunOptions[] = {
    "NATive",       ProcRunNative,      MK_PE, 0,
    "WINdows",      ProcRunWindows,     MK_PE, 0,
    "CONsole",      ProcRunConsole,     MK_PE, 0,
    "POSix",        ProcRunPosix,       MK_PE, 0,
    "OS2",          ProcRunOS2,         MK_PE, 0,
    "DOSstyle",     ProcRunDosstyle,    MK_PE, 0,
    "RDOS",         ProcRunRDOS,        MK_PE, 0,
    "EFIBoot",      ProcRunEFIBoot,     MK_PE, 0,
    NULL
};

bool ProcOS2Runtime( void )
/*************************/
{
    return( ProcOne( RunOptions, SEP_NO ) );
}


/****************************************************************
 * "Format" Directive
 ****************************************************************/

static bool ProcInitGlobal( void )
/********************************/
{
    FmtData.u.os2fam.flags &= ~INIT_INSTANCE_FLAG;
    return( true );
}

static bool ProcInitInstance( void )
/**********************************/
{
    FmtData.u.os2fam.flags |= INIT_INSTANCE_FLAG;
    return( true );
}

static bool ProcInitThread( void )
/********************************/
{
    FmtData.u.os2fam.flags |= INIT_THREAD_FLAG;
    return( true );
}

static bool ProcTermGlobal( void )
/********************************/
{
    FmtData.u.os2fam.flags &= ~TERM_INSTANCE_FLAG;
    return( true );
}

static bool ProcTermInstance( void )
/**********************************/
{
    FmtData.u.os2fam.flags |= TERM_INSTANCE_FLAG;
    return( true );
}

static bool ProcTermThread( void )
/********************************/
{
    FmtData.u.os2fam.flags |= TERM_THREAD_FLAG;
    return( true );
}

static parse_entry  Init_Keywords[] = {
    "INITGlobal",   ProcInitGlobal,     MK_OS2 | MK_PE, 0,
    "INITInstance", ProcInitInstance,   MK_OS2 | MK_PE, 0,
    "INITThread",   ProcInitThread,     MK_PE, 0,
    NULL
};

static parse_entry  Term_Keywords[] = {
    "TERMGlobal",   ProcTermGlobal,     MK_OS2_LE | MK_OS2_LX | MK_PE, 0,
    "TERMInstance", ProcTermInstance,   MK_OS2_LE | MK_OS2_LX | MK_PE, 0,
    "TERMThread",   ProcTermThread,     MK_PE, 0,
    NULL
};

static bool ProcOS2DLL( void )
/****************************/
{
    Extension = E_DLL;
    FmtData.dll = true;
    if( FmtData.type & MK_WINDOWS ) {
        FmtData.u.os2fam.flags &= ~MULTIPLE_AUTO_DATA;
        FmtData.u.os2fam.flags |= SINGLE_AUTO_DATA;
        FmtData.def_seg_flags |= SEG_PURE | SEG_MOVABLE;
    }
    if( ProcOne( Init_Keywords, SEP_NO ) ) {
        if( !ProcOne( Term_Keywords, SEP_NO ) ) {
            if( FmtData.u.os2fam.flags & INIT_INSTANCE_FLAG ) {
                FmtData.u.os2fam.flags |= TERM_INSTANCE_FLAG;
            }
        }
    }
    return( true );
}

static bool ProcLX( void )
/************************/
{
    return( true );
}

static bool ProcLE( void )
/************************/
{
    return( true );
}

static bool ProcTNT( void )
/*************************/
{
    FmtData.u.pe.tnt = true;
    return( true );
}

static parse_entry  PESubFormats[] = {
    "TNT",          ProcTNT,            MK_PE, 0,
    NULL
};

static bool ProcPE( void )
/************************/
{
    ProcOne( PESubFormats, SEP_NO );
    FmtData.u.os2fam.heapsize = PE_DEF_HEAP_SIZE;   // another arbitrary non-zero default
    FmtData.u.os2fam.segment_shift = 9;             // 512 byte arbitrary rounding
    FmtData.u.pe.heapcommit = PE_DEF_HEAP_COMMIT;   // arbitrary non-zero default.
    FmtData.u.pe.stackcommit = DEF_VALUE;
    return( true );
}

static bool ProcDynamicDriver( void )
/***********************************/
{
    FmtData.u.os2fam.flags |= VIRT_DEVICE;
    return( true );
}

static bool ProcStaticDriver( void )
/**********************************/
{
    FmtData.u.os2fam.flags |= PHYS_DEVICE;
    return( true );
}

static parse_entry  VXDSubFormats[] = {
    "DYNamic",      ProcDynamicDriver,  MK_WIN_VXD, 0,
    "STATic",       ProcStaticDriver,   MK_WIN_VXD, 0,
    NULL
};

static bool ProcVXD( void )
/*************************/
{
    ProcOne( VXDSubFormats, SEP_NO );
    FmtData.dll = true;
    return( true );
}

static bool ProcMemory( void )
/****************************/
{
    FmtData.u.os2fam.flags |= CLEAN_MEMORY;
    return( true );
}

static bool ProcFont( void )
/**************************/
{
    FmtData.u.os2fam.flags |= PROPORTIONAL_FONT;
    return( true );
}

static bool ProcPM( void )
/************************/
{
    FmtData.u.os2fam.flags |= PM_APPLICATION;
    return( true );
}

static bool ProcPMCompatible( void )
/**********************************/
{
    FmtData.u.os2fam.flags |= PM_COMPATIBLE;
    return( true );
}

static bool ProcPMFullscreen( void )
/**********************************/
{
    FmtData.u.os2fam.flags |= PM_NOT_COMPATIBLE;
    return( true );
}

static bool ProcPhysDevice( void )
/********************************/
{
    Extension = E_DLL;
    FmtData.dll = true;
    FmtData.u.os2fam.flags |= PHYS_DEVICE;
    return( true );
}

static bool ProcVirtDevice( void )
/********************************/
{
    Extension = E_DLL;
    FmtData.dll = true;
    FmtData.u.os2fam.flags |= VIRT_DEVICE;
    return( true );
}

static parse_entry  WindowsFormatOptions[] = {
    "MEMory",       ProcMemory,         MK_WINDOWS,             0,
    "FOnt",         ProcFont,           MK_WINDOWS,             0,
    NULL
};

static parse_entry  WindowsSubFormats[] = {
    "DLl",          ProcOS2DLL,         MK_WINDOWS | MK_PE,     0,
    NULL
};

static parse_entry  WindowsFormats[] = {
    "NT",           ProcPE,             MK_PE,                  0,
    "PE",           ProcPE,             MK_PE,                  0,
    "VXD",          ProcVXD,            MK_WIN_VXD,             0,
    NULL
};

static parse_entry  OS2SubFormats[] = {
    "DLl",          ProcOS2DLL,         MK_ONLY_OS2,            0,
    "PHYSdevice",   ProcPhysDevice,     MK_OS2_LE | MK_OS2_LX,  0,
    "VIRTdevice",   ProcVirtDevice,     MK_OS2_LE | MK_OS2_LX,  0,
    "PM",           ProcPM,             MK_ONLY_OS2,            0,
    "PMCompatible", ProcPMCompatible,   MK_ONLY_OS2,            0,
    "FULLscreen",   ProcPMFullscreen,   MK_ONLY_OS2,            0,
    NULL
};

static parse_entry  OS2Formats[] = {
    "FLat",         ProcLX,             MK_OS2_LX,              0,
    "LE",           ProcLE,             MK_OS2_LE,              0,
    "LX",           ProcLX,             MK_OS2_LX,              0,
    NULL
};

bool ProcOS2Format( void )
/*************************
 * process the format os2 or format windows directives
 */
{
    Extension = E_LOAD;
    ProcOne( OS2Formats, SEP_NO );
    ProcOne( OS2SubFormats, SEP_NO );
    if( FmtData.type & MK_OS2_LX ) {
        if( FmtData.dll ) {
            FmtData.u.os2fam.gen_int_relocs = true;
        }
    }
    if( FmtData.type & MK_ONLY_OS2_16 ) {   // if no 32-bit thing specd
        HintFormat( MK_ONLY_OS2_16 );       // make sure 16-bit is what we get
        if( FmtData.dll ) {
            FmtData.u.os2fam.flags &= ~MULTIPLE_AUTO_DATA;
            FmtData.u.os2fam.flags |= SINGLE_AUTO_DATA;
        }
    }
    return( true );
}

bool ProcWindowsFormat( void )
/*****************************
 * process the format windows directives
 */
{
    Extension = E_LOAD;
    ProcOne( WindowsFormats, SEP_NO );
    ProcOne( WindowsSubFormats, SEP_NO );
    if( FmtData.type & MK_WINDOWS ) {
        while( ProcOne( WindowsFormatOptions, SEP_NO ) ) {
            // loop all options
        }
    }
    return( true );
}

#endif
