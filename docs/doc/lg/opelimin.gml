.*
.*
.option ELIMINATE
.*
.np
The "ELIMINATE" option can be used to enable dead code elimination.
.ix 'dead code elimination'
Dead code elimination is a process the linker uses to remove
unreferenced segments from the application.
The linker will only remove segments that contain code; unreferenced
data segments will not be removed.
.np
The format of the "ELIMINATE" option (short form "EL") is as follows.
.mbigbox
    OPTION ELIMINATE
.embigbox
.*
.begnote $break
.*
.note Linking C/C++ Applications
.*
Typically, a module of C/C++ code contains a number of functions.
When this module is compiled, all functions will be placed in the same
code segment.
The chances of each function in the module being unreferenced are
remote and the usefulness of the "ELIMINATE" option is greatly
reduced.
.np
.ix '&company C/C++ options' 'zm'
.ix 'zm compiler option (&company C/C++)'
In order to maximize the effect of the "ELIMINATE" option, the "zm"
compiler option is available to tell the &company C/C++ compiler to
place each function in its own code segment.
This allows the linker to remove unreferenced functions from modules
that contain many functions.
.np
Note, that if a function is referenced by data, as in a jump table,
the linker will not be able to eliminate the code for the function
even if the data that references it is unreferenced.
.*
.note Linking FORTRAN 77 Applications
.*
The &cmpfname compiler always places each function and subroutine in
its own code segment, even if they are contained in the same module.
Therefore when linking with the "ELIMINATE" option the linker will be
able to eliminate code on a function/subroutine basis.
.*
.endnote
