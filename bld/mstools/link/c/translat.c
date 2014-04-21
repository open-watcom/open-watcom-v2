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
* Description:  Translate Microsoft LINK to Watcom options.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "cmdline.h"
#include "deffile.h"
#include "error.h"
#include "file.h"
#include "fuzzy.h"
#include "link.h"
#include "message.h"
#include "memory.h"
#include "pathconv.h"
#include "translat.h"

#define UNSUPPORTED_STR_SIZE    512

#if defined(__TARGET_386__)
    #define SYS_NT_CHARMODE     "nt"
    #define SYS_NT_WINDOWED     "nt_win"
    #define SYS_NT_DLL          "nt_dll"
    #define LIBDIR_SUFFIX       "\\lib386"
    #define NT_LIBDIR_SUFFIX    "\\lib386\\nt"
#elif defined(__TARGET_AXP__)
    #define SYS_NT_CHARMODE     "ntaxp"
    #define SYS_NT_WINDOWED     "ntaxp_win"
    #define SYS_NT_DLL          "ntaxp_dll"
    #define LIBDIR_SUFFIX       "\\libaxp"
    #define NT_LIBDIR_SUFFIX    "\\libaxp\\nt"
#elif defined(__TARGET_PPC__)
    #define SYS_NT_CHARMODE     "ntppc"
    #define SYS_NT_WINDOWED     "ntppc_win"
    #define SYS_NT_DLL          "ntppc_dll"
    #define LIBDIR_SUFFIX       "\\libppc"
    #define NT_LIBDIR_SUFFIX    "\\libppc\\nt"
#else
    #error Unrecognized CPU type
#endif


/*
 * Various flags to keep in mind while translating options.
 */
static struct XlatStatus {
    int     dll                 : 1;    /* we're building a DLL */
    int     exp                 : 1;    /* there is an .exp file */
} status;



/*
 * Initialize a struct XlatStatus.
 */
static void init_status( struct XlatStatus *status )
/**************************************************/
{
    memset( status, 0, sizeof(struct XlatStatus) );
}


/*
 * Add one more unsupported option to optStr.
 */
static void append_unsupported( char *optStr, char *opt )
/*******************************************************/
{
    if( optStr[0] != '\0' ) {
        strcat( optStr, " /" );
    } else {
        strcat( optStr, "/" );
    }
    strcat( optStr, opt );
}


/*
 * Parse unsupported options.
 */
static void unsupported_opts( const OPT_STORAGE *cmdOpts )
/********************************************************/
{
    char                opts[UNSUPPORTED_STR_SIZE];

    /*** Build a string listing all unsupported options that were used ***/
    opts[0] = '\0';
    if( cmdOpts->debugtype   )  append_unsupported( opts, "DEBUGTYPE"   );
    if( cmdOpts->exetype     )  append_unsupported( opts, "EXETYPE"     );
    if( cmdOpts->fixed       )  append_unsupported( opts, "FIXED"       );
    if( cmdOpts->machine     )  append_unsupported( opts, "MACHINE"     );
    if( cmdOpts->noentry     )  append_unsupported( opts, "NOENTRY"     );
    if( cmdOpts->order       )  append_unsupported( opts, "ORDER"       );
    if( cmdOpts->pdb         )  append_unsupported( opts, "PDB"         );
    if( cmdOpts->profile     )  append_unsupported( opts, "PROFILE"     );
    if( cmdOpts->section     )  append_unsupported( opts, "SECTION"     );
    if( cmdOpts->verbose     )  append_unsupported( opts, "VERBOSE"     );
    if( cmdOpts->vxd         )  append_unsupported( opts, "VXD"         );
    if( cmdOpts->warn        )  append_unsupported( opts, "WARN"        );

    /*** If an unsupported option was used, give a warning ***/
    if( opts[0] != '\0' ) {
        UnsupportedOptsMessage( opts );
    }
}


/*
 * Add another string to an OPT_STRING.
 */
static void add_string( OPT_STRING **p, char *str )
/*************************************************/
{
    OPT_STRING *        buf;
    OPT_STRING *        curElem;

    /*** Make a new list item ***/
    buf = AllocMem( sizeof(OPT_STRING) + strlen(str) );
    strcpy( buf->data, str );
    buf->next = NULL;

    /*** Put it at the end of the list ***/
    if( *p == NULL ) {
        *p = buf;
    } else {
        curElem = *p;
        while( curElem->next != NULL )  curElem = curElem->next;
        curElem->next = buf;
    }
}


/*
 * Parse a .def file if necessary.
 */
static void def_file_opts( OPT_STORAGE *cmdOpts )
/***********************************************/
{
    DefInfo *           info;
    StringList *        strList;
    char *              newstr;

    if( cmdOpts->def ) {
        #ifdef __TARGET_AXP__
            info = ParseDefFile( cmdOpts->def_value->data,
                                 !cmdOpts->nofuzzy );
        #else
            info = ParseDefFile( cmdOpts->def_value->data );
        #endif
        if( info != NULL ) {
            strList = info->description;
            while( strList != NULL ) {
                add_string( &cmdOpts->comment_value, strList->str );
                strList = strList->next;
            }

            strList = info->exports;
            while( strList != NULL ) {
                add_string( &cmdOpts->export_value, strList->str );
                cmdOpts->export = 1;
                strList = strList->next;
            }

            if( !cmdOpts->dll ) {
                if( info->makeDll ) {
                    cmdOpts->dll = 1;
                }
            }
            if( !cmdOpts->base ) {
                if( info->baseAddr != NULL ) {
                    add_string( &cmdOpts->base_value, info->baseAddr );
                    cmdOpts->base = 1;
                }
            }

            if( !cmdOpts->heap ) {
                if( info->heapsize != NULL ) {
                    add_string( &cmdOpts->heap_value, info->heapsize );
                    cmdOpts->heap = 1;
                }
            }


            if( !cmdOpts->internaldllname ) {
                if( info->internalDllName != NULL ) {
                    add_string( &cmdOpts->internaldllname_value, info->internalDllName );
                    cmdOpts->internaldllname = 1;
                }
            }

            if( !cmdOpts->out ) {
                if( info->name != NULL ) {
                    newstr = PathConvert( info->name, '\'' );
                    add_string( &cmdOpts->out_value, newstr );
                    cmdOpts->out = 1;
                }
            }

            if( !cmdOpts->stack ) {
                if( info->stacksize != NULL ) {
                    add_string( &cmdOpts->stack_value, info->stacksize );
                    cmdOpts->stack = 1;
                }
            }

            if( !cmdOpts->stub ) {
                if( info->stub != NULL ) {
                    add_string( &cmdOpts->stub_value, info->stub );
                    cmdOpts->stub = 1;
                }
            }

            if( !cmdOpts->version ) {
                if( info->version != NULL ) {
                    add_string( &cmdOpts->version_value, info->version );
                    cmdOpts->version = 1;
                }
            }
            FreeDefInfo(info);
        } else {
            FatalError( "Error parsing %s -- aborting",
                        cmdOpts->def_value->data );
        }
    }
}


/*
 * Called by InitFuzzy when an error occurs.
 */
static int fuzzy_init_callback( const char *filename )
/****************************************************/
{
    Warning( "Cannot extract external symbols from '%s' -- fuzzy linking may not work",
             filename );
    return( 1 );
}


/*
 * Given a symbol, fuzzy match it and create an export directive.
 * Returns a pointer to a newly allocated buffer, which the caller is
 * responsible for freeing.  This function was formerly ExportFuzzy().
 */
static char *fuzzy_export( char *export )
/***************************************/
{
    char *              internalname = NULL;
    char *              ordinal = NULL;
    char *              therest = NULL;
    char *              entryname = NULL;
    char *              exportcopy;
    char *              p = NULL;
    size_t              len = 4; /* for the '.', '=', ' ', and required +1 */

    exportcopy = DupStrMem( export );
    p = strchr( exportcopy+1, '\'' );
    if( exportcopy[0] != '\''  ||  p == NULL )  Zoinks(); // the entryname must be in quotes
    p++;
    if( *p != '\0' ) { /* there is something after the entryname */
        entryname = p; // char after the entryname
        p = strchr( p, '=' );
        if( p != NULL ) { /* internalname found */
            *p = '\0';
            internalname = p + 1;
            p = strchr( internalname, ' ' );
            if( p != NULL ) { /* there is something after the internalname */
                *p = '\0';
                therest = p + 1;
            }
        }

        p = strchr( entryname, '.' );
        if( p != NULL ) { /* ordinal found */
            *p = '\0';
            ordinal = p + 1;
            if( internalname == NULL ) {
                p = strchr( ordinal, ' ' );
                if( p != NULL ) { /* there is something after the ordinal */
                    *p = '\0';
                    therest = p + 1;
                }
            }
            len += strlen( ordinal );
        }

        if( internalname == NULL  &&  ordinal == NULL ) {
            p = strchr( entryname, ' ' );
            if( p != NULL ) { /* there is something after the entryname */
                *p = '\0';
                therest = p + 1;
            }
        }

        *entryname = '\0'; /* separate the entry name from the rest of the export directive */
    }
    entryname = exportcopy;
    len += strlen( entryname );

    if( internalname == NULL ) {
        internalname = MatchFuzzy( entryname );
    } else {
        internalname = MatchFuzzy( internalname );
    }

    if( therest != NULL ) {
        strupr( therest );
        len += strlen( therest );
    }

    /*** Create and return the proper export directive ***/
    if( internalname != NULL ) {
        len += strlen( internalname );
        p = AllocMem( len );
        *p = '\0';
        strcat( p, entryname );
        if( ordinal ) {
            strcat( p, "." );
            strcat( p, ordinal );
        }
        strcat( p, "=" );
        strcat( p, internalname );
        if( therest != NULL ) {
            strcat( p, " " );
            strcat( p, therest );
        }
    } else { // no changes to export directive are required
        p = export;
    }

    FreeMem( exportcopy );
    return( p );
}


/*
 * Initialize fuzzy linking.
 */
static void init_fuzzy( OPT_STRING *objs, OPT_STRING *libs,
                        OPT_STRING *defaultlibs )
/*********************************************************/
{
    char *              envvar;
    unsigned            count;
    char *              newstr;
    char *              start;
    char *              end;
    OPT_STRING *        optStr;
    char **             objsvector;
    char **             libsvector;
    char **             libpathsvector;
    size_t              len;

    /*** Get the object file names into an array ***/
    count = 0;
    optStr = objs;
    while( optStr != NULL ) {
        count++;
        optStr = optStr->next;
    }
    objsvector = AllocMem( (count+1) * sizeof(char*) );
    count = 0;
    optStr = objs;
    while( optStr != NULL ) {
        objsvector[count] = optStr->data;
        count++;
        optStr = optStr->next;
    }
    objsvector[count] = NULL;

    /*** Get the library file names into an array ***/
    count = 0;
    optStr = libs;
    while( optStr != NULL ) {
        count++;
        optStr = optStr->next;
    }
    optStr = defaultlibs;
    while( optStr != NULL ) {
        count++;
        optStr = optStr->next;
    }
    libsvector = AllocMem( (count+1) * sizeof(char*) );
    count = 0;
    optStr = libs;
    while( optStr != NULL ) {
        libsvector[count] = optStr->data;
        count++;
        optStr = optStr->next;
    }
    optStr = defaultlibs;
    while( optStr != NULL ) {
        libsvector[count] = optStr->data;
        count++;
        optStr = optStr->next;
    }
    libsvector[count] = NULL;

    /*** Get the library paths from LIB into the 'libpathsvector' array ***/
    count = 0;
    libpathsvector = AllocMem( (count+1) * sizeof(char*) );
    envvar = getenv( "LIB" );
    if( envvar != NULL ) {
        newstr = DupStrMem( envvar );
        start = newstr;
        while( start != NULL  &&  *start != '\0' ) {
            end = strchr( start, ';' );
            if( end != NULL ) {
                while( *end == ';' ) {
                    *end = '\0';
                    end++;
                }
            }
            count++;
            libpathsvector = ReallocMem( libpathsvector, (count+1) * sizeof(char*) );
            libpathsvector[count-1] = DupStrMem( start );
            start = end;
        }
        FreeMem( newstr );
    }

    /*** Get the default library paths into the 'libpathsvector' array ***/
    envvar = getenv( "WATCOM" );
    if( envvar != NULL ) {
        count += 2;
        libpathsvector = ReallocMem( libpathsvector, (count+1) * sizeof(char*) );

        len = strlen( envvar ) + strlen( NT_LIBDIR_SUFFIX ) + 1;
        libpathsvector[count-2] = AllocMem( len );
        sprintf( libpathsvector[count-2], "%s%s", envvar, NT_LIBDIR_SUFFIX );

        len = strlen( envvar ) + strlen( LIBDIR_SUFFIX ) + 1;
        libpathsvector[count-1] = AllocMem( len );
        sprintf( libpathsvector[count-1], "%s%s", envvar, LIBDIR_SUFFIX );
    }
    libpathsvector[count] = NULL;

    /*** Ok, now do something with these arrays ***/
    InitFuzzy( (const char**)objsvector, (const char**)libsvector,
               (const char**)libpathsvector, fuzzy_init_callback );
    FreeMem( objsvector );
    FreeMem( libsvector );
    for( count=0; libpathsvector[count]!=NULL; count++ ) {
        FreeMem( libpathsvector[count] );
    }
    FreeMem( libpathsvector );
}


/*
 * Determine the name of the executable, and place it in the given buffer.
 */
static void get_executable_name( const OPT_STORAGE *cmdOpts, char *firstObj,
                                 char *executable )
/**************************************************************************/
{
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                fname[_MAX_FNAME];

    if( cmdOpts->out ) {
        strcpy( executable, cmdOpts->out_value->data );
    } else {
        _splitpath( firstObj, drive, dir, fname, NULL );
        if( !cmdOpts->dll ) {
            _makepath( executable, drive, dir, fname, ".exe" );
        } else {
            _makepath( executable, drive, dir, fname, ".dll" );
        }
    }
}


/*
 * Destroy an OPT_STRING.
 */
static void del_string( OPT_STRING **p )
/**************************************/
{
    OPT_STRING *        s;

    while( *p != NULL ) {
        s = *p;
        *p = s->next;
        FreeMem( s );
    }
}


/*
 * Parse linker options.
 */
static void linker_opts( struct XlatStatus *status,
                         const OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/*********************************************************************/
{
    OPT_STRING *        optStr;
    OPT_STRING *        objs = NULL;
    OPT_STRING *        libs = NULL;
    char *              p;
    char *              newstr;
    char *              filename;
    int                 fileType;
    int                 numFiles;
    char *              firstObj = NULL;
    char                executable[_MAX_PATH];
    char *              tmp;
    char *              envvar;

    /*** Process all object file names ***/
    for( numFiles=0; ; numFiles++ ) {
        filename = GetNextFile( &fileType, TYPE_OBJ_FILE, TYPE_INVALID_FILE );
        if( filename == NULL )  break;
        if( cmdOpts->export || cmdOpts->entry ) {
            add_string( &objs, filename );
        }
        newstr = PathConvert( filename, '\'' );
        if( firstObj == NULL )  firstObj = newstr;
        if( numFiles == 0 ) {
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "FILE {" );
        }
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, " %s", newstr );
    }
    if( numFiles != 0 ) {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "}" );
    } else {
        Warning( "No object files specified" );
        FatalError( "Nothing to do!" );
    }

    /*** Process all library file names ***/
    for( ;; ) {
        filename = GetNextFile( &fileType, TYPE_LIB_FILE, TYPE_INVALID_FILE );
        if( filename == NULL )  break;
        if( cmdOpts->export || cmdOpts->entry ) {
            add_string( &libs, filename );
        }
        newstr = PathConvert( filename, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "LIBRARY %s", newstr );
    }

    /*** Process all '.exp' file names ***/
    for( ;; ) {
        filename = GetNextFile( &fileType, TYPE_EXP_FILE, TYPE_INVALID_FILE );
        if( filename == NULL )  break;
        status->exp=1;
        newstr = PathConvert( filename, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "@ %s", newstr );
    }

    /*** Process all '.rbj' and '.rs' file names ***/
    for( ;; ) {
        filename = GetNextFile( &fileType, TYPE_RBJ_FILE, TYPE_RS_FILE, TYPE_INVALID_FILE );
        if( filename == NULL )  break;
        newstr = PathConvert( filename, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION RESOURCE %s", newstr );
    }

    /*** Process the LIB environment variable ***/
    envvar = getenv( "LIB" );
    if( envvar != NULL ) {
        newstr = PathConvert( envvar, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "LIBPATH %s", newstr );
    }

    if( cmdOpts->incremental ) {
        if( !stricmp( cmdOpts->incremental_value->data, "yes" ) ) {
            if( cmdOpts->release ) {
                Warning( "Ignoring /INCREMENTAL due to /RELEASE" );
            } else {
                if( !cmdOpts->_10x ) {
                    AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION incremental" );
                }
            }
        }
    }

    if( cmdOpts->align ) {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION alignment=%u",
                          cmdOpts->align_value );
    }

    if( cmdOpts->base ) {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION offset=%s",
                          cmdOpts->base_value->data );
    }

    optStr = cmdOpts->comment_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION description %s",
                          optStr->data );
        optStr = optStr->next;
    }

    if( cmdOpts->debug ) {
        if( cmdOpts->_10x ) {
            AppendCmdLine( cmdLine, LINK_SYSTEM_SECTION, "DEBUG dwarf" );
        } else {
            AppendCmdLine( cmdLine, LINK_SYSTEM_SECTION, "DEBUG dwarf all" );
        }
    }

    optStr = cmdOpts->defaultlib_value;
    while( optStr != NULL ) {
        newstr = PathConvert( optStr->data, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "LIBRARY %s", newstr );
        optStr = optStr->next;
    }

    /*** Initialize fuzzy linking if necessary ***/
    if( cmdOpts->export || cmdOpts->entry ) {
        if( !cmdOpts->nofuzzy ) {
            init_fuzzy( objs, libs, cmdOpts->defaultlib_value );
        }
    }

    /*** Handle the /entry option ***/
    if( cmdOpts->entry ) {
        if( cmdOpts->nofuzzy ) {
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION,
                              "REFERENCE %s OPTION start=%s",
                              cmdOpts->entry_value->data,
                              cmdOpts->entry_value->data );
        } else {
            p = MatchFuzzy( cmdOpts->entry_value->data );
            if( p != NULL ) {
                AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION,
                                  "REFERENCE %s OPTION start=%s", p, p );
                FreeMem( p );
            } else {
                AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION,
                                  "REFERENCE %s OPTION start=%s",
                                  cmdOpts->entry_value->data,
                                  cmdOpts->entry_value->data );
            }
        }
    }

    /*** Handle export directives ***/
    if( cmdOpts->export ) {
        if( cmdOpts->nofuzzy ) {
            optStr = cmdOpts->export_value;
            while( optStr != NULL ) {
                AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "EXPORT %s",
                                  optStr->data );
                optStr = optStr->next;
            }
        } else {
            optStr = cmdOpts->export_value;
            while( optStr != NULL ) {
                p = fuzzy_export( optStr->data );
                if( p != NULL ) {
                    AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION,
                                      "EXPORT %s", p );
                    FreeMem( p );
                }
                optStr = optStr->next;
            }
        }
    }
    if( objs != NULL )  del_string( &objs );
    if( libs != NULL )  del_string( &libs );

    /*** Deinitialize fuzzy linking if necessary ***/
    if( cmdOpts->export || cmdOpts->entry ) {
        if( !cmdOpts->nofuzzy ) {
            FiniFuzzy();
        }
    }

    if( cmdOpts->force_multiple ) {
        AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION redefsok" );
    }
    if( cmdOpts->force_undefined ) {
        AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION undefsok" );
    }

    if( cmdOpts->heap ) {
        p = strchr( cmdOpts->heap_value->data, ',' );
        if( p == NULL ) {               /* /HEAP:reserve */
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION heapsize=%s",
                              cmdOpts->heap_value->data );
        } else {                        /* /HEAP:reserve,commit */
            *p = '\0';
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION heapsize=%s",
                              cmdOpts->heap_value->data );
            p++;
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "COMMIT heap=%s", p );
        }
    }

    if( cmdOpts->implib ) {
        newstr = PathConvert( cmdOpts->implib_value->data, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION implib=%s",
                          newstr );
    }

    optStr = cmdOpts->include_value;
    while( optStr != NULL ) {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "REFERENCE %s",
                          optStr->data );
        optStr = optStr->next;
    }

    if( cmdOpts->internaldllname ) {
        newstr = DupQuoteStrMem( cmdOpts->internaldllname_value->data, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION modname=%s",
                          newstr );
    }

    if( cmdOpts->map ) {
        if( cmdOpts->map_value != NULL ) {
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION map=%s",
                              cmdOpts->map_value->data );
        } else {
            AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION map" );
        }
    }

    if( cmdOpts->nowopts && cmdOpts->nodefaultlib ) {
        AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION nodefaultlibs" );
    }

    if( cmdOpts->opt_level == OPT_opt_level_opt_ref ) {
        AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION eliminate" );
    }

    if( cmdOpts->out ) {
        newstr = PathConvert( cmdOpts->out_value->data, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "NAME %s", newstr );
    }

    if( cmdOpts->release ) {
        AppendCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION checksum" );
    }

    if( cmdOpts->stack ) {
        p = strchr( cmdOpts->stack_value->data, ',' );
        if( p == NULL ) {               /* /STACK:reserve */
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION stack=%s",
                              cmdOpts->stack_value->data );
        } else {                        /* /STACK:reserve,commit */
            *p = '\0';
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION stack=%s",
                              cmdOpts->stack_value->data );
            p++;
            AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "COMMIT stack=%s", p );
        }
    } else {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION stack=1M" );
    }

    if( cmdOpts->stub ) {
        if( (*cmdOpts->stub_value->data == '\'') ) {
            tmp = cmdOpts->stub_value->data + 1; /* skip leading ' */
            tmp[ strlen(tmp)-1 ] = '\0';         /* smite trailing ' */
        } else {
            tmp = cmdOpts->stub_value->data;
        }
        newstr = PathConvert( tmp, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION stub=%s",
                          newstr );
    }

    if( cmdOpts->version ) {
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION version=%s",
                          cmdOpts->version_value->data );
    }

    /*** Process all resource file names ***/
    get_executable_name( cmdOpts, firstObj, executable );
    for( ;; ) {
        filename = GetNextFile( &fileType, TYPE_RES_FILE, TYPE_INVALID_FILE );
        if( filename == NULL )  break;
        newstr = PathConvert( filename, '\'' );
        AppendFmtCmdLine( cmdLine, LINK_OPTS_SECTION, "OPTION resource=%s", newstr );
    }
}


/*
 * Activate default options.
 */
static void default_opts( struct XlatStatus *status,
                          const OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
{
    status = status;
    if (!cmdOpts->nowopts)
    {
        AppendCmdLine(cmdLine, LINK_OPTS_SECTION, "OPTION quiet");
    } /* if */
} /* default_opts() */


/*
 * Activate options which have been parsed but not yet turned on.
 */
static void merge_opts( struct XlatStatus *status, const OPT_STORAGE *cmdOpts,
                        CmdLine *cmdLine )
/****************************************************************************/
{
    OPT_STRING *curr;
    char       *p;
    char       *system = SYS_NT_CHARMODE;

    if( cmdOpts->subsystem ) {
        p = strchr( cmdOpts->subsystem_value->data, ',' );
        if( p != NULL )  *p = '=';
        p = cmdOpts->subsystem_value->data;

        if( !strnicmp( p, "WINDOWS", 7 )  ||  !strnicmp( p, "NATIVE", 6 ) ) {
            AppendCmdLine(cmdLine, LINK_SYSTEM_SECTION, "RUNTIME windows=4.0");
            system = SYS_NT_WINDOWED;
        } else {
            AppendFmtCmdLine( cmdLine, LINK_SYSTEM_SECTION, "RUNTIME %s", p);
        }
    }

    if( cmdOpts->dll ) {
        AppendFmtCmdLine( cmdLine, LINK_SYSTEM_SECTION, "SYSTEM %s initinstance terminstance", SYS_NT_DLL );
        if( !cmdOpts->implib && !status->exp ) {
            AppendFmtCmdLine( cmdLine, LINK_SYSTEM_SECTION, "OPTION implib" );
        }
    } else {
        AppendFmtCmdLine( cmdLine, LINK_SYSTEM_SECTION, "SYSTEM %s", system );
    }

    /*** Add any options meant for the Watcom tools ***/
    if (cmdOpts->passwopts)
    {
        for (curr = cmdOpts->passwopts_value; curr; curr = curr->next)
        {
            AppendCmdLine(cmdLine, LINK_OPTS_SECTION, curr->data);
        }
    }
}


/*
 * Translate scanned MS options to Watcom options.
 */
void OptionsTranslate( OPT_STORAGE *cmdOpts, CmdLine *cmdLine )
/*************************************************************/
{
    /*** Parse the /nologo switch now so we can print the banner ***/
    init_status( &status );
    if( cmdOpts->nologo ) {
        QuietModeMessage();
    } else {
        BannerMessage();
    }

    /*** Parse everything ***/
    unsupported_opts( cmdOpts );
    default_opts( &status, cmdOpts, cmdLine );
    def_file_opts( cmdOpts );
    linker_opts( &status, cmdOpts, cmdLine );
    merge_opts( &status, cmdOpts, cmdLine );
}
