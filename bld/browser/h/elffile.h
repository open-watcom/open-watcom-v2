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


//   ElfFile is class designed to read and create elf files.  It handles
//   all the dwarf debugging sections and a separate, user
//   defined section that contains component file information

#ifndef __ELFFILE_H__

// disable "integral value may be truncated" warning
#pragma warning 389 9

#include <time.h>
#include <wstd.h>

#ifndef _WATCOM_H_INCLUDED_     // otherwise <exeelf.h> will try to haul it in
#  define _WATCOM_H_INCLUDED_
#endif

#include <exeelf.h>
#include <dr.h>     // needed for dr_section

template <class Type> class WCValOrderedVector;
template <class Type> class WCPtrOrderedVector;
class CheckedFile;
class CheckedBufferedFile;

#pragma pack(push, 1);

struct ComponentFile {
    time_t          time;           /* time last modified */
    bool            enabled;        /* true if enabled */
    uint_16         nameLen;        /* length of file name, including '\0' */
    char            name[1];        /* file name (entire path) */

    bool    operator== ( const ComponentFile& other ) const {
                return this == &other;
            }
    static ComponentFile *  createComponent( uint_32, bool, const char * name );
    static void             freeComponent( ComponentFile * );
};

#pragma pack(pop);

struct SectHdr : public Elf32_Shdr {
    bool    operator== ( const SectHdr& other ) const {
                return( sh_name == other.sh_name );
            }
};

typedef WCPtrOrderedVector<ComponentFile>* CompFileList;

class ElfFile
{
public:
                ElfFile();
                ElfFile( const char * fName, bool buffered );
                ~ElfFile();

    typedef         void (*writeCb)( void *, ElfFile *, int );

    void            endRead();  // close file
    bool            initSections();
    unsigned long*  getDRSizes() { return _drSizes; }
    void            readSect( dr_section sect, void * buf, int len );
    void            seekSect( dr_section sect, long pos );
    long            getSectOff( dr_section sect );

    void            setComponentFiles( CompFileList list );
    void            addComponentFile( const char * fName, bool enable = true );
    CompFileList    getComponentFiles() { return _components; }
    void            getEnabledComponents( CompFileList list );
    void            getDisabledComponents( CompFileList list );
    void            resetComponents();

    bool            isInitialized() { return _initialized; }
    const char *    getFileName() const;
    void            setFileName( const char * fn );


    long            seek( dr_section sect, long offset ) { return seek( _drSectNames[ sect ], offset ); }
    long            seek( const char * sect, long offset );
    long            tell( dr_section sect ) { return tell( _drSectNames[ sect ] ); }
    long            tell( const char * sect );

    void            startWrite();

    void            startWriteSect( dr_section sect );
    void            startWriteSect( const char *, long sh_type = SHT_PROGBITS );
    void            write( void * buffer, int len );
    void            endWriteSect();

    void            endWrite();

protected:

    unsigned long       _drSections[ DR_DEBUG_NUM_SECTS ];
    unsigned long       _drSizes[ DR_DEBUG_NUM_SECTS ];


    CheckedFile *       _file;

private:
    bool            addSection( const char *, void * );
    void            readComponentSect( long offset, long len );
    void            writeComponentSect();
    void            writeStringSect();
    void            writeSectHdrs( long strSectOff, long strSectLen,
                                   long compSectOff, long compSectLen );

    WCPtrOrderedVector<SectHdr> *       _sections;
    WCValOrderedVector<const char *> *  _sectNames;
    CompFileList                        _components;
    bool                                _hasComponents;
    bool                                _initialized;       // ready for reading
    int                                 _sectNameOff;       // next available slot
                                                            // in sectName section

    Elf32_Ehdr                          _elfHdr;

    static const char * _drSectNames[ DR_DEBUG_NUM_SECTS ];
    static const char * _sectNameName;
    static const char * _componentSectName;
};

#define __ELFFILE_H__
#endif
