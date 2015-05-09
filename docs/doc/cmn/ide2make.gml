.chap The IDE2MAKE Utility
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'IDE' 'make'
This chapter describes the &id2mkname utility.
.ix 'generating' 'makefile'
&id2mkname loads an IDE project file and, using the associated .tgt
files, generates make files that can be invoked with &makcmdup..
If the .wpj file does not exist, a default project is used. If any
tgtfile(s) are specified, they are used as the targets in creating the
make files.
.np
The &id2mkname command line syntax is:
.ix '&id2mkname' 'command line format'
.ix 'command line format' '&id2mkname'
.ix 'invoking &id2mkname'
.mbigbox
&id2mkcmd [options] [tgtfile]
.embigbox
.pc
The square brackets [ ] denote items which are optional.
At least one item must be specified, otherwise the program usage is shown.
.begnote
.note options
is a list of valid options, each preceded by a slash
("/") or a dash ("&minus.").
Options may be specified in any order.
.note tgtfile
is the file specification for the targetfile to be used.
Any number of file specifications may be listed.
.endnote
.np
The following is a description of the options available.
.begnote $break $compact
.note p <wpjfile>
loads wpjfile.wpj (project.wpj by default)
.note c <cfgfile>
loads cfgfile instead of ide.cfg
.note i
directory to search configuration files
.note d
generate makefiles using development switch set
.note l
generate makefiles with long lines (no length limit)
.note r
generate makefiles using release switch set
.note h <number>
generate makefiles for selected host OS (default is current host)
.br
0 - Windows 3.x
.br
1 - OS/2 PM
.br
2 - Windows NT
.br
3 - Win-OS/2
.br
4 - Windows 95
.br
5 - Japanese Windows 3.x on an IBM
.br
6 - Japanese Windows 3.x on a Nec98
.br
7 - Dec Alpha (Windows NT)
.br
8 - DOS
.br
9 - Linux
.endnote
.*
.section &id2mkname Operation
.*
.np
&id2mkname is used to create makefiles from &vip project and
targetfiles.
If no targetfile is specified, makefiles for all targets are generated.
