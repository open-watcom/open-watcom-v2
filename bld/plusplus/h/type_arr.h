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
    Use as follows for initializing arrays that index via type->id

    #define ENTRY_ERROR blah_error,
    ...
    #define ENTRY_FREE  blah_free,

    #include "type_arr.h"
*/
ENTRY_ERROR
ENTRY_BOOL
ENTRY_CHAR
ENTRY_SCHAR
ENTRY_UCHAR
ENTRY_WCHAR
ENTRY_SSHORT
ENTRY_USHORT
ENTRY_SINT
ENTRY_UINT
ENTRY_SLONG
ENTRY_ULONG
ENTRY_SLONG64
ENTRY_ULONG64
ENTRY_FLOAT
ENTRY_DOUBLE
ENTRY_LONG_DOUBLE
ENTRY_ENUM
ENTRY_POINTER
ENTRY_TYPEDEF
ENTRY_CLASS
ENTRY_BITFIELD
ENTRY_FUNCTION
ENTRY_ARRAY
ENTRY_DOT_DOT_DOT
ENTRY_VOID
ENTRY_MODIFIER
ENTRY_MEMBER_POINTER
ENTRY_GENERIC

#undef ENTRY_ERROR
#undef ENTRY_CHAR
#undef ENTRY_SCHAR
#undef ENTRY_UCHAR
#undef ENTRY_WCHAR
#undef ENTRY_SSHORT
#undef ENTRY_USHORT
#undef ENTRY_SINT
#undef ENTRY_UINT
#undef ENTRY_SLONG
#undef ENTRY_ULONG
#undef ENTRY_FLOAT
#undef ENTRY_DOUBLE
#undef ENTRY_LONG_DOUBLE
#undef ENTRY_ENUM
#undef ENTRY_POINTER
#undef ENTRY_TYPEDEF
#undef ENTRY_CLASS
#undef ENTRY_BITFIELD
#undef ENTRY_FUNCTION
#undef ENTRY_ARRAY
#undef ENTRY_DOT_DOT_DOT
#undef ENTRY_VOID
#undef ENTRY_MODIFIER
#undef ENTRY_MEMBER_POINTER
#undef ENTRY_GENERIC
