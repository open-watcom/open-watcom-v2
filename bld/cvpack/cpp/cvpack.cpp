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
* Description:  CVPACK mainline.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <new.h>
#include "common.hpp"
#include "cverror.hpp"

#include "typemap.hpp"
#include "cvpack.hpp"
#include "retrieve.hpp"
#include "makeexe.hpp"
#include "subsect.hpp"
#include "packtype.hpp"
#include "cssymbol.hpp"
#include "symdis.hpp"

#include "banner.h"

static const char* CVpackHeader =
    banner1w( "CV4 Symbolic Debugging Information Compactor", BAN_VER_STR ) "\n" \
    banner2( "1995" ) "\n" \
    banner3 "\n" \
    banner3a;

static const char* CVpackUsage = "usage : cvpack [/nologo] <exefile>\n";

static const int MAX_FILE_NAME = 256;
static char      fName[MAX_FILE_NAME];

// map table for type indices.
TypeIndexMap    TypeMap;

// Table of numeric leaf size except variable length string, because its
// length is not constant.
static const NumLeafSize[] = { 1, 2, 2, 4, 4, 4, 8, 10,
                               16, 8, 8, 6, 8, 16, 20, 32 };

//
// This function returns the length of a numeric leaf starting at buffer.
//
uint NumLeafLength( const char* buffer )
/**************************************/
{
    uint length = 0;
    uint index = * (unsigned_16 *) buffer;

    // skip the index of the numeric leaf.
    buffer += WORD;

    // length of the leaf index.
    length += WORD;

    //
    // If it is a variable length string, its length is specified by the
    // next 2 bytes.
    //
    if ( index == LF_VARSTRING ) {
        length += * (unsigned_16 *) buffer;
    } else if ( index >= LF_NUMERIC ) {
        length += NumLeafSize[index-LF_NUMERIC];
    }
    return length;
}

void ConvertFName( char* f )
/**************************/
{
    strcpy(fName,f);
    if ( strchr(f,'.') == NULL ) {
        strcat(fName,".exe");
    }
}
void outOfMemory() {
/******************/
    throw OutOfMemory();
}

void CVpack::DoExeCode()
/**********************/
{
    unsigned_32 length = _aRetriever.TellExeLength();
    if ( length ) {
        char* buffer = new char [length];
        if ( ! _aRetriever.ReadExeCode(buffer) ) {
            throw MiscError("error while reading executable code.");
        }
        _eMaker.DumpToExe(buffer,length);
        delete [] buffer;
    }
    _lfaBase = length;
    _ddeBase = _aRetriever.TellDDEBase();
}

uint CVpack::DoSstModule()
/************************/
{
    char*       buffer;
    unsigned_32 length;
    uint        module;

    for( module = 1;
          _aRetriever.ReadSubsection( buffer, length, sstModule, module );
          module++ ) {
        _newDir.Insert( sstModule, module, OFBase(), length );
        _eMaker.DumpToExe( buffer, length );
    }
    return module-1;
}

void CVpack::DumpSig()
/********************/
{
    _eMaker.DumpToExe( NB09, LONG_WORD );
    //_eMaker.DumpToExe( NB11, LONG_WORD );
}

uint CVpack::DoSrcModule( const uint module )
/*******************************************/
{
    unsigned_32 length;
    char*       buffer;

    if ( _aRetriever.ReadSubsection(buffer,length,sstSrcModule,module) ) {
        _newDir.Insert(sstSrcModule,module,OFBase(),length);
        _eMaker.DumpToExe(buffer,length);
        return ( * (unsigned_16 *) (buffer + WORD) );
    } else {
        return( 0 );
    }
}


uint CVpack::DoSegMap()
/*********************/
{
    uint        cSeg = 0;
    unsigned_32 length;
    char*       buffer;

    if ( _aRetriever.ReadSubsection(buffer,length,sstSegMap) ) {
        cSeg = * (unsigned_16 *) buffer;
        _newDir.Insert(sstSegMap,MODULE_INDEPENDENT,OFBase(),length);
        _eMaker.DumpToExe(buffer,length);
    } else {
        throw MiscError("no sstSegMap present.");
    }
    return( cSeg );
}

void CVpack::DoAlignSym( SstAlignSym*      alignSym,
                         const uint        module )
/**************************************************/
{
    _newDir.Insert(sstAlignSym,module,OFBase(),alignSym -> Length());
    alignSym -> Put(_eMaker);
}


void CVpack::DoGlobalSym( SstGlobalSym& globalSym, const uint cSeg )
/******************************************************************/
{
    unsigned_32 oldOffset = OFBase();
    globalSym.Put(_eMaker,cSeg);
    unsigned_32 length = OFBase() - oldOffset;
    _newDir.Insert(sstGlobalSym,MODULE_INDEPENDENT,oldOffset,length);
}

void CVpack::DoStaticSym( SstStaticSym& staticSym, const uint cSeg )
/******************************************************************/
{
    unsigned_32 oldOffset = OFBase();
    staticSym.Put(_eMaker,cSeg);
    unsigned_32 length = OFBase() - oldOffset;
    _newDir.Insert(sstStaticSym,MODULE_INDEPENDENT,oldOffset,length);
}

void CVpack::DoDirectory()
/************************/
{
    streampos dirOffset = OFBase();
    unsigned_32 cvSize;
    _newDir.Put(_eMaker);

    DumpSig();
    // compute lfoBase and dump it out.
    cvSize = (unsigned_32)(_eMaker.TellPos()+LONG_WORD-_lfaBase);
    _eMaker.DumpToExe(cvSize);

    _eMaker.SeekTo(_lfaBase+LONG_WORD);
    _eMaker.DumpToExe((unsigned_32)dirOffset);

    if (_ddeBase) {
        _eMaker.SeekTo(_ddeBase+0x10);  // offset of debug_size field entry
        _eMaker.DumpToExe(cvSize);      // write new segment size to PE debug dir entry
    }
}

void CVpack::DoPublics( const uint segNum,
                        const uint moduleNum )
/********************************************/
{
    unsigned_32  length;
    uint         index;
    char*        buffer = NULL;
    char*        ptr = NULL;
    char*        end = NULL;
    SstGlobalPub globalPub;
    for ( uint module = 1; module <= moduleNum; module++ ) {
        if ( ! _aRetriever.ReadSubsection(buffer,length,sstPublicSym,module)) {
            continue;
        }
        ptr = buffer;
        end = &ptr[length];
        ptr += LONG_WORD; // skip 0x00000001 header.
        while ( ptr < end ) {
            index = * (unsigned_16 *)(ptr + WORD);
            if ( index == S_PUB16 ) {
                if (!globalPub.Insert(CSPub16::Construct(ptr))) {
                    cerr << "Error: Failed : globalPub.Insert(CSPub16::Construct(ptr))\n";
                    cerr.flush();
                }
            } else if ( index == S_PUB32 ) {
                if (!globalPub.Insert(CSPub32::Construct(ptr))) {
                    cerr << "Error: Failed :globalPub.Insert(CSPub32::Construct(ptr))\n";
                    cerr.flush();
                }
            } else if ( index == S_PUB32_NEW ) {
                if (!globalPub.Insert(CSPub32_new::Construct(ptr))) {
                    cerr << "Error: Failed :globalPub.Insert(CSPub32_new::Construct(ptr))\n";
                    cerr.flush();
                }
            }
            ptr += * (unsigned_16 *) ptr + WORD;
        }
    }
    unsigned_32 oldOffset = OFBase();
    /*
    cerr << "writing publics for segment ";
    cerr << segNum;
    cerr << " now\n";
    cerr.flush();
    */
    globalPub.Put(_eMaker,segNum);
    unsigned_32 secLen = OFBase() - oldOffset;
    if ( secLen != 0 ) {
        _newDir.Insert(sstGlobalPub,MODULE_INDEPENDENT,oldOffset,secLen);
    }
}

void CVpack::DoLibraries()
/************************/
{
    unsigned_32 length;
    char*       buffer;
    if ( _aRetriever.ReadSubsection(buffer,length,sstLibraries) ) {
        _newDir.Insert(sstLibraries,MODULE_INDEPENDENT,OFBase(),length);
        _eMaker.DumpToExe(buffer,length);
    }
}

void CVpack::DoGlobalTypes( SstGlobalTypes& globalType )
/******************************************************/
{
    unsigned_32 oldOffset = OFBase();
    globalType.Put(_eMaker);
    unsigned_32 length = OFBase() - oldOffset;
    _newDir.Insert(sstGlobalTypes,MODULE_INDEPENDENT,oldOffset,length);
}

void CVpack::CreatePackExe()
/**************************/
{
    uint moduleNum = 0;
    // number of segment received code from a module.
    uint moduleSeg = 0;
    DoExeCode();
    DumpSig();
    // reserve for directory offset.
    _eMaker.Reserve(LONG_WORD);
    moduleNum = DoSstModule();

    SstGlobalTypes globalType;
    SstAlignSym*   alignSym;
    SstGlobalSym   globalSym;
    SstStaticSym   staticSym;
    bool           gottype, gotsrcmod;
    unsigned_32    length;
    char           *buffer, *buffer2;

    SymbolDistributor symDis(_aRetriever, globalSym, staticSym);
    for ( uint module = 1; ; module++ ) {
        if( _aRetriever.IsAtSubsection( sstSegMap ) ) break;
        gottype = globalType.LoadTypes(_aRetriever,module);
        gotsrcmod = FALSE;
        if ( _aRetriever.ReadSubsection(buffer,length,sstSrcModule,module) ) {
            moduleSeg = ( * (unsigned_16 *) (buffer + WORD) );
            gotsrcmod = TRUE;
            buffer2 = new char [length];
            memcpy( buffer2, buffer, length );
        }
        if( gottype ) {
            alignSym = new SstAlignSym(moduleSeg);
            if ( ! symDis.Distribute(module, *alignSym) ) {
                delete alignSym;
                break;
            }
            DoAlignSym(alignSym, module);
            delete alignSym;
        }
        if (gotsrcmod) {
            _newDir.Insert(sstSrcModule,module,OFBase(),length);
            _eMaker.DumpToExe(buffer2,length);
            delete buffer2;
            buffer2 = 0;
        }

    }
    uint cSeg = DoSegMap();
    DoPublics(cSeg,moduleNum);
    //cerr << "finished DoPublics.\n"; cerr.flush();

    DoGlobalSym(globalSym,cSeg);
    //cerr << "finished DoGlobalSym.\n"; cerr.flush();

    DoLibraries();
    //cerr << "finished DoLibraries.\n"; cerr.flush();

    DoGlobalTypes(globalType);
    //cerr << "finished DoGlobalTypes.\n"; cerr.flush();

    DoStaticSym(staticSym,cSeg);
    //cerr << "finished DoStaticSym.\n"; cerr.flush();

    DoDirectory();
    //cerr << "finished DoDirectory.\n"; cerr.flush();

    _eMaker.Close();
    //cerr << "finished CreatePackExe()\n"; cerr.flush();
}


int main(int argc, char* argv[])
/******************************/
{
    char tmpFile[L_tmpnam];
    bool quiet;

    quiet = FALSE;
    set_new_handler(outOfMemory);
    try {
        if( argc != 2 ) {
            if( argc == 3 && stricmp( argv[1], "/nologo" ) == 0 ) {
                quiet = TRUE;
                argv[1] = argv[2];
            } else {
                cerr << CVpackUsage;
                return 1;
            }
        }
        if( !quiet ) {
            cout << CVpackHeader << endl;
        }
        ::ConvertFName(argv[1]);
        ifstream  fd(fName, ios::in | ios::binary);
        if ( !fd ) {
            throw FileError(fName);
        }
        tmpnam(tmpFile);
        CVpack packMaker(fd,tmpFile);
        //cerr << "calling packMaker.CreatePackExe()\n";
        //cerr.flush();

        packMaker.CreatePackExe();
        //cout << "cvpack, packMaker.CreatePackExe() OK\n";
        //cout.flush();

        fd.close();
        if ( remove(fName) ) {
            throw MiscError(strerror(errno));
        }
        if ( rename(tmpFile,fName) ) {
            throw MiscError(strerror(errno));
        }
    }
    catch (CVpackError& CVerr) {
        CVerr.ErrorPrint();
        remove(tmpFile);
        exit(1);
    }
    return 0;
}
