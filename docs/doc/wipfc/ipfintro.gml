.*
.chap *refid='ipfintro' Introduction
.*
.np
Documentation for programs in the form of help files can make the user's life much easier.
The &ipfname produces such documentation for the OS/2 operating system's Information Presentation
Facility (IPF). It consumes text documents containing special IPF tags, commands, and (optionally)
bit-mapped graphics, and outputs help files in the .inf or .hlp formats.
.np
The IPF tags are similar to other SGML tags, the most familiar of which is likely HTML. The IPF tags
allow you, the documentation author, to:
.begbull
.bull
Specify the source (highlighted text or regions of an image) and destination of hyper-link jumps
.bull
Set right and left text margins
.bull
Create lists, footnotes, notes, and various other kinds of notices
.bull
Create tables
.bull
Add illustrations in the form of examples, text figures, and bit-mapped graphics
.bull
Change the size, style, and color of displayed text
.bull
Change whether text spacing and new lines in the source are significant or not
.bull
Customize the behavior of windows in the help viewer
.bull
Create communication links to other applications
.endbull
.np
This document provides a basic description of how tags are used (:HDREF refid='ipftags'.) as well as a 
complete reference for each tag (:HDREF refid='ipftref'.). For more advanced topics, please refer to IBM's
"IPF Programming Guide and Reference" (ipfref.inf) which is available on the internet.

