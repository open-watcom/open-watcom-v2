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


#ifndef __ITERFREC_H_
#define __ITERFREC_H_

//---------------------------------------------------------------------------
// ITERFREC -- classes to iterate over a file of records
//
// 92/12/30 -- J.W.Welch        -- defined
//---------------------------------------------------------------------------


#include "truefals.h"
#include "iters.h"
#include "fstream.h"


//---------------------------------------------------------------------------
// IterFileRec -- iterator over a file of records
//---------------------------------------------------------------------------

class IterFileRec
    : public Iter
{                               // private data
                                // ------------
ifstream &file;                 // file being read
const char* filename;           // name of file
char buffer[256];               // buffer

private:                        // private functions
                                // -----------------
virtual Boolean next            // NEXT ITERATION
        ( void );
virtual void* element           // GET ELEMENT POINTER
        ( void ) const;

public:                         // public functions
                                // ----------------

IterFileRec                     // CONSTRUCTOR
    ( const char* fname         // - file name
    , ifstream &file            // - file information
    , Boolean& retn             // - TRUE ==> file opened
    , Boolean reqd = TRUE );    // - TRUE ==> file must exist

~IterFileRec();                 // DESTRUCTOR

};


//---------------------------------------------------------------------------
// IterOverFileRec -- iteration over records in a file
//---------------------------------------------------------------------------

class IterOverFileRec
    : public IterOver
{                               // private functions
                                // -----------------
    virtual void process        // PROCESS EACH ITERATION
        ( void* element         // - element
        , void* control_info )  // - control information
        = 0;

};

#endif

