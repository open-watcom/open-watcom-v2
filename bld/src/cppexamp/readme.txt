Description of cppexamp directory
=================================

Overview
--------

This directory contains a number of sub-directories, each of which is intended
to demonstrate a feature or technique with the C++ compiler.  A list of the
sub-directories follows:

    Directory   Description
    ~~~~~~~~~   ~~~~~~~~~~~
    excarea     Demonstrates how to change the size of the static area used
                to store exception objects.
                
    membfun     Demonstrates how to call a C++ member function from a function
                compiled with the C compiler.

    rtti        Demonstrates how to use run time type identification to
                invoke class-specific member functions on objects in a
                heterogeneous collection.
                

How to use the examples
-----------------------

The examples are intended to be compiled and executed with the sample
sub-directory as the current directory.  The wmake utility is used to compile
and execute the example.


Conventions
-----------

(1) Each sample directory contains a file readme.txt that may inspected for
    special instructions regarding the sample.
    
(2) The module main.cpp in each directory contains the main function.

(3) In each directory, makefile includes a cppexamp.mif from the cppexamp
    directory in order to effect commonality between the examples.  You
    may wish to modify this file for your own situation.
