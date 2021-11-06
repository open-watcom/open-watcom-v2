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
.reffunc fopen
.ct ,
.reffunc fread
.ct ,
.reffunc fwrite
.ct , and
.reffunc fclose
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
.id char
.period
The functions test characters in various ways and convert them between
upper and lowercase.
.fdbeg
.fd isalnum
test for letter or digit
.fd isalpha
test for letter
.fd isascii
test for ASCII character
.fd isblank
test for blank character
.fd iscntrl
test for control character
.fd __iscsym
test for letter, underscore or digit
.fd __iscsymf
test for letter or underscore
.fd isdigit
test for digit
.fd isgraph
test for printable character, except space
.fd islower
test for letter in lowercase
.fd isprint
test for printable character, including space
.fd ispunct
test for punctuation characters
.fd isspace
test for "white space" characters
.fd isupper
test for letter in uppercase
.fd isxdigit
test for hexadecimal digit
.fd tolower
convert character to lowercase
.fd toupper
convert character to uppercase
.fdend
.*======================================================================
.section Wide Character Manipulation Functions
.*
.np
.ix '&CharTest'
.ix '&Wide'
These functions operate upon wide characters of type
.id wchar_t
.period
The functions test wide characters in various ways and convert them
between upper and lowercase.
.fdbeg
.fd iswalnum
test for letter or digit
.fd iswalpha
test for letter
.fd iswascii
test for ASCII character
.fd iswblank
test for blank character
.fd iswcntrl
test for control character
.fd __iswcsym
test for letter, underscore or digit
.fd __iswcsymf
test for letter or underscore
.fd iswdigit
test for digit
.fd iswgraph
test for printable character, except space
.fd iswlower
test for letter in lowercase
.fd iswprint
test for printable character, including space
.fd iswpunct
test for punctuation characters
.fd iswspace
test for "white space" characters
.fd iswupper
test for letter in uppercase
.fd iswxdigit
test for hexadecimal digit
.fd wctype
construct a property value for a given "property"
.fd iswctype
test a character for a specific property
.fd towlower
convert character to lowercase
.fd towupper
convert character to uppercase
.fd wctrans
construct mapping value for a given "property"
.fd towctrans
convert a character based on a specific property
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
.fd _fmbccmp
compare one multibyte character with another
.fd _fmbccpy
copy one multibyte character from one string to another
.fd _fmbcicmp
compare one multibyte character with another (case insensitive)
.fd _fmbclen
return number of bytes comprising multibyte character
.fd _fmblen
determine length of next multibyte character
.fd _fmbgetcode
get next single-byte or double-byte character from far string
.fd _fmbputchar
store single-byte or double-byte character into far string
.fd _fmbrlen
determine length of next multibyte character
.fd _fmbrtowc
convert far multibyte character to wide character
.fd _fmbsbtype
return type of byte in multibyte character string
.fd _fmbtowc
convert far multibyte character to wide character
.fd _ismbbalnum
test for isalnum or _ismbbkalnum
.fd _ismbbalpha
test for isalpha or _ismbbkalpha
.fd _ismbbgraph
test for isgraph or _ismbbkprint
.fd _ismbbkalnum
test for non-ASCII text symbol other than punctuation
.fd _ismbbkana
test for single-byte Katakana character
.fd _ismbbkalpha
test for non-ASCII text symbol other than digits or punctuation
.fd _ismbbkprint
test for non-ASCII text or non-ASCII punctuation symbol
.fd _ismbbkpunct
test for non-ASCII punctuation character
.fd _ismbblead
test for valid first byte of multibyte character
.fd _ismbbprint
test for isprint or _ismbbkprint
.fd _ismbbpunct
test for ispunct or _ismbbkpunct
.fd _ismbbtrail
test for valid second byte of multibyte character
.fd _ismbcalnum
test for _ismbcalpha or _ismbcdigit
.fd _ismbcalpha
test for a multibyte alphabetic character
.fd _ismbccntrl
test for a multibyte control character
.fd _ismbcdigit
test for a multibyte decimal-digit character '0' through '9'
.fd _ismbcgraph
test for a printable multibyte character except space
.fd _ismbchira
test for a double-byte Hiragana character
.fd _ismbckata
test for a double-byte Katakana character
.fd _ismbcl0
test for a double-byte non-Kanji character
.fd _ismbcl1
test for a JIS level 1 double-byte character
.fd _ismbcl2
test for a JIS level 2 double-byte character
.fd _ismbclegal
test for a valid multibyte character
.fd _ismbclower
test for a valid lowercase multibyte character
.fd _ismbcprint
test for a printable multibyte character including space
.fd _ismbcpunct
test for any multibyte punctuation character
.fd _ismbcspace
test for any multibyte space character
.fd _ismbcsymbol
test for valid multibyte symbol (punctuation and other special graphics)
.fd _ismbcupper
test for valid uppercase multibyte character
.fd _ismbcxdigit
test for any multibyte hexadecimal-digit character
:cmt. .fd _ismbdalnum" test for
:cmt. .fd _ismbdalpha" test for
:cmt. .fd _ismbdcntrl" test for
:cmt. .fd _ismbddigit" test for
:cmt. .fd _ismbdgraph" test for
:cmt. .fd _ismbdlower" test for
:cmt. .fd _ismbdprint" test for
:cmt. .fd _ismbdpunct" test for
:cmt. .fd _ismbdspace" test for
:cmt. .fd _ismbdupper" test for
:cmt. .fd _ismbdxdigit" test for
.fd _mbbtombc
return double-byte equivalent to single-byte character
.fd _mbbtype
determine type of byte in multibyte character
.fd _mbccmp
compare one multibyte character with another
.fd _mbccpy
copy one multibyte character from one string to another
.fd _mbcicmp
compare one multibyte character with another (case insensitive)
.fd _mbcjistojms
convert JIS code to shift-JIS code
.fd _mbcjmstojis
convert shift-JIS code to JIS code
.fd _mbclen
return number of bytes comprising multibyte character
.fd _mbctolower
convert double-byte uppercase character to double-byte lowercase character
.fd _mbctoupper
convert double-byte lowercase character to double-byte uppercase character
.fd _mbctohira
convert double-byte Katakana character to Hiragana character
.fd _mbctokata
convert double-byte Hiragana character to Katakana character
.fd _mbctombb
return single-byte equivalent to double-byte character
.fd _mbgetcode
get next single-byte or double-byte character from string
.fd mblen
determine length of next multibyte character
.fd _mbputchar
store single-byte or double-byte character into string
.fd mbrlen
determine length of next multibyte character
.fd mbrtowc
convert multibyte character to wide character
.fd _mbsbtype
return type of byte in multibyte character string
.fd mbsinit
determine if mbstate_t object describes an initial conversion state
.fd mbtowc
convert multibyte character to wide character
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
.fd _fmemccpy
copy far memory block up to a certain character
.fd _fmemchr
search far memory block for a character value
.fd _fmemcmp
compare any two memory blocks (near or far)
.fd _fmemcpy
copy far memory block, overlap not allowed
.fd _fmemicmp
compare far memory, case insensitive
.fd _fmemmove
copy far memory block, overlap allowed
.fd _fmemset
set any memory block (near of far) to a character
.fd memccpy
copy memory block up to a certain character
.fd memchr
search memory block for a character value
.fd memcmp
compare memory blocks
.fd memcpy
copy memory block, overlap not allowed
.fd _memicmp
compare memory, case insensitive
.fd memmove
copy memory block, overlap allowed
.fd memset
set memory block to a character
.fd movedata
copy memory block, with segment information
.fd swab
swap bytes of a memory block
.* .fd _wmemccpy
.* copy memory block up to a certain character
.fd wmemchr
search memory block for a wide character value
.fd wmemcmp
compare memory blocks
.fd wmemcpy
copy memory block, overlap not allowed
.* .fd _wmemicmp
.* compare memory, case insensitive
.fd wmemmove
copy memory block, overlap allowed
.fd wmemset
set memory block to a wide character
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
can be determined by searching for the terminating null character.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd bcmp
.deprec
compare two byte strings
.fd bcopy
.deprec
copy a byte string
.fd _bprintf
formatted transmission to fixed-length string
.fd bzero
.deprec
zero a byte string
.fd _fstrcat
concatenate two far strings
.fd _fstrchr
locate character in far string
.fd _fstrcmp
compare two far strings
.fd _fstrcpy
copy far string
.fd _fstrcspn
get number of string characters not from a set of characters
.fd _fstricmp
compare two far strings with case insensitivity
.fd _fstrlen
length of a far string
.fd _fstrlwr
convert far string to lowercase
.fd _fstrncat
concatenate two far strings, up to a maximum length
.fd _fstrncmp
compare two far strings up to maximum length
.fd _fstrncpy
copy a far string, up to a maximum length
.fd _fstrnicmp
compare two far strings with case insensitivity up to a maximum length
.fd _fstrnset
fill far string with character to a maximum length
.fd _fstrpbrk
locate occurrence of a string within a second string
.fd _fstrrchr
locate last occurrence of character from a character set
.fd _fstrrev
reverse a far string in place
.fd _fstrset
fill far string with a character
.fd _fstrspn
find number of characters at start of string which are also in a second string
.fd _fstrstr
find first occurrence of string in second string
.fd _fstrtok
get next token from a far string
.fd _fstrupr
convert far string to uppercase
.fd sprintf
formatted transmission to string
.fd sscanf
scan from string under format control
.fd strcat
concatenate string
.fd strchr
locate character in string
.fd strcmp
compare two strings
.fd strcmpi
.deprec
compare two strings with case insensitivity
.fd strcoll
compare two strings using "locale" collating sequence
.fd strcpy
copy a string
.fd strcspn
get number of string characters not from a set of characters
.fd _strdec
returns pointer to the previous character in string
.fd strdup
allocate and duplicate a string
.fd strerror
get error message as string
.fd _stricmp
compare two strings with case insensitivity
.fd _strinc
return pointer to next character in string
.fd strlcat
concatenate string into a bounded buffer
.fd strlcpy
copy string into a bounded buffer
.fd strlen
string length
.fd _strlwr
convert string to lowercase
.fd strncat
concatenate two strings, up to a maximum length
.fd strncmp
compare two strings up to maximum length
.fd _strncnt
count the number of characters in the first "n" bytes
.fd strncpy
copy a string, up to a maximum length
.fd _strnextc
return integer value of the next character in string
.fd _strnicmp
compare two strings with case insensitivity up to a maximum length
.fd _strninc
increment character pointer by "n" characters
.fd _strnset
fill string with character to a maximum length
.fd strpbrk
locate occurrence of a string within a second string
.fd strrchr
locate last occurrence of character from a character set
.fd _strrev
reverse a string in place
.fd _strset
fill string with a character
.fd strspn
find number of characters at start of string which are also in a second string
.fd _strspnp
return pointer to first character of string not in set
.fd strstr
find first occurrence of string in second string
.fd strtok
get next token from string
.fd _strupr
convert string to uppercase
.fd strxfrm
transform string to locale's collating sequence
.fd _vbprintf
same as "_bprintf" but with variable arguments
.fd vsscanf
same as "sscanf" but with variable arguments
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
can be determined by searching for the terminating null character.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd _bwprintf
formatted wide character transmission to fixed-length wcsing
:cmt. .fd _fwcscat" concatenate two far strings
:cmt. .fd _fwcschr" locate character in far string
:cmt. .fd _fwcscmp" compare two far strings
:cmt. .fd _fwcscpy" copy far string
:cmt. .fd _fwcscspn" get number of string characters not from a set of characters
:cmt. .fd _fwcsicmp" compare two far strings with case insensitivity
:cmt. .fd _fwcslen" length of a far string
:cmt. .fd _fwcslwr" convert far string to lowercase
:cmt. .fd _fwcsncat" concatenate two far strings, up to a maximum length
:cmt. .fd _fwcsncmp" compare two far strings up to maximum length
:cmt. .fd _fwcsncpy" copy a far string, up to a maximum length
:cmt. .fd _fwcsnicmp" compare two far strings with case insensitivity up to a maximum length
:cmt. .fd _fwcsnset" fill far string with character to a maximum length
:cmt. .fd _fwcspbrk" locate occurrence of a string within a second string
:cmt. .fd _fwcsrchr" locate last occurrence of character from a character set
:cmt. .fd _fwcsrev" reverse a far string in place
:cmt. .fd _fwcsset" fill far string with a character
:cmt. .fd _fwcsspn" find number of characters at start of string which are also in a second string
:cmt. .fd _fwcsstr" find first occurrence of string in second string
:cmt. .fd _fwcstok" get next token from a far string
:cmt. .fd _fwcsupr" convert far string to uppercase
.fd swprintf
formatted wide character transmission to string
.fd swscanf
scan from wide character string under format control
.fd _vbwprintf
same as "_bwprintf" but with variable arguments
.fd vswscanf
same as "swscanf" but with variable arguments
.fd wcscat
concatenate string
.fd wcschr
locate character in string
.fd wcscmp
compare two strings
.fd wcscmpi
.deprec
compare two strings with case insensitivity
.fd wcscoll
compare two strings using "locale" collating sequence
.fd wcscpy
copy a string
.fd wcscspn
get number of string characters not from a set of characters
.fd _wcsdec
returns pointer to the previous character in string
.fd wcsdup
allocate and duplicate a string
.fd _wcserror
get error message as string
.fd _wcsicmp
compare two strings with case insensitivity
.fd _wcsinc
return pointer to next character in string
.fd wcslcat
concatenate string into a bounded buffer
.fd wcslcpy
copy string into a bounded buffer
.fd wcslen
string length
.fd _wcslwr
convert string to lowercase
.fd wcsncat
concatenate two strings, up to a maximum length
.fd wcsncmp
compare two strings up to maximum length
.fd _wcsncnt
count the number of characters in the first "n" bytes
.fd wcsncpy
copy a string, up to a maximum length
.fd _wcsnextc
return integer value of the next multi-byte character in string
.fd _wcsnicmp
compare two strings with case insensitivity up to a maximum length
.fd _wcsninc
increment wide character pointer by "n" characters
.fd _wcsnset
fill string with character to a maximum length
.fd wcspbrk
locate occurrence of a string within a second string
.fd wcsrchr
locate last occurrence of character from a character set
.fd _wcsrev
reverse a string in place
.fd _wcsset
fill string with a character
.fd wcsspn
find number of characters at start of string which are also in a second string
.fd _wcsspnp
return pointer to first character of string not in set
.fd wcsstr
find first occurrence of string in second string
.fd wcstok
get next token from string
.fd _wcsupr
convert string to uppercase
.fd wcsxfrm
transform string to locale's collating sequence
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
size can be determined by searching for the terminating null character.
The functions that begin with "_f" accept
.id far
pointers as their arguments allowing manipulation of any memory location
regardless of which memory model your program has been compiled for.
.fdbeg
.fd btowc
return wide character version of single-byte character
.fd _fmbscat
concatenate two far strings
.fd _fmbschr
locate character in far string
.fd _fmbscmp
compare two far strings
.fd _fmbscpy
copy far string
.fd _fmbscspn
get number of string characters not from a set of characters
.fd _fmbsdec
returns far pointer to the previous character in far string
.fd _fmbsdup
allocate and duplicate a far string
.fd _fmbsicmp
compare two far strings with case insensitivity
.fd _fmbsinc
return far pointer to next character in far string
.fd _fmbslen
length of a far string
.fd _fmbslwr
convert far string to lowercase
.fd _fmbsnbcat
append up to "n" bytes of string to another string
.fd _fmbsnbcmp
compare up to "n" bytes in two strings
.fd _fmbsnbcnt
count the number of characters in the first "n" bytes
.fd _fmbsnbcpy
copy up to "n" bytes of a string
.fd _fmbsnbicmp
compare up to "n" bytes in two strings with case insensitivity
.fd _fmbsnbset
fill string with up to "n" bytes
.fd _fmbsncat
concatenate two far strings, up to a maximum length
.fd _fmbsnccnt
count the number of characters in the first "n" bytes
.fd _fmbsncmp
compare two far strings up to maximum length
.fd _fmbsncpy
copy a far string, up to a maximum length
.fd _fmbsnextc
return integer value of the next multi-byte character in far string
.fd _fmbsnicmp
compare two far strings with case insensitivity up to a maximum length
.fd _fmbsninc
increment wide character far pointer by "n" characters
.fd _fmbsnset
fill far string with character to a maximum length
.fd _fmbspbrk
locate occurrence of a string within a second string
.fd _fmbsrchr
locate last occurrence of character from a character set
.fd _fmbsrev
reverse a far string in place
.fd _fmbsrtowcs
convert multibyte character string to wide character string
.fd _fmbsset
fill far string with a character
.fd _fmbsspn
find number of characters at start of string which are also in a second string
.fd _fmbsspnp
return far pointer to first character of far string not in set
.fd _fmbsstr
find first occurrence of string in second string
.fd _fmbstok
get next token from a far string
.fd _fmbstowcs
convert multibyte character string to wide character string
.fd _fmbsupr
convert far string to uppercase
.fd _fmbterm
determine if next multibyte character in string is null
.fd _fmbvtop
store multibyte character into far string
.fd _fwcrtomb
convert wide character to multibyte character and store
.fd _fwcsrtombs
convert far wide character string to far multibyte character string
.fd _fwcstombs
convert far wide character string to far multibyte character string
.fd _fwctomb
convert wide character to multibyte character
.fd _mbscat
concatenate string
.fd _mbschr
locate character in string
.fd _mbscmp
compare two strings
.fd _mbscoll
compare two strings using "locale" collating sequence
.fd _mbscpy
copy a string
.fd _mbscspn
get number of string characters not from a set of characters
.fd _mbsdec
returns pointer to the previous character in string
.fd _mbsdup
allocate and duplicate a string
.fd _mbsicmp
compare two strings with case insensitivity
.fd _mbsinc
return pointer to next character in string
.fd mbsinit
determine if mbstate_t object describes an initial conversion state
.fd _mbslen
string length
.fd _mbslwr
convert string to lowercase
.fd _mbsnbcat
append up to "n" bytes of string to another string
.fd _mbsnbcmp
compare up to "n" bytes in two strings
.fd _mbsnbcnt
count the number of characters in the first "n" bytes
.fd _mbsnbcpy
copy up to "n" bytes of a string
.fd _mbsnbicmp
compare up to "n" bytes in two strings with case insensitivity
.fd _mbsnbset
fill string with up to "n" bytes
.fd _mbsncat
concatenate two strings, up to a maximum length
.fd _mbsnccnt
count the number of characters in the first "n" bytes
.fd _mbsncmp
compare two strings up to maximum length
.fd _mbsncpy
copy a string, up to a maximum length
.fd _mbsnextc
return integer value of the next multi-byte character in string
.fd _mbsnicmp
compare two strings with case insensitivity up to a maximum length
.fd _mbsninc
increment wide character pointer by "n" characters
.fd _mbsnset
fill string with up to "n" multibyte characters
.fd _mbspbrk
locate occurrence of a string within a second string
.fd _mbsrchr
locate last occurrence of character from a character set
.fd _mbsrev
reverse a string in place
.fd mbsrtowcs
convert multibyte character string to wide character string
.fd _mbsset
fill string with a character
.fd _mbsspn
find number of characters at start of string which are also in a second string
.fd _mbsspnp
return pointer to first character of string not in set
.fd _mbsstr
find first occurrence of string in second string
.fd _mbstok
get next token from string
.fd mbstowcs
convert multibyte character string to wide character string
.fd _mbsupr
convert string to uppercase
.fd _mbterm
determine if next multibyte character in string is null
.fd _mbvtop
store multibyte character into string
.fd wcrtomb
convert wide character to multibyte character and store
.fd wcsrtombs
convert wide character string to multibyte character string
.fd wcstombs
convert wide character string to multibyte character string
.fd wctob
return single-byte character version of wide character
.fd wctomb
convert wide character to multibyte character
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
.fd atof
string to "double"
.fd atoh
hexadecimal string to "unsigned int"
.fd atoi
string to "int"
.fd atol
string to "long int"
.fd atoll
string to "long long int"
.fd ecvt
"double" to E-format string
.fd fcvt
"double" to F-format string
.fd gcvt
"double" to string
.fd itoa
"int" to string
.fd lltoa
"long long int" to string
.fd ltoa
"long int" to string
.fd strtod
string to "double"
.fd strtol
string to "long int"
.fd strtoll
string to "long long int"
.fd strtoul
string to "unsigned long int"
.fd strtoull
string to "unsigned long long int"
.fd ulltoa
"unsigned long long int" to string
.fd ultoa
"unsigned long int" to string
.fd utoa
"unsigned int" to string
.fdend
.np
These functions perform conversions between objects of various types
and wide character strings.
.fdbeg
.fd _itow
"int" to wide character string
.fd _lltow
"long long int" to wide character string
.fd _ltow
"long int" to wide character string
.fd _ulltow
"unsigned long long int" to wide character string
.fd _ultow
"unsigned long int" to wide character string
.fd _utow
"unsigned int" to wide character string
.fd wcstod
wide character string to "double"
.fd wcstol
wide character string to "long int"
.fd wcstoll
wide character string to "long long int"
.fd wcstoul
wide character string to "unsigned long int"
.fd wcstoull
wide character string to "unsigned long long int"
.fd _wtof
wide character string to "double"
.fd _wtoi
wide character string to "int"
.fd _wtol
wide character string to "long int"
.fd _wtoll
wide character string to "long long int"
.fdend
.np
See also
.reffunc tolower
.ct ,
.reffunc towlower
.ct ,
.reffunc _mbctolower
.ct ,
.reffunc toupper
.ct ,
.reffunc towupper
.ct ,
.reffunc _mbctoupper
.ct ,
.reffunc _strlwr
.ct ,
.reffunc _wcslwr
.ct ,
.reffunc _mbslwr
.ct ,
.reffunc _strupr
.ct ,
.reffunc _wcsupr
and
.reffunc _mbsupr
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
.reffunc _nmalloc
function allocates space within this area while the
.reffunc _fmalloc
function allocates space outside the area (if it is available).
.np
In a small data model, the
.reffunc malloc
.ct ,
.reffunc calloc
and
.reffunc realloc
functions use the
.reffunc _nmalloc
function to acquire memory; in a large data model, the
.reffunc _fmalloc
function is used.
.np
It is also possible to allocate memory from a based heap using
.reffunc _bmalloc
.period
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
.reffunc _nfree
function should be used to free space acquired by the
.reffunc _ncalloc
.ct ,
.reffunc _nmalloc
.ct , or
.reffunc _nrealloc
functions.
The
.reffunc _ffree
function should be used to free space acquired by the
.reffunc _fcalloc
.ct ,
.reffunc _fmalloc
.ct , or
.reffunc _frealloc
functions.
The
.reffunc _bfree
function should be used to free space acquired by the
.reffunc _bcalloc
.ct ,
.reffunc _bmalloc
.ct , or
.reffunc _brealloc
functions.
.np
The
.reffunc free
function will use the
.reffunc _nfree
function when the small data memory model is used; it will use the
.reffunc _ffree
function when the large data memory model is being used.
.np
It should be noted that the
.reffunc _fmalloc
and
.reffunc _nmalloc
functions can both be used in either data memory model.
.fdbeg
.fd alloca
allocate auto storage from stack
.fd _bcalloc
allocate and zero memory from a based heap
.fd _bexpand
expand a block of memory in a based heap
.fd _bfree
free a block of memory in a based heap
.fd _bfreeseg
free a based heap
.fd _bheapseg
allocate a based heap
.fd _bmalloc
allocate a memory block from a based heap
.fd _bmsize
return the size of a memory block
.fd _brealloc
re-allocate a memory block in a based heap
.fd calloc
allocate and zero memory
.fd _expand
expand a block of memory
.fd _fcalloc
allocate and zero a memory block (outside default data segment)
.fd _fexpand
expand a block of memory (outside default data segment)
.fd _ffree
free a block allocated using "_fmalloc"
.fd _fmalloc
allocate a memory block (outside default data segment)
.fd _fmsize
return the size of a memory block
.fd _frealloc
re-allocate a memory block (outside default data segment)
.fd free
free a block allocated using "malloc", "calloc" or "realloc"
.fd _freect
return number of objects that can be allocated
.fd halloc
allocate huge array
.fd hfree
free huge array
.fd malloc
allocate a memory block (using current memory model)
.fd _memavl
return amount of available memory
.fd _memmax
return largest block of memory available
.fd _msize
return the size of a memory block
.fd _ncalloc
allocate and zero a memory block (inside default data segment)
.fd _nexpand
expand a block of memory (inside default data segment)
.fd _nfree
free a block allocated using "_nmalloc"
.fd _nmalloc
allocate a memory block (inside default data segment)
.fd _nmsize
return the size of a memory block
.fd _nrealloc
re-allocate a memory block (inside default data segment)
.fd realloc
re-allocate a block of memory
.fd sbrk
set allocation "break" position
.fd stackavail
determine available amount of stack space
.fdend
.*======================================================================
.section Heap Functions
.*
.np
.ix '&Heap'
These functions provide the ability to shrink and grow the heap,
as well as, find heap related problems.
.fdbeg
.fd _heapchk
perform consistency check on the heap
.fd _bheapchk
perform consistency check on a based heap
.fd _fheapchk
perform consistency check on the far heap
.fd _nheapchk
perform consistency check on the near heap
.fd _heapgrow
grow the heap
.fd _fheapgrow
grow the far heap
.fd _nheapgrow
grow the near heap up to its limit of 64K
.fd _heapmin
shrink the heap as small as possible
.fd _bheapmin
shrink a based heap as small as possible
.fd _fheapmin
shrink the far heap as small as possible
.fd _nheapmin
shrink the near heap as small as possible
.fd _heapset
fill unallocated sections of heap with pattern
.fd _bheapset
fill unallocated sections of based heap with pattern
.fd _fheapset
fill unallocated sections of far heap with pattern
.fd _nheapset
fill unallocated sections of near heap with pattern
.fd _heapshrink
shrink the heap as small as possible
.fd _fheapshrink
shrink the far heap as small as possible
.fd _bheapshrink
shrink a based heap as small as possible
.fd _nheapshrink
shrink the near heap as small as possible
.fd _heapwalk
walk through each entry in the heap
.fd _bheapwalk
walk through each entry in a based heap
.fd _fheapwalk
walk through each entry in the far heap
.fd _nheapwalk
walk through each entry in the near heap
.fdend
.*======================================================================
.section Math Functions
.*
.np
.ix '&Math'
.ix '&Trig'
These functions operate with objects of type
.id double
.ct , also known as floating-point numbers.
The Intel 8087 processor (and its successor chips) is commonly used
to implement floating-point operations on personal computers.
Functions ending in "87" pertain to this specific hardware
and should be isolated in programs when portability is a consideration.
.fdbeg
.fd abs
absolute value of an object of type "int"
.fd acos
arccosine
.fd acosh
inverse hyperbolic cosine
.fd asin
arcsine
.fd asinh
inverse hyperbolic sine
.fd atan
arctangent of one argument
.fd atan2
arctangent of two arguments
.fd atanh
inverse hyperbolic tangent
.fd Bessel Functions
bessel functions
.reffunc j0
.ct ,
.reffunc j1
.ct ,
.reffunc jn
.ct ,
.reffunc y0
.ct ,
.reffunc y1
.ct , and
.reffunc yn
.fd cabs
absolute value of complex number
.fd cbrt
cubed root of a number
.fd ceil
ceiling function
.fd _clear87
clears floating-point status
.fd _control87
sets new floating-point control word
.fd copysign
copies the sign of one number to another
.fd cos
cosine
.fd cosh
hyperbolic cosine
.fd div
compute quotient, remainder from division of an "int" object
.fd erf
computes the error function
.fd erfc
computes the complementary error function
.fd exp
exponential function
.fd exp2
two raised to a value
.fd expm1
exponential of a number minus one
.fd fabs
absolute value of "double"
.fd fdim
positive difference of two numbers
.fd _finite
determines whether floating-point value is valid
.fd floor
floor function
.fd fma
fused multiply-add
.fd fmax
returns larger of two numbers
.fd fmin
returns smaller of two numbers
.fd fmod
modulus function
.fd _fpreset
initializes for floating-point operations
.fd frexp
fractional exponent
.fd hypot
compute hypotenuse
.fd ilogb
retrieve the exponent of a "double"
.fd imaxabs
get quotient, remainder from division of object of maximum-size integer type
.fd imaxdiv
absolute value of an object of maximum-size integer type
.fd j0
return Bessel functions of the first kind (described under
.reffunc Bessel Functions
)
.fd j1
return Bessel functions of the first kind (described under
.reffunc Bessel Functions
)
.fd jn
return Bessel functions of the first kind (described under
.reffunc Bessel Functions
)
.fd labs
absolute value of an object of type "long int"
.fd ldexp
multiply by a power of two
.fd ldiv
get quotient, remainder from division of object of type "long int"
.fd lgamma
natural logarithm of the absolute value of the Gamma function
.fd lgamma_r
natural logarithm of the absolute value of the Gamma function (thread-safe)
.fd log
natural logarithm
.fd log10
logarithm, base 10
.fd log1p
natural logarithm of one plus the argument
.fd log2
logarithm, base 2
.fd logb
retrieve the exponent of a "double"
.fd matherr
handles error from math functions
.fd max
return maximum of two arguments
.fd min
return minimum of two arguments
.fd modf
get integral, fractional parts of "double"
.fd nearbyint
returns nearest integer based on rounding mode
.fd nextafter
returns next machine-representable floating point value
.fd pow
raise to power
.fd rand
random integer
.fd remainder
retrieves the remainder of a division operation
.fd rint
returns nearest integer based on rounding mode
.fd round
rounds to the nearest integer
.fd scalbn
compute a "double" times two raised to a power
.fd _set_matherr
specify a math error handler
.fd sin
sine
.fd sinh
hyperbolic sine
.fd sqrt
square root
.fd srand
set starting point for generation of random numbers using "rand" function
.fd _status87
gets floating-point status
.fd tan
tangent
.fd tanh
hyperbolic tangent
.fd tgamma
compute the Gamma function
.fd trunc
truncate a floating point value
.fd y0
return Bessel functions of the second kind (described under
.reffunc Bessel Functions
)
.fd y1
return Bessel functions of the second kind (described under
.reffunc Bessel Functions
)
.fd yn
return Bessel functions of the second kind (described under
.reffunc Bessel Functions
)
.fdend
.*======================================================================
.section Searching Functions
.*
.np
.ix 'Searching Functions'
These functions provide searching and sorting capabilities.
.fdbeg
.fd bsearch
find a data item in an array using binary search
.fd lfind
find a data item in an array using linear search
.fd lsearch
linear search array, add item if not found
.fd qsort
sort an array
.fdend
.*======================================================================
.section Time Functions
.*
.np
.ix 'Time Functions'
These functions are concerned with dates and times.
.fdbeg
.fd asctime
makes time string from time structure
.fd _asctime
makes time string from time structure
.fd _wasctime
makes time string from time structure
.fd __wasctime
makes time string from time structure
.fd clock
gets time since program start
.fd ctime
gets calendar time string
.fd _ctime
gets calendar time string
.fd _wctime
gets calendar time string
.fd __wctime
gets calendar time string
.fd difftime
calculate difference between two times
.fd ftime
returns the current time in a "timeb" structure
.fd gmtime
convert calendar time to Coordinated Universal Time (UTC)
.fd _gmtime
convert calendar time to Coordinated Universal Time (UTC)
.fd localtime
convert calendar time to local time
.fd _localtime
convert calendar time to local time
.fd mktime
make calendar time from local time
.fd _strdate
return date in buffer
.fd strftime
format date and time
.fd wcsftime
format date and time
.fd _wstrftime_ms
format date and time
.fd _strtime
return time in buffer
.fd _wstrtime
return time in buffer
.fd time
get current calendar time
.fd tzset
set global variables to reflect the local time zone
.fd _wstrdate
return date in buffer
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
.fd va_arg
get next variable argument
.fd va_end
complete access of variable arguments
.fd va_start
start access of variable arguments
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
.reffunc freopen
function.
.np
See also the section
.us File Manipulation Functions
for other functions which operate upon files.
.np
The functions referenced in the section
.us Operating System I/O Functions
may also be invoked (use the
.reffunc fileno
function to obtain the file &handle).
Since the stream functions may buffer input and output, these
functions should be used with caution to avoid unexpected results.
.np
.fdbeg
.fd clearerr
clear end-of-file and error indicators for stream
.fd fclose
close stream
.fd fcloseall
close all open streams
.fd fdopen
open stream, given &handle
.fd feof
test for end of file
.fd ferror
test for file error
.fd fflush
flush output buffer
.fd fgetc
get next character from file
.fd _fgetchar
equivalent to "fgetc" with the argument "stdin"
.fd fgetpos
get current file position
.fd fgets
get a string
.fd flushall
flush output buffers for all streams
.fd fopen
open a stream
.fd fprintf
format output
.fd fputc
write a character
.fd _fputchar
write a character to the "stdout" stream
.fd fputs
write a string
.fd fread
read a number of objects
.fd freopen
re-opens a stream
.fd fscanf
scan input according to format
.fd fseek
set current file position, relative
.fd fsetpos
set current file position, absolute
.fd _fsopen
open a shared stream
.fd ftell
get current file position
.fd fwrite
write a number of objects
.fd getc
read character
.fd getchar
get next character from "stdin"
.fd gets
get string from "stdin"
.fd _getw
read int from stream file
.fd perror
write error message to "stderr" stream
.fd printf
format output to "stdout"
.fd putc
write character to file
.fd putchar
write character to "stdout"
.fd puts
write string to "stdout"
.fd _putw
write int to stream file
.fd rewind
position to start of file
.fd scanf
scan input from "stdin" under format control
.fd setbuf
set buffer
.fd setvbuf
set buffering
.fd tmpfile
create temporary file
.fd ungetc
push character back on input stream
.fd vfprintf
same as "fprintf" but with variable arguments
.fd vfscanf
same as "fscanf" but with variable arguments
.fd vprintf
same as "printf" but with variable arguments
.fd vscanf
same as "scanf" but with variable arguments
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
.reffunc freopen
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
.fd fgetwc
get next wide character from file
.fd _fgetwchar
equivalent to "fgetwc" with the argument "stdin"
.fd fgetws
get a wide character string
.fd fprintf
"C" and "S" extensions to the format specifier
.fd fputwc
write a wide character
.fd _fputwchar
write a character to the "stdout" stream
.fd fputws
write a wide character string
.fd fscanf
"C" and "S" extensions to the format specifier
.fd fwprintf
formatted wide character output
.fd fwscanf
scan wide character input according to format
.fd getwc
read wide character
.fd getwchar
get next wide character from "stdin"
.fd _getws
get wide character string from "stdin"
.fd putwc
write wide character to file
.fd putwchar
write wide character to "stdout"
.fd _putws
write wide character string to "stdout"
.fd ungetwc
push wide character back on input stream
.fd vfwprintf
same as "fwprintf" but with variable arguments
.fd vfwscanf
same as "fwscanf" but with variable arguments
.fd vswprintf
same as "swprintf" but with variable arguments
.fd vwprintf
same as "wprintf" but with variable arguments
.fd vwscanf
same as "wscanf" but with variable arguments
.fd _wfdopen
open stream, given &handle using a wide character "mode"
.fd _wfopen
open a stream using wide character arguments
.fd _wfreopen
re-opens a stream using wide character arguments
.fd _wfsopen
open a shared stream using wide character arguments
.fd _wperror
write error message to "stderr" stream
.fd wprintf
format wide character output to "stdout"
.fd wscanf
scan wide character input from "stdin" under format control
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
.reffunc spawn&grpsfx
functions
.bull
the
.reffunc exec&grpsfx
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
.reffunc spawn&grpsfx
functions
.bull
.reffunc system
is used
.endbull
.np
.fdbeg
.fd abort
immediate termination of process, return code 3
.fd alarm
send calling process SIGALRM signal after specified time
.fd atexit
register exit routine
.fd _beginthread
start a new thread of execution
.fd cwait
wait for a child process to terminate
.fd _cwait
wait for a child process to terminate
.fd delay
delay for number of milliseconds
.fd _endthread
end the current thread
.fd execl
chain to program
.fd _execl
chain to program
.fd execle
chain to program, pass environment
.fd _execle
chain to program, pass environment
.fd execlp
chain to program
.fd _execlp
chain to program
.fd execlpe
chain to program, pass environment
.fd _execlpe
chain to program, pass environment
.fd execv
chain to program
.fd _execv
chain to program
.fd execve
chain to program, pass environment
.fd _execve
chain to program, pass environment
.fd execvp
chain to program
.fd _execvp
chain to program
.fd execvpe
chain to program, pass environment
.fd _execvpe
chain to program, pass environment
.fd exit
exit process, set return code
.fd _Exit
exit process, set return code
.fd _exit
exit process, set return code
.fd fork
create a new process
.fd kill
send a signal to specified process
.fd _onexit
register exit routine
.fd pause
suspend calling process until signal
.fd raise
signal an exceptional condition
.fd sigaction
examine or specify action for specific signal
.fd sigaddset
add specified signal to set
.fd sigdelset
delete specified signal from set
.fd sigemptyset
initialize signal set to exclude certain signals
.fd sigfillset
initialize signal set to include certain signals
.fd sigismember
test whether signal is a member of a set
.fd signal
set handling for exceptional condition
.fd sigpending
store set of pending signals
.fd sigprocmask
examine or change process's signal mask
.fd sigsuspend
replace process's signal mask and then suspend process
.fd sleep
delay for number of seconds
.fd spawnl
create process
.fd _spawnl
create process
.fd spawnle
create process, set environment
.fd _spawnle
create process, set environment
.fd spawnlp
create process
.fd _spawnlp
create process
.fd spawnlpe
create process, set environment
.fd _spawnlpe
create process, set environment
.fd spawnv
create process
.fd _spawnv
create process
.fd spawnve
create process, set environment
.fd _spawnve
create process, set environment
.fd spawnvp
create process
.fd _spawnvp
create process
.fd spawnvpe
create process, set environment
.fd _spawnvpe
create process, set environment
.fd system
execute system command
.fd wait
wait for any child process to terminate
.fd waitpid
wait for a child process to terminate
.fd _wexecl
chain to program
.fd _wexecle
chain to program, pass environment
.fd _wexeclp
chain to program
.fd _wexeclpe
chain to program, pass environment
.fd _wexecv
chain to program
.fd _wexecve
chain to program, pass environment
.fd _wexecvp
chain to program
.fd _wexecvpe
chain to program, pass environment
.fd _wspawnl
create process
.fd _wspawnle
create process, set environment
.fd _wspawnlp
create process
.fd _wspawnlpe
create process, set environment
.fd _wspawnv
create process
.fd _wspawnve
create process, set environment
.fd _wspawnvp
create process
.fd _wspawnvpe
create process, set environment
.fd _wsystem
execute system command
.fdend
.np
There are eight
.reffunc spawn&grpsfx
and
.reffunc exec&grpsfx
functions each.
The
.mono "&grpsfx"
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
.fd _bgetcmd
get command line
.fd clearenv
delete environment variables
.fd confstr
get configuration-defined string values
.fd ctermid
return name of controlling terminal
.fd cuserid
generate login name of process owner
.fd getcmd
get command line
.fd getegid
get effective group ID of calling process
.fd getenv
get environment variable value
.fd geteuid
get effective user ID
.fd getgid
get real group ID of calling process
.fd getgroups
get supplementary group IDs
.fd getlogin
get user's login name
.fd getnid
get netword ID
.fd getpgrp
get process group ID of calling process
.fd getpid
get process ID of calling process
.fd _getpid
get process ID of calling process
.fd getppid
get parent process ID of calling process
.fd gettid
get thread ID of current thread
.fd getuid
get real user ID
.fd isatty
determine if file descriptor associated with a terminal
.fd putenv
add, change or delete environment variable
.fd _searchenv
search for a file in list of directories
.fd searchenv
search for a file in list of directories
.fd setegid
set the effective group ID
.fd setenv
add, change or delete environment variable
.fd seteuid
set the effective user ID
.fd setgid
set real group ID, effective group ID
.fd setpgid
set process group ID for job control
.fd setsid
create a new session and set process group ID
.fd setuid
set real user ID, effective user ID
.fd sysconf
determine value of configurable system limit or option
.fd times
return time-accounting information
.fd ttyname
return pointer to string containing pathname of terminal associated with file descriptor argument
.fd uname
return operating system identification
.fd unsetenv
delete environment variable
.fd _wgetenv
get environment variable value
.fd _wputenv
add, change or delete environment variable
.fd _wsearchenv
search for a file in list of directories
.fd _wsetenv
add, change or delete environment variable
.fd _wunsetenv
delete environment variable
.fdend
.*======================================================================
.section Directory Functions
.*
.np
.ix '&Direct'
These functions pertain to directory manipulation.
.fdbeg
.fd chdir
change current working directory
.fd closedir
close opened directory file
.fd getcwd
get current working directory
.fd _getdcwd
get current directory on drive
.fd mkdir
make a new directory
.fd opendir
open directory file
.fd readdir
read file name from directory
.fd rewinddir
reset position of directory stream
.fd rmdir
remove a directory
.fd _wchdir
change current working directory
.fd _wclosedir
close opened directory file
.fd _wgetcwd
get current working directory
.fd _wgetdcwd
get current directory on drive
.fd _wmkdir
make a new directory
.fd _wopendir
open directory file
.fd _wreaddir
read file name from directory
.fd _wrewinddir
reset position of directory stream
.fd _wrmdir
remove a directory
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
and are part of the ISO/ANSI standard for the C language.
.np
The functions in this section reference opened files and devices using a
.us file &handle
which is returned when the file is opened.
The file &handle is passed to the other functions.
.np
.fdbeg
.fd _chsize
change the size of a file
.fd close
close file
.fd _commit
commit changes to disk
.fd creat
create a file
.fd dup
duplicate file &handle, get unused &handle number
.fd dup2
duplicate file &handle, supply new &handle number
.fd _eof
test for end of file
.fd fcntl
control over an open file
.fd fdatasync
write queued file data to disk
.fd _filelength
get file size
.fd fileno
get file &handle for stream file
.fd fstat
get file status
.fd fsync
write queued file and filesystem data to disk
.fd _hdopen
get POSIX handle from OS handle
.fd link
create new directory entry
.fd lock
lock a section of a file
.fd locking
lock/unlock a section of a file
.fd lseek
set current file position
.fd ltrunc
truncate a file
.fd mkfifo
make a FIFO special file
.fd mknod
make a filesystem entry point
.fd mount
mount a filesystem
.fd open
open a file
.fd _os_handle
get OS handle from POSIX handle
.fd _pclose
close a pipe
.fd pclose
close a pipe
.fd pipe
create an inter-process channel
.fd _popen
open a pipe
.fd popen
open a pipe
.fd read
read a record
.fd readlink
read a symbolic link
.fd readv
read several records placing them into a specified number of buffers
.fd select
synchronous I/O multiplexing
.fd _setmode
set file mode
.fd _sopen
open a file for shared access
.fd symlink
create a symbolic link
.fd sync
sync the filesystem
.fd _tell
get current file position
.fd umask
set file permission mask
.fd umount
unmount a filesystem
.fd unlink
delete a file
.fd unlock
unlock a section of a file
.fd write
write a record
.fd writev
write several records from a specified number of buffers
.fd _wcreat
create a file
.fd _wopen
open a file
.fd _wpopen
open a pipe
.fd _wsopen
open a file for shared access
.fd _wunlink
delete a file
.fdend
.*======================================================================
.section File Manipulation Functions
.*
.np
.ix '&FileOp'
These functions operate directly with files.
.fdbeg
.fd access
test file or directory for mode of access
.fd chmod
change permissions for a file
.fd chown
change owner user ID and group ID of the specified file
.fd fchmod
change the permissions for the file associated with the specified file descriptor to the specified mode
.fd fchown
change the user ID and group ID of the file associated with the specified file descriptor to the specified numeric values
.fd fpathconf
determine value of configurable limit associated with a file or directory
.fd lstat
get file status
.fd pathconf
determine value of configurable limit associated with a file or directory
.fd remove
delete a file
.fd rename
rename a file
.fd stat
get file status
.fd tmpnam
create name for temporary file
.fd utime
set modification time for a file
.fd _waccess
test file or directory for mode of access
.fd _wchmod
change permissions for a file
.fd _wremove
delete a file
.fd _wrename
rename a file
.fd _wstat
get file status
.fd _wtmpnam
create name for temporary file
.fd _wutime
set modification time for a file
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
.fd cgets
get a string from the console
.fd cprintf
print formatted string to the console
.fd cputs
write a string to the console
.fd cscanf
scan formatted data from the console
.fd getch
get character from console, no echo
.fd getche
get character from console, echo it
.fd kbhit
test if keystroke available
.fd putch
write a character to the console
.fd ungetch
push back next character from console
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
.fd _dwDeleteOnClose
delete console window upon close
.fd _dwSetAboutDlg
set about dialogue box title and contents
.fd _dwSetAppTitle
set main window's application title
.fd _dwSetConTitle
set console window's title
.fd _dwShutDown
shut down default windowing system
.fd _dwYield
yield control to other processes
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
.fd clock_getres
get the resolution of a clock
.fd clock_gettime
get the current time of a clock
.fd clock_setres
set the resolution of a clock
.fd clock_settime
set the current time of a clock
.fd nanosleep
suspend process until a timeout or signal
.fd sched_get_priority_max
get the maximum priority for a given scheduling policy
.fd sched_get_priority_min
get the minimum priority for a given scheduling policy
.fd sched_getparam
get current scheduling parameters of a process
.fd sched_getscheduler
get current scheduling policy of a process
.fd sched_setparam
set scheduling parameters of a process
.fd sched_setscheduler
set priority and scheduling policy of a process
.fd sched_yield
give up the processor to another process
.fd timer_create
create a timer for current process
.fd timer_delete
return a timer to the free list
.fd timer_gettime
get pending expiration time of a timer
.fd timer_settime
set expiration time of a timer
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
.fd mmap
map process addresses to a memory object
.fd mprotect
change memory protection
.fd munmap
unmap previously mapped addresses
.fd shm_open
open a shared memory object
.fd shm_unlink
remove a shared memory object
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section POSIX Terminal Control Functions
.*
.np
.ix 'POSIX Terminal Control Functions'
.fdbeg
.fd cfgetispeed
get terminal input baud rate
.fd cfgetospeed
get terminal output baud rate
.fd cfsetispeed
set terminal input baud rate
.fd cfsetospeed
set terminal output baud rate
.fd tcdrain
wait until all output written
.fd tcdropline
initiate a disconnect condition upon the communication line associated with the specified opened file descriptor
.fd tcflow
suspend transmission or reception of data
.fd tcflush
discard untransmitted data
.fd tcgetattr
get terminal attributes
.fd tcgetpgrp
return value of process group ID of the foreground process group associated with the terminal
.fd tcsetattr
set terminal attributes
.fd tcsetct
make the terminal device, associated with the specified file descriptor, a controlling terminal which is associated with the specified process ID
.fd tcsetpgrp
set foreground process group ID associated with the terminal
.fd tcsendbreak
transmit a terminal "break" for a specified time
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
:cmt. .fd term_attr_type" display characters from a text string on the screen at a specified location
:cmt. .fd term_axis" display an axis on the screen
:cmt. .fd term_bar" display a bar on the screen
:cmt. .fd term_box" display a box on the screen
:cmt. .fd term_box_fill" display a filled box on the screen
:cmt. .fd term_box_off" disable the line drawing character set for the terminal
:cmt. .fd term_box_on" enable the line drawing character set for the terminal
:cmt. .fd term_clear" clear a portion of the screen
:cmt. .fd term_color" extract color information from specified attribute description and set the default color for subsequent characters displayed on the terminal via the term functions when no color is specified
:cmt. .fd term_cur" move the cursor to the specified position
:cmt. .fd term_delete_char" delete "n" characters from the current cursor position, pulling the remainder of the line to the left
:cmt. .fd term_delete_line" delete "n" lines at the specified row, pulling up the text below that the corresponding number of lines
:cmt. .fd term_down" position the cursor "n" rows below the current cursor row without changing the cursor column
:cmt. .fd term_field" obtain an input field from the terminal
:cmt. .fd term_fill" define the current fill color
:cmt. .fd term_flush" flush all output to the terminal
:cmt. .fd term_get_line" transfer characters at very high speed, from an in-memory image of the screen, into the specified buffer
:cmt. .fd term_home" positions the cursor to the top left corner of the screen
:cmt. .fd term_init" set up the terminal for full screen interaction
:cmt. .fd term_insert_char" insert "n" insert characters at the current cursor position
:cmt. .fd term_insert_line" insert "n" blank lines at the specified row, pushing the text on that row down the corresponding number of lines
:cmt. .fd term_insert_off" turn insert mode off for the terminal
:cmt. .fd term_insert_on" turn insert mode on for the terminal
:cmt. .fd term_key" return a key code from the terminal
:cmt. .fd term_left" position the cursor "n" columns to the left of the current cursor column without changing the cursor row
:cmt. .fd term_lmenu" similar to term_menu
:cmt. .fd term_load" initialize the global "term_state" structure from the system terminal database and set up the terminal for full screen interaction
:cmt. .fd term_menu" display a horizontal menu on the screen at the specified position, allowing you to select a menu item using the cursor keys or by typing the first character of an item
:cmt. .fd term_mouse_flags" control mouse events
:cmt. .fd term_mouse_hide" hide the cursor
:cmt. .fd term_mouse_move" move the cursor to specified row and column
:cmt. .fd term_mouse_off" suppress mouse events
:cmt. .fd term_mouse_on" enable mouse events
:cmt. .fd term_printf" display formatted output on the terminal
:cmt. .fd term_relearn_size" relearn the screen size
:cmt. .fd term_resize_off" disable screen resize learning
:cmt. .fd term_resize_on" enable screen resize learning
:cmt. .fd term_restore" reset the terminal attributes to their original state, as they were prior to a call to "term_load" or "term_init"
:cmt. .fd term_restore_image" display data on the screen at the specified location, where the data within the output buffer was previously loaded by a call to "term_save_image"
:cmt. .fd term_right" position the cursor "n" columns to the right of the current cursor column without changing the cursor row
:cmt. .fd term_save_image" transfer characters from an in-memory screen image starting at the specified co-ordinates into a buffer
:cmt. .fd term_scroll_down" scroll the screen down one line
:cmt. .fd term_scroll_up" scroll the screen up one line
:cmt. .fd term_state" this global structure contains the state information about the current terminal
:cmt. .fd term_type" display characters from a text string on the screen at the specified location
:cmt. .fd term_unkey" return (unget) the specified character to the "term_key" input buffer such that it will be the next character got by the "term_key" function
:cmt. .fd term_up" position the cursor "n" rows above the current cursor row without changing the cursor column
:cmt. .fd term_video_off" release the buffer created by "term_video_on" which was used to maintain an in-memory image of the displayed terminal screen
:cmt. .fd term_video_on" allocate a buffer that enables the other term functions to maintain an in-memory image of the displayed terminal screen
:cmt. .fdend
:cmt. .do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section System Database Functions
.*
.np
.ix 'System Database Functions'
.fdbeg
.fd getgrent
get next group entry
.fd getgrgid
get group information based on group ID
.fd getgrnam
get group information based on name
.fd getpwent
peruse the list of system users
.fd getpwnam
get password information based on name
.fd getpwuid
get password information based on user ID
.fdend
.do end
.*======================================================================
.if '&machsys' eq 'QNX' .do begin
.section Miscellaneous QNX Functions
.*
.np
.ix 'Miscellaneous QNX Functions'
.fdbeg
:cmt. .fd abstimer" set timer to expire on the absolute date specified
.fd basename
return a pointer to the first character following the last "/" in a string
.fd block_read
read a specified number of blocks of data from a file
.fd block_write
write a specified number of blocks of data to a file
.fd console_active
make the indicated console the "active" console
.fd console_arm
trigger the specified proxy whenever one of the indicated console events occurs on the console
.fd console_close
close a previously opened control channel to the device driver of a console device
.fd console_ctrl
set bits in the console control word
.fd console_font
change a console font
.fd console_info
get console information
.fd console_open
open up a control channel to the device driver of a console device
.fd console_protocol
change a console's protocol emulation
.fd console_read
read data directly from the video buffer of the indicated console
.fd console_size
change and or report the size of the indicated console
.fd console_state
test and set (or clear) the state of the current events which are pending on the indicated console device
.fd console_write
write data directly to the video buffer of the indicated console
.fd Creceive
check to see if a message is waiting from the process identified by the specified process ID
.fd Creceivemx
check to see if a message is waiting from the process identified by the specified process ID
.fd dev_arm
trigger the specified proxy whenever one of the indicated terminal device events occurs on the terminal device associated with the specified file descriptor
.fd dev_fdinfo
return information about the terminal device which is controlled by the specified server process, and which is associated with the specified file descriptor belonging to the specified process
.fd dev_info
return information about the terminal device associated with the specified file descriptor
.fd dev_insert_chars
insert characters into the canonical input buffer of the opened device
.fd dev_ischars
returns the number of bytes available to be read from the terminal device associated with the specified file descriptor
.fd dev_mode
test and optionally set (or clear) one of the input/output modes pertaining to the terminal device associated with specified file descriptor
.fd dev_read
read up to "n" bytes of data from the specified terminal device into a buffer
.fd dev_state
test and set (or clear) the state of the current events which are pending on the terminal device associated with the specified file descriptor
.fd disk_get_entry
obtain information about the disk (both logical partition and physical disk) associated with the specified file descriptor
.fd disk_space
obtain information about the size of the disk associated with the specified file descriptor
.fd errno
this variable is set to certain error values by many functions whenever an error has occurred
.fd fnmatch
check the specified string to see if it matches the specified pattern
.fd fsys_fdinfo
queries the server for information about the file associated with the descriptor fd that belongs to process pid
.fd fsys_get_mount_dev
find and return the device that a file is on
.fd fsys_get_mount_pt
find and return the file system mount point ("directory") associated with the named device
:cmt. .fd getclock" get the current time of the clock of the specified type into the specified buffer
.fd fsys_fstat
obtain detailed information about an open file
.fd fsys_stat
obtain detailed information about a file
.fd getopt
a command-line parser that can be used by applications that follow guidelines 3,4,5,6,7,9 and 10 in POSIX Utility Syntax Guidelines 1003.2 2.11.2
.fd getprio
return the current priority of the specified process ID
:cmt. .fd getscheduler" return the current scheduling policy of the specified process ID
:cmt. .fd gettimer" return the current value of the specified timer into the specified value
.fd input_line
get a string of characters from the specified file and store them in the specified array
:cmt. .fd mktimer" create an interval timer of the specified type
.fd mouse_close
close a previously opened control channel to a mouse driver
.fd mouse_flush
discard any pending mouse events in the mouse queue associated with the specified mouse control channel
.fd mouse_open
open up a control channel to the mouse driver
.fd mouse_param
set or query the current control parameters for the specified mouse
.fd mouse_read
read up to "n" mouse events from the mouse server associated with the specified mouse control channel into the specified buffer
.fd print_usage
print a program's usage message
.fd Readmsg
read "n" bytes of data into a specified buffer from the process identified by the specified process ID
.fd Readmsgmx
read data into a specified array of buffers from the process identified by the specified process ID
.fd Receive
wait for a message from the process identified by the specified process ID
.fd Receivemx
check to see if a message is waiting from the process identified by the specified process ID
.fd regcomp
regular expression compiler
.fd regerror
print an error string resulting from a regcomp or regexex call
.fd regexec
compare the specified string against the compiled regular expression
.fd regfree
release all memory allocated by the regular expression compiler
.fd Relay
take the message received from the specified source process, and relay it to the specified target process
:cmt. .fd reltimer" set the specified timer to expire on the relative offset from the current clock
.fd Reply
reply with a number of bytes of the specified data to the process identified by the specified process ID
.fd Replymx
reply with a message taken from the specified array of message buffers to the process identified by the specified process ID
:cmt. .fd rmtimer" remove a previously attached timer based upon the specified timer ID returned from the "mktimer" function
.fd Send
send the specified message to the process identified by the specified process ID
.fd Sendfd
send the specified message to the process associated with the specified file descriptor
.fd Sendfdmx
send a message taken from the specified array of buffers to the process associated with the specified file descriptor
.fd Sendmx
send a message taken from the specified array of buffers to the process identified by the specified process ID
:cmt. .fd setclock" set the time (of the clock of the specified type) from the specified buffer
.fd _setmx
stuff the fields of a "_mxfer_entry"
.fd setprio
change the priority of the process identified by the specified process ID to the specified priority
:cmt. .fd setscheduler" change the priority and scheduling policy of the process identified by the specified process ID
:cmt. .fd SpoolAddData"
:cmt. append "n" bytes of the specified data to the job associated with
:cmt. specified job ID in the spooler with the specified process identifier
:cmt. .fd SpoolError"
:cmt. return the error status associated with the last "Spool..." function
:cmt. invoked
:cmt. .fd SpoolGetQueueId"
:cmt. query the specified spooler for the identifier of the specified queue
:cmt. .fd SpoolJobFlush"
:cmt. instruct the specified spooler to flush the specified job ID without
:cmt. executing it
:cmt. .fd SpoolJobKill"
:cmt. ask the specified spooler to kill the specified job ID
:cmt. .fd SpoolJobPause"
:cmt. ask the specified spooler to make the specified job ID ineligible for
:cmt. despooling
:cmt. .fd SpoolJobStart"
:cmt. tell the specified spooler that the specified job ID is ready for
:cmt. spooling
:cmt. .fd SpoolQueueFlush"
:cmt. instruct the specified spooler to flush all job(s) in the specified
:cmt. queue, without executing the job(s)
:cmt. .fd SpoolQueuePause"
:cmt. ask the specified spooler to stop the despooling process on the
:cmt. specified queue
:cmt. .fd SpoolQueueStart"
:cmt. ask the specified spooler to start the despooling process on the
:cmt. specified queue
:cmt. .fd SpoolSubmit"
:cmt. submit a job to the specified spooler
.fd Trace
log timestamped events into a system trace buffer
.fd Trigger
trigger the specified proxy to send a message to the process which owns it
:cmt. .fd usage" display a usage message for any utility
.fd Writemsg
write "n" bytes of data from the specified buffer to the reply message buffer identified by the specified process ID
.fd Writemsgmx
write a message taken from the specified array of message buffers to the reply message buffer identified by the specified process ID
.fd Yield
check to see if other processes at the same priority as that of the calling process are READY to run
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
.fd qnx_device_attach

.fd qnx_device_detach

.fd qnx_display_hex

.fd qnx_display_msg

.fd qnx_fd_attach

.fd qnx_fd_detach

.fd qnx_fd_query

.fd qnx_fullpath

.fd qnx_getclock

.fd qnx_getids

.fd qnx_hint_attach

.fd qnx_hint_detach

.fd qnx_hint_mask

.fd qnx_hint_query

.fd qnx_ioctl

.fd qnx_name_attach

.fd qnx_name_detach

.fd qnx_name_locate

.fd qnx_name_locators

.fd qnx_name_query

.fd qnx_nidtostr

.fd qnx_osinfo

.fd qnx_osstat

.fd qnx_pflags

.fd qnx_prefix_attach

.fd qnx_prefix_detach

.fd qnx_prefix_getroot

.fd qnx_prefix_query

.fd qnx_prefix_setroot

.fd qnx_proxy_attach

.fd qnx_proxy_detach

.fd qnx_proxy_rem_attach

.fd qnx_proxy_rem_detach

.fd qnx_psinfo

.fd qnx_scheduler

.fd qnx_segment_alloc

.fd qnx_segment_alloc_flags

.fd qnx_segment_arm

.fd qnx_segment_flags

.fd qnx_segment_free

.fd qnx_segment_get

.fd qnx_segment_huge

.fd qnx_segment_index

.fd qnx_segment_info

.fd qnx_segment_overlay_flags

.fd qnx_segment_overlay

.fd qnx_segment_put

.fd qnx_segment_raw_alloc

.fd qnx_segment_raw_free

.fd qnx_segment_realloc

.fd qnx_setclock

.fd qnx_setids

.fd qnx_sflags

.fd qnx_sid_query

.fd qnx_spawn

.fd qnx_spawn_options

.fd qnx_strtonid

.fd qnx_sync

.fd qnx_trace_close

.fd qnx_trace_info

.fd qnx_trace_open

.fd qnx_trace_read

.fd qnx_trace_severity

.fd qnx_trace_trigger

.fd qnx_umask

.fd qnx_vc_attach

.fd qnx_vc_detach

.fd qnx_vc_name_attach

.fd qnx_vc_poll_parm

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
.fd _bios_disk
provide disk access functions
.fd _bios_equiplist
determine equipment list
.fd _bios_keybrd
provide low-level keyboard access
.fd _bios_memsize
determine amount of system board memory
.fd _bios_printer
provide access to printer services
.fd _bios_serialcom
provide access to serial services
.fd _bios_timeofday
get and set system clock
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
.fd bdos
DOS call (short form)
.fd dosexterr
extract DOS error information
.fd _dos_allocmem
allocate a block of memory
.fd _dos_close
close a file
.fd _dos_commit
flush buffers to disk
.fd _dos_creat
create a file
.fd _dos_creatnew
create a new file
.fd _dos_findclose
close find file matching
.fd _dos_findfirst
find first file matching a specified pattern
.fd _dos_findnext
find the next file matching a specified pattern
.fd _dos_freemem
free a block of memory
.fd _dos_getdate
get current system date
.fd _dos_getdiskfree
get information about disk
.fd _dos_getdrive
get the current drive
.fd _dos_getfileattr
get file attributes
.fd _dos_getftime
get file's last modification time
.fd _dos_gettime
get the current system time
.fd _dos_getvect
get contents of interrupt vector
.fd _dos_keep
install a terminate-and-stay-resident program
.fd _dos_open
open a file
.fd _dos_read
read data from a file
.fd _dos_setblock
change the size of allocated block
.fd _dos_setdate
change current system date
.fd _dos_setdrive
change the current default drive
.fd _dos_setfileattr
set the attributes of a file
.fd _dos_setftime
set a file's last modification time
.fd _dos_settime
set the current system time
.fd _dos_setvect
set an interrupt vector
.fd _dos_write
write data to a file
.fd intdos
cause DOS interrupt
.fd intdosx
cause DOS interrupt, with segment registers
.fd _wdos_findclose
close find file matching
.fd _wdos_findfirst
find first file matching a specified pattern
.fd _wdos_findnext
find the next file matching a specified pattern
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
.fd _chain_intr
chain to the previous interrupt handler
.fd _disable
disable interrupts
.fd _enable
enable interrupts
.fd FP_OFF
get offset part of far pointer
.fd FP_SEG
get segment part of far pointer
.fd inp
get one byte from hardware port
.fd inpw
get two bytes (one word) from hardware port
.fd int386
cause 386/486/Pentium CPU interrupt
.fd int386x
cause 386/486/Pentium CPU interrupt, with segment registers
.fd int86
cause 8086 CPU interrupt
.fd int86x
cause 8086 CPU interrupt, with segment registers
.fd intr
cause 8086 CPU interrupt, with segment registers
.fd intrf
cause 8086 CPU interrupt, with segment registers and CPU flags
.fd MK_FP
make a far pointer from the segment and offset values
.fd nosound
turn off the speaker
.fd outp
write one byte to hardware port
.fd outpw
write two bytes (one word) to hardware port
.fd segread
read segment registers
.fd sound
turn on the speaker at specified frequency
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
.fd _m_empty
empty multimedia state
.fd _m_from_int
form 64-bit MM value from unsigned 32-bit integer value
.fd _m_packssdw
pack and saturate 32-bit double-words from two MM elements into signed 16-bit words
.fd _m_packsswb
pack and saturate 16-bit words from two MM elements into signed bytes
.fd _m_packuswb
pack and saturate signed 16-bit words from two MM elements into unsigned bytes
.fd _m_paddb
add packed bytes
.fd _m_paddd
add packed 32-bit double-words
.fd _m_paddsb
add packed signed bytes with saturation
.fd _m_paddsw
 add packed signed 16-bit words with saturation
.fd _m_paddusb
add packed unsigned bytes with saturation
.fd _m_paddusw
add packed unsigned 16-bit words with saturation
.fd _m_paddw
add packed 16-bit words
.fd _m_pand
AND 64 bits of two MM elements
.fd _m_pandn
invert the 64 bits in MM element, then AND 64 bits from second MM element
.fd _m_pcmpeqb
compare packed bytes for equality
.fd _m_pcmpeqd
compare packed 32-bit double-words for equality
.fd _m_pcmpeqw
compare packed 16-bit words for equality
.fd _m_pcmpgtb
compare packed bytes for greater than relationship
.fd _m_pcmpgtd
compare packed 32-bit double-words for greater than relationship
.fd _m_pcmpgtw
compare packed 16-bit words for greater than relationship
.fd _m_pmaddwd
multiply packed 16-bit words, then add 32-bit results pair-wise
.fd _m_pmulhw
multiply the packed 16-bit words of two MM elements, then store high-order 16 bits of results
.fd _m_pmullw
multiply the packed 16-bit words of two MM elements, then store low-order 16 bits of results
.fd _m_por
OR 64 bits of two MM elements
.fd _m_pslld
shift left each 32-bit double-word by amount specified in second MM element
.fd _m_pslldi
shift left each 32-bit double-word by amount specified in constant value
.fd _m_psllq
shift left each 64-bit quad-word by amount specified in second MM element
.fd _m_psllqi
shift left each 64-bit quad-word by amount specified in constant value
.fd _m_psllw
shift left each 16-bit word by amount specified in second MM element
.fd _m_psllwi
shift left each 16-bit word by amount specified in constant value
.fd _m_psrad
shift right (with sign propagation) each 32-bit double-word by amount specified in second MM element
.fd _m_psradi
shift right (with sign propagation) each 32-bit double-word by amount specified in constant value
.fd _m_psraw
shift right (with sign propagation) each 16-bit word by amount specified in second MM element
.fd _m_psrawi
shift right (with sign propagation) each 16-bit word by amount specified in constant value
.fd _m_psrld
shift right (with zero fill) each 32-bit double-word by an amount specified in second MM element
.fd _m_psrldi
shift right (with zero fill) each 32-bit double-word by an amount specified in constant value
.fd _m_psrlq
shift right (with zero fill) each 64-bit quad-word by an amount specified in second MM element
.fd _m_psrlqi
shift right (with zero fill) each 64-bit quad-word by an amount specified in constant value
.fd _m_psrlw
shift right (with zero fill) each 16-bit word by an amount specified in second MM element
.fd _m_psrlwi
shift right (with zero fill) each 16-bit word by an amount specified in constant value
.fd _m_psubb
subtract packed bytes in MM element from second MM element
.fd _m_psubd
subtract packed 32-bit dwords in MM element from second MM element
.fd _m_psubsb
subtract packed signed bytes in MM element from second MM element with saturation
.fd _m_psubsw
subtract packed signed 16-bit words in MM element from second MM element with saturation
.fd _m_psubusb
subtract packed unsigned bytes in MM element from second MM element with saturation
.fd _m_psubusw
subtract packed unsigned 16-bit words in MM element from second MM element with saturation
.fd _m_psubw
 subtract packed 16-bit words in MM element from second MM element
.fd _m_punpckhbw
interleave bytes from the high halves of two MM elements
.fd _m_punpckhdq
interleave 32-bit double-words from the high halves of two MM elements
.fd _m_punpckhwd
interleave 16-bit words from the high halves of two MM elements
.fd _m_punpcklbw
interleave bytes from the low halves of two MM elements
.fd _m_punpckldq
interleave 32-bit double-words from the low halves of two MM elements
.fd _m_punpcklwd
interleave 16-bit words from the low halves of two MM elements
.fd _m_pxor
XOR 64 bits from two MM elements
.fd _m_to_int
retrieve low-order 32 bits from MM value
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
.fd assert
test an assertion and output a string upon failure
.fd _fullpath
return full path specification for file
.fd _getmbcp
get current multibyte code page
.fd getopt
a command-line parser that can be used by applications that follow guidelines outlined in the Single UNIX Specification
.fd _harderr
critical error handler
.fd _hardresume
critical error handler resume
.fd localeconv
obtain locale specific conversion information
.fd longjmp
return and restore environment saved by "setjmp"
.fd _lrotl
rotate an "unsigned long" left
.fd _lrotr
rotate an "unsigned long" right
.fd main
the main program (user written)
.fd offsetof
get offset of field in structure
.fd _rotl
rotate an "unsigned int" left
.fd _rotr
rotate an "unsigned int" right
.fd setjmp
save environment for use with "longjmp" function
.fd _makepath
make a full filename from specified components
.fd setlocale
set locale category
.fd _setmbcp
set current multibyte code page
.fd sigsetjmp
save environment and process's signal mask for use with "siglongjmp" function
.fd siglongjmp
return and restore environment and process's signal mask saved by "sigsetjmp"
.fd _splitpath
split a filename into its components
.fd _splitpath2
split a filename into its components
.fd _wfullpath
return full path specification for file
.fd _wmakepath
make a full filename from specified components
.fd _wsetlocale
set locale category
.fd _wsplitpath
split a filename into its components
.fd _wsplitpath2
split a filename into its components
.fdend
.*======================================================================
.section DOS LFN aware Functions
.*
.np
.ix 'DOS LFN aware Functions'
These functions deal with DOS Long File Name if an application is compiled
with -D__WATCOM_LFN__ option and DOS LFN support is available on host system.
.fdbeg
.fd access
test file or directory for mode of access
.fd chdir
change current working directory
.fd chmod
change permissions for a file
.fd creat
create a file
.fd _dos_creat
create a file
.fd _dos_creatnew
create a new file
.fd _dos_findfirst
find first file matching a specified pattern
.fd _dos_getfileattr
get file attributes
.fd _dos_open
open a file
.fd _dos_setfileattr
set the attributes of a file
.fd _findfirst
find first file matching a specified pattern
.fd _fullpath
return full path specification for file
.fd getcwd
get current working directory
.fd _getdcwd
get current directory on drive
.fd lstat
get file status
.fd mkdir
make a new directory
.fd open
open a file
.fd opendir
open directory file
.fd remove
delete a file
.fd rename
rename a file
.fd rmdir
remove a directory
.fd _sopen
open a file for shared access
.fd stat
get file status
.fd tmpnam
create name for temporary file
.fd unlink
delete a file
.fd utime
set modification time for a file
.fd _waccess
test file or directory for mode of access
.fd _wchdir
change current working directory
.fd _wchmod
change permissions for a file
.fd _wcreat
create a file
.fd _wdos_findfirst
find first file matching a specified pattern
.fd _wfindfirst
find first file matching a specified pattern
.fd _wfullpath
return full path specification for file
.fd _wgetcwd
get current working directory
.fd _wgetdcwd
get current directory on drive
.fd _wmkdir
make a new directory
.fd _wopen
open a file
.fd _wopendir
open directory file
.fd _wremove
delete a file
.fd _wrename
rename a file
.fd _wrmdir
remove a directory
.fd _wsopen
open a file for shared access
.fd _wstat
get file status
.fd _wtmpnam
create name for temporary file
.fd _wunlink
delete a file
.fd _wutime
set modification time for a file
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
.reffunc setenv
or
.reffunc putenv
library functions:
.millust begin
    setenv( "TZ", "PST8PDT", 1 );
    putenv( "TZ=PST8PDT" );
.millust end
.np
The value of the variable can be obtained by using the
.reffunc getenv
function:
.millust begin
    char *tzvalue;
     . . .
    tzvalue = getenv( "TZ" );
.millust end
.np
The
.reffunc tzset
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
.reffunc ctime
.ct ,
.reffunc _ctime
.ct ,
.reffunc localtime
.ct ,
.reffunc _localtime
or
.reffunc mktime
is called, the time zone names contained in the external variable
.kw tzname
will be set as if the
.reffunc tzset
function had been called.
The same is true if the
.mono %Z
directive of
.reffunc strftime
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
