NAME = more
OBJS =  more.obj

!ifeq host_os os2
!ifeq host_cpu 386
extra_lnk = xlnk
!endif
!endif

!include $(posix_dir)/mif/makeone.mif

# this is required for OS2V2 because some thunk routines need a big chunk
# of stack space
xlnk: .PROCEDURE
    @%append $(name).lnk op stack=20k
