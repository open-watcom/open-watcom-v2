.*
.chap *refid='ipfcmds' Using IPF Commands
.*
.np
In addition to tags, the &ipfname recognizes 5 commands (or control words) that invoke special processing
procedures. Each command must start with a period ('.') in the first column of a line, followed by the name
of the command. Commands that start anywhere except the first column of a line will be treated as regular text.
.np
The &ipfname recognizes these commands:
.begnote $compact $setptnt 12
.notehd1 Command
.notehd2 Description
.note .*
Begin a comment.
.note .br
Add a line break (start a new line).
.note .ce
Center a single line of text.
.note .im filename
Imbed (include)
.keyword filename
as if it were text in the current file.
.note .nameit
Define a macro for text substitution.
.endnote
.*
.section Comment
.ix 'Comment'
.ix '.*'
.ix 'Command' 'comment'
.ix 'Command' '.*'
.*
.np
Comments are always useful. They allow you to document your documentation! Any text on a line beginning with
the comment command (.*) is passed over without further processing.
.np
Don't put a comment within a tag, or between a tag and text that must immediately follow it.
.*
.section *refid='br' br
.ix 'Break'
.ix '.br'
.ix 'Command' 'break'
.ix 'Command' '.br'
.*
There are times when you want to interrupt the normal flow of text and begin a new line. The .br command
does just that. It must be the only thing on  the line.
.np
Any text following a &period.br command on the same line will be ignored.
.*
.section *refid='ce' ce
.ix 'Center'
.ix '.ce'
.ix 'Command' 'center'
.ix 'Command' '.ce'
.*
The ce command (.ce) centers the line of text that follows it.
.np
The single line of text may not contain any other tags, but may contain nameit and entity references.
.*
.section *refid='imbed' imbed
.ix 'Imbed'
.ix '.im'
.ix 'Commands' 'imbed'
.ix 'Commands' '.im'
.*
.np
The &ipfname allows source files to be embedded in other source files. The top-most file is the master file.
The imbed command (.im) is followed by the name of the file to include. This file is then processed as if it
were in-line with the document including it.
.np
The ability to include other files makes it possible to break large documents into smaller, more manageable 
logical chunks.
.begnote
.note Note:
The embedded files cannot contain the userdoc and euserdoc tags (see :HDREF refid='userdoc'.). Only the master 
document may contain these tags.
.endnote
.np
The embedded files must be in the current directory or in a directory listed in the IPFCIMBED environment 
variable (see :HDREF refid="ipfenv".). Otherwise, a full path name must be used.
.*
.section *refid='nameit' nameit
.ix 'Nameit'
.ix '.nameit'
.ix 'Command' 'nameit'
.ix 'Command' '.nameit'
.*
.np
The nameit command creates a text-substitution macro. The nameit command takes two parameters (
.keyword symbol
=name and
.keyword text
='string') that can appear in any order.
.np
The
.keyword symbol
parameter is the name of the macro. Symbol names were limited to 10 characters (0-9A-Za-z) for the 
original OS/2 help compiler, but &ipfname does not have this restriction. Note that the '&' character 
is not part of the symbol name, but is used when referencing the symbol.
.np
The 
.keyword text
parameter defines 'string' as the text to be substituted on each occurrence of &amp.name. in the text. 
.np
A .nameit symbol is used in the same way as an entity reference.

