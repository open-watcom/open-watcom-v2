main(n,args)               /* C REFERENCE MANUAL         */
int n;
char **args;
{

/*   This program performs a series of tests on a C compiler,
based on information in the

             C REFERENCE MANUAL

which appears as Appendix A to the book "The C Programming
Language" by Brian W. Kernighan and Dennis M. Ritchie
(Prentice-Hall, 1978, $10.95). This Appendix is hereafter
referred to as "the Manual".

     The rules followed in writing this program are:

     1. The entire program is written in legal C, according
     to the Manual. It should compile with no error messages,
     although some warning messages may be produced by some
     compilers. Failure to compile should be interpreted as
     a compiler error.

     2. The program is clean, in that it does not make use
     of any features of the operating system on which it runs,
     with the sole exceptions of the printf() function, and an
     internal "options" routine, which is easily excised.

     3. No global variables are used, except for the spec-
     ific purpose of testing the global variable facility.

     The program is divided into modules having names of the
form snnn... These modules correspond to those sections of the
Manual, as identified by boldface type headings, in which
there is something to test. For example, s241() corresponds
to section 2.4.1 of the Manual (Integer constants) and tests
the facilities described therein. The module numbering
scheme is ambiguous, especially when it names modules
referring to more than one section; module s7813, for ex-
ample, deals with sections 7.8 through 7.13. Nonetheless,
it is surprisingly easy to find a section in the Manual
corresponding to a section of code, and vice versa.

     Note also that there seem to be "holes" in the program,
at least from the point of view that there exist sections in the
Manual for which there is no corresponding code. Such holes
arise from three causes: (a) there is nothing in that partic-
ular section to test, (b) everything in that section is tested
elsewhere, and (c) it was deemed advisable not to check cer-
tain features like preprocessor or listing control features.

     Modules are called by a main program main(). The mod-
ules that are called, and the sequence in which they are 
called, are determined by two lists in main(), in which the
module names appear. The first list (an extern statement)
declares the module names to be external. The second (a stat-
ic int statement) names the modules and defines the sequence
in which they are called. There is no need for these lists
to be in the same order, but it is probably a good idea to keep
them that way in the interest of clarity. Since there are no
cross-linkages between modules, new modules may be added,
or old ones deleted, simply by editing the lists, with one
exception: section s26, which pokes around at the hardware
trying to figure out the characteristics of the machine that
it is running on, saves information that is subsequently
used by sections s626, s72, and s757. If this program is
to be broken up into smallish pieces, say for running on
a microcomputer, take care to see that s26 is called before
calling any of the latter three sections.  The size
of the lists, i.e., the number of modules to be called, is
not explicitly specified as a program parameter, but is
determined dynamically using the sizeof operator.

     Communication between the main program and the modules
takes place in two ways. In all cases, a pointer to a structure
is passed to the called module. The structure contains flags
that will determine the type of information to be published
by the module, and fields that may be written in by the
module. The former include "flgm" and "flgd", which, if set
to a nonzero value, specify that machine dependencies are to
be announced or that error messages are to be printed, re-
spectively. The called module's name, and the hardware char-
acteristics probed in s26() comprise the latter.


     Also, in all cases, a return code is returned by the called
module. A return code of zero indicates that all has gone well;
nonzero indicates otherwise. Since more than one type of error
may be detected by a module, the return code is a composite
of error indicators, which, individually, are given as numbers
that are powers of two. Thus, a return code of 10 indicates
that two specific errors, 8 and 2, were detected. Whether or
not the codes returned by the modules are printed by the main
program is determined by setting "flgs" to 1 (resp. 0).

     The entire logic of the main program is contained in the
half-dozen or so lines at the end. The somewhat cryptic 
statement:

           d0.rrc = (*sec[j])(pd0);

in the for loop calls the modules. The rest of the code is
reasonably straightforward.

     Finally, in each of the modules, there is the following
prologue:

           snnn(pd0)
           #include "defs"
           struct defs *pd0;
           {
              static char snnner[] = "snnn,er%d\n";
              static char qsnnn[8] = "snnn   ";
              char *ps, *pt;
              int rc;

              rc = 0;
              ps = qsnnn;
              pt = pd0->rfs;
              while(*pt++ = *ps++);

used for housekeeping, handshaking and module initialization.

                                                           */
#include "defs"
   extern
     s22(),
     s241(),
     s243(),
     s244(),
     s25(),
     s26(),
     s4(),
     s61(),
     s626(),
     s71(),
     s72(),
     s757(),
     s7813(),
     s714(),
     s715(),
     s81(),
     s84(),
     s85(),
     s86(),
     s88(),
     s9()
   ;

   int j;
   static int (*sec[])() = {
     s22,
     s241,
     s243,
     s244,
     s25,
     s26,
     s4,
     s61,
     s626,
     s71,
     s72,
     s757,
     s7813,
     s714,
     s715,
     s81,
     s84,
     s85,
     s86,
     s88,
     s9
   };

   static struct defs d0, *pd0;
    
     d0.flgs = 1;          /* These flags dictate            */
     d0.flgm = 1;          /*     the verbosity of           */
     d0.flgd = 1;          /*         the program.           */
     d0.flgl = 1;

   pd0 = &d0;

   for (j=0; j<sizeof(sec) / sizeof(sec[0]); j++) {
     d0.rrc = (*sec[j])(pd0);
     d0.crc = d0.crc+d0.rrc;
     if(d0.flgs != 0) printf("Section %s returned %d.\n",d0.rfs,d0.rrc);
   }
  
   if(d0.crc == 0) printf("\nNo errors detected.\n");
   else printf("\nFailed.\n");
}
