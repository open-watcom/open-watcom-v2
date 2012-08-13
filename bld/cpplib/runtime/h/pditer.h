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


#ifndef __PDITER_H__
#define __PDITER_H__

// PDITER -- structure used to iterate through calls, on alpha


typedef void (*code_ptr)( void );
typedef void* data_ptr;

struct _UINT64                  // 64-bit number
{   unsigned w0;                // - word[0]
    unsigned w1;                // - word[1]
};

struct _PDITER                  // Iteration structure
{   data_ptr SP_reg;            // - SP register contents
    code_ptr RA_reg;            // - RA (return register) register
    unsigned SP_decrement;      // - amount SP decremented by in routine
    unsigned RA_offset;         // - offset from SP where RA stored
    ProcDesc* procDescr;        // - procedure descriptor (or NULL)
    RW_DTREG* rw;               // - R/W block (or NULL)
    RO_DTREG* ro;               // - R/O block (or NULL)
};


// Prototypes

extern "C" {

code_ptr CPPLIB( alpha_get_ra ) // GET RA register
    ( void )
;
data_ptr CPPLIB( alpha_get_sp ) // GET SP register
    ( void )
;
int CPPLIB( PditerInit )        // START THE ITERATION
    ( _PDITER* pdit             // - iteration block
    , void* SP_reg              // - SP register contents
    , void (*RA_reg)( void ) )  // - RA (return register) register
;
int CPPLIB( PditerUnwind )      // UNWIND ONE PROCEDURE CALL
    ( _PDITER* pdit )           // - iteration block
;


};


#endif
