NAME = rm
OBJS =  rm.obj

!include $(posix_dir)/mif/targos.mif
extra_lnk = xlnk
!ifeq targos dos
model=s
!else ifeq targos dosos2
model=s
!else
model=f
!endif

!include $(posix_dir)/mif/makeone.mif

xlnk: .PROCEDURE
    @%append $(name).lnk op stack=10k
