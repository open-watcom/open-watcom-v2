#
#   General makefile for creating wmake.  Place this file in an 'o' directory,
#   set the appropriate flags, and then wmake it.
#
#

# ndebug =                  # turn off assertions

# cachestat =               # turn on cache stats

# opts_cc =                 # options for all cc cmd lines

# def_cc =                  # add -dDEFINE flags to cc command line

# -dDEVELOPMENT             # extra code for debugging purposes
                            # (implies -dINTERNAL_VERSION )
# -dINTERNAL_VERSION        # Changes PRODUCT message to reflect internal

# _OS = _OS_DOS             # MSDOS version
# _OS = _OS_OS2             # OS2 version

# this is how to create the DOS side of the release version

_OS     = _OS_DOS
ndebug  =
opts_cc = -zq -oals -s -w4

!include ..\mif\master.mif
