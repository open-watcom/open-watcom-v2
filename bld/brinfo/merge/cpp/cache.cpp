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


#include "wpch.hpp"
#include "cache.hpp"
#include "hashtbl.hpp"

#define HEADER_MAGIC    0x4D524257      // "WBRM"
#define COMPONENT_MAGIC 0x454C4946      // "FILE"

struct CacheHeader {
    uint_32     magic;
    uint_32     fileLen;
    uint_32     dirStart;
    uint_32     numComponents;
};

struct DirEntry {
    char        *name;
    uint_32     start;
};

CacheOutFile::CacheOutFile()
/**********************/
{
    _header = NULL;
    _file = NULL;
    _directory = new LList<DirEntry>;
}


CacheOutFile::~CacheOutFile()
/***********************/
{
    if( _header != NULL ){
        Close();
    }
    delete _directory;
}


WBool CacheOutFile::Open( char const *filename )
/******************************************/
{
    WBool       result;

    result = FALSE;
    _file = fopen( filename, "wb" );
    if( _file != NULL ){
        _header = new CacheHeader;
        fwrite( &_header, sizeof( CacheHeader ), 1, _file );
        _filePos = sizeof( CacheHeader );
        _curEntry = NULL;
        _numComponents = 0;
        result = TRUE;
    }
    return result;
}


void CacheOutFile::Close()
/**********************/
{
    DirEntry    *current;
    uint_32     magic = COMPONENT_MAGIC;
    uint_32     dirLen;
    int         strLength;

    // Write out the directory.
    dirLen = 2*sizeof(uint_32);
    fwrite( &magic, sizeof(uint_32), 1, _file );
    fwrite( &dirLen, sizeof(uint_32), 1, _file );
    current = _directory->First();
    while( current != NULL ){
        strLength = strlen( current->name ) + 1;
        dirLen += strLength + 2*sizeof( uint_32 );
        fwrite( &strLength, sizeof( uint_32 ), 1, _file );
        fwrite( current->name, 1, strLength, _file );
        fwrite( &current->start, sizeof( uint_32 ), 1, _file );
        delete[] current->name;
        delete current;
        current = _directory->Next();
    }

    fseek( _file, _filePos + sizeof( uint_32 ), SEEK_SET );
    fwrite( &dirLen, sizeof( uint_32 ), 1, _file );

    _header->dirStart = _filePos;
    _header->numComponents = _numComponents;
    _header->magic = HEADER_MAGIC;
    _header->fileLen = _filePos + dirLen;
    fseek( _file, 0, SEEK_SET );
    fwrite( _header, sizeof( CacheHeader ), 1, _file );
    fclose( _file );
    _file = NULL;
    delete _header;
    _header = NULL;
    _directory->Clear();
}


void CacheOutFile::StartComponent( char const *name )
/*************************************************/
{
    DirEntry    *newEntry;
    int         nameLen;
    uint_32     fileHeader[2] = { COMPONENT_MAGIC, 0x00000000 };

    nameLen = strlen( name ) + 1;
    newEntry = new DirEntry;
    newEntry->name = new char[nameLen];
    memcpy( newEntry->name, name, nameLen );
    newEntry->start = _filePos;
    _directory->Append( newEntry );
    _curEntry = newEntry;
    fwrite( fileHeader, sizeof( uint_32 ), 2, _file );
    _filePos += 2*sizeof( uint_32 );
}


void CacheOutFile::EndComponent()
/*****************************/
{
    uint_32     curPos;
    uint_32     start;
    uint_32     length;

    if( _curEntry == NULL ){
        return;
    }

    curPos = _filePos;
    start = _curEntry->start;
    length = curPos - start;
    fseek( _file, start + sizeof( uint_32 ), SEEK_SET );
    fwrite( &length, sizeof( uint_32 ), 1, _file );
    fseek( _file, curPos, SEEK_SET );
    _curEntry = NULL;
    _numComponents += 1;

    return;
}


void CacheOutFile::AddData( void *data, int length )
/**************************************************/
{
    fwrite( data, 1, length, _file );
    _filePos += length;
}


void CacheOutFile::AddDword( uint_32 dword, int compress )
/********************************************************/
{
    if( compress ){
        dword <<= 1;
        if( dword <= 0xFFFF ){
            fwrite( &dword, sizeof( uint_16 ), 1, _file );
            _filePos += sizeof( uint_16 );
        } else {
            dword |= 1;
            fwrite( &dword, sizeof( uint_32 ), 1, _file );
            _filePos += sizeof( uint_32 );
        }
    } else {
        fwrite( &dword, sizeof( uint_32 ), 1, _file );
        _filePos += sizeof( uint_32 );
    }
}


void CacheOutFile::AddByte( uint_8 byte )
/***************************************/
{
    fputc( byte, _file );
    _filePos++;
}


CacheInFile::CacheInFile()
/**************************/
{
    _file = NULL;
    _directory = new LList<DirEntry>;
}


CacheInFile::~CacheInFile()
/***************************/
{
    if( _file != NULL ){
        Close();
    }
    delete _directory;
}


WBool CacheInFile::Open( char const *filename )
/********************************************/
{
    CacheHeader         header;
    uint_32             fileHeader[2];
    uint_32             curOffset;
    uint_32             dirLen;
    FILE                *file;
    uint_32             strLength;
    DirEntry            *newEntry;
    int                 i;

    file = fopen( filename, "rb" );
    if( file == NULL ){
        goto error;
    }
    fread( &header, sizeof( CacheHeader ), 1, file );
    if( header.magic != HEADER_MAGIC ){
        goto error;
    }
    fseek( file, 0, SEEK_END );
    _fileLen = ftell( file );
    if( _fileLen != header.fileLen ){
        goto error;
    }
    _numComponents = header.numComponents;

    // Read in the directory file.
    fseek( file, header.dirStart, SEEK_SET );
    fread( fileHeader, sizeof( uint_32 ), 2, file );
    if( fileHeader[0] != COMPONENT_MAGIC ){
        goto error;
    }
    curOffset = 2*sizeof( uint_32 );
    dirLen = fileHeader[1];
    for( i=0; i<_numComponents; i++ ){
        fread( &strLength, sizeof( uint_32 ), 1, file );
        curOffset += strLength + 2*sizeof( uint_32 );
        if( curOffset > dirLen ){
            goto error;
        }
        newEntry = new DirEntry;
        newEntry->name = new char[ strLength ];
        fread( newEntry->name, 1, strLength, file );
        fread( &newEntry->start, sizeof( uint_32 ), 1, file );
        _directory->Append( newEntry );
    }

    _curEntry = NULL;
    _file = file;

    return TRUE;

error:
    fclose( file );
    return FALSE;
}


void CacheInFile::Close()
/************************/
{
    DirEntry    *current;

    current = _directory->First();
    while( current != NULL ){
        delete[] current->name;
        delete current;
        current = _directory->Next();
    }
    _directory->Clear();

    fclose( _file );
    _file = NULL;
}


WBool CacheInFile::OpenComponent( char const *name )
/*************************************************/
{
    DirEntry    *current;
    uint_32     fileHeader[2];
    uint_32     filePos;

    current = _directory->First();
    while( current != NULL ){
        if( strcmp( name, current->name ) == 0 ){
            break;
        }
        current = _directory->Next();
    }
    if( current == NULL ){
        goto error;
    }
    filePos = current->start;
    fseek( _file, filePos, SEEK_SET );
    fread( fileHeader, sizeof( uint_32 ), 2, _file );
    if( fileHeader[0] != COMPONENT_MAGIC ){
        goto error;
    }
    _curEnd = filePos + fileHeader[1];
    _filePos = filePos + 2*sizeof( uint_32 );
    _curEntry = current;

    return TRUE;

error:
    return FALSE;
}


void CacheInFile::CloseComponent()
/*********************************/
{
    _curEntry = NULL;
}


int CacheInFile::ReadData( void *data, int length )
/****************************************************/
{
    if( _curEntry == NULL ){
        return 0;
    }

    if( _filePos + length > _curEnd ){
        length = _curEnd-_filePos;
    }

    if( length > 0 ){
        fread( data, 1, length, _file );
        _filePos += length;
    }
    return length;
}


int CacheInFile::ReadDword( void *dword, int compress )
/*****************************************************/
{
    if( _curEntry == NULL ){
        return 0;
    }

    if( compress ){
        uint_8  bytes[4];
        uint_32 limit;

        limit = _curEnd - _filePos;
        if( limit < sizeof( uint_16 ) ){
            return 0;
        }

        *((uint_32 *) bytes) = 0;
        fread( &bytes[0], sizeof( uint_16 ), 1, _file );
        if( bytes[0] % 2 ){
            if( limit < sizeof( uint_32 ) ){
                return 0;
            }
            fread( &bytes[2], sizeof( uint_16 ), 1, _file );
            _filePos += sizeof( uint_32 );
        } else {
            _filePos += sizeof( uint_16 );
        }

        *((uint_32 *) dword) = *((uint_32 *) bytes) >> 1;
    } else {
        if( _filePos + sizeof( uint_32 ) > _curEnd ){
            return 0;
        }

        fread( dword, sizeof( uint_32 ), 1, _file );
        _filePos += sizeof( uint_32 );
    }

    return sizeof( uint_32 );
}


int CacheInFile::ReadByte( uint_8 &byte )
/***************************************/
{
    if( _curEntry == NULL ){
        return 0;
    }
    if( _filePos >= _curEnd ){
        return 0;
    }

    byte = (uint_8) fgetc( _file );
    _filePos++;

    return 1;
}
