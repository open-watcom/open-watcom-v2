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


/*

  B_write - interface for writing Optima++ pre-merge .BRI files

*/

#ifndef B_WRITE_H
#define B_WRITE_H

#include "brmtypes.h"

/*

  Write Library Callback Definitions

*/

#define BRI_SEEK_SET    0        /* Seek relative to the start of file   */
#define BRI_SEEK_CUR    1        /* Seek relative to current position    */
#define BRI_SEEK_END    2        /* Seek relative to the end of the file */

typedef struct BRI_Routines {
    // I/O Routines
    int  (* write)( int cookie, void const *, unsigned len );
    long (* lseek)( int cookie, long offset, int whence );

    // Memory Routines
    void * (* malloc)( size_t );
    void   (* free)( void * );
} BRI_Routines;


typedef struct BRI_PCHRtns {
    uint_8      (* read1)( void * cookie );
    uint_32     (* read4)( void * cookie );
    void *      (* read)( void * cookie, unsigned size );
} BRI_PCHRtns;


/*

   Write Library functions

*/

typedef struct BRI_Handle       BRI_Handle, *BRI_HANDLE;

#define BRI_NO_CHANGE           ((uint_32) -1)


BRI_HANDLE      BRIBeginWrite(          // Begin writing browse info
    BRI_Routines const  *rtns,          //  -- callback routines
    int                 io_cookie,      //  -- cookie for io routines
    unsigned long       start );        //  -- start position for browse info

void            BRIEndWrite(            // Finish writing browse info
    BRI_HANDLE          handle );       // and delete the handle

BRI_HANDLE      BRICreate(              // Create a browse handle, but
    BRI_Routines const  *rtns );        // don't start writing browse info.

BRI_HANDLE      BRIOpen(                // Re-open a previously closed handle
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_Routines const  *rtns,          //  -- callback routines
    int                 io_cookie,      //  -- io_cookie
    unsigned long       start );        //  -- start position for browse info

BRI_HANDLE      BRIClose(               // Finish writing browse info,
    BRI_HANDLE          handle );       // but don't delete the handle

BRI_HANDLE      BRIBuildHandle(
    BRI_HANDLE          handle,
    BRI_PCHRtns const   *rtns,
    void                *io_cookie );

void            BRIStartFile(           // Signal start of new source file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_StringID        filename );     //  -- name of new source file

void            BRIEndFile(             // Signal end of current source file
    BRI_HANDLE          handle );       //  -- browse handle


void            BRIStartTemplate(       // Signal start of template instance
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_StringID        filename );     //  -- file where template is defined

void            BRIEndTemplate(         // Signal end of template instance
    BRI_HANDLE          handle );


BRI_StringID    BRIAddString(           // Add a string to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_StringID        index,          //  -- index of new string
    const char          *string );      //  -- string to add


BRI_SymbolID    BRIAddSymbol(           // Add a symbol to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_SymbolID        sym_id,         //  -- index of new symbol
    BRI_StringID        name_id,        //  -- name of the symbol
    BRI_TypeID          type_id,        //  -- type of the symbol
    BRI_SymbolAttributes sym_type,      //  -- flags
    BRI_SymbolAttributes access );      //  -- access flags (e.g. private)

BRI_ScopeID     BRIStartScope(          // Add a scope to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_ScopeID         index,          //  -- index of this scope
    BRI_ScopeType       flags,          //  -- flags (e.g. kind of scope)
    uint_32 owner );                    //  -- owner of the scope

BRI_ScopeID     BRIStartFnScope(        // Add a function scope to the file
    BRI_HANDLE          handle,         // -- browse handle
    BRI_ScopeID         index,          // -- index of this scope
    BRI_StringID        name_id,        // -- name of the owning function
    BRI_TypeID          type_id );      // -- type of the owning function

void            BRIEndScope(            // Signal end of current scope
    BRI_HANDLE          handle );       //  -- browse handle

void            BRIAddDefinition(       // Add a definition to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_StringID        file,           //  -- name of the containing file
    uint_32             line,           //  -- location of definition
    uint_32             col,            //  -- location of definition
    BRI_SymbolID        symbol );       //  -- symbol being defined

// Note:  "target" below may be either a BRI_SymbolID or a
//        BRI_TypeID.

void            BRIAddReference(        // Add a reference to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_StringID        file,           //  -- name of the containing file
    uint_32             line,           //  -- location of reference
    uint_32             col,            //  -- location of reference
    uint_32             target,         //  -- target of the reference
    BRI_ReferenceType   ref_type );     //  -- kind of the reference


int             BRITypeAlreadySeen(     // Has a type already been used?
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_TypeID          type_id );      //  -- type to look for

BRI_TypeID      BRIAddType(             // Add a type to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_TypeID          type_id,        //  -- index of type to add
    BRI_TypeCode        code,           //  -- kind of type, eg pointer type
    int                 num_ops,        //  -- number of ops to follow
    ... );                              //  -- uint_32 operands

BRI_TypeID      BRIVAddType(            // Add a type to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_TypeID          type_id,        //  -- index of type to add
    BRI_TypeCode        code,           //  -- kind of type, eg pointer type
    int                 num_ops,        //  -- number of ops to follow
    uint_32             *ops );         //  -- array of operands


void            BRIAddGuard(            // Add a guard to the browse file
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_GuardTypes      type,           //  -- kind of guard
    BRI_StringID        string_id,      //  -- name of the macro involved
    uint_32             num_params,     //  -- number of macro params
    uint_32             length,         //  -- length of the definition
    uint_8 const        *defn );        //  -- definition of the macro


void            BRISetPos(              // Change the current position
    BRI_HANDLE          handle,         //  -- browse handle
    uint_32             line,           //  -- new line number
    uint_32             col );          //  -- new column number

void            BRIAddPCHInclude(       // Add a PCH Include statement
    BRI_HANDLE          handle,         //  -- browse handle
    BRI_StringID        filename_id );  //  -- name of the PCH file

#endif  // B_WRITE_H
