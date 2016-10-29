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
* Description:  Definitions used in parsing Microsoft command files.
*
****************************************************************************/


typedef enum {
    BUFFERED,
    NONBUFFERED,
    COMMANDLINE,
    INTERACTIVE
} how_type;

typedef enum {
    MIDST,
    ENDOFLINE,
    ENDOFFILE,
    ENDOFCMD
} where_type;

typedef enum {
    SEP_NO,
    SEP_COLON,
    SEP_AT,
    SEP_QUOTE,
    SEP_EQUALS,
    SEP_PERIOD,
    SEP_SPACE
} sep_type;

typedef struct cmdfilelist {
    struct cmdfilelist *next;   // the next file in the list.
    char            *buffer;    // the data in the file.
    char            *current;   // current location in the data.
    char            *token;     // token start;
    size_t          len;        // token length;
    how_type        how;        // as above
    how_type        oldhow;     // also as above.
    where_type      where;
    f_handle        file;
    char            *name;
} cmdfilelist;

extern cmdfilelist  *CmdFile;
extern bool         MakeToken( sep_type separator, bool include_fn );
