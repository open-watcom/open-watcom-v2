include <student.sc>

class:
    UnderGraduateStudent;

parent:
    Student;

data:
    char date[16];   /* graduation date */
methods:
    override printStudentInfo;
    override getStudentType;
    void setUpUnderGraduateStudent(char *id, char *name, char *date);
