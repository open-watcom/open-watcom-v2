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


#include <wlib.h>

extern void OMFLibWalk( libfile io, char *name, void *(rtn)( arch_header *arch, libfile io ) );
extern void OMFSkipThisObject(arch_header *, libfile);

static void ExtractObj(libfile io, char *name, file_offset size,
                       arch_header *arch)
{
    file_offset  pos;
    libfile      out;
    char        *obj_name;

    /*
     * If this is an OMF library then we don't have the right object size.
     */
    if (Options.libtype == WL_TYPE_OMF) {
        pos = LibTell(io);
        OMFSkipThisObject(arch, io);
        size = LibTell(io) - pos;
        LibSeek(io, pos, SEEK_SET);
    }

    obj_name = MakeObjOutputName( name );
    unlink(obj_name);
    out = LibOpen( obj_name, LIBOPEN_BINARY_WRITE );
    pos = LibTell( io );
    Copy( io, out, size );
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
        ExtractObj(io, arch->name, arch->size, arch);
    }
    deleted = FALSE;
    for (cmd = CmdList; cmd != NULL; cmd = cmd->next)
    {
        if (SameName( arch->name, cmd->name))
        {

            if (!Options.explode)
            {
                if ((cmd->ops & OP_EXTRACT) && !(cmd->ops & OP_EXTRACTED))
                {
                    ExtractObj(io, cmd->name, arch->size, arch);
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

    if (deleted) {
        if (Options.libtype == WL_TYPE_OMF) {
            OMFSkipThisObject(arch, io);
        }

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
}

typedef enum {
    OBJ_SKIP,
    OBJ_ERROR,
    OBJ_PROCESS,
} objproc;

static void ProcessLibOrObj( char *name, objproc obj, void (*process)( arch_header *arch, libfile io ) )
{
    libfile     io;
    char        buff[AR_IDENT_LEN];
    arch_header arch;

    NewArchHeader( &arch, name );
    io = LibOpen( name, LIBOPEN_BINARY_READ );
    if( LibRead( io, buff, sizeof( buff ) ) != sizeof( buff ) ) {
        FatalError( ERR_CANT_READ, name, strerror( errno ) );
    }
    if( strncmp( buff, AR_IDENT, sizeof( buff ) ) == 0 ) {
        // AR format
        AddInputLib( io, name );
        LibWalk( io, name, process );
        if( Options.libtype == 0 ) {
            Options.libtype = WL_TYPE_AR;
        }
    } else if( strncmp( buff, LIBMAG, LIBMAG_LEN ) == 0 ) {
        // MLIB format
        if( LibRead( io, buff, sizeof( buff ) ) != sizeof( buff ) ) {
            FatalError( ERR_CANT_READ, name, strerror( errno ) );
        }
        if( strncmp( buff, LIB_CLASS_DATA_SHOULDBE, LIB_CLASS_LEN + LIB_DATA_LEN ) ) {
            BadLibrary( name );
        }
        AddInputLib( io, name );
        LibWalk( io, name, process );
        if( Options.libtype == 0 ) {
            Options.libtype = WL_TYPE_MLIB;
        }
    } else if( AddImport( &arch, io ) ) {
        LibClose( io );
    } else if( buff[0] == LIB_HEADER_REC && buff[ 1 ] != 0x01 ) {
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
        if (Options.libtype == 0) {
            Options.libtype = WL_TYPE_OMF;
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

static void WalkInputLib()
{
    ProcessLibOrObj( Options.input_name, OBJ_ERROR, ProcessOneObject );
}

static void AddModules()
{
    lib_cmd     *cmd;
    char        buff[MAX_IMPORT_STRING];

    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( !( cmd->ops & OP_ADD ) ) continue;
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


static void DelModules()
{
    lib_cmd     *cmd;
    char        buff[MAX_IMPORT_STRING];

    for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
        if( !( cmd->ops & OP_DELETE ) ) continue;
        strcpy( buff, cmd->name );
        DefaultExtension( buff, EXT_OBJ );
        if( IsExt( buff, EXT_LIB ) ){
            ProcessLibOrObj( buff, OBJ_SKIP, DelOneObject );
            cmd->ops |= OP_DELETED;
        }
        if( !( cmd->ops & OP_DELETED ) && !(cmd->ops & OP_ADD ) ) {
                Warning( ERR_CANT_DELETE, cmd->name );
        } else if ( ( cmd->ops & OP_DELETED ) && !( cmd->ops & OP_ADD ) && Options.ar && Options.verbose ) {
            Message( "-d %s", cmd->name );
        }
    }
}

void EmitWarnings()
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


void ProcessCommands()
{
    if( !Options.new_library ) {
        WalkInputLib();
    }
    DelModules();
    AddModules();
    CleanFileTab();
    if( Options.modified ) {
        WriteNewLib();
    }
    if( Options.list_contents ) {
        ListContents();
    }
    EmitWarnings();
}
