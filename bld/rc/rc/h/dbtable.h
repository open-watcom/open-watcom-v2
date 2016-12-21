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



/***************************************************************************
 * FILE Format:
 *
 *      DBTableHeader
 *
 *          uint_8              (repeated 256 times)
 *              this array identifies which characters begin double byte
 *              characters.  If array[x] == DB_BEG_CHAR then x begins
 *              a double byte character.
 *
 *          DBIndexEntry        (repeated num_indices times)
 *              to find the translation of a character x find the index
 *              record where min <= x <= max and  use
 *              (base + x - min) as an index into the array of entries
 *              that follows
 *
 *          uint_16             (repeated( num_entries times)
 *              these are the actual translation values of the characters
 **************************************************************************/

#define DB_TABLE_SIG_1          0xCD235673
#define DB_TABLE_SIG_2          0xDD429632

#define DB_BEG_CHAR             1

#define DB_TABLE_VER            1

#pragma pack(1)
typedef struct {
    uint_32     sig[2];
    uint_16     ver;
    uint_16     num_indices;    /* the number of contiguous ranges of
                                 * characters in the code page */
    uint_16     num_entries;    /* the number of valid characters in the
                                 * code page */
    uint_16     reserved[4];
}DBTableHeader;

/* Each DBIndexEntry represents a contiguous range of characters in the
 * code page */
typedef struct {
    uint_16     min;            /* the first character in the range */
    uint_16     max;            /* the last character in the range */
    uint_16     base;           /* the offset into the entries array
                                 * of the translation of the first
                                 * character in the range */
}DBIndexEntry;
#pragma pack()

extern int          DBStringToUnicode( int len, const char *str, char *buf );
extern RcStatus     OpenTable( char *fname, char *path );
extern void         DbtableInitStatics( void );
extern const char   *GetLeadBytes( void );
extern void         FiniTable( void );
