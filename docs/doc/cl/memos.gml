.code begin
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

.code break
static const char FileName[] = { "memos.db" };
static const char TempName[] = { "tempmemo.db" };

static MEMO_EL *  MemoHead      = NULL;
static int        MemosModified = FALSE;
static int        QuitFlag      = TRUE;

.code break
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

.code break
/* This table maps action keywords onto the "actions" defined
 * above. The table also defines short forms for the keywords.
 */
typedef struct {
    ACTION act;
    char * keyword;
} ACTION_MAP;

.code break
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

.code break
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

.code break
extern int main( int argc, char * argv[] )
/****************************************/
{
    int       index;
    MEMO_EL * el;

    printf( "Memo facility\n" );

.code break
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

.code break
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

.code break
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
.code break
    if( fid == NULL ) {
        printf( "Memos file not found."
                " Starting with no memos.\n" );
        return;
    }

.code break
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
.code break
        if( prev_el == NULL ) {
            MemoHead = new_el;
            new_el->prev = NULL;
        } else {
            prev_el->next = new_el;
            new_el->prev = prev_el;
        }
.code break
        new_el->next = NULL;
        prev_el = new_el;
    }
}

.code break
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
.code break
    return( strlen( buffer ) - 1 );
}

.code break
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

.code break
/* First line must be of the form "Date:" or "Date:YY/MM/DD":
 */
    if( (len != 5  &&  len != 13)
            ||  strncmp( buffer, "Date:", 5 ) != 0 ) {
        BadFormat();
    }
.code break
    buffer[len] = NULLCHAR;
    el = MemoMAlloc( sizeof( MEMO_EL ) );
    el->text = NULL;
    strcpy( el->date, buffer + 5 );
    line = NULL;
.code break
    for( ;; ) {
        len = ReadLine( buffer, MAXLEN, fid );
        if( len == EOF ) {
            BadFormat();
        }
.code break
        buffer[len] = NULLCHAR;
        if( strcmp( buffer, "====" ) == 0 ) {
            return( el );
        }
        line = AddLine( buffer, el, line );
    }
}

.code break
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
.code break
    if( prevline == NULL ) {
        el->text = line;
    } else {
        prevline->next = line;
    }
    return( line );
}

.code break
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
.code break
        if( act != INVALID ) {
            return( act );
        }
        printf( "\nThat selection was not valid.\n" );
        Help();
    }
}

.code break
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
.code break
    return( GetAction( buffer ) );
}

.code break
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
.code break
    for( actmap = KeywordMap; actmap->act != INVALID; ++actmap ) {
        if( strcmp( buffer, actmap->keyword ) == 0 ) break;
    }
    return( actmap->act );
}

.code break
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

.code break
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
.code break
      case ADD:
        new_el = AddMemo( el );
        if( new_el != NULL ) {
            el = new_el;
            MemosModified = TRUE;
        }
        break;
.code break
      case DELETE:
        el = DeleteMemo( el );
        MemosModified = TRUE;
        break;
.code break
      case REPLACE:
        prev_el = el;
        new_el = AddMemo( el );
        if( new_el != NULL ) {
            DeleteMemo( prev_el );
            MemosModified = TRUE;
        }
        break;
.code break
      case SHOW:
        DisplayMemo( el );
        break;
.code break
      case UP:
        el = DoUpAction( el );
        break;
.code break
      case DOWN:
        el = DoDownAction( el );
        break;
.code break
      case TOP:
        el = NULL;
        break;
.code break
      case TODAY:
        el = ShowTodaysMemos();
        break;
.code break
      case SAVE:
        if( SaveMemos() ) {
            MemosModified = FALSE;
        }
        break;
.code break
      case QUIT:
        if( WantToQuit() ) {
            QuitFlag = TRUE;
            el = NULL;
        }
    }
.code break
    return( el );
}

.code break
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
.code break
    if( el == NULL ) {
        next = MemoHead;
        MemoHead = new_el;
    } else {
        next = el->next;
        el->next = new_el;
    }
.code break
    new_el->prev = el;
    new_el->next = next;
    if( next != NULL ) {
        next->prev = new_el;
    }
    return( new_el );
}

.code break
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
.code break
    if( gets( buffer ) == NULL ) {
        printf( "Error reading from terminal.\n" );
        return( NULL );
    }
.code break
    len = strlen( buffer );
.code break
    if( len != 0
            &&  (len != 8
                ||  buffer[2] != '/'
                || buffer[5] != '/') ) {
        printf( "Date is not valid.\n" );
        return( NULL );
    }
.code break
    el = MemoMAlloc( sizeof( MEMO_EL ) );
    el->text = NULL;
.code break
    strcpy( el->date, buffer );
    line = NULL;
.code break
    printf( "\nEnter the text of the memo.\n" );
    printf( "To terminate the memo,"
            " enter a line starting with =\n" );
.code break
    for( ;; ) {
        if( gets( buffer ) == NULL ) {
            printf( "Error reading from terminal.\n" );
            return( NULL );
        }
.code break
        if( buffer[0] == '=' ) {
            return( el );
        }
        line = AddLine( buffer, el, line );
    }
}

.code break
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
.code break
    prev = el->prev;
    next = el->next;
    ret_el = next;
    if( ret_el == NULL ) {
        ret_el = prev;
    }

.code break
/* If it's the first memo, set a new MemoHead value.
 */
    if( prev == NULL ) {
        MemoHead = next;
        if( next != NULL ) {
            next->prev = NULL;
        }
.code break
    } else {
        prev->next = next;
        if( next != NULL ) {
            next->prev = prev;
        }
    }
.code break
    DisposeMemo( el );
    return( ret_el );
}

.code break
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

.code break
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

.code break
static MEMO_EL * ShowTodaysMemos( void )
/**************************************/

/* Show all memos that either:
 * (1) match today's date
 * (2) don't have a date stored.
 * Return a pointer to the last displayed memo.
 */
.code break
{
    MEMO_EL * el;
    MEMO_EL * last_el;
    time_t    timer;
    struct tm ltime;
    char      date[9];

.code break
/* Get today's time in YY/MM/DD format.
 */
    time( &timer );
    ltime = *localtime( &timer );
.code break
    strftime( date, 9, "%y/%m/%d", &ltime );
    last_el = NULL;
.code break
    for( el = MemoHead; el != NULL; el = el->next ) {
        if( el->date[0] == NULLCHAR
                ||  strcmp( date, el->date ) == 0 ) {
            DisplayMemo( el );
            last_el = el;
        }
    }
    return( last_el );
}

.code break
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
.code break
    if( el->date[0] == NULLCHAR ) {
        printf( "\nUndated memo\n" );
    } else {
        printf( "\nDated: %s\n", el->date );
    }
.code break
    for( tline = el->text; tline != NULL; tline = tline->next ) {
        printf( "    %s\n", tline->text );
    }
}

.code break
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

.code break
/* Open a temporary filename in case something goes wrong
 * during the save.
 */
    fid = fopen( TempName, "w" );
    if( fid == NULL ) {
        printf( "Unable to open \"%s\" for writing.\n", TempName );
        printf( "Save not performed.\n" );
        return( FALSE );
    }
.code break
    for( el = MemoHead; el != NULL; el = el->next ) {
        sprintf( buffer, "Date:%s", el->date );
        if( !WriteLine( buffer, fid ) ) {
            return( FALSE );
        }
        tline = el->text;
.code break
        for( ; tline != NULL; tline = tline->next ) {
            if( !WriteLine( tline->text, fid ) ) {
                return( FALSE );
            }
        }
.code break
        if( !WriteLine( "====", fid ) ) {
            return( FALSE );
        }
    }

.code break
/* Now get rid of the old file, if it's there, then rename
 * the new one.
 */
    fclose( fid );
    fid = fopen( FileName, "r" );
.code break
    if( fid != NULL ) {
        fclose( fid );
        if( remove( FileName ) != 0 ) {
            perror( "Can't remove old memos file" );
            return( FALSE );
        }
    }
.code break
    if( rename( TempName, FileName ) != 0 ) {
        perror( "Can't rename new memos file" );
        return( FALSE );
    }
    return( TRUE );
}

.code break
static int WriteLine( char * text, FILE * fid )
/*********************************************/
{
    if( fprintf( fid, "%s\n", text ) < 0 ) {
        perror( "Error writing memos file" );
        return( FALSE );
    }
    return( TRUE );
}

.code break
/* Routines for displaying HELP and other simple text.
 */

.code break
static void Usage( void )
/***********************/
{
    printf( "Usage:\n" );
    printf( "    memos ?\n" );
    printf( "         displays this text\n" );
.code break
    printf( "  or\n" );
.code break
    printf( "    memos\n" );
    printf( "         prompts for all actions.\n" );
.code break
    printf( "  or\n" );
.code break
    printf( "    memos action\n" );
    printf( "         performs the action.\n" );
    printf( "         More than one action may be specified.\n" );
    printf( "         action is one of:\n" );
    ShowActions();
}

.code break
static void ShowActions( void )
/*****************************/
{
    printf( "            Help    (display this text)\n" );
.code break
    printf( "            Add     (add new memo here)\n" );
.code break
    printf( "            DELete  (delete current memo)\n" );
.code break
    printf( "            REPlace (replace current memo)\n" );
.code break
    printf( "            SHow    (show the current memo again)\n" );
.code break
    printf( "            Up      (move up one memo)\n" );
.code break
    printf( "            Down    (move down one memo)\n" );
.code break
    printf( "            TOP     (move to the top of the list\n" );
.code break
    printf( "            TODay   (display today's memos)\n" );
.code break
    printf( "            SAve    (write the memos to disk)\n" );
}

.code break
static void Help( void )
/**********************/
{
    printf( "Choose one of:\n" );
    ShowActions();
    printf( "            Quit\n" );
}

.code break
static void DisplayTop( void )
/****************************/
{
    printf( "Top of memos.\n" );
}

.code break
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
.code break
    printf( "\nThe memos have been modified but not saved.\n" );
    printf( "Do you want to leave without saving them?\n" );
.code break
    gets( buffer );
    return( tolower( buffer[0] ) == 'y' );
}

.code break
static void BadFormat( void )
/***************************/
{
    printf( "Invalid format for memos file\n" );
    abort();
}

.code break
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

.code break
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
.code end
