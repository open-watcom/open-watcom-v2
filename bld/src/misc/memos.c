#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "memos.h"

/* This program implements a simple memo facility.
 * Memos may be added to a memo file, displayed
 * on the screen, and deleted.
 *
 * Modified     by              reason
 * ========     ==              ======
 * 87/10/02     Steve McDowell  Initial implementation.
 * 88/09/20     Steve McDowell  Fixed up some style issues,
 *                              introduced use of TRUE and
 *                              FALSE.
 */

/* Define some constants to make the code more readable.
 */
#define TRUE        1
#define FALSE       0
#define NULLCHAR    '\0'

static const char FileName[] = { "memos.db" };
static const char TempName[] = { "tempmemo.db" };

static MEMO_EL *  MemoHead      = NULL;
static int        MemosModified = FALSE;
static int        QuitFlag      = TRUE;

typedef enum {
    INVALID,
    HELP,
    ADD,
    DELETE,
    REPLACE,
    SHOW,
    UP,
    DOWN,
    TOP,
    TODAY,
    SAVE,
    QUIT
} ACTION;

/* This table maps action keywords onto the "actions" defined
 * above. The table also defines short forms for the keywords.
 */
typedef struct {
    ACTION act;
    char * keyword;
} ACTION_MAP;

static ACTION_MAP KeywordMap[] = {
    HELP,    "help",
    HELP,    "h",
    ADD,     "add",
    ADD,     "a",
    DELETE,  "delete",
    DELETE,  "del",
    REPLACE, "replace",
    REPLACE, "rep",
    SHOW,    "show",
    SHOW,    "sh",
    UP,      "up",
    UP,      "u",
    DOWN,    "down",
    DOWN,    "d",
    DOWN,    "",
    TOP,     "top",
    TODAY,   "today",
    TODAY,   "tod",
    SAVE,    "save",
    SAVE,    "sa",
    QUIT,    "quit",
    QUIT,    "q",

    INVALID, "" };

/* Maximum buffer length (maximum length of line of memo).
 */
#define MAXLEN 80

/* Function prototypes.
 */
static TEXT_LINE *  AddLine();
static MEMO_EL *    AddMemo();
static MEMO_EL *    DeleteMemo();
static MEMO_EL *    DoActions();
static MEMO_EL *    DoDownAction();
static MEMO_EL *    DoUpAction();
static MEMO_EL *    EnterAMemo();
static ACTION       GetAction();
static void *       MemoMAlloc();
static ACTION       PromptAction();
static ACTION       ReadAction();
static MEMO_EL *    ReadAMemo();
static MEMO_EL *    ShowTodaysMemos();

extern int main( int argc, char * argv[] )
/****************************************/
{
    int       index;
    MEMO_EL * el;

    printf( "Memo facility\n" );

/* Check for a single argument that is a question mark,
 * If found, then display the usage notes.
 */
    if( argc == 2  &&  strcmp( argv[1], "?" ) == 0 ) {
        Usage();
        exit( 0 );
    }
    ReadMemos();
    MemosModified = FALSE;
    QuitFlag      = FALSE;

/* Use the command line parameters, if any, as the first
 * actions to be performed on the memos.
 */
    el = NULL;
    for( index = 1; index < argc; ++index ) {
        el = DoActions( el, GetAction( argv[index] ) );
        if( QuitFlag ) {
            return( FALSE );
        }
    }
    HandleMemoActions( el );
    return( FALSE );
}

static void ReadMemos( void )
/***************************/

/* Read the memos file, building the structure to contain it.
 */
{
    FILE *    fid;
    MEMO_EL * new_el;
    MEMO_EL * prev_el;
    int       mcount;

    fid = fopen( FileName, "r" );
    if( fid == NULL ) {
        printf( "Memos file not found."
                " Starting with no memos.\n" );
        return;
    }

/* Loop reading entire memos.
 */
    prev_el = NULL;
    for( mcount = 0;; mcount++ ) {
        new_el = ReadAMemo( fid );
        if( new_el == NULL ) {
            printf( "%d memo(s) found.\n", mcount );
            fclose( fid );
            return;
        }
        if( prev_el == NULL ) {
            MemoHead = new_el;
            new_el->prev = NULL;
        } else {
            prev_el->next = new_el;
            new_el->prev = prev_el;
        }
        new_el->next = NULL;
        prev_el = new_el;
    }
}

static int ReadLine( char buffer[], int len, FILE * fid )
/*******************************************************/

/* Read a line from the memos file. Handle any I/O errors and
 * EOF. Return the length read, not counting the newline on
 * the end.
 */
{
    if( fgets( buffer, len, fid ) == NULL ) {
        if( feof( fid ) ) {
            return( EOF );
        }
        perror( "Error reading memos file" );
        abort();
    }
    return( strlen( buffer ) - 1 );
}

static MEMO_EL * ReadAMemo( FILE * fid )
/**************************************/

/* Read one memo, creating the memo structure and filling it
 * in. Return a pointer to the memo (NULL if none read).
 */
{
    MEMO_EL *   el;
    int         len;
    TEXT_LINE * line;
    char        buffer[MAXLEN];

    len = ReadLine( buffer, MAXLEN, fid );
    if( len == EOF ) {
        return( NULL );
    }

/* First line must be of the form "Date:" or "Date:YY/MM/DD":
 */
    if( (len != 5  &&  len != 13)
            ||  strncmp( buffer, "Date:", 5 ) != 0 ) {
        BadFormat();
    }
    buffer[len] = NULLCHAR;
    el = MemoMAlloc( sizeof( MEMO_EL ) );
    el->text = NULL;
    strcpy( el->date, buffer + 5 );
    line = NULL;
    for( ;; ) {
        len = ReadLine( buffer, MAXLEN, fid );
        if( len == EOF ) {
            BadFormat();
        }
        buffer[len] = NULLCHAR;
        if( strcmp( buffer, "====" ) == 0 ) {
            return( el );
        }
        line = AddLine( buffer, el, line );
    }
}

static TEXT_LINE * AddLine( char        buffer[],
                            MEMO_EL *   el,
                            TEXT_LINE * prevline )
/************************************************/

/* Add a line of text to the memo, taking care of all the
 * details of modifying the structure.
 */
{
    TEXT_LINE * line;

    line = MemoMAlloc( sizeof( TEXT_LINE ) + strlen( buffer ) );
    strcpy( line->text, buffer );
    line->next = NULL;
    if( prevline == NULL ) {
        el->text = line;
    } else {
        prevline->next = line;
    }
    return( line );
}

static ACTION PromptAction( void )
/********************************/

/* The user didn't specify an action on the command line,
 * so prompt for it.
 */
{
    ACTION act;

    for( ;; ) {
        printf( "\nEnter an action:\n" );
        act = ReadAction();
        if( act != INVALID ) {
            return( act );
        }
        printf( "\nThat selection was not valid.\n" );
        Help();
    }
}

static ACTION ReadAction( void )
/******************************/

/* Read an action from the terminal.
 * Return the action code.
 */
{
    char buffer[80];

    if( gets( buffer ) == NULL ) {
        perror( "Error reading action" );
        abort();
    }
    return( GetAction( buffer ) );
}

static ACTION GetAction( char buffer[] )
/**************************************/

/* Given the string in the buffer, return the action that
 * corresponds to it.
 * The string in the buffer is first zapped into lower case
 * so that mixed-case entries are recognized.
 */
{
    ACTION_MAP * actmap;
    char *       bufptr;

    for( bufptr = buffer; *bufptr != NULLCHAR; ++bufptr ) {
        *bufptr = tolower( *bufptr );
    }
    for( actmap = KeywordMap; actmap->act != INVALID; ++actmap ) {
        if( strcmp( buffer, actmap->keyword ) == 0 ) break;
    }
    return( actmap->act );
}

static void HandleMemoActions( MEMO_EL * el )
/*******************************************/

/* Handle all the actions entered from the keyboard.
 */
{
    for( ;; ) {
        el = DoActions( el, PromptAction() );
        if( QuitFlag ) break;
    }
}

static MEMO_EL * DoActions( MEMO_EL * el, ACTION act )
/****************************************************/

/* Perform one action on the memos.
 */
{
    MEMO_EL * new_el;
    MEMO_EL * prev_el;

    switch( act ) {
      case HELP:
        Help();
        break;
      case ADD:
        new_el = AddMemo( el );
        if( new_el != NULL ) {
            el = new_el;
            MemosModified = TRUE;
        }
        break;
      case DELETE:
        el = DeleteMemo( el );
        MemosModified = TRUE;
        break;
      case REPLACE:
        prev_el = el;
        new_el = AddMemo( el );
        if( new_el != NULL ) {
            DeleteMemo( prev_el );
            MemosModified = TRUE;
        }
        break;
      case SHOW:
        DisplayMemo( el );
        break;
      case UP:
        el = DoUpAction( el );
        break;
      case DOWN:
        el = DoDownAction( el );
        break;
      case TOP:
        el = NULL;
        break;
      case TODAY:
        el = ShowTodaysMemos();
        break;
      case SAVE:
        if( SaveMemos() ) {
            MemosModified = FALSE;
        }
        break;
      case QUIT:
        if( WantToQuit() ) {
            QuitFlag = TRUE;
            el = NULL;
        }
    }
    return( el );
}

static MEMO_EL * AddMemo( MEMO_EL * el )
/**************************************/

/* Add a memo following the current one.
 */
{
    MEMO_EL * new_el;
    MEMO_EL * next;

    new_el = EnterAMemo();
    if( new_el == NULL ) {
        return( NULL );
    }
    if( el == NULL ) {
        next = MemoHead;
        MemoHead = new_el;
    } else {
        next = el->next;
        el->next = new_el;
    }
    new_el->prev = el;
    new_el->next = next;
    if( next != NULL ) {
        next->prev = new_el;
    }
    return( new_el );
}

static MEMO_EL * EnterAMemo( void )
/*********************************/

/* Read a memo from the keyboard, creating the memo structure
 * and filling it in. Return a pointer to the memo (NULL if
 * none read).
 */
{
    MEMO_EL *   el;
    int         len;
    TEXT_LINE * line;
    char        buffer[MAXLEN];

    printf( "What date do you want the memo displayed"
            " (YY/MM/DD)?\n" );
    if( gets( buffer ) == NULL ) {
        printf( "Error reading from terminal.\n" );
        return( NULL );
    }
    len = strlen( buffer );
    if( len != 0
            &&  (len != 8
                ||  buffer[2] != '/'
                || buffer[5] != '/') ) {
        printf( "Date is not valid.\n" );
        return( NULL );
    }
    el = MemoMAlloc( sizeof( MEMO_EL ) );
    el->text = NULL;
    strcpy( el->date, buffer );
    line = NULL;
    printf( "\nEnter the text of the memo.\n" );
    printf( "To terminate the memo,"
            " enter a line starting with =\n" );
    for( ;; ) {
        if( gets( buffer ) == NULL ) {
            printf( "Error reading from terminal.\n" );
            return( NULL );
        }
        if( buffer[0] == '=' ) {
            return( el );
        }
        line = AddLine( buffer, el, line );
    }
}

static MEMO_EL * DeleteMemo( MEMO_EL * el )
/*****************************************/

/* Delete the current memo.
 * Return a pointer to another memo, usually the following one.
 */
{
    MEMO_EL * prev;
    MEMO_EL * next;
    MEMO_EL * ret_el;

    if( el == NULL ) {
        return( MemoHead );
    }
    prev = el->prev;
    next = el->next;
    ret_el = next;
    if( ret_el == NULL ) {
        ret_el = prev;
    }

/* If it's the first memo, set a new MemoHead value.
 */
    if( prev == NULL ) {
        MemoHead = next;
        if( next != NULL ) {
            next->prev = NULL;
        }
    } else {
        prev->next = next;
        if( next != NULL ) {
            next->prev = prev;
        }
    }
    DisposeMemo( el );
    return( ret_el );
}

static MEMO_EL * DoUpAction( MEMO_EL * el )
/*****************************************/

/* Perform the UP action, including displaying the memo.
 */
{
    if( el == NULL ) {
        DisplayTop();
    } else {
        el = el->prev;
        DisplayMemo( el );
    }
    return( el );
}

static MEMO_EL * DoDownAction( MEMO_EL * el )
/*******************************************/

/* Perform the DOWN action, including displaying the memo.
 */
{
    MEMO_EL * next_el;

    next_el = (el == NULL) ? MemoHead : el->next;
    if( next_el == NULL ) {
        printf( "No more memos.\n" );
    } else {
        el = next_el;
        DisplayMemo( el );
    }
    return( el );
}

static MEMO_EL * ShowTodaysMemos( void )
/**************************************/

/* Show all memos that either:
 * (1) match today's date
 * (2) don't have a date stored.
 * Return a pointer to the last displayed memo.
 */
{
    MEMO_EL * el;
    MEMO_EL * last_el;
    time_t    timer;
    struct tm ltime;
    char      date[9];

/* Get today's time in YY/MM/DD format.
 */
    time( &timer );
    ltime = *localtime( &timer );
    strftime( date, 9, "%y/%m/%d", &ltime );
    last_el = NULL;
    for( el = MemoHead; el != NULL; el = el->next ) {
        if( el->date[0] == NULLCHAR
                ||  strcmp( date, el->date ) == 0 ) {
            DisplayMemo( el );
            last_el = el;
        }
    }
    return( last_el );
}

static void DisplayMemo( MEMO_EL * el )
/*************************************/

/* Display a memo on the screen.
 */
{
    TEXT_LINE * tline;

    if( el == NULL ) {
        DisplayTop();
        return;
    }
    if( el->date[0] == NULLCHAR ) {
        printf( "\nUndated memo\n" );
    } else {
        printf( "\nDated: %s\n", el->date );
    }
    for( tline = el->text; tline != NULL; tline = tline->next ) {
        printf( "    %s\n", tline->text );
    }
}

static int SaveMemos( void )
/**************************/

/* Save the memos to the memos file.
 */
{
    FILE *      fid;
    MEMO_EL *   el;
    TEXT_LINE * tline;
    char        buffer[20];

    if( MemoHead == NULL ) {
        printf( "No memos to save.\n" );
        return( FALSE );
    }

/* Open a temporary filename in case something goes wrong
 * during the save.
 */
    fid = fopen( TempName, "w" );
    if( fid == NULL ) {
        printf( "Unable to open \"%s\" for writing.\n", TempName );
        printf( "Save not performed.\n" );
        return( FALSE );
    }
    for( el = MemoHead; el != NULL; el = el->next ) {
        sprintf( buffer, "Date:%s", el->date );
        if( !WriteLine( buffer, fid ) ) {
            return( FALSE );
        }
        tline = el->text;
        for( ; tline != NULL; tline = tline->next ) {
            if( !WriteLine( tline->text, fid ) ) {
                return( FALSE );
            }
        }
        if( !WriteLine( "====", fid ) ) {
            return( FALSE );
        }
    }

/* Now get rid of the old file, if it's there, then rename
 * the new one.
 */
    fclose( fid );
    fid = fopen( FileName, "r" );
    if( fid != NULL ) {
        fclose( fid );
        if( remove( FileName ) != 0 ) {
            perror( "Can't remove old memos file" );
            return( FALSE );
        }
    }
    if( rename( TempName, FileName ) != 0 ) {
        perror( "Can't rename new memos file" );
        return( FALSE );
    }
    return( TRUE );
}

static int WriteLine( char * text, FILE * fid )
/*********************************************/
{
    if( fprintf( fid, "%s\n", text ) < 0 ) {
        perror( "Error writing memos file" );
        return( FALSE );
    }
    return( TRUE );
}

/* Routines for displaying HELP and other simple text.
 */

static void Usage( void )
/***********************/
{
    printf( "Usage:\n" );
    printf( "    memos ?\n" );
    printf( "         displays this text\n" );
    printf( "  or\n" );
    printf( "    memos\n" );
    printf( "         prompts for all actions.\n" );
    printf( "  or\n" );
    printf( "    memos action\n" );
    printf( "         performs the action.\n" );
    printf( "         More than one action may be specified.\n" );
    printf( "         action is one of:\n" );
    ShowActions();
}

static void ShowActions( void )
/*****************************/
{
    printf( "            Help    (display this text)\n" );
    printf( "            Add     (add new memo here)\n" );
    printf( "            DELete  (delete current memo)\n" );
    printf( "            REPlace (replace current memo)\n" );
    printf( "            SHow    (show the current memo again)\n" );
    printf( "            Up      (move up one memo)\n" );
    printf( "            Down    (move down one memo)\n" );
    printf( "            TOP     (move to the top of the list\n" );
    printf( "            TODay   (display today's memos)\n" );
    printf( "            SAve    (write the memos to disk)\n" );
}

static void Help( void )
/**********************/
{
    printf( "Choose one of:\n" );
    ShowActions();
    printf( "            Quit\n" );
}

static void DisplayTop( void )
/****************************/
{
    printf( "Top of memos.\n" );
}

static int WantToQuit( void )
/***************************/

/* Check to see if the memos have been modified, but not saved.
 * If so, query the user to make sure that he/she wants to quit
 * without saving the memos.
 */
{
    char buffer[MAXLEN];

    if( !MemosModified  ||  MemoHead == NULL ) {
        return( TRUE );
    }
    printf( "\nThe memos have been modified but not saved.\n" );
    printf( "Do you want to leave without saving them?\n" );
    gets( buffer );
    return( tolower( buffer[0] ) == 'y' );
}

static void BadFormat( void )
/***************************/
{
    printf( "Invalid format for memos file\n" );
    abort();
}

static void * MemoMAlloc( int size )
/**********************************/

/* Allocate the specified size of memory, dealing with the
 * case of a failure by displaying a message and quitting.
 */
{
    register char * mem;

    mem = malloc( size );
    if( mem == NULL ) {
        printf( "Unable to allocate %d characters of memory\n",
                size );
        abort();
    }
    return( mem );
}

static void DisposeMemo( MEMO_EL * el )
/*************************************/

/* Dispose of a memo, including its lines.
 */
{
    TEXT_LINE * tline;
    TEXT_LINE * next;

    tline = el->text;
    while( tline != NULL ) {
        next = tline->next;
        free( tline );
        tline = next;
    }
    free( el );
}
