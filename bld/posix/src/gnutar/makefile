name = gnutar
objs =  tar.obj create.obj extract.obj buffer.obj getoopt.obj &
        list.obj names.obj port.obj getopt.obj
# MSDOS is defined when compiling for DOSOS2.  Make sure it is set for
# any other configuration
extra_cflags=-DUSG -DMSDOS -DNONAMES -DDEF_AR_FILE="ARCHIVE.TAR" -DDEFBLOCKING=20

!include $(posix_dir)\mif\makeone.mif
