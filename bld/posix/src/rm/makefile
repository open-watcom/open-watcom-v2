NAME = rm
OBJS =  rm.obj

extra_lnk = xlnk

!include $(posix_dir)/makeone.mif

!ifeq host_cpu i86
model=s
!else
model=f
!endif

xlnk: .PROCEDURE
    @%append $(name).lnk op stack=10k
