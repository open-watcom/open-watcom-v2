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


#include <string.h>
#include <assert.h>
#include <wcvector.h>

#include "death.h"
#include "util.h"
#include "elffile.h"
#include "mem.h"
#include "chbffile.h"
#include "fileinfo.h"

static const char * ElfFile::_drSectNames[ DR_DEBUG_NUM_SECTS ] = {
    ".debug_info",
    ".debug_pubnames",
    ".debug_aranges",
    ".debug_line",
    ".debug_loc",
    ".debug_abbrev",
    ".debug_macinfo",
    ".debug_str",
    ".WATCOM_references"
};

static const char * ElfFile::_sectNameName = ".shstrtab";
static const char * ElfFile::_componentSectName = ".comment_browser_components";

static const char * ComponentSignature = "\144\144\001WBRComp";

#define COMP_HDR_SIG_LEN 7

#pragma pack(push, 1);

struct ComponentHeader {
    char    signature[ COMP_HDR_SIG_LEN ];
    short   numItms;
};

#pragma pack(pop);

static ComponentFile * ComponentFile::createComponent( uint_32 t, bool en, const char * name )
//--------------------------------------------------------------------------------------------
{
    ComponentFile * comp;
    uint_16 len;

    len = (uint_16)( strlen( name ) + 1 );
    comp = (ComponentFile *) new char[ sizeof( ComponentFile ) + len ];

    comp->time =    t;
    comp->enabled = en;
    comp->nameLen = len;
    memcpy( comp->name, name, len );

    return comp;
}

static void ComponentFile::freeComponent( ComponentFile * comp )
//--------------------------------------------------------------
{
    delete [] ( (char *) comp );
}

ElfFile::ElfFile()
    : _initialized( false )
    , _file( NULL )
    , _sectNameOff( 0 )
{
}

ElfFile::ElfFile( const char * fName, bool buffered )
    : _initialized( false )
    , _sectNameOff( 0 )
{
    if( buffered ) {
        _file = new CheckedBufferedFile( fName );
    } else {
        _file = new CheckedFile( fName );
    }

    _sections = new WCPtrOrderedVector<SectHdr>;
    _sectNames = new  WCValOrderedVector<const char *>;
    _components = new WCPtrOrderedVector<ComponentFile>;
}

ElfFile::~ElfFile()
//-----------------
{
    for( int i = 0; i < _components->entries(); i += 1 ) {
        ComponentFile::freeComponent( (*_components)[ i ] );
    }
    _components->clear();
    delete _components;

    _sections->clearAndDestroy();
    delete _sections;

    _sectNames->clear();
    delete _sectNames;

    delete _file;
}


bool ElfFile::initSections()
//--------------------------
{
    Elf32_Shdr      sectHdr;
    bool            ok = false;
    char *          buffer;

    _file->open( CheckedFile::ReadBinary, CheckedFile::UserReadWrite );
    _file->read( &_elfHdr, sizeof( Elf32_Ehdr ) );

    if( memcmp( _elfHdr.e_ident, ELF_SIGNATURE, ELF_SIGNATURE_LEN ) ) {
        throw DEATH_BY_BAD_SIGNATURE;
    }
    if( _elfHdr.e_ident[EI_VERSION] != EV_CURRENT
        || _elfHdr.e_version != EV_CURRENT ) {
        throw DEATH_BY_BAD_SIGNATURE;
    }

    if( _elfHdr.e_shentsize != sizeof( Elf32_Shdr ) ) {
        throw DEATH_BY_BAD_SIGNATURE;
    }

    memset( _drSections, 0, DR_DEBUG_NUM_SECTS * sizeof( unsigned long ) );
    memset( _drSizes, 0, DR_DEBUG_NUM_SECTS * sizeof( unsigned long ) );

    _file->seek( _elfHdr.e_shoff + _elfHdr.e_shstrndx * _elfHdr.e_shentsize, SEEK_SET );
    _file->read( &sectHdr, _elfHdr.e_shentsize );

    buffer = new char[ sectHdr.sh_size ];
    _file->seek( sectHdr.sh_offset, SEEK_SET );
    _file->read( buffer, sectHdr.sh_size );

    for( int i = 0; i < _elfHdr.e_shnum; i += 1 ) {
        _file->seek( _elfHdr.e_shoff + i * _elfHdr.e_shentsize, SEEK_SET );
        _file->read( &sectHdr, _elfHdr.e_shentsize );

        addSection( buffer + sectHdr.sh_name, &sectHdr );// this may do seeks and reads
    }

    delete [] buffer;
    _initialized = true;

    return true;
}

void ElfFile::endRead()
//---------------------
{
    if( _initialized ) {
        _file->close();
        _initialized = false;
    }
}


bool ElfFile::addSection( const char * name, void * h )
//-----------------------------------------------------
{
    Elf32_Shdr * hdr = (Elf32_Shdr *) h;

    for( int i = DR_DEBUG_NUM_SECTS; i > 0; i -= 1 ) {
        if( !strcmp( name, _drSectNames[ i - 1 ] ) ) {
            _drSections[ i - 1 ] = hdr->sh_offset;
            _drSizes[ i - 1 ] = hdr->sh_size;

            return true;
        }
    }

    if( !strcmp( name, _componentSectName ) ) {
        readComponentSect( hdr->sh_offset, hdr->sh_size );
        return true;
    }

    return false;
}

void ElfFile::writeStringSect()
//-----------------------------
{
    int             i;
    const char *    str;

    startWriteSect( _sectNameName, SHT_STRTAB );


    for( i = 0; i < _sectNames->entries(); i += 1 ) {
        str = (*_sectNames)[ i ];
        _file->write( str, strlen( str ) + 1 );
    }

    endWriteSect();
}

void ElfFile::readSect( dr_section sect, void * buf, int len )
//------------------------------------------------------------
// NOTE: this assumes that the file is already opened and positioned at the
// right spot!
{
    sect = sect;        // to avoid a warning.
    assert( _drSections[sect] != 0 );
    _file->read( buf, len );
}

void ElfFile::seekSect( dr_section sect, long pos )
//-------------------------------------------------
{
    assert( _drSections[sect] != 0 );
    _file->seek( pos + _drSections[ sect ], SEEK_SET );
}

long ElfFile::getSectOff( dr_section sect )
//-----------------------------------------
{
    assert( _drSections[sect] != 0 );
    return _file->tell() - _drSections[ sect ];
}

void ElfFile::setComponentFiles( CompFileList list )
//--------------------------------------------------
{
    (*_components) = (*list);   // vector copy
}


void ElfFile::addComponentFile( const char * fileName, bool enable )
//------------------------------------------------------------------
{
    ComponentFile * comp;
    bool            found = false;
    FileInfo        finf( fileName );

    // NYI -- use the WCVector find!
    for( int i = 0; i < _components->entries(); i += 1 ) {
        if( strcmp( (*_components)[i]->name, fileName ) == 0 ) {
            comp = (*_components)[ i ];
            found = true;
        }
    }

    if( !found ) {
        comp = ComponentFile::createComponent( finf.st_mtime(), enable, fileName );
    }

    if( !found ) {
        strcpy( comp->name, fileName );
        _components->append( comp );
    }
}

void ElfFile::readComponentSect( long offset, long )
//--------------------------------------------------
{
    ComponentHeader hdr;
    ComponentFile   tmp;
    ComponentFile * newFile;

    _file->seek( offset, SEEK_SET );
    _file->read( &hdr, sizeof( ComponentHeader ) );

    if( memcmp( hdr.signature, ComponentSignature, COMP_HDR_SIG_LEN ) != 0 ) {
        throw DEATH_BY_BAD_SIGNATURE;
    }

    for( int i = 0; i < hdr.numItms; i += 1 ) {
        _file->read( &tmp, sizeof( ComponentFile ) );

        newFile = (ComponentFile *) new char[  sizeof( ComponentFile ) + tmp.nameLen - 1 ];

        *newFile = tmp;             // copy over fields already read
        _file->read( newFile->name + 1, newFile->nameLen - 1 );
        _components->append( newFile );
    }
}

void ElfFile::writeComponentSect()
//--------------------------------
{
    ComponentHeader hdr;
    ComponentFile * curr;

    startWriteSect( _componentSectName, SHT_PROGBITS );

    memcpy( hdr.signature, ComponentSignature, COMP_HDR_SIG_LEN );
    hdr.numItms = (short) _components->entries();
    _file->write( &hdr, sizeof( ComponentHeader ) );

    for( int i = 0; i < _components->entries(); i += 1 ) {
        curr = (*_components)[ i ];
        _file->write( curr, sizeof( ComponentFile ) + curr->nameLen - 1 );
    }

    endWriteSect();
}

void ElfFile::getEnabledComponents( CompFileList list )
//-----------------------------------------------------
{
    for( int i = 0; i < _components->entries(); i += 1 ) {
        if( (*_components)[ i ]->enabled ) {
            list->append( (*_components)[ i ] );
        }
    }
}

void ElfFile::getDisabledComponents( CompFileList list )
//------------------------------------------------------
{
    for( int i = 0; i < _components->entries(); i += 1 ) {
        if( !(*_components)[ i ]->enabled ) {
            list->append( (*_components)[ i ] );
        }
    }
}

void ElfFile::resetComponents()
//-----------------------------
{
    for( int i = 0; i < _components->entries(); i += 1 ) {
        ComponentFile::freeComponent( (*_components)[ i ] );
    }

    _components->clear();
}

const char * ElfFile::getFileName() const
//---------------------------------------
{
    return _file->getFileName();
}

void ElfFile::setFileName( const char * fn )
//------------------------------------------
{
    _file->setFileName( fn );
}

long ElfFile::seek( const char * sect, long offset )
//--------------------------------------------------
// this is meant for use on the write pass only until the read
// pass is updated to not be dwarf specific
{
    int  index;

    index = _sectNames->index( sect );
    if( index < 0 ) {
        return 0;       // <------------ early return for bad sect
    }

    offset += (*_sections)[ index ]->sh_offset;

    return _file->seek( offset, SEEK_SET );
}

long ElfFile::tell( const char * sect )
//-------------------------------------
// give the offset from the start of a given section
{
    int  index;
    long offset;

    index = _sectNames->index( sect );
    if( index < 0 ) {
        return 0;       // <------------ early return for bad sect
    }

    offset = _file->tell();
    offset -= (*_sections)[ index ]->sh_offset;

    return offset;
}


void ElfFile::startWrite()
//------------------------
{
    SectHdr *   invalid;        // invalid section header is the first
    int         i;

    invalid = new SectHdr;

    memset( &_elfHdr, 0, sizeof( Elf32_Ehdr ) );
    memcpy( _elfHdr.e_ident, ELF_SIGNATURE, ELF_SIGNATURE_LEN );
    _elfHdr.e_ident[ EI_CLASS ] =   ELFCLASS32;
    _elfHdr.e_ident[ EI_DATA ] =    ELFDATA2LSB;
    _elfHdr.e_ident[ EI_VERSION ] = EV_CURRENT;

    _elfHdr.e_type = ET_DYN;        // shared object file
    _elfHdr.e_machine = EM_386;
    _elfHdr.e_version = EV_CURRENT;

    _elfHdr.e_shoff = 0;            // don't know section header offset yet.

    _elfHdr.e_ehsize = sizeof( Elf32_Ehdr );
    _elfHdr.e_shentsize = sizeof( Elf32_Shdr );
    _elfHdr.e_shnum = 0;            // don't know number of sections yet
    _elfHdr.e_shstrndx = 0;         // don't know string section index yet

    _file->open( CheckedFile::WriteBinary | O_CREAT, CheckedFile::UserReadWrite );

    _file->write( &_elfHdr, sizeof( Elf32_Ehdr ) );

    memset( invalid, 0, sizeof( SectHdr ) );
    _sections->append( invalid );
    _sectNames->append( "" );
    _sectNameOff += 1;          // strlen( "" );

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        _drSections[ i ] = 0;
        _drSizes[ i ] = 0;
    }
}

void ElfFile::startWriteSect( dr_section sect )
//---------------------------------------------
{
    startWriteSect( _drSectNames[ sect ] );
}

void ElfFile::startWriteSect( const char * name, long sh_type )
//-------------------------------------------------------------
{
    SectHdr *   shdr;
    int         i;

    shdr = new SectHdr;
    memset( shdr, 0, sizeof( SectHdr ) );

    shdr->sh_name = _sectNameOff;
    _sectNameOff += strlen( name ) + 1;

    shdr->sh_type = sh_type;
    shdr->sh_offset = _file->tell();
    shdr->sh_size = 0;                  // don't know length yet

    _sections->append( shdr );
    _sectNames->append( name );

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        if( !strcmp( _drSectNames[ i ], name ) ) {
            _drSections[ i ] = _file->tell();
            break;
        }
    }
}

void ElfFile::endWriteSect()
//--------------------------
{
    SectHdr *   shdr;
    int         i;

    shdr = (*_sections)[ _sections->entries() - 1 ];

    _file->seek( 0, SEEK_END );
    shdr->sh_size = _file->tell() - shdr->sh_offset;

    for( i = 0; i < DR_DEBUG_NUM_SECTS; i += 1 ) {
        if( !strcmp( _drSectNames[ i ], _sectNames->last() ) ) {
            _drSizes[ i ] = _file->st_size() - _drSections[ i ];
            break;
        }
    }
}

void ElfFile::endWrite()
//----------------------
{
    int i;

    writeComponentSect();
    writeStringSect();

    _elfHdr.e_shoff = _file->tell();
    _elfHdr.e_shnum = (uint_16) _sections->entries();
    _elfHdr.e_shstrndx = (uint_16)( _sections->entries() - 1 );

    for( i = 0; i < _sections->entries(); i += 1 ) {
        _file->write( (*_sections)[ i ], sizeof( Elf32_Shdr ) );
    }

    _file->seek( 0, SEEK_SET );
    _file->write( &_elfHdr, sizeof( Elf32_Ehdr ) );
    _file->close();
}
