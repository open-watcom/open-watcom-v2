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


#include "param.h"
#include "read.h"


TableOrData tableOrData( ResDirEntry *dir )
/******************************************/
{
    if( dir->dir.entry_rva & PE_RESOURCE_MASK_ON )  return( TABLE );
    else                                            return( DATA );
}

NameOrID nameOrID( ResDirEntry *dir )
/************************************/
{
    if( dir->dir.id_name & PE_RESOURCE_MASK_ON ) return( NAME );
    else                                         return( ID );
}

pe_va getDirNameRva( ResDirEntry *dir )
/**************************************/
{
    return( dir->dir.id_name & PE_RESOURCE_MASK );
}

long int getDirNameAbs( ResDirEntry *dir, ExeFile *exeFile )
/***********************************************************/
{
    return( exeFile->resObj.physical_offset +
            getDirNameRva( dir ) );
}

long int getDirChildAbs( ResDirEntry *dir, ExeFile *exeFile )
/************************************************************/
{
    return( exeFile->resObj.physical_offset +
            ( dir->dir.entry_rva & PE_RESOURCE_MASK ) );
}

long int getResDataRva( ResDataEntry *data, ExeFile *exeFile )
/*************************************************************/
{
    return( data->entry.data_rva - exeFile->resObj.rva );
}

long int getResDataAbs( ResDataEntry *data, ExeFile *exeFile )
/*************************************************************/
{
    return( exeFile->resObj.physical_offset +
            getResDataRva( data, exeFile ) );
}


bool openExeFile( ExeFile *exeFile, Parameters *param )
/******************************************************/
{
    exeFile->file = fopen( param->filename, "rb" );
    if( exeFile->file == NULL ) {
        printf( ERR_READ_CANNOT_OPEN_FILE, param->filename );
        return( false );
    } else {
        return( true );
    }
}

bool readExeHeaders( ExeFile *exeFile )
/**************************************/
{
    long int     prevPos;

    prevPos = ftell( exeFile->file );
    if( fseek( exeFile->file, 0L, SEEK_SET ) ) {
        printf( ERR_READ_CANNOT_FIND_START );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    if( fread( (void *) &exeFile->dosHdr,
               (size_t) sizeof( dos_exe_header ),
               (size_t) 1,
               exeFile->file ) != 1 ) {
        printf( ERR_READ_DOS_EXE_HEADER );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }

    if( fseek( exeFile->file, NH_OFFSET, SEEK_SET ) ||
        fread( (void *) &exeFile->pexHdrAddr,
               (size_t) sizeof( unsigned_32 ),
               (size_t) 1,
               exeFile->file ) != 1 ) {
        printf( ERR_READ_CANNOT_FIND_PE_HEADER );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    if( fseek( exeFile->file, exeFile->pexHdrAddr, SEEK_SET ) ) {
        printf( ERR_READ_CANNOT_FIND_PE_HEADER );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    if( fread( (void *) &exeFile->pexHdr,
               (size_t) sizeof( pe_header ),
               (size_t) 1,
               exeFile->file ) != 1 ) {
        printf( ERR_READ_PE_EXE_HEADER );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    fseek( exeFile->file, prevPos, SEEK_SET );
    if( exeFile->pexHdr.signature != PE_SIGNATURE &&
        exeFile->pexHdr.signature != PL_SIGNATURE ) {
        printf( ERR_READ_NOT_PE_EXE );
        return( false );
    } else {
        return( true );
    }
}

bool findResourceObject( ExeFile *exeFile )
/******************************************/
{
    long int    prevPos;
    int         i;

    prevPos = ftell( exeFile->file );
    if( fseek( exeFile->file,
               exeFile->pexHdrAddr + sizeof( pe_header ),
               SEEK_SET ) ) {
        printf( ERR_READ_CANNOT_FIND_OBJECTS );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    for( i=0; i < exeFile->pexHdr.num_objects; i++ ) {
        exeFile->resObjAddr = ftell( exeFile->file );
        if( fread( (void *) &exeFile->resObj,
                   (size_t) sizeof( pe_object ),
                   (size_t) 1,
                   exeFile->file ) != 1 ) {
            printf( ERR_READ_OBJECT );
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( false );
        }
        if( stricmp( exeFile->resObj.name, RESOURCE_OBJECT_NAME ) == 0 ) {
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( true );
        }
    }
    printf( ERR_READ_CANNOT_FIND_RES_OBJECT );
    fseek( exeFile->file, prevPos, SEEK_SET );
    return( false );
}

bool loadTableEntry( ResTableEntry *table, ExeFile *exeFile, long int addr )
/***************************************************************************/
{
    long int    prevPos;
    int         i;
    int         entriesCount;

    table->header.num_name_entries = 0;
    table->header.num_id_entries = 0;
    table->dirs = NULL;

    prevPos = ftell( exeFile->file );
    if( fseek( exeFile->file,
               (size_t) addr,
               SEEK_SET ) ) {
        printf( ERR_READ_CANNOT_FIND_TABLE_HEADER );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    if( fread( (void *) &table->header,
               (size_t) sizeof( resource_dir_header ),
               (size_t) 1,
               exeFile->file ) != 1 ) {
        printf( ERR_READ_TABLE_HEADER );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }

    entriesCount = table->header.num_name_entries +
                   table->header.num_id_entries;
    table->dirs = (ResDirEntry *) malloc( sizeof( ResDirEntry ) * entriesCount );
    if( table->dirs == NULL ) {
        printf( ERR_READ_OUT_OF_MEMORY );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    for( i = 0; i < entriesCount; i++ ) {
        if( !loadDirEntry( &table->dirs[ i ], exeFile ) ) {
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( false );
        }
    }
    fseek( exeFile->file, prevPos, SEEK_SET );
    return( true );
}


bool loadDirEntry( ResDirEntry *dir, ExeFile *exeFile )
/******************************************************/
{
    long int            prevPos;

    dir->table = NULL;
    dir->data = NULL;
    dir->nameID = ID;
    dir->entryType = TABLE;
    dir->name = NULL;
    dir->nameSize = 0;

    if( fread( (void *) &dir->dir,
               (size_t) sizeof( resource_dir_entry ),
               (size_t) 1,
               exeFile->file ) != 1 ) {
        printf( ERR_READ_DIR_ENTRY );
        return( false );
    }

    if( nameOrID( dir ) == NAME ) {
        dir->nameID = NAME;
        prevPos = ftell( exeFile->file );
        if( fseek( exeFile->file,
                   getDirNameAbs( dir, exeFile ),
                   SEEK_SET ) ) {
            printf( ERR_READ_DIR_ENTRY_NAME );
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( false );
        }
        if( fread( (void *) &dir->nameSize,
                   (size_t) sizeof( unsigned_16 ),
                   (size_t) 1,
                   exeFile->file ) != 1 ) {
            printf( ERR_READ_DIR_ENTRY_NAME );
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( false );
        }
        dir->name = (unsigned_16 *) malloc( dir->nameSize * sizeof( unsigned_16 ) );
        if( dir->name == NULL ) {
            printf( ERR_READ_OUT_OF_MEMORY );
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( false );
        }
        if( fread( (void *) dir->name,
                   (size_t) sizeof( unsigned_16 ),
                   (size_t) dir->nameSize,
                   exeFile->file ) != dir->nameSize ) {
            printf( ERR_READ_DIR_ENTRY_NAME );
            fseek( exeFile->file, prevPos, SEEK_SET );
            return( false );
        }
        fseek( exeFile->file, prevPos, SEEK_SET );
    } else {
        dir->nameID = ID;
    }

    dir->entryType = tableOrData( dir );
    if( dir->entryType == TABLE ) {
        dir->table = (ResTableEntry *) malloc( (size_t) sizeof( ResTableEntry ) );
        if( dir->table == NULL ) {
            printf( ERR_READ_OUT_OF_MEMORY );
            return( false );
        }
        return( loadTableEntry( dir->table, exeFile,
                                getDirChildAbs( dir, exeFile ) ) );
    } else {
        dir->data = (ResDataEntry *) malloc( sizeof( ResDataEntry ) );
        if( dir->data == NULL ) {
            printf( ERR_READ_OUT_OF_MEMORY );
            return( false );
        }
        return( loadDataEntry( dir->data, exeFile,
                               getDirChildAbs( dir, exeFile ) ) );
    }
}

bool loadDataEntry( ResDataEntry *data, ExeFile *exeFile, long int addr )
/************************************************************************/
{
    long int prevPos;

    prevPos = ftell( exeFile->file );
    if( fseek( exeFile->file,
               (size_t) addr,
               SEEK_SET ) ) {
        printf( ERR_READ_CANNOT_FIND_DATA_ENTRY );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    if( fread( (void *) &data->entry,
               (size_t) sizeof( resource_entry ),
               (size_t) 1,
               exeFile->file ) != 1 ) {
        printf( ERR_READ_DATA_ENTRY );
        fseek( exeFile->file, prevPos, SEEK_SET );
        return( false );
    }
    fseek( exeFile->file, prevPos, SEEK_SET );
    return( true );
}

bool loadAllResources( ExeFile *exeFile )
/****************************************/
{
    exeFile->tabEnt = (ResTableEntry *) malloc( sizeof( ResTableEntry ) );
    if( exeFile->tabEnt == NULL ) {
        printf( ERR_READ_OUT_OF_MEMORY );
        return( false );
    } else {
        return( loadTableEntry( exeFile->tabEnt, exeFile, exeFile->resObj.physical_offset ) );
    }
}

void freeTableContents( ResTableEntry *table );

void freeDirContents( ResDirEntry *dir )
/***************************************/
{
    if( dir == NULL ) {
        return;
    }
    if( dir->entryType == TABLE ) {
        freeTableContents( dir->table );
        free( (void *) dir->table );
    } else {
        free( (void *) dir->data );
    }
    if( dir->nameID == NAME ) {
        free( (void *) dir->name );
    }
}

void freeTableContents( ResTableEntry *table )
/*********************************************/
{
    int i;
    if( table == NULL ) {
        return;
    }
    for( i = 0;
         i < table->header.num_name_entries + table->header.num_id_entries;
         i++ ) {
        freeDirContents( &table->dirs[ i ] );
    }
    free( table->dirs );
}

void freeAllResources( ExeFile *exeFile )
/****************************************/
{
    freeTableContents( exeFile->tabEnt );
    free( (void *) exeFile->tabEnt );
    free( (void *) exeFile->file );
}
