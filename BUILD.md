
Building Open Watcom
====================

This document describes how to build Open Watcom. The Quick Start is an
overview and may be sufficient information for some developers. The
following section gives additional details describing some options
provided by the build system.

Quick Start
-----------

If you haven't done so already you should read the file `README.md`. That
document gives an overview of the project and a quick description of the
layout of the Open Watcom source tree.

You will first need to install a few tools. In particular you will need a
build compiler. Nominally we use Open Watcom as our build compiler, but
other compilers are possible and even necessary when targeting 64 bit
code (Open Watcom currently can't generate 64 bit executables).

Below is list of supported compilers.

                            OW for 16/32-bit hosts   OW for 64-bit hosts
    build OS                supported compilers      supported compilers
    --------------------------------------------------------------------
    DOS                     Open Watcom              -
    OS/2                    Open Watcom              -
    Windows 32-bit          Open Watcom, Visual C++  -
    Windows 64-bit          Open Watcom*, Visual C++ Visual C++ (for x64 target)
    Linux x86 32-bit        Open Watcom, GCC         -
    Linux x64 64-bit        Open Watcom*, GCC        GCC (for x64 target)

    * Using 32 bit Open Watcom on the 64 bit host.

Building the documentation requires additional tools some of which may not
be readily available for all platforms. See `docs/howto.txt` for more
information.

Start by modifying `setvars.bat` (DOS/Windows), `setvars.cmd` (OS/2), or
`setvars.sh` (Linux) to reflect your system. See the comments within the
file for additional information. Of particular interest is the `OWROOT`
environment variable that specifies the location of the source tree. We
recommend that you make a copy of the the `setvars` script before
modifying it.

*WARNING*: Some platforms (notably Windows 9x) require DOS style line
endings in batch files.

The build process consists of two phases. The first phase creates a
minimal set of Open Watcom tools which are sufficient to build the entire
system. The second phase builds all of Open Watcom using the minimal set
of tools built during the first phase.

The build and clean-up processes are handled by two scripts.

1. `build.bat` (DOS/Windows), `build.cmd` (OS/2), or `build.sh` (Linux).
    This script builds all the software by executing both build phases.
    Normally it is sufficient to run this script to build the system. More
    control can be had by using the `builder` command directly as
    described below. A word of warning: running a full build may take
    upwards of two hours even on a fairly fast machine. There is a *lot*
    to build!

    You may want to run `builder cprel` from inside the `bld` directory
    after successfully building everything to copy the complete system to
    the release (`rel`) tree.

2. `clean.bat` (DOS/Windows), `clean.cmd` (OS/2), or `clean.sh` (Linux).
    This script erases all the object files, executable files, etc.
    created by any part of build process so you can build everything from
    scratch.

It is also possible to restore your code base to a pristine state using

    git clean -dfx

This command removes all files and directories unknown to git, including
those that are explicitly ignored. It should leave your code base in
exactly the same state as it had after a fresh clone operation.

More Details
------------

At the top level we have a tool that oversees traversing the build tree,
deciding which projects to build for what platforms, logging the results
to a file, and copying the finished software into the release tree
(`rel`), making fully automated builds a possibility. This tool is called
`builder`. See `\bld\builder\builder.doc` for detailed info on the tool
and the source if the documentation doesn't satisfy you.

Each project has a `builder.ctl` builder script file. If you go to a
project directory and run `builder`, it will make only that project; if
you go to `bld` and run `builder`, it will build everything. The overall
build uses `bld\builder.ctl` which includes all of the individual project
`builder.ctl` files. Note that `builder` will traverse directories upward
until it finds a `builder.ctl`.

The results of the build are logged to `build.log` in the current project
directory (or `bld`), the previous `build.log` file is copied to
`build.lo1`.

Common builder commands:

    builder boot
      - First build phase (bootstrap) only. Creates all tools necessary
        for building entire Open Watcom (phase two).

    builder bootclean
      - Erases objects and tools, created during first build phase.

    builder build
      - Second build phase. Builds the entire system.

    builder cprel
      - Copies the system into the "rel" release tree.

    builder rel
      - Second build phase. Builds the entire system and copies it into
        the rel release tree. This is equivalent to "builder build"
        followed by "builder cprel."

    builder clean
      - Erase object files, executable files, etc. created during second
        build phase so you can do second build phase from scratch.

    builder docs
      - Build all documentation.

    builder docsclean
      - Erases all objects, etc. created during documentation build so you
        can start documentation build from scratch.

    builder test
      - Run all automated tests.

Many of the projects use the `pmake` features of builder (see
`builder.doc`). To determine what to build, `pmake` source is in
`bld\pmake`.

Each `makefile` has a comment line at the top of the file which is read by
`pmake`. Most of our `builder.ctl` files will have a line similar to this:

    pmake -d build -h ...

this will cause `wmake` to be run in every subdirectory where the
`makefile` contains `build` on the `#pmake` line.

You can also specify more parameters to build a smaller subset of files.
This is especially useful if you do not have all required
tools/headers/libraries for all target platforms.

For example:

    builder rel os_nt

will build only the NT (32 bit Windows) version of the tools.

It is generally possible to build specific binaries/libraries by going to
their directory and running `wmake`. For instance to build the OS/2
version of `wlink` you can go to `bld\wl\os2386` and run `wmake` there
(note that the process won't successfully finish unless several required
libraries are built). Builder is useful for making full builds while
running `wmake` in the right spot is handy during development.

Even More Details
-----------------

This information and much more is in the Open Watcom Developer's Guide. To
produce a PostScript version go to `docs\ps` and run

    wmake hbook=devguide

If you have everything set up correctly, you should end up with
`devguide.ps` which you can print or view.

