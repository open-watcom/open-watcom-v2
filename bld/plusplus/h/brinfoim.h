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


#ifndef __BRINFOIM_H__
#define __BRINFOIM_H__

// BRINFOIM -- private implementation-dependent definitions for browsing
//          -- these are visible only to the Browse implementation modules

#include "brinfo.h"

#ifdef OPT_BR

#include "cgio.h"
#include "b_write.h"

typedef struct _mac_value MACVALUE;
typedef struct MACDEP MACDEP;
typedef struct SRCDEP SRCDEP;

typedef enum                    // MAC_VTYPE -- types of macro values
{   MVT_VALUE                   // - macro defined, value used
,   MVT_DEFINED                 // - macro defined
,   MVT_UNDEFED                 // - macro not defined
} MAC_VTYPE;

// PROTOTYPES

void BrinfDepInit               // MODULE INITIALIZATION
    ( void )
;
void BrinfDepFini               // MODULE COMPLETION
    ( void )
;
void BrinfDepMacAdd             // ADD A MACRO DEPENDENCY
    ( MEPTR macro               // - the macro
    , MACVALUE* value           // - value
    , MAC_VTYPE type )          // - dependency type
;
void BrinfDepRestart            // MODULE RESTART DURING PCH READ
    ( void )
;
SRCDEP* BrinfDepSrcBeg          // BEGIN SOURCE-FILE DEPENDENCIES
    ( SRCFILE srcfile )         // - source file
;
void BrinfDepSrcEnd             // END OF SOURCE-FILE DEPENDENCIES
    ( void )
;
SRCFILE BrinfDepSrcfile         // GET SRCFILE FOR DEPENDENCY
    ( SRCDEP const * sd )       // - dependency
;
char const * BrinfDepSrcFname   // GET FILE NAME FOR SOURCE-DEPENDENCY
    ( SRCDEP const *sd )        // - dependency
;
void BrinfDepWrite              // WRITE DEPENDENCY INFORMATION
    ( SRCDEP const *sd )        // - dependency for source file
;
void BrinfIcReference           // WRITE OUT A REFERENCE
    ( CGINTEROP opcode          // - opcode
    , void const *ptr           // - operand
    , TOKEN_LOCN const* locn )  // - location
;
MACVALUE* BrinfMacAddDefin      // ADD A defined(name) VALUE
    ( char const* name          // - macro name
    , unsigned nlen )           // - name length
;
MACVALUE* BrinfMacAddUndef      // ADD A !defined(name) VALUE
    ( char const* name          // - macro name
    , unsigned nlen )           // - name length
;
MACVALUE* BrinfMacAddValue      // ADD A VALUE
    ( MEPTR mac )               // - the macro
;
void BrinfMacRestart            // RESTART DURING PCH READ
    ( void )
;
void BrinfMacUndef              // RECORD UNDEFINE OF MACRO
    ( MEPTR macro               // - macro
    , SRCFILE src )             // - source file in which it occurred
;
SRCFILE BrinfMacUndefSource     // GET SOURCE OF AN UNDEF
    ( char const *name )        // - macro name
;
uint_8 const * BrinfMacValueDefn // GET DEFINITION FOR MACVALUE DECLARATION
    ( MACVALUE const *mv        // - the MACVALUE
    , unsigned* a_length )      // - addr[ length of definition ]
;
TOKEN_LOCN const* BrinfMacValueLocn // GET LOCATION FOR MACVALUE DECLARATION
    ( MACVALUE const *mv )      // - the MACVALUE
;
char const *BrinfMacValueName   // GET NAME FROM MACVALUE
    ( MACVALUE const *mv )      // - the MACVALUE
;
unsigned BrinfMacValueParmCount // GET # PARAMETERS FOR MACVALUE DECLARATION
    ( MACVALUE const *mv )      // - the MACVALUE
;
void BrinfWriteDepMacDefed      // WRITE DEPENDENCY ON MACRO DEFINED
    ( MACVALUE const * value )  // - value
;
void BrinfWriteDepMacUndefed    // WRITE DEPENDENCY ON MACRO UNDEFINED
    ( MACVALUE const * value )  // - value
;
void BrinfWriteDepMacVal        // WRITE DEPENDENCY ON MACRO VALUE
    ( MACVALUE const * value )  // - value
;
void BrinfSrcBeg                // START OF SOURCE (DEPENDENCY)
    ( SRCFILE srcfile )         // - source file
;
void BrinfSrcRestart            // BrinfSrc RESTART FOR PCH READ
    ( void )
;
void BrinfSrcEndFile            // END OF A SOURCE FILE
    ( SRCFILE srcfile )         // - file being closed
;
void BrinfSrcFini               // BrinfSrc COMPLETION
    ( void )
;
void BrinfSrcInc                // START OF SRC-INCLUDE
    ( void* short_name          // - short name
    , TOKEN_LOCN const* locn )  // - location
;
void BrinfSrcInit               // BrinfSrc INITIALIZATION
    ( void )
;
void BrinfSrcMacDecl            // MACRO DECLARATION
    ( MACVALUE const * defn )   // - definition value
;
void BrinfSrcMacReference       // MACRO REFERENCE
    ( MACVALUE const * val )    // - reference value
;
void BrinfSrcSync               // SYNCHONIZE THE OPEN/CLOSE DIRECTIVES
    ( TOKEN_LOCN const* locn )  // - synchronizing location
;
void BrinfWriteFile             // WRITE BROWSE INFORMATION TO FILE
    ( CGFILE* intermediate      // - intermediate file
    , BRI_Routines const * rtns // - call-back routines
    , void * file_handle        // - file handle
    , BRI_Handle* b_handle )    // - browse handle
;
void* BrinfCreateFile           // CREATE A BROWSE HANDLE FOR LATER WRITING
    ( BRI_Routines const *rtns )// - call-back routines
;
void BrinfWriteIns              // WRITE IC, PTR OPERAND
    ( CGINTEROP opcode          // - opcode
    , void const *ptr )         // - operand
;
void* BrinfWritePchFile         // WRITE OUT BROWSE INFORMATION TO PCH
    ( CGFILE *virtual_file
    , BRI_Routines const *rtns
    , void * file_handle
    , unsigned long start )
;
void BrinfReadPchFile           // READ BROWSE INFORMATION FROM A PCH FILE
    ( BRI_PCHRtns const *rtns
    , void * file_handle )
;
void BrinfWriteSrcClose         // WRITE IC'S FOR SOURCE CLOSE
    ( void )
;
void BrinfWriteSrcOpen          // WRITE IC'S FOR SOURCE OPEN
    ( char const * fname        // - file name (as entered)
    , TOKEN_LOCN const* locn    // - location
    , SRCDEP const *dep )       // - dependency entry
;


#endif  // OPT_BR
#endif  // __BRINFOIM_H__
