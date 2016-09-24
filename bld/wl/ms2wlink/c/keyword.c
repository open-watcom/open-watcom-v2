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
* Description:  Routines for parsing Microsoft keywords.
*
****************************************************************************/


#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "ms2wlink.h"
#include "command.h"

#include "clibext.h"

static void             (*MultiLine)( void ) = NULL;
static char             *OptionBuffer;
static size_t           BufferLeft;
static bool             SegWarning = false;
static bool             GotModName = false;
static bool             GotOvl;

/* An entry for parsing .def files. */
typedef struct parse_entry {
    const char          *keyword;
    void                (*rtn)( void );
    unsigned char       minlen;
} parse_entry;

/* An entry for parsing command line switches. */
typedef struct switch_entry {
    const char          *keyword;
    void                (*rtn)( const char *arg );
    unsigned char       minlen;
} switch_entry;

static void ProcAppLoader( void );
static void ProcCode( void );
static void ProcData( void );
static void ProcDescription( void );
static void ProcExeType( void );
static void ProcExports( void );
static void ProcFunctions( void );
static void ProcHeapsize( void );
static void ProcImports( void );
static void ProcInclude( void );
static void ProcLibrary( void );
static void ProcName( void );
static void ProcOld( void );
static void ProcProtMode( void );
static void ProcRealMode( void );
static void ProcDefSegments( void );
static void ProcStackSize( void );
static void ProcStub( void );
static void ProcVirtual( void );

static  parse_entry     DefStatements[] = {
    "apploader",        ProcAppLoader,      9,
    "code",             ProcCode,           4,
    "data",             ProcData,           4,
    "description",      ProcDescription,    11,
    "exetype",          ProcExeType,        7,
    "exports",          ProcExports,        7,
    "functions",        ProcFunctions,      9,
    "heapsize",         ProcHeapsize,       8,
    "imports",          ProcImports,        7,
    "include",          ProcInclude,        7,
    "library",          ProcLibrary,        7,
    "name",             ProcName,           4,
    "old",              ProcOld,            3,
    "protmode",         ProcProtMode,       8,
    "realmode",         ProcRealMode,       8,
    "segments",         ProcDefSegments,    8,
    "stacksize",        ProcStackSize,      9,
    "stub",             ProcStub,           4,
    "virtual",          ProcVirtual,        7,
    NULL
};

static void ProcPreload( void );
static void ProcLoadOnCall( void );
static void ProcExecuteOnly( void );
static void ProcExecuteRead( void );
static void ProcIopl( void );
static void ProcNoIopl( void );
static void ProcConforming( void );
static void ProcNonConforming( void );
static void ProcMoveable( void );
static void ProcFixed( void );
static void ProcDiscardable( void );
static void ProcNonDiscardable( void );

static  parse_entry     CodeAttributes[] = {
    "preload",          ProcPreload,        7,
    "loadoncall",       ProcLoadOnCall,     10,
    "executeonly",      ProcExecuteOnly,    11,
    "executeread",      ProcExecuteRead,    11,
    "iopl",             ProcIopl,           4,
    "noiopl",           ProcNoIopl,         6,
    "conforming",       ProcConforming,     10,
    "nonconforming",    ProcNonConforming,  13,
    "moveable",         ProcMoveable,       8,
    "movable",          ProcMoveable,       7,
    "fixed",            ProcFixed,          5,
    "discardable",      ProcDiscardable,    11,
    "nondiscardable",   ProcNonDiscardable, 14,
    NULL
};

static void ProcReadOnly( void );
static void ProcReadWrite( void );
static void ProcShared( void );
static void ProcNonShared( void );

static  parse_entry     SegAttributes[] = {
    "preload",          ProcPreload,        7,
    "loadoncall",       ProcLoadOnCall,     10,
    "executeonly",      ProcExecuteOnly,    11,
    "executeread",      ProcExecuteRead,    11,
    "iopl",             ProcIopl,           4,
    "noiopl",           ProcNoIopl,         6,
    "conforming",       ProcConforming,     10,
    "nonconforming",    ProcNonConforming,  13,
    "readonly",         ProcReadOnly,       8,
    "readwrite",        ProcReadWrite,      9,
    "shared",           ProcShared,         6,
    "nonshared",        ProcNonShared,      9,
    "moveable",         ProcMoveable,       7,
    "fixed",            ProcFixed,          5,
    "discardable",      ProcDiscardable,    11,
    "nondiscardable",   ProcNonDiscardable, 14,
    NULL
};

static void ProcNone( void );
static void ProcSingle( void );
static void ProcMultiple( void );

static  parse_entry     DataAttributes[] = {
    "none",             ProcNone,           4,
    "single",           ProcSingle,         6,
    "multiple",         ProcMultiple,       8,
    "shared",           ProcShared,         6,
    "nonshared",        ProcNonShared,      9,
    "readonly",         ProcReadOnly,       8,
    "readwrite",        ProcReadWrite,      9,
    "iopl",             ProcIopl,           4,
    "noiopl",           ProcNoIopl,         6,
    "preload",          ProcPreload,        7,
    "loadoncall",       ProcLoadOnCall,     10,
    "moveable",         ProcMoveable,       7,
    "fixed",            ProcFixed,          5,
    "discardable",      ProcDiscardable,    11,
    "nondiscardable",   ProcNonDiscardable, 14,
    NULL
};

static void ProcClass( void );
static void ProcOvl( void );

static  parse_entry     SegOptions[] = {
    "class",            ProcClass,      5,
    "ovl",              ProcOvl,        3,
    NULL
};
static void NullRoutine( void );
static void SetWindows( void );
static void SetWindowsVxD( void );
static void SetWindowsVxDDyn( void );
static void SetOS2( void );

static  parse_entry     ExeTypeKeywords[] = {
    "os2",              SetOS2,             3,
    "windows",          SetWindows,         7,
    "dos4",             NullRoutine,        4,
    "dev386",           SetWindowsVxD,      6,
    "dynamic",          SetWindowsVxDDyn,   7,
    "unknown",          NullRoutine,        7,
    NULL
};

static void ProcWindowApi( void );
static void ProcWindowCompat( void );
static void ProcNotWindowCompat( void );
static void ProcNewFiles( void );

static  parse_entry     ApplicationTypes[] = {
    "windowapi",        ProcWindowApi,      9,
    "windowcompat",     ProcWindowCompat,   12,
    "notwindowcompat",  ProcNotWindowCompat,15,
    "newfiles",         ProcNewFiles,       8,
    NULL
};

static void ProcPrivateLib( void );

static  parse_entry     LibraryTypes[] = {
    "privatelib",       ProcPrivateLib,     10,
    NULL
};

static void ProcDevice( void );

static  parse_entry     DeviceKeyword[] = {
    "device",           ProcDevice,         6,
    NULL
};

static void ProcHelp( const char *arg );
static void ProcAlignment( const char *arg );
static void ProcBatch( const char *arg );
static void ProcCodeview( const char *arg );
static void ProcCParMaxAlloc( const char *arg );
static void ProcDosseg( const char *arg );
static void ProcDSAllocate( const char *arg );
static void ProcDynamic( const char *arg );
static void ProcExePack( const char *arg );
static void ProcFarCallTrans( const char *arg );
static void ProcHigh( const char *arg );
static void ProcIncremental( const char *arg );
static void ProcInformation( const char *arg );
static void ProcLineNumbers( const char *arg );
static void ProcMap( const char *arg );
static void ProcNoDefLibSearch( const char *arg );
static void ProcNoExtDictSearch( const char *arg );
static void ProcNoFarCallTrans( const char *arg );
static void ProcNoGroupAssoc( const char *arg );
static void ProcNoIgnoreCase( const char *arg );
static void ProcNoLogo( const char *arg );
static void ProcNoNullsDosseg( const char *arg );
static void ProcNoPackCode( const char *arg );
static void ProcNoPackFunctions( const char *arg );
static void ProcOldOverlay( const char *arg );
static void ProcOnError( const char *arg );
static void ProcOverlayInterrupt( const char *arg );
static void ProcPackCode( const char *arg );
static void ProcPackData( const char *arg );
static void ProcPadCode( const char *arg );
static void ProcPadData( const char *arg );
static void ProcPause( const char *arg );
static void ProcPMType( const char *arg );
static void ProcQuickLibrary( const char *arg );
static void ProcSegments( const char *arg );
static void ProcStack( const char *arg );
static void ProcTiny( const char *arg );
static void ProcWarnFixup( const char *arg );


static  switch_entry        OptionsTable[] = {
/*  option name               processing routine    shortest match */
    "?",                        ProcHelp,               1,
    "alignment",                ProcAlignment,          1,
    "batch",                    ProcBatch,              1,
    "codeview",                 ProcCodeview,           2,
    "cparmaxalloc",             ProcCParMaxAlloc,       2,
    "dosseg",                   ProcDosseg,             4,
    "dsallocate",               ProcDSAllocate,         2,
    "dynamic",                  ProcDynamic,            2,
    "exepack",                  ProcExePack,            1,
    "farcalltranslation",       ProcFarCallTrans,       1,
    "help",                     ProcHelp,               2,
    "high",                     ProcHigh,               2,
    "incremental",              ProcIncremental,        3,
    "information",              ProcInformation,        1,
    "linenumbers",              ProcLineNumbers,        2,
    "map",                      ProcMap,                1,
    "nodefaultlibrarysearch",   ProcNoDefLibSearch,     3,
    "noextdictionary",          ProcNoExtDictSearch,    3,
    "nofarcalltranslation",     ProcNoFarCallTrans,     3,
    "nogroupassociation",       ProcNoGroupAssoc,       3,
    "noignorecase",             ProcNoIgnoreCase,       3,
    "nologo",                   ProcNoLogo,             3,
    "nonullsdosseg",            ProcNoNullsDosseg,      3,
    "nopackcode",               ProcNoPackCode,         7,
    "nopackfunctions",          ProcNoPackFunctions,    7,
    "oldoverlay",               ProcOldOverlay,         2,
    "onerror",                  ProcOnError,            2,
    "overlayinterrupt",         ProcOverlayInterrupt,   2,
    "packcode",                 ProcPackCode,           5,
    "packdata",                 ProcPackData,           5,
    "padcode",                  ProcPadCode,            4,
    "paddata",                  ProcPadData,            4,
    "pause",                    ProcPause,              3,
    "pmtype",                   ProcPMType,             2,
    "quicklibrary",             ProcQuickLibrary,       1,
    "segments",                 ProcSegments,           2,
    "stack",                    ProcStack,              2,
    "tiny",                     ProcTiny,               1,
    "warnfixup",                ProcWarnFixup,          1,
    NULL
};

static bool GetNumberStr( unsigned long *val, const char *s, size_t len )
/***********************************************************************/
// kinda like strtoul(), but not quite...
{
    unsigned long   value;
    unsigned        radix;
    bool            isvalid;
    bool            isdig;
    char            ch;

    value = 0ul;
    radix = 10;
    if( *s == '0' ) {
        --len;
        radix = 8;
        if( tolower( *++s ) == 'x') {
            radix = 16;
            ++s;
            --len;
        }
    }
    for( ; len != 0; --len ) {
        ch = tolower( *s++ );
        if( ch == 'k' ) {         // constant of the form 64k
            if( len > 1 ) {
                return( false );
            } else {
                value <<= 10;        // value = value * 1024;
            }
        } else {
            isdig = isdigit( ch ) != 0;
            if( radix == 10 ) {
                isvalid = isdig;
            } else if( radix == 8 ) {
                if( ch == '8' || ch == '9' || !isdig ) {
                    isvalid = false;
                } else {
                    isvalid = true;
                }
            } else {
                isvalid = isxdigit( ch ) != 0;
            }
            if( !isvalid ) {
                return( false );
            }
            value *= radix;
            if( isdig ) {
                value += ch - '0';
            } else {
                value += ch - 'a' + 10;
            }
        }
    }
    *val = value;
    return( true );
}

extern bool GetNumber( unsigned long *val )
/*****************************************/
{
    return( GetNumberStr( val, CmdFile->token, CmdFile->len ) );
}


static void CheckNum( const char *arg )
/*************************************/
{
    unsigned long       value;

    /* Check but ignore numeric argument. */
    if( arg != NULL && !GetNumberStr( &value, arg , strlen( arg ) ) ) {
        Warning( "invalid numeric value", OPTION_SLOT );
    }
}

static bool ProcessKeyList( parse_entry *entry, const char *arg, size_t arg_len )
/*******************************************************************************/
{
    const char          *key;
    const char          *ptr;
    size_t              len;
    size_t              plen;

    for( ; entry->keyword != NULL; ++entry ) {
        key = entry->keyword;
        ptr = arg;
        len = 0;
        plen = arg_len;
        for(;;) {
            if( plen == 0 && len >= entry->minlen ) {
                MultiLine = NULL;               // for processdefcommand.
                (*entry->rtn)();
                return( true );
            }
            if( *key == '\0' || tolower( *ptr ) != *key )
                break;
            ptr++;
            key++;
            len++;
            plen--;
        }
        /* here if this is no match */
    }
    /* here if no match in table */
    return( false );
}

static bool ProcessKeyword( parse_entry *entry )
/**********************************************/
// returns true if keyword found.
{
    bool    ret;

    if( !MakeToken( SEP_NO, false ) ) {
        return( false );
    }
    ret = ProcessKeyList( entry, CmdFile->token, CmdFile->len );
    if( !ret ) {
        CmdFile->current = CmdFile->token;     // reparse the token later.
    }
    return( ret );
}

// the microsoft definitions file.

extern void ProcessDefCommand( void )
/***********************************/
{
    if( !ProcessKeyword( DefStatements ) ) {
        if( MultiLine != NULL ) {
            (*MultiLine)();
        } else {
            DirectiveError();
        }
    }
}

static void SetWindows( void )
/****************************/
{
    unsigned long       bogus;

    FmtType = FMT_WINDOWS;
    if( !MakeToken( SEP_NO, false ) )
        return;
    if( !GetNumber( &bogus ) ) {
        CmdFile->current = CmdFile->token;      // reparse the token later
    }
}

static void SetWindowsVxD( void )
/****************************/
{
    FmtType = FMT_WINVXD;
    FmtInfo = NO_EXTRA;
}

static void SetWindowsVxDDyn( void )
/****************************/
{
    FmtType = FMT_WINVXDDYN;
    FmtInfo = NO_EXTRA;
}

static void SetOS2( void )
/************************/
{
    FmtType = FMT_OS2;
}

static void ProcAppLoader( void )
/*******************************/
{
    NotSupported( "apploader" );
    if( !MakeToken( SEP_QUOTE, true ) ) {
        MakeToken( SEP_NO, true );
    }
}

#define CODE_BUFFER_LEN 105

static void ProcCode( void )
/**************************/
{
    char    buffer[ CODE_BUFFER_LEN ];
    char    *result;
    size_t  len;

    memcpy( buffer, "segment type code", 17 );
    OptionBuffer = buffer + 17;
    BufferLeft = CODE_BUFFER_LEN - 18;
    if( !ProcessKeyword( CodeAttributes ) ) {
        Warning( "argument for code statement not valid", OPTION_SLOT );
    } else {    // get the rest of the attributes.
        while( ProcessKeyword( CodeAttributes ) )
            {}     // NULL statement.
        if( BufferLeft != CODE_BUFFER_LEN - 18 ) {      // attribute spec'd.
            len = CODE_BUFFER_LEN - BufferLeft - 1;
            result = MemAlloc( CODE_BUFFER_LEN - BufferLeft + 1 );
            memcpy( result, buffer, len );
            *(result + len) = '\0';
            AddCommand( result, OPTION_SLOT, false );
        }
    }
}

static void ProcData( void )
/**************************/
// NYI: serious copied code from ProcCode.  Should be united into one routine.
{
    char    buffer[ CODE_BUFFER_LEN ];
    char    *result;
    size_t  len;

    memcpy( buffer, "segment type data", 17 );
    OptionBuffer = buffer + 17;
    BufferLeft = CODE_BUFFER_LEN - 18;
    if( !ProcessKeyword( DataAttributes ) ) {
        Warning( "argument for data statement not valid", OPTION_SLOT );
    } else {    // get the rest of the attributes.
        while( ProcessKeyword( DataAttributes ) )
            {}     // NULL statement.
        if( BufferLeft != CODE_BUFFER_LEN - 18 ) {      // attribute spec'd.
            len = CODE_BUFFER_LEN - BufferLeft - 1;
            result = MemAlloc( CODE_BUFFER_LEN - BufferLeft + 1 );
            memcpy( result, buffer, len );
            *(result + len) = '\0';
            AddCommand( result, OPTION_SLOT, false );
        }
    }
}

static void AddToBuffer( const char *cmd, size_t len )
/****************************************************/
{
    if( BufferLeft < ( len + 1 ) ) {
        Warning( "too many code attributes", OPTION_SLOT );
    } else {
        BufferLeft -= len + 1;
        *OptionBuffer++ = ' ';
        memcpy( OptionBuffer, cmd, len );
        OptionBuffer += len;
    }
}

static void ProcPreload( void )
/*****************************/
{
    AddToBuffer( "preload", 7 );
}

static void ProcLoadOnCall( void )
/********************************/
{
    AddToBuffer( "loadoncall", 10 );
}

static void ProcExecuteOnly( void )
/*********************************/
{
    AddToBuffer( "executeonly", 11 );
}

static void ProcExecuteRead( void )
/*********************************/
{
    AddToBuffer( "executeread", 11 );
}

static void ProcIopl( void )
/**************************/
{
    AddToBuffer( "iopl", 4 );
}

static void ProcNoIopl( void )
/****************************/
{
    AddToBuffer( "noiopl", 6 );
}

static void ProcConforming( void )
/********************************/
{
    AddToBuffer( "conforming", 10 );
}

static void ProcNonConforming( void )
/***********************************/
{
    AddToBuffer( "nonconforming", 13 );
}

static void ProcMoveable( void )
/*****************************/
{
    AddToBuffer( "moveable", 8 );
    ImplyFormat( FMT_WINDOWS );
}

static void ProcFixed( void )
/***************************/
{
    AddToBuffer( "fixed", 5 );
    ImplyFormat( FMT_WINDOWS );
}

static void ProcDiscardable( void )
/*********************************/
{
    AddToBuffer( "discardable", 11 );
    ImplyFormat( FMT_WINDOWS );
}

static void ProcNonDiscardable( void )
/************************************/
{
    AddToBuffer( "nondiscardable", 14 );
    ImplyFormat( FMT_WINDOWS );
//    NotNecessary( "nondiscardable" );
}

static void ProcNone( void )
/**************************/
{
    AddOption( "noautodata" );
}

static void ProcSingle( void )
/****************************/
{
    AddOption( "oneautodata" );
}

static void ProcMultiple( void )
/******************************/
{
    AddOption( "manyautodata" );
}

static void ProcReadOnly( void )
/******************************/
{
    AddToBuffer( "readonly", 8 );
}

static void ProcReadWrite( void )
/*******************************/
{
    AddToBuffer( "readwrite", 9 );
}

static void ProcShared( void )
/****************************/
{
    AddToBuffer( "shared", 6 );
}

static void ProcNonShared( void )
/*******************************/
{
    AddToBuffer( "nonshared", 11 );
}

static void ProcDescription( void )
/*********************************/
{
    #define PREFIX      "description '"
    char        *msg;

    MakeToken( SEP_QUOTE, true );
    msg = alloca( CmdFile->len + sizeof( PREFIX ) + 1 );
    memcpy( msg, PREFIX, sizeof( PREFIX ) );
    memcpy( msg + sizeof( PREFIX ) - 1, CmdFile->token, CmdFile->len );
    strcpy( msg + sizeof( PREFIX ) - 1 + CmdFile->len, "'" );
    AddOption( msg );
}

static void NullRoutine( void )
/*****************************/
{
}

static void ProcExeType( void )
/*****************************/
{
    ProcessKeyword( ExeTypeKeywords );           // skip the keywords
}

static void GetExport( void )
/***************************/
{
    char            *internal;
    size_t          intlen;
    char            *name;
    size_t          namelen;
    unsigned long   value;
    unsigned long   iopl;
    bool            isresident;
    bool            gottoken;
    bool            gotnodata;
    size_t          toklen;
    char            *command;
    char            *currloc;

    if( !MakeToken( SEP_NO, true ) ) {
        Warning( "invalid export name", OPTION_SLOT );
        return;
    }
    name = alloca( CmdFile->len );        // store it temporarily
    toklen = namelen = CmdFile->len;
    memcpy( name, CmdFile->token, namelen );
    internal = NULL;
    intlen = 0;
    if( MakeToken( SEP_EQUALS, true ) ) {   // got an internal name.
        internal = alloca( CmdFile->len );  // store it temporarily
        intlen = CmdFile->len;
        memcpy( internal, CmdFile->token, intlen );
        toklen += intlen + 1;        // +1 for = sign.
    }
    value = 0xFFFFF; // arbitrary >64K.
    if( MakeToken( SEP_AT, true ) ) {       // got an ordinal.
        if( !GetNumber( &value ) || value > (64 * 1024UL) ) {
            Warning( "export ordinal value is invalid", OPTION_SLOT );
            return;
        } else {
            toklen += 6;      // maximum integer length + dot
        }
    }
    isresident = false;
    EatWhite();
    gottoken = MakeToken( SEP_NO, true );
    if( gottoken ) {
        if( CmdFile->len == 12 && memicmp( CmdFile->token, "residentname", 12 ) == 0 ) {
            isresident = true;
            gottoken = MakeToken( SEP_NO, true );
            toklen += 9;           // length of resident + space.
        }
    }
    gotnodata = false;
    if( gottoken ) {
        if( memicmp( CmdFile->token, "nodata", 6 ) == 0 ) {
            gottoken = MakeToken( SEP_NO, true );
            gotnodata = true;
        }
    }
    iopl = 1024;      // arbitrary > 63
    if( gottoken ) {
        if( !GetNumber( &iopl ) || iopl > 63 ) {
            Warning( "iopl value is invalid", OPTION_SLOT );
        } else {
            toklen += 3;    // maximum iopl length + space.
        }
        if( !gotnodata ) {
            gottoken = MakeToken( SEP_NO, true );
        }
    }
    if( gottoken && !gotnodata ) {
        if( memicmp( CmdFile->token, "nodata", 6 ) == 0 ) {
            gotnodata = true;
        } else {
            CmdFile->current = CmdFile->token;      // reparse the token later
        }
    }
    toklen += 8;       // export keyword + space + nullchar;
    command = MemAlloc( toklen );
    memcpy( command, "export ", 7 );
    currloc = command + 7;
    memcpy( currloc, name, namelen );
    currloc += namelen;
    if( value <= (64*1024UL) ) {   // if an ordinal was specified....
        *currloc++ = '.';
        ultoa( value, currloc, 10 );
        while( *currloc != '\0' ) {    // find end of string.
            currloc++;
        }
    }
    if( internal != NULL ) {
        *currloc++ = '=';
        memcpy( currloc, internal, intlen );
        currloc += intlen;
    }
    if( isresident ) {
        *currloc++ = ' ';
        memcpy( currloc, "resident", 8 );
        currloc += 8;
    }
    if( iopl <= 63 ) {
        *currloc++ = ' ';
        ultoa( iopl * 2, currloc, 10 ); // convert iopl value to a byte value
    } else {
        *currloc = '\0';
    }
    AddCommand( command, OPTION_SLOT, false );
}

static void ProcExports( void )
/*****************************/
// indicate that a keyword with multiple lines of arguments is to be parsed.
{
    MultiLine = GetExport;
}

static void SkipFunction( void )
/******************************/
// this skips a function name
{
    MakeToken( SEP_SPACE, true );       // skip a function name
}

static void ProcFunctions( void )
/*******************************/
{
    Warning( "Functions style overlay management not directly translatable to watcom", OPTION_SLOT );
    MakeToken( SEP_COLON, false );       // skip number.
    MultiLine = SkipFunction;
}

static void ProcHeapsize( void )
/******************************/
{
    unsigned long value;

    if( !MakeToken( SEP_NO, true ) ) {
        Warning( "argument for heapsize not recognized", OPTION_SLOT );
    } else if( memicmp( CmdFile->token, "maxval", 6 ) == 0 ) {
        AddNumOption( "heapsize", 0xFFFF );
    } else if( !GetNumber( &value ) || value >= (64*1024UL) ) {
        Warning( "argument for heapsize not valid", OPTION_SLOT );
    } else {
        AddNumOption( "heapsize", value );
    }
}

static void GetImport( void )
/***************************/
{
    char            *first;
    size_t          firstlen;
    char            *second;
    size_t          secondlen;
    unsigned long   value;
    char            *result;
    size_t          toklen;
    char            *currloc;

    if( !MakeToken( SEP_NO, false ) ) {
        Warning( "import library name is invalid", OPTION_SLOT );
        return;
    }
    toklen = firstlen = CmdFile->len;
    first = alloca( firstlen );
    memcpy( first, CmdFile->token, firstlen );
    second = NULL;
    secondlen = 0;
    if( MakeToken( SEP_EQUALS, false ) ) {        // got an internal name.
        secondlen = CmdFile->len;
        second = alloca( secondlen );
        memcpy( second, CmdFile->token, secondlen );
        toklen += secondlen + 1;                        // name & = sign.
    }
    if( !MakeToken( SEP_PERIOD, true ) ) {
        Warning( "import entry is invalid", OPTION_SLOT );
        return;
    }
    value = 0xFFFFF;      // arbitrary > 64k.
    if( GetNumber( &value ) ) {
        if( second == NULL ) {
            Warning("must have an internal name when an ordinal is specified", OPTION_SLOT );
            return;
        } else if( value >= (64*1024UL) ) {
            Warning( "import ordinal out of range", OPTION_SLOT );
            return;
        }
        toklen += 6;      // maximum size of ordinal + dot.
    } else {
        toklen += CmdFile->len + 1;      // string length + dot.
    }
    toklen += 8;       // import keyword + space + nullchar;
    result = MemAlloc( toklen );
    memcpy( result, "import ", 7 );
    currloc = result + 7;
    if( second != NULL ) {       // got a internal name in first.
        memcpy( currloc, first, firstlen );
        currloc += firstlen;
        first = second;         // make sure module name is in first.
        firstlen = secondlen;
    } else {
        memcpy( currloc, CmdFile->token, CmdFile->len );
        currloc += CmdFile->len;
    }
    *currloc++ = ' ';
    memcpy( currloc, first, firstlen );   // module name
    currloc += firstlen;
    if( value < (64*1024UL) ) {
        *currloc++ = '.';
        ultoa( value, currloc, 10 );
    } else {
        if( second != NULL ) {
            *currloc++ = '.';
            memcpy( currloc, CmdFile->token, CmdFile->len );
            currloc += CmdFile->len;
        }
        *currloc = '\0';
    }
    AddCommand( result, OPTION_SLOT, false );
}

static void ProcImports( void )
/*****************************/
{
    MultiLine = GetImport;
}

static void ProcInclude ( void )
/******************************/
{
    if( MakeToken( SEP_QUOTE, true ) || MakeToken( SEP_NO, true ) ) {
        StartNewFile( ToString() );
        ParseDefFile();
    } else {
        Warning( "invalid include statement", OPTION_SLOT );
    }
}

static bool IsInitType( void )
/****************************/
{
    if( CmdFile->len == 10 && memicmp( CmdFile->token, "initglobal", 10 ) == 0 ) {
        FmtInfo = DLL_INITGLOBAL;
        return( true );
    } else if ( CmdFile->len == 12 && memicmp( CmdFile->token, "initinstance", 12 ) == 0 ) {
        FmtInfo = DLL_INITINSTANCE;
        return( true );
    }
    return( false );
}

static void ProcPrivateLib( void )
/********************************/
{
    NotSupported( "privatelib" );
}

static void ProcLibrary( void )
/*****************************/
{
    bool gotprivate;

    gotprivate = ProcessKeyword( LibraryTypes );
    FmtInfo = DLL_INITGLOBAL;
    if( !MakeToken( SEP_NO, true ) ) {
        return;
    }
    if( !IsInitType() ) {
        if( CmdFile->len > 8 ) {
            Warning( "module name too large", OPTION_SLOT );
        } else if( GotModName ) {
            Warning( "module name multiply defined" , OPTION_SLOT );
        } else {
            AddStringOption( "modname", CmdFile->token, CmdFile->len );
            GotModName = true;
        }
        if( !MakeToken( SEP_NO, true ) ) {
            return;
        }
    }
    if( !IsInitType() ) {
        DirectiveError();
    }
    if( !gotprivate ) {
        ProcessKeyword( LibraryTypes );
    }
}

static void ProcName( void )
/**************************/
{
    int index;

    if( !MakeToken( SEP_NO, true ) ) {
        return;
    }
    if( !ProcessKeyList( ApplicationTypes, CmdFile->token, CmdFile->len ) ) {
        if( GotModName ) {
            Warning( "module name multiply defined", OPTION_SLOT );
        } else {
            AddStringOption( "modname", CmdFile->token, CmdFile->len );
            GotModName = true;
        }
    }
    for( index = 0; index < 2; index++ ) {
        if( !MakeToken( SEP_NO, true ) ) {
            return;
        }
        if( !ProcessKeyList( ApplicationTypes, CmdFile->token, CmdFile->len ) ) {
            DirectiveError();
        }
    }
}

static void ProcWindowApi( void )
/*******************************/
{
    FmtInfo = APP_PM;
    ImplyFormat( FMT_OS2 );
}

static void ProcWindowCompat( void )
/**********************************/
{
    FmtInfo = APP_PMCOMPATIBLE;
    ImplyFormat( FMT_OS2 );
}

static void ProcNotWindowCompat( void )
/*************************************/
{
    FmtInfo = APP_FULLSCREEN;
    ImplyFormat( FMT_OS2 );
}

static void ProcNewFiles( void )
/******************************/
{
    AddOption( "newfiles" );
    ImplyFormat( FMT_OS2 );
}

static void ProcOld( void )
/*************************/
{
    if( !MakeToken( SEP_QUOTE, true ) ) {
        Warning( "argument for old not valid", OPTION_SLOT );
    } else {
        AddStringOption( "old", CmdFile->token, CmdFile->len );
    }
}

static void ProcProtMode( void )
/******************************/
{
    AddOption( "protmode" );
}

static void ProcRealMode( void )
/******************************/
{
    NotSupported( "realmode" );
}

static void ProcClass( void )
/***************************/
{
    if( !MakeToken( SEP_QUOTE, true ) ) {
        Warning( "class argument invalid", OPTION_SLOT );
    }
}

static void ProcOvl( void )
/*************************/
{
    MakeToken( SEP_COLON, false );
    Warning( "ovl option on segment directive not supported by wlink", OPTION_SLOT );
    GotOvl = true;
}

static void GetSegments( void )
/*****************************/
{
    char    *segname;
    size_t  seglen;
    char    buffer[ CODE_BUFFER_LEN ];
    char    *result;
    char    *currloc;
    size_t  len;

    GotOvl = false;
    if( MakeToken( SEP_QUOTE, true ) || MakeToken( SEP_NO, true ) ) {
        seglen = CmdFile->len;
        segname = alloca( seglen );
        memcpy( segname, CmdFile->token, seglen );
    } else {
        Warning( "segment argument not recognized", OPTION_SLOT );
        return;
    }
    if( ProcessKeyword( SegOptions ) ) {        // process the options
        ProcessKeyword( SegOptions );
    }
    OptionBuffer = buffer;
    BufferLeft = CODE_BUFFER_LEN;
    if( !ProcessKeyword( SegAttributes ) && !GotOvl ) {
        Warning( "no segment attributes specified", OPTION_SLOT );
    } else {    // get the rest of the attributes.
        while( ProcessKeyword( SegAttributes ) )
            {}     // NULL statement.
        if( BufferLeft != CODE_BUFFER_LEN && !GotOvl ) { // something spec'd.
            len = CODE_BUFFER_LEN - BufferLeft;
            result = MemAlloc( len + seglen + 12 );
            memcpy( result, "segment '", 9 );
            currloc = result + 9;
            memcpy( currloc, segname, seglen );
            currloc += seglen;
            *currloc++ = '\'';
            *currloc++ = ' ';
            memcpy( currloc, buffer, len );
            *(currloc + len) = '\0';
            AddCommand( result, OPTION_SLOT, false );
        }
    }
    MultiLine = GetSegments;
}

static void ProcDefSegments( void )
/*********************************/
// try to approximate the microsoft segment keyword.
{
    if( !SegWarning ) {
        SegWarning = true;
        Warning( "definition file segments keyword handled differently in WLINK", OPTION_SLOT );
    }
    MultiLine = GetSegments;
}

static void ProcStackSize( void )
/*******************************/
{
    unsigned long value;

    if( !MakeToken( SEP_NO, true ) ) {
        Warning( "argument for stacksize is invalid", OPTION_SLOT );
    } else if( !GetNumber( &value ) || value >= (64*1024UL) ) {
        Warning( "argument for stacksize is invalid", OPTION_SLOT );
    } else {
        AddNumOption( "stack", value );
    }
}

static void ProcStub( void )
/**************************/
{
    if( !MakeToken( SEP_QUOTE, true ) ) {
        Warning( "argument for stub not valid", OPTION_SLOT );
    } else {
        AddStringOption( "stub", CmdFile->token, CmdFile->len );
    }
}

static void ProcDevice( void )
/****************************/
// no processing necessary - just has to be there.
{
}

static void ProcVirtual( void )
/*****************************/
{
    if( ProcessKeyword( DeviceKeyword ) ) {
        ImplyFormat( FMT_OS2 );
        FmtInfo = APP_VIRTDEVICE;
    }
}

// the microsoft command line options.

static void ProcAlignment( const char *arg )
/******************************************/
{
    bool              success;
    unsigned long     value;

    value = 0;
    success = ( arg != NULL && GetNumberStr( &value, arg , strlen( arg ) ) );
    if( !success ) {
        Warning( "invalid alignment specification", OPTION_SLOT );
    } else {
        AddNumOption( "alignment", value );
    }
}

static void ProcBatch( const char *arg )
/**************************************/
{
    /* This option disables prompting, but only when LINK asks for additional
     * information like missing libraries. It does not affect the standard
     * object file, run file, etc. prompts. For ms2wlink, it is effectively
     * always true.
     */
    arg=arg;
    NotNecessary( "batch" );
}

static void ProcCodeview( const char *arg )
/*****************************************/
{
    arg=arg;
    DebugInfo = true;
}

static void ProcCParMaxAlloc( const char *arg )
/*********************************************/
{
    NotSupported( "cparmaxalloc" );
    CheckNum( arg );
}

static void ProcDosseg( const char *arg )
/***************************************/
{
    arg=arg;
    AddOption( "dosseg" );
}

static void ProcDSAllocate( const char *arg )
/*******************************************/
{
    arg=arg;
    NotSupported( "dsallocate" );
}

static void ProcDynamic( const char *arg )
/****************************************/
{
    NotNecessary( "dynamic" );
    CheckNum( arg );
}

static void ProcExePack( const char *arg )
/****************************************/
{
    arg=arg;
    NotSupported( "exepack" );
}

static void ProcFarCallTrans( const char *arg )
/*********************************************/
{
    arg=arg;
    Warning( "far call translation happens automatically for WATCOM .obj files", OPTION_SLOT );
    Warning( "use FCENABLE for far call translation on non-WATCOM .obj files", OPTION_SLOT );
}

static void WriteOptions( switch_entry *entry )
/*********************************************/
{
    char        help_line[80];
    int         i = 0;
    const char  *kw;

    for( ; entry->keyword; ++entry ) {
        help_line[i++] = ' ';
        help_line[i++] = ' ';
        help_line[i++] = '/';
        for( kw = entry->keyword; *kw != '\0'; ++kw ) {
            help_line[i++] = *kw;
        }
        if( i < 31 ) {  
            while( i < 31 ) {   // pad to second column
                help_line[i++] = ' ';
            }
        } else {
            help_line[i] = '\0';
            i = 0;
            CommandOut( help_line );
        }
    }
    if( i ) {
        help_line[i] = '\0';
        CommandOut( help_line );
    }
}

static void ProcHelp( const char *arg )
/*************************************/
{
    arg=arg;
    WriteHelp();
    WriteOptions( OptionsTable );
    Suicide();
}

static void ProcHigh( const char *arg )
/*************************************/
{
    arg=arg;
    NotSupported( "high" );
}

static void ProcIncremental( const char *arg )
/********************************************/
{
    arg=arg;
    NotSupported( "incremental" );
}

static void ProcInformation( const char *arg )
/********************************************/
{
    arg=arg;
    NotNecessary( "information" );
}

static void ProcLineNumbers( const char *arg )
/********************************************/
{
    arg=arg;
    NotSupported( "linenumbers" );
}

static void ProcMap( const char *arg )
/************************************/
{
    arg=arg;
    MapOption = true;
    /* Argument is ignored. */
}

static void ProcNoDefLibSearch( const char *arg )
/***********************************************/
{
    AddOption( "nodefaultlibs" );
    if( arg != NULL ) {
        Warning( "nodefaultlibrarysearch applies to all libraries in WLINK", OPTION_SLOT );
    }
}

static void ProcNoExtDictSearch( const char *arg )
/************************************************/
{
    arg=arg;
    NotNecessary( "noextdictionary" );
}

static void ProcNoFarCallTrans( const char *arg )
/***********************************************/
{
    arg=arg;
    NotNecessary( "nofarcalltranslation" );
}

static void ProcNoGroupAssoc( const char *arg )
/*********************************************/
{
    arg=arg;
    NotSupported( "nogroupassociation" );
}

static void ProcNoIgnoreCase( const char *arg )
/*********************************************/
{
    arg=arg;
    AddOption( "caseexact" );
}

static void ProcNoLogo( const char *arg )
/***************************************/
{
    arg=arg;
    AddOption( "quiet" );
}

static void ProcNoNullsDosseg( const char *arg )
/**********************************************/
{
    arg=arg;
    NotSupported( "nonullsdosseg" );
}

static void ProcNoPackCode( const char *arg )
/*******************************************/
{
    arg=arg;
    AddNumOption( "packcode", 0 );    // option packcode=0 doesn't pack code.
}

static void ProcNoPackFunctions( const char *arg )
/************************************************/
{
    arg=arg;
    NotNecessary( "nopackfunctions" );
}

static void ProcOldOverlay( const char *arg )
/*******************************************/
{
    arg=arg;
    AddOption( "standard" );
}

static void ProcOnError( const char *arg )
/****************************************/
{
    arg=arg;
    Warning( "undefsok behaviour slightly different than onerror", OPTION_SLOT );
    AddOption( "undefsok" );
}

static void ProcOverlayInterrupt( const char *arg )
/*************************************************/
{
    NotNecessary( "overlayinterrupt" );
    CheckNum( arg );
}

static void ProcPackCode( const char *arg )
/*****************************************/
{
    unsigned long   value;

    value = 65535;
    if( arg != NULL && !GetNumberStr( &value, arg , strlen( arg ) ) )
        Warning( "invalid numeric value", OPTION_SLOT );
    AddNumOption( "packcode", value );
}

static void ProcPackData( const char *arg )
/*****************************************/
{
    NotSupported( "packdata" );
    CheckNum( arg );
}

static void ProcPadCode( const char *arg )
/****************************************/
{
    NotSupported( "padcode" );
    CheckNum( arg );
}

static void ProcPadData( const char *arg )
/****************************************/
{
    NotSupported( "paddata" );
    CheckNum( arg );
}

static void ProcPause( const char *arg )
/**************************************/
{
    arg=arg;
    NotSupported( "pause" );
}

static void ProcNoVIO( void )
/***************************/
{
    FmtInfo = NO_EXTRA;
    NotNecessary( "novio" );
}


static  parse_entry     PMTypes[] = {
    "pm",               ProcWindowApi,      1,
    "vio",              ProcWindowCompat,   1,
    "novio",            ProcNoVIO,          1,
    NULL
};

static void ProcPMType( const char *arg )
/***************************************/
{
    switch( FmtType ) {
    case FMT_DEFAULT:
    case FMT_DOS:
    case FMT_COM:
        FmtType = FMT_OS2;
        break;
    }
    if( arg == NULL || !ProcessKeyList( PMTypes, arg, strlen( arg ) ) ) {
        Warning( "invalid argument for pmtype option", OPTION_SLOT );
    }
}

static void ProcSegments( const char *arg )
/*****************************************/
{
    NotNecessary( "segments" );
    CheckNum( arg );
}

static void ProcStack( const char *arg )
/**************************************/
{
    bool                success;
    unsigned long       value;

    value = 0;
    success = ( arg != NULL && GetNumberStr( &value, arg , strlen( arg ) ) );
    if( !success ) {
        Warning( "stack argument not recognized", OPTION_SLOT );
    } else {
        AddNumOption( "stack", value );
    }
}

static void ProcQuickLibrary( const char *arg )
/*********************************************/
{
    arg=arg;
    NotSupported( "quicklibrary" );
}

static void ProcTiny( const char *arg )
/*************************************/
{
    arg=arg;
    FmtType = FMT_COM;
}

static void ProcWarnFixup( const char *arg )
/******************************************/
{
    arg=arg;
    NotSupported( "warnfixup" );
}

extern void ProcessOption( const char *opt )
/******************************************/
{
    switch_entry        *entry;
    const char          *key;
    const char          *ptr;
    const char          *arg;
    size_t              len;
    size_t              plen;
    size_t              opt_len;

    entry = OptionsTable;

    /* Locate an option argument, if it exists. Note: Unexpected arguments
     * are usually ignored by option processing routines.
     */
    arg = strchr( opt, ':' );
    if( arg != NULL ) {
        opt_len = arg - opt;
        ++arg;  /* Point to the next character after ':'. */
        if( *arg == '\0' ) {
            arg = NULL;     /* Empty argument is no argument. */
        }
    } else {
        opt_len = strlen( opt );
    }

    /* Look for a valid command line option. */
    for( ; entry->keyword != NULL; ++entry ) {
        key = entry->keyword;
        ptr = opt;
        len = 0;
        plen = opt_len;
        for( ;; ) {
            if( plen == 0 && len >= entry->minlen ) {
                (*entry->rtn)( arg );
                return;
            }
            if( *key == '\0' || tolower( *ptr ) != *key )
                break;
            ptr++;
            key++;
            len++;
            plen--;
        }
        /* Entry didn't match. */
    }
    /* No match found; probably junk. */
    NotRecognized( opt );
}
