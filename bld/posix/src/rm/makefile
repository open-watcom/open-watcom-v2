NAME = rm
OBJS =  rm.obj

extra_lnk = xlnk
!ifeq host_cpu i86
model=s
!else
model=f
!endif

!include $(posix_dir)/mif/makeone.mif

xlnk: .PROCEDURE
    @%append $(name).lnk op stack=10k
