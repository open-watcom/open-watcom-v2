Note on wsplice;

Wsplice originally had its own directory. There the makefile for ntaxp
specified extra link options:
reference mainCRTStartup option start=mainCRTStartup
The builder program, which was initially the only one in the builder
directory, did not have such extra options, and it is, like wsplice, a
console program. Therefore I have omitted extra link options for
wsplice. This file is a reminder of what has been changed in case there are
problems in the future.
Mat Nieuwenhoven 20020623
