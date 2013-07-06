Note: This information and much more is now in the Open Watcom Developer's
Guide. To produce a PostScript version go to docs\ps and run

 wmake hbook=devguide

If you have everything set up correctly, you should end up with devguide.ps
which you can print or view.
-------------------------------------------------------------------------------


  Getting up to Speed on Open Watcom
  ==================================

Here's a quick intro on the build layout and build process. Original
Sybase build tree was more or less fixed to d:\, Open Watcom is more
flexible and the build tree root can be located anywhere. Where this
file refers to d:\, substitute your Open Watcom root location (stored
in the owroot environment variable).

1) Default build machine layout:

d:\
    bld     - this is the root of the build tree
              each project we build has a subdirectory under d:\bld
              for example:
              d:\bld\cg - code generator
              d:\bld\cc - C compiler
              d:\bld\plusplus - C++ compiler
              etc. (see projects.txt for details)

    build   - various files used by building tools
              of most interest are the .ctl files - scripts for the "builder"
              tool (see below) and make files (makeint et al.).

    docs    - here is everything related to documentation, sources and tools

    distrib - contains manifests and scripts for building binary
              distribution packages

    contrib - 3rd party source code which is not integral part of
              Open Watcom. Contains primarily several DOS extenders.

    ---------

    build/bin - this is where all build tools created during phase 1 are placed.

    rel     - this is default location where the software we actually ship gets
              copied after it is built - it matches the directory structure of 
              our shipping Open Watcom C/C++/FORTRAN tools. 

    Note: the rel directory structure is created on the fly.
          location of rel tree can be changed by OWRELROOT environment variable


------------------------------
2) To set up a new machine, modify setvars.bat/setvars.cmd/setvars.sh to
   reflect your setup. See comments within the file for additional
   information.

    Your path should look something like this if you are on NT:

    PATH=C:\WINNT\system32;
    C:\WINNT;
    d:\bat;
    d:\lang\binnt;
    d:\lang\binw;
    d:\bin;
    d:\tools;
    d:\bld\build\binnt

  WARNING: Some platforms (notably Windows 9x) require DOS style line endings
  in batch files. If source files were moved from a UNIX platform. as is the
  case with ow_daily.tar.bz2, you must make sure that line endings are
  properly converted. Loading the batch files in an editor such as EDIT and
  saving them is one way to do this.
  
------------------------------
3) Build process:

WGML OW tool is still only available as DOS version that on platforms
where can not be run some DOS emulator is necessary.
Under Linux, DOSEMU and FREEDOS need to be installed.
Under 64-bit Windows, DOSBOX need to be installed.

Build process consists from two phases.
First one creates minimal set of OW tools which are sufficient to build full OW.
Second one build full OW by minimal set of pre-build OW tools from phase 1.

Bellow is list of supported compilers.

			OW for 16/32-bit hosts	OW for 64-bit hosts
build OS		supported compilers	supported compilers
DOS			Open Watcom		-
OS/2			Open Watcom		-
Windows 32-bit		Open Watcom, Visual C++	-
Windows 64-bit		Visual C++		Visual C++ (for x64 target)
Linux x86 32-bit	Open Watcom, GCC	-
Linux x64 64-bit	GCC			GCC (for x64 target)

Note: OW for 64-bit hosts can be build only on appropriate build OS, 
    because it uses native 64-bit compilers (OW doesn't support 64-bit targets)

We use the Open Watcom C/C++ compilers and Open Watcom wmake to build our
tools, but at the top level we have a tool which oversees traversing the
build tree, deciding which projects to build for what platforms, logging
the results to a file, and copying the finished software into the release
tree (rel), making fully automated builds a possibility.

This tool is called builder.

See \bld\builder\builder.doc for detailed info on the tool and the source
if the documentation doesn't satisfy you.

Here's how we use it:

Each project has a "builder.ctl" builder script file.
If you go to a project directory and run builder, it will make only that
project; if you go to \bld and run builder, it will build everything
the overall build uses \bld\builder.ctl which includes all of the individual
project builder.ctl files that we use. Note that if you run builder, it will
traverse directories upwards until it finds a builder.ctl (or it hits the
root and still doesn't find anything, but then you must have done something
wrong).

Results are logged to "build.log" in the current project directory (or d:\bld),
the previous build.log file is copied to build.lo1.

Common commands:

builder build - build the software
builder rel   - build the software, and copy it into the "rel" release tree
builder cprel - copy software into the "rel" release tree
builder clean - erase object files, exe's, etc. so you can build from scratch

Many of the projects use the "pmake" features of builder (see builder.doc).
To determine what to build, pmake source is in \bld\pmake.

Each makefile has a comment line at the top of the file which is read by pmake.
Most of our builder.ctl files will have a line similar to this:

pmake -d build -h ...

this will cause wmake to be run in every subdirectory where the makefile
contains "build" on the #pmake line.

You can also specify more parmeters to build a smaller subset of files. This
is especially useful if you do not have all required tools/headers/libraries
for all target platforms.

For example:

builder rel os_nt

will (generally) build only the NT version of the tools.

A word of warning: running a full build may take upwards of two hours on
a ~1GHz machine. There is a LOT to build! This is not your ol' OS kernel
or a single-host, single-target C/C++ compiler.

It is generally possible to build specific binaries/libraries by going to
their directory and running wmake. For instance to build the OS/2 version
of WLINK you can go to \bld\wl\os2386 and run wmake there (note that the
process won't successfully finish unless several required libraries are built).
Builder is useful for making full builds while running wmake in the right spot
is handy during development.

------------------------------
4) Getting more information:

If you have any further questions (and you will if you're serious), visit
http://www.openwatcom.org/ and subscribe to the Open Watcom newsgroups at
news://news.openwatcom.org/, particularly openwatcom.contributors.
