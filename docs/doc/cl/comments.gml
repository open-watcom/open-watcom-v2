.ix 'comment'
.pp
A
.us comment
is identified by
.mono /*
followed by any characters and terminated by
.mono */.
Comments are recognized anywhere in a program, except inside a
character constant or string.
Once the
.mono /*
is found, characters
are examined only until the
.mono */
is found.
This excludes nesting
of comments.
.pp
A comment is treated as a "white-space"
character, meaning that it is like a space character.
.pp
For example, the program fragment,
.code begin
/* Close all the files.
 */
    for( i = 0; i < fcount; i++ ) { /* loop through list */
        fclose( flist[i] );         /* close the file */
    }
.code end
..sk 1 c
is equivalent to,
.code begin
    for( i = 0; i < fcount; i++ ) {
        fclose( flist[i] );
    }
.code end
.pp
Comments are sometimes used to temporarily remove a section of code
during testing or debugging of a program.
For example, the second program fragment could be "commented out" as
follows:
.millust begin
/*
    for( i = 0; i < fcount; i++ ) {
        fclose( flist[i] );
    }
*/
.millust end
.pc
This technique will not work on the first fragment because it contains
comments, and comments may not be nested.
For these cases, the
.kwpp #if
directive of the C preprocessor may be used.
Refer to the chapter "The Preprocessor" for more details.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
The &wcboth. compilers support an extension for comments.
The symbol
.mono //
can be used at any point in a physical source line
(except inside a character constant or string literal).
Any characters from the
.mono //
to the end of the line
are treated as comment characters.
The comment is terminated by the end of the line.
There is no explicit symbol for terminating the comment.
For example, the program fragment used at the beginning of this section
can be rewritten as,
.code begin
// Close all the files.

    for( i = 0; i < fcount; i++ ) { // loop through list
        fclose( flist[i] );         // close the file
    }
.code end
.pp
This form of comment can be used to "comment out" code without
the difficulties encountered with
.mono /*.
.* .pp
.* Note that this extension may not be portable to other C compilers.
.shade end
..do end
.*
.************************************************************************
.*
