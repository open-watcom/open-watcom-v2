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
* Description:  Microsoft CL clone utility.
*
****************************************************************************/


#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cl.h"
#include "cmdline.h"
#include "context.h"
#include "error.h"
#include "file.h"
#include "macro.h"
#include "message.h"
#include "optparse.h"
#include "parse.h"
#include "pathconv.h"
#include "translat.h"
#include "system.h"


#if defined(__TARGET_386__)
    #define C_COMPILER          "wcc386"
    #define CPP_COMPILER        "wpp386"
    #define LINKER              "link"
#elif defined(__TARGET_AXP__)
    #define C_COMPILER          "wccaxp"
    #define CPP_COMPILER        "wppaxp"
    #define LINKER              "linkaxp"
#elif defined(__TARGET_PPC__)
    #define C_COMPILER          "wccppc"
    #define CPP_COMPILER        "wppppc"
    #define LINKER              "linkppc"
#else
    #error Unrecognized CPU type
#endif

#define COMPILE_SUCCESS         0
#define COMPILE_NOACTION        (-1)
#define COMPILE_ERROR           (-2)

#define LINK_SUCCESS            0
#define LINK_NOACTION           (-1)
#define LINK_ERROR              (-2)


/*
 * Top-level parsing routine.  Returns the number of items parsed.
 */
static int do_parsing( OPT_STORAGE *cmdOpts )
/*******************************************/
{
    int                 itemsParsed = 0;

    /*** Process the WATCOM_CLONE_OPTIONS environment variable ***/
    if( OpenEnvironContext( "WATCOM_CLONE_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the CL_OPTIONS environment variable ***/
    if( OpenEnvironContext( "CL_OPTIONS" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the CL environment variable ***/
    if( OpenEnvironContext( "CL" )  ==  0 ) {
        CmdStringParse( cmdOpts, &itemsParsed );
    }

    /*** Process the command line ***/
    OpenCmdLineContext();
    CmdStringParse( cmdOpts, &itemsParsed );

    return( itemsParsed );
}


/*
 * Compile any C and C++ files.  Returns COMPILE_NOACTION if there was no
 * file to compile, COMPILE_ERROR if the compiler returned a bad status code
 * or if the compiler could not be spawned, or else COMPILE_SUCCESS if
 * everything went smoothly.
 */
static int compile( const OPT_STORAGE *cmdOpts, CmdLine *compCmdLine )
/********************************************************************/
{
    CmdLine *           cloneCmdLine;
    char **             args;
    char *              filename;
    int                 fileType;
    char *              compiler;
    int                 rc;
    int                 alive = 1;
    int                 numCompiled = 0;
    char                drive[_MAX_DRIVE];
    char                dir[_MAX_DIR];
    char                fname[_MAX_FNAME];
    char                ext[_MAX_EXT];
    char                fullPath[_MAX_PATH];
    int                 count;

    /*** Process all the source files, in the order they were given ***/
    while( alive ) {
        filename = GetNextFile( &fileType, TYPE_C_FILE, TYPE_CPP_FILE,
                                TYPE_INVALID_FILE );
        if( filename == NULL )  break;

        /*** Prepare to spawn the compiler ***/
        cloneCmdLine = CloneCmdLine( compCmdLine );
        HandleFileTranslate( filename, cloneCmdLine, NULL );
        switch( fileType ) {
          case TYPE_C_FILE:
            compiler = C_COMPILER;
            AppendCmdLine( cloneCmdLine, CL_C_PROGNAME_SECTION, compiler );
            AppendCmdLine( cloneCmdLine, CL_C_FILENAMES_SECTION, filename );
            if( !cmdOpts->nowopts ) {
                AppendCmdLine( cloneCmdLine, CL_C_OPTS_SECTION, "-aa" );
            }
            args = MergeCmdLine( cloneCmdLine, CL_C_PROGNAME_SECTION,
                                 CL_C_MACROS_SECTION, CL_C_OPTS_SECTION,
                                 CL_C_FILENAMES_SECTION,
                                 INVALID_MERGE_CMDLINE );
            break;
          case TYPE_CPP_FILE:
            compiler = CPP_COMPILER;
            AppendCmdLine( cloneCmdLine, CL_C_PROGNAME_SECTION, compiler );
            AppendCmdLine( cloneCmdLine, CL_C_FILENAMES_SECTION, filename );
            args = MergeCmdLine( cloneCmdLine, CL_C_PROGNAME_SECTION,
                                 CL_C_MACROS_SECTION, CL_C_OPTS_SECTION,
                                 CL_C_CPP_OPTS_SECTION,
                                 CL_C_FILENAMES_SECTION,
                                 INVALID_MERGE_CMDLINE );
            break;
          default:
            Zoinks();
        }

        /*** Spawn the compiler ***/
        _splitpath( filename, drive, dir, fname, ext );
        fprintf( stderr, "%s%s\n", fname, ext ); /* print name of file we're compiling */
        if( cmdOpts->showwopts ) {
            for( count=0; args[count]!=NULL; count++ ) {
                fprintf( stderr, "%s ", args[count] );
            }
            fprintf( stderr, "\n" );
        }
        if( !cmdOpts->noinvoke ) {
            rc = spawnvp( P_WAIT, compiler, (const char **)args );
            if( rc != 0 ) {
                if( rc == -1  ||  rc == 255 ) {
                    FatalError( "Unable to execute '%s'", compiler );
                } else {
                    return( COMPILE_ERROR );
                }
            }
        }

        /*** Add the object file to the linker list, observe -Fo ***/
        if( cmdOpts->Fo ) {
            AddFile( TYPE_OBJ_FILE, PathConvert( cmdOpts->Fo_value->data, '"' ) );
        } else {
            _makepath( fullPath, NULL, NULL, fname, ".obj" );
            AddFile( TYPE_OBJ_FILE, fullPath );
        }

        /*** Prepare for the next iteration ***/
        DestroyCmdLine( cloneCmdLine );
        numCompiled++;
    }

    if( numCompiled > 0 ) {
        return( COMPILE_SUCCESS );
    } else {
        return( COMPILE_NOACTION );
    }
}


/*
 * Link any object and library files.  Returns LINK_NOACTION if there was no
 * file to compile, LINK_ERROR if the linker returned a bad status code or
 * if the compiler could not be spawned, or else LINK_SUCCESS if everything
 * went smoothly.
 */
static int link( const OPT_STORAGE *cmdOpts, CmdLine *linkCmdLine )
/*****************************************************************/
{
    char **             args;
    char *              filename;
    int                 fileType;
    int                 numFiles;
    int                 rc;
    char *              defFile;
    char *              prevDefFile = NULL;

    /*** Process all object and library file names ***/
    for( numFiles=0; ; numFiles++ ) {
        filename = GetNextFile( &fileType, TYPE_OBJ_FILE, TYPE_LIB_FILE, TYPE_RES_FILE, TYPE_INVALID_FILE );
        if( filename == NULL )  break;
        AppendCmdLine( linkCmdLine, CL_L_FILENAMES_SECTION, filename );
    }

    /*** Process .def files ***/
    for( ;; ) {
        defFile = GetNextFile( NULL, TYPE_DEF_FILE, TYPE_INVALID_FILE );
        if( defFile == NULL )  break;
        if( prevDefFile != NULL ) {
            Warning( "Overriding %s with %s", prevDefFile, defFile );
        }
        prevDefFile = defFile;
    };
    if( prevDefFile != NULL ) {
        AppendFmtCmdLine( linkCmdLine, CL_L_OPTS_SECTION, "/DEF:%s", prevDefFile );
    } else {
        if( numFiles == 0 )  return( LINK_NOACTION );
    }

    /*** Spawn the linker ***/
    AppendCmdLine( linkCmdLine, CL_L_PROGNAME_SECTION, LINKER );
    args = MergeCmdLine( linkCmdLine, INVALID_MERGE_CMDLINE );
    rc = spawnvp( P_WAIT, LINKER, (const char **)args );
    if( rc != 0 ) {
        if( rc == -1  ||  rc == 255 ) {
            FatalError( "Unable to execute '%s'", LINKER );
        } else {
            return( LINK_ERROR );
        }
    }
    return( LINK_SUCCESS );
}


/*
 * Program entry point.
 */
void main( int argc, char *argv[] )
/*********************************/
{
    OPT_STORAGE         cmdOpts;
    CmdLine *           compCmdLine;
    CmdLine *           linkCmdLine;
    int                 itemsParsed;
    int                 compRc = COMPILE_NOACTION;
    int                 linkRc = LINK_NOACTION;

    /*** Initialize ***/
    SetBannerFuncError( BannerMessage );
    compCmdLine = InitCmdLine( CL_C_NUM_SECTIONS );
    linkCmdLine = InitCmdLine( CL_L_NUM_SECTIONS );
    SetDefaultFile( TYPE_C_FILE, "source" );
    AllowTypeFile( TYPE_C_FILE, TYPE_CPP_FILE, TYPE_DEF_FILE, TYPE_OBJ_FILE,
                   TYPE_LIB_FILE, TYPE_RES_FILE, TYPE_INVALID_FILE );
    InitMacro();

    /*** Parse the command line and translate to Watcom options ***/
    InitParse( &cmdOpts );
    itemsParsed = do_parsing( &cmdOpts );
    if( itemsParsed==0 || cmdOpts.help ) {
        if( !cmdOpts.nologo )
            BannerMessage();
        PrintHelpMessage();
        exit( EXIT_SUCCESS );
    }
    OptionsTranslate( &cmdOpts, compCmdLine, linkCmdLine );

    /*** Spawn the compiler ***/
    compRc = compile( &cmdOpts, compCmdLine );
    if( compRc == COMPILE_ERROR )  exit( EXIT_FAILURE );
    if( !cmdOpts.c ) {
        linkRc = link( &cmdOpts, linkCmdLine );
    }
    if( compRc == COMPILE_NOACTION  &&  linkRc == LINK_NOACTION ) {
        FatalError( "Nothing to do!" );
    }
    FiniParse( &cmdOpts );
    exit( EXIT_SUCCESS );
}
