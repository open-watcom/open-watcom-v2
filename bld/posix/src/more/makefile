NAME = more
OBJS =  more.obj

!include $(posix_dir)\mif\targos.mif
!ifeq targos os2v2
extra_lnk = xlnk
!endif

!include $(posix_dir)\mif\makeone.mif

# this is required for OS2V2 because some thunk routines need a big chunk
# of stack space
xlnk: .PROCEDURE
    @%append $(LNK) op stack=20k
