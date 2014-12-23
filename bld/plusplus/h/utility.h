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


// UTILITY.H -- C++ Utility Definitions
//
// 91/06/03 -- J.W.Welch        -- defined

#include "ppops.h"
#include "specname.h"

int classify_escape_char(       // CLASSIFY TYPE OF ESCAPE
    int chr )                   // - character after "\"
;
NAME CppArrayDtorName(          // CREATE NAME FOR ARRAY DTOR
    TYPE ar_type )              // - array type
;
NAME CppConstructorName(        // GET C++ CONSTRUCTOR NAME
    void )
;
NAME CppConversionName(         // GET C++ USER-DEFINED CONVERSION NAME
    void )
;
NAME CppDestructorName(         // GET C++ DESTRUCTOR NAME
    void )
;
const char *CppGetEnv           // COVER FOR getenv
    ( char const * name )       // - environment variable
;
NAME CppIndexMappingName(       // CREATE NAME OF VIRTUAL BASE INDEX MAPPING
    SCOPE from,                 // - from scope
    SCOPE to )                  // - to scope
;
bool CppLookupOperatorName(     // FIND OPERATOR FOR NAME (FALSE IF NOT FOUND)
    NAME name,                  // - name to find
    CGOP *oper )                // - index found
;
const char *GetMangledName(     // MANGLE SYMBOL NAME
    SYMBOL sym )                // - symbol to mangle
;
NAME CppMembPtrOffsetName(      // CREATE NAME FOR MEMBER-PTR OFFSET FUNCTION
    SYMBOL sym )                // - field for offsetting
;
bool IsCppNameInterestingDebug( // CHECK FOR INTERNAL NAMES
    SYMBOL sym )                // - symbol
;
char *CppNameDebug(             // TRANSLATE INTERNAL NAME TO DEBUGGER NAME
    SYMBOL sym )                // - symbol
;
char *CppClassPathDebug(        //TRANSLATE INTERNAL NAME TO CLASS PREFIXED DEBUGGER NAME
    SYMBOL sym )
;
NAME CppNameStateTableCmd(      // NAME OF STATE-TABLE COMMAND
    unsigned index )            // - command index
;
NAME CppNameUniqueNS(           // NAME OF UNIQUE NAMESPACE
    TOKEN_LOCN *locn )          // - location of start of unique namespace
;
NAME CppNameThrowRo(            // NAME OF THROW R/O BLOCK
    TYPE type )                 // - the type
;
NAME CppTsName(                 // MANGLED NAME FOR TYPE SIGNATURE
    TYPE type )                 // - type being signified
;
NAME CppPCHDebugInfoName(       // MANGLED NAME FOR PCH DEBUG INFO
    char *include_file )        // - include file of PCH
;
NAME CppNameTypeSig(            // NAME OF TYPE SIGNATURE
    TYPE type )                 // - the type
;
NAME CppOperatorName(           // GET C++ OPERATOR NAME
    CGOP op )                   // - operator number
;
NAME CppSpecialName(            // CREATE NAME OF SPECIAL INTERNAL ID
    unsigned index )            // - index of special name
;
NAME CppStaticInitName(         // CREATE NAME FOR INITIALIZING AUTO AGGREGATES
    SYMBOL sym )                // - variable to be initialized
;
NAME CppStaticOnceOnlyName(     // CREATE NAME FOR ONCE ONLY CHECK OF STATICS
    void )
;
NAME CppThunkName(              // CREATE NAME OF VIRTUAL FN THUNK
    SCOPE scope,                // - exact class that needs vftables
    THUNK_ACTION *thunk )       // - thunk being created
;
NAME CppTypeidName(             // CREATE NAME FOR TYPEID
    unsigned *len,              // - addr( len of type mangle )
    TYPE type )                 // - type for typeid
;
char *CppGetTypeidContents(     // CREATE CONTENTS FOR TYPEID STRUCTURE
    TYPE type,                  // - typeid type
    unsigned *len )             // - addr( strlen of contents )
;
NAME CppVBTableName(            // CREATE NAME OF VIRTUAL BASE OFFSET TABLE
    SCOPE scope,                // - class table is used in
    target_offset_t delta )     // - offset of vbptr table is used for
;
NAME CppVATableName(            // CREATE NAME OF VIRTUAL FN ADJUSTOR TABLE
    SCOPE scope )               // - class table is used in
;
NAME CppVFTableName(            // CREATE NAME OF VIRTUAL FN ADDR TABLE
    SCOPE scope,                // - class table is used in
    target_offset_t delta )     // - offset of vfptr table is used for
;
int escape_char(                // GET ESCAPE CHAR FOR A LETTER
    int chr )                   // - character after "\"
;
int hex_dig(                    // GET HEXADECIMAL DIGIT FOR CHAR (OR 16)
    int chr )                   // - character
;
void Int64From32                // CREATE 64-BIT VALUE FROM 32-BIT VALUE
    ( TYPE type                 // - source integral type (signed or unsigned)
    , signed_32 value           // - integral value (signed or unsigned)
    , signed_64* result )       // - addr[ result (signed or unsigned) ]
;
void Int64FromU32               // CREATE 64-BIT VALUE FROM UNSIGNED 32-BIT VALUE
    ( unsigned_32 value         // - integral value (unsigned)
    , unsigned_64* result )     // - addr[ result (unsigned) ]
;
bool IsCppMembPtrOffsetName(    // TEST IF NAME IS MEMBER-PTR OFFSET FUNCTION
    NAME name )                 // - name of field for offsetting
;
bool IsCppSpecialName(          // TEST IF NAME IS SPECIAL NAME
    NAME name,                  // - name to find
    unsigned *idx )             // - index found
;
bool IsVftName(                 // TEST IF SYMBOL IS VFT NAME
    NAME name )                 // - name to be tested
;
int octal_dig(                  // GET OCTAL DIGIT FOR CHAR (OR 8)
    int chr )                   // - character
;
double SafeAtof(                // CONVERT STRING TO DOUBLE
    char *src )                 // - source charcaters
;
char *stdcpy(                   // CONCATENATE DECIMAL NUMBER
    char *tgt,                  // - target location
    unsigned int value )        // - value to be concatenated
;
char *sticpy(                   // CONCATENATE INTEGER NUMBER
    char *tgt,                  // - target location
    signed int value )          // - value to be concatenated
;
char *sti64cpy(                 // CONCATENATE I64 NUMBER
    char *tgt,                  // - target location
    signed_64 value )           // - value to be concatenated
;
char *stpcpy(                   // CONCATENATE STRING AS STRING
    char *string,               // - target location
    const char *src_string )    // - source string
;
char *stpcpy_after(             // COPY STRING, UPDATE SOURCE POINTER
    char *tgt,                  // - target
    char const **src )          // - addr( source )
;
char *strend(                   // POINT AT END OF STRING
    char *string )              // - string
;
bool strpref(                   // IS STRING A PREFIX OF A STRING
    char const *prefix,         // - possible prefix
    char const *str )           // - string
;
char *strsave(                  // ALLOCATE AND SAVE A STRING
    const char *str )           // - source string
;
char *strpermsave(              // ALLOCATE AND SAVE A STRING IN PERMANENT STORAGE
    const char *str )           // - source string
;
char *stvcpyr(                  // CONCATENATE VECTOR AS A STRING REVERSE
    char *string,               // - target location
    const char *vector,         // - source vector to be copied reversed
    size_t vsize )              // - size of source vector
;
char *stvcpy(                   // CONCATENATE VECTOR AS A STRING
    char *string,               // - target location
    const char *vector,         // - source vector to be copied
    size_t vsize )              // - size of source vector
;
char *stvcpy_after(             // COPY VECTOR, UPDATE SOURCE POINTER
    char *tgt,                  // - target
    char const **src,           // - addr( source )
    size_t vsize )              // - vector size
;
char *vctsave(                  // ALLOCATE AND SAVE VECTOR AS STRING
    const char *vector,         // - source vector
    size_t vsize )              // - size of source vector
;
void CppStartFuncMessage( SYMBOL sym );
