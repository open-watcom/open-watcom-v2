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


#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "context.h"
#include "deffile.h"
#include "error.h"
#include "memory.h"
#include "message.h"


#define SET_ERROR               error = TRUE
#define CLEAR_GOT_FLAGS         exportsGotEntryName = FALSE;    \
                                exportsGotInternalName = FALSE; \
                                exportsGotOrdinal = FALSE;      \
                                exportsGotPrivate = FALSE;      \
                                exportsGotResident = FALSE;     \
                                heapGotReserve = FALSE;         \
                                heapGotCommit = FALSE;          \
                                stackGotReserve = FALSE;        \
                                stackGotCommit = FALSE;         \
                                stubGotFile = FALSE;            \
                                versionGotMajor = FALSE;        \
                                versionGotMinor = FALSE;


static int next_token( int state, bool *newCmd, bool *newLine, bool fileCharsOk, bool atCharsOk );
static char *   get_word( void );
static void     strip_quotes( char *str );
static bool     eat_comments( bool *newLine );
static char *   string_convert( const char *str );


enum {
    STATE_CLEAR,                        /* general states */
    STATE_EOF,
    STATE_BASE,                         /* keyword states */
    STATE_DESCRIPTION,
    STATE_EXETYPE,
    STATE_EXPORT,
    STATE_EXPORTS,
    STATE_HEAPSIZE,
    STATE_LIBRARY,
    STATE_NAME,
    STATE_SECTIONS,
    STATE_SEGMENTS,
    STATE_STACKSIZE,
    STATE_STUB,
    STATE_VERSION,
    STATE_PRIVATE,
    STATE_PROTMODE,                     /* unsupported states */
    STATE_CODE,
    STATE_DATA,
    STATE_NONAME,
    STATE_RESIDENTNAME,
    STATE_AT,                           /* separator states, they must be together !!! */
    STATE_COMMA,
    STATE_EQUAL,
    STATE_PERIOD,
    STATE_STRING                        /* option value states */
};


static int          breakStates[] = {   /* break a command on these */
    STATE_CLEAR,
    STATE_EOF,
    STATE_DESCRIPTION,
    STATE_EXETYPE,
    STATE_EXPORT,
    STATE_EXPORTS,
    STATE_HEAPSIZE,
    STATE_LIBRARY,
    STATE_NAME,
    STATE_SECTIONS,
    STATE_SEGMENTS,
    STATE_STACKSIZE,
    STATE_STUB,
    STATE_PROTMODE,
    STATE_CODE,
    STATE_DATA,
    STATE_NONAME,
    -1
};

static char *       curToken = NULL;


#ifdef __TARGET_AXP__
/*
 *  Strip leading _ and @nn from stdcall names when alpha
 */
void FuzzyMassageAXPStdcall( char *name ) {
    char *first;
    char *last;
    first = name;
    last = name + strlen( name ) - 1;
    if( *first == '\'' ) {
        first++;
        name++;
    }
    if( *last == '\'' ) last--;
    if( *first == '_' && isdigit( *last ) ) {
        first++;
        last--;
        while( isdigit( *last ) ) {
            last--;
        }
        if( *last == '@' ) {
            *last++ = '\'';
            *last = '\0';
            while( *first ) {
                *name++ = *first++;
            }
            *name = '\0';
        }
    }
}
#endif


/*
 * Add another string to a StringList.
 */
static void add_string( StringList **p, char *str )
/*************************************************/
{
    StringList *        buf;
    StringList *        curElem;

    /*** Make a new list item ***/
    buf = AllocMem( sizeof(StringList) );
    buf->str = DupStrMem( str );
    buf->next = NULL;

    /*** Put it at the end of the list ***/
    if( *p == NULL ) {
        *p = buf;
    } else {
        curElem = *p;
        while( curElem->next != NULL )  curElem = curElem->next;
        curElem->next = buf;
    }
}


/*
 * Parse a .def file.  Returns NULL on error, or a DefInfo* on success.
 */
#ifdef __TARGET_AXP__
    DefInfo *ParseDefFile( const char *filename, int dofuzzylink )
#else
    DefInfo *ParseDefFile( const char *filename )
#endif
/****************************************************************/
{
    char *              newfilename = DupStrMem( filename );
    DefInfo *           info;
    bool                alive = TRUE;
    bool                error = FALSE;
    int                 state = STATE_CLEAR;
    bool                mergeNow = FALSE;
    bool                mergeNow2;
    int                 mergeType = STATE_CLEAR;
    bool                foundNewLine;
    char *              exportsEntryName = NULL;
    bool                exportsGotEntryName;
    char *              exportsInternalName = NULL;
    bool                exportsGotInternalName;
    char *              exportsOrdinal = NULL;
    bool                exportsGotOrdinal;
    bool                exportsGotPrivate;
    bool                exportsGotResident;
    char *              stackReserve = NULL;
    bool                stackGotReserve;
    char *              stackCommit = NULL;
    bool                stackGotCommit;
    bool                stubGotFile;
    char *              heapReserve = NULL;
    bool                heapGotReserve;
    char *              heapCommit = NULL;
    bool                heapGotCommit;
    char *              versionMajor = NULL;
    bool                versionGotMajor;
    char *              versionMinor = NULL;
    bool                versionGotMinor;
    size_t              len;
    char *              str;

#ifdef __TARGET_AXP__
    dofuzzylink = dofuzzylink;
#endif
    CLEAR_GOT_FLAGS;            /* initialize locals */
    /*** Initialize ***/
    strip_quotes( newfilename );
    if( OpenFileContext( newfilename ) ) {
        FreeMem( newfilename );
        return( NULL );
    }
    info = AllocMem( sizeof( DefInfo ) );
    memset( info, 0, sizeof( DefInfo ) );

    /*** Process the commands, one by one ***/
    while( alive ) {
        if( error ) {
            Warning( "Error near '%s' in %s -- aborting .def processing",
                     curToken, newfilename );
            FreeMem( info );
            FreeMem( newfilename );
            return( NULL );
        }
        switch( state ) {
          case STATE_CLEAR:
            CLEAR_GOT_FLAGS;            /* initialize locals */
            mergeType = STATE_CLEAR;
            mergeNow = FALSE;
            state = next_token( STATE_CLEAR, NULL, NULL, FALSE, !exportsGotEntryName ); /* get next command */
            break;
          case STATE_DESCRIPTION:       /* DESCRIPTION "text" */
            state = next_token( STATE_DESCRIPTION, NULL, NULL, FALSE, FALSE );
            switch( state ) {
              case STATE_STRING:
                str = string_convert( curToken );
                if( *str == '"' ) { // change " to '
                    *str ='\'';
                    str[strlen(str)-1] = '\'';
                }
                add_string( &info->description, str );
                state = STATE_CLEAR;
                break;
              default:
                SET_ERROR;
                break;
            }
            break;
          case STATE_EXPORTS:
          /* EXPORTS entryname[=internalname] [@ordinal[ NONAME ]  [DATA] [ PRIVATE ] [RESIDENTNAME] */
            state = next_token( STATE_EXPORTS, &mergeNow, &foundNewLine, FALSE, !exportsGotEntryName );
            if( mergeNow ) {
                if( !exportsGotEntryName ) {
                    mergeNow = FALSE;
                    break;
                }
                while( !foundNewLine ) {
                        if (next_token( STATE_EXPORTS, NULL, &foundNewLine, FALSE, FALSE ) == STATE_EOF) break;
                }
                state = STATE_EXPORTS;
                break;
            }
            switch( state ) {
              case STATE_STRING:
                if( exportsGotEntryName ) {
                    SET_ERROR;
                } else {
                    exportsEntryName = DupQuoteStrMem( curToken, '\'' );
#ifdef __TARGET_AXP__
                    FuzzyMassageAXPStdcall( exportsEntryName );
#endif
                    exportsGotEntryName = TRUE;
                    mergeType = STATE_EXPORTS;
                    if( foundNewLine ) {
                        mergeNow = TRUE;
                    }
                }
                state = STATE_EXPORTS;
                break;
              case STATE_AT:
                if( !exportsGotEntryName ) {
                    SET_ERROR;
                } else {
                    state = next_token( STATE_EXPORTS, &mergeNow, &foundNewLine, FALSE, !exportsGotEntryName );
                    if( mergeNow )  break;
                    switch( state ) {
                      case STATE_STRING:
                        exportsOrdinal = DupStrMem( curToken );
                        exportsGotOrdinal = TRUE;
                        state = STATE_EXPORTS;
                        if( foundNewLine ) {
                            mergeNow = TRUE;
                        }
                        break;
                      case STATE_EOF:
                        break;
                      default:
                        SET_ERROR;
                        break;
                    }
                }
                break;
              case STATE_EQUAL:
                if( !exportsGotEntryName || exportsGotOrdinal ) {
                    SET_ERROR;
                } else {
                    state = next_token( STATE_EXPORTS, &mergeNow, &foundNewLine, FALSE, !exportsGotInternalName );
                    if( mergeNow )  break;
                    switch( state ) {
                      case STATE_STRING:
                        exportsInternalName = DupQuoteStrMem( curToken, '\'' );
#ifdef __TARGET_AXP__
                        FuzzyMassageAXPStdcall( exportsInternalName );
#endif
                        exportsGotInternalName = TRUE;
                        state = STATE_EXPORTS;
                        if( foundNewLine ) {
                            mergeNow = TRUE;
                        }
                        break;
                      case STATE_EOF:
                        break;
                      default:
                        SET_ERROR;
                        break;
                    }
                }
                break;
              case STATE_PRIVATE:
                if( !exportsGotEntryName ) {
                    SET_ERROR;
                } else {
                    exportsGotPrivate = TRUE;
                    mergeNow = TRUE;
                }
                state = STATE_EXPORTS;
                break;
              case STATE_RESIDENTNAME:
                if( !exportsGotEntryName ) {
                    SET_ERROR;
                } else {
                    exportsGotResident = TRUE;
                    mergeNow = TRUE;
                }
                state = STATE_EXPORTS;
                break;
              case STATE_COMMA:
                if( !exportsGotEntryName ) {
                    SET_ERROR;
                }
                state = STATE_EXPORTS;
                break;
              default:
                break;
            }
            break;
          case STATE_LIBRARY:           /* LIBRARY [library][BASE=address] */
            state = next_token( STATE_LIBRARY, &mergeNow, NULL, TRUE, FALSE );
            if( mergeNow )  break;
            switch( state ) {
              case STATE_STRING:
                strip_quotes( curToken );
                if( info->internalDllName != NULL )  FreeMem( info->internalDllName );
                info->internalDllName = DupStrMem( curToken );
                state = STATE_LIBRARY;
                break;
              case STATE_BASE:
                state = next_token( STATE_LIBRARY, NULL, NULL, FALSE, FALSE );
                switch( state ) {
                  case STATE_EQUAL:
                    state = next_token( STATE_LIBRARY, NULL, NULL, FALSE, FALSE );
                    switch( state ) {
                      case STATE_STRING:
                        if( info->baseAddr != NULL )  FreeMem( info->baseAddr );
                        info->baseAddr = DupStrMem( curToken );
                        break;
                      default:
                        break;
                    }
                  default:
                    break;
                }
                state = STATE_LIBRARY;
                break;
              default:
                break;
            }
            break;
          case STATE_NAME:      /* NAME [application][BASE=address] */
            state = next_token( STATE_NAME, &mergeNow, NULL, TRUE, FALSE );
            if( mergeNow )  break;
            switch( state ) {
              case STATE_STRING:
                strip_quotes( curToken );
                if( info->name != NULL )  FreeMem( info->name );
                info->name = DupStrMem( curToken );
                state = STATE_NAME;
                break;
              case STATE_BASE:
                state = next_token( STATE_NAME, NULL, NULL, FALSE, FALSE );
                switch( state ) {
                  case STATE_EQUAL:
                    state = next_token( STATE_NAME, NULL, NULL, FALSE, FALSE );
                    switch( state ) {
                      case STATE_STRING:
                        if( info->baseAddr != NULL )  FreeMem( info->baseAddr );
                        info->baseAddr = DupStrMem( curToken );
                        break;
                      default:
                        break;
                    }
                  default:
                    break;
                }
                state = STATE_NAME;
                break;
              default:
                break;
            }
            break;
          case STATE_SECTIONS:  /* SECTIONS definitions */
            UnsupportedOptsMessage( "SECTIONS" );
            state=STATE_STRING;
            mergeNow2 = FALSE;
            mergeNow = FALSE;
            for( ; ; ) {
                state=next_token( STATE_STRING, &mergeNow, NULL, FALSE, FALSE );
                if ( (state < STATE_AT) && mergeNow2 ) break;
                mergeNow2 = mergeNow;
                mergeNow = FALSE;
            }
            mergeNow = FALSE;
            break;
          case STATE_STACKSIZE: /* STACKSIZE reserve[,commit] */
            state = next_token( STATE_STACKSIZE, &mergeNow, NULL, FALSE, FALSE );
            if( mergeNow && !stackGotReserve )  SET_ERROR;
            switch( state ) {
              case STATE_STRING:
                if( stackGotCommit ) {
                    SET_ERROR;
                } else if (stackGotReserve){
                    stackCommit = DupStrMem( curToken );
                    stackGotCommit = TRUE;
                    state = STATE_CLEAR;
                    break;
                } else {
                    stackReserve = DupStrMem( curToken );
                    stackGotReserve = TRUE;
                    mergeType = STATE_STACKSIZE;
                }
                state = STATE_STACKSIZE;
                break;
              case STATE_COMMA:
                state = next_token( STATE_STACKSIZE, NULL, NULL, FALSE, FALSE );
                switch( state ) {
                  case STATE_STRING:
                    stackCommit = DupStrMem( curToken );
                    stackGotCommit = TRUE;
                    state = STATE_CLEAR;
                    break;
                  default:
                    SET_ERROR;
                    break;
                }
                break;
              case STATE_EOF:
                break;
              default:
                if( !stackGotReserve )  SET_ERROR;
                break;
            }
            break;
          case STATE_STUB:      /* STUB "newfilename" */
            if (stubGotFile) {
                FreeMem(info->stub);
                info->stub=NULL;
            }
            state = next_token( STATE_STUB, NULL, NULL, TRUE, FALSE );
            switch( state ) {
              case STATE_STRING:
                stubGotFile = TRUE;
                str = string_convert( curToken );
                info->stub = DupStrMem(str);
                state = STATE_CLEAR;
                break;
              default:
                SET_ERROR;
                break;
            }
            break;
          case STATE_HEAPSIZE:  /* HEAPSIZE reserve[,commit] */
            state = next_token( STATE_HEAPSIZE, &mergeNow, NULL, FALSE, FALSE );
            if( mergeNow && !heapGotReserve )  SET_ERROR;
            switch( state ) {
              case STATE_STRING:
                if( heapGotCommit ) {
                    SET_ERROR;
                } else if (heapGotReserve){
                    heapCommit = DupStrMem( curToken );
                    heapGotCommit = TRUE;
                    state = STATE_CLEAR;
                    break;
                } else {
                    heapReserve = DupStrMem( curToken );
                    heapGotReserve = TRUE;
                    mergeType = STATE_HEAPSIZE;
                }
                state = STATE_HEAPSIZE;
                break;
              case STATE_COMMA:
                state = next_token( STATE_HEAPSIZE, NULL, NULL, FALSE, FALSE );
                switch( state ) {
                  case STATE_STRING:
                    heapCommit = DupStrMem( curToken );
                    heapGotCommit = TRUE;
                    state = STATE_CLEAR;
                    break;
                  default:
                    SET_ERROR;
                    break;
                }
                break;
              case STATE_EOF:
                break;
              default:
                if( !heapGotReserve )  SET_ERROR;
                break;
            }
            break;
          case STATE_VERSION:   /* VERSION major[.minor] */
            state = next_token( STATE_VERSION, &mergeNow, NULL, FALSE, FALSE );
            if( mergeNow && !versionGotMajor )  SET_ERROR;
            switch( state ) {
              case STATE_STRING:
                if( versionGotMajor ) {
                    SET_ERROR;
                } else {
                    versionMajor = DupStrMem( curToken );
                    versionGotMajor = TRUE;
                    mergeType = STATE_VERSION;
                }
                state = STATE_VERSION;
                break;
              case STATE_PERIOD:
                if( !versionGotMajor ) {
                    SET_ERROR;
                } else {
                    state = next_token( STATE_VERSION, NULL, NULL, FALSE, FALSE );
                    switch( state ) {
                      case STATE_STRING:
                        versionMinor = DupStrMem( curToken );
                        versionGotMinor = TRUE;
                        state = STATE_CLEAR;
                        break;
                      default:
                        SET_ERROR;
                        break;
                    }
                }
                break;
              case STATE_EOF:
                break;
              default:
                SET_ERROR;
                break;
            }
            break;
          case STATE_PROTMODE:
            /*** ignore this token ***/
            UnsupportedOptsMessage( "PROTMODE" );
            state = STATE_CLEAR;
            break;
          case STATE_CODE:
            /*** ignore this token and the rest of the line ***/
            UnsupportedOptsMessage( "CODE" );
            next_token( STATE_CODE, NULL, &foundNewLine, FALSE, FALSE );
            while( !foundNewLine ) {
                next_token( STATE_CODE, NULL, &foundNewLine, FALSE, FALSE );
            }
            state = STATE_CLEAR;
            break;
          case STATE_EXETYPE:
            /*** ignore this token and the rest of the line ***/
            UnsupportedOptsMessage( "EXETYPE" );
            next_token( STATE_EXETYPE, NULL, &foundNewLine, FALSE, FALSE );
            while( !foundNewLine ) {
                next_token( STATE_EXETYPE, NULL, &foundNewLine, FALSE, FALSE );
            }
            state = STATE_CLEAR;
            break;
          case STATE_DATA:
            /*** ignore this token and the rest of the line ***/
            UnsupportedOptsMessage( "DATA" );
            next_token( STATE_DATA, NULL, &foundNewLine, FALSE, FALSE );
            while( !foundNewLine ) {
                next_token( STATE_DATA, NULL, &foundNewLine, FALSE, FALSE );
            }
            state = STATE_CLEAR;
            break;
          case STATE_EOF:
            alive = FALSE;
            break;
          default:
            SET_ERROR;
        }

        /*** Merge stuff together if necessary ***/
        if( mergeNow || ( state == STATE_CLEAR || state == STATE_EOF ) ) {
            switch( mergeType ) {
              case STATE_EXPORTS:
                len = strlen( exportsEntryName );
                if( exportsGotOrdinal ) {
                    len++;                      /* for '.' */
                    len += strlen( exportsOrdinal );
                    if( exportsGotResident ) {
                        len += 9;                   /* for ' RESIDENT' */
                    }
                }
                if( exportsGotInternalName ) {
                    len++;                      /* for '=' */
                    len += strlen( exportsInternalName );
                }
                if( exportsGotPrivate ) {
                    len += 8;                   /* for ' PRIVATE' */
                }
                len++;                  /* for null byte */
                str = AllocMem( len );
                strcpy( str, exportsEntryName );
                if( exportsGotOrdinal ) {
                    strcat( str, "." );
                    strcat( str, exportsOrdinal );
                }
                if( exportsGotInternalName ) {
                    strcat( str, "=" );
                    strcat( str, exportsInternalName );
                }
                if( exportsGotPrivate ) {
                    strcat( str, " PRIVATE" );
                }
                if( exportsGotResident && exportsGotOrdinal ) {
                    strcat( str, " RESIDENT" );
                }
                add_string( &info->exports, str );
                FreeMem( str );
                FreeMem( exportsEntryName );
                if( exportsGotOrdinal ) {
                    FreeMem( exportsOrdinal );
                }
                if( exportsGotInternalName ) {
                    FreeMem( exportsInternalName );
                }
                break;
              case STATE_STACKSIZE:
                state = STATE_CLEAR;
                len = strlen( stackReserve );
                if( stackGotCommit ) {
                    len++;                      /* for ',' */
                    len += strlen( stackCommit );
                }
                len++;                  /* for null byte */
                str = AllocMem( len );
                strcpy( str, stackReserve );
                if( stackGotCommit ) {
                    strcat( str, "," );
                    strcat( str, stackCommit );
                }
                info->stacksize = DupStrMem( str );
                FreeMem( stackReserve );
                if( stackGotCommit ) {
                    FreeMem( stackCommit );
                }
                break;
              case STATE_HEAPSIZE:
                state = STATE_CLEAR;
                len = strlen( heapReserve );
                if( heapGotCommit ) {
                    len++;                      /* for ',' */
                    len += strlen( heapCommit );
                }
                len++;                  /* for null byte */
                str = AllocMem( len );
                strcpy( str, heapReserve );
                if( heapGotCommit ) {
                    strcat( str, "," );
                    strcat( str, heapCommit );
                }
                info->heapsize = DupStrMem( str );
                FreeMem( heapReserve );
                if( heapGotCommit ) {
                    FreeMem( heapCommit );
                }
                break;
              case STATE_VERSION:
                state = STATE_CLEAR;
                len = strlen( versionMajor );
                if( versionGotMinor ) {
                    len++;                      /* for '.' */
                    len += strlen( versionMinor );
                }
                len++;                  /* for null byte */
                str = AllocMem( len );
                strcpy( str, versionMajor );
                if( versionGotMinor ) {
                    strcat( str, "." );
                    strcat( str, versionMinor );
                }
                info->version = DupStrMem( str );
                FreeMem( versionMajor );
                if( versionGotMinor ) {
                    FreeMem( versionMinor );
                }
                break;
              default:
                break;
            }
            mergeType = STATE_CLEAR;
            mergeNow = FALSE;
            CLEAR_GOT_FLAGS;
        }
    }

    FreeMem( newfilename );
    return( info );
}


/*
 * Get the next token, returning the new state.
 */
static int next_token( int state, bool *newCmd, bool *newLine, bool fileCharsOk, bool atCharsOk )
/**********************************************************************************************/
{
    char                ch = '\0';
    long                start;
    int                 len = 0;
    bool                alive = TRUE;
    bool                quoteUsed = FALSE;
    char                usingQuote = '\0';
    int                 count;
    int                 retcode;
    bool                gotNewLine = FALSE;
    bool                gotEof = FALSE;
    bool                goBackOne = FALSE;
    char *              buf;

    /*** Read up until the end of the token ***/
    eat_comments( &gotNewLine );
    if( gotNewLine ) {
        if( newLine != NULL ) {
            *newLine = gotNewLine;
        }
        if ( newCmd != NULL ) {
            *newCmd = TRUE;
        }
        return state;
    }
    start = GetPosContext();
    while( alive ) {
        ch = GetCharContext();
        switch( ch ) {
          case ':':
          case '.':
            if( fileCharsOk )  break;
            /* note possible fall through */
          case '@':
            if( atCharsOk )  break;
            /* note possible fall through */
          case '=':
          case ',':
          case ' ':
          case '\n':
          case '\r':
          case '\t':
            if( !quoteUsed ) {
                alive = FALSE;          /* break a token on any of these */
                if( len != 0 ) {
                    goBackOne = TRUE;   /* save breaking char for later */
                }
                break;
            }
            break;
          case '"':
            if( quoteUsed && usingQuote == '"' ) {
                usingQuote = '\0';
                alive = FALSE;
            } else if( !quoteUsed ) {
                usingQuote = '"';
                quoteUsed = TRUE;
            }
            break;
          case '\'':
            if( quoteUsed  &&  usingQuote == '\'' ) {
                usingQuote = '\0';
                alive = FALSE;
            } else if( !quoteUsed ) {
                usingQuote = '\'';
                quoteUsed = TRUE;
            }
            break;
          case '\0':
            gotEof = TRUE;
            alive = FALSE;
            break;
          default:
            break;
        }
        len++;
    }
    if( goBackOne ) {
        len--;
        UngetCharContext();
        ch = GetCharContext();
        UngetCharContext();
    }
    if( ch == '\n' || ch == '\r' ) {
        gotNewLine = TRUE;
    }


    /*** Copy it to a buffer ***/
    buf = AllocMem( len + 1 );
    SetPosContext( start );
    for( count=0; count<len; count++ ) {
        buf[count] = GetCharContext();
    }
    buf[count] = '\0';
    if( strlen( buf ) > 0 ) {
        if( curToken != NULL )  FreeMem( curToken );
        curToken = buf;
    }

    /*** Determine what the token is ***/
    if( gotEof ) {
        retcode = STATE_EOF;
    } else if( !stricmp( curToken, "BASE" ) ) {
        retcode = STATE_BASE;
    } else if( !stricmp( curToken, "DESCRIPTION" ) ) {
        retcode = STATE_DESCRIPTION;
    } else if( !stricmp( curToken, "EXETYPE" ) ) {
        retcode = STATE_EXETYPE;
    } else if( !stricmp( curToken, "EXPORT" ) ) {   /* synonym for EXPORTS */
        retcode = STATE_EXPORTS;
    } else if( !stricmp( curToken, "EXPORTS" ) ) {
        retcode = STATE_EXPORTS;
    } else if( !stricmp( curToken, "HEAPSIZE" ) ) {
        retcode = STATE_HEAPSIZE;
    } else if( !stricmp( curToken, "LIBRARY" ) ) {
        retcode = STATE_LIBRARY;
    } else if( !stricmp( curToken, "NAME" ) ) {
        retcode = STATE_NAME;
    } else if( !stricmp( curToken, "SECTIONS" ) ) {
        retcode = STATE_SECTIONS;
    } else if( !stricmp( curToken, "SEGMENTS" ) ) { /* synonym for SECTIONS */
        retcode = STATE_SECTIONS;
    } else if( !stricmp( curToken, "STACKSIZE" ) ) {
        retcode = STATE_STACKSIZE;
    } else if( !stricmp( curToken, "STUB" ) ) {
        retcode = STATE_STUB;
    } else if( !stricmp( curToken, "VERSION" ) ) {
        retcode = STATE_VERSION;
    } else if( !stricmp( curToken, "PROTMODE" ) ) {
        retcode = STATE_PROTMODE;
    } else if( !stricmp( curToken, "CODE" ) ) {
        retcode = STATE_CODE;
    } else if( !stricmp( curToken, "DATA" ) ) {
        retcode = STATE_DATA;
    } else if( !stricmp( curToken, "NONAME" ) ) {
        retcode = STATE_NONAME;
    } else if( !stricmp( curToken, "RESIDENTNAME" ) ) {
        retcode = STATE_RESIDENTNAME;
    } else if( !stricmp( curToken, "PRIVATE" ) ) {
        retcode = STATE_PRIVATE;
    } else if( !stricmp( curToken, "@" ) ) {
        retcode = STATE_AT;
    } else if( !stricmp( curToken, "," ) ) {
        retcode = STATE_COMMA;
    } else if( !stricmp( curToken, "=" ) ) {
        retcode = STATE_EQUAL;
    } else if( !stricmp( curToken, "." ) ) {
        retcode = STATE_PERIOD;
    } else {
        retcode = STATE_STRING;
    }

    /*** If it's the start of a new command, set *newCmd ***/
    if( newCmd != NULL ) {
        *newCmd = FALSE;
        for( count=0; ; count++ ) {
            if( breakStates[count] == -1 )  break;
            if( retcode == breakStates[count] ) {
                *newCmd = TRUE;
                break;
            }
        }
    }
    /*** If we saw a NewLine as the end of the token, set *newLine ***/
    if( newLine != NULL ) {
        *newLine = gotNewLine;
    }

    return( retcode );
}


/*
 * Strip leading and trailing whitespace and double quotes from string.
 */
static void strip_quotes( char *str )
/***********************************/
{
    size_t              count;
    size_t              diff;
    size_t              len;
    char                *p;

    len = strlen( str );
    /*** Strip trailing quotes and whitespace ***/
    p = str + len - 1;
    while( *p == '"'  ||  isspace( *p ) )  *(p--) = '\0';

    /*** Strip leading quotes and whitespace ***/
    p = str;
    while( *p == '"'  ||  isspace( *p ) )  p++;
    if( p != str ) {
        diff = p - str;
        for( count=0; count + diff <= len; count++ ) {
            str[count] = str[count + diff];
        }
    }
}


/*
 * Skip all whitespace characters, such that the next read will retrieve the
 * first non-whitespace character.
 */
static void ScanWhitespace( bool *newLine )
/****************************************/
{
    char    ch;

    do {
        ch = GetCharContext();
    } while( isspace( ch )  &&  ch != '\0' && ch != '\n' );
    if( ch != '\0' && ch != '\n' )
        UngetCharContext();
    if( ch == '\n' ) {
        *newLine = FALSE;
    }
}


/*
 * Skip comment(s) and whitespace.  Returns non-zero if not yet at EOF.
 */
static bool eat_comments( bool *newLine )
/***************************************/
{
    char    ch;
    bool    alive = TRUE;


    *newLine = FALSE;
    do {
        ScanWhitespace( newLine );
        ch = GetCharContext();
        if( ch == ';' ) {
            do {                /* ignore everything up to end of line */
                ch = GetCharContext();
                if( ch == '\n' ) {
                    *newLine = TRUE;
                }
            } while( ch != '\0'  &&  ch != '\n' );
        } else {
            alive = FALSE;
        }
        if( ch != '\0' ) {
            UngetCharContext();
        }
    } while( alive );
    ScanWhitespace( newLine );

    return( ch == '\0' ? FALSE : TRUE );
}


/*
 * Convert strings like "foo ' bar" to "foo \' bar" form.
 */
static char *string_convert( const char *str )
/********************************************/
{
    const char *        p = str;
    char *              out;
    char *              outStart;
    bool                gotQuote = FALSE;
    char                quoteType = '\0';

    outStart = AllocMem( 2 * strlen( str ) + 1 );
    out = outStart;
    while( *p != '\0' ) {
        if( *p == '"'  ||  *p == '\'' ) {
            if( !gotQuote ) {
                gotQuote = TRUE;
                quoteType = *p;
                *out++ = *p++;
            } else if( quoteType != *p  &&  *p == '\'' ) {
                *out++ = '\\';
                *out++ = '\'';
                p++;
            } else {
                *out++ = *p++;
            }
        } else {
            *out++ = *p++;
        }
    }
    *out = '\0';
    return( outStart );
}


/*
 * Destroy a StringList.
 */
static void FreeStringList( StringList **p )
/******************************************/
{
    StringList *        s;

    while( *p != NULL ) {
        s = *p;
        *p = s->next;
        FreeMem( s );
    }
}


/*
 * Destroy a DefInfo structure.
 */
void FreeDefInfo( DefInfo *p )
/****************************/
{
    FreeMem( p->name );
    FreeMem( p->internalDllName );
    FreeMem( p->baseAddr );
    FreeMem( p->stub );
    FreeStringList( &(p->description) );
    FreeStringList( &(p->exports) );
    FreeMem( p->stacksize );
    FreeMem( p->heapsize );
    FreeMem( p->version );
    FreeMem( p );
}
