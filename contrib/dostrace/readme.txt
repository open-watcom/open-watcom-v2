
  trace - trace DOS system calls
  ------------------------------

 This is the DOS trace utility, originally written by Diomidis Spinellis.
It was ported to Open Watcom (original was designed for Microsoft C/C++ 7.0),
slightly updated for additional INT 21h subfunctions, and several minor bugs
have been fixed. The tool is modeled on the trace/strace tool available on
many UNIX systems.

 Usage is documented in trace.man. The PostScript file article.ps contains
author's article about the original version.


 Design notes:

 - The code must be built with -zu (SS != DS). A significant chunk of the
   code runs in the context of other processes with a stack that is unlikely
   to be addressable via trace's DS.

 - As a consequence of the above, many library routines cannot be used. Among
   them is vsprintf(), which is why trace comes with its own printf()-like
   routine of BSD heritage. A side benefit of that routine, apart from being
   usable from within an INT 21h interrupt handler, is that it is smaller
   than the Watcom clib version.

 - The _doprnt() routine is designed to write to a stream, hence the awfully
   ugly hack to pretend the local buffer is part of a FILE stream. This could
   be cleaned up and prettified.

 - This program is a good example of writing complex DOS interrupt handlers
   in C. It is clearly possible, but whether this is ultimately a win is an
   open question.

 - This tool writes output to a file directly from the interrupt handler.
   The advantage of this approach is that data will be written to disk even
   when the traced application crashes and renders the system unusable. The
   disadvantage is that if the DOS Critical Flag (InDOS) is set, no logging
   can be performed. An alternative approach would be storing data in a
   memory buffer, which could be accessed at any time.

 - No attempt is made to trace protected mode activities. However, since DOS
   extenders must call DOS in real mode, this is not a problem in practice
   and DOS extended applications may be easily traced (the -e switch may be
   required to trace across EXEC calls).
