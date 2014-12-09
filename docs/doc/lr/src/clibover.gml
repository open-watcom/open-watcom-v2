.im cfunmacs
.*
.chap C Library Overview
.*
.np
The C library provides much of the power usually associated with the C
language.
This chapter introduces the individual functions (and macros) that
comprise the &company C library.
The chapter
.us Library Functions and Macros
describes each function and macro in complete detail.
.np
Library functions are called as if they had been defined within the
program.
When the program is linked, the code for these routines is incorporated
into the program by the linker.
.np
Strictly speaking, it is not necessary to declare most library
functions since they return
.id int
values for the most part.
It is preferred, however, to declare all functions by including the
header files found in the synopsis section with each function.
Not only does this declare the return value, but also the type
expected for each of the arguments as well as the number of arguments.
This enables the &company C and C++ compilers to check the arguments
coded with each function call.
.*======================================================================
.section Classes of Functions
.*
.np
.ix 'function classification'
The functions in the &company C library can be organized into a number
of classes:
.begnote
.*
.note Character Manipulation Functions
.sk 0
These functions deal with single characters.
.*
.note Wide Character Manipulation Functions
.sk 0
These functions deal with wide characters.
.*
.note Multibyte Character Manipulation Functions
.sk 0
These functions deal with multibyte characters.
.*
.note Memory Manipulation Functions
.sk 0
These functions manipulate blocks of memory.
.*
.note String Manipulation Functions
.sk 0
These functions manipulate strings of characters.
A character string is an array of zero or more adjacent characters
followed by a null character
.mono ('\0')
which marks the end of the string.
.*
.note Wide String Manipulation Functions
.sk 0
These functions manipulate strings of wide characters.
A wide character string is an array of zero or more adjacent wide
characters followed by a null wide character
.mono (L'\0')
which marks the end of the wide string.
.*
.note Multibyte String Manipulation Functions
.sk 0
These functions manipulate strings of multibyte characters.
A multibyte character is either a single-byte or double-byte
character.
The Chinese, Japanese and Korean character sets are examples of
character sets containing both single-byte and double-byte characters.
.np
What determines whether a character is a single-byte or double-byte
character is the value of the lead byte in the sequence.
For example, in the Japanese DBCS (double-byte character set),
double-byte characters are those in which the first byte falls
in the range 0x81 - 0x9F or 0xE0 - 0xFC and the second byte falls in
the range 0x40 - 0x7E or 0x80 - 0xFC.
A string of multibyte characters must be scanned from the first byte
(index 0) to the last byte (index n) in sequence in order to determine
if a particular byte is part of a double-byte character.
For example, suppose that a multibyte character string contains the
following byte values.
.millust begin
0x31 0x40 0x41 0x81 0x41 // "1@A.." where .. is a DB char
.millust end
.pc
Among other characters, it contains the letter "A" (the first 0x41)
and a double-byte character (0x81 0x41).
The second 0x41 is not the letter "A" and that could only be
determined by scanning from left to right starting with the first byte
(0x31).
.*
.note Conversion Functions
.sk 0
These functions convert values from one representation to another.
Numeric values, for example, can be converted to strings.
.*
.note Memory Allocation Functions
.sk 0
These functions are concerned with allocating and deallocating memory.
.*
.note Heap Functions
.sk 0
These functions provide the ability to shrink and grow the heap,
as well as, find heap related problems.
.*
.note Math Functions
.sk 0
The mathematical functions perform mathematical computations
such as the common trigonometric calculations.
These functions operate on
.id double
values, also known as floating-point values.
.*
.note Searching Functions
.sk 0
These functions provide searching and sorting capabilities.
.*
.note Time Functions
.sk 0
These functions provide facilities to obtain and manipulate
times and dates.
.*
.note Variable-length Argument Lists
.sk 0
These functions provide the capability to process a variable number of
arguments to a function.
.*
.note Stream I/O Functions
.sk 0
These functions provide the "standard" functions to read and write
files.
Data can be transmitted as characters, strings, blocks of memory or
under format control.
.*
.note Wide Character Stream I/O Functions
.sk 0
These functions provide the "standard" functions to read and write
files of wide characters.
Data can be transmitted as wide characters, wide character strings,
blocks of memory or under format control.
.*
.note Process Primitive Functions
.sk 0
.if '&machsys' eq 'QNX' .do begin
These functions deal with process creation, execution and termination,
signal handling, and timer operations.
.do end
.el .do begin
These functions deal with process creation, execution and termination,
signal handling, and timer operations.
.do end
.*
.note Process Environment
.sk 0
These functions deal with process identification,
user identification, process groups, system identification,
system time and process time, environment variables,
terminal identification, and configurable system variables.
.*
.note Directory Functions
.sk 0
These functions provide directory services.
.*
.note Operating System I/O Functions
.sk 0
.if '&machsys' eq 'QNX' .do begin
These functions are described in the "IEEE Standard Portable Operating
System Interface for Computer Environments" (POSIX 1003.1).
The POSIX input/output functions provide the capability to perform I/O
at a "lower level" than the C Language "stream I/O" functions (e.g.,
.kw fopen
.ct,
.kw fread
.ct,
.kw fwrite
.ct,
and
.kw fclose
.ct ).
.do end
.el .do begin
These "non-standard" file operations are more primitive than the
"standard" functions in that they are directly interfaced to the
operating system.
They are included to provide compatibility with other C implementations
and to provide the capability to directly use operating-system
file operations.
.do end
.*
.note File Manipulation Functions
.sk 0
These functions operate directly on files, providing facilities such
as deletion of files.
.*
.note Console I/O Functions
.sk 0
These functions provide the capability to directly read and write
characters from the console.
.*
.note Default Windowing Functions
.sk 0
These functions provide the capability to manipulate various dialog
boxes in &company's default windowing system.
.*
.if '&machsys' eq 'QNX' .do begin
.note POSIX Realtime Timer Functions
.sk 0
These functions provide realtime timer capabilities.
.do end
.*
.if '&machsys' eq 'QNX' .do begin
.note POSIX Shared Memory Functions
.sk 0
These functions provide memory mapping capabilities.
.do end
.*
.if '&machsys' eq 'QNX' .do begin
.note POSIX Terminal Control Functions
.sk 0
These functions deal with terminal attributes such as baud rate
and terminal interface control functions.
.do end
.*
:cmt. .if '&machsys' eq 'QNX' .do begin
:cmt. .note QNX Terminal Functions
:cmt. .sk 0
:cmt. These functions provide terminal I/O control, cursor movement,
:cmt. character and line insertion/deletion, menus, saving/restoring
:cmt. portions of the screen, etc.
:cmt. .do end
.*
.if '&machsys' eq 'QNX' .do begin
.note System Database Functions
.sk 0
These functions allow an application to access group and user database
information.
.do end
.*
.if '&machsys' eq 'QNX' .do begin
.note Miscellaneous QNX Functions
.sk 0
These functions provide access to a variety of QNX functions such as
message passing.
.do end
.*
.if '&machsys' eq 'QNX' .do begin
.note QNX Low-level Functions
.sk 0
These functions provide access to low-level QNX facilities.
.do end
.*
.if '&machsys' ne 'QNX' .do begin
.if '&machsys' ne 'WIN32' .do begin
.note BIOS Functions
.sk 0
This set of functions allows access to services provided by the BIOS.
.do end
.do end
.*
.if '&machsys' ne 'QNX' .do begin
.note DOS-Specific Functions
.sk 0
This set of functions allows access to DOS-specific functions.
.do end
.*
.note Intel 80x86 Architecture-Specific Functions
.sk 0
This set of functions allows access to Intel 80x86 processor-related
functions.
.*
.note Intel Pentium Multimedia Extension Functions
.sk 0
This set of functions allows access to Intel Architecture Multimedia
Extensions (MMX).
.*
:cmt. QNX will do later .if '&machsys' eq 'QNX' .do begin
:cmt. QNX will do later .note UNIX portability Functions
:cmt. QNX will do later .sk 0
:cmt. QNX will do later A collection of headers and functions to aid in porting traditional
:cmt. QNX will do later UNIX code.
:cmt. QNX will do later .do end
.*
.note Miscellaneous Functions
.sk 0
This collection consists of the remaining functions.
.*
.note DOS LFN aware Functions
.sk 0
These functions are DOS LFN capable.
.endnote
.np
The following subsections describe these function classes in more detail.
Each function in the class is noted with a brief description of its
purpose.
The chapter
.us Library Functions and Macros
provides a complete description of each function and macro.
.*
.*
.beglevel
.*======================================================================
.section Character Manipulation Functions
.*
.np
.ix '&CharTest'
These functions operate upon single characters of type
.id char.
The functions test characters in various ways and convert them between
upper and lowercase.
.fdbeg
.fd *fun="isalnum" test for letter or digit
.fd *fun="isalpha" test for letter
.fd *fun="isascii" test for ASCII character
.fd *fun="isblank" test for blank character
.fd *fun="iscntrl" test for control character
.fd *fun="__iscsym" test for letter, underscore or digit
.fd *fun="__iscsymf" test for letter or underscore
.fd *fun="isdigit" test for digit
.fd *fun="isgraph" test for printable character, except space
.fd *fun="islower" test for letter in lowercase
.fd *fun="isprint" test for printable character, including space
.fd *fun="ispunct" test for punctuation characters
.fd *fun="isspace" test for "white space" characters
.fd *fun="isupper" test for letter in uppercase
.fd *fun="isxdigit" test for hexadecimal digit
.fd *fun="tolower" convert character to lowercase
.fd *fun="toupper" convert character to uppercase
.fdend
.*======================================================================
.section Wide Character Manipulation Functions
.*
.np
.ix '&CharTest'
.ix '&Wide'
These functions operate upon wide characters of type
.id wchar_t.
The functions test wide characters in various ways and convert them
between upper and lowercase.
.fdbeg
.fd *fun="iswalnum" test for letter or digit
.fd *fun="iswalpha" test for letter
.fd *fun="iswascii" test for ASCII character
.fd *fun="iswblank" test for blank character
.fd *fun="iswcntrl" test for control character
.fd *fun="__iswcsym" test for letter, underscore or digit
.fd *fun="__iswcsymf" test for letter or underscore
.fd *fun="iswdigit" test for digit
.fd *fun="iswgraph" test for printable character, except space
.fd *fun="iswlower" test for letter in lowercase
.fd *fun="iswprint" test for printable character, including space
.fd *fun="iswpunct" test for punctuation characters
.fd *fun="iswspace" test for "white space" characters
.fd *fun="iswupper" test for letter in uppercase
.fd *fun="iswxdigit" test for hexadecimal digit
.fd *fun="wctype" construct a property value for a given "property"
.fd *fun="iswctype" test a character for a specific property
.fd *fun="towlower" convert character to lowercase
.fd *fun="towupper" convert character to uppercase
.fd *fun="wctrans" construct mapping value for a given "property"
.fd *fun="towctrans" convert a character based on a specific property
.fdend
.*======================================================================
.section Multibyte Character Manipulation Functions
.*
.np
.ix '&CharTest'
.ix '&Multibyte'
These functions operate upon multibyte characters.
The functions test wide characters in various ways and convert them
between upper and lowercase.
.fdbeg
.fd *fun="_fmbccmp" compare one multibyte character with another
.fd *fun="_fmbccpy" copy one multibyte character from one string to another
.fd *fun="_fmbcicmp" compare one multibyte character with another (case insensitive)
.fd *fun="_fmbclen" return number of bytes comprising multibyte character
.fd *fun="_fmblen" determine length of next multibyte character
.fd *fun="_fmbgetcode" get next single-byte or double-byte character from far string
.fd *fun="_fmbputchar" store single-byte or double-byte character into far string
.fd *fun="_fmbrlen" determine length of next multibyte character
.fd *fun="_fmbrtowc" convert far multibyte character to wide character
.fd *fun="_fmbsbtype" return type of byte in multibyte character string
.fd *fun="_fmbtowc" convert far multibyte character to wide character
.fd *fun="_ismbbalnum" test for isalnum or _ismbbkalnum
.fd *fun="_ismbbalpha" test for isalpha or _ismbbkalpha
.fd *fun="_ismbbgraph" test for isgraph or _ismbbkprint
.fd *fun="_ismbbkalnum" test for non-ASCII text symbol other than punctuation
.fd *fun="_ismbbkana" test for single-byte Katakana character
.fd *fun="_ismbbkalpha" test for non-ASCII text symbol other than digits or punctuation
.fd *fun="_ismbbkprint" test for non-ASCII text or non-ASCII punctuation symbol
.fd *fun="_ismbbkpunct" test for non-ASCII punctuation character
.fd *fun="_ismbblead" test for valid first byte of multibyte character
.fd *fun="_ismbbprint" test for isprint or _ismbbkprint
.fd *fun="_ismbbpunct" test for ispunct or _ismbbkpunct
.fd *fun="_ismbbtrail" test for valid second byte of multibyte character
.fd *fun="_ismbcalnum" test for _ismbcalpha or _ismbcdigit
.fd *fun="_ismbcalpha" test for a multibyte alphabetic character
.fd *fun="_ismbccntrl" test for a multibyte control character
.fd *fun="_ismbcdigit" test for a multibyte decimal-digit character '0' through '9'
.fd *fun="_ismbcgraph" test for a printable multibyte character except space
.fd *fun="_ismbchira" test for a double-byte Hiragana character
.fd *fun="_ismbckata" test for a double-byte Katakana character
.fd *fun="_ismbcl0" test for a double-byte non-Kanji character
.fd *fun="_ismbcl1" test for a JIS level 1 double-byte character
.fd *fun="_ismbcl2" test for a JIS level 2 double-byte character
.fd *fun="_ismbclegal" test for a valid multibyte character
.fd *fun="_ismbclower" test for a valid lowercase multibyte character
.fd *fun="_ismbcprint" test for a printable multibyte character including space
.fd *fun="_ismbcpunct" test for any multibyte punctuation character
.fd *fun="_ismbcspace" test for any multibyte space character
.fd *fun="_ismbcsymbol" test for valid multibyte symbol (punctuation and other special graphics)
.fd *fun="_ismbcupper" test for valid uppercase multibyte character
.fd *fun="_ismbcxdigit" test for any multibyte hexadecimal-digit character
:cmt. .fd *fun="_ismbdalnum" test for
:cmt. .fd *fun="_ismbdalpha" test for
:cmt. .fd *fun="_ismbdcntrl" test for
:cmt. .fd *fun="_ismbddigit" test for
:cmt. .fd *fun="_ismbdgraph" test for
:cmt. .fd *fun="_ismbdlower" test for
:cmt. .fd *fun="_ismbdprint" test for
:cmt. .fd *fun="_ismbdpunct" test for
:cmt. .fd *fun="_ismbdspace" test for
:cmt. .fd *fun="_ismbdupper" test for
:cmt. .fd *fun="_ismbdxdigit" test for
.fd *fun="_mbbtombc" return double-byte equivalent to single-byte character
.fd *fun="_mbbtype" determine type of byte in multibyte character
.fd *fun="_mbccmp" compare one multibyte character with another
.fd *fun="_mbccpy" copy one multibyte character from one string to another
.fd *fun="_mbcicmp" compare one multibyte character with another (case insensitive)
.fd *fun="_mbcjistojms" convert JIS code to shift-JIS code
.fd *fun="_mbcjmstojis" convert shift-JIS code to JIS code
.fd *fun="_mbclen" return number of bytes comprising multibyte character
.fd *fun="_mbctolower" convert double-byte uppercase character to double-byte lowercase character
.fd *fun="_mbctoupper" convert double-byte lowercase character to double-byte uppercase character
.fd *fun="_mbctohira" convert double-byte Katakana character to Hiragana character
.fd *fun="_mbctokata" convert double-byte Hiragana character to Katakana character
.fd *fun="_mbctombb" return single-byte equivalent to double-byte character
.fd *fun="_mbgetcode" get next single-byte or double-byte character from string
.fd *fun="mblen" determine length of next multibyte character
.fd *fun="_mbputchar" store single-byte or double-byte character into string
.fd *fun="mbrlen" determine length of next multibyte character
.fd *fun="mbrtowc" convert multibyte character to wide character
.fd *fun="_mbsbtype" return type of byte in multibyte character string
.fd *fun="mbsinit" determine if mbstate_t object describes an initial conversion state
.fd *fun="mbtowc" convert multibyte character to wide character
.fdend
.*======================================================================
.section Memory Manipulation Functions
.*
.np
.ix 'Memory Manipulation Functions'
These functions manipulate blocks of memory.
In each case, the address of the memory block and its size is passed
to the function.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd *fun="_fmemccpy" copy far memory block up to a certain character
.fd *fun="_fmemchr" search far memory block for a character value
.fd *fun="_fmemcmp" compare any two memory blocks (near or far)
.fd *fun="_fmemcpy" copy far memory block, overlap not allowed
.fd *fun="_fmemicmp" compare far memory, case insensitive
.fd *fun="_fmemmove" copy far memory block, overlap allowed
.fd *fun="_fmemset" set any memory block (near of far) to a character
.fd *fun="memccpy" copy memory block up to a certain character
.fd *fun="memchr" search memory block for a character value
.fd *fun="memcmp" compare memory blocks
.fd *fun="memcpy" copy memory block, overlap not allowed
.fd *fun="memicmp" compare memory, case insensitive
.fd *fun="memmove" copy memory block, overlap allowed
.fd *fun="memset" set memory block to a character
.fd *fun="movedata" copy memory block, with segment information
.fd *fun="swab" swap bytes of a memory block
.fd *fun="_wmemccpy" copy memory block up to a certain character
.fd *fun="wmemchr" search memory block for a wide character value
.fd *fun="wmemcmp" compare memory blocks
.fd *fun="wmemcpy" copy memory block, overlap not allowed
.fd *fun="_wmemicmp" compare memory, case insensitive
.fd *fun="wmemmove" copy memory block, overlap allowed
.fd *fun="wmemset" set memory block to a wide character
.fdend
.np
See the section
.us "String Manipulation Functions"
for descriptions of functions that manipulate strings of data.
See the section
.us "Wide String Manipulation Functions"
for descriptions of functions that manipulate wide strings of data.
.*======================================================================
.section String Manipulation Functions
.*
.np
.ix '&String'
A
.us string
is an array of characters (with type
.id char
.ct ) that is terminated with an extra null character
.mono ('\0').
Functions are passed only the address of the string since the size
can be determined by searching for the terminating character.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd *fun="bcmp" compare two byte strings
.fd *fun="bcopy" copy a byte string
.fd *fun="_bprintf" formatted transmission to fixed-length string
.fd *fun="bzero" zero a byte string
.fd *fun="_fstrcat" concatenate two far strings
.fd *fun="_fstrchr" locate character in far string
.fd *fun="_fstrcmp" compare two far strings
.fd *fun="_fstrcpy" copy far string
.fd *fun="_fstrcspn" get number of string characters not from a set of characters
.fd *fun="_fstricmp" compare two far strings with case insensitivity
.fd *fun="_fstrlen" length of a far string
.fd *fun="_fstrlwr" convert far string to lowercase
.fd *fun="_fstrncat" concatenate two far strings, up to a maximum length
.fd *fun="_fstrncmp" compare two far strings up to maximum length
.fd *fun="_fstrncpy" copy a far string, up to a maximum length
.fd *fun="_fstrnicmp" compare two far strings with case insensitivity up to a maximum length
.fd *fun="_fstrnset" fill far string with character to a maximum length
.fd *fun="_fstrpbrk" locate occurrence of a string within a second string
.fd *fun="_fstrrchr" locate last occurrence of character from a character set
.fd *fun="_fstrrev" reverse a far string in place
.fd *fun="_fstrset" fill far string with a character
.fd *fun="_fstrspn" find number of characters at start of string which are also in a second string
.fd *fun="_fstrstr" find first occurrence of string in second string
.fd *fun="_fstrtok" get next token from a far string
.fd *fun="_fstrupr" convert far string to uppercase
.fd *fun="sprintf" formatted transmission to string
.fd *fun="sscanf" scan from string under format control
.fd *fun="strcat" concatenate string
.fd *fun="strchr" locate character in string
.fd *fun="strcmp" compare two strings
.fd *fun="strcmpi" compare two strings with case insensitivity
.fd *fun="strcoll" compare two strings using "locale" collating sequence
.fd *fun="strcpy" copy a string
.fd *fun="strcspn" get number of string characters not from a set of characters
.fd *fun="_strdec" returns pointer to the previous character in string
.fd *fun="_strdup" allocate and duplicate a string
.fd *fun="strerror" get error message as string
.fd *fun="_stricmp" compare two strings with case insensitivity
.fd *fun="_strinc" return pointer to next character in string
.fd *fun="strlcat" concatenate string into a bounded buffer
.fd *fun="strlcpy" copy string into a bounded buffer
.fd *fun="strlen" string length
.fd *fun="_strlwr" convert string to lowercase
.fd *fun="strncat" concatenate two strings, up to a maximum length
.fd *fun="strncmp" compare two strings up to maximum length
.fd *fun="_strncnt" count the number of characters in the first "n" bytes
.fd *fun="strncpy" copy a string, up to a maximum length
.fd *fun="_strnextc" return integer value of the next character in string
.fd *fun="_strnicmp" compare two strings with case insensitivity up to a maximum length
.fd *fun="_strninc" increment character pointer by "n" characters
.fd *fun="_strnset" fill string with character to a maximum length
.fd *fun="strpbrk" locate occurrence of a string within a second string
.fd *fun="strrchr" locate last occurrence of character from a character set
.fd *fun="_strrev" reverse a string in place
.fd *fun="_strset" fill string with a character
.fd *fun="strspn" find number of characters at start of string which are also in a second string
.fd *fun="_strspnp" return pointer to first character of string not in set
.fd *fun="strstr" find first occurrence of string in second string
.fd *fun="strtok" get next token from string
.fd *fun="_strupr" convert string to uppercase
.fd *fun="strxfrm" transform string to locale's collating sequence
.fd *fun="_vbprintf" same as "_bprintf" but with variable arguments
.fd *fun="vsscanf" same as "sscanf" but with variable arguments
.fdend
.np
For related functions see the sections
.us Conversion Functions
(conversions to and from strings),
.us Time Functions
(formatting of dates and times), and
.us Memory Manipulation Functions
(operate on arrays without terminating null character).
.*======================================================================
.section Wide String Manipulation Functions
.*
.np
.ix '&Wide'
.ix '&Multibyte'
A
.us wide string
is an array of wide characters (with type
.id wchar_t
.ct ) that is terminated with an extra null wide character
.mono (L'\0').
Functions are passed only the address of the string since the size
can be determined by searching for the terminating character.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd *fun="_bwprintf" formatted wide character transmission to fixed-length wcsing
:cmt. .fd *fun="_fwcscat" concatenate two far strings
:cmt. .fd *fun="_fwcschr" locate character in far string
:cmt. .fd *fun="_fwcscmp" compare two far strings
:cmt. .fd *fun="_fwcscpy" copy far string
:cmt. .fd *fun="_fwcscspn" get number of string characters not from a set of characters
:cmt. .fd *fun="_fwcsicmp" compare two far strings with case insensitivity
:cmt. .fd *fun="_fwcslen" length of a far string
:cmt. .fd *fun="_fwcslwr" convert far string to lowercase
:cmt. .fd *fun="_fwcsncat" concatenate two far strings, up to a maximum length
:cmt. .fd *fun="_fwcsncmp" compare two far strings up to maximum length
:cmt. .fd *fun="_fwcsncpy" copy a far string, up to a maximum length
:cmt. .fd *fun="_fwcsnicmp" compare two far strings with case insensitivity up to a maximum length
:cmt. .fd *fun="_fwcsnset" fill far string with character to a maximum length
:cmt. .fd *fun="_fwcspbrk" locate occurrence of a string within a second string
:cmt. .fd *fun="_fwcsrchr" locate last occurrence of character from a character set
:cmt. .fd *fun="_fwcsrev" reverse a far string in place
:cmt. .fd *fun="_fwcsset" fill far string with a character
:cmt. .fd *fun="_fwcsspn" find number of characters at start of string which are also in a second string
:cmt. .fd *fun="_fwcsstr" find first occurrence of string in second string
:cmt. .fd *fun="_fwcstok" get next token from a far string
:cmt. .fd *fun="_fwcsupr" convert far string to uppercase
.fd *fun="swprintf" formatted wide character transmission to string
.fd *fun="swscanf" scan from wide character string under format control
.fd *fun="_vbwprintf" same as "_bwprintf" but with variable arguments
.fd *fun="vswscanf" same as "swscanf" but with variable arguments
.fd *fun="wcscat" concatenate string
.fd *fun="wcschr" locate character in string
.fd *fun="wcscmp" compare two strings
.fd *fun="wcscmpi" compare two strings with case insensitivity
.fd *fun="wcscoll" compare two strings using "locale" collating sequence
.fd *fun="wcscpy" copy a string
.fd *fun="wcscspn" get number of string characters not from a set of characters
.fd *fun="_wcsdec" returns pointer to the previous character in string
.fd *fun="_wcsdup" allocate and duplicate a string
.fd *fun="wcserror" get error message as string
.fd *fun="_wcsicmp" compare two strings with case insensitivity
.fd *fun="_wcsinc" return pointer to next character in string
.fd *fun="wcslcat" concatenate string into a bounded buffer
.fd *fun="wcslcpy" copy string into a bounded buffer
.fd *fun="wcslen" string length
.fd *fun="_wcslwr" convert string to lowercase
.fd *fun="wcsncat" concatenate two strings, up to a maximum length
.fd *fun="wcsncmp" compare two strings up to maximum length
.fd *fun="_wcsncnt" count the number of characters in the first "n" bytes
.fd *fun="wcsncpy" copy a string, up to a maximum length
.fd *fun="_wcsnextc" return integer value of the next multibyte-character in string
.fd *fun="_wcsnicmp" compare two strings with case insensitivity up to a maximum length
.fd *fun="_wcsninc" increment wide character pointer by "n" characters
.fd *fun="_wcsnset" fill string with character to a maximum length
.fd *fun="wcspbrk" locate occurrence of a string within a second string
.fd *fun="wcsrchr" locate last occurrence of character from a character set
.fd *fun="_wcsrev" reverse a string in place
.fd *fun="_wcsset" fill string with a character
.fd *fun="wcsspn" find number of characters at start of string which are also in a second string
.fd *fun="_wcsspnp" return pointer to first character of string not in set
.fd *fun="wcsstr" find first occurrence of string in second string
.fd *fun="wcstok" get next token from string
.fd *fun="_wcsupr" convert string to uppercase
.fd *fun="wcsxfrm" transform string to locale's collating sequence
.fdend
.np
For related functions see the sections
.us Conversion Functions
(conversions to and from strings),
.us Time Functions
(formatting of dates and times), and
.us Memory Manipulation Functions
(operate on arrays without terminating null character).
.*======================================================================
.section Multibyte String Manipulation Functions
.*
.np
.ix '&Wide'
.ix '&Multibyte'
A
.us wide string
is an array of wide characters (with type
.id wchar_t
.ct ) that is terminated with an extra null wide character
.mono (L'\0').
Functions are passed only the address of the wide string since the
size can be determined by searching for the terminating character.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd *fun="btowc" return wide-character version of single-byte character
.fd *fun="_fmbscat" concatenate two far strings
.fd *fun="_fmbschr" locate character in far string
.fd *fun="_fmbscmp" compare two far strings
.fd *fun="_fmbscpy" copy far string
.fd *fun="_fmbscspn" get number of string characters not from a set of characters
.fd *fun="_fmbsdec" returns far pointer to the previous character in far string
.fd *fun="_fmbsdup" allocate and duplicate a far string
.fd *fun="_fmbsicmp" compare two far strings with case insensitivity
.fd *fun="_fmbsinc" return far pointer to next character in far string
.fd *fun="_fmbslen" length of a far string
.fd *fun="_fmbslwr" convert far string to lowercase
.fd *fun="_fmbsnbcat" append up to "n" bytes of string to another string
.fd *fun="_fmbsnbcmp" compare up to "n" bytes in two strings
.fd *fun="_fmbsnbcnt" count the number of characters in the first "n" bytes
.fd *fun="_fmbsnbcpy" copy up to "n" bytes of a string
.fd *fun="_fmbsnbicmp" compare up to "n" bytes in two strings with case insensitivity
.fd *fun="_fmbsnbset" fill string with up to "n" bytes
.fd *fun="_fmbsncat" concatenate two far strings, up to a maximum length
.fd *fun="_fmbsnccnt" count the number of characters in the first "n" bytes
.fd *fun="_fmbsncmp" compare two far strings up to maximum length
.fd *fun="_fmbsncpy" copy a far string, up to a maximum length
.fd *fun="_fmbsnextc" return integer value of the next multibyte-character in far string
.fd *fun="_fmbsnicmp" compare two far strings with case insensitivity up to a maximum length
.fd *fun="_fmbsninc" increment wide character far pointer by "n" characters
.fd *fun="_fmbsnset" fill far string with character to a maximum length
.fd *fun="_fmbspbrk" locate occurrence of a string within a second string
.fd *fun="_fmbsrchr" locate last occurrence of character from a character set
.fd *fun="_fmbsrev" reverse a far string in place
.fd *fun="_fmbsrtowcs" convert multibyte character string to wide character string
.fd *fun="_fmbsset" fill far string with a character
.fd *fun="_fmbsspn" find number of characters at start of string which are also in a second string
.fd *fun="_fmbsspnp" return far pointer to first character of far string not in set
.fd *fun="_fmbsstr" find first occurrence of string in second string
.fd *fun="_fmbstok" get next token from a far string
.fd *fun="_fmbstowcs" convert multibyte character string to wide character string
.fd *fun="_fmbsupr" convert far string to uppercase
.fd *fun="_fmbterm" determine if next multibyte character in string is null
.fd *fun="_fmbvtop" store multibyte character into far string
.fd *fun="_fwcrtomb" convert wide character to multibyte character and store
.fd *fun="_fwcsrtombs" convert far wide character string to far multibyte character string
.fd *fun="_fwcstombs" convert far wide character string to far multibyte character string
.fd *fun="_fwctomb" convert wide character to multibyte character
.fd *fun="_mbscat" concatenate string
.fd *fun="_mbschr" locate character in string
.fd *fun="_mbscmp" compare two strings
.fd *fun="_mbscoll" compare two strings using "locale" collating sequence
.fd *fun="_mbscpy" copy a string
.fd *fun="_mbscspn" get number of string characters not from a set of characters
.fd *fun="_mbsdec" returns pointer to the previous character in string
.fd *fun="_mbsdup" allocate and duplicate a string
.fd *fun="_mbsicmp" compare two strings with case insensitivity
.fd *fun="_mbsinc" return pointer to next character in string
.fd *fun="mbsinit" determine if mbstate_t object describes an initial conversion state
.fd *fun="_mbslen" string length
.fd *fun="_mbslwr" convert string to lowercase
.fd *fun="_mbsnbcat" append up to "n" bytes of string to another string
.fd *fun="_mbsnbcmp" compare up to "n" bytes in two strings
.fd *fun="_mbsnbcnt" count the number of characters in the first "n" bytes
.fd *fun="_mbsnbcpy" copy up to "n" bytes of a string
.fd *fun="_mbsnbicmp" compare up to "n" bytes in two strings with case insensitivity
.fd *fun="_mbsnbset" fill string with up to "n" bytes
.fd *fun="_mbsncat" concatenate two strings, up to a maximum length
.fd *fun="_mbsnccnt" count the number of characters in the first "n" bytes
.fd *fun="_mbsncmp" compare two strings up to maximum length
.fd *fun="_mbsncpy" copy a string, up to a maximum length
.fd *fun="_mbsnextc" return integer value of the next multibyte-character in string
.fd *fun="_mbsnicmp" compare two strings with case insensitivity up to a maximum length
.fd *fun="_mbsninc" increment wide character pointer by "n" characters
.fd *fun="_mbsnset" fill string with up to "n" multibyte characters
.fd *fun="_mbspbrk" locate occurrence of a string within a second string
.fd *fun="_mbsrchr" locate last occurrence of character from a character set
.fd *fun="_mbsrev" reverse a string in place
.fd *fun="mbsrtowcs" convert multibyte character string to wide character string
.fd *fun="_mbsset" fill string with a character
.fd *fun="_mbsspn" find number of characters at start of string which are also in a second string
.fd *fun="_mbsspnp" return pointer to first character of string not in set
.fd *fun="_mbsstr" find first occurrence of string in second string
.fd *fun="_mbstok" get next token from string
.fd *fun="mbstowcs" convert multibyte character string to wide character string
.fd *fun="_mbsupr" convert string to uppercase
.fd *fun="_mbterm" determine if next multibyte character in string is null
.fd *fun="_mbvtop" store multibyte character into string
.fd *fun="wcrtomb" convert wide character to multibyte character and store
.fd *fun="wcsrtombs" convert wide character string to multibyte character string
.fd *fun="wcstombs" convert wide character string to multibyte character string
.fd *fun="wctob" return single-byte character version of wide character
.fd *fun="wctomb" convert wide character to multibyte character
.fdend
.np
For related functions see the sections
.us Conversion Functions
(conversions to and from strings),
.us Time Functions
(formatting of dates and times), and
.us Memory Manipulation Functions
(operate on arrays without terminating null character).
.*======================================================================
.section Conversion Functions
.*
.np
.ix '&Conversion'
These functions perform conversions between objects of various types
and strings.
.fdbeg
.fd *fun="atof" string to "double"
.fd *fun="atoh" hexadecimal string to "unsigned int"
.fd *fun="atoi" string to "int"
.fd *fun="atol" string to "long int"
.fd *fun="atoll" string to "long long int"
.fd *fun="ecvt" "double" to E-format string
.fd *fun="fcvt" "double" to F-format string
.fd *fun="gcvt" "double" to string
.fd *fun="itoa" "int" to string
.fd *fun="lltoa" "long long int" to string
.fd *fun="ltoa" "long int" to string
.fd *fun="strtod" string to "double"
.fd *fun="strtol" string to "long int"
.fd *fun="strtoll" string to "long long int"
.fd *fun="strtoul" string to "unsigned long int"
.fd *fun="strtoull" string to "unsigned long long int"
.fd *fun="ulltoa" "unsigned long long int" to string
.fd *fun="ultoa" "unsigned long int" to string
.fd *fun="utoa" "unsigned int" to string
.fdend
.np
These functions perform conversions between objects of various types
and wide character strings.
.fdbeg
.fd *fun="_itow" "int" to wide character string
.fd *fun="_lltow" "long long int" to wide character string
.fd *fun="_ltow" "long int" to wide character string
.fd *fun="_ulltow" "unsigned long long int" to wide character string
.fd *fun="_ultow" "unsigned long int" to wide character string
.fd *fun="_utow" "unsigned int" to wide character string
.fd *fun="wcstod" wide character string to "double"
.fd *fun="wcstol" wide character string to "long int"
.fd *fun="wcstoll" wide character string to "long long int"
.fd *fun="wcstoul" wide character string to "unsigned long int"
.fd *fun="wcstoull" wide character string to "unsigned long long int"
.fd *fun="_wtof" wide character string to "double"
.fd *fun="_wtoi" wide character string to "int"
.fd *fun="_wtol" wide character string to "long int"
.fd *fun="_wtoll" wide character string to "long long int"
.fdend
.np
See also
.kw tolower
.ct,
.kw towlower
.ct,
.kw _mbctolower
.ct,
.kw toupper
.ct,
.kw towupper
.ct,
.kw _mbctoupper
.ct,
.kw strlwr
.ct,
.kw _wcslwr
.ct,
.kw _mbslwr
.ct,
.kw strupr
.ct,
.kw _wcsupr
and
.kw _mbsupr
which convert the cases of characters and strings.
.*======================================================================
.section Memory Allocation Functions
.*
.np
.ix '&Memory'
These functions allocate and de-allocate blocks of memory.
.if '&machsys' eq 'QNX' .do begin
.np
The default data segment has a maximum size of 64K bytes.
It may be less in a machine with insufficient memory or when other
programs in the computer already occupy some of the memory.
.do end
.el .do begin
.np
Unless you are running your program in 32-bit protect mode, where
segments have a limit of 4 gigabytes,
the default data segment has a maximum size of 64K bytes.
It may be less in a machine with insufficient memory or when other
programs in the computer already occupy some of the memory.
.do end
The
.kw _nmalloc
function allocates space within this area while the
.kw _fmalloc
function allocates space outside the area (if it is available).
.np
In a small data model, the
.kw malloc
.ct,
.kw calloc
and
.kw realloc
functions use the
.kw _nmalloc
function to acquire memory; in a large data model, the
.kw _fmalloc
function is used.
.np
It is also possible to allocate memory from a based heap using
.kw _bmalloc
.ct .li .
Based heaps are similar to far heaps in that they are located
outside the normal data segment.
Based pointers only store the offset portion of the full address,
so they behave much like near pointers.
The selector portion of the full address specifies which based heap
a based pointer belongs to, and must be passed to the various based
heap functions.
.np
It is important to use the appropriate memory-deallocation function to
free memory blocks.
The
.kw _nfree
function should be used to free space acquired by the
.kw _ncalloc
.ct,
.kw _nmalloc
.ct,
or
.kw _nrealloc
functions.
The
.kw _ffree
function should be used to free space acquired by the
.kw _fcalloc
.ct,
.kw _fmalloc
.ct,
or
.kw _frealloc
functions.
The
.kw _bfree
function should be used to free space acquired by the
.kw _bcalloc
.ct,
.kw _bmalloc
.ct,
or
.kw _brealloc
functions.
.np
The
.kw free
function will use the
.kw _nfree
function when the small data memory model is used; it will use the
.kw _ffree
function when the large data memory model is being used.
.np
It should be noted that the
.kw _fmalloc
and
.kw _nmalloc
functions can both be used in either data memory model.
.fdbeg
.fd *fun="alloca" allocate auto storage from stack
.fd *fun="_bcalloc" allocate and zero memory from a based heap
.fd *fun="_bexpand" expand a block of memory in a based heap
.fd *fun="_bfree" free a block of memory in a based heap
.fd *fun="_bfreeseg" free a based heap
.fd *fun="_bheapseg" allocate a based heap
.fd *fun="_bmalloc" allocate a memory block from a based heap
.fd *fun="_bmsize" return the size of a memory block
.fd *fun="_brealloc" re-allocate a memory block in a based heap
.fd *fun="calloc" allocate and zero memory
.fd *fun="_expand" expand a block of memory
.fd *fun="_fcalloc" allocate and zero a memory block (outside default data segment)
.fd *fun="_fexpand" expand a block of memory (outside default data segment)
.fd *fun="_ffree" free a block allocated using "_fmalloc"
.fd *fun="_fmalloc" allocate a memory block (outside default data segment)
.fd *fun="_fmsize" return the size of a memory block
.fd *fun="_frealloc" re-allocate a memory block (outside default data segment)
.fd *fun="free" free a block allocated using "malloc", "calloc" or "realloc"
.fd *fun="_freect" return number of objects that can be allocated
.fd *fun="halloc" allocate huge array
.fd *fun="hfree" free huge array
.fd *fun="malloc" allocate a memory block (using current memory model)
.fd *fun="_memavl" return amount of available memory
.fd *fun="_memmax" return largest block of memory available
.fd *fun="_msize" return the size of a memory block
.fd *fun="_ncalloc" allocate and zero a memory block (inside default data segment)
.fd *fun="_nexpand" expand a block of memory (inside default data segment)
.fd *fun="_nfree" free a block allocated using "_nmalloc"
.fd *fun="_nmalloc" allocate a memory block (inside default data segment)
.fd *fun="_nmsize" return the size of a memory block
.fd *fun="_nrealloc" re-allocate a memory block (inside default data segment)
.fd *fun="realloc" re-allocate a block of memory
.fd *fun="sbrk" set allocation "break" position
.fd *fun="stackavail" determine available amount of stack space
.fdend
.*======================================================================
.section Heap Functions
.*
.np
.ix '&Heap'
These functions provide the ability to shrink and grow the heap,
as well as, find heap related problems.
.fdbeg
.fd *fun="_heapchk" perform consistency check on the heap
.fd *fun="_bheapchk" perform consistency check on a based heap
.fd *fun="_fheapchk" perform consistency check on the far heap
.fd *fun="_nheapchk" perform consistency check on the near heap
.fd *fun="_heapgrow" grow the heap
.fd *fun="_fheapgrow" grow the far heap
.fd *fun="_nheapgrow" grow the near heap up to its limit of 64K
.fd *fun="_heapmin" shrink the heap as small as possible
.fd *fun="_bheapmin" shrink a based heap as small as possible
.fd *fun="_fheapmin" shrink the far heap as small as possible
.fd *fun="_nheapmin" shrink the near heap as small as possible
.fd *fun="_heapset" fill unallocated sections of heap with pattern
.fd *fun="_bheapset" fill unallocated sections of based heap with pattern
.fd *fun="_fheapset" fill unallocated sections of far heap with pattern
.fd *fun="_nheapset" fill unallocated sections of near heap with pattern
.fd *fun="_heapshrink" shrink the heap as small as possible
.fd *fun="_fheapshrink" shrink the far heap as small as possible
.fd *fun="_bheapshrink" shrink a based heap as small as possible
.fd *fun="_nheapshrink" shrink the near heap as small as possible
.fd *fun="_heapwalk" walk through each entry in the heap
.fd *fun="_bheapwalk" walk through each entry in a based heap
.fd *fun="_fheapwalk" walk through each entry in the far heap
.fd *fun="_nheapwalk" walk through each entry in the near heap
.fdend
.*======================================================================
.section Math Functions
.*
.np
.ix '&Math'
.ix '&Trig'
These functions operate with objects of type
.id double,
also known as floating-point numbers.
The Intel 8087 processor (and its successor chips) is commonly used
to implement floating-point operations on personal computers.
Functions ending in "87" pertain to this specific hardware
and should be isolated in programs when portability is a consideration.
.fdbeg
.fd *fun="abs" absolute value of an object of type "int"
.fd *fun="acos" arccosine
.fd *fun="acosh" inverse hyperbolic cosine
.fd *fun="asin" arcsine
.fd *fun="asinh" inverse hyperbolic sine
.fd *fun="atan" arctangent of one argument
.fd *fun="atan2" arctangent of two arguments
.fd *fun="atanh" inverse hyperbolic tangent
.fd *fun="bessel" bessel functions j0, j1, jn, y0, y1, and yn
.fd *fun="cabs" absolute value of complex number
.fd *fun="cbrt" cubed root of a number
.fd *fun="ceil" ceiling function
.fd *fun="_clear87" clears floating-point status
.fd *fun="_control87" sets new floating-point control word
.fd *fun="copysign" copies the sign of one number to another
.fd *fun="cos" cosine
.fd *fun="cosh" hyperbolic cosine
.fd *fun="div" compute quotient, remainder from division of an "int" object
.fd *fun="erf" computes the error function
.fd *fun="erfc" computes the complementary error function
.fd *fun="exp" exponential function
.fd *fun="exp2" two raised to a value
.fd *fun="expm1" exponential of a number minus one
.fd *fun="fabs" absolute value of "double"
.fd *fun="fdim" positive difference of two numbers
.fd *fun="_finite" determines whether floating-point value is valid
.fd *fun="floor" floor function
.fd *fun="fma" fused multiply-add
.fd *fun="fmax" returns larger of two numbers
.fd *fun="fmin" returns smaller of two numbers
.fd *fun="fmod" modulus function
.fd *fun="_fpreset" initializes for floating-point operations
.fd *fun="frexp" fractional exponent
.fd *fun="hypot" compute hypotenuse
.fd *fun="ilogb" retrieve the exponent of a "double"
.fd *fun="imaxabs" get quotient, remainder from division of object of maximum-size integer type
.fd *fun="imaxdiv" absolute value of an object of maximum-size integer type
.fd *fun="j0" return Bessel functions of the first kind (described under "bessel Functions")
.fd *fun="j1" return Bessel functions of the first kind (described under "bessel Functions")
.fd *fun="jn" return Bessel functions of the first kind (described under "bessel Functions")
.fd *fun="labs" absolute value of an object of type "long int"
.fd *fun="ldexp" multiply by a power of two
.fd *fun="ldiv" get quotient, remainder from division of object of type "long int"
.fd *fun="lgamma" natural logarithm of the absolute value of the Gamma function
.fd *fun="lgamma_r" natural logarithm of the absolute value of the Gamma function (thread-safe)
.fd *fun="log" natural logarithm
.fd *fun="log10" logarithm, base 10
.fd *fun="log1p" natural logarithm of one plus the argument
.fd *fun="log2" logarithm, base 2
.fd *fun="logb" retrieve the exponent of a "double"
.fd *fun="matherr" handles error from math functions
.fd *fun="max" return maximum of two arguments
.fd *fun="min" return minimum of two arguments
.fd *fun="modf" get integral, fractional parts of "double"
.fd *fun="nearbyint" returns nearest integer based on rounding mode
.fd *fun="nextafter" returns next machine-representable floating point value
.fd *fun="pow" raise to power
.fd *fun="rand" random integer
.fd *fun="remainder" retrieves the remainder of a division operation
.fd *fun="rint" returns nearest integer based on rounding mode
.fd *fun="round" rounds to the nearest integer
.fd *fun="scalbn" compute a "double" times two raised to a power
.fd *fun="_set_matherr" specify a math error handler
.fd *fun="sin" sine
.fd *fun="sinh" hyperbolic sine
.fd *fun="sqrt" square root
.fd *fun="srand" set starting point for generation of random numbers using "rand" function
.fd *fun="_status87" gets floating-point status
.fd *fun="tan" tangent
.fd *fun="tanh" hyperbolic tangent
.fd *fun="tgamma" compute the Gamma function
.fd *fun="trunc" truncate a floating point value
.fd *fun="y0" return Bessel functions of the second kind (described under "bessel")
.fd *fun="y1" return Bessel functions of the second kind (described under "bessel")
.fd *fun="yn" return Bessel functions of the second kind (described under "bessel")
.fdend
.*======================================================================
.section Searching Functions
.*
.np
.ix 'Searching Functions'
These functions provide searching and sorting capabilities.
.fdbeg
.fd *fun="bsearch" find a data item in an array using binary search
.fd *fun="lfind" find a data item in an array using linear search
.fd *fun="lsearch" linear search array, add item if not found
.fd *fun="qsort" sort an array
.fdend
.*======================================================================
.section Time Functions
.*
.np
.ix 'Time Functions'
These functions are concerned with dates and times.
.fdbeg
.fd *fun="asctime" makes time string from time structure
.fd *fun="_asctime" makes time string from time structure
.fd *fun="_wasctime" makes time string from time structure
.fd *fun="__wasctime" makes time string from time structure
.fd *fun="clock" gets time since program start
.fd *fun="ctime" gets calendar time string
.fd *fun="_ctime" gets calendar time string
.fd *fun="_wctime" gets calendar time string
.fd *fun="__wctime" gets calendar time string
.fd *fun="difftime" calculate difference between two times
.fd *fun="ftime" returns the current time in a "timeb" structure
.fd *fun="gmtime" convert calendar time to Coordinated Universal Time (UTC)
.fd *fun="_gmtime" convert calendar time to Coordinated Universal Time (UTC)
.fd *fun="localtime" convert calendar time to local time
.fd *fun="_localtime" convert calendar time to local time
.fd *fun="mktime" make calendar time from local time
.fd *fun="_strdate" return date in buffer
.fd *fun="strftime" format date and time
.fd *fun="wcsftime" format date and time
.fd *fun="_wstrftime_ms" format date and time
.fd *fun="_strtime" return time in buffer
.fd *fun="_wstrtime" return time in buffer
.fd *fun="time" get current calendar time
.fd *fun="tzset" set global variables to reflect the local time zone
.fd *fun="_wstrdate" return date in buffer
.fdend
.*======================================================================
.section Variable-length Argument Lists
.*
.np
.ix 'variable arguments'
Variable-length argument lists are used
when a function does not have a fixed number of arguments.
These macros provide the capability to access these arguments.
.fdbeg
.fd *fun="va_arg" get next variable argument
.fd *fun="va_end" complete access of variable arguments
.fd *fun="va_start" start access of variable arguments
.fdend
.*======================================================================
.section Stream I/O Functions
.*
.np
.ix '&StrIo'
A
.us stream
is the name given to a file or device which has been opened for
data transmission.
When a stream is opened, a pointer to a
.kw FILE
structure is returned.
This pointer is used to reference the stream when other functions
are subsequently invoked.
.if '&machsys' ne 'QNX' .do begin
.np
There are two modes by which data can be transmitted:
.begnote
.note binary
Data is transmitted unchanged.
.note text
On input, carriage-return characters are removed before
following linefeed characters.
On output, carriage-return characters are inserted before linefeed
characters.
.endnote
.np
These modes are required since text files are stored with the two
characters delimiting a line of text, while the C convention is
for only the linefeed character to delimit a text line.
.do end
.np
When a program begins execution, there are a number of streams
already open for use:
.begnote
.note stdin
.ix '&KbIo' 'stdin'
.ix 'stdin'
Standard Input: input from the
console
.note stdout
Standard Output: output to the
console
.ix '&KbIo' 'stdout'
.ix 'stdout'
.note stderr
.ix '&Errs' 'stderr'
.ix 'stderr'
Standard Error: output to the
console (used for error messages)
.if '&machsys' ne 'QNX' .do begin
.note stdaux
.ix 'stdaux'
Standard Auxiliary: auxiliary port, available for use by a program
(not available in some Windows platforms)
.note stdprn
.ix 'stdprn'
Standard Printer: available for use by a program
(not available in some Windows platforms)
.do end
.endnote
.np
These standard streams may be re-directed by use of the
.kw freopen
function.
.np
See also the section
.us File Manipulation Functions
for other functions which operate upon files.
.np
The functions referenced in the section
.us Operating System I/O Functions
may also be invoked (use the
.kw fileno
function to obtain the file &handle).
Since the stream functions may buffer input and output, these
functions should be used with caution to avoid unexpected results.
.np
.fdbeg
.fd *fun="clearerr" clear end-of-file and error indicators for stream
.fd *fun="fclose" close stream
.fd *fun="fcloseall" close all open streams
.fd *fun="fdopen" open stream, given &handle
.fd *fun="feof" test for end of file
.fd *fun="ferror" test for file error
.fd *fun="fflush" flush output buffer
.fd *fun="fgetc" get next character from file
.fd *fun="_fgetchar" equivalent to "fgetc" with the argument "stdin"
.fd *fun="fgetpos" get current file position
.fd *fun="fgets" get a string
.fd *fun="flushall" flush output buffers for all streams
.fd *fun="fopen" open a stream
.fd *fun="fprintf" format output
.fd *fun="fputc" write a character
.fd *fun="_fputchar" write a character to the "stdout" stream
.fd *fun="fputs" write a string
.fd *fun="fread" read a number of objects
.fd *fun="freopen" re-opens a stream
.fd *fun="fscanf" scan input according to format
.fd *fun="fseek" set current file position, relative
.fd *fun="fsetpos" set current file position, absolute
.fd *fun="_fsopen" open a shared stream
.fd *fun="ftell" get current file position
.fd *fun="fwrite" write a number of objects
.fd *fun="getc" read character
.fd *fun="getchar" get next character from "stdin"
.fd *fun="gets" get string from "stdin"
.fd *fun="_getw" read int from stream file
.fd *fun="perror" write error message to "stderr" stream
.fd *fun="printf" format output to "stdout"
.fd *fun="putc" write character to file
.fd *fun="putchar" write character to "stdout"
.fd *fun="puts" write string to "stdout"
.fd *fun="_putw" write int to stream file
.fd *fun="rewind" position to start of file
.fd *fun="scanf" scan input from "stdin" under format control
.fd *fun="setbuf" set buffer
.fd *fun="setvbuf" set buffering
.fd *fun="tmpfile" create temporary file
.fd *fun="ungetc" push character back on input stream
.fd *fun="vfprintf" same as "fprintf" but with variable arguments
.fd *fun="vfscanf" same as "fscanf" but with variable arguments
.fd *fun="vprintf" same as "printf" but with variable arguments
.fd *fun="vscanf" same as "scanf" but with variable arguments
.fdend
.np
See the section
.us Directory Functions
for functions which are related to directories.
.*======================================================================
.section Wide Character Stream I/O Functions
.*
.np
.ix '&StrIo'
.ix '&StrIo' '&Multibyte'
.ix '&StrIo' '&Wide'
The previous section describes some general aspects of stream
input/output.
The following describes functions dealing with streams containing
multibyte character sequences.
.np
After a stream is associated with an external file, but before any
operations are performed on it, the stream is without orientation.
Once a wide character input/output function has been applied to a
stream without orientation, the stream becomes
.us wide-oriented.
Similarly, once a byte input/output function has been applied to a
stream without orientation, the stream becomes
.us byte-oriented.
Only a successful call to
.kw freopen
can otherwise alter the orientation of a stream (it removes any
orientation).
You cannot mix byte input/output functions and wide character
input/output functions on the same stream.
.np
A file positioning function can cause the next wide character output
function to overwrite a partial multibyte character.
This can lead to the subsequent reading of a stream of multibyte
characters containing an invalid character.
.np
When multibyte characters are read from a stream, they are converted
to wide characters.
Similarly, when wide characters are written to a stream, they are
converted to multibyte characters.
.np
.fdbeg
.fd *fun="fgetwc" get next wide character from file
.fd *fun="_fgetwchar" equivalent to "fgetwc" with the argument "stdin"
.fd *fun="fgetws" get a wide character string
.fd *fun="fprintf" "C" and "S" extensions to the format specifier
.fd *fun="fputwc" write a wide character
.fd *fun="_fputwchar" write a character to the "stdout" stream
.fd *fun="fputws" write a wide character string
.fd *fun="fscanf" "C" and "S" extensions to the format specifier
.fd *fun="fwprintf" formatted wide character output
.fd *fun="fwscanf" scan wide character input according to format
.fd *fun="getwc" read wide character
.fd *fun="getwchar" get next wide character from "stdin"
.fd *fun="_getws" get wide character string from "stdin"
.fd *fun="putwc" write wide character to file
.fd *fun="putwchar" write wide character to "stdout"
.fd *fun="_putws" write wide character string to "stdout"
.fd *fun="ungetwc" push wide character back on input stream
.fd *fun="vfwprintf" same as "fwprintf" but with variable arguments
.fd *fun="vfwscanf" same as "fwscanf" but with variable arguments
.fd *fun="vswprintf" same as "swprintf" but with variable arguments
.fd *fun="vwprintf" same as "wprintf" but with variable arguments
.fd *fun="vwscanf" same as "wscanf" but with variable arguments
.fd *fun="_wfdopen" open stream, given &handle using a wide character "mode"
.fd *fun="_wfopen" open a stream using wide character arguments
.fd *fun="_wfreopen" re-opens a stream using wide character arguments
.fd *fun="_wfsopen" open a shared stream using wide character arguments
.fd *fun="_wperror" write error message to "stderr" stream
.fd *fun="wprintf" format wide character output to "stdout"
.fd *fun="wscanf" scan wide character input from "stdin" under format control
.fdend
.np
See the section
.us Directory Functions
for functions which are related to directories.
.*======================================================================
.section Process Primitive Functions
.*
.np
.ix '&Process'
.if '&machsys' eq 'QNX' .do begin
These functions deal with process creation, execution and termination,
signal handling, and timer operations.
.do end
.el .do begin
These functions deal with process creation, execution and termination,
signal handling, and timer operations.
.do end
.np
When a new process is started, it may replace the existing process
.begbull
.bull
.kw P_OVERLAY
is specified with the
.kw spawn...
functions
.bull
the
.kw exec...
routines are invoked
.endbull
.pc
or the existing process may be suspended while the new process
executes (control continues at the point following the place where the
new process was started)
.begbull
.bull
.kw P_WAIT
is specified with the
.kw spawn...
functions
.bull
.kw system
is used
.endbull
.np
.fdbeg
.fd *fun="abort" immediate termination of process, return code 3
.fd *fun="alarm" send calling process SIGALRM signal after specified time
.fd *fun="atexit" register exit routine
.fd *fun="_beginthread" start a new thread of execution
.fd *fun="cwait" wait for a child process to terminate
.fd *fun="_cwait" wait for a child process to terminate
.fd *fun="delay" delay for number of milliseconds
.fd *fun="_endthread" end the current thread
.fd *fun="execl" chain to program
.fd *fun="_execl" chain to program
.fd *fun="execle" chain to program, pass environment
.fd *fun="_execle" chain to program, pass environment
.fd *fun="execlp" chain to program
.fd *fun="_execlp" chain to program
.fd *fun="execlpe" chain to program, pass environment
.fd *fun="_execlpe" chain to program, pass environment
.fd *fun="execv" chain to program
.fd *fun="_execv" chain to program
.fd *fun="execve" chain to program, pass environment
.fd *fun="_execve" chain to program, pass environment
.fd *fun="execvp" chain to program
.fd *fun="_execvp" chain to program
.fd *fun="execvpe" chain to program, pass environment
.fd *fun="_execvpe" chain to program, pass environment
.fd *fun="exit" exit process, set return code
.fd *fun="_Exit" exit process, set return code
.fd *fun="_exit" exit process, set return code
.fd *fun="fork" create a new process
.fd *fun="kill" send a signal to specified process
.fd *fun="onexit" register exit routine
.fd *fun="pause" suspend calling process until signal
.fd *fun="raise" signal an exceptional condition
.fd *fun="sigaction" examine or specify action for specific signal
.fd *fun="sigaddset" add specified signal to set
.fd *fun="sigdelset" delete specified signal from set
.fd *fun="sigemptyset" initialize signal set to exclude certain signals
.fd *fun="sigfillset" initialize signal set to include certain signals
.fd *fun="sigismember" test whether signal is a member of a set
.fd *fun="signal" set handling for exceptional condition
.fd *fun="sigpending" store set of pending signals
.fd *fun="sigprocmask" examine or change process's signal mask
.fd *fun="sigsuspend" replace process's signal mask and then suspend process
.fd *fun="sleep" delay for number of seconds
.fd *fun="spawnl" create process
.fd *fun="_spawnl" create process
.fd *fun="spawnle" create process, set environment
.fd *fun="_spawnle" create process, set environment
.fd *fun="spawnlp" create process
.fd *fun="_spawnlp" create process
.fd *fun="spawnlpe" create process, set environment
.fd *fun="_spawnlpe" create process, set environment
.fd *fun="spawnv" create process
.fd *fun="_spawnv" create process
.fd *fun="spawnve" create process, set environment
.fd *fun="_spawnve" create process, set environment
.fd *fun="spawnvp" create process
.fd *fun="_spawnvp" create process
.fd *fun="spawnvpe" create process, set environment
.fd *fun="_spawnvpe" create process, set environment
.fd *fun="system" execute system command
.fd *fun="wait" wait for any child process to terminate
.fd *fun="waitpid" wait for a child process to terminate
.fd *fun="_wexecl" chain to program
.fd *fun="_wexecle" chain to program, pass environment
.fd *fun="_wexeclp" chain to program
.fd *fun="_wexeclpe" chain to program, pass environment
.fd *fun="_wexecv" chain to program
.fd *fun="_wexecve" chain to program, pass environment
.fd *fun="_wexecvp" chain to program
.fd *fun="_wexecvpe" chain to program, pass environment
.fd *fun="_wspawnl" create process
.fd *fun="_wspawnle" create process, set environment
.fd *fun="_wspawnlp" create process
.fd *fun="_wspawnlpe" create process, set environment
.fd *fun="_wspawnv" create process
.fd *fun="_wspawnve" create process, set environment
.fd *fun="_wspawnvp" create process
.fd *fun="_wspawnvpe" create process, set environment
.fd *fun="_wsystem" execute system command
.fdend
.np
There are eight
.kw spawn...
and
.kw exec...
functions each.
The
.mono "..."
is one to three letters:
.begbull
.bull
"l" or "v" (one is required) to indicate the way
the process parameters are passed
.bull
"p" (optional) to indicate whether the
.bd PATH
environment variable is searched to locate the program for the process
.bull
"e" (optional) to indicate that the
environment variables are being passed
.endbull
.*======================================================================
.section Process Environment
.*
.np
.ix '&Process'
.if '&machsys' ne 'QNX' .do begin
These functions deal with process identification,
process groups, system identification, system time,
environment variables, and terminal identification.
.do end
.if '&machsys' eq 'QNX' .do begin
These functions deal with process identification, user identification,
process groups, system identification, system time and process time,
environment variables, terminal identification, and configurable
system variables.
.do end
.fdbeg
.fd *fun="_bgetcmd" get command line
.fd *fun="clearenv" delete environment variables
.fd *fun="confstr" get configuration-defined string values
.fd *fun="ctermid" return name of controlling terminal
.fd *fun="cuserid" generate login name of process owner
.fd *fun="getcmd" get command line
.fd *fun="getnid" return netword ID
.fd *fun="getpid" return process ID of calling process
.fd *fun="_getpid" return process ID of calling process
.fd *fun="getppid" return parent process ID of calling process
.fd *fun="getegid" return effective group ID
.fd *fun="getenv" get environment variable value
.fd *fun="geteuid" return effective user ID
.fd *fun="getgid" return real group ID
.fd *fun="getgroups" get supplementary group IDs
.fd *fun="getlogin" return user's login name
.fd *fun="getpgrp" return process group ID
.fd *fun="getuid" return real user ID
.fd *fun="isatty" determine if file descriptor associated with a terminal
.fd *fun="putenv" add, change or delete environment variable
.fd *fun="_searchenv" search for a file in list of directories
.fd *fun="searchenv" search for a file in list of directories
.fd *fun="setegid" set the effective group ID
.fd *fun="setenv" add, change or delete environment variable
.fd *fun="seteuid" set the effective user ID
.fd *fun="setgid" set real group ID, effective group ID
.fd *fun="setpgid" set process group ID for job control
.fd *fun="setsid" create a new session and set process group ID
.fd *fun="setuid" set real user ID, effective user ID
.fd *fun="sysconf" determine value of configurable system limit or option
.fd *fun="times" return time-accounting information
.fd *fun="ttyname" return pointer to string containing pathname of terminal associated with file descriptor argument
.fd *fun="uname" return operating system identification
.fd *fun="_wgetenv" get environment variable value
.fd *fun="_wputenv" add, change or delete environment variable
.fd *fun="_wsearchenv" search for a file in list of directories
.fd *fun="_wsetenv" add, change or delete environment variable
.fdend
.*======================================================================
.section Directory Functions
.*
.np
.ix '&Direct'
These functions pertain to directory manipulation.
.fdbeg
.fd *fun="chdir" change current working directory
.fd *fun="closedir" close opened directory file
.fd *fun="getcwd" get current working directory
.fd *fun="_getdcwd" get current directory on drive
.fd *fun="mkdir" make a new directory
.fd *fun="opendir" open directory file
.fd *fun="readdir" read file name from directory
.fd *fun="rewinddir" reset position of directory stream
.fd *fun="rmdir" remove a directory
.fd *fun="_wchdir" change current working directory
.fd *fun="_wclosedir" close opened directory file
.fd *fun="_wgetcwd" get current working directory
.fd *fun="_wgetdcwd" get current directory on drive
.fd *fun="_wmkdir" make a new directory
.fd *fun="_wopendir" open directory file
.fd *fun="_wreaddir" read file name from directory
.fd *fun="_wrewinddir" reset position of directory stream
.fd *fun="_wrmdir" remove a directory
.fdend
.*======================================================================
.section Operating System I/O Functions
.*
.np
.ix '&OsIo'
These functions operate at the operating-system level and are
included for compatibility with other C implementations.
It is recommended that the functions used in the section
.us File Manipulation Functions
be used for new programs, as these functions are defined portably
and are part of the ANSI standard for the C language.
.np
The functions in this section reference opened files and devices using a
.us file &handle
which is returned when the file is opened.
The file &handle is passed to the other functions.
.np
.fdbeg
.fd *fun="chsize" change the size of a file
.fd *fun="close" close file
.fd *fun="_commit" commit changes to disk
.fd *fun="creat" create a file
.fd *fun="dup" duplicate file &handle, get unused &handle number
.fd *fun="dup2" duplicate file &handle, supply new &handle number
.fd *fun="eof" test for end of file
.fd *fun="fcntl" control over an open file
.fd *fun="fdatasync" write queued file data to disk
.fd *fun="filelength" get file size
.fd *fun="fileno" get file &handle for stream file
.fd *fun="fstat" get file status
.fd *fun="fsync" write queued file and filesystem data to disk
.fd *fun="_hdopen" get POSIX handle from OS handle
.fd *fun="link" create new directory entry
.fd *fun="lock" lock a section of a file
.fd *fun="locking" lock/unlock a section of a file
.fd *fun="lseek" set current file position
.fd *fun="ltrunc" truncate a file
.fd *fun="mkfifo" make a FIFO special file
.fd *fun="mknod" make a filesystem entry point
.fd *fun="mount" mount a filesystem
.fd *fun="open" open a file
.fd *fun="_os_handle" get OS handle from POSIX handle
.fd *fun="_pclose" close a pipe
.fd *fun="pclose" close a pipe
.fd *fun="pipe" create an inter-process channel
.fd *fun="_popen" open a pipe
.fd *fun="popen" open a pipe
.fd *fun="read" read a record
.fd *fun="readlink" read a symbolic link
.fd *fun="readv" read several records placing them into a specified number of buffers
.fd *fun="select" synchronous I/O multiplexing
.fd *fun="setmode" set file mode
.fd *fun="sopen" open a file for shared access
.fd *fun="symlink" create a symbolic link
.fd *fun="sync" sync the filesystem
.fd *fun="tell" get current file position
.fd *fun="umask" set file permission mask
.fd *fun="umount" unmount a filesystem
.fd *fun="unlink" delete a file
.fd *fun="unlock" unlock a section of a file
.fd *fun="write" write a record
.fd *fun="writev" write several records from a specified number of buffers
.fd *fun="_wcreat" create a file
.fd *fun="_wopen" open a file
.fd *fun="_wpopen" open a pipe
.fd *fun="_wsopen" open a file for shared access
.fd *fun="_wunlink" delete a file
.fdend
.*======================================================================
.section File Manipulation Functions
.*
.np
.ix '&FileOp'
These functions operate directly with files.
.fdbeg
.fd *fun="access" test file or directory for mode of access
.fd *fun="chmod" change permissions for a file
.fd *fun="chown" change owner user ID and group ID of the specified file
.fd *fun="fchmod" change the permissions for the file associated with the specified file descriptor to the specified mode
.fd *fun="fchown" change the user ID and group ID of the file associated with the specified file descriptor to the specified numeric values
.fd *fun="fpathconf" determine value of configurable limit associated with a file or directory
.fd *fun="lstat" get file status
.fd *fun="pathconf" determine value of configurable limit associated with a file or directory
.fd *fun="remove" delete a file
.fd *fun="rename" rename a file
.fd *fun="stat" get file status
.fd *fun="tmpnam" create name for temporary file
.fd *fun="utime" set modification time for a file
.fd *fun="_waccess" test file or directory for mode of access
.fd *fun="_wchmod" change permissions for a file
.fd *fun="_wremove" delete a file
.fd *fun="_wrename" rename a file
.fd *fun="_wstat" get file status
.fd *fun="_wtmpnam" create name for temporary file
.fd *fun="_wutime" set modification time for a file
.fdend
.*======================================================================
.section Console I/O Functions
.*
.np
.ix '&KbIo'
.ix '&PortIo'
These functions provide the capability to read and write data
from the console.
Data is read or written without any special initialization (devices are
not opened or closed), since the functions operate at the hardware level.
.np
.fdbeg
.fd *fun="cgets" get a string from the console
.fd *fun="cprintf" print formatted string to the console
.fd *fun="cputs" write a string to the console
.fd *fun="cscanf" scan formatted data from the console
.fd *fun="getch" get character from console, no echo
.fd *fun="getche" get character from console, echo it
.fd *fun="kbhit" test if keystroke available
.fd *fun="putch" write a character to the console
.fd *fun="ungetch" push back next character from console
.fdend
.*======================================================================
.if '&machsys' ne 'QNX' .do begin
.section Default Windowing Functions
.*
.np
.ix '&WinIo'
These functions provide the capability to manipulate attributes of
various windows created by &company's default windowing system for
Microsoft Windows and IBM OS/2.
.np
.fdbeg
.fd *fun="_dwDeleteOnClose" delete console window upon close
.fd *fun="_dwSetAboutDlg" set about dialogue box title and contents
.fd *fun="_dwSetAppTitle" set main window's application title
.fd *fun="_dwSetConTitle" set console window's title
.fd *fun="_dwShutDown" shut down default windowing system
.fd *fun="_dwYield" yield control to other processes
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section POSIX Realtime Timer Functions
.*
.np
.ix 'POSIX Realtime Timer Functions'
These functions provide realtime timer capabilities.
.fdbeg
.fd *fun="clock_getres" get the resolution of a clock
.fd *fun="clock_gettime" get the current time of a clock
.fd *fun="clock_setres" set the resolution of a clock
.fd *fun="clock_settime" set the current time of a clock
.fd *fun="nanosleep" suspend process until a timeout or signal
.fd *fun="sched_get_priority_max" get the maximum priority for a given scheduling policy
.fd *fun="sched_get_priority_min" get the minimum priority for a given scheduling policy
.fd *fun="sched_getparam" get current scheduling parameters of a process
.fd *fun="sched_getscheduler" get current scheduling policy of a process
.fd *fun="sched_setparam" set scheduling parameters of a process
.fd *fun="sched_setscheduler" set priority and scheduling policy of a process
.fd *fun="sched_yield" give up the processor to another process
.fd *fun="timer_create" create a timer for current process
.fd *fun="timer_delete" return a timer to the free list
.fd *fun="timer_gettime" get pending expiration time of a timer
.fd *fun="timer_settime" set expiration time of a timer
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section POSIX Shared Memory Functions
.*
.np
.ix 'POSIX Shared Memory Functions'
These functions provide memory mapping capabilities.
.fdbeg
.fd *fun="mmap" map process addresses to a memory object
.fd *fun="mprotect" change memory protection
.fd *fun="munmap" unmap previously mapped addresses
.fd *fun="shm_open" open a shared memory object
.fd *fun="shm_unlink" remove a shared memory object
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section POSIX Terminal Control Functions
.*
.np
.ix 'POSIX Terminal Control Functions'
.fdbeg
.fd *fun="cfgetispeed" get terminal input baud rate
.fd *fun="cfgetospeed" get terminal output baud rate
.fd *fun="cfsetispeed" set terminal input baud rate
.fd *fun="cfsetospeed" set terminal output baud rate
.fd *fun="tcdrain" wait until all output written
.fd *fun="tcdropline" initiate a disconnect condition upon the communication line associated with the specified opened file descriptor
.fd *fun="tcflow" suspend transmission or reception of data
.fd *fun="tcflush" discard untransmitted data
.fd *fun="tcgetattr" get terminal attributes
.fd *fun="tcgetpgrp" return value of process group ID of the foreground process group associated with the terminal
.fd *fun="tcsetattr" set terminal attributes
.fd *fun="tcsetct" make the terminal device, associated with the specified file descriptor, a controlling terminal which is associated with the specified process id
.fd *fun="tcsetpgrp" set foreground process group ID associated with the terminal
.fd *fun="tcsendbreak" transmit a terminal "break" for a specified time
.fdend
.do end
.*======================================================================
:cmt. .if '&machsys' eq 'QNX' .do begin
:cmt. .section QNX Terminal Functions
:cmt. .*
:cmt. .np
:cmt. These functions provide the capability to invoke terminal control
:cmt. functions from a program.
:cmt. .fdbeg
:cmt. .fd *fun="term_attr_type" display characters from a text string on the screen at a specified location
:cmt. .fd *fun="term_axis" display an axis on the screen
:cmt. .fd *fun="term_bar" display a bar on the screen
:cmt. .fd *fun="term_box" display a box on the screen
:cmt. .fd *fun="term_box_fill" display a filled box on the screen
:cmt. .fd *fun="term_box_off" disable the line drawing character set for the terminal
:cmt. .fd *fun="term_box_on" enable the line drawing character set for the terminal
:cmt. .fd *fun="term_clear" clear a portion of the screen
:cmt. .fd *fun="term_color" extract color information from specified attribute description and set the default color for subsequent characters displayed on the terminal via the term functions when no color is specified
:cmt. .fd *fun="term_cur" move the cursor to the specified position
:cmt. .fd *fun="term_delete_char" delete "n" characters from the current cursor position, pulling the remainder of the line to the left
:cmt. .fd *fun="term_delete_line" delete "n" lines at the specified row, pulling up the text below that the corresponding number of lines
:cmt. .fd *fun="term_down" position the cursor "n" rows below the current cursor row without changing the cursor column
:cmt. .fd *fun="term_field" obtain an input field from the terminal
:cmt. .fd *fun="term_fill" define the current fill color
:cmt. .fd *fun="term_flush" flush all output to the terminal
:cmt. .fd *fun="term_get_line" transfer characters at very high speed, from an in-memory image of the screen, into the specified buffer
:cmt. .fd *fun="term_home" positions the cursor to the top left corner of the screen
:cmt. .fd *fun="term_init" set up the terminal for full screen interaction
:cmt. .fd *fun="term_insert_char" insert "n" insert characters at the current cursor position
:cmt. .fd *fun="term_insert_line" insert "n" blank lines at the specified row, pushing the text on that row down the corresponding number of lines
:cmt. .fd *fun="term_insert_off" turn insert mode off for the terminal
:cmt. .fd *fun="term_insert_on" turn insert mode on for the terminal
:cmt. .fd *fun="term_key" return a key code from the terminal
:cmt. .fd *fun="term_left" position the cursor "n" columns to the left of the current cursor column without changing the cursor row
:cmt. .fd *fun="term_lmenu" similar to term_menu
:cmt. .fd *fun="term_load" initialize the global "term_state" structure from the system terminal database and set up the terminal for full screen interaction
:cmt. .fd *fun="term_menu" display a horizontal menu on the screen at the specified position, allowing you to select a menu item using the cursor keys or by typing the first character of an item
:cmt. .fd *fun="term_mouse_flags" control mouse events
:cmt. .fd *fun="term_mouse_hide" hide the cursor
:cmt. .fd *fun="term_mouse_move" move the cursor to specified row and column
:cmt. .fd *fun="term_mouse_off" suppress mouse events
:cmt. .fd *fun="term_mouse_on" enable mouse events
:cmt. .fd *fun="term_printf" display formatted output on the terminal
:cmt. .fd *fun="term_relearn_size" relearn the screen size
:cmt. .fd *fun="term_resize_off" disable screen resize learning
:cmt. .fd *fun="term_resize_on" enable screen resize learning
:cmt. .fd *fun="term_restore" reset the terminal attributes to their original state, as they were prior to a call to "term_load" or "term_init"
:cmt. .fd *fun="term_restore_image" display data on the screen at the specified location, where the data within the output buffer was previously loaded by a call to "term_save_image"
:cmt. .fd *fun="term_right" position the cursor "n" columns to the right of the current cursor column without changing the cursor row
:cmt. .fd *fun="term_save_image" transfer characters from an in-memory screen image starting at the specified co-ordinates into a buffer
:cmt. .fd *fun="term_scroll_down" scroll the screen down one line
:cmt. .fd *fun="term_scroll_up" scroll the screen up one line
:cmt. .fd *fun="term_state" this global structure contains the state information about the current terminal
:cmt. .fd *fun="term_type" display characters from a text string on the screen at the specified location
:cmt. .fd *fun="term_unkey" return (unget) the specified character to the "term_key" input buffer such that it will be the next character got by the "term_key" function
:cmt. .fd *fun="term_up" position the cursor "n" rows above the current cursor row without changing the cursor column
:cmt. .fd *fun="term_video_off" release the buffer created by "term_video_on" which was used to maintain an in-memory image of the displayed terminal screen
:cmt. .fd *fun="term_video_on" allocate a buffer that enables the other term functions to maintain an in-memory image of the displayed terminal screen
:cmt. .fdend
:cmt. .do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section System Database Functions
.*
.np
.ix 'System Database Functions'
.fdbeg
.fd *fun="getgrent" get next group entry
.fd *fun="getgrgid" get group information based on group ID
.fd *fun="getgrnam" get group information based on name
.fd *fun="getpwent" peruse the list of system users
.fd *fun="getpwnam" get password information based on name
.fd *fun="getpwuid" get password information based on user ID
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section Miscellaneous QNX Functions
.*
.np
.ix 'Miscellaneous QNX Functions'
.fdbeg
:cmt. .fd *fun="abstimer" set timer to expire on the absolute date specified
.fd *fun="basename" return a pointer to the first character following the last "/" in a string
.fd *fun="block_read" read a specified number of blocks of data from a file
.fd *fun="block_write" write a specified number of blocks of data to a file
.fd *fun="console_active" make the indicated console the "active" console
.fd *fun="console_arm" trigger the specified proxy whenever one of the indicated console events occurs on the console
.fd *fun="console_close" close a previously opened control channel to the device driver of a console device
.fd *fun="console_ctrl" set bits in the console control word
.fd *fun="console_font" change a console font
.fd *fun="console_info" get console information
.fd *fun="console_open" open up a control channel to the device driver of a console device
.fd *fun="console_protocol" change a console's protocol emulation
.fd *fun="console_read" read data directly from the video buffer of the indicated console
.fd *fun="console_size" change and or report the size of the indicated console
.fd *fun="console_state" test and set (or clear) the state of the current events which are pending on the indicated console device
.fd *fun="console_write" write data directly to the video buffer of the indicated console
.fd *fun="Creceive" check to see if a message is waiting from the process identified by the specified process ID
.fd *fun="Creceivemx" check to see if a message is waiting from the process identified by the specified process ID
.fd *fun="dev_arm" trigger the specified proxy whenever one of the indicated terminal device events occurs on the terminal device associated with the specified file descriptor
.fd *fun="dev_fdinfo" return information about the terminal device which is controlled by the specified server process, and which is associated with the specified file descriptor belonging to the specified process
.fd *fun="dev_info" return information about the terminal device associated with the specified file descriptor
.fd *fun="dev_insert_chars" insert characters into the canonical input buffer of the opened device
.fd *fun="dev_ischars" returns the number of bytes available to be read from the terminal device associated with the specified file descriptor
.fd *fun="dev_mode" test and optionally set (or clear) one of the input/output modes pertaining to the terminal device associated with specified file descriptor
.fd *fun="dev_read" read up to "n" bytes of data from the specified terminal device into a buffer
.fd *fun="dev_state" test and set (or clear) the state of the current events which are pending on the terminal device associated with the specified file descriptor
.fd *fun="disk_get_entry" obtain information about the disk (both logical partition and physical disk) associated with the specified file descriptor
.fd *fun="disk_space" obtain information about the size of the disk associated with the specified file descriptor
.fd *fun="errno" this variable is set to certain error values by many functions whenever an error has occurred
.fd *fun="fnmatch" check the specified string to see if it matches the specified pattern
.fd *fun="fsys_fdinfo" queries the server for information about the file associated with the descriptor fd that belongs to process pid
.fd *fun="fsys_get_mount_dev" find and return the device that a file is on
.fd *fun="fsys_get_mount_pt" find and return the file system mount point ("directory") associated with the named device
:cmt. .fd *fun="getclock" get the current time of the clock of the specified type into the specified buffer
.fd *fun="fsys_fstat" obtain detailed information about an open file
.fd *fun="fsys_stat" obtain detailed information about a file
.fd *fun="getopt" a command-line parser that can be used by applications that follow guidelines 3,4,5,6,7,9 and 10 in POSIX Utility Syntax Guidelines 1003.2 2.11.2
.fd *fun="getprio" return the current priority of the specified process ID
:cmt. .fd *fun="getscheduler" return the current scheduling policy of the specified process ID
:cmt. .fd *fun="gettimer" return the current value of the specified timer into the specified value
.fd *fun="input_line" get a string of characters from the specified file and store them in the specified array
:cmt. .fd *fun="mktimer" create an interval timer of the specified type
.fd *fun="mouse_close" close a previously opened control channel to a mouse driver
.fd *fun="mouse_flush" discard any pending mouse events in the mouse queue associated with the specified mouse control channel
.fd *fun="mouse_open" open up a control channel to the mouse driver
.fd *fun="mouse_param" set or query the current control parameters for the specified mouse
.fd *fun="mouse_read" read up to "n" mouse events from the mouse server associated with the specified mouse control channel into the specified buffer
.fd *fun="print_usage" print a program's usage message
.fd *fun="Readmsg" read "n" bytes of data into a specified buffer from the process identified by the specified process ID
.fd *fun="Readmsgmx" read data into a specified array of buffers from the process identified by the specified process ID
.fd *fun="Receive" wait for a message from the process identified by the specified process ID
.fd *fun="Receivemx" check to see if a message is waiting from the process identified by the specified process ID
.fd *fun="regcomp" regular expression compiler
.fd *fun="regerror" print an error string resulting from a regcomp or regexex call
.fd *fun="regexec" compare the specified string against the compiled regular expression
.fd *fun="regfree" release all memory allocated by the regular expression compiler
.fd *fun="Relay" take the message received from the specified source process, and relay it to the specified target process
:cmt. .fd *fun="reltimer" set the specified timer to expire on the relative offset from the current clock
.fd *fun="Reply" reply with a number of bytes of the specified data to the process identified by the specified process ID
.fd *fun="Replymx" reply with a message taken from the specified array of message buffers to the process identified by the specified process ID
:cmt. .fd *fun="rmtimer" remove a previously attached timer based upon the specified timer ID returned from the "mktimer" function
.fd *fun="Send" send the specified message to the process identified by the specified process ID
.fd *fun="Sendfd" send the specified message to the process associated with the specified file descriptor
.fd *fun="Sendfdmx" send a message taken from the specified array of buffers to the process associated with the specified file descriptor
.fd *fun="Sendmx" send a message taken from the specified array of buffers to the process identified by the specified process ID
:cmt. .fd *fun="setclock" set the time (of the clock of the specified type) from the specified buffer
.fd *fun="_setmx" stuff the fields of a "_mxfer_entry"
.fd *fun="setprio" change the priority of the process identified by the specified process ID to the specified priority
:cmt. .fd *fun="setscheduler" change the priority and scheduling policy of the process identified by the specified process ID
:cmt. .fd *fun="SpoolAddData"
:cmt. append "n" bytes of the specified data to the job associated with
:cmt. specified job ID in the spooler with the specified process identifier
:cmt. .fd *fun="SpoolError"
:cmt. return the error status associated with the last "Spool..." function
:cmt. invoked
:cmt. .fd *fun="SpoolGetQueueId"
:cmt. query the specified spooler for the identifier of the specified queue
:cmt. .fd *fun="SpoolJobFlush"
:cmt. instruct the specified spooler to flush the specified job ID without
:cmt. executing it
:cmt. .fd *fun="SpoolJobKill"
:cmt. ask the specified spooler to kill the specified job ID
:cmt. .fd *fun="SpoolJobPause"
:cmt. ask the specified spooler to make the specified job ID ineligible for
:cmt. despooling
:cmt. .fd *fun="SpoolJobStart"
:cmt. tell the specified spooler that the specified job ID is ready for
:cmt. spooling
:cmt. .fd *fun="SpoolQueueFlush"
:cmt. instruct the specified spooler to flush all job(s) in the specified
:cmt. queue, without executing the job(s)
:cmt. .fd *fun="SpoolQueuePause"
:cmt. ask the specified spooler to stop the despooling process on the
:cmt. specified queue
:cmt. .fd *fun="SpoolQueueStart"
:cmt. ask the specified spooler to start the despooling process on the
:cmt. specified queue
:cmt. .fd *fun="SpoolSubmit"
:cmt. submit a job to the specified spooler
.fd *fun="Trace" log timestamped events into a system trace buffer
.fd *fun="Trigger" trigger the specified proxy to send a message to the process which owns it
:cmt. .fd *fun="usage" display a usage message for any utility
.fd *fun="Writemsg" write "n" bytes of data from the specified buffer to the reply message buffer identified by the specified process ID
.fd *fun="Writemsgmx" write a message taken from the specified array of message buffers to the reply message buffer identified by the specified process ID
.fd *fun="Yield" check to see if other processes at the same priority as that of the calling process are READY to run
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section QNX Low-level Functions
.*
.np
.ix 'QNX Low-level Functions'
These functions provide the capability to invoke QNX functions
directly from a program.
.fdbeg
.fd *fun="qnx_device_attach"
.fd *fun="qnx_device_detach"
.fd *fun="qnx_display_hex"
.fd *fun="qnx_display_msg"
.fd *fun="qnx_fd_attach"
.fd *fun="qnx_fd_detach"
.fd *fun="qnx_fd_query"
.fd *fun="qnx_fullpath"
.fd *fun="qnx_getclock"
.fd *fun="qnx_getids"
.fd *fun="qnx_hint_attach"
.fd *fun="qnx_hint_detach"
.fd *fun="qnx_hint_mask"
.fd *fun="qnx_hint_query"
.fd *fun="qnx_ioctl"
.fd *fun="qnx_name_attach"
.fd *fun="qnx_name_detach"
.fd *fun="qnx_name_locate"
.fd *fun="qnx_name_locators"
.fd *fun="qnx_name_query"
.fd *fun="qnx_nidtostr"
.fd *fun="qnx_osinfo"
.fd *fun="qnx_osstat"
.fd *fun="qnx_pflags"
.fd *fun="qnx_prefix_attach"
.fd *fun="qnx_prefix_detach"
.fd *fun="qnx_prefix_getroot"
.fd *fun="qnx_prefix_query"
.fd *fun="qnx_prefix_setroot"
.fd *fun="qnx_proxy_attach"
.fd *fun="qnx_proxy_detach"
.fd *fun="qnx_proxy_rem_attach"
.fd *fun="qnx_proxy_rem_detach"
.fd *fun="qnx_psinfo"
.fd *fun="qnx_scheduler"
.fd *fun="qnx_segment_alloc"
.fd *fun="qnx_segment_alloc_flags"
.fd *fun="qnx_segment_arm"
.fd *fun="qnx_segment_flags"
.fd *fun="qnx_segment_free"
.fd *fun="qnx_segment_get"
.fd *fun="qnx_segment_huge"
.fd *fun="qnx_segment_index"
.fd *fun="qnx_segment_info"
.fd *fun="qnx_segment_overlay_flags"
.fd *fun="qnx_segment_overlay"
.fd *fun="qnx_segment_put"
.fd *fun="qnx_segment_raw_alloc"
.fd *fun="qnx_segment_raw_free"
.fd *fun="qnx_segment_realloc"
.fd *fun="qnx_setclock"
.fd *fun="qnx_setids"
.fd *fun="qnx_sflags"
.fd *fun="qnx_sid_query"
.fd *fun="qnx_spawn"
.fd *fun="qnx_spawn_options"
.fd *fun="qnx_strtonid"
.fd *fun="qnx_sync"
.fd *fun="qnx_trace_close"
.fd *fun="qnx_trace_info"
.fd *fun="qnx_trace_open"
.fd *fun="qnx_trace_read"
.fd *fun="qnx_trace_severity"
.fd *fun="qnx_trace_trigger"
.fd *fun="qnx_umask"
.fd *fun="qnx_vc_attach"
.fd *fun="qnx_vc_detach"
.fd *fun="qnx_vc_name_attach"
.fd *fun="qnx_vc_poll_parm"
.fdend
.do end
.*======================================================================
.if '&machsys' ne 'QNX' .do begin
.if '&machsys' ne 'WIN32' .do begin
.section BIOS Functions
.*
.np
.ix '&BiosFunc'
This set of functions allows access to services provided by the BIOS.
.fdbeg
.fd *fun="_bios_disk" provide disk access functions
.fd *fun="_bios_equiplist" determine equipment list
.fd *fun="_bios_keybrd" provide low-level keyboard access
.fd *fun="_bios_memsize" determine amount of system board memory
.fd *fun="_bios_printer" provide access to printer services
.fd *fun="_bios_serialcom" provide access to serial services
.fd *fun="_bios_timeofday" get and set system clock
.fdend
.do end
.do end
.*======================================================================
.if '&machsys' ne 'QNX' .do begin
.section DOS-Specific Functions
.*
.np
.ix '&DosFunc'
These functions provide the capability to invoke DOS functions
directly from a program.
.fdbeg
.fd *fun="bdos" DOS call (short form)
.fd *fun="dosexterr" extract DOS error information
.fd *fun="_dos_allocmem" allocate a block of memory
.fd *fun="_dos_close" close a file
.fd *fun="_dos_commit" flush buffers to disk
.fd *fun="_dos_creat" create a file
.fd *fun="_dos_creatnew" create a new file
.fd *fun="_dos_findclose" close find file matching
.fd *fun="_dos_findfirst" find first file matching a specified pattern
.fd *fun="_dos_findnext" find the next file matching a specified pattern
.fd *fun="_dos_freemem" free a block of memory
.fd *fun="_dos_getdate" get current system date
.fd *fun="_dos_getdiskfree" get information about disk
.fd *fun="_dos_getdrive" get the current drive
.fd *fun="_dos_getfileattr" get file attributes
.fd *fun="_dos_getftime" get file's last modification time
.fd *fun="_dos_gettime" get the current system time
.fd *fun="_dos_getvect" get contents of interrupt vector
.fd *fun="_dos_keep" install a terminate-and-stay-resident program
.fd *fun="_dos_open" open a file
.fd *fun="_dos_read" read data from a file
.fd *fun="_dos_setblock" change the size of allocated block
.fd *fun="_dos_setdate" change current system date
.fd *fun="_dos_setdrive" change the current default drive
.fd *fun="_dos_setfileattr" set the attributes of a file
.fd *fun="_dos_setftime" set a file's last modification time
.fd *fun="_dos_settime" set the current system time
.fd *fun="_dos_setvect" set an interrupt vector
.fd *fun="_dos_write" write data to a file
.fd *fun="intdos" cause DOS interrupt
.fd *fun="intdosx" cause DOS interrupt, with segment registers
.fd *fun="_wdos_findclose" close find file matching
.fd *fun="_wdos_findfirst" find first file matching a specified pattern
.fd *fun="_wdos_findnext" find the next file matching a specified pattern
.fdend
.do end
.*======================================================================
.section Intel 80x86 Architecture-Specific Functions
.*
.np
.ix 'Intel-Specific Functions'
These functions provide the capability to invoke Intel 80x86
processor-related functions directly from a program.
Functions that apply to the Intel 8086 CPU apply to that family including
the 80286, 80386, 80486 and Pentium processors.
.fdbeg
.fd *fun="_chain_intr" chain to the previous interrupt handler
.fd *fun="_disable" disable interrupts
.fd *fun="_enable" enable interrupts
.fd *fun="FP_OFF" get offset part of far pointer
.fd *fun="FP_SEG" get segment part of far pointer
.fd *fun="inp" get one byte from hardware port
.fd *fun="inpw" get two bytes (one word) from hardware port
.fd *fun="int386" cause 386/486/Pentium CPU interrupt
.fd *fun="int386x" cause 386/486/Pentium CPU interrupt, with segment registers
.fd *fun="int86" cause 8086 CPU interrupt
.fd *fun="int86x" cause 8086 CPU interrupt, with segment registers
.fd *fun="intr" cause 8086 CPU interrupt, with segment registers
.fd *fun="MK_FP" make a far pointer from the segment and offset values
.fd *fun="nosound" turn off the speaker
.fd *fun="outp" write one byte to hardware port
.fd *fun="outpw" write two bytes (one word) to hardware port
.fd *fun="segread" read segment registers
.fd *fun="sound" turn on the speaker at specified frequency
.fdend
.*======================================================================
.section Intel Pentium Multimedia Extension Functions
.*
.np
.ix 'Multimedia Extension functions'
.ix 'MMX functions'
.ix 'IA MMX functions'
This set of functions allows access to Intel Architecture Multimedia
Extensions (MMX).
These functions are implemented as in-line intrinsic functions.
The general format for most functions is:
.millust begin
mm_result = mm_function( mm_operand1, mm_operand2 );
.millust end
.np
These functions provide a simple model for use of Intel Multimedia
Extension (MMX).
More advanced use of MMX can be implemented in much the same way that
these functions are implemented.
See the
.hdrfile mmintrin.h
header file for examples.
.fdbeg
.fd *fun="_m_empty" empty multimedia state
.fd *fun="_m_from_int" form 64-bit MM value from unsigned 32-bit integer value
.fd *fun="_m_packssdw" pack and saturate 32-bit double-words from two MM elements into signed 16-bit words
.fd *fun="_m_packsswb" pack and saturate 16-bit words from two MM elements into signed bytes
.fd *fun="_m_packuswb" pack and saturate signed 16-bit words from two MM elements into unsigned bytes
.fd *fun="_m_paddb" add packed bytes
.fd *fun="_m_paddd" add packed 32-bit double-words
.fd *fun="_m_paddsb" add packed signed bytes with saturation
.fd *fun="_m_paddsw"  add packed signed 16-bit words with saturation
.fd *fun="_m_paddusb" add packed unsigned bytes with saturation
.fd *fun="_m_paddusw" add packed unsigned 16-bit words with saturation
.fd *fun="_m_paddw" add packed 16-bit words
.fd *fun="_m_pand" AND 64 bits of two MM elements
.fd *fun="_m_pandn" invert the 64 bits in MM element, then AND 64 bits from second MM element
.fd *fun="_m_pcmpeqb" compare packed bytes for equality
.fd *fun="_m_pcmpeqd" compare packed 32-bit double-words for equality
.fd *fun="_m_pcmpeqw" compare packed 16-bit words for equality
.fd *fun="_m_pcmpgtb" compare packed bytes for greater than relationship
.fd *fun="_m_pcmpgtd" compare packed 32-bit double-words for greater than relationship
.fd *fun="_m_pcmpgtw" compare packed 16-bit words for greater than relationship
.fd *fun="_m_pmaddwd" multiply packed 16-bit words, then add 32-bit results pair-wise
.fd *fun="_m_pmulhw" multiply the packed 16-bit words of two MM elements, then store high-order 16 bits of results
.fd *fun="_m_pmullw" multiply the packed 16-bit words of two MM elements, then store low-order 16 bits of results
.fd *fun="_m_por" OR 64 bits of two MM elements
.fd *fun="_m_pslld" shift left each 32-bit double-word by amount specified in second MM element
.fd *fun="_m_pslldi" shift left each 32-bit double-word by amount specified in constant value
.fd *fun="_m_psllq" shift left each 64-bit quad-word by amount specified in second MM element
.fd *fun="_m_psllqi" shift left each 64-bit quad-word by amount specified in constant value
.fd *fun="_m_psllw" shift left each 16-bit word by amount specified in second MM element
.fd *fun="_m_psllwi" shift left each 16-bit word by amount specified in constant value
.fd *fun="_m_psrad" shift right (with sign propagation) each 32-bit double-word by amount specified in second MM element
.fd *fun="_m_psradi" shift right (with sign propagation) each 32-bit double-word by amount specified in constant value
.fd *fun="_m_psraw" shift right (with sign propagation) each 16-bit word by amount specified in second MM element
.fd *fun="_m_psrawi" shift right (with sign propagation) each 16-bit word by amount specified in constant value
.fd *fun="_m_psrld" shift right (with zero fill) each 32-bit double-word by an amount specified in second MM element
.fd *fun="_m_psrldi" shift right (with zero fill) each 32-bit double-word by an amount specified in constant value
.fd *fun="_m_psrlq" shift right (with zero fill) each 64-bit quad-word by an amount specified in second MM element
.fd *fun="_m_psrlqi" shift right (with zero fill) each 64-bit quad-word by an amount specified in constant value
.fd *fun="_m_psrlw" shift right (with zero fill) each 16-bit word by an amount specified in second MM element
.fd *fun="_m_psrlwi" shift right (with zero fill) each 16-bit word by an amount specified in constant value
.fd *fun="_m_psubb" subtract packed bytes in MM element from second MM element
.fd *fun="_m_psubd" subtract packed 32-bit dwords in MM element from second MM element
.fd *fun="_m_psubsb" subtract packed signed bytes in MM element from second MM element with saturation
.fd *fun="_m_psubsw" subtract packed signed 16-bit words in MM element from second MM element with saturation
.fd *fun="_m_psubusb" subtract packed unsigned bytes in MM element from second MM element with saturation
.fd *fun="_m_psubusw" subtract packed unsigned 16-bit words in MM element from second MM element with saturation
.fd *fun="_m_psubw"  subtract packed 16-bit words in MM element from second MM element
.fd *fun="_m_punpckhbw" interleave bytes from the high halves of two MM elements
.fd *fun="_m_punpckhdq" interleave 32-bit double-words from the high halves of two MM elements
.fd *fun="_m_punpckhwd" interleave 16-bit words from the high halves of two MM elements
.fd *fun="_m_punpcklbw" interleave bytes from the low halves of two MM elements
.fd *fun="_m_punpckldq" interleave 32-bit double-words from the low halves of two MM elements
.fd *fun="_m_punpcklwd" interleave 16-bit words from the low halves of two MM elements
.fd *fun="_m_pxor" XOR 64 bits from two MM elements
.fd *fun="_m_to_int" retrieve low-order 32 bits from MM value
.fdend
.*======================================================================
:cmt. QNX will do later .if '&machsys' eq 'QNX' .do begin
:cmt. QNX will do later .section UNIX portability Functions
:cmt. QNX will do later .*
:cmt. QNX will do later .np
:cmt. QNX will do later A collection of headers and functions to aid in porting traditional
:cmt. QNX will do later UNIX code.
:cmt. QNX will do later .np
:cmt. QNX will do later The technical note &mn./etc/readme/technotes/Unix.port&emn. describes
:cmt. QNX will do later UNIX portability issues.
:cmt. QNX will do later .do end
.*======================================================================
.section Miscellaneous Functions
.*
.np
.ix 'Miscellaneous Functions'
.fdbeg
.fd *fun="assert" test an assertion and output a string upon failure
.fd *fun="_fullpath" return full path specification for file
.fd *fun="_getmbcp" get current multibyte code page
.fd *fun="getopt" a command-line parser that can be used by applications that follow guidelines outlined in the Single UNIX Specification
.fd *fun="_harderr" critical error handler
.fd *fun="_hardresume" critical error handler resume
.fd *fun="localeconv" obtain locale specific conversion information
.fd *fun="longjmp" return and restore environment saved by "setjmp"
.fd *fun="_lrotl" rotate an "unsigned long" left
.fd *fun="_lrotr" rotate an "unsigned long" right
.fd *fun="main" the main program (user written)
.fd *fun="offsetof" get offset of field in structure
.fd *fun="_rotl" rotate an "unsigned int" left
.fd *fun="_rotr" rotate an "unsigned int" right
.fd *fun="setjmp" save environment for use with "longjmp" function
.fd *fun="_makepath" make a full filename from specified components
.fd *fun="setlocale" set locale category
.fd *fun="_setmbcp" set current multibyte code page
.fd *fun="sigsetjmp" save environment and process's signal mask for use with "siglongjmp" function
.fd *fun="siglongjmp" return and restore environment and process's signal mask saved by "sigsetjmp"
.fd *fun="_splitpath" split a filename into its components
.fd *fun="_splitpath2" split a filename into its components
.fd *fun="_wassert" test an assertion and output wide character string upon failure
.fd *fun="_wfullpath" return full path specification for file
.fd *fun="_wmakepath" make a full filename from specified components
.fd *fun="_wsetlocale" set locale category
.fd *fun="_wsplitpath" split a filename into its components
.fd *fun="_wsplitpath2" split a filename into its components
.fdend
.*======================================================================
.section DOS LFN aware Functions
.*
.np
.ix 'DOS LFN aware Functions'
These functions deal with DOS Long File Name if an application is compiled
with -D__WATCOM_LFN__ option and DOS LFN support is available on host system.
.fdbeg
.fd *fun="access" test file or directory for mode of access
.fd *fun="chdir" change current working directory
.fd *fun="chmod" change permissions for a file
.fd *fun="creat" create a file
.fd *fun="_dos_creat" create a file
.fd *fun="_dos_creatnew" create a new file
.fd *fun="_dos_findfirst" find first file matching a specified pattern
.fd *fun="_dos_getfileattr" get file attributes
.fd *fun="_dos_open" open a file
.fd *fun="_dos_setfileattr" set the attributes of a file
.fd *fun="_findfirst" find first file matching a specified pattern
.fd *fun="_fullpath" return full path specification for file
.fd *fun="getcwd" get current working directory
.fd *fun="_getdcwd" get current directory on drive
.fd *fun="lstat" get file status
.fd *fun="mkdir" make a new directory
.fd *fun="open" open a file
.fd *fun="opendir" open directory file
.fd *fun="remove" delete a file
.fd *fun="rename" rename a file
.fd *fun="rmdir" remove a directory
.fd *fun="sopen" open a file for shared access
.fd *fun="stat" get file status
.fd *fun="tmpnam" create name for temporary file
.fd *fun="unlink" delete a file
.fd *fun="utime" set modification time for a file
.fd *fun="_waccess" test file or directory for mode of access
.fd *fun="_wchdir" change current working directory
.fd *fun="_wchmod" change permissions for a file
.fd *fun="_wcreat" create a file
.fd *fun="_wdos_findfirst" find first file matching a specified pattern
.fd *fun="_wfindfirst" find first file matching a specified pattern
.fd *fun="_wfullpath" return full path specification for file
.fd *fun="_wgetcwd" get current working directory
.fd *fun="_wgetdcwd" get current directory on drive
.fd *fun="_wmkdir" make a new directory
.fd *fun="_wopen" open a file
.fd *fun="_wopendir" open directory file
.fd *fun="_wremove" delete a file
.fd *fun="_wrename" rename a file
.fd *fun="_wrmdir" remove a directory
.fd *fun="_wsopen" open a file for shared access
.fd *fun="_wstat" get file status
.fd *fun="_wtmpnam" create name for temporary file
.fd *fun="_wunlink" delete a file
.fd *fun="_wutime" set modification time for a file
.fdend
.endlevel
.*
.im header
.*
.im globdata
.*======================================================================
.section The TZ Environment Variable
.*
.np
.ix 'time zone'
The
.kw TZ
environment variable is used to establish the local time zone.
.ix 'Coordinated Universal Time'
.ix 'UTC'
.ix 'Greenwich Mean Time'
.ix 'GMT'
The value of the variable is used by various time functions to compute
times relative to Coordinated Universal Time (UTC) (formerly known as
Greenwich Mean Time (GMT)).
.np
.if '&machsys' eq 'QNX' .do begin
The time on the computer should be set to UTC.
Use the
.qnxcmd date
if the time is not automatically maintained by the computer hardware.
.do end
.el .do begin
The time on the computer should be set to the local time.
Use the
.doscmd time
and the
.doscmd date
if the time is not automatically maintained by the computer hardware.
.do end
.np
The
.kw TZ
environment variable can be set (before the program is executed) by
.if '&machsys' eq 'QNX' .do begin
using the
.qnxcmd export
as follows:
.millust begin
    export TZ=PST8PDT
.millust end
.do end
.el .do begin
using the
.doscmd set
as follows:
.millust begin
    SET TZ=PST8PDT
.millust end
.do end
.pc
or (during the program execution) by using the
.kw setenv
or
.kw putenv
library functions:
.millust begin
    setenv( "TZ", "PST8PDT", 1 );
    putenv( "TZ=PST8PDT" );
.millust end
.np
The value of the variable can be obtained by using the
.kw getenv
function:
.millust begin
    char *tzvalue;
     . . .
    tzvalue = getenv( "TZ" );
.millust end
.np
The
.kw tzset
function processes the
.kw TZ
environment variable and sets the global variables
.kw daylight
(indicates if daylight saving time is supported in the locale),
.kw timezone
(contains the number of seconds of time difference between the local
time zone and Coordinated Universal Time (UTC)),
and
.kw tzname
(a vector of two pointers to character strings containing the
standard and daylight time-zone names).
.np
The value of the
.kw TZ
environment variable should be set as follows (spaces are for clarity
only):
.begnote
.note std offset dst offset , rule
.endnote
On the OS/2 platform, an alternate format is also supported.
Please refer to the following section for details.
.np
The expanded format is as follows:
.begnote
.note stdoffset[dst[offset][,start[/time],end[/time]]]
.endnote
.begnote
.note std, dst
three or more letters that are the designation for the standard
.us (std)
or summer
.us (dst)
time zone.
Only
.us std
is required.
If
.us dst
is omitted, then summer time does not apply in this locale.
Upper- and lowercase letters are allowed.
Any characters except for a leading colon (:), digits, comma (,),
minus (&minus), plus (+), and ASCII NUL (\0) are allowed.
.note offset
.ix 'Coordinated Universal Time'
.ix 'UTC'
indicates the value one must add to the local time to arrive at
Coordinated Universal Time (UTC).
The
.us offset
has the form:
.begnote
.note hh[:mm[:ss]]
.endnote
.pc
The minutes
.us (mm)
and seconds
.us (ss)
are optional.
The hour
.us (hh)
is required and may be a single digit.
The
.us offset
following
.us std
is required.
If no
.us offset
follows
.us dst,
summer time is assumed to be one hour ahead of standard time.
One or more digits may be used;
the value is always interpreted as a decimal number.
The hour may be between 0 and 24, and the minutes (and seconds)
- if present - between 0 and 59.
If preceded by a "&minus", the time zone will be east of the
.ix 'Prime Meridian'
.us Prime Meridian
.'ct ; otherwise it will be west (which may be indicated by an optional
preceding "+").
.note rule
indicates when to change to and back from summer time.
The
.us rule
has the form:
.begnote
.note date/time,date/time
.endnote
.pc
where the first
.us date
describes when the change from standard to summer time occurs and the
second
.us date
describes when the change back happens.
Each
.us time
field describes when,
in current local time,
the change to the other time is made.
.np
The format of
.us date
may be one of the following:
.begnote
.note Jn
The Julian day n (1 <= n <= 365).
Leap days are not counted.
That is, in all years - including leap years - February 28 is
day 59 and March 1 is day 60.
It is impossible to explicitly refer to the occasional February 29.
.note n
The zero-based Julian day (0 <= n <= 365).
Leap years are counted, and it is possible to refer to February 29.
.note Mm.n.d
The d'th day (0 <= d <= 6) of week n of month m of the year (1 <= n <= 5,
1 <= m <= 12, where week 5 means "the last d day in month m" which may
occur in the fourth or fifth week).
Week 1 is the first week in which the d'th day occurs.
Day zero is Sunday.
.endnote
.np
The
.us time
has the same format as
.us offset
except that no leading sign ("+" or "&minus") is allowed.
The default, if
.us time
is omitted, is
.mono 02:00:00.
.endnote
.np
Whenever
.kw ctime
.ct ,
.kw _ctime
.ct ,
.kw localtime
.ct ,
.kw _localtime
or
.kw mktime
is called, the time zone names contained in the external variable
.kw tzname
will be set as if the
.kw tzset
function had been called.
The same is true if the
.mono %Z
directive of
.kw strftime
is used.
.np
Some examples are:
.begnote $break
.note TZ=EST5EDT
Eastern Standard Time is 5 hours earlier than Coordinated Universal
Time (UTC).
Standard time and daylight saving time both apply to this locale.
By default, Eastern Daylight Time (EDT) is one hour ahead of standard
time (i.e., EDT4).
Since it is not specified, daylight saving time starts on the first
Sunday of April at 2:00 A.M. and ends on the last Sunday of October at
2:00 A.M.
This is the default when the
.kw TZ
variable is not set.
.note TZ=EST5EDT4,M4.1.0/02:00:00,M10.5.0/02:00:00
This is the full specification for the default when the
.kw TZ
variable is not set.
Eastern Standard Time is 5 hours earlier than Coordinated Universal
Time (UTC).
Standard time and daylight saving time both apply to this locale.
Eastern Daylight Time (EDT) is one hour ahead of standard time.
Daylight saving time starts on the first (1) Sunday (0) of April (4)
at 2:00 A.M. and ends on the last (5) Sunday (0) of October (10) at
2:00 A.M.
.note TZ=PST8PDT
Pacific Standard Time is 8 hours earlier than Coordinated Universal
Time (UTC).
Standard time and daylight saving time both apply to this locale.
By default, Pacific Daylight Time is one hour ahead of standard time
(i.e., PDT7).
Since it is not specified, daylight saving time starts on the first
Sunday of April at 2:00 A.M. and ends on the last Sunday of October at
2:00 A.M.
.note TZ=NST3:30NDT1:30
Newfoundland Standard Time is 3 and 1/2 hours earlier than Coordinated
Universal Time (UTC).
Standard time and daylight saving time both apply to this locale.
Newfoundland Daylight Time is 1 and 1/2 hours earlier than Coordinated
Universal Time (UTC).
.note TZ=Central Europe Time-2:00
Central European Time is 2 hours later than Coordinated Universal Time
(UTC).
Daylight saving time does not apply in this locale.
.endnote
.*
.im tzos2
.im cfunrems
