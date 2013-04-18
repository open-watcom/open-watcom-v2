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
* Description:  Stream processing for wmake.
*
****************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include "sopen.h"
#include "make.h"
#include "macros.h"
#include "mrcmsg.h"
#include "msg.h"
#include "mstream.h"
#include "msuffix.h"
#include "mmemory.h"
#include "mpreproc.h"

/*
 * This file implements something I'll call a "stream".  A stream consists
 * of a sequence of characters, and some special tokens.  The stream works
 * similar to a stack.  The caller stacks input into the stream, and GetCHR()
 * returns the next available character.
 *
 * Objects which the caller may stack consist of: files, null-terminated
 * strings, and single characters.  The corresponding routines are InsFile,
 * and InsOpenFile (for files); InsString (for strings); and UnGetCH for
 * characters.
 *
 * The caller calls GetCHR to receive the next character of input.  This
 * character is one of the standard character set (ie: ASCII) or one of the
 * special tokens.  The token of particular note is STRM_END, which is
 * similar to EOF in buffered input.  When STRM_END has been received, there
 * is no further input available in the stream.
 */

typedef enum {                  /* the different stream types           */
    SENT_FILE,
    SENT_STR,
    SENT_CHAR
} STYPE_T;


/* the most used items are at the top each union member */
typedef struct streamEntry {
    union {                         /* data required for each stream type   */
        struct {
            char        *cur;       /* next character to be read            */
            char        *max;       /* maximum position in buffer           */
            int         fh;         /* Posix i/o handle                     */
            char        *buf;       /* beginning of buffer                  */
            UINT16      line;       /* current line number                  */
            const char  *name;      /* file name                            */
            nest_level  nestLevel;  /* nest level of the file beginning     */
        } file;                     /* for SENT_FILE                        */

        struct {
            const char    *cur;     /* current position in string           */
            const char    *str;     /* beginning of string                  */
        } str;                      /* for SENT_STR                         */

        STRM_T s;                   /* for SENT_CHAR                        */
    } data;

    struct streamEntry *next;       /* linked list representation of stack  */

    BIT     type : 2;               /* must hold an STYPE_T                 */
    BIT     free : 1;               /* should we free resources?            */

} SENT;


/*
 * We keep two stacks of SENTs - one is the actual stack of SENTs in use.
 * (ie: open files, pending strings...) and the other is a stack of used
 * SENTs which we will reuse as needed.  pushSENT() and popSENT() take care
 * of the details.
 *
 * Important note: headSent is the current head at all times - we pop it
 * WHEN we are done with it - we don't pop it to use it.
 */
STATIC SENT *headSent;      /* stack of sents in use    */
STATIC SENT *freeSent;      /* stack of free sents      */

STATIC int  flagEOF;

STATIC SENT *getSENT( STYPE_T type )
/***********************************
 * get a free sent, and push it into stack.  Determines next char of input.
 */
{
    SENT    *d;

    if( freeSent != NULL ) {    /* check if any free SENTs lying around */
        d = freeSent;
        freeSent = freeSent->next;
    } else {
        d = MallocSafe( sizeof( *d ) );
    }
    d->type = type;
    d->next = headSent;
    headSent = d;
    return( d );
}


STATIC void popSENT( void )
/**************************
 * pop top SENT off stack.  If necessary, close or free the apropriate things.
 */
{
    SENT    *tmp;

    assert( headSent != NULL );

    tmp = headSent;

    switch( tmp->type ) {
    case SENT_FILE:
        if( tmp->free ) {
            close( tmp->data.file.fh );
            PrtMsg( DBG | INF | LOC | FINISHED_FILE, tmp->data.file.name );
        }
        FreeSafe( tmp->data.file.buf );
        FreeSafe( (void *)headSent->data.file.name );
        break;
    case SENT_STR:
        if( tmp->free ) {
            FreeSafe( (void *)headSent->data.str.str );
        }
        break;
    case SENT_CHAR:
        break;
    }

    headSent = tmp->next;       /* advance to next SENT */
    tmp->next = freeSent;       /* push onto free stack */
    freeSent = tmp;
}


STATIC void pushFH( SENT *sent, int fh )
/***************************************
 * given an open file handle, this routine allocates a buffer, and pushes a
 * SENT onto the stack
 */
{
    assert( sent != NULL );

    sent->data.file.fh = fh;
    sent->data.file.buf = MallocSafe( FILE_BUFFER_SIZE );
    sent->data.file.cur = sent->data.file.buf;
    sent->data.file.max = sent->data.file.buf;
    sent->data.file.line = 1;
    sent->data.file.nestLevel = -1;
}


STATIC BOOLEAN fillBuffer( void )
/********************************
 * Fill the top file buffer, and reset counters
 * returns: TRUE if buffer is not empty
 *          FALSE if buffer is empty (EOF)
 */
{
    int     max;
    SENT    *tmp;   /* just to make sure optimizer will registerize this */

    assert( headSent != NULL && headSent->type == SENT_FILE );

    tmp = headSent;

    if( tmp->data.file.nestLevel == -1 ) {
        tmp->data.file.nestLevel = GetNestLevel();
    }

    tmp->data.file.cur = tmp->data.file.buf;

    max = read( tmp->data.file.fh, tmp->data.file.buf, FILE_BUFFER_SIZE - 1 );
    if( max < 0 ) {     /* 31-jul-91 DJG */
        PrtMsg( ERR | READ_ERROR, tmp->data.file.name );
        max = 0;
    } else if( max > 0 && tmp->data.file.buf[max - 1] == '\r' ) {
        /* read one more character if it ends in \r (possibly CRLF) */
        int     max2;

        max2 = read( tmp->data.file.fh, &tmp->data.file.buf[max], 1 );
        if( max2 < 0 ) {     /* 13-sep-03 BEO */
            PrtMsg( ERR | READ_ERROR, tmp->data.file.name );
            max2 = 0;
        }
        max += max2;
    }
    tmp->data.file.max = tmp->data.file.buf + max;
    return( max > 0 );
}


#ifdef __WATCOMC__
#pragma on (check_stack);
#endif
RET_T InsFile( const char *name, BOOLEAN envsearch )
/**********************************************************
 * Open file named name, and push it into stream.  If can't find name, it
 * tries an implicit suffix search (possibly using the env variable PATH)
 */
{
    SENT    *tmp;
    int     fh;
    char    path[_MAX_PATH];

    assert( name != NULL );

    if( TrySufPath( path, name, NULL, envsearch ) == RET_SUCCESS ) {
        PrtMsg( DBG | INF |LOC | ENTERING_FILE, path );

        fh = sopen3( path, O_RDONLY | O_BINARY, SH_DENYWR );
        if( fh == -1 ) {
            return( RET_ERROR );
        }

        tmp = getSENT( SENT_FILE );
        tmp->free = TRUE;
        tmp->data.file.name = StrDupSafe( path );

        pushFH( tmp, fh );

        if( !Glob.overide ) {
            UnGetCH( EOL );
            InsString( path, FALSE );
            InsString( "$+$(__MAKEFILES__)$- ", FALSE );
            DefMacro( "__MAKEFILES__" );
        }

        return( RET_SUCCESS );
    }
    return( RET_ERROR );
}
#ifdef __WATCOMC__
#pragma off(check_stack);
#endif

void InsOpenFile( int fh )
/********************************
 * Push an already open file into the stream (ie: stdin)
 */
{
    SENT    *tmp;

#ifndef BOOTSTRAP
    assert( eof( fh ) == 0 );   /* not at eof, and proper fh */
#endif

    tmp = getSENT( SENT_FILE );
    tmp->free = FALSE;
    tmp->data.file.name = NULL;

    pushFH( tmp, fh );
}


void InsString( const char *str, BOOLEAN weFree )
/*******************************************************
 * Push a string into the stream.  Assumes that contents of string remain
 * static while it is in the stream.  (ie: it doesn't make its own copy).
 * If weFree then, it will be FreeSafe'd when it is done.
 */
{
    SENT    *tmp;

    assert( str != NULL );

    tmp = getSENT( SENT_STR );
    tmp->free = weFree;
    tmp->data.str.str = str;
    tmp->data.str.cur = str;
}


void UnGetCH( STRM_T s )
/******************************
 * Push back a single character
 */
{
    SENT    *tmp;

    assert( isascii( s ) || s == STRM_END || s == STRM_MAGIC || s == STRM_TMP_EOL || s == STRM_TMP_LEX_START);

    tmp = getSENT( SENT_CHAR );
    tmp->data.s = s;
}


STRM_T GetCHR( void )
/**************************
 * Get single next character of input
 */
{
    SENT    *head;  /* this is just here for optimizing purposes */
    STRM_T  result;

    flagEOF = 0;
    for( ;; ) {
        head = headSent;

        if( head == NULL ) {
            return( STRM_END ); /* the big mama ending! no more stream! */
        }

        switch( head->type ) {
        case SENT_FILE:
            /* GetFileLine() depends on the order of execution here */
            if( head->data.file.cur == head->data.file.max ) {
                if( !fillBuffer() ) {
                    if( head->data.file.nestLevel != GetNestLevel() ) {
                        PrtMsg( WRN | EOF_BEFORE_ENDIF, "endif" );
                    }
                    popSENT();
                    flagEOF = 1;
                    return( EOL );
                }
            }
            result = *(head->data.file.cur++);
            if( isbarf( result ) ) {
                /* ignore \r in \r\n */
                if( result == '\r' && head->data.file.cur[0] == EOL ) {
                    result = *(head->data.file.cur++);
                } else if( Glob.compat_nmake && result == 0x1a ) {
                    /* embedded ^Z terminates stream in MS mode */
                    result = EOL;
                    popSENT();
                    flagEOF = 1;
                } else {
                    PrtMsg( FTL | LOC | BARF_CHARACTER, result );
                }
            }
            if( result == '\f' ) {
                result = EOL;
            }
            if( result == EOL ) {
                head->data.file.line++;
            }
            return( result );
        case SENT_STR:
            result = *(head->data.str.cur++);
            if( result == NULLCHAR ) {
                popSENT();
                continue;   /* try again */
            }
            return( result );
        case SENT_CHAR:
            result = head->data.s;
            popSENT();
            return( result );
        }
        assert( FALSE );    /* should never get here */
    }
}

#ifdef USE_SCARCE
STATIC RET_T streamScarce( void )
/*******************************/
{
    SENT    *cur;

    if( freeSent == NULL ) {
        return( RET_ERROR );
    }

    while( freeSent != NULL ) {
        cur = freeSent;
        freeSent = freeSent->next;
        FreeSafe( cur );
    }

    return( RET_SUCCESS );
}
#endif

void StreamFini( void )
/****************************/
{
    while( headSent != NULL ) {
        popSENT();
    }
}


void StreamInit( void )
/****************************/
{
    int     count;
    SENT    *sent;

    /* preallocate storage to speed things up, and reduce fragmentation */

    freeSent = NULL;
    for( count = 0; count < STREAM_ALLOC_SENT; count++ ) {
        sent = MallocSafe( sizeof( *sent ) );
        sent->next = freeSent;
        freeSent = sent;
    }

#ifdef USE_SCARCE
    IfMemScarce( streamScarce );
#endif
}


RET_T GetFileLine( const char **pname, UINT16 *pline )
/************************************************************
 * get filename, and line number of file closest to top of stack
 * FALSE - no files in stack; TRUE - returned data from top file
 */
{
    SENT    *cur;

    cur = headSent;
    while( cur != NULL ) {
        if( cur->type == SENT_FILE ) {
            break;
        }
        cur = cur->next;
    }

    if( cur == NULL ) {
        return( RET_ERROR );
    }

    /*
     * Because we do a line++ when we return a {nl}, we have to check if
     * the last character returned was a {nl}.  We check the last character
     * if any characters have been read - it is just at cur[-1].  The only
     * time that cur > buf == FALSE is when nothing has been read.  (Check
     * the code for reading - even after filling the buffer this doesn't
     * evaluate improperly).
     */
    *pline = cur->data.file.line;
    if( cur->data.file.cur > cur->data.file.buf &&
        cur->data.file.cur[-1] == EOL ) {
        --(*pline);
    }

    *pname = cur->data.file.name;

    return( RET_SUCCESS );
}


int IsStreamEOF( void )
{
    return( flagEOF );
}


#ifdef DEVELOPMENT      /* code to dump the stack of SENTs */
void dispSENT( void )
/**************************/
{
    char    buf[256];
    size_t  pos;
    SENT    *cur;

    if( headSent ) {
        PrtMsg( INF | PRNTSTR, "\n[stream contents:]" );
        cur = headSent;
        while( cur ) {
            pos = FmtStr( buf, "[type %d, ", cur->type );
            switch( cur->type ) {
            case SENT_FILE:
                if( cur->data.file.cur == cur->data.file.max ) {
                    pos += FmtStr( &buf[pos], "fh %d, buffer empty, line %d",
                        cur->data.file.fh, cur->data.file.line );
                } else {
                    pos += FmtStr( &buf[pos],
                        "fh %d, in buf %d, next 0x%x, line %d",
                        cur->data.file.fh, cur->data.file.max - cur->data.file.cur,
                        *(cur->data.file.cur), cur->data.file.line );
                }
                if( cur->data.file.name ) {
                    pos += FmtStr( &buf[pos], ", name %s", cur->data.file.name );
                }
                pos += FmtStr( &buf[pos], "]" );
                PrtMsg( INF | PRNTSTR, buf );
                break;
            case SENT_STR:
                pos += FmtStr( &buf[pos], "(" );
                PrtMsg( INF | NEOL | PRNTSTR, buf );
                PrtMsg( INF | NEOL | PRNTSTR, cur->data.str.cur );
                PrtMsg( INF | PRNTSTR, ")]" );
                break;
            case SENT_CHAR:
                pos += FmtStr( &buf[pos], "character 0x%x]", cur->data.ch );
                PrtMsg( INF| PRNTSTR, buf );
                break;
            }
            cur = cur->next;
        }
    } else {
        PrtMsg( INF | PRNTSTR, "[stream empty]" );
    }
}
#endif
