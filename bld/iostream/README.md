
New IOStreams
=============

This folder contains a new version of the IOStreams library for Open
Watcom. This version is intended to be as conformant to the C++ standard as
possible given compiler limitations.

This library is being developed completely independently of the main Open
Watcom build (for now) and is configured as a kind of third party library.
Builds of this library are done manually as would be the case for any other
third party library. Once it has reached a certain level of maturity and
completeness this library will (hopefully) be integrated into the main Open
Watcom build and eventually be a replacement for the existing IOStreams
library in the distribution. However, to simplify development and minimize
conflicts and disruption to the main build this library is currently quite
isolated from the rest of the Open Watcom code base.

It is our intention for people interested in experimenting with or
contributing to this library to be able to use it simultaneously with the
regular IOStreams library in the Open Watcom distribution. To this end this
library is contained in namespace 'stream' rather than namespace 'std'.
This, of course, will be changed when and if the library is made the
officially distributed IOStreams library.

For now the header files for this library are given a .hpp extension rather
than being extensionless as required by the standard. This is so they can
be included into a program that is also using the existing IOStreams
library. For example

    #include <iostream>     // From existing library.
    #include <fstream.h>    // "Old" form of header from existing library.
    #include <sstream.hpp>  // From new library.

This also makes it easier to configure syntax aware tools to recognize the
header files as C++ during development.


Design Goals
------------

The primary goal of this implementation of IOStreams is to be as conformant
to the C++ standard (ideally the 2011 standard) as possible using the Open
Watcom C++ compiler. This library is intended to ultimately be a
replacement for the old-style IOStreams library currently distributed with
Open Watcom.

In this first generation implementation this library will be built on top
of the C standard library I/O facilities. While this may be less efficient
than direct operating system access it has the advantage of allowing a
single version of this library to work for all platforms targeted by Open
Watcom. Considering the small developer resources available to build this
library such efficiency in development is more valuable than efficiency in
execution. In the future direct support for certain critical targets could
be added as time and need require.


Contact Information
-------------------

For more information or for questions, concerns, bug reports, etc, please
contact Peter Chapin using PChapin@vtc.vsc.edu.
