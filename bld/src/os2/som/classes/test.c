#include <student.h>
#include <course.h>
#include <graduate.h>
#include <undgrad.h>

void main(void)
{
    Course *course = CourseNew();
    GraduateStudent *jane = GraduateStudentNew();
    UnderGraduateStudent *mark = UnderGraduateStudentNew();

    /* SOM_TraceLevel = 2; */

    _setUpCourse(course, "303", "Compilers", "Dr. David Johnson", 3, 15);
    _setUpGraduateStudent(jane, "423538", "Jane Brown",
        "Code Optimization", "Ph.D.");
    _setUpUnderGraduateStudent(mark, "399542", "Mark Smith", "12/17/92");
    _addStudent(course, jane);
    _addStudent(course, mark);
    _printCourseInfo(course);
}
