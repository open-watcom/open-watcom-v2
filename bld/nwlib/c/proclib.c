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
* Description:  Process librarian commands.
*
****************************************************************************/


#include "wlib.h"
#include "ar.h"

#include "clibext.h"


static void SkipObject( libfile io )
{
    if( Options.libtype == WL_LTYPE_OMF ) {
        OmfSkipObject( io );
    }
}

static void CopyObj( libfile io, libfile out, arch_header *arch )
{
    if( Options.libtype == WL_LTYPE_OMF ) {
        OmfExtract( io, out );
    } else {
        Copy( io, out, arch->size );
    }
}

static void ExtractObj( libfile io, char *name, arch_header *arch, char *newname )
{
    file_offset  pos;
    libfile      out;
    char        *obj_name;

    obj_name = MakeObjOutputName( name, newname );
    remove( obj_name );
    out = LibOpen( obj_name, LIBOPEN_WRITE );
    pos = LibTell( io );
    CopyObj( io, out, arch );
    LibSeek( io, pos, SEEK_SET );
    LibClose( out );
    if( Options.ar && Options.verbose ) {
        Message( "x - %s", obj_name );
    }
}

static void ProcessOneObject( arch_header *arch, libfile io )
{
    lib_cmd  *cmd;
    bool      deleted;

    if( Options.explode ) {
        if( Options.explode_count ) {
            char number[10];
            sprintf( number, "%8.8d", Options.explode_count );
            memcpy( Options.explode_ext, number, 8 );
            Options.explode_count++;
        }
        ExtractObj( io, arch->name, arch, Options.explode_ext );
    }
    deleted = FALSE;
    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( SameName( arch->name, cmd->name ) ) {

            if( !Options.explode ) {
                if( ( cmd->ops & OP_EXTRACT ) && !( cmd->ops & OP_EXTRACTED ) ) {
                    if( cmd->fname != NULL ) {
                        ExtractObj( io, cmd->name, arch, cmd->fname );
                    } else {
                        ExtractObj( io, cmd->name, arch, EXT_OBJ );
                    }
                    cmd->ops |= OP_EXTRACTED;
                }
            }
            if( cmd->ops & OP_DELETE ) {
                deleted = TRUE;
                cmd->ops |= OP_DELETED;
            }
            cmd->ops |= OP_FOUND;
            break;
        }
    }

    if( deleted ) {
        SkipObject( io );
        Options.modified = TRUE;
    } else {
        AddObjectSymbols( arch, io, LibTell( io ) );
    }
}

static void AddOneObject( arch_header *arch, libfile io )
{
    AddObjectSymbols( arch, io, LibTell( io ) );
}

static void DelOneObject( arch_header *arch, libfile io )
{
    RemoveObjectSymbols( arch->name );
    SkipObject( io );
}

typedef enum {
    OBJ_SKIP,
    OBJ_ERROR,
    OBJ_PROCESS,
} objproc;

static void ProcessLibOrObj( char *name, objproc obj, void (*process)( arch_header *arch, libfile io ) )
{
    libfile     io;
    unsigned char   buff[ AR_IDENT_LEN ];
    arch_header arch;

    NewArchHeader( &arch, name );
    io = LibOpen( name, LIBOPEN_READ );
    if( LibRead( io, buff, sizeof( buff ) ) != sizeof( buff ) ) {
        FatalError( ERR_CANT_READ, name, strerror( errno ) );
    }
    if( memcmp( buff, AR_IDENT, sizeof( buff ) ) == 0 ) {
        // AR format
        AddInputLib( io, name );
        LibWalk( io, name, process );
        if( Options.libtype == WL_LTYPE_NONE ) {
            Options.libtype = WL_LTYPE_AR;
        }
    } else if( memcmp( buff, LIBMAG, LIBMAG_LEN ) == 0 ) {
        // MLIB format
        if( LibRead( io, buff, sizeof( buff ) ) != sizeof( buff ) ) {
            FatalError( ERR_CANT_READ, name, strerror( errno ) );
        }
        if( memcmp( buff, LIB_CLASS_DATA_SHOULDBE, LIB_CLASS_LEN + LIB_DATA_LEN ) ) {
            BadLibrary( name );
        }
        AddInputLib( io, name );
        LibWalk( io, name, process );
        if( Options.libtype == WL_LTYPE_NONE ) {
            Options.libtype = WL_LTYPE_MLIB;
        }
    } else if( AddImport( &arch, io ) ) {
        LibClose( io );
    } else if( buff[ 0 ] == LIB_HEADER_REC && buff[ 1 ] != 0x01 ) {
        /*
          The buff[ 1 ] != 1 bit above is a bad hack to get around
          the fact that the coff cpu_type for PPC object files is
          0x1f0.  Really, we should be reading in the first object
          record and doing the checksum and seeing if it is actually
          a LIB_HEADER_REC.  All file format designers who are too
          stupid to recognize the need for a signature should be
          beaten up with large blunt objects.
         */
        // OMF format
        AddInputLib( io, name );
        LibSeek( io, 0, SEEK_SET );
        if( Options.libtype == WL_LTYPE_NONE ) {
            Options.libtype = WL_LTYPE_OMF;
        }
        OMFLibWalk( io, name, process );
    } else if( obj == OBJ_PROCESS ) {
        // Object
        LibSeek( io, 0, SEEK_SET );
        AddObjectSymbols( &arch, io, 0 );
        LibClose( io );
    } else if( obj == OBJ_ERROR ) {
        BadLibrary( name );
    } else {
        LibClose( io );
    }
}

static void WalkInputLib( void )
{
    ProcessLibOrObj( Options.input_name, OBJ_ERROR, ProcessOneObject );
}

static void AddModules( void )
{
    lib_cmd     *cmd;
    char        buff[ MAX_IMPORT_STRING ];

    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( !( cmd->ops & OP_ADD ) )
            continue;
        strcpy( buff, cmd->name );
        if( cmd->ops & OP_IMPORT ) {
            ProcessImport( buff );
        } else {
            DefaultExtension( buff, EXT_OBJ );
            ProcessLibOrObj( buff, OBJ_PROCESS, AddOneObject );
        }
        Options.modified = TRUE;
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
    char        buff[ MAX_IMPORT_STRING ];

    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( !( cmd->ops & OP_DELETE ) )
            continue;
        strcpy( buff, cmd->name );
        DefaultExtension( buff, EXT_OBJ );
        if( IsExt( buff, EXT_LIB ) ) {
            ProcessLibOrObj( buff, OBJ_SKIP, DelOneObject );
            cmd->ops |= OP_DELETED;
        }
        if( !( cmd->ops & OP_DELETED ) && !( cmd->ops & OP_ADD ) ) {
                Warning( ERR_CANT_DELETE, cmd->name );
        } else if( ( cmd->ops & OP_DELETED ) && !( cmd->ops & OP_ADD ) && Options.ar && Options.verbose ) {
            Message( "-d %s", cmd->name );
        }
    }
}

static void EmitWarnings( void )
{
    lib_cmd     *cmd;

    // give a warning if we couldn't find objects to extract
    //  - note: deletes which we didn't find have already been taken
    //    care of by DelModules above (ack :P)
    //    this might make ordering of warnings a little odd...
    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( cmd->ops & OP_EXTRACT ) {
            if( ( cmd->ops & OP_FOUND ) == 0 ) {
                Warning( ERR_CANT_EXTRACT, cmd->name );
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
