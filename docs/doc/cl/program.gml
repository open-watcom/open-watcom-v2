.pp
This chapter presents an entire C program, to illustrate many of
the features of the language, and to illustrate elements of
programming style.
.pp
This program implements a memo system suitable for maintaining
a set of memos, and displaying them on the screen. The program
allows the user to display memos relevant to today's date,
move through the memos adding new ones and
replacing or deleting existing ones.
The program displays help information whenever an invalid action is
entered, or when the sole parameter to the program is a question mark.
.pp
The program is in complete conformance to the ISO C standard. It should
be able to run, without modification, on any system that provides an
ISO-conforming C compiler.
.*
.section The memos.h File
.*
.pp
The
source file
.mono memos.h
contains the structures used for storing the memos:
..im memos2
.*
..pa
.section The memos.c File
.*
.pp
The source for the program follows:
..im memos
