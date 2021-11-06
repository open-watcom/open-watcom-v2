NAME = more
OBJS =  more.obj

# this is required for OS2V2 because some thunk routines need a big chunk
# of stack space
!ifeq host_os os2
!ifeq host_cpu 386
extra_link = op stack=20k
!endif
!endif

!include $(posix_dir)/makeone.mif
