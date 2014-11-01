
Open Watcom v2 Unofficial Fork
==============================

This document gives an overview of this project and a quick description of
the source tree layout. For build instructions see the file `BUILD.md`.

Overview
--------

This project is a friendly fork of [Open
Watcom](http://www.openwatcom.org/). The primary purpose of this fork is
to forward development on 64 bit support in the Open Watcom tools. We are
interested in both creating tools that are themselves 64 bit executables
and also that can generate and manipulate 64 bit code. However, all
reasonable contributions intended to improve or extend Open Watcom are
also welcome.

We owe a debt of thanks for all the work done on Open Watcom over the
years by contributors too numerous to name here. Without their efforts
Open Watcom would not be where it is today. We hope to continue developing
and improving Open Watcom in the same tradition.

[Binary installers](http://sourceforge.net/projects/openwatcom/) of this
version of Open Watcom can be downloaded from SourceForge. The binaries
are refreshed on an irregular schedule. Bug reports, enhancement requests,
and related discussion should be done on the GitHub site issues list.

Source Tree Layout
------------------

Open Watcom allows you to place the source tree almost anywhere (although
we recommend avoiding paths containing spaces). The root of the source
tree should be specified by the `OWROOT` environment variable in `setvars`
(as described in `BUILD.md`). All relative paths in this document are
taken relative to that location. Also this document uses the backslash
character as a path separator as is normal for DOS, Windows, and OS/2. Of
course on Linux systems a slash character should be used instead.

The directory layout is as follows:

    bld
      - The root of the build tree. Each project has a subdirectory under
        bld. For example:
          bld\cg       -> code generator
          bld\cc       -> C compiler
          bld\plusplus -> C++ compiler
          (see projects.txt for details)

    build
      - Various files used by building tools. Of most interest are the
        *.ctl files which are scripts for the builder tool (see below)
        and make files (makeint et al.).

    docs
      - Here is everything related to documentation, sources and tools.

    distrib
      - Contains manifests and scripts for building binary distribution
        packages.

    contrib
      - Third party source code which is not integral part of Open Watcom.
        This directory contains especially several DOS extenders.

    build\bin
      - This is where all build tools created during phase one are placed.

    rel
      - This is default location where the software we actually ship gets
        copied after it is built - it matches the directory structure of
        our shipping Open Watcom C/C++/FORTRAN tools. You can install the
        system by copying the rel directory to your host and then setting
        several environment variables.

        Note: the rel directory structure is created on the fly. The
        location of rel tree can be changed by `OWRELROOT` environment
        variable.
