#
# Additional extensions
#
.EXTENSIONS : .ih .sc .csc

#
# Compiler and linker flags
#
CFLAGS = -zq -I.
LFLAGS = op quiet
!ifdef debug
CFLAGS += -d2
LFLAGS += debug all
!endif

#
# Default rules
#
.c.obj : .autodepend
    wcc386 $(CFLAGS) -bd $<

.csc.ih :
    sc -s"ih" -w $<

.csc.h :
    sc -s"h" -w $<

.csc.sc :
    sc -s"sc" -w $<

#
# Targets
#
test.exe : student.dll undgrad.dll graduate.dll course.dll test.obj
    wlink $(LFLAGS) sys os2v2 file $]@ lib somtk,student,course,graduate,undgrad

test.obj : test.c student.h course.h graduate.h undgrad.h
    wcc386 $(CFLAGS) $[@

student.dll : student.ih student.h student.obj
    wlink $(LFLAGS) op impl sys os2v2_dll file $]@ lib somtk export StudentCClassData,StudentClassData,StudentNewClass

undgrad.dll : student.sc undgrad.ih undgrad.h undgrad.obj
    wlink $(LFLAGS) op impl sys os2v2_dll file $]@ lib student,somtk export UnderGraduateStudentCClassData,UnderGraduateStudentClassData,UnderGraduateStudentNewClass

graduate.dll : student.sc graduate.ih graduate.h graduate.obj
    wlink $(LFLAGS) op impl sys os2v2_dll file $]@ lib student,somtk export GraduateStudentCClassData,GraduateStudentClassData,GraduateStudentNewClass

course.dll : course.ih course.h student.h course.obj
    wlink $(LFLAGS) op impl sys os2v2_dll file $]@ lib student,somtk export CourseCClassData,CourseClassData,CourseNewClass

#
# Clean rule
#
clean : .symbolic
    rm -f *.obj *.exe *.dll *.lib *.sc *.h *.ih *.err
