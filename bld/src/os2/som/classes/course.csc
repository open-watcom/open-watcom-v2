include <somobj.sc>

class:
    Course;

-- "Course" class describes the interfaces required to setup the course
-- information. The students are "Student" class type and can be added to
-- or dropped from courses through the "addStudent" and "dropStudent"
-- methods

parent:
    SOMObject;

data:
    char    code[8];          /* course code number */
    char    title[32];        /* course title */
    char    instructor[32];   /* name of the instructor teaching */
    int     credit;           /* number of credits earned for the class */
    int     capacity;         /* maximum number of seats in the class */
    Student *studentList[20]; /* enrolled student list */
    int     enrollment;       /* number of enrolled students */

methods:
    override somInit;

    void setUpCourse(char *code, char *title, char *instructor,
        int credit, int capacity);
    -- sets up a new course

    int addStudent(Student *student);
    -- enrolls a student to the course

    void dropStudent(char *studentId);
    -- drops the student from the course

    void printCourseInfo();
    -- prints the information about the course and enrolled students
