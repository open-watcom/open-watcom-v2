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
* Description:  "Human readable" PowerPC type definitions.
*
****************************************************************************/


#if !defined(pick)
#define pick( e, x0, x1, x2 )   PPCT_##e,
#define defining_enums
enum {
#endif
    /* Target byte order */
    /* integers */
    pick( BYTE,         BYTE,           1, U1 )
    pick( HWORD,        HWORD,          1, U2 )
    pick( WORD,         WORD,           1, U4 )
    pick( DWORD,        DWORD,          1, U8 )
    pick( CHAR,         CHAR,           0, I1 )
    pick( SHORT,        SHORT,          0, I2 )
    pick( LONG,         LONG,           0, I4 )
    pick( INT64,        INT64,          0, I8 )
    pick( UCHAR,        UCHAR,          0, U1 )
    pick( USHORT,       USHORT,         0, U2 )
    pick( ULONG,        ULONG,          0, U4 )
    pick( UINT64,       UNSIGNED64,     0, U8 )
    /* addresses */
    pick( N32_PTR,      N32_PTR,        1, N32 )
//    pick( N64_PTR,    N64_PTR,        1, N64 )
    /* floats */
    pick( FLOAT,        FLOAT,          0, F4 )
    pick( DOUBLE,       DOUBLE,         0, F8 )
    pick( HDOUBLE,      NIL,            1, F8 )
    /* customs */

    /* Host byte order */
    /* integers */
    pick( H_BYTE,       BYTE,           1, H_U1 )
    pick( H_HWORD,      HWORD,          1, H_U2 )
    pick( H_WORD,       WORD,           1, H_U4 )
    pick( H_DWORD,      DWORD,          1, H_U8 )
    pick( H_CHAR,       CHAR,           0, H_I1 )
    pick( H_SHORT,      SHORT,          0, H_I2 )
    pick( H_LONG,       LONG,           0, H_I4 )
    pick( H_INT64,      INT64,          0, H_I8 )
    pick( H_UCHAR,      UCHAR,          0, H_U1 )
    pick( H_USHORT,     USHORT,         0, H_U2 )
    pick( H_ULONG,      ULONG,          0, H_U4 )
    pick( H_UINT64,     UNSIGNED64,     0, H_U8 )
    /* addresses */
    pick( H_N32_PTR,    N32_PTR,        1, H_N32 )
//    pick( H_N64_PTR,  N64_PTR,        1, H_N64 )
    /* floats */
    pick( H_FLOAT,      FLOAT,          0, H_F4 )
    pick( H_DOUBLE,     DOUBLE,         0, H_F8 )
    pick( H_HDOUBLE,    NIL,            1, H_F8 )
    /* customs */

#if defined(defining_enums)
PPCT_LAST_ONE };
#undef pick
#undef defining_enums
#endif
