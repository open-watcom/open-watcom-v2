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


//
//  system.h    --Class to write the |SYSTEM internal file of a .HLP file.
//                For the Watcom .HLP compiler.
//

#ifndef  SYSTEM_H
#define  SYSTEM_H

#include <watcom.h>
#include "hlpdir.h"
#include "context.h"


//
//  SystemRec   -- A base class for records used in the |SYSTEM file.
//

class SystemRec : public Dumpable
{
protected:
    uint_16 _flag;
    uint_16 _size;
    SystemRec   *_next;

public:
    // Override one of Dumpable's virtual functions.
    uint_32 size() { return (uint_32) _size+4; };

    // Other access function.
    uint_16 flag() { return _flag; };

    friend class    HFSystem;
};


//
//  SystemText  -- A |SYSTEM record containing a string.
//

class SystemText : public SystemRec
{
    char    *_text;

    // Assignment of SystemText's is not allowed.
    SystemText( SystemText const & ) {};
    SystemText &    operator=( SystemText const & ) { return *this; };

public:
    SystemText( uint_16 flg, const char *txt );
    ~SystemText();

    int dump( OutFile * dest );
};


//
//  SystemNum   -- A |SYSTEM record containing a 4-byte number.
//

class SystemNum : public SystemRec
{
    uint_32 _num;
public:
    SystemNum( uint_16 flg, uint_32 val );

    int dump( OutFile * dest );
};


//
//  SystemWin   -- A |SYSTEM record to store secondary
//         window information.
//

#define HLP_SYS_TYPE    10
#define HLP_SYS_NAME    9
#define HLP_SYS_CAP 51
class SystemWin : public SystemRec
{
    uint_16 _winFlags;
    char    _type[HLP_SYS_TYPE];
    char    _name[HLP_SYS_NAME];
    char    _caption[HLP_SYS_CAP];
    uint_16 _position[4];
    uint_16 _maximize;
    uint_32 _rgbMain;
    uint_32 _rgbNonScroll;
public:
    SystemWin( uint_16 wflgs,
               char const type[],
           char const name[],
           char const cap[],
           uint_16 x, uint_16 y,
           uint_16 w, uint_16 h,
           uint_16 use_max,
           uint_32 main_col,
           uint_32 back_col );

    int dump( OutFile * dest );

    friend class    HFSystem;
};


//
//  HFSystem    -- Class to write the |SYSTEM file.
//

class HFSystem : public Dumpable
{
    uint_16     _compLevel; // Compression level.
    uint_32     _size;      // size of the entire |SYSTEM file.
    uint_32     _contentNum;
    HFContext   *_hashFile;


    // All system records are kept in a linked list.

    SystemRec   *_first;
    SystemRec   *_last;

    // Assignment of HFSystem's is not allowed.
    HFSystem( HFSystem const & ) {};
    HFSystem &  operator=( HFSystem const & ) { return *this; };

public:

    // Record types.
    enum {
        SYS_TITLE       = 0x0001,
        SYS_COPYRIGHT   = 0x0002,
        SYS_CONTENTS    = 0x0003,
        SYS_MACRO       = 0x0004,
        SYS_ICON        = 0x0005,
        SYS_WINDOW      = 0x0006,
        SYS_CITATION    = 0x0008
    };

    HFSystem( HFSDirectory *d_file, HFContext *h_file );
    ~HFSystem();

    // Set the compression level.
    void        setCompress( int val );
    bool        isCompressed();
    void        setContents( uint_32 hval );

    // Add a new system record.
    void        addRecord( SystemRec *nextrec );

    // Get an index number for a given window type.
    // If successful, the return value will fit in a byte.
    int         winNumberOf( char * win_name );
    static const int    NoSuchWin;

    // Overrided version of the Dumpable functions.
    uint_32     size() { return _size ; };
    int         dump( OutFile * dest );
};

#endif
