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

char classify_escape_char(      // CLASSIFY TYPE OF ESCAPE
    char chr )                  // - character after "\"
;
char *CppArrayDtorName(         // CREATE NAME FOR ARRAY DTOR
    TYPE ar_type )              // - array type
;
char *CppConstructorName(       // GET C++ CONSTRUCTOR NAME
    void )
;
char *CppConversionName(        // GET C++ USER-DEFINED CONVERSION NAME
    void )
;
char *CppDestructorName(        // GET C++ DESTRUCTOR NAME
    void )
;
char* CppGetEnv                 // COVER FOR getenv
    ( char const * name )       // - environment variable
;
char *CppIndexMappingName(      // CREATE NAME OF VIRTUAL BASE INDEX MAPPING
    SCOPE from,                 // - from scope
    SCOPE to )                  // - to scope
;
boolean CppLookupName(          // FIND OPERATOR FOR NAME (FALSE IF NOT FOUND)
    char *name,                 // - name to find
    CGOP *oper )                // - index found
;
char *CppMangleName(            // MANGLE SYMBOL NAME
    char *patbuff,              // - control of result
    SYMBOL sym )                // - symbol to mangle
;
char *CppMembPtrOffsetName(     // CREATE NAME FOR MEMBER-PTR OFFSET FUNCTION
    SYMBOL sym )                // - field for offsetting
;
boolean IsCppNameInterestingDebug(      // CHECK FOR INTERNAL NAMES
    SYMBOL sym )                        // - symbol
;
char *CppNameDebug(             // TRANSLATE INTERNAL NAME TO DEBUGGER NAME
    SYMBOL sym )                // - symbol
;
char *CppClassPathDebug(  //TRANSLATE INTERNAL NAME TO CLASS PREFIXED DEBUGGER NAME
    SYMBOL sym )
;
char *CppNameStateTableCmd(     // NAME OF STATE-TABLE COMMAND
    unsigned index )            // - command index
;
char *CppNameUniqueNS(          // NAME OF UNIQUE NAMESPACE
    TOKEN_LOCN *locn )          // - location of start of unique namespace
;
char *CppNameThrowRo(           // NAME OF THROW R/O BLOCK
    TYPE type )                 // - the type
;
char const* CppTsName(          // MANGLED NAME FOR TYPE SIGNATURE
    TYPE type )                 // - type being signified
;
char *CppPCHDebugInfoName(      // MANGLED NAME FOR PCH DEBUG INFO
    char *include_file )        // - include file of PCH
;
char *CppNameTypeSig(           // NAME OF TYPE SIGNATURE
    TYPE type )                 // - the type
;
char *CppOperatorName(          // GET C++ OPERATOR NAME
    unsigned op )               // - operator number
;
char *CppSpecialName(           // CREATE NAME OF SPECIAL INTERNAL ID
    unsigned index )            // - index of special name
;
char *CppStaticInitName(        // CREATE NAME FOR INITIALIZING AUTO AGGREGATES
    SYMBOL sym )                // - variable to be initialized
;
char *CppStaticOnceOnlyName(    // CREATE NAME FOR ONCE ONLY CHECK OF STATICS
    void )
;
char *CppThunkName(             // CREATE NAME OF VIRTUAL FN THUNK
    SCOPE scope,                // - exact class that needs vftables
    THUNK_ACTION *thunk )       // - thunk being created
;
char *CppTypeidName(            // CREATE NAME FOR TYPEID
    unsigned *len,              // - addr( len of type mangle )
    TYPE type )                 // - type for typeid
;
char *CppGetTypeidContents(     // CREATE CONTENTS FOR TYPEID STRUCTURE
    TYPE type,                  // - typeid type
    unsigned *len )             // - addr( strlen of contents )
;
char *CppVBTableName(           // CREATE NAME OF VIRTUAL BASE OFFSET TABLE
    SCOPE scope,                // - class table is used in
    target_offset_t delta )     // - offset of vbptr table is used for
;
char *CppVATableName(           // CREATE NAME OF VIRTUAL FN ADJUSTOR TABLE
    SCOPE scope )               // - class table is used in
;
char *CppVFTableName(           // CREATE NAME OF VIRTUAL FN ADDR TABLE
    SCOPE scope,                // - class table is used in
    target_offset_t delta )     // - offset of vfptr table is used for
;
char escape_char(               // GET ESCAPE CHAR FOR A LETTER
    char chr )                  // - character after "\"
;
char hex_dig(                   // GET HEXADECIMAL DIGIT FOR CHAR (OR 16)
    char chr )                  // - character
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
boolean IsCppMembPtrOffsetName( // TEST IF NAME IS MEMBER-PTR OFFSET FUNCTION
    char *name )                // - name of field for offsetting
;
boolean IsCppSpecialName(       // TEST IF NAME IS SPECIAL NAME
    char *name,                 // - name to find
    unsigned *idx )             // - index found
;
boolean IsVftName(              // TEST IF SYMBOL IS VFT NAME
    char* name )                // - name to be tested
;
char octal_dig(                 // GET OCTAL DIGIT FOR CHAR (OR 8)
    char chr )                  // - character
;
double SafeAtof(                // CONVERT STRING TO DOUBLE
    char *src )                 // - source charcaters
;
char *stdcpy(                   // CONCATENATE DECIMAL NUMBER
    char *tgt,                  // - target location
    unsigned value )            // - value to be concatenated
;
char *sticpy(                   // CONCATENATE INTEGER NUMBER
    char *tgt,                  // - target location
    int value )                 // - value to be concatenated
;
#if __WATCOMC__ >= 1100
char *sti64cpy(                 // CONCATENATE I64 NUMBER
    char *tgt,                  // - target location
    __int64 value )             // - value to be concatenated
;
#endif
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
int strpref(                    // IS STRING A PREFIX OF A STRING
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
