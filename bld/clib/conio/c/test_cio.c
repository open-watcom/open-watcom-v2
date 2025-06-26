/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025      The Open Watcom Contributors. All Rights Reserved.
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


/*
    This program tests all the console i/o functions.
    Without the _NO_IO_REDIRECTION_ define, one should be able to execute:
        "test_cio > nul < nul" without losing any information.
*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>


#define NUMFUNC 9

/*
    Note: if _NO_IO_REDIRECTION_ is defined, i/o redirection should not
          be used on the test program. This symbol is set in case cgets(),
          getche(), or cputs() is/are disfunctional.
*/

void wait( void ) // Waits for <Enter>.
{
    char buff[20];

#if defined(_NO_IO_REDIRECTION_)
    gets( buff );
#else
    *buff = 4;
    cgets( buff);
#endif
}

int oneLetter( void ) // Returns a letter. Used in getting answers.
{
    int c;

#if defined(_NO_IO_REDIRECTION_)
    char buff[20];

    gets( buff );
    c = *(unsigned char *)buff;
#else
    c = getche();
    dispStr( "\r\n" );
#endif
    return( c );
}

void dispStr( char *string ) // Used to display messages to console.
{
#if defined(_NO_IO_REDIRECTION_)
    fprintf( stdout, string );
    fflush( stdout );
#else
    cputs( string );
#endif
}

int main( void )
{
    bool error;
    bool errFlags[NUMFUNC];
    char *funcName[] = { "cgets()",
                         "cprintf()",
                         "cputs()",
                         "getche()",
                         "getch()",
                         "kbhit()",
                         "putch()",
                         "ungetch()",
                         "cscanf()" };
    char buffer[83], str1[80], str2[80];
    int num1, num2;
    double fnum1;

    buffer[0] = 11;
    dispStr( "Start of cgets()...\r\n" );
    sprintf( str1, "Note: buffer[0] = %d.\r\n", buffer[0] );
    dispStr( str1 );
    dispStr( "Type the following (no quotes): \"12345\"\r\n" );
    cgets( buffer );
    sprintf( str1, "\r\nCharacter count = %d.\r\n", buffer[1] );
    dispStr( str1 );
    dispStr( "\r\nVerify: input string " );
    dispStr( "was (between the arrows):\r\n" );
    sprintf( str1, "-->%s<--\r\n", &buffer[2] );
    dispStr( str1 );
    dispStr( "Note: there shouldn't be any CR/LF contained in the string.\r\n" );
    errFlags[0] = ( strcmp( &buffer[2], "12345" ) != 0 );
    dispStr( "Type the following (no quotes): \"123456789012345\"\r\n" );
    dispStr( "Note: only the first 10 characters should be allowed.\r\n" );
    cgets( buffer );
    sprintf( str1, "\r\nCharacter count = %d.\r\n", buffer[1] );
    dispStr( str1 );
    dispStr( "\r\nVerify: input string (first 10 characters) " );
    dispStr( "was (between the arrows):\r\n" );
    sprintf( str1, "-->%s<--\r\n", &buffer[2] );
    dispStr( str1 );
    dispStr( "Note: there shouldn't be any CR/LF contained in the string.\r\n" );
    if( !errFlags[0] ) {
        errFlags[0] = ( strcmp( &buffer[2], "1234567890" ) != 0 );
    }
    dispStr( "Type the following: \"\" (just press <Enter>)\r\n" );
    cgets( buffer );
    sprintf( str1, "\r\nCharacter count = %d.\r\n", buffer[1] );
    dispStr( str1 );
    dispStr( "\r\nVerify: input string " );
    dispStr( "was (between the arrows):\r\n" );
    sprintf( str1, "-->%s<--\r\n", &buffer[2] );
    dispStr( str1 );
    dispStr( "Note: there shouldn't be anything contained in the string.\r\n" );
    if( !errFlags[0] ) {
        errFlags[0] = ( strcmp( &buffer[2], "" ) != 0 );
    }
    dispStr( "End of cgets() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of cprintf()...\r\n" );
    dispStr( "Compare the following:\r\n" );
    dispStr( "-----------------------------------------\r\n" );
    dispStr( "|-Start-> Testing <-|\r\n" );
    dispStr( "                     |-> 12345.67 <-End-|\r\n" );
    dispStr( "-----------------------------------------\r\n" );
    dispStr( "with the following. " );
    dispStr( "If cprintf() is ok,\r\nthey should be identical:\r\n" );
    strcpy( buffer, "Testing" );
    fnum1 = 12345.666;
    dispStr( "-----------------------------------------\r\n" );
    cprintf( "|-Start-> %s <-|\n|-> %8.2lf <-End-|\n\r",buffer, fnum1);
    dispStr( "-----------------------------------------\r\n" );
    dispStr( "Note: there shouldn't be any blank lines in between.\r\n" );
    dispStr( "Does cprintf() seem to work properly (y/n) ?" );
    *buffer = oneLetter();
    errFlags[1] = ( *strlwr( buffer ) != 'y' );
    dispStr( "End of cprintf() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of cputs()...\r\n" );
    dispStr( "You should see \"|-->Testing<--|\" on next line:\r\n" );
    strcpy( buffer, "|-->" );
    cputs( buffer );
    strcpy( buffer, "Testing" );
    cputs( buffer );
    strcpy( buffer, "<--" );
    cputs( buffer );
    dispStr( "|\r\nDoes cputs() seem to work properly (y/n) ?" );
    *buffer = oneLetter();
    errFlags[2] = ( *strlwr( buffer ) ) != 'y' );
    dispStr( "End of cputs() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of getche()...\r\n" );
    dispStr( "Press the key 'a':\r\n" );
    num1 = getche();
    dispStr( "\r\nthe character should have been echoed back.\r\n" );
    sprintf( buffer, "Check: you pressed ->%c<- (value=%d)\r\n", num1, num1 );
    dispStr( buffer );
    errFlags[3] = ( num1 != 'a' );
    dispStr( "End of getche() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of getch()...\r\n" );
    dispStr( "Press the key 'b':\r\n" );
    num1 = getch();
    dispStr( "\r\nNothing should have been echoed back.\r\n" );
    sprintf( buffer, "Check: you pressed ->%c<- (value=%d)\r\n", num1, num1 );
    dispStr( buffer );
    errFlags[4] = ( num1 != 'b' );
    dispStr( "End of getch() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of kbhit()...\r\n" );
    dispStr( "Waiting for user to press the key 'c' (looping):\r\n" );
    for( ; !kbhit(); );
    num1 = getch();
    dispStr( "getch() was used to obtain the character pressed.\r\n" );
    sprintf( buffer, "Check: you pressed ->%c<- (value=%d).\r\n", num1, num1 );
    dispStr( buffer );
    errFlags[5] = ( num1 != 'c' );
    dispStr( "End of kbhit() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of putch()...\r\n" );
    dispStr( "You should see:\r\n--\r\nH\r\n i\r\n--\r\nin the next four lines:\r\n" );
    dispStr( "--\r\n");
    num1 = 'H';
    num2 = 'i';
    putch( num1 );
    putch( '\n' );
    putch( num2 );
    putch( '\r' );
    putch( '\n' );
    dispStr( "--\r\n");
    dispStr( "Does putch() seem to work properly (y/n) ?" );
    *buffer = oneLetter();
    errFlags[6] = ( *strlwr( buffer ) != 'y' );
    dispStr( "End of putch() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of ungetch()...\r\n" );
    dispStr( "Note: getch() will be used to get " );
    dispStr( "a character from the console.\r\n" );
    dispStr( "      Ignore this test if getch() is disfunctional.\r\n" );
    dispStr( "Waiting for user to press the key 'd':\r\n" );
    num1 = getch();
    sprintf( buffer, "Received the key ->%c<-\r\n",num1 );
    dispStr( buffer );
    num2 = ungetch( num1 );
    sprintf( buffer, "The character ->%c<- has been ungetch()'ed.\r\n", num1 );
    dispStr( buffer );
    dispStr( "Now, the character will once again be obtained using ");
    dispStr( "getch().\r\n" );
    num2 = getch();
    sprintf( buffer, "The character above ->%c<- was the one ", num2 );
    dispStr( buffer );
    dispStr( "being pushed back.\r\n" );
    errFlags[7] = ( num1 != num2 || num1 != 'd' );
    dispStr( "End of ungetch() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "\r\nStart of cscanf()...\r\n" );
    dispStr( "Input the following: 'test1 test2 1234 1.45'\r\n" );
    cscanf( "%s %s %d %lf", str1, str2, &num1, &fnum1 );
    getch();    // Discard the ending character that's left over
    dispStr( "\r\nThis is what you entered:\r\n" );
    sprintf( buffer, "\"%s, %s, %d, %4.2lf\"\r\n", str1, str2, num1, fnum1 );
    dispStr( buffer );
    errFlags[8] = ( strcmp( str1, "test1" ) != 0 || strcmp( str2, "test2" ) != 0 || num1 != 1234 || fnum1 != 1.45 );
    dispStr( "End of cscanf() test. Press <Enter> to continue.\r\n" );
    wait();

    dispStr( "End of all tests.\r\n\r\n" );
    error = false;
    for( num1 = 0; num1 < NUMFUNC; ++num1 ) {
        if( errFlags[num1] ) {
            sprintf( buffer, "%s\r\n", funcName[num1] );
            dispStr( buffer );
            error = true;
        }
    }
    dispStr( "\r\n" );
    if( error ) {
        dispStr( "This program has found unexpected result(s) when testing" );
        dispStr( "\r\nthe above function(s). Reasons could be that the\r\n" );
        dispStr( "function(s) being used in the test(s) might have" );
        dispStr( " problems.\r\n" );
    } else {
        dispStr( "All the functions seem to pass the test!\r\n" );
    }
    return( EXIT_SUCCESS );
}
