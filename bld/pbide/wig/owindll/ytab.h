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


#ifndef __YYSTYPE_DEFINED
#define __YYSTYPE_DEFINED
typedef union {
        char            *o_string;
        id_type         o_type;
        long            o_value;
        VarInfo         o_var;
        TypeInfo        o_typeinfo;
        ArrayInfo       o_array;
        List            *o_list;
} YYSTYPE;
#endif
#define ST_FUNCTION             0x7f
#define ST_SUBROUTINE           0x80
#define ST_ON                   0x81
#define ST_END                  0x82
#define ST_FROM                 0x83
#define ST_PUBLIC               0x84
#define ST_PROTECTED            0x85
#define ST_PRIVATE              0x86
#define ST_COMMENT              0x87
#define ST_PERIOD               0x88
#define ST_COLON                0x89
#define ST_COMMA                0x8a
#define ST_LSQ_BRACKET          0x8b
#define ST_RSQ_BRACKET          0x8c
#define ST_LBRACKET             0x8d
#define ST_RBRACKET             0x8e
#define ST_EQ                   0x8f
#define ST_EXCLAM               0x90
#define ST_DOLLAR               0x91
#define ST_FORWARD              0x92
#define ST_TYPE                 0x93
#define ST_GLOBAL               0x94
#define ST_SHARED               0x95
#define ST_PROTOTYPES           0x96
#define ST_VARIABLES            0x97
#define ST_NOTYPE               0x98
#define ST_REF                  0x99
#define ST_LIBRARY              0x9a
#define ST_RETURN               0x9b
#define ST_TO                   0x9c
#define ID_IDENTIFIER           0x9d
#define CT_INTEGER              0x9e
#define CT_STRING               0x9f
#define FI_EOF                  0xa0
#define FI_EOL                  0xa1
#define FI_COMMENT              0xa2
