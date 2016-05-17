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
* Description:  make C file for errors messages
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined( __WATCOMC__ ) || !defined( __UNIX__ )
#include <process.h>
#endif

#include "clibext.h"

#define NULLCHAR        '\0'

typedef struct word_list {
    char                *word;
    int                 word_num;
    int                 ref_count;
    struct word_list    *link;
    struct word_list    *sortlink;
} word_list;

typedef struct msg_word {
    struct word_list    *word;
    struct msg_word     *link;
} msg_word;

typedef struct msg_list {
    struct msg_word     *msg;
    int                 caret;
    int                 count;
    struct msg_list     *link;
} msg_list;

typedef struct group_list {
    struct group_list   *link;
    char                name[3];
    int                 start_msg_num;
    int                 end_msg_num;
} group_list;

static  void            AddWords( msg_list *curr_msg, char *text );
static  msg_list        *InitMsg( void );
static  word_list       *ProcessWord( char *text );

static  word_list       *HeadWord;
static  msg_list        *HeadMsg;
static  group_list      *HeadGroup;
static  FILE            *MsgFile;
static  FILE            *ErrMsg;
static  FILE            *ErrGrp;
static  FILE            *ErrCod;
static  FILE            *ErrFile;
static  FILE            *RCFile;
static  int             MaxRefCount;
static  word_list       *SortPtr;
static  word_list       *SortHead;
static  int             RecNum;

#define BUFF_LEN        133

// Error messages contain a 5-character field with the following information:
//      1. language of the message
//              english                         'e' or ' '
//              japanese                        'j'
//      2. message used by WATFOR-77            'w'
//         message used by WFC                  'o'
//         message used by WFL                  'l'
//         message used by WFL, WFC             'c'
//         message used by WATFOR-77, WFL, WFC  '.'
//         message used by WATFOR-77, WFC       ' '
//      3. message is 80386-specific            '3'
//         message is 8086-specific             'i'
//         message is 80x86-specific            '8'
//         message is RISC-specific             'r'
//         message is target-independant        ' '
//      4. message is used at compile-time      'c'
//         message is used at run-time          'r'
//         message is used during both          ' '
//      5. caret information                    '0', '1' or '2'

static  char    sw_language = { ' ' };
static  char    sw_compiler = { ' ' };
static  char    sw_target = { ' ' };
static  char    sw_used_at = { ' ' };
static  char    *input_file = NULL;
static  char    file_prefix[2] = { 0 };

static  void    ProcArgs( char *args )
//====================================
{
    size_t  len;
    char    delim = ' ';
    char    c;
    char    *p;

    while( *args == ' ' ) {
        ++args;
    }
    if( *args == '\0' )
        return;
    len = strlen( args ) + 1;
    if( *args == '"' ) {
        delim = '"';
        len -= 2;
        ++args;
    }
    input_file = malloc( len );
    p = input_file;
    while( (c = *args) != '\0' ) {
        ++args;
        if( c == delim ) {
            break;
        }
        *p++ = c;
    }
    *p = '\0';
    while( *args == ' ' ) {
        ++args;
    }
    sw_language = *args;
    if( sw_language == 'e' ) {
        sw_language = ' ';
    }
    if( sw_language != ' ' ) {
        file_prefix[0] = sw_language;
        file_prefix[1] = '\0';
    }
    ++args;
    if( *args == '\0' )
        return;
    sw_compiler = *args;
    ++args;
    if( *args == '\0' )
        return;
    sw_target = *args;
    ++args;
    if( *args == '\0' )
        return;
    sw_used_at = *args;
}


static  int     Initialize( void )
//================================
{
    char    output_file[14];

    MsgFile = fopen( input_file, "rt" );
    if( MsgFile == NULL ) {
        return( 1 );
    }
    sprintf( output_file, "%serrmsg.gc", file_prefix );
    ErrMsg = fopen( output_file, "wt" );
    if( ErrMsg == NULL ) {
        fclose( MsgFile );
        return( 1 );
    }
    sprintf( output_file, "%serrgrp.gc", file_prefix );
    ErrGrp = fopen( output_file, "wt" );
    if( ErrGrp == NULL ) {
        fclose( MsgFile );
        fclose( ErrMsg );
        return( 1 );
    }
    sprintf( output_file, "%serrcod.gh", file_prefix );
    ErrCod = fopen( output_file, "wt" );
    if( ErrCod == NULL ) {
        fclose( MsgFile );
        fclose( ErrMsg );
        fclose( ErrGrp );
        return( 1 );
    }
    sprintf( output_file, "%serrmsg.gh", file_prefix );
    ErrFile = fopen( output_file, "wt" );
    if( ErrFile == NULL ) {
        fclose( MsgFile );
        fclose( ErrMsg );
        fclose( ErrGrp );
        fclose( ErrCod );
        return( 1 );
    }
    sprintf( output_file, "%serrmsg.grc", file_prefix );
    RCFile = fopen( output_file, "wt" );
    if( RCFile == NULL ) {
        fclose( MsgFile );
        fclose( ErrMsg );
        fclose( ErrGrp );
        fclose( ErrCod );
        fclose( ErrFile );
        return( 1 );
    }
    HeadWord = NULL;
    HeadMsg = NULL;
    HeadGroup = NULL;
    MaxRefCount = 0;
    RecNum = 1 ;
    return( 0 );
}


static  void    Finalize( void )
//==============================
{
    fclose( MsgFile );
    fclose( ErrMsg );
    fclose( ErrGrp );
    fclose( ErrCod );
    fclose( ErrFile );
    fclose( RCFile );
}

static int      IsTarget( char target ) {
//=======================================

    if( (sw_target == target) || (target == ' ') ) {
        return( 1 );
    } else if( (sw_target == 'i') || (sw_target =='3') ) {
        return( target == '8' );
    }
    return( 0 );
}

static  char    UseMessage( char cmp, char target, char used_at ) {
//=================================================================

    if( sw_compiler == 'o' ) {
        if( (cmp != 'o') && (cmp != ' ') && (cmp != '.') && (cmp != 'c') ) {
            return( 0 );
        }
    } else if( sw_compiler == 'w' ) {
        if( (cmp != 'w') && (cmp != ' ') && (cmp != '.') ) {
            return( 0 );
        }
    } else if( sw_compiler == 'l' ) {
        if( (cmp != 'l') && (cmp != 'c') && (cmp != '.') ) {
            return( 0 );
        }
    }
    if( IsTarget( target ) ) {
        if( sw_compiler == 'w' ) {
            if( sw_used_at == ' ' ) {
                // load'n go compiler doesn't care whether message is
                // used at compile-time or run-time
                return( 1 );
            } else if( (sw_used_at == used_at) || (used_at == ' ') ) {
                return( 1 );
            }
        } else if( (sw_used_at == used_at) || (used_at == ' ') ) {
            return( 1 );
        }
    }
    return( 0 );
}

static  int     ReadInFile( char *buff )
//======================================
{
    size_t      len;

    for( ;; ) {
        if( fgets( buff, BUFF_LEN, MsgFile ) == NULL ) {
            return( 1 );
        }
        len = strlen( buff );
        if( buff[len - 1] == '\n' )
            buff[len - 1] = NULLCHAR;
        if( *buff != ' ' ) {
            break;
        }
    }
    return( 0 );
}


static  msg_list        *InitMsg( void )
//======================================
{
    msg_list    *curr_msg;

    curr_msg = malloc( sizeof( msg_list ) );
    curr_msg->count = 0;
    curr_msg->msg = NULL;
    curr_msg->link = NULL;
    curr_msg->caret = 0;
    return( curr_msg );
}


static  void AddWords( msg_list *curr_msg, char *text )
//=====================================================
{
    int         index;
    msg_word    *curr_word;

    curr_word = NULL;
    for( ;; ) {
        while( *text == ' ' ) {
            ++text;
        }
        if( *text == NULLCHAR )
            break;
        index = 0;
        for( ; text[index] != NULLCHAR; ++index ) {
            if( text[index] == ' ' ) {
                text[index++] = NULLCHAR;
                break;
            }
        }
        if( curr_word == NULL ) {
            curr_word = malloc( sizeof( msg_word ) );
            curr_msg->msg = curr_word;
        } else {
            curr_word->link = malloc( sizeof( msg_word ) );
            curr_word = curr_word->link;
        }
        curr_word->link = NULL;
        curr_word->word = ProcessWord( text );
        curr_msg->count++;
        text += index;
    }
}


static  word_list       *ProcessWord( char *text )
//================================================
{
    word_list   *curr_word;
    word_list   *prev_word;
    word_list   *new_word;
    char        *w1;
    char        *w2;

    prev_word = NULL;
    w1 = NULL;
    w2 = NULL;
    for( curr_word = HeadWord; curr_word != NULL; curr_word = curr_word->link ) {
        w1 = curr_word->word;
        w2 = text;
        while( *w1 != NULLCHAR && *w1 == *w2 ) {
            ++w1;
            ++w2;
        }
        if( *w2 <= *w1 )
            break;
        prev_word = curr_word;
    }
    if( ( curr_word != NULL ) && ( *w1 == *w2 ) ) {
        curr_word->ref_count++;
    } else {
        new_word = malloc( sizeof( word_list ) );
        if( prev_word == NULL ) {
            HeadWord = new_word;
        } else {
            prev_word->link = new_word;
        }
        new_word->link = curr_word;
        new_word->sortlink = NULL;
        new_word->ref_count = 1;
        new_word->word = malloc( strlen( text ) + 1 );
        strcpy( new_word->word, text );
        curr_word = new_word;
    }
    if( curr_word->ref_count > MaxRefCount ) {
        MaxRefCount = curr_word->ref_count;
    }
    return( curr_word );
}


static  int     PhraseCount( msg_list *curr_msg, msg_word *curr_word,
                             word_list *word1, word_list *word2 ) {
//===================================================================

    int         n;

    n = 0;
    for( ;; ) {
        for( ; curr_word != NULL; curr_word = curr_word->link ) {
            if( curr_word->word == word1 ) {
                if( curr_word->link == NULL ) {
                    return( n );
                }
                if( curr_word->link->word != word2 ) {
                    return( n );
                }
                ++n;
            }
        }
        curr_msg = curr_msg->link;
        if( curr_msg == NULL )
            break;
        curr_word = curr_msg->msg;
    }
    return( n );
}


static  void    Combine( word_list *word1, word_list *word2 ) {
//=============================================================

    msg_list    *curr_msg;
    msg_word    *curr_word;
    msg_word    *next_word;
    char        *phrase;
    size_t      len1;
    size_t      len2;
    word_list   *this_word;

    for( curr_msg = HeadMsg; curr_msg != NULL; curr_msg = curr_msg->link ) {
        for( curr_word = curr_msg->msg; curr_word != NULL; curr_word = curr_word->link ) {
            if( curr_word->word == word1 ) {
                next_word = curr_word->link;
                curr_word->link = next_word->link;
                curr_msg->count--;
                free( next_word );
            }
        }
    }
    len1 = strlen( word1->word );
    len2 = strlen( word2->word );
    phrase = malloc( len1 + len2 + 2 );
    memcpy( phrase, word1->word, len1 );
    phrase[len1] = ' ';
    strcpy( &phrase[len1 + 1], word2->word );
    free( word1->word );
    free( word2->word );
    word1->word = phrase;
    this_word = HeadWord;
    if( this_word == word2 ) {
        HeadWord = word2->link;
    } else {
        for(;;) {
            if( this_word->link == word2 ) {
                this_word->link = word2->link;
            }
            this_word = this_word->link;
            if( this_word == NULL ) break;
        }
    }
    free( word2 );
}


static  void    FindPhrases( void )
//=================================
{
    msg_list    *msg;
    msg_word    *word;
    word_list   *w1;
    word_list   *w2;

    for( msg = HeadMsg; (word = msg->msg) != NULL; msg = msg->link ) {
        while( word != NULL ) {
            w1 = word->word;
            if( word->link == NULL )
                break;
            w2 = word->link->word;
            if( ( w1 != w2 ) &&
                ( w1->ref_count == w2->ref_count ) &&
                ( PhraseCount( msg, word, w1, w2 ) == w1->ref_count ) ) {
                Combine( w1, w2 );
            } else {
                word = word->link;
            }
        }
    }
}

static  void    DumpMsg( void )
//=============================
{
    msg_list    *msg;
    msg_word    *word;
    group_list  *group;
    int         msg_num;
    int         word_index;
    char        delim;
    size_t      msg_len;

    if( sw_compiler == 'w' ) {
        fprintf( ErrGrp, "#if !defined( __RT__ )\n\n" );
    }
    if( (sw_compiler == 'w') ||
        ( (sw_compiler == 'o') && (sw_used_at == 'c') ) ) {
        fprintf( ErrGrp, "#define    NO__CARET  0\n" );
        fprintf( ErrGrp, "#define    OPR_CARET  1\n" );
        fprintf( ErrGrp, "#define    OPN_CARET  2\n\n" );
        fprintf( ErrGrp, "const unsigned char __FAR CaretTable[] = {\n" );
    }
    msg = HeadMsg;
    for( group = HeadGroup; group != NULL; group = group->link ) {
        msg_num = group->start_msg_num;
        if( group->end_msg_num == msg_num )
            continue;
        fprintf( ErrMsg, "static const unsigned char __FAR Msg%d[] = {\n", msg_num );
        delim = ' ';
        while( msg_num < group->end_msg_num ) {
            word_index = 0;
            msg_len = 0;
            word = msg->msg;
            if( word != NULL ) {
                fprintf( ErrMsg, "/* " );
                while( word != NULL ) {
                    fprintf( ErrMsg, "%s ", word->word->word );
                    fprintf( ErrFile, "%s ", word->word->word );
                    msg_len += strlen( word->word->word ) + 1;
                    word_index += 1 + ( word->word->word_num / 255 );
                    word = word->link;
                }
                fprintf( ErrMsg, " */\n" );
            }
            while( msg_len < 132 ) {
                fputc( ' ', ErrFile );
                ++msg_len;
            }
            fputc( '\n', ErrFile );
            fprintf( ErrMsg, "%c", delim );
            if( (sw_compiler == 'w') ||
                ( (sw_compiler == 'o') && (sw_used_at == 'c') ) ) {
                if( msg->caret == 0 ) {
                    fprintf( ErrGrp, "NO__CARET,\n" );
                } else if( msg->caret == 1 ) {
                    fprintf( ErrGrp, "OPR_CARET,\n" );
                } else {
                    fprintf( ErrGrp, "OPN_CARET,\n" );
                }
            }
            fprintf( ErrMsg, "%d", word_index );
            for( word = msg->msg; word != NULL; word = word->link ) {
                for( word_index = word->word->word_num; word_index >= 255; word_index -= 255 ) {
                    fprintf( ErrMsg, ",%d", 255 );
                }
                fprintf( ErrMsg, ",%d", word_index );
            }
            fprintf( ErrMsg, "\n" );
            delim = ',';
            ++msg_num;
            msg = msg->link;
        }
        fprintf( ErrMsg, "};\n" );
    }
    fprintf( ErrMsg, "\n\n" );
    if( (sw_compiler == 'w') ||
        ( (sw_compiler == 'o') && (sw_used_at == 'c') ) ) {
        fprintf( ErrGrp, "};\n" );
    }
    if( sw_compiler == 'w' ) {
        fprintf( ErrGrp, "\n#endif\n" );
    }
    if( (sw_compiler == 'w') ||
        ( (sw_compiler == 'o') && (sw_used_at == 'c') ) ) {
        fprintf( ErrGrp, "\n" );
    }
}


static  void    DumpGroupTable( void )
//====================================
{
    group_list  *curr;
    char        delim;

    fprintf( ErrMsg, "const unsigned char __FAR * const __FAR GroupTable[] = {\n" );
    fprintf( ErrGrp, "const unsigned char __FAR GrpCodes[] = {\n" );
    delim = ' ';
    for( curr = HeadGroup; curr != NULL; curr = curr->link ) {
        if( curr->start_msg_num == curr->end_msg_num ) {
            fprintf( ErrGrp, "    %c'%c','%c',%d\n", delim, ' ', ' ', 0 );
            fprintf( ErrMsg, "    %cNULL\n", delim );
        } else {
            fprintf( ErrMsg, "    %cMsg%d\n", delim, curr->start_msg_num );
            fprintf( ErrGrp, "    %c'%c','%c',%d\n", delim, curr->name[0],
                        curr->name[1], curr->end_msg_num - curr->start_msg_num );
        }
        delim = ',';
    }
    fprintf( ErrMsg, "};\n" );
    fprintf( ErrGrp, "};\n" );
}

static  group_list      *NextGroup( group_list *group ) {
//=======================================================

    if( group == NULL ) {
        group = HeadGroup;
    } else {
        group = group->link;
    }
    if( group != NULL ) {
        while( group->start_msg_num == group->end_msg_num ) {
            group = group->link;
        }
    }
    return( group );
}

static  void    PrtRefs( word_list *this_word ) {
//===============================================

    int         n;
    int         group_index;
    msg_list    *a_msg;
    msg_word    *a_word;
    group_list  *a_group;
    int         first;

    first = 1;
    n = this_word->ref_count;
    if( n > 5 ) {
        n = 5;
    }
    fprintf( ErrMsg, " Group:offset =" );
    group_index = 0;
    a_group = NextGroup( NULL );
    for( a_msg = HeadMsg; a_msg != NULL; a_msg = a_msg->link ) {
        for( a_word = a_msg->msg; a_word != NULL; a_word = a_word->link ) {
            if( a_word->word == this_word ) {
                break;
            }
        }
        if( a_word != NULL ) {
            if( first == 0 ) {
                fprintf( ErrMsg, "," );
            }
            fprintf( ErrMsg," %s:%2d", a_group->name, group_index );
            if( --n == 0 )
                break;
            first = 0;
        }
        if( ++group_index >= a_group->end_msg_num % 256 ) {
            group_index = 0;
            a_group = NextGroup( a_group );
        }
    }
    if( this_word->ref_count > 5 ) {
        fprintf( ErrMsg, ",..." );
    }
    fprintf( ErrMsg, " */\n" );
}


static  void    DumpErrWord( void )
//=================================
{
    word_list   *cw;
    int         first_word;
    int         phrase_count;
    size_t      sum;
    char        *word;
    size_t      len;
    int         chars_per;

    fprintf( ErrMsg, "\n\nconst char __FAR ErrWord[] = {\n" );
    phrase_count = 0;
    sum = 0;
    len = 0;
    first_word = 1;
    for( cw = SortHead; cw != NULL; cw = cw->sortlink ) {
        fprintf( ErrMsg, "          " );
        if( first_word == 0 ) {
            fprintf( ErrMsg, "," );
        }
        first_word = 0;
        word = cw->word;
        if( word == NULL ) {
            fprintf( ErrMsg, "%u", (unsigned)len );
            continue;
        }
        len = strlen( word );
        fprintf( ErrMsg, "%u", (unsigned)len );
        chars_per = 10;
        for(;;) {
            if( --chars_per == 0 ) {
                fprintf( ErrMsg, "\n              " );
                chars_per = 10;
            }
            if( *word == '\\' ) {
                fprintf( ErrMsg, ",'\\\\'" );
            } else if( *word == '\'' ) {
                fprintf( ErrMsg, ",'\\''" );
            } else {
                fprintf( ErrMsg, ",'%c'", *word );
            }
            ++word;
            if( *word == NULLCHAR ) break;
        }
        fprintf( ErrMsg, "\n" );
        fprintf( ErrMsg, "/* count=%3d", cw->ref_count );
        PrtRefs( cw );
        sum += len * cw->ref_count;
        ++phrase_count;
    }
    fprintf( ErrMsg, "};\n" );
    fprintf( ErrMsg, "/* Total number of phrases = %d */\n", phrase_count );
    printf( "Total number of phrases = %d\n", phrase_count );
    printf( "word size * reference count = %d\n", (int)sum );
}


static  int     FindMax( int upper_bound ) {
//==========================================

    word_list   *curr_word;
    int         curr_max;

    curr_max = 0;
    for( curr_word = HeadWord; curr_word != NULL; curr_word = curr_word->link ) {
        if( ( curr_word->ref_count > curr_max ) &&
            ( curr_word->ref_count < upper_bound ) ) {
            curr_max = curr_word->ref_count;
            SortPtr = curr_word;
        }
    }
    return( curr_max );
}


static  void    SortByRef( void )
//===============================
{
    word_list   *curr_word;
    word_list   *sort_tail;
    int         ceiling;
    int         first_word;

    sort_tail = NULL;
    ceiling = MaxRefCount + 1;
    first_word = 1;
    for(;;) {
        ceiling = FindMax( ceiling );
        if( ceiling == 0 ) break;
        curr_word = SortPtr;
        if( first_word == 1 ) {
            SortHead = SortPtr;
            curr_word = curr_word->link;
            sort_tail = SortHead;
            first_word = 0;
        }
        while( curr_word != NULL ) {
            if( curr_word->ref_count == ceiling ) {
                sort_tail->sortlink = curr_word;
                sort_tail = curr_word;
            }
            curr_word = curr_word->link;
        }
    }
}


static  void    ReNumber( void )
//==============================
{
    word_list   *curr_word;
    int         index;

    curr_word = SortHead;
    index = 0;
    for(;;) {
        curr_word->word_num = index;
        ++index;
        curr_word = curr_word->sortlink;
        if( curr_word == NULL ) break;
    }
}

static  void    BuildLists( void )
//================================
{
    int         index;
    group_list  *curr_group;
    int         group;
    msg_list    *prev_msg;
    msg_list    *msg_ptr;
    msg_list    *last_non_null_msg;
    msg_list    **p_null_msg;
    msg_word    *word;
    int         caret;
    char        rec[BUFF_LEN+1];
    char        msg_used_at;
    char        msg_compiler;
    char        msg_target;
    char        delim;

    fprintf( ErrCod, "#define    NO_CARROT  0\n" );
    fprintf( ErrCod, "#define    OPR_CARROT 1\n" );
    fprintf( ErrCod, "#define    OPN_CARROT 2\n" );
    fprintf( RCFile, "#include \"errcod.h\"\n\n" );
    fprintf( RCFile, "STRINGTABLE\nBEGIN\n\n" );
    group = 0;
    curr_group = NULL;
    ReadInFile( rec );
    prev_msg = NULL;
    last_non_null_msg = NULL;
    for( ;; ) {
        if( HeadGroup == NULL ) {
            HeadGroup = malloc( sizeof( group_list ) );
            curr_group = HeadGroup;
        } else {
            curr_group->link = malloc( sizeof( group_list ) );
            curr_group = curr_group->link;
        }
        curr_group->link = NULL;
        curr_group->start_msg_num = group * 256;
        curr_group->end_msg_num = curr_group->start_msg_num;
        curr_group->name[0] = rec[0];
        curr_group->name[1] = rec[1];
        curr_group->name[2] = NULLCHAR;
        for( ;; ) {
            if( ReadInFile( rec ) != 0 ) {
                fprintf( RCFile, "\nEND\n" );
                return;
            }
            ++RecNum;
            if( ( strlen( rec ) > 2 ) && ( rec[2] == ' ' ) )
                // Group record
                break;
            index = 3;
            while( rec[index] != ' ' ) {
                ++index;
            }
            // End of message ID
            rec[index] = '\0';
            ++index;
            ++index;    // skip [
            if( rec[index] != sw_language )
                continue;
            ++index;
            msg_compiler = rec[index];
            ++index;
            msg_target = rec[index];
            ++index;
            msg_used_at = rec[index];
            ++index;
            caret = rec[index] - '0';
            ++index;
            ++index;    // skip ]
            msg_ptr = InitMsg();
            if( UseMessage( msg_compiler, msg_target, msg_used_at ) ) {
                // proccess message text
                AddWords( msg_ptr, &rec[index] );
            }
            msg_ptr->caret = caret;
            if( msg_ptr->msg != NULL ) {
                fprintf( ErrCod, "#define    %s %d\n", rec, curr_group->end_msg_num );
                fprintf( RCFile, "    %s+MSG_LANG_BASE \"", rec );
                delim = ' ';
                for( word = msg_ptr->msg; word != NULL; word = word->link ) {
                    if( word->link == NULL ) {
                        delim = '"';
                    }
                    fprintf( RCFile, "%s%c", word->word->word, delim );
                }
                fprintf( RCFile, "\n" );
                last_non_null_msg = msg_ptr;
            }
            curr_group->end_msg_num++;
            if( HeadMsg == NULL ) {
                HeadMsg = msg_ptr;
            } else {
                prev_msg->link = msg_ptr;
            }
            prev_msg = msg_ptr;
        }
        prev_msg = last_non_null_msg;
        if( last_non_null_msg == NULL ) {
            p_null_msg = &HeadMsg;
        } else {
            p_null_msg = &last_non_null_msg->link;
        }
        while( *p_null_msg != NULL ) {
            msg_ptr = (*p_null_msg)->link;
            free( *p_null_msg );
            curr_group->end_msg_num--;
            *p_null_msg = msg_ptr;
        }
        ++group;
    }
}

int     main( int argc, char **argv )
//===================================
{
    char        cmd[128+1];

#ifndef __WATCOMC__
    _argc = argc;
    _argv = argv;
#else
    argc = argc;
    argv = argv;
#endif
    ProcArgs( getcmd( cmd ) );
    if( Initialize() != 0 ) {
        return( 1 );
    }
    printf( "Building Lists...\n" );
    BuildLists();
    printf( "Finding Phrases...\n" );
    FindPhrases();
    printf( "Sorting...\n" );
    SortByRef();
    printf( "ReNumbering...\n" );
    ReNumber();
    printf( "Dumping Msgs...\n" );
    DumpMsg();
    printf( "Dumping GroupTable...\n" );
    DumpGroupTable();
    printf( "Dumping ErrWords...\n" );
    DumpErrWord();
    Finalize();
    return( 0 );
}
