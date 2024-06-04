/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Process librarian commands.
*
****************************************************************************/


#include "wlib.h"
#include <errno.h>
#include "ar.h"

#include "clibext.h"


static void SkipObject( libfile io, const arch_header *arch )
{
    if( arch->libtype == WL_LTYPE_OMF ) {
        OmfSkipObject( io );
    }
}

static void CopyObj( libfile src, libfile dst, const arch_header *arch )
{
    if( arch->libtype == WL_LTYPE_OMF ) {
        OmfExtract( src, dst );
    } else {
        Copy( src, dst, arch->size );
    }
}

static void ExtractObj( libfile src, const char *name, const arch_header *arch, const char *newname )
{
    long        pos;
    libfile     dst;
    char        *obj_name;

    obj_name = MakeObjOutputName( name, newname );
    remove( obj_name );
    dst = LibOpen( obj_name, LIBOPEN_WRITE );
    pos = LibTell( src );
    CopyObj( src, dst, arch );
    LibSeek( src, pos, SEEK_SET );
    LibClose( dst );
    if( Options.ar && Options.verbose ) {
        Message( "x - %s", obj_name );
    }
}

static void ProcessOneObject( libfile io, const arch_header *arch )
{
    lib_cmd  *cmd;
    bool      deleted;

    if( Options.explode ) {
        if( Options.explode_count ) {
            char number[10];
            sprintf( number, FILE_TEMPLATE_FMT, Options.explode_count );
            memcpy( Options.explode_ext, number, sizeof( FILE_TEMPLATE_MASK ) - 1 );
            Options.explode_count++;
        }
        ExtractObj( io, arch->name, arch, Options.explode_ext );
    }
    deleted = false;
    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( IsSameModuleCase( arch->name, cmd->name, ( arch->libtype == WL_LTYPE_OMF ) ) ) {
            if( !Options.explode ) {
                if( (cmd->ops & OP_EXTRACT) && (cmd->ops & OP_EXTRACTED) == 0 ) {
                    if( cmd->fname != NULL ) {
                        ExtractObj( io, cmd->name, arch, cmd->fname );
                    } else {
                        ExtractObj( io, cmd->name, arch, "." EXT_OBJ );
                    }
                    cmd->ops |= OP_EXTRACTED;
                }
            }
            if( cmd->ops & OP_DELETE ) {
                deleted = true;
                cmd->ops |= OP_DELETED;
            }
            cmd->ops |= OP_FOUND;
            break;
        }
    }

    if( deleted ) {
        SkipObject( io, arch );
        Options.modified = true;
    } else {
        AddObjectSymbols( io, LibTell( io ), arch );
    }
}

static void AddOneObject( libfile io, const arch_header *arch )
{
    AddObjectSymbols( io, LibTell( io ), arch );
}

static void DelOneObject( libfile io, const arch_header *arch )
{
    RemoveObjectSymbols( arch );
    SkipObject( io, arch );
}

typedef enum {
    OBJ_SKIP,
    OBJ_ERROR,
    OBJ_PROCESS,
} objproc;

static void ProcessLibOrObj( const char *filename, objproc obj, libwalk_fn *process )
{
    libfile         io;
    unsigned char   buff[AR_IDENT_LEN];
    arch_header     arch;

    io = NewArchLibOpen( &arch, filename );
    if( LibRead( io, buff, sizeof( buff ) ) != sizeof( buff ) ) {
        FatalError( ERR_CANT_READ, io->name, strerror( errno ) );
    }
    if( memcmp( buff, AR_IDENT, sizeof( buff ) ) == 0 ) {
        /*
         * Library AR format
         */
        arch.libtype = WL_LTYPE_AR;
        AddInputLib( io );
        LibWalk( io, &arch, process );
        if( Options.libtype == WL_LTYPE_NONE ) {
            Options.libtype = WL_LTYPE_AR;
        }
    } else if( memcmp( buff, LIBMAG, LIBMAG_LEN ) == 0 ) {
        /*
         * Library MLIB format
         */
        if( LibRead( io, buff, sizeof( buff ) ) != sizeof( buff ) ) {
            FatalError( ERR_CANT_READ, io->name, strerror( errno ) );
        }
        if( memcmp( buff, LIB_CLASS_DATA_SHOULDBE, LIB_CLASS_LEN + LIB_DATA_LEN ) ) {
            BadLibrary( io );
        }
        arch.libtype = WL_LTYPE_MLIB;
        AddInputLib( io );
        LibWalk( io, &arch, process );
        if( Options.libtype == WL_LTYPE_NONE ) {
            Options.libtype = WL_LTYPE_MLIB;
        }
    } else if( AddImport( io, &arch ) ) {   /* import DLL exported symbols */
        LibClose( io );
    } else if( buff[0] == LIB_HEADER_REC && buff[1] != 0x01 ) {
        /*
         * Library OMF format
         *
         * The buff[1] != 1 bit above is a bad hack to get around
         * the fact that the coff cpu_type for PPC object files is
         * 0x1f0.  Really, we should be reading in the first object
         * record and doing the checksum and seeing if it is actually
         * a LIB_HEADER_REC.  All file format designers who are too
         * stupid to recognize the need for a signature should be
         * beaten up with large blunt objects.
         */
        arch.libtype = WL_LTYPE_OMF;
        AddInputLib( io );
        LibSeek( io, 0, SEEK_SET );
        LibWalk( io, &arch, process );
        if( Options.libtype == WL_LTYPE_NONE ) {
            Options.libtype = WL_LTYPE_OMF;
        }
    } else if( obj == OBJ_PROCESS ) {
        /*
         * Object
         */
        LibSeek( io, 0, SEEK_SET );
        AddObjectSymbols( io, 0, &arch );
        LibClose( io );
    } else if( obj == OBJ_ERROR ) {
        BadLibrary( io );
    } else {
        LibClose( io );
    }
    FreeNewArch( &arch );
}

static void WalkInputLib( void )
{
    ProcessLibOrObj( Options.input_name, OBJ_ERROR, ProcessOneObject );
}

static void AddModules( void )
{
    lib_cmd     *cmd;
    char        buff[MAX_IMPORT_STRING];

    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( (cmd->ops & OP_ADD) == 0 )
            continue;
        strcpy( buff, cmd->name );
        if( cmd->ops & OP_IMPORT ) {
            ProcessImportWlib( buff );
        } else {
            DefaultExtension( buff, EXT_OBJ );
            ProcessLibOrObj( buff, OBJ_PROCESS, AddOneObject );
        }
        Options.modified = true;
        if( Options.ar && Options.verbose ) {
            if( cmd->ops & OP_DELETED ) {
                Message( "r - %s", cmd->name );
            } else {
                Message( "a - %s", cmd->name );
            }
        }
    }
}


static void DelModules( void )
{
    lib_cmd     *cmd;
    char        buff[MAX_IMPORT_STRING];

    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( (cmd->ops & OP_DELETE) == 0 )
            continue;
        strcpy( buff, cmd->name );
        DefaultExtension( buff, EXT_OBJ );
        if( IsExt( buff, EXT_LIB ) ) {
            ProcessLibOrObj( buff, OBJ_SKIP, DelOneObject );
            cmd->ops |= OP_DELETED;
        }
        if( (cmd->ops & OP_ADD) == 0 ) {
            if( cmd->ops & OP_DELETED ) {
                if( Options.ar && Options.verbose ) {
                    Message( "-d %s", cmd->name );
                }
            } else {
                if( !Options.quiet ) {
                    Warning( ERR_CANT_DELETE, cmd->name );
                }
            }
        }
    }
}

static void EmitWarnings( void )
{
    lib_cmd     *cmd;

    /*
     * give a warning if we couldn't find objects to extract
     *  - note: deletes which we didn't find have already been taken
     *    care of by DelModules above (ack :P)
     *    this might make ordering of warnings a little odd...
     */
    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( cmd->ops & OP_EXTRACT ) {
            if( ( cmd->ops & OP_FOUND ) == 0 ) {
                if( !Options.quiet ) {
                    Warning( ERR_CANT_EXTRACT, cmd->name );
                }
            }
        }
    }
}


void ProcessCommands( void )
{
    InitOmfUtil();
    if( !Options.new_library ) {
        WalkInputLib();
    }
    DelModules();
    AddModules();
    CleanFileTab();
    if( Options.modified || Options.new_library ) {
        WriteNewLib();
    }
    if( Options.list_contents ) {
        ListContents();
    }
    EmitWarnings();
    FiniOmfUtil();
}
