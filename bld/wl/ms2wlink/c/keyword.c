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


/*
 *  KEYWORD : Routines for parsing Microsoft keywords.
 *
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <malloc.h>
#include "ms2wlink.h"
#include "command.h"

extern void         AddCommand( char *, int, bool );
extern void         NotNecessary( char * );
extern void         NotSupported( char * );
extern void         AddNumOption( char *, unsigned );
extern void         AddStringOption( char *, char *, int );
extern void         Warning( char *, int );
extern void         AddOption( char * );
extern void         Suicide( void );
extern void *       MemAlloc( unsigned );
extern void         EatWhite( void );
extern bool         MakeToken( sep_type, bool );
extern void         DirectiveError( void );
extern bool         GetNumber( unsigned long * );
extern void         WriteHelp( void );
extern void         ImplyFormat( format_type );
extern void         StartNewFile( char * );
extern void         ParseDefFile( void );
extern char *       ToString( void );

extern bool         MapOption;
extern format_type  FmtType;
extern extra_type   FmtInfo;
extern bool         DebugInfo;

extern cmdfilelist *CmdFile;

static void             (*MultiLine)() = NULL;
static char *           OptionBuffer;
static int              BufferLeft;
static bool             SegWarning = FALSE;
static bool             GotModName = FALSE;
static bool             GotOvl;

typedef struct parse_entry {
    char                *keyword;
    bool                (*rtn)();
    char                minlen;
} parse_entry;

static void ProcAlignment( void );
static void ProcBatch( void );
static void ProcCodeview( void );
static void ProcCParMaxAlloc( void );
static void ProcDosseg( void );
static void ProcDSAllocate( void );
static void ProcDynamic( void );
static void ProcExePack( void );
static void ProcFarCallTranslation( void );
static void ProcHelp( void );
static void ProcHigh( void );
static void ProcIncremental( void );
static void ProcInformation( void );
static void ProcLineNumbers( void );
static void ProcMap( void );
static void ProcNoDefaultLibrarySearch( void );
static void ProcNoExtendedDictSearch( void );
static void ProcNoFarCallTranslation( void );
static void ProcNoGroupAssociation( void );
static void ProcNoIgnoreCase( void );
static void ProcNoLogo( void );
static void ProcNoNullsDosseg( void );
static void ProcNoPackCode( void );
static void ProcNoPackFunctions( void );
static void ProcOldOverlay( void );
static void ProcOnError( void );
static void ProcOverlayInterrupt( void );
static void ProcPackCode( void );
static void ProcPackData( void );
static void ProcPackFunctions( void );
static void ProcPadCode( void );
static void ProcPadData( void );
static void ProcPause( void );
static void ProcPMType( void );
static void ProcQuickLibrary( void );
static void ProcR( void );
static void ProcSegments( void );
static void ProcStack( void );
static void ProcTiny( void );
static void ProcWarnFixup( void );

static  parse_entry     OptionsTable[] = {
    "alignment",        ProcAlignment,      1,
    "batch",            ProcBatch,          1,
    "codeview",         ProcCodeview,       2,
    "cparmaxalloc",     ProcCParMaxAlloc,   2,
    "dynamic",          ProcDynamic,        2,
    "dosseg",           ProcDosseg,         2,
    "dsallocate",       ProcDSAllocate,     2,
    "exepack",          ProcExePack,        1,
    "farcalltranslation", ProcFarCallTranslation, 1,
    "help",             ProcHelp,           2,
    "high",             ProcHigh,           2,
    "incremental",      ProcIncremental,    3,
    "information",      ProcInformation,    1,
    "linenumbers",      ProcLineNumbers,    2,
    "map",              ProcMap,            1,
    "nodefaultlibrarysearch", ProcNoDefaultLibrarySearch, 3,
    "noextendeddictsearch", ProcNoExtendedDictSearch, 3,
    "nofarcalltranslation", ProcNoFarCallTranslation, 3,
    "nogroupassociation", ProcNoGroupAssociation, 3,
    "noignorecase",     ProcNoIgnoreCase,   3,
    "nologo",           ProcNoLogo,         3,
    "nonullsdosseg",    ProcNoNullsDosseg,  3,
    "nopackcode",       ProcNoPackCode,     7,
    "nopackfunctions",  ProcNoPackFunctions,7,
    "oldoverlay",       ProcOldOverlay,     4,
    "onerror",          ProcOnError,        3,
    "overlayinterrupt", ProcOverlayInterrupt,1,
    "packcode",         ProcPackCode,       5,
    "packdata",         ProcPackData,       5,
    "padcode",          ProcPadCode,        4,
    "paddata",          ProcPadData,        4,
    "pause",            ProcPause,          3,
    "pmtype",           ProcPMType,         2,
    "quicklibrary",     ProcQuickLibrary,   1,
    "r",                ProcR,              1,
    "segments",         ProcSegments,       2,
    "stack",            ProcStack,          2,
    "tiny",             ProcTiny,           1,
    "warnfixup",        ProcWarnFixup,      1,
    NULL
};

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
static void SetOS2( void );

static  parse_entry     ExeTypeKeywords[] = {
    "os2",              SetOS2,             3,
    "windows",          SetWindows,         7,
    "dos4",             NullRoutine,        4,
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

static void ProcNoVIO( void );

static  parse_entry     PMTypes[] = {
    "pm",               ProcWindowApi,      2,
    "vio",              ProcWindowCompat,   3,
    "novio",            ProcNoVIO,          5,
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

static bool ProcessKeyList( parse_entry *entry )
/**********************************************/
{
    char                *key;
    char                *ptr;
    int                 len;
    int                 plen;

    while( entry->keyword != NULL ) {
        key = entry->keyword;
        ptr = CmdFile->token;
        len = 0;
        plen = CmdFile->len;
        for(;;) {
            if( plen == 0 && len >= entry->minlen ) {
                MultiLine = NULL;               // for processdefcommand.
                (*entry->rtn)();
                return( TRUE );
            }
            if( *key == '\0' || tolower( *ptr ) != *key ) break;
            ptr++;
            key++;
            len++;
            plen--;
        }
        /* here if this is no match */
        entry++;
    }
    /* here if no match in table */
    return( FALSE );
}

static bool ProcessKeyword( parse_entry *entry )
/**********************************************/
// returns TRUE if keyword found.
{
    bool    ret;

    if( !MakeToken( SEP_NO, FALSE ) ) {
        return( FALSE );
    }
    ret = ProcessKeyList( entry );
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
    if( !MakeToken( SEP_NO, FALSE ) ) return;
    if( !GetNumber( &bogus ) ) {
        CmdFile->current = CmdFile->token;      // reparse the token later
    }
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
    if( !MakeToken( SEP_QUOTE, TRUE ) ) {
        MakeToken( SEP_NO, TRUE );
    }
}

#define CODE_BUFFER_LEN 105

static void ProcCode( void )
/**************************/
{
    char    buffer[ CODE_BUFFER_LEN ];
    char *  result;
    int     len;

    memcpy( buffer, "segment type code", 17 );
    OptionBuffer = buffer + 17;
    BufferLeft = CODE_BUFFER_LEN - 18;
    if( !ProcessKeyword( CodeAttributes ) ) {
        Warning( "argument for code statement not valid", OPTION_SLOT );
    } else {    // get the rest of the attributes.
        while( ProcessKeyword( CodeAttributes ) ) {}     // NULL statement.
        if( BufferLeft != CODE_BUFFER_LEN - 18 ) {      // attribute spec'd.
            len = CODE_BUFFER_LEN - BufferLeft - 1;
            result = MemAlloc( CODE_BUFFER_LEN - BufferLeft + 1);
            memcpy( result, buffer, len );
            *(result + len) = '\0';
            AddCommand( result, OPTION_SLOT, FALSE );
        }
    }
}

static void ProcData( void )
/**************************/
// NYI: serious copied code from ProcCode.  Should be united into one routine.
{
    char    buffer[ CODE_BUFFER_LEN ];
    char *  result;
    int     len;

    memcpy( buffer, "segment type data", 17 );
    OptionBuffer = buffer + 17;
    BufferLeft = CODE_BUFFER_LEN - 18;
    if( !ProcessKeyword( DataAttributes ) ) {
        Warning( "argument for data statement not valid", OPTION_SLOT );
    } else {    // get the rest of the attributes.
        while( ProcessKeyword( DataAttributes ) ) {}     // NULL statement.
        if( BufferLeft != CODE_BUFFER_LEN - 18 ) {      // attribute spec'd.
            len = CODE_BUFFER_LEN - BufferLeft - 1;
            result = MemAlloc( CODE_BUFFER_LEN - BufferLeft + 1);
            memcpy( result, buffer, len );
            *(result + len) = '\0';
            AddCommand( result, OPTION_SLOT, FALSE );
        }
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

static void AddToBuffer( char *cmd, int len )
/*******************************************/
{
    if( BufferLeft - (len+1) < 0 ) {
        Warning( "too many code attributes", OPTION_SLOT );
    } else {
        BufferLeft -= len + 1;
        *OptionBuffer++ = ' ';
        memcpy( OptionBuffer, cmd, len );
        OptionBuffer += len;
    }
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
    NotSupported( "nondiscardable" );
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

    MakeToken( SEP_QUOTE, TRUE );
    msg = alloca( CmdFile->len + (sizeof( PREFIX ) + 1 ) );
    memcpy( msg, PREFIX, sizeof( PREFIX ) );
    memcpy( msg + (sizeof( PREFIX )-1), CmdFile->token, CmdFile->len );
    strcpy( msg + (sizeof( PREFIX )-1) + CmdFile->len, "'" );
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
    char *          internal;
    int             intlen;
    char *          name;
    int             namelen;
    unsigned long   value;
    unsigned long   iopl;
    bool            isresident;
    bool            gottoken;
    bool            gotnodata;
    int             toklen;
    char *          command;
    char *          currloc;

    if( !MakeToken( SEP_NO, TRUE ) ) {
        Warning( "invalid export name", OPTION_SLOT );
        return;
    }
    name = alloca( CmdFile->len );        // store it temporarily
    toklen = namelen = CmdFile->len;
    memcpy( name, CmdFile->token, namelen );
    internal = NULL;
    if( MakeToken( SEP_EQUALS, TRUE ) ) {   // got an internal name.
        internal = alloca( CmdFile->len );        // store it temporarily
        intlen = CmdFile->len;
        memcpy( internal, CmdFile->token, intlen );
        toklen += intlen + 1;        // +1 for = sign.
    }
    value = 0xFFFFF; // arbitrary >64K.
    if( MakeToken( SEP_AT, TRUE ) ) {        // got an ordinal.
        if( !GetNumber( &value ) || value > (64*1024UL) ) {
            Warning( "export ordinal value is invalid", OPTION_SLOT );
            return;
        } else {
            toklen += 6;      // maximum integer length + dot
        }
    }
    isresident = FALSE;
    EatWhite();
    gottoken = MakeToken( SEP_NO, TRUE );
    if( gottoken ) {
        if( CmdFile->len == 12 && memicmp(CmdFile->token, "residentname", 12)==0){
            isresident = TRUE;
            gottoken = MakeToken( SEP_NO, TRUE );
            toklen += 9;           // length of resident + space.
        }
    }
    gotnodata = FALSE;
    if( gottoken ) {
        if( memicmp( CmdFile->token, "nodata", 6 ) == 0 ) {
            gottoken = MakeToken( SEP_NO, TRUE );
            gotnodata = TRUE;
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
            gottoken = MakeToken( SEP_NO, TRUE );
        }
    }
    if( gottoken && !gotnodata ) {
        if( memicmp( CmdFile->token, "nodata", 6 ) == 0 ) {
            gotnodata = TRUE;
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
        utoa( value, currloc, 10 );
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
        utoa( iopl * 2, currloc, 10 ); // convert iopl value to a byte value
    } else {
        *currloc = '\0';
    }
    AddCommand( command, OPTION_SLOT, FALSE );
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
    MakeToken( SEP_SPACE, TRUE );       // skip a function name
}

static void ProcFunctions( void )
/*******************************/
{
    Warning( "Functions style overlay management not directly translatable to watcom", OPTION_SLOT );
    MakeToken( SEP_COLON, FALSE );       // skip number.
    MultiLine = SkipFunction;
}

static void ProcHeapsize( void )
/******************************/
{
    unsigned long value;

    if( !MakeToken( SEP_NO, TRUE ) ) {
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
    char *          first;
    int             firstlen;
    char *          second;
    int             secondlen;
    unsigned long   value;
    char *          result;
    int             toklen;
    char *          currloc;

    if( !MakeToken( SEP_NO, FALSE ) ) {
        Warning( "import library name is invalid", OPTION_SLOT );
        return;
    }
    toklen = firstlen = CmdFile->len;
    first = alloca( firstlen );
    memcpy( first, CmdFile->token, firstlen );
    second = NULL;
    if( MakeToken( SEP_EQUALS, FALSE ) ) {        // got an internal name.
        secondlen = CmdFile->len;
        second = alloca( secondlen );
        memcpy( second, CmdFile->token, secondlen );
        toklen += secondlen + 1;                        // name & = sign.
    }
    if( !MakeToken( SEP_PERIOD, TRUE ) ) {
        Warning( "import entry is invalid", OPTION_SLOT );
        return;
    }
    value = 0xFFFFF;      // arbitrary > 64k.
    if( GetNumber( &value ) ) {
        if( second == NULL ) {
            Warning("must have an internal name when an ordinal is specified",
                                                                  OPTION_SLOT );
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
        utoa( value, currloc, 10 );
    } else {
        if( second != NULL ) {
            *currloc++ = '.';
            memcpy( currloc, CmdFile->token, CmdFile->len );
            currloc += CmdFile->len;
        }
        *currloc = '\0';
    }
    AddCommand( result, OPTION_SLOT, FALSE );
}

static void ProcImports( void )
/*****************************/
{
    MultiLine = GetImport;
}

static void ProcInclude ( void )
/******************************/
{
    if( MakeToken( SEP_QUOTE, TRUE ) || MakeToken( SEP_NO, TRUE ) ) {
        StartNewFile( ToString() );
        ParseDefFile();
    } else {
        Warning( "invalid include statement", OPTION_SLOT );
    }
}

static bool IsInitType( void )
/****************************/
{
    if( CmdFile->len == 10 && memicmp( CmdFile->token, "initglobal", 10 )==0 ) {
        FmtInfo = DLL_INITGLOBAL;
        return( TRUE );
    } else if ( CmdFile->len == 12
                    && memicmp( CmdFile->token, "initinstance", 12 )==0 ) {
        FmtInfo = DLL_INITINSTANCE;
        return( TRUE );
    }
    return( FALSE );
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
    if( !MakeToken( SEP_NO, TRUE ) ) {
        return;
    }
    if( !IsInitType() ) {
        if( CmdFile->len > 8 ) {
            Warning( "module name too large", OPTION_SLOT );
        } else if( GotModName ) {
            Warning( "module name multiply defined" , OPTION_SLOT );
        } else {
            AddStringOption( "modname", CmdFile->token, CmdFile->len );
            GotModName = TRUE;
        }
        if( !MakeToken( SEP_NO, TRUE ) ) {
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

    if( !MakeToken( SEP_NO, TRUE ) ) {
        return;
    }
    if( !ProcessKeyList( ApplicationTypes ) ) {
        if( GotModName ) {
            Warning( "module name multiply defined", OPTION_SLOT );
        } else {
            AddStringOption( "modname", CmdFile->token, CmdFile->len );
            GotModName = TRUE;
        }
    }
    for( index = 0; index < 2; index++ ) {
        if( !MakeToken( SEP_NO, TRUE ) ) {
            return;
        }
        if( !ProcessKeyList( ApplicationTypes ) ) {
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
    if( !MakeToken( SEP_QUOTE, TRUE ) ) {
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
    if( !MakeToken( SEP_QUOTE, TRUE ) ) {
        Warning( "class argument invalid", OPTION_SLOT );
    }
}

static void ProcOvl( void )
/*************************/
{
    MakeToken( SEP_COLON, FALSE );
    Warning( "ovl option on segment directive not supported by wlink", OPTION_SLOT );
    GotOvl = TRUE;
}

static void GetSegments( void )
/*****************************/
{
    char *  segname;
    int     seglen;
    char    buffer[ CODE_BUFFER_LEN ];
    char *  result;
    char *  currloc;
    int     len;

    GotOvl = FALSE;
    if( MakeToken( SEP_QUOTE, TRUE ) || MakeToken( SEP_NO, TRUE ) ) {
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
        while( ProcessKeyword( SegAttributes ) ) {}     // NULL statement.
        if( BufferLeft != CODE_BUFFER_LEN && !GotOvl ) { // something spec'd.
            len = CODE_BUFFER_LEN - BufferLeft;
            result = MemAlloc( len + seglen + 12);
            memcpy( result, "segment '", 9 );
            currloc = result + 9;
            memcpy( currloc, segname, seglen );
            currloc += seglen;
            *currloc++ = '\'';
            *currloc++ = ' ';
            memcpy( currloc, buffer, len );
            *(currloc + len) = '\0';
            AddCommand( result, OPTION_SLOT, FALSE );
        }
    }
    MultiLine = GetSegments;
}

static void ProcDefSegments( void )
/*********************************/
// try to approximate the microsoft segment keyword.
{
    if( !SegWarning ) {
        SegWarning = TRUE;
        Warning( "definition file segments keyword handled differently in WLINK", OPTION_SLOT );
    }
    MultiLine = GetSegments;
}

static void ProcStackSize( void )
/*******************************/
{
    unsigned long value;

    if( !MakeToken( SEP_NO, TRUE ) ) {
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
    if( !MakeToken( SEP_QUOTE, TRUE ) ) {
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

extern bool ProcessOptions( void )
/********************************/
{
    return( ProcessKeyword( OptionsTable ) );
}

static void ProcAlignment( void )
/*******************************/
{
    bool              success;
    unsigned long     value;

    success = MakeToken( SEP_COLON, FALSE );
    if( !success || !GetNumber( &value ) ) {
        Warning( "alignment argument not recognized", OPTION_SLOT );
    } else {
        AddNumOption( "alignment", value );
    }
}

static void ProcBatch( void )
/***************************/
{
    NotNecessary( "batch" );
}

static void ProcCodeview( void )
/******************************/
{
    DebugInfo = TRUE;
}

static void ProcCParMaxAlloc( void )
/**********************************/
{
    NotSupported( "cparmaxalloc" );
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcDosseg( void )
/****************************/
{
    AddOption( "dosseg" );
}

static void ProcDSAllocate( void )
/********************************/
{
    NotSupported( "dsallocate" );
}

static void ProcDynamic( void )
/*****************************/
{
    NotNecessary( "dynamic" );
    MakeToken( SEP_COLON, FALSE );      // skip number
}

static void ProcExePack( void )
/*****************************/
{
    NotSupported( "exepack" );
}

static void ProcFarCallTranslation( void )
/****************************************/
{
    Warning( "far call translation happens automatically for WATCOM .obj files", OPTION_SLOT );
    Warning( "use FCENABLE for far call translation on non-WATCOM .obj files", OPTION_SLOT );
}

static void ProcHelp( void )
/**************************/
{
    WriteHelp();
    Suicide();
}

static void ProcHigh( void )
/**************************/
{
    NotSupported( "high" );
}

static void ProcIncremental( void )
/*********************************/
{
    NotSupported( "incremental" );
}

static void ProcInformation( void )
/*********************************/
{
    NotNecessary( "information" );
}

static void ProcLineNumbers( void )
/*********************************/
{
    NotSupported( "linenumbers" );
}

static void ProcMap( void )
/*************************/
{
    MapOption = TRUE;
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcNoDefaultLibrarySearch( void )
/********************************************/
{
    AddOption( "nodefaultlibs" );
    if( MakeToken( SEP_COLON, TRUE ) ) {
        Warning( "nodefaultlibrarysearch applies to all libraries in WLINK",
                                                                 OPTION_SLOT );
    }
}

static void ProcNoExtendedDictSearch( void )
/******************************************/
{
    NotNecessary( "noextendeddictsearch" );
}

static void ProcNoFarCallTranslation( void )
/******************************************/
{
    NotNecessary( "nofarcalltranslation" );
}

static void ProcNoGroupAssociation( void )
/****************************************/
{
    NotSupported( "nogroupassociation" );
}

static void ProcNoIgnoreCase( void )
/**********************************/
{
    AddOption( "caseexact" );
}

static void ProcNoLogo( void )
/****************************/
{
    AddOption( "quiet" );
}

static void ProcNoNullsDosseg( void )
/***********************************/
{
    NotSupported( "nonullsdosseg" );
}

static void ProcNoPackCode( void )
/********************************/
{
    AddNumOption( "packcode", 0 );    // option packcode=0 doesn't pack code.
}

static void ProcNoPackFunctions( void )
/*************************************/
{
    NotNecessary( "nopackfunctions" );
}

static void ProcOldOverlay( void )
/********************************/
{
    AddOption( "standard" );
}

static void ProcOnError( void )
/*****************************/
{
    Warning( "undefsok behaviour slightly different than onerror", OPTION_SLOT);
    AddOption( "undefsok" );
}

static void ProcOverlayInterrupt( void )
/**************************************/
{
    NotNecessary( "overlayinterrupt" );
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcPackCode( void )
/******************************/
{
    unsigned long   value;

    if( !MakeToken( SEP_COLON, FALSE ) || !GetNumber( &value ) ) {
        value = 65535;
    }
    AddNumOption( "packcode", value );
}

static void ProcPackData( void )
/******************************/
{
    NotSupported( "packdata" );
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcPadCode( void )
/*****************************/
{
    NotSupported( "padcode" );
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcPadData( void )
/*****************************/
{
    NotSupported( "paddata" );
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcPause( void )
/***************************/
{
    NotSupported( "pause" );
}

static void ProcR( void )
/**********************/
{
    NotSupported( "r" );
}

static void ProcNoVIO( void )
/***************************/
{
    FmtInfo = NO_EXTRA;
    NotNecessary( "novio" );
}

static void ProcPMType( void )
/****************************/
{
    switch( FmtType ) {
    case FMT_DEFAULT:
    case FMT_DOS:
    case FMT_COM:
        FmtType = FMT_OS2;
        break;
    }
    if( !MakeToken( SEP_COLON, FALSE ) || !ProcessKeyList( PMTypes ) ) {
        Warning( "invalid argument for pmtype option", OPTION_SLOT );
    }
}

static void ProcSegments( void )
/******************************/
{
    NotNecessary( "segments" );
    MakeToken( SEP_COLON, FALSE );       // skip number.
}

static void ProcStack( void )
/***************************/
{
    bool              success;
    unsigned long     value;

    success = MakeToken( SEP_COLON, FALSE );
    if( !success || !GetNumber( &value ) ) {
        Warning( "stack argument not recognized", OPTION_SLOT );
    } else {
        AddNumOption( "stack", value );
    }
}

static void ProcQuickLibrary( void )
/**********************************/
{
    NotSupported( "quicklibrary" );
}

static void ProcTiny( void )
/**************************/
{
    FmtType = FMT_COM;
}

static void ProcWarnFixup( void )
/*******************************/
{
    NotSupported( "warnfixup" );
}
