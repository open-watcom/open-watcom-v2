.chap *refid=cpabout About This Manual
.*
.if &e'&dohelp eq 1 .do begin
.if '&lang' eq 'C/C++' .do begin
:HBMP 'cguide.bmp' i
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:HBMP 'fguide.bmp' i
.do end
.do end
.*
.sr $$chpn=0
.dm autochap begin
.sr $$chpn=&$$chpn.+1
.dm autochap end
.*
.np
.ix 'overview of contents'
This manual contains the following chapters:
.begpoint
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cpabout'..
.np
This chapter provides an overview of the contents of this guide.
.*
:CMT. .autochap
:CMT. .point Chapter &$$chpn. &mdash. :HDREF refid='cpinst'..
:CMT. .np
:CMT. This chapter describes how to install &product.. It outlines the
:CMT. minimum hardware configuration requirements for &product. and it
:CMT. describes the package contents.
:CMT. .*
:CMT. .autochap
:CMT. .point Chapter &$$chpn. &mdash. :HDREF refid='cptutor'..
:CMT. .np
:CMT. This chapter provides a hands-on introduction to &product.. In the
:CMT. tutorial, you take an existing set of C++ source files and use them as
:CMT. a basis to create a project in the new integrated development
:CMT. environment. The tutorial guides you through the phases of application
:CMT. development using &company's new suite of graphical development tools
:CMT. including the debugger, profiler, and resource editors.
.*
.if '&lang' eq 'C/C++' .do begin
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cpopts'..
.np
This chapter provides a summary and reference section for all the
C and C++ compiler options.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cpwcc'..
.np
This chapter describes how to compile an application from the command
line.
This chapter also describes
compiler environment variables,
benchmarking hints,
compiler diagnostics,
#include file processing,
the preprocessor,
predefined macros,
extended keywords,
and the code generator.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cphdr'..
.np
This chapter describes the use of precompiled headers to speed up
compilation.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cplibr'..
.np
This chapter describes the &product
library directory structure,
C libraries,
class libraries,
math libraries,
80x87 math libraries,
alternate math libraries,
the "NO87" environment variable, and
the run-time initialization routines.
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='fpopts'..
.np
This chapter also provides a summary and reference section for the
valid compiler options.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='fpwfc'..
.np
This chapter
describes how to compile an application from the command line,
describes compiler environment variables,
provides examples of command line use of the compiler,
and
and describes compiler diagnostics.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='fplibr'..
.np
This chapter describes the &product run-time libraries.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='fcond'..
.np
This chapter describes compiler directives including
INCLUDE file processing.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='fdosfil'..
.np
This chapter describes run-time file handling.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='flib'..
.np
This chapter describes subprograms available for special
operations.
.*
.do end
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='mdl86'..
.np
This chapter describes the &product
memory models (including code and data models),
the tiny memory model,
the mixed memory model,
linking applications for the various memory models,
creating a tiny memory model application,
and
memory layout in an executable.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cal86'..
.np
This chapter describes issues relating to 16-bit interfacing
such as parameter passing conventions.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='prg86'..
.np
This chapter describes the use of pragmas with the 16-bit compilers.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='mdl386'..
.np
This chapter describes the &product
memory models (including code and data models),
the flat memory model,
the mixed memory model,
linking applications for the various memory models,
and
memory layout in an executable.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cal386'..
.np
This chapter describes issues relating to 32-bit interfacing
such as parameter passing conventions.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='prg386'..
.np
This chapter describes the use of pragmas with the 32-bit compilers.
.*
.if '&lang' eq 'C/C++' .do begin
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cpasm'..
.np
This chapter describes in-line assembly language programming using the
auxiliary pragma.
.*
.if '&target' ne 'QNX' .do begin
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='cromabl'..
.np
This chapter discusses some embedded systems issues as they pertain to
the C library.
.*
.do end
.*
.if '&target' eq 'QNX' .do begin
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='wlusage'..
.np
This chapter introduces the &lnkname..
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='genchap'..
.np
This chapter describes the &lnkname directives and options that apply
to QNX in alphabetical order.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='qnxchap'..
.np
This chapter describes the QNX executable file format.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='lerrmsg'..
.np
This chapter explains the &lnkname error messages.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='wlib'..
.np
This chapter describe the &libname..
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='wasm'..
.np
This chapter describe the &asmname..
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='wdisasm'..
.np
This chapter describe the &disname..
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='wlfcopt'..
.np
This chapter describes the optimization of far calls.
.*
.autochap
.point Chapter &$$chpn. &mdash. :HDREF refid='wstrip'..
.np
This chapter describe the &stripname..
.*
.do end
.*
.do end
.*
.point Appendix A. &mdash. :HDREF refid='environ'..
.np
This appendix describes all the environment variables used by the
compilers and related tools.
.*
.if '&lang' eq 'C/C++' .do begin
.*
.if &e'&dohelp eq 0 .do begin
.point Appendix B. &mdash. :HDREF refid='cmsgs'..
.np
This appendix lists all of the &cmpcname diagnostic messages with an
explanation for each.
.*
.point Appendix C. &mdash. :HDREF refid='cppmsgs'..
.np
This appendix lists all of the &cmppname diagnostic messages with an
explanation for each.
.*
.point Appendix D. &mdash. :HDREF refid='crtmsgs'..
.do end
.el .do begin
.point Appendix B. &mdash. :HDREF refid='crtmsgs'..
.do end
.np
This appendix lists all of the &lang run-time diagnostic messages with
an explanation for each.
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.if &e'&dohelp eq 0 .do begin
.point Appendix B. &mdash. :HDREF refid='ferrmsg'..
.np
This appendix lists all of the &cmpname diagnostic messages with an
explanation for each.
.do end
.*
.do end
.endpoint
