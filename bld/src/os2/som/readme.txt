 SOM (System Object Model) and WPS (Worplace Shell) Programming Examples
 -----------------------------------------------------------------------

 The SOM programming examples require the IBM SOMobjects Developer Toolkit.
This toolkit is not distributed with Open Watcom; it is part of the IBM OS/2
Developer's Toolkit. These examples are intended to be used in conjunction
with version 4.5 of the IBM OS/2 Developer's Toolkit, or with version 2.1 of
the standalone IBM SOMobjects Developer Toolkit.

 Makefiles are provided for the examples. However, the makefiles assume that
the environment is properly set up to enable use of the SOMobjects Toolkit.
In particular, the following environment variables must be set:

    PATH     - must contain the SOMobjects Compiler, 'sc'
    INCLUDE  - must contain the SOM include directory
    LIB      - must contain the SOM library directory

 The SOMobjects Toolkit may require additional environment variables
to be set.

 The aim of these examples is demonstrating how to use the Open Watcom
C and C++ compilers to develop SOM and WPS classes. 

 For information on SOM programming concepts and details of the SOMobjects
compiler usage, please refer to "System Object Model Programming Guide" and
"System Object Model Programming Reference", both included with the IBM
SOMobjects Developer Toolkit.

 For information on WPS programming concepts, please refer to the "Workplace
Shell Programming Guide" and "Workplace Shell Programming Reference", both
included in the IBM OS/2 Developer's Toolkit.

 Note: Building the C sample programs (helloc and animals ) assumes that SOM
header files have the C++ or OIDL-compatible form, generated with 'somstars'
(as opposed to 'somcorba'). The makefile explicitly specifies the -maddstar
option to the SOM compiler (sc), which is equivalent to having the SMADDSTAR
environment variable set.
