.func fnmatch
.synop begin
#include <fnmatch.h>
int fnmatch( const char *pattern,
             const char *string, int flags );
.synop end
.*
.desc begin
The
.id &func.
function checks the string specified by the
.arg string
argument to see if it matches the pattern specified by the
.arg pattern
argument.
.np
The
.arg flag
argument is a bitwise inclusive OR of the bits described below. It
modifies the interpretation of
.arg pattern
and
.arg string
.ct .li .
.begterm 6
.termhd1 Flag
.termhd2 Meaning
.term FNM_PATHNAME
If set, a path separator in
.arg string
is explicitly matched by a slash in
.arg pattern
.ct .li .
It isn't matched by either the asterisk or question mark special characters,
or by a bracket expression.
.term FNM_PERIOD
If set, a leading period in
.arg string
matches a period in
.arg pattern
.ct , where the definition of "leading" depends on FNM_PATHNAME:
.begbull $compact
.bull
If FNM_PATHNAME is set, a period is leading if it's the first character in
.arg string
.ct , or if it immediately follows a path separator.
.bull
If FNM_PATHNAME isn't set, a period is leading only if it's
the first character in
.arg string
.ct .li .
.endbull
.term FNM_NOESCAPE
If set, disables backslash escaping:
.begbull $compact
.bull
If FNM_NOESCAPE isn't set in
.arg flags
.ct , a backslash character (\) in
.arg pattern
followed by any other character matches that second character in
.arg string
.ct .li .
In particular, \\ matches a backslash in
.arg string
.ct .li .
.bull
If FNM_NOESCAPE is set, a backslash character is treated as an
ordinary character.
.endbull
.term FNM_IGNORECASE
If set, the matching is case-insensitive.
.term FNM_CASEFOLD
A synonym for FNM_IGNORECASE.
.term FNM_LEADING_DIR
If set, the final path separator and any following characters in
.arg string
are ignored during matching.
.endterm
.np
A pattern-matching special character that is quoted is a pattern that matches
the special character itself. When not quoted, such special characters have
special meaning in the specification of patterns. The pattern-matching
special characters and the contexts in which they have their special meaning
are as follows:
.begnote
.note ?
a ? is a pattern that matches any printable or nonprintable character
except <newline>.
.note *
the * matches any string, including the null string.
.note [br_exp]
a pattern that matches a single character as per Regular Expression Bracket
Expressions (1003.2 2.9.1.2) except that
.begbull $compact
.bull
The exclamation point character (!) replaces the circumflex character (^)
in its role as a nonmatching list in the regular expression notation.
.bull
The backslash is used as an escape character within bracket expressions.
.endbull
.endnote
The
.mono ?,
.mono *
and
.mono [
characters aren't special when used inside a
bracket expression.
.np
The concatenation of patterns matching a single character is a valid pattern
that matches the concatenation of the single characters matched by each of
the concatenated patterns. For example, the pattern
.mono a[bc]
matches the strings
.mono ab
and
.mono ac.
.np
The concatenation of one or more patterns matching a single character with
one or more asterisks (*) is a valid pattern. In such patterns, each asterisk
matches a string of zero or more characters, up to the first character that
matches the character following the asterisk in the pattern. For example,
the pattern
.mono a*d
matches the strings
.mono ad,
.mono abd,
and
.mono abcd,
but not the string
.mono abc.
.np
When asterisk is the first or last character in a pattern, it matches zero
or more characters that precede or follow the characters matched by the
remainded of the pattern. For example, the pattern
.mono a*d*
matches the strings
.mono ad,
.mono abcd,
.mono abcdef,
.mono aaaad
and
.mono adddd.
The pattern
.mono *a*d
matches the strings
.mono ad,
.mono abcd,
.mono efabcd,
.mono aaaad
and
.mono adddd.
.desc end
.*
.return begin
The
.id &func.
function returns zero when
.arg string
matches the pattern specified by 
.arg pattern
.ct .li .
If there is no match, FNM_NOMATCH is returned. If an error occurs, &func
returns another non-zero value.
.return end
.*
.exmp begin
#include <stdio.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <limits.h>
.exmp break
int main( int argc, char **argv )
{
    int     i;
    char    buffer[PATH_MAX+1];
.exmp break
    while( gets( buffer ) ) {
        for( i = 1; i < argc; i++ ) {
            if( fnmatch( argv[i], buffer, 0 ) == 0 ) {
                printf( "'%s' matches pattern '%s'\n",
                        buffer, argv[i] );
                break;
            }
        }
    }
    return( EXIT_SUCCESS );
}
.exmp end
.class POSIX 1003.2
.system
