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
* Description:  emit object code
*
****************************************************************************/


#include <string.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif
#include "wio.h"
#include "ftnstd.h"
#include "progsw.h"
#include "global.h"
#include "fcgbls.h"
#include "errcod.h"
#include "omodes.h"
#include "cioconst.h"
#include "fcodes.h"
#include "fio.h"
#include "fmemmgr.h"
#include "emitobj.h"
#include "ferror.h"
#include "iopath.h"
#include "pathlist.h"

#if defined( __386__ )
  #define PAGE_SIZE     (16*1024)
#else
  #define PAGE_SIZE     (1*1024)
#endif
#define _PageNumber( v_ptr ) ((v_ptr) / PAGE_SIZE)
#define _PageOffset( v_ptr ) (ObjCode + ( (v_ptr) - ( (v_ptr) / PAGE_SIZE ) * PAGE_SIZE ))
#define _MakeVirtual( page, obj_ptr ) ((page) * PAGE_SIZE + ( (obj_ptr) - ObjCode ))

static  file_attr       PageFileAttrs = { REC_FIXED | SEEK };
static  char            *PageFileName = { "__wfc__.vm" };
static  char            PageFileBuff[_MAX_PATH];
static  int             CurrPage;
static  int             MaxPage;
static  unsigned_8      PageFlags;
static  file_handle     PageFile;
static  char            *ObjPtr;
static  char            *ObjCode;
static  char            *ObjEnd;

extern  file_handle     SDOpen(char *,int);
extern  void            SDClose(file_handle);
extern  void            SDSeek(file_handle,int,int);
extern  uint            SDRead(file_handle,char *,uint);
extern  void            SDWrite(file_handle,char *,int);
extern  bool            SDError(file_handle,char *);
extern  bool            SDEof(file_handle);
extern  void            SDScratch(char *);
extern  void            SDSetAttr(file_attr);

#define PF_INIT         0x00    // initial page flags
#define PF_DIRTY        0x01    // page has been updated

static  void    ChkIOErr( file_handle fp, int error ) {
//=====================================================

// Check for i/o errors to page file.

    char        err_msg[ERR_BUFF_SIZE+1];

    if( SDError( fp, err_msg ) ) {
        Error( error, PageFileName, err_msg );
    }
}

void    InitObj( void ) {
//=================

// Allocate memory for object code.

    char        *fn;
    char        *tmp;
    int         idx;

    ObjCode = NULL; // in case FMemAlloc() fails
    ObjCode = FMemAlloc( PAGE_SIZE );
    ObjEnd = ObjCode + PAGE_SIZE;
    ObjPtr = ObjCode;
    *(unsigned_16 *)ObjPtr = FC_END_OF_SEQUENCE; // in case no source code in file
    PageFile = NULL;
    if( ( ProgSw & PS_DONT_GENERATE ) == 0 ) {
        fn = PageFileBuff;
        tmp = getenv( "TMP" );
        if( tmp != NULL && *tmp != NULLCHAR ) {
            GetPathElement( tmp, &fn );
            if( fn != PageFileBuff ) {
                char c = fn[-1];
                if( !IS_PATH_SEP( c ) ) {
                    *fn++ = DIR_SEP;
                }
            }
        }
        strcpy( fn, PageFileName );
        fn += strlen( fn );
        fn[1] = NULLCHAR;
        for( idx = 0; idx < 26; idx++ ) {
            fn[0] = 'a' + idx;
            if( access( PageFileBuff, 0 ) == -1 ) {
                break;
            }
        }
        if( idx == 26 ) {
            Error( SM_OUT_OF_VM_FILES, PageFileName );
        } else {
            SDSetAttr( PageFileAttrs );
            PageFile = SDOpen( PageFileBuff, UPDATE_FILE );
            ChkIOErr( PageFile, SM_OPENING_FILE );
        }
    }
    PageFlags = PF_INIT;
    CurrPage = 0;
    MaxPage = 0;
}

void    FiniObj( void ) {
//=======================

// Release memory allocated for object code.

    if( ObjCode != NULL ) {
        FMemFree( ObjCode );
        ObjCode = NULL;
    }
    if( PageFile != NULL ) {
        SDClose( PageFile );
        PageFile = NULL;
        SDScratch( PageFileBuff );
    }
}

static  void    DumpCurrPage( void ) {
//====================================

// Dump current page to disk.

    if( PageFlags & PF_DIRTY ) {
        if( CurrPage > MaxPage ) {
            MaxPage = CurrPage;
        }
        SDSeek( PageFile, CurrPage, PAGE_SIZE );
        ChkIOErr( PageFile, SM_IO_WRITE_ERR );
        SDWrite( PageFile, ObjCode, PAGE_SIZE );
        ChkIOErr( PageFile, SM_IO_WRITE_ERR );
        PageFlags &= ~PF_DIRTY;
    }
}

static  void    LoadPage( unsigned_16 page ) {
//============================================

// Load a page into memory.

    if( page != CurrPage ) {
        DumpCurrPage();
        SDSeek( PageFile, page, PAGE_SIZE );
        ChkIOErr( PageFile, SM_IO_READ_ERR );
        SDRead( PageFile, ObjCode, PAGE_SIZE );
        // If we seek to the end of the last page in the disk
        // file (which is the start of a non-existent page file),
        // we will get end-of-file when we do the read.
        if( !SDEof( PageFile ) ) {
            ChkIOErr( PageFile, SM_IO_READ_ERR );
        }
        CurrPage = page;
        PageFlags = PF_INIT;
    }
}

static  void    NewPage( void ) {
//===============================

// Page for F-Codes is full. Dump it to disk and start a new one.

    if( CurrPage < MaxPage ) {
        LoadPage( CurrPage + 1 );
    } else {
        DumpCurrPage();
        PageFlags = PF_INIT;
        CurrPage++;
    }
    ObjPtr = ObjCode;
}

obj_ptr ObjTell( void ) {
//=======================

// Return pointer to next F-Code.

    obj_ptr     curr_obj;

    curr_obj = CurrPage;
    return( _MakeVirtual( curr_obj, ObjPtr ) );
}


obj_ptr ObjSeek( obj_ptr new_obj ) {
//==================================

// Seek to specifed location in F-Code stream.

    obj_ptr     curr_obj;

    curr_obj = CurrPage;
    curr_obj = _MakeVirtual( curr_obj, ObjPtr );
    LoadPage( _PageNumber( new_obj ) );
    ObjPtr = _PageOffset( new_obj );
    return( curr_obj );
}


unsigned_16     ObjOffset( obj_ptr prev_obj ) {
//=============================================

// Return offset from ObjPtr to given pointer.

    obj_ptr     curr_obj;

    curr_obj = CurrPage;
    return( _MakeVirtual( curr_obj, ObjPtr ) - prev_obj );
}


void    AlignEven( void ) {
//=========================

// Align ObjPtr on an even boundary.

    if( (int)(pointer_int)ObjPtr & 0x0001 ) {
        OutByte( 0 );
    }
}


static  void    SplitValue( void *ptr, int size, int part_1 ) {
//=============================================================

// Split value across pages.

    memcpy( ObjPtr, ptr, part_1 );
    PageFlags |= PF_DIRTY;
    NewPage();
    memcpy( ObjPtr, (char *)ptr + part_1, size - part_1 );
    PageFlags |= PF_DIRTY;
    ObjPtr += size - part_1;
}


void    OutPtr( pointer val ) {
//=============================

// Output a pointer to object memory.

    if( ( ProgSw & ( PS_ERROR | PS_DONT_GENERATE ) ) == 0 ) {
        if( ObjEnd - ObjPtr < sizeof( pointer ) ) {
            if( ObjPtr < ObjEnd ) {   // value overlaps pages
                SplitValue( &val, sizeof( pointer ), ObjEnd - ObjPtr );
                return;
            }
            NewPage();
        }
        *(pointer *)ObjPtr = val;
        ObjPtr += sizeof( pointer );
        PageFlags |= PF_DIRTY;
    }
}


void    OutU16( unsigned_16 val ) {
//=================================

// Output 16-bit value to object memory.

    if( ( ProgSw & ( PS_ERROR | PS_DONT_GENERATE ) ) == 0 ) {
        if( ObjEnd - ObjPtr < sizeof( unsigned_16 ) ) {
            if( ObjPtr < ObjEnd ) {   // value overlaps pages
                SplitValue( &val, sizeof( unsigned_16 ), ObjEnd - ObjPtr );
                return;
            }
            NewPage();
        }
        *(unsigned_16 *)ObjPtr = val;
        ObjPtr += sizeof( unsigned_16 );
        PageFlags |= PF_DIRTY;
    }
}


void    OutInt( inttarg val ) {
//=============================

// Output target integer value to object memory.

#if _CPU == 8086
    OutU16( val );
#else // _CPU == 386
    OutConst32( val );
#endif
}


void    OutConst32( signed_32 val ) {
//===================================

// Output 32-bit constant to object memory.

    if( ( ProgSw & ( PS_ERROR | PS_DONT_GENERATE ) ) == 0 ) {
        if( ObjEnd - ObjPtr < sizeof( signed_32 ) ) {
            if( ObjPtr < ObjEnd ) {   // value overlaps pages
                SplitValue( &val, sizeof( signed_32 ), ObjEnd - ObjPtr );
                return;
            }
            NewPage();
        }
        *(signed_32 *)ObjPtr = val;
        ObjPtr += sizeof( signed_32 );
        PageFlags |= PF_DIRTY;
    }
}


void    OutObjPtr( obj_ptr val ) {
//================================

// Output object code pointer to object memory.

    if( ( ProgSw & ( PS_ERROR | PS_DONT_GENERATE ) ) == 0 ) {
        if( ObjEnd - ObjPtr < sizeof( obj_ptr ) ) {
            if( ObjPtr < ObjEnd ) {   // value overlaps pages
                SplitValue( &val, sizeof( obj_ptr ), ObjEnd - ObjPtr );
                return;
            }
            NewPage();
        }
        *(obj_ptr *)ObjPtr = val;
        ObjPtr += sizeof( obj_ptr );
        PageFlags |= PF_DIRTY;
    }
}


void    OutByte( byte val ) {
//===========================

// Output a byte to object memory.

    if( ( ProgSw & ( PS_ERROR | PS_DONT_GENERATE ) ) == 0 ) {
        if( ObjEnd - ObjPtr < sizeof( byte ) ) {
            NewPage();
        }
        *(byte *)ObjPtr = val;
        ObjPtr += sizeof( byte );
        PageFlags |= PF_DIRTY;
    }
}


void    InitFCode( void ) {
//=========================

// Setup for accessing F-Codes from object memory.

    LoadPage( 0 );
    ObjPtr = ObjCode;
}


static  void    JoinValue( void *ptr, int size, int part_1 ) {
//============================================================

// Join value that is split across pages.

    memcpy( ptr, ObjPtr, part_1 );
    LoadPage( CurrPage + 1 );
    ObjPtr = ObjCode;
    memcpy( (char *)ptr + part_1, ObjPtr, size - part_1 );
    ObjPtr += size - part_1;
}


void    *GetPtr( void ) {
//=======================

// Get a pointer from object memory.

    pointer     val;

    if( ObjEnd - ObjPtr < sizeof( pointer ) ) {
        if( ObjPtr < ObjEnd ) {   // value split across pages
            JoinValue( &val, sizeof( pointer ), ObjEnd - ObjPtr );
            return( val );
        }
        LoadPage( CurrPage + 1 );
        ObjPtr = ObjCode;
    }
    val = *(pointer *)ObjPtr;
    ObjPtr += sizeof( pointer );
    return( val );
}


unsigned_16     GetU16( void ) {
//==============================

// Get an unsigned 16-bit value from object memory.

    unsigned_16 val;

    if( ObjEnd - ObjPtr < sizeof( unsigned_16 ) ) {
        if( ObjPtr < ObjEnd ) {   // value split across pages
            JoinValue( &val, sizeof( unsigned_16 ), ObjEnd - ObjPtr );
            return( val );
        }
        LoadPage( CurrPage + 1 );
        ObjPtr = ObjCode;
    }
    val = *(unsigned_16 *)ObjPtr;
    ObjPtr += sizeof( unsigned_16 );
    return( val );
}


signed_32       GetConst32( void ) {
//==================================

// Get 32-bit constant from object memory.

    signed_32   val;

    if( ObjEnd - ObjPtr < sizeof( signed_32 ) ) {
        if( ObjPtr < ObjEnd ) {   // value split across pages
            JoinValue( &val, sizeof( signed_32 ), ObjEnd - ObjPtr );
            return( val );
        }
        LoadPage( CurrPage + 1 );
        ObjPtr = ObjCode;
    }
    val = *(signed_32 *)ObjPtr;
    ObjPtr += sizeof( signed_32 );
    return( val );
}


inttarg GetInt( void ) {
//======================

// Get integer from object memory.

#if _CPU == 8086
    return( GetU16() );
#else // _CPU == 386
    return( GetConst32() );
#endif
}


obj_ptr GetObjPtr( void ) {
//=========================

// Get object code pointer from object memory.

    obj_ptr     val;

    if( ObjEnd - ObjPtr < sizeof( obj_ptr ) ) {
        if( ObjPtr < ObjEnd ) {   // value split across pages
            JoinValue( &val, sizeof( obj_ptr ), ObjEnd - ObjPtr );
            return( val );
        }
        LoadPage( CurrPage + 1 );
        ObjPtr = ObjCode;
    }
    val = *(obj_ptr *)ObjPtr;
    ObjPtr += sizeof( obj_ptr );
    return( val );
}


byte    GetByte( void ) {
//=======================

// Get a byte from object memory.

    byte        val;

    if( ObjEnd - ObjPtr < sizeof( byte ) ) {
        LoadPage( CurrPage + 1 );
        ObjPtr = ObjCode;
    }
    val = *(byte *)ObjPtr;
    ObjPtr += sizeof( byte );
    return( val );
}


obj_ptr FCodeSeek( obj_ptr new_obj ) {
//====================================

// Set ObjPtr to specified location.

    obj_ptr     curr_obj;

    curr_obj = CurrPage;
    curr_obj = _MakeVirtual( curr_obj, ObjPtr );
    LoadPage( _PageNumber( new_obj ) );
    ObjPtr = _PageOffset( new_obj );
    return( curr_obj );
}


obj_ptr FCodeTell( int offset ) {
//===============================

// Return current ObjPtr + offset.

    obj_ptr     new_obj;

    new_obj = CurrPage;
    if( ObjEnd - ObjPtr < offset ) {
        new_obj = _MakeVirtual( new_obj + 1,
                                 ObjCode + offset - ( ObjEnd - ObjPtr ) );
    } else {
        new_obj = _MakeVirtual( new_obj, ObjPtr + offset );
    }
    return( new_obj );
}


void    FCSeek( void ) {
//======================

// Seek to ObjPtr + offset.

    FCodeSeek( FCodeTell( GetU16() ) );
}


FCODE   GetFCode( void ) {
//========================

// Get an F-Code from object memory.

    return( GetU16() );
}
