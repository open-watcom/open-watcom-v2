#                             EXAMPLE MAKEFILE
#                             ----------------
#
# DESCRIPTION
# -----------
# Generic make file for a C program.

# EXAMPLE
# -------
# This makefile assumes you are trying to compile a program for OS/2 2.x
# called "example.c" and subroutines contained in two separate files
# called "sub1.c" and "sub2.c".

# INSTRUCTIONS
# ------------
# Set the following macros to appropriate values:
#       Compiler        - the name of the compiler. Either 'wcc386' for
#                         32-bit programs or 'wcc' for 16-bit ones.
#       Compiler_options- what ever options you wish for your compiles.
#       Linker_options  - what ever options you wish for your links.
#       System          - The name of a system from the
#                         "\WATCOM\BINB\WLSYSTEM.LNK" file. This identifies
#                         the target operating system the program is to
#                         run on.
#       Exe_file        - the name of the resulting executable file.
#       Object_files    - the list of object files to be linked together.
#
# If this makefile is called "makefile" then just run WMAKE.
# If it has another name such as "example.mak" then you have to
# give the file name at the command line:
#   WMAKE /f example.mak

# REMARKS
# -------
# Default Watcom compiler options are set using an environment variable
# which can be placed in "config.sys". For instance, one may have:
#
# SET WCC386=/3 /FPI87 /Ox /D1
#
# Then, the Compiler_Options variable below will modify the default.
#
# WVIDEO DEBUGGER SETUP
# ---------------------
# The following environment variable makes WVIDEO a bit nicer in OS/2
# (you can set this environment variable in config.sys)
#
#  SET WVIDEO=/Lines#33
#
# The /Lines#33 parameter makes WVIDEO use a 33 line display which
# in an OS/2 Window on VGA makes more use of the screen area.

# BEGINNING OF MAKE FILE....

##########################
## User settable macros ##
##########################

Compiler = wcc386
#Compiler = wcc

# Compiler_Options =
# Following is best setup for WVIDEO debugger.
Compiler_Options = /D2 /W3

# Linker_options   =
Linker_options   = debug all

System       = os2v2

Exe_file     = example.exe

Object_files = example.obj &
               sub1.obj    &
               sub2.obj

####################
## Makefile rules ##
####################

$(Exe_file): $(Object_files)
             *wlink system $(System) $(Linker_Options) name $(Exe_file) &
                        file {$(Object_files)}

.c.obj:
        *$(Compiler) $(Compiler_Options) $<
