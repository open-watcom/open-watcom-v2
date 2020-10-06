/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Generate C files from messages stored in GML source text.
*
****************************************************************************/


#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#if defined( __UNIX__ ) || defined( __WATCOMC__ )
    #include <sys/types.h>
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#include "bool.h"
#include "wio.h"
#include "watcom.h"
#include "lsspec.h"
#include "encodlng.h"
#include "cvttable.h"

#include "clibext.h"


#define ALL_TAGS \
def_tag( msggrp ) \
def_tag( emsggrp ) \
def_tag( msggrptxt ) \
def_tag( msgjgrptxt ) \
def_tag( msggrpnum ) \
def_tag( msggrpstr ) \
def_tag( msgsym ) \
def_tag( msgtxt ) \
def_tag( msgjtxt ) \
def_tag( ansi ) \
def_tag( ansierr ) \
def_tag( ansiwarn ) \
def_tag( warning ) \
def_tag( info ) \
def_tag( ansicomp ) \
def_tag( errbad ) \
def_tag( eerrbad ) \
def_tag( errgood ) \
def_tag( eerrgood ) \
def_tag( errbreak ) \
def_tag( style ) \
def_tag( jck ) \

#define NEXT_ARG_CHECK() \
        --argc1; ++argv1; if( argc1 < NUM_FILES ) { return( 1 ); }

typedef enum {
#define def_tag( e ) TAG_##e,
    ALL_TAGS
#undef def_tag
    TAG_MAX
} tag_id;

static const char *tagNames[] = {
#define def_tag( e ) #e "." ,
    ALL_TAGS
#undef def_tag
    NULL
};

#define ALL_MSG_TYPES \
def_msg_type( ERROR, "ERR_" ) \
def_msg_type( WARNING, "WARN_" ) \
def_msg_type( INFO, "INF_" ) \
def_msg_type( ANSI, "ANSI_" ) \
def_msg_type( ANSIERR, "ANSIERR_" ) \
def_msg_type( ANSIWARN, "ANSIWARN_" ) \
def_msg_type( STYLE, "WARN_" ) \
def_msg_type( JCK, "JCK_" ) \

typedef enum {
#define def_msg_type( e, p )    MSG_TYPE_##e,
    ALL_MSG_TYPES
    def_msg_type( END, "" )
#undef def_msg_type
} msg_type;

static const char *msgTypeNames[] = {
#define def_msg_type( e, p )    "MSG_TYPE_" #e ,
    ALL_MSG_TYPES
#undef def_msg_type
};

static const char *msgTypeNamesGP[] = {
#define def_msg_type( e, p )     #e ,
    ALL_MSG_TYPES
#undef def_msg_type
};

const char *langName[] = {
    #define LANG_DEF( id, dbcs )        #id ,
    LANG_DEFS
    #undef LANG_DEF
};

unsigned langTextCount[LANG_MAX];

typedef struct msggroup MSGGROUP;
typedef struct msgsym MSGSYM;
typedef struct word MSGWORD;
typedef struct wordref MSGWORDREF;

struct msggroup {
    MSGGROUP    *next;
    char        prefix[3];
    unsigned    msgIndex;    //first msg in group
    unsigned    emsgIndex;   //last message + 1
    unsigned    num;
    char        name[1];
};

struct msgsym {
    MSGSYM      *next;
    MSGSYM      *sortedByName[2];
    char        *lang_txt[LANG_MAX];
    char        *fname;
    unsigned    line;
    unsigned    index;
    unsigned    grpIndex;  //msg index in group
    msg_type    mtype;
    unsigned    level;
    unsigned    style : 1;
    MSGGROUP    *grp;
    MSGWORDREF  *words;
    char        name[1];
};

struct word {
    MSGWORD     *sortedByName[2];
    MSGWORD     *sortedByRef[2];
    MSGWORD     *all;
    unsigned    len;
    unsigned    index;
    unsigned    references;
    char        name[1];
};

struct wordref {
    MSGWORDREF  *next;
    MSGWORD     *word;
};

#define LINE_SIZE       (512)

static struct {
    boolbit     international           : 1;    // - dump internationalized data
    boolbit     quiet                   : 1;    // - quiet mode
    boolbit     gen_pick                : 1;    // - generate pick macros unstead of #defines
    boolbit     grouped                 : 1;    // - groups detected
    boolbit     have_msg                : 1;    // - have first message
    boolbit     gen_gpick               : 1;    // - generate generalized pick macros and tables
    boolbit     ignore_prefix           : 1;    // - ignore matching XXX_ prefix with message type
    boolbit     warnings_always_rebuild : 1;    // - warnings gen files with old dates to constantly force rebuilds
    boolbit     no_warn                 : 1;    // - don't print warning messages
    boolbit     out_utf8                : 1;    // - output texts uses UTF-8 encoding
    unsigned    max_len;
} flags;

typedef enum {
    EK_NULL,
    EK_BAD,
    EK_GOOD,
} err_type;

static struct {
    unsigned    line;
    err_type    kind;
    boolbit     active : 1;
} examples;

static char *ifname;
static FILE *i_gml;
static FILE *o_msgc;
static FILE *o_msgh;
static FILE *o_levh;

static MSGWORD *allWords;
static MSGWORD *nameWords;
static MSGWORD *refWords;
static MSGSYM *messageSyms;
static MSGSYM **currMSGSYM = &messageSyms;
static MSGSYM *sortedSyms;
static MSGGROUP *allGroups;
static MSGGROUP *currGroup;

static unsigned groupIndex;
static unsigned groupCounter;
static unsigned messageIndex;
static unsigned messageCounter;
static unsigned line;
static unsigned errors;
static unsigned warnings;
static unsigned nextOutputFName;

static const char *outputFNames[10];

static char *entireGML;
static char *currGML;
static char *ibuff;

// token buffers
static char tag[LINE_SIZE];
static char group[LINE_SIZE];
static char sym[LINE_SIZE];
static char word[LINE_SIZE];

// statistics
static unsigned uniqueWords;
static unsigned multiRefWords;
static size_t maxWordLen;
static size_t maxMsgLen;
static size_t totalMsgLen;
static size_t totalBytes;

// some local functions which need predefining
static void outputNum (FILE *fp, unsigned n);

// encoding
#define MAX_MSGWORD_LEN 31
#define ENC_BIT         0x80
#define LARGE_BIT       0x40
#define USE_SMALL_ENC   0x3f

#define NO_INDEX        (unsigned)(-1)

#define IS_ASCII(c)     (c < 0x80)

typedef int (*comp_fn)(const void *,const void *);

/*
 * Shift-JIS (CP932) lead byte ranges
 * 0x81-0x9F
 * 0xE0-0xFC
 */
static cvt_chr cvt_table_932[] = {
    #define pickb(s,u) {s, u},
    #define picki(s,u)
    #include "cp932uni.h"
    #undef picki
    #undef pickb
};

#if 0
static int compare_enc( const cvt_chr *p1, const cvt_chr *p2 )
{
    return( p1->s - p2->s );
}
#endif

static int compare_utf8( const cvt_chr *p1, const cvt_chr *p2 )
{
    return( p1->u - p2->u );
}

static void error( const char *f, ... )
{
    va_list args;

    ++errors;
    va_start( args, f );
    if( line ) {
        printf( "%s(%u): Error! E000: ", ifname, line );
    }
    vprintf( f, args );
    va_end( args );
}

static void warn( const char *f, ... )
{
    va_list args;

    ++warnings;
    va_start( args, f );
    if( !flags.no_warn ) {
        if( line ) {
            printf( "%s(%u): Warning! W000: ", ifname, line );
        }
        vprintf( f, args );
    }
    va_end( args );
}

static void errorLocn( const char *fn, unsigned ln, const char *f, ... )
{
    va_list args;

    ++errors;
    va_start( args, f );
    if( ln ) {
        printf( "%s(%u): Error! E000: ", fn, ln );
    }
    vprintf( f, args );
    va_end( args );
}

static void fatal( const char *m )
{
    error( "fatal: %s\n", m );
    exit( EXIT_FAILURE );
}

static void fatal1( const char *m, const char *p )
{
    error( "fatal: %s (%s)\n", m, p );
    exit( EXIT_FAILURE );
}

static void initFILE( FILE **f, const char *n, const char *m )
{
    *f = fopen( n, m );
    if( *f == NULL ) {
        fatal( "cannot open file" );
    }
    if( m[0] == 'r' ) {
    } else {
        // write access
        outputFNames[nextOutputFName++] = n;
    }
}

#define NUM_FILES   4

static bool processOptions( int argc1, char **argv1 )
{
    if( argc1 < NUM_FILES ) {
        return( true );
    }
    if( strcmp( *argv1, "-w" ) == 0 ) {
        flags.warnings_always_rebuild = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-s" ) == 0 ) {
        flags.no_warn = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-i" ) == 0 ) {
        flags.international = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-ip" ) == 0 ) {
        flags.ignore_prefix = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-q" ) == 0 ) {
        flags.quiet = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-p" ) == 0 ) {
        flags.gen_pick = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-g" ) == 0 ) {
        flags.gen_gpick = true;
        NEXT_ARG_CHECK();
    }
    if( strcmp( *argv1, "-utf8" ) == 0 ) {
        flags.out_utf8 = true;
        NEXT_ARG_CHECK();
    }
    flags.max_len = 127;
    if( strcmp( *argv1, "-len" ) == 0 ) {
        long    val;

        NEXT_ARG_CHECK();
        val = atol( *argv1 );
        if( val > 0 ) {
            flags.max_len = (unsigned)val;
        }
        NEXT_ARG_CHECK();
    }
    if( argc1 != NUM_FILES )
        return( true );
    ifname = *argv1;
    initFILE( &i_gml, *argv1, "rb" );
    --argc1;
    ++argv1;
    initFILE( &o_msgc, *argv1, "w" );
    --argc1;
    ++argv1;
    initFILE( &o_msgh, *argv1, "w" );
    --argc1;
    ++argv1;
    initFILE( &o_levh, *argv1, "w" );
    if( !flags.out_utf8 ) {
        qsort( cvt_table_932, sizeof( cvt_table_932 ) / sizeof( cvt_table_932[0] ), sizeof( cvt_table_932[0] ), (comp_fn)compare_utf8 );
    }
    return( false );
}

static size_t skipSpace( const char *start )
{
    const char  *p;

    for( p = start; *p != '\0'; ++p ) {
        if( !isspace( *p ) ) {
            break;
        }
    }
    return( p - start );
}

static size_t skipNonSpace( char *t, const char *start )
{
    const char  *p;

    for( p = start; *p != '\0'; ++p ) {
        if( isspace( *p ) ) {
            break;
        }
        *t++ = *p;
    }
    *t = '\0';
    return( p - start );
}

static tag_id getId( const char *p, const char **update_p )
{
    const char  **tc;

    p += skipNonSpace( tag, p );
    if( p[-1] != '.' ) {
        error( "tag missing '.': %s\n", tag );
        return( TAG_MAX );
    }
    p += skipSpace( p );
    *update_p = p;
    for( tc = tagNames; *tc != NULL; ++tc ) {
        if( stricmp( tag, *tc ) == 0 ) {
            return( tc - tagNames );
        }
    }
    error( "unknown tag: %s\n", tag );
    return( TAG_MAX );
}

static MSGSYM *mustBeProceededByMSGSYM( void )
{
    if( currMSGSYM == &messageSyms ) {
        error( "tag %s must be proceeded by :MSGSYM.\n", tag );
        fatal( "cannot continue" );
    }
    assert( offsetof( MSGSYM, next ) == 0 );
    return( (MSGSYM*)currMSGSYM );
}

static unsigned long pickUpNum( const char *p )
{
    unsigned long   num;
    char            *end;

    p += skipSpace( p );
    num = strtoul( p, &end, 0 );
    return( num );
}


static unsigned pickUpLevel( const char *p )
{
    unsigned long   level;

    level = pickUpNum( p );
    if( level == 0 || level > 15 ) {
        error( "<level> can only be in the range 1-15\n" );
    }
    return( level );
}

static MSGSYM *addToSorted( MSGSYM *m )
{
    int         s;
    MSGSYM      **h;
    MSGSYM      *c;
    const char  *name;

    name = m->name;
    h = &sortedSyms;
    for( ;; ) {
        c = *h;
        if( c == NULL )
            break;
        s = strcmp( c->name, name );
        if( s < 0 ) {
            h = &(c->sortedByName[0]);
        } else if( s > 0 ) {
            h = &(c->sortedByName[1]);
        } else {
            return( c );
        }
    }
    *h = m;
    return( NULL );
}


#define do_msgjgrptxt   NULL
#define do_ansicomp     NULL
#define do_errbreak     NULL

static void noActive( err_type kind )
{
    if( examples.active ) {
        error( "example already active (started on line %u)\n", examples.line );
    }
    examples.active = true;
    examples.kind = kind;
    examples.line = line;
}

static void yesActive( err_type check )
{
    if( !examples.active ) {
        error( "no example active\n" );
    } else {
        if( examples.kind != check ) {
            error( "end example doesn't match start example on line %u\n", examples.line );
        }
    }
    examples.active = false;
}

static void do_errbad( const char *p )
{
    /* unused parameters */ (void)p;

    noActive( EK_BAD );
}

static void do_eerrbad( const char *p )
{
    /* unused parameters */ (void)p;

    yesActive( EK_BAD );
}

static void do_errgood( const char *p )
{
    /* unused parameters */ (void)p;

    noActive( EK_GOOD );
}

static void do_eerrgood( const char *p )
{
    /* unused parameters */ (void)p;

    yesActive( EK_GOOD );
}

static void do_msggrptxt( const char *p )
{
    /* unused parameters */ (void)p;
}

static void do_msggrpstr( const char *p )
{
    MSGGROUP *grp;
    size_t len;

    grp = currGroup;
    p += skipNonSpace( group, p );
    len = strlen( group );
    if( len > 2 ) {
        error( ":msggrpstr value '%s' is too long\n", group );
        len = 2;
    }
    if( grp != NULL ) {
        strncpy( grp->prefix, group, len ); //default
        grp->prefix[len] = '\0';
    }
}

static void do_msggrpnum( const char *p )
{
    MSGGROUP *grp;
    grp = currGroup;
    groupIndex = pickUpNum( p );
    if( grp != NULL ) {
        grp->num = groupIndex; //set with value
    }
}

static void do_emsggrp( const char *p )
{
    MSGGROUP *grp;

    /* unused parameters */ (void)p;

    grp = currGroup;
    if( !flags.grouped ) {
        error( "missing :msggroup\n" );
    }
    if( grp != NULL ) {
        grp->emsgIndex = messageCounter;
    }
    flags.grouped = false;
    flags.have_msg = false;
    groupIndex += 256;
}

static void do_msggrp( const char *p )
{
    MSGGROUP    *grp;
    MSGGROUP    *cur;
    MSGGROUP    **end;
    size_t      len;
    bool        saw_dup;

    ++groupCounter;
    if( flags.grouped ) {
        error( "missing :emsggroup\n" );
    } else {
        if( flags.have_msg ) {
            error( ":msgsym not enclosed in :msggrp\n" );
        }
    }
    flags.grouped = true;
    p += skipNonSpace( group, p );
    len = strlen( group );
    if( !flags.gen_gpick && len != 2 ) {
        error( ":msggroup code '%s' not two characters\n", group );
        switch( len ) {
        case 0 :
            group[0] = ' ';
            // drops thru
        case 1 :
            group[1] = ' ';
            // drops thru
        default :
            group[2] = '\0';
        }
        len = 2;
    }
    grp = malloc( sizeof( *grp ) + len );
    grp->next = NULL;
    strcpy( grp->name, group );
    messageIndex = 0;
    grp->msgIndex = messageCounter;
    grp->num = groupIndex; //set with default
    strncpy( grp->prefix, group, 2 ); //default
    grp->prefix[2] = '\0';
    currGroup = grp;
    end = &allGroups;
    saw_dup = false;
    for( ;; ) {
        cur = *end;
        if( 0 == cur ) {
            *end = grp;
            break;
        } else {
            if( (strcmp( cur->name, group ) == 0) && !saw_dup ) {
                error( "msggroup code '%s' same as previous code\n", group );
                saw_dup = true;
            }
        }
        end = &cur->next;
    }
}

static void do_msgsym( const char *p )
{
    MSGSYM  *msg;
    int     i;
    size_t  len;

    ++messageCounter;
    flags.have_msg = true;
    p += skipNonSpace( sym, p );
    len = strlen( sym );
    msg = malloc( sizeof( *msg ) + len );
    msg->next = *currMSGSYM;
    *currMSGSYM = msg;
    currMSGSYM = &(msg->next);
    msg->sortedByName[0] = NULL;
    msg->sortedByName[1] = NULL;
    msg->fname = ifname;
    msg->line = line;
    msg->grpIndex =  messageIndex;
    msg->index = groupIndex + messageIndex;
    ++messageIndex;
    msg->level = 0;
    msg->style = 0;
    msg->mtype = MSG_TYPE_ERROR;
    for( i = 0; i < LANG_MAX; ++i ) {
        msg->lang_txt[i] = NULL;
    }
    msg->words = NULL;
    strcpy( msg->name, sym );
    if( addToSorted( msg ) != NULL ) {
        error( "message name has been used before '%s'\n", sym );
    }
    msg->grp = currGroup;
}

static size_t commonTxt( const char *p )
{
    size_t len;

    len = strlen( p ) + 1;
    if( len > maxMsgLen ) {
        maxMsgLen = len;
    }
    return( len );
}

static void do_msgtxt( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    m->lang_txt[LANG_English] = strdup( p );
    totalMsgLen += commonTxt( p );
}

static void do_msgjtxt( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    m->lang_txt[LANG_Japanese] = strdup( p );
    commonTxt( p );
    if( p[0] ) {
        langTextCount[LANG_Japanese]++;
    }
}

static void do_info( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    /* unused parameters */ (void)p;

    m->mtype = MSG_TYPE_INFO;
}

static void do_style( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    /* unused parameters */ (void)p;

    if( m->mtype != MSG_TYPE_WARNING ) {
        error( ":style. can only be used with :warning.\n" );
    } else {
        m->mtype = MSG_TYPE_STYLE;
    }
}

static void do_warning( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    m->mtype = MSG_TYPE_WARNING;
    m->level = pickUpLevel( p );
}

static void do_ansierr( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    /* unused parameters */ (void)p;

    m->mtype = MSG_TYPE_ANSIERR;
}

static void do_ansiwarn( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    m->mtype = MSG_TYPE_ANSIWARN;
    m->level = pickUpLevel( p );
}

static void do_ansi( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    m->mtype = MSG_TYPE_ANSI;
    m->level = pickUpLevel( p );
}

static void do_jck( const char *p )
{
    MSGSYM *m = mustBeProceededByMSGSYM();

    m->mtype = MSG_TYPE_JCK;
    m->level = pickUpLevel( p );
}

static void (*processLine[])( const char * ) = {
#define def_tag( e ) do_##e,
    ALL_TAGS
#undef def_tag
};

static void checkForGMLEscape( const char *p )
{
    bool is_escape;
    char c1, c2;

    ++p;
    c1 = *p++;
    if( c1 == '\0' || ! isalpha( c1 ) ) {
        return;
    }
    c2 = *p++;
    if( c2 == '\0' || ! isalpha( c2 ) ) {
        return;
    }
    is_escape = false;
    if( *p == '\0' || ! isalpha( *p ) ) {
        is_escape = true;
    }
    if( is_escape ) {
        error( "possible GML escape sequence: &%c%c\n", c1, c2 );
    }
}

static char *inputIO( void )
{
    char    c;
    char    *s;
    char    *p;

    s = currGML;
    p = s;
    for( ;; ) {
        c = *p;
        if( c == '&' ) {
            checkForGMLEscape( p );
        }
        if( c == '\0' ) {
            ibuff = NULL;
            return( NULL );
        }
        if( c == '\n' )
            break;
        if( c == '\r' && p[1] == '\n' ) {
            *p = '\0';
            ++p;
            break;
        }
        ++p;
    }
    currGML = p + 1;
    *p = '\0';
    ibuff = s;
    return( s );
}

static void suckInFile( void )
{
    int fh;
    struct stat info;

    fh = fileno( i_gml );
    if( fstat( fh, &info ) != 0 ) {
        fatal( "cannot get size of input file" );
    }
    entireGML = malloc( info.st_size + 2 );
    if( read( fh, entireGML, info.st_size ) != info.st_size ) {
        fatal( "cannot read entire input file" );
    }
    entireGML[info.st_size] = '\n';
    entireGML[info.st_size + 1] = '\0';
    currGML = entireGML;
}

static int strPref( MSGSYM *m, const char *pref )
{
    return memcmp( m->name, pref, strlen( pref ) );
}

static int percentPresent( char c, const char *p )
{
    char f;

    if( *p == '\0' ) {
        // will get the English text so it is fine
        return( 1 );
    }
    for( ; *p != '\0'; ++p ) {
        if( *p != '%' ) continue;
        f = *++p;
        if( f == '\0' ) break;
        if( f == c ) {
            return( 1 );
        }
    }
    return( 0 );
}

static void checkReplacements( MSGSYM *m, int start_lang, int end_lang )
{
    int i;
    char c;
    char *eng_text;
    char *p;

    eng_text = m->lang_txt[LANG_English];
    if( eng_text == NULL ) {
        return;
    }
    for( p = eng_text; *p; ++p ) {
        if( *p != '%' ) {
            continue;
        }
        c = *++p;
        if( c == '\0' ) break;
        for( i = start_lang; i < end_lang; ++i ) {
            char *intl_text;
            if( i == LANG_English ) continue;
            intl_text = m->lang_txt[i];
            if( intl_text == NULL ) continue;
            if( ! percentPresent( c, intl_text ) ) {
                errorLocn( m->fname, m->line, "MSGSYM %s's %s text has format mismatch for %%%c\n", m->name,
                langName[i], c );
            }
        }
    }
}

static void checkMessages( void )
{
    MSGSYM *m;
    int start_lang;
    int end_lang;
    int i;

    for( m = messageSyms; m != NULL; m = m->next ) {
        if( flags.international ) {
            start_lang = 0;
            end_lang = LANG_MAX;
        } else {
            start_lang = LANG_English;
            end_lang = start_lang + 1;
        }
        for( i = start_lang; i < end_lang; ++i ) {
            if( m->lang_txt[i] == NULL ) {
                errorLocn( m->fname, m->line, "MSGSYM %s has no %s text\n", m->name,
                langName[i] );
            }
        }
        checkReplacements( m, start_lang, end_lang );
        if( !flags.ignore_prefix ) {
            switch( m->mtype ) {
            #define def_msg_type( e, p ) case MSG_TYPE_##e: if( strPref( m, p ) ) errorLocn( m->fname, m->line, "MSGSYM %s has incorrect prefix (should be " p ")\n", m->name ); break;
                ALL_MSG_TYPES
            #undef def_msg_type
            default:
                break;
            }
        }
    }
}

static void readGML( void )
{
    tag_id      tag_id;
    const char  *p;
    void        (*process)( const char * );

    suckInFile();
    line = 0;
    currGroup = NULL;
    for( ;; ) {
        ++line;
        if( inputIO() == NULL )
            break;
        if( ibuff[0] != ':' ) {
            continue;
        }
        if( tolower( ibuff[1] ) == 'c'
          && tolower( ibuff[2] ) == 'm'
          && tolower( ibuff[3] ) == 't' ) {
            continue;
        }
        tag_id = getId( ibuff + 1, &p );
        if( tag_id == TAG_MAX ) {
            continue;
        }
        process = processLine[tag_id];
        if( process != NULL ) {
            process( p );
        }
    }
    messageIndex=0;
    checkMessages();
    if( errors > 0 ) {
        fatal( "cannot continue due to errors" );
    }
    // messages involve an extra ' ' at the end
    // so we'll add 16 and round up to mod 16 to be absolutely safe
    maxMsgLen = ( ( maxMsgLen + 16 ) + 0x0f ) & ~ 0x0f;
}

static MSGWORD *addWord( MSGSYM *m )
{
    int s;
    MSGWORD **h;
    MSGWORD *c;
    size_t len;

    h = &nameWords;
    for( ;; ) {
        c = *h;
        if( c == NULL ) break;
        s = strcmp( c->name, word );
        if( s < 0 ) {
            h = &(c->sortedByName[0]);
        } else if( s > 0 ) {
            h = &(c->sortedByName[1]);
        } else {
            c->references++;
            return( c );
        }
    }
    len = strlen( word );
    if( len > maxWordLen ) {
        maxWordLen = len;
    }
    if( len > MAX_MSGWORD_LEN ) {
        errorLocn( m->fname, m->line, "MSGSYM %s: word '%s' is too long\n", m->name, word );
    }
    ++uniqueWords;
    c = calloc( 1, sizeof( *c ) + len );
    *h = c;
    c->references = 1;
    c->index = NO_INDEX;
    c->len = (unsigned)len;
    c->all = allWords;
    allWords = c;
    strcpy( c->name, word );
    return( c );
}

static int cmpRef( MSGWORD *l, MSGWORD *r )
{
    int s;

    if( l->references < r->references ) {
        return( -1 );
    } else if( l->references > r->references ) {
        return( 1 );
    }
    if( l->len < r->len ) {
        return( -1 );
    } else if( l->len > r->len ) {
        return( 1 );
    }
    if( l->index < r->index ) {
        return( -1 );
    } else if( l->index > r->index ) {
        return( 1 );
    }
    s = strcmp( l->name, r->name );
    assert( s != 0 );
    return( s );
}

static void addRef( MSGWORD *w )
{
    MSGWORD **h;
    MSGWORD *c;
    int s;

    if( w->references > 1 ) {
        ++multiRefWords;
    }
    h = &refWords;
    for( ;; ) {
        c = *h;
        if( c == NULL ) break;
        s = cmpRef( w, c );
        if( s < 0 ) {
            h = &(c->sortedByRef[0]);
        } else if( s > 0 ) {
            h = &(c->sortedByRef[1]);
        } else {
            *h = w;
            w->sortedByRef[0] = c;
            w->sortedByRef[1] = c->sortedByRef[1];
            c->sortedByRef[1] = NULL;
            return;
        }
    }
    *h = w;
}

static void sortByRefs( void )
{
    MSGWORD *w;

    for( w = allWords; w != NULL; w = w->all ) {
        addRef( w );
    }
}

static void traverseHiToLoRefs( MSGWORD *w, void (*t)( MSGWORD *, void * ), void *data )
{
    MSGWORD *r;

    while( w != NULL ) {
        r = w->sortedByRef[1];
        if( r != NULL ) {
            traverseHiToLoRefs( r, t, data );
        }
        t( w, data );
        w = w->sortedByRef[0];
    }
}

static void splitIntoWords( void )
{
    MSGSYM *m;
    MSGWORD *w;
    MSGWORDREF *r;
    MSGWORDREF **a;
    char *p;

    for( m = messageSyms; m != NULL; m = m->next ) {
        p = m->lang_txt[LANG_English];
        a = &(m->words);
        for( ;; ) {
            if( p[0] && isspace( p[0] ) && isspace( p[1] ) ) {
                errorLocn( m->fname, m->line, "MSGSYM %s text has too many blanks '%s'\n", m->name, p );
            }
            p += skipSpace( p );
            if( *p == '\0' )
                break;
            p += skipNonSpace( word, p );
            w = addWord( m );
            r = malloc( sizeof( *r ) );
            r->word = w;
            r->next = NULL;
            *a = r;
            a = &(r->next);
        }
    }
}

#if 0
static void doDumpWORD( MSGWORD *w, void *d )
{
    d = d;
    printf( "%6u %s\n", w->references, w->name );
}
#endif

static void compressMsgs( void )
{
    splitIntoWords();
    sortByRefs();
    //traverseHiToLoRefs( refWords, doDumpWORD, NULL );
}

static void writeExtraDefs( FILE *fp )
{
    fputc( '\n', fp );
    fputs(
        "#define ENC_BIT 0x80\n"
        "#define LARGE_BIT 0x40\n"
        "#define MAX_MSG ", fp );
    outputNum( fp, (unsigned)maxMsgLen );
    fputc( '\n', fp );
    fputc( '\n', fp );
}

static void writeMsgH( void )
{
    MSGSYM *m;

    if( !flags.gen_pick ) {
        for( m = messageSyms; m != NULL; m = m->next ) {
            fputs( "#define ", o_msgh );
            fputs( m->name, o_msgh );
            fputc( ' ', o_msgh );
            outputNum( o_msgh, m->index );
            fputc( '\n', o_msgh );
        }
        fputs(
            "\ntypedef struct msg_level_info {\n"
            "    unsigned    type : 4;\n"
            "    unsigned    level : 4;\n"
            "    unsigned    enabled : 1;\n"
            "} msg_level_info;\n", o_msgh );
    } else {
        fputs( "#define MSG_DEFS \\\n", o_msgh );
        for( m = messageSyms; m != NULL; m = m->next ) {
            fputs( "MSG_DEF( ", o_msgh );
            fputs( m->name, o_msgh );
            fputs( " , ", o_msgh );
            outputNum( o_msgh, m->index );
            fputs( " )\\\n", o_msgh );
        }
        fputs( "\n\n", o_msgh );
    }
    writeExtraDefs( o_msgh );
}

static void writeMsgHGP( void )
{
    MSGSYM *m;
    MSGGROUP *grp;
    int     index;

    fputs( "//MSG_DEF( name, group, kind, level, group_index )\n",o_msgh );
    fputs( "\n\n", o_msgh );
    fputs( "#define MSG_DEFS \\\n", o_msgh );
    index = 0;
    for( m = messageSyms; m != NULL; m = m->next,++index ) {
        fputs( "MSG_DEF( ", o_msgh );
        fputs( m->name, o_msgh );
        fputs( " , ", o_msgh );
        grp = m->grp;
        if( grp != NULL ) {
            fputs( grp->name, o_msgh );
            fputs( " , ", o_msgh );
        }
        fputs( msgTypeNamesGP[m->mtype], o_msgh );
        fputs( " , ", o_msgh );
        outputNum( o_msgh, m->level );
        fputs( " , ", o_msgh );
        outputNum( o_msgh, m->grpIndex );
        fputs( " )\\\n", o_msgh );
    }
    fputs( "\n\n", o_msgh );
}

static void writeMsgCGP( void )
{
    MSGSYM *m;
    for( m = messageSyms; m != NULL; m = m->next ) {
        fputs( "\"", o_msgc );
        fputs( m->lang_txt[LANG_English], o_msgc );
        fputs( "\",\n", o_msgc );
    }
}

static void writeLevHGP( void )
{
    MSGGROUP *grp;
    int     index;

    fputs( "\n\n", o_levh );
    fputs( "#define MSGTYPES_DEFS \\\n", o_levh );
    for( index = 0; index < MSG_TYPE_END; ++index ) {
        fputs( "MSGTYPES_DEF( ", o_levh );
        fputs( msgTypeNamesGP[index], o_levh );
        fputs( " )\\\n", o_levh );
    }
    fputs( "\n\n", o_levh );
    fputs( "//define GRP_DEF( name,prefix,num,index,eindex )\n", o_levh );
    fputs( "#define GRP_DEFS \\\n", o_levh );
    for( grp = allGroups; grp != NULL; grp = grp->next ) {
        fputs( "GRP_DEF( ", o_levh );
        fputs( grp->name, o_levh );
        fputc( ',', o_levh );
        fputs( grp->prefix, o_levh );
        fputc( ',', o_levh );
        outputNum( o_levh, grp->num );
        fputc( ',', o_levh );
        outputNum( o_levh, grp->msgIndex );
        fputc( ',', o_levh );
        outputNum( o_levh, grp->emsgIndex );
        fputs( " )\\\n", o_levh );
    }
    fputs( "\n\n", o_levh );
}

static void outputNum( FILE *fp, unsigned n )
{
    char buff[16];

    sprintf( buff, "%u", n );
    fputs( buff, fp );
}

static void outputNumJ( FILE *fp, unsigned n, int width )
{
    char buff[16];
    char *p;

    sprintf( buff, "%u", n );
    for( p = buff; *p; ++p ) {
        --width;
    }
    while( width > 0 ) {
        --width;
        fputc( ' ', fp );
    }
    fputs( buff, fp );
}

static void outputChar( FILE *fp, char c )
{
    fputc( '\'', fp );
    switch( c ) {
    case '\'':
        fputc( '\\', fp );
        fputc( '\'', fp );
        break;
    case '\\':
        fputc( '\\', fp );
        fputc( '\\', fp );
        break;
    default:
        if( isprint( c ) ) {
            fputc( c, fp );
        } else {
            char buff[16];

            fputc( '\\', fp );
            fputc( 'x', fp );
            sprintf( buff, "%x", c );
            fputs( buff, fp );
        }
    }
    fputc( '\'', fp );
    fputc( ',', fp );
}

static void outputTableName( FILE *fp, const char *type, const char *name )
{
    fprintf( fp, "MSG_SCOPE %s MSG_MEM %s[] = {\n", type, name );
}

typedef struct {
    unsigned    current_base;
    unsigned    *word_base;
    MSGWORD     **keep_base;
    unsigned    current_text;
} data_word_tables;

static void doEncodeWORD( MSGWORD *w, void *d )
{
    data_word_tables *data = d;
    char *p;

    if( w->references <= 1 ) {
        return;
    }
    w->index = data->current_base++;
    data->word_base[w->index] = data->current_text;
    data->keep_base[w->index] = w;
    data->current_text += w->len;
    for( p = w->name; *p; ++p ) {
        outputChar( o_msgc, *p );
    }
    fputc( '\n', o_msgc );
}

static void writeWordTable( void )
{
    MSGWORD *w;
    unsigned i;
    data_word_tables data_w;

    data_w.current_base = 0;
    data_w.word_base = malloc( ( multiRefWords + 1 ) * sizeof( unsigned ) );
    data_w.keep_base = malloc( ( multiRefWords + 1 ) * sizeof( MSGWORD * ) );
    data_w.current_text = 0;
    outputTableName( o_msgc, "char const", "word_text" );
    traverseHiToLoRefs( refWords, doEncodeWORD, &data_w );
    fputs( "};\n", o_msgc );
    totalBytes += data_w.current_text;
    data_w.word_base[data_w.current_base] = data_w.current_text;
    data_w.keep_base[data_w.current_base] = NULL;
    data_w.current_base++;
    totalBytes += data_w.current_base * sizeof( short );
    outputTableName( o_msgc, "unsigned short const", "word_base" );
    for( i = 0; i < data_w.current_base; ++i ) {
        w = data_w.keep_base[i];
        outputNumJ( o_msgc, data_w.word_base[i], 6 );
        fputc( ',', o_msgc );
        if( w != NULL ) {
            fputs( " /* ", o_msgc );
            outputNumJ( o_msgc, w->references, 6 );
            fputc( ' ', o_msgc );
            fputc( '(', o_msgc );
            outputNum( o_msgc, i );
            fputc( ')', o_msgc );
            fputc( ' ', o_msgc );
            fputs( w->name, o_msgc );
            fputs( " */", o_msgc );
        }
        fputc( '\n', o_msgc );
    }
    fputs( "};\n", o_msgc );
    free( data_w.word_base );
    free( data_w.keep_base );
}

static void writeMsgTable( void )
{
    MSGSYM *m;
    unsigned *msg_base;
    unsigned current_base;
    unsigned current_text;
    unsigned i;
    MSGWORDREF *r;
    MSGWORD *w;
    char *p;

    current_text = 0;
    current_base = 0;
    msg_base = malloc( ( messageCounter + 1 ) * sizeof( unsigned ) );
    outputTableName( o_msgc, "uint_8 const", "msg_text" );
    for( m = messageSyms; m != NULL; m = m->next ) {
        msg_base[current_base++] = current_text;
        fputs( "\n/* ", o_msgc );
        outputNumJ( o_msgc, m->index, 4 );
        fputc( ' ', o_msgc );
        fputs( m->lang_txt[LANG_English], o_msgc );
        fputs( " */\n", o_msgc );
        for( r = m->words; r != NULL; r = r->next ) {
            w = r->word;
            if( w->index == NO_INDEX ) {
                outputNum( o_msgc, w->len );
                fputs( ", ", o_msgc );
                for( p = w->name; *p; ++p ) {
                    outputChar( o_msgc, *p );
                }
                fputc( '\n', o_msgc );
                current_text += 1 + w->len;
            } else {
                if( w->index <= USE_SMALL_ENC ) {
                    fputs( "ENC_BIT | ", o_msgc );
                    outputNum( o_msgc, w->index );
                    ++current_text;
                } else {
                    fputs( "ENC_BIT | LARGE_BIT | ", o_msgc );
                    outputNum( o_msgc, ( w->index >> 8 ) );
                    fputs( ", ", o_msgc );
                    outputNum( o_msgc, ( w->index & 0x0ff ) );
                    current_text += 2;
                }
                fputs( ", /* ", o_msgc );
                fputs( w->name, o_msgc );
                fputs( " */\n", o_msgc );
            }
        }
    }
    fputs( "};\n", o_msgc );
    totalBytes += current_text;
    msg_base[current_base++] = current_text;
    totalBytes += current_base * sizeof( short );
    outputTableName( o_msgc, "unsigned short const", "msg_base" );
    for( i = 0; i < current_base; ++i ) {
        outputNum( o_msgc, msg_base[i] );
        fputc( ',', o_msgc );
        fputc( '\n', o_msgc );
    }
    fputs( "};\n", o_msgc );
    if( allGroups != NULL ) {
        MSGGROUP *g;
        outputTableName( o_msgc, "unsigned short const", "msg_group_base" );
        for( g = allGroups; g != NULL; g = g->next ) {
            outputNum( o_msgc, g->msgIndex );
            fputc( ',', o_msgc );
            fputc( '\n', o_msgc );
        }
        assert( messageIndex == 0 );
        outputNum( o_msgc, messageCounter );
        fputs( ",\n};\n", o_msgc );
        fputs( "MSG_SCOPE char const MSG_MEM msg_group_name[][2] = {\n"
             , o_msgc );
        for( g = allGroups; g != NULL; g = g->next ) {
            char buf[10];
            buf[0] = '\'';
            buf[1] = g->name[0];
            buf[2] = '\'';
            buf[3] = ',';
            buf[4] = '\'';
            buf[5] = g->name[1];
            buf[6] = '\'';
            buf[7] = ',';
            buf[8] = '\n';
            buf[9] = '\0';
            fputs( buf, o_msgc );
        }
        fputs( "};\n", o_msgc );
        fputs( "#define MSGS_GROUPED\n", o_msgc );
    }
    free( msg_base );
}

static void writeMSGIfndefs( void )
{
    fputs(
        "#ifndef MSG_SCOPE\n"
        "#define MSG_SCOPE\n"
        "#endif\n"
        "#ifndef MSG_MEM\n"
        "#define MSG_MEM\n"
        "#endif\n", o_msgc );
}

static void writeMsgC( void )
{
    writeMSGIfndefs();
    writeExtraDefs( o_msgc );
    writeWordTable();
    writeMsgTable();
}

static void writeLevH( void )
{
    MSGSYM *m;

    fputs( "#ifndef MSG_CONST\n", o_levh );
    fputs( "#define MSG_CONST const\n", o_levh );
    fputs( "#endif\n", o_levh );
    fprintf( o_levh, "typedef enum {\n" );
#define def_msg_type( e,p )     fprintf( o_levh, "   MSG_TYPE_" #e ",\n" );
    ALL_MSG_TYPES
#undef def_msg_type
    fprintf( o_levh, "} MSG_TYPE;\n" );
    outputTableName( o_levh, "msg_level_info MSG_CONST", "msg_level" );
    for( m = messageSyms; m != NULL; m = m->next ) {
        fputc( '{', o_levh );
        fputc( ' ', o_levh );
        fputs( msgTypeNames[m->mtype], o_levh );
        if( m->level == 0 ) {
            fputs( ", 0, true }, /* ", o_levh );
        } else {
            fputs( ", ", o_levh );
            outputNum( o_levh, m->level );
            fputs( ", true }, /* ", o_levh );
        }
        fputs( m->name, o_levh );
        fputs( " */\n", o_levh );
    }
    totalBytes += messageIndex;
    fputs( "};\n", o_levh );
}

static void dumpStats( void )
{
    if( allGroups != NULL ) {
    printf( "# of groups                          %u\n", groupCounter );
    }
    printf( "# of messages                        %u\n", messageCounter );
    printf( "# of unique words                    %u\n", uniqueWords );
    printf( "# of words referenced more than once %u\n", multiRefWords );
    printf( "max message length                   %u\n", (unsigned)maxMsgLen );
    printf( "max word length                      %u\n", (unsigned)maxWordLen );
    printf( "total input bytes                    %u\n", (unsigned)totalMsgLen );
    printf( "total output bytes                   %u\n", (unsigned)totalBytes );
    printf( "%% compression                        %u\n", (unsigned)( ( totalBytes * 100 ) / totalMsgLen ) );
}

static void closeFiles( void )
{
    fclose( i_gml );
    fclose( o_msgh );
    fclose( o_msgc );
    fclose( o_levh );
}

static size_t utf8_to_cp932( const char *src, char *dst )
{
    size_t      i;
    size_t      o;
    size_t      src_len;
    cvt_chr     x;
    cvt_chr     *p;

    src_len = strlen( src );
    o = 0;
    for( i = 0; i < src_len && o < LINE_SIZE - 6; i++ ) {
        x.u = (unsigned char)src[i];
        if( IS_ASCII( x.u ) ) {
            /*
             * ASCII (0x00-0x7F), no conversion
             */
            dst[o++] = (char)x.u;
        } else {
            /*
             * UTF-8 to UNICODE conversion
             */
            if( (x.u & 0xF0) == 0xE0 ) {
                x.u &= 0x0F;
                x.u = (x.u << 6) | ((unsigned char)src[++i] & 0x3F);
            } else {
                x.u &= 0x1F;
            }
            x.u = (x.u << 6) | ((unsigned char)src[++i] & 0x3F);
            /*
             * UNICODE to CP932 encoding conversion
             */
            p = bsearch( &x, cvt_table_932, sizeof( cvt_table_932 ) / sizeof( cvt_table_932[0] ), sizeof( cvt_table_932[0] ), (comp_fn)compare_utf8 );
            if( p == NULL ) {
                printf( "unknown unicode character: 0x%4.4X\n", x.u );
                x.s = '?';
            } else {
                x.s = p->s;
            }
            if( x.s > 0xFF ) {
                /* write lead byte first */
                dst[o++] = (char)(x.s >> 8);
            }
            dst[o++] = (char)x.s;
        }
    }
    dst[o] = '\0';
    return( o );
}


static void dumpInternational( void )
{
    char const *text;
    MSGSYM *m;
    FILE *fp;
    unsigned lang;
    unsigned len;
    bool dump_warning;
    char err_fname[16];
    LocaleErrors errors_header;
    char tmp[512];

    for( lang = LANG_FIRST_INTERNATIONAL; lang < LANG_MAX; ++lang ) {
        sprintf( err_fname, "errors%02u." LOCALE_DATA_EXT, lang );
        fp = fopen( err_fname, "wb" );
        if( fp == NULL ) {
            fatal( "cannot open international file for write" );
        }
        memset( &errors_header, 0, sizeof( errors_header ) );
        errors_header.header.code = LS_Errors;
        errors_header.header.signature = LS_Errors_SIG;
        errors_header.number = messageCounter;
        fwrite( &errors_header, offsetof( LocaleErrors, data ), 1, fp );
        if( langTextCount[lang] > ( messageCounter / 2 ) ) {
            dump_warning = true;
        } else {
            dump_warning = false;
        }
        for( m = messageSyms; m != NULL; m = m->next ) {
            text = m->lang_txt[lang];
            if( *text == '\0' ) {
                if( dump_warning ) {
                    warn( "using English text for %s version of %s\n",
                        langName[lang], m->name );
                }
                text = m->lang_txt[LANG_English];
            }
            if( lang == LANG_Japanese ) {
                if( !flags.out_utf8 ) {
                    utf8_to_cp932( text, tmp );
                    tmp[sizeof( tmp ) - 1] = '\0';
                    text = tmp;
                }
            }
            len = (unsigned)strlen( text );
            if( len > flags.max_len ) {
                fatal1( "length of a international message is too long", text );
            }
            fputc( len, fp );
            fwrite( text, len, 1, fp );
        }
        fputc( 0, fp );
        fclose( fp );
    }
}

void forceRebuild( void )
{
    unsigned i;

    for( i = 0; i < nextOutputFName; ++i ) {
        struct stat info;
        struct utimbuf uinfo;

        if( stat( outputFNames[i], &info ) != -1 ) {
            unsigned three_years_ago = 60 * 60 * 24 * 366 * 3;
            memset( &uinfo, 0, sizeof(uinfo) );
            uinfo.actime = info.st_mtime - three_years_ago;
            uinfo.modtime = info.st_mtime - three_years_ago;
            utime( outputFNames[i], &uinfo );
        }
    }
}

int main( int argc, char **argv )
{
    bool    langs_ok;

    langs_ok = _LANG_DEFS_OK();
    if( !langs_ok )
        fatal( "language index mismatch\n" );

    if( processOptions( argc - 1, argv + 1 ) ) {
        error( "fatal: invalid argument\n\n" );
        fatal( "usage: msgencod [options] <gml> <msgc> <msgh> <levh>\n"
               "\n"
               "Options: (must appear in following order)\n"
               "  -w warnings generate files with old dates to constantly force rebuilds\n"
               "  -s no warnings\n"
               "  -i create international file with non-english data\n"
               "  -ip ignore matching XXX_ prefix with message type\n"
               "  -q quiet operation\n"
               "  -p generate pick macros instead of #defines\n"
               "  -g generate generalized pick macros and tables\n"
               "  -utf8 output texts use UTF-8 encoding"
             );
    }
    readGML();
    if( flags.gen_gpick ) {
        writeMsgHGP();
        writeMsgCGP();
        writeLevHGP();
    } else {
        compressMsgs();
        writeMsgH();
        writeMsgC();
        writeLevH();
    }
    closeFiles();
    if( flags.international ) {
        dumpInternational();
    }
    if( !flags.quiet ) {
        dumpStats();
    }
    if( warnings > 0 && flags.warnings_always_rebuild ) {
        forceRebuild();
    }
    return( ( errors > 0 ) ? EXIT_FAILURE : EXIT_SUCCESS );
}
