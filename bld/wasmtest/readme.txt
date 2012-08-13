How to use tests for the Open Watcom x86 assembler
--------------------------------------------------

There are two kinds of tests. One is for inline assembler built into the
C and C++ compilers; the other is for standalone wasm assembler. Test results
are logged in testcomp.log and testasm.log, respectively.

1. Inline assembler tests
--------------------------
These tests are located in 'inline' subdirectory. The tests must be run in
that directory. For example to test the 16-bit C compiler, run

wmake testcomp=wcc

(uses first C compiler on the path, if testcomp macro is not defined, wcc386
is the default compiler)

or

wmake testcomp=..\..\..\cc\nt386.i86\wcci86e.exe

(specifies exact C compiler)


2. Standalone assembler (wasm) tests
------------------------------------
These tests are located in three subdirectories named 'test1', 'test2' and
'test3'. The tests must be run in the appropriate subdirectory.

To test the standalone assembler, run

wmake 

(uses the first wasm executable on the path)

or

wmake testasm=..\..\nt386\wasm.exe

(uses specified wasm executable)

Note:
The tests require standard Open Watcom build environment and use the following
tools: wmake, wdis, diff, dmpobj.
