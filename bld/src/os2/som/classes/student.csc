include <somobj.sc>

class:
    Student;

-- "Student" class provides a base class to generate more specialized
-- students like "GraduateStudent" and "UnderGraduateStudent"

parent:
    SOMObject;

data:
    char id[16];    /* student id */
    char name[32];  /* student name */

methods:

    void setUpStudent(char *id, char *name);
    -- sets up a new student

    void printStudentInfo();
    -- prints the student information

    char *getStudentType();
    -- returns the student type

    char *getStudentId();
    -- returns the student ID
