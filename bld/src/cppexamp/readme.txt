Description of CPPEXAMP directory
=================================

Overview
--------

This directory contains a number of sub-directories, each of which is intended
to demonstrate a feature or technique with the C++ compiler.  A list of the
sub-directories follows:

    Directory   Description
    ~~~~~~~~~   ~~~~~~~~~~~
    EXCAREA     Demonstrates how to change the size of the static area used
                to store exception objects.
                
    MEMBFUN     Demonstrates how to call a C++ member function from a function
                compiled with the C compiler.
                

How to use the examples
-----------------------

The examples are intended to be compiled and executed with the sample
sub-directory as the current directory.  WMAKE is used to compile and execute
the example.


Conventions
-----------

(1) Each sample directory contains a file README.TXT that may inspected for
    special instructions regarding the sample.
    
(2) The module MAIN.CPP in each directory contains the main function.

(3) In each directory, MAKEFILE includes a CPPEXAMP.MIF from the CPPEXAMP
    directory in order to effect commonality between the examples.  You
    may wish to modify this file for your own situation.
