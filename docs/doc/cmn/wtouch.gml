.chap The Touch Utility
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'Touch'
.ix 'make' 'Touch'
.ix 'modification'
.ix 'touch utility'
.ix 'time-stamp'
This chapter describes the &tchname utility.
.ix 'setting' 'modification date'
.ix 'setting' 'modification time'
&tchname will set the time-stamp (i.e., the modification date and time)
of one or more files.
The new modification date and time may be the current
date and time, the modification date and time of another file, or
a date and time specified on the command line.
This utility is normally used in conjunction with the &makname
utility.
The rationale for bringing a file up-to-date without altering its
contents is best understood by reading the chapter which describes the
Make utility.
.np
The &tchname command line syntax is:
.ix '&tchcmdup' 'command line format'
.ix 'command line format' '&tchcmdup'
.ix 'invoking &tchname'
.mbigbox
&tchcmdup [options] file_spec [file_spec...]
.embigbox
.pc
The square brackets [ ] denote items which are optional.
.begnote
.note options
is a list of valid options, each preceded by a slash
("/") or a dash ("&minus.").
Options may be specified in any order.
.note file_spec
is the file specification for the file to be touched.
Any number of file specifications may be listed.
The wild card characters "*" and "?" may be used.
.endnote
.np
The following is a description of the options available.
.begnote $break $compact
.note c
do not create an empty file if the specified file does not exist
.note d <date>
specify the date for the file time-stamp in "mm-dd-yy" format
.note f <file>
use the time-stamp from the specified file
.note i
increment time-stamp before touching the file
.note q
suppress informational messages
.note r
touch file even if it is marked read-only
.note t <time>
specify the time for the file time-stamp in "hh:mm:ss" format
.note u
use USA date/time format regardless of country
.note ?
display help screen
.endnote
.*
.section &tchcmdup Operation
.*
.np
&tchcmdup is used to set the time-stamp (i.e., the modification date
and time) of a file.
The contents of the file are not affected by this operation.
If the specified file does not exist, it will be created as an empty
file.
.ix '&tchcmdup options'
This behaviour may be altered with the "c" option so that if the file
is not present, a new empty file will not be created.
.exam begin
(will not create myfile.dat)
C>&tchcmd /c myfile.dat
.exam end
.pc
If a wild card file specification is used and no files match the pattern,
no files will have their time-stamps altered.
The date and time that all the specified files are set to is determined
as follows:
.autonote
.note
The current date and time is used as a default value.
.note
A time-stamp from an "age file" may replace the current date and time.
The "f" option is used to specify the file that will supply the time-stamp.
.exam begin
(use the date and time from file "last.tim")
C>&tchcmd /f last.tim file*.dat
.exam end
.note
The date and/or time may be specified from the command line
to override a part of the time-stamp that will be used.
The "d" and "t" options are used to override the date and time respectively.
.exam begin
(use current date but use different time)
C>&tchcmd /t 2:00p file*.dat
(completely specify date and time)
C>&tchcmd /d 10-31-90 /t 8:00:00 file*.dat
(use date from file "last.tim" but set time)
C>&tchcmd /f last.tim /t 12:00 file*.dat
.exam end
.pc
The format of the date and time on the command line depends on the
country information provided by the host operating system.
&tchname should accept dates and times in a similar format
to any operating system utilities (i.e., the DATE and TIME utilities
provided by DOS).
The "a" and "p" suffix is an extension to the time syntax for specifying
whether the time is A.M. or P.M., but this is only available if
the operating system is not configured for military or 24-hour time.
.endnote
