#include "pcfist.h"
#include <string.h>
#include <stdlib.h>
#include <iostream.h>

class Student : public pcfist<Student> {
private:
    char   _name[36];
    short  _age;
public:
          Student(char * name,  int age)
      : _age(age)
      {
        strncpy(_name,name,35);
        _name[35] = 0;
      }

friend ostream& operator<<(ostream& o, Student& s)
   {
      /* broken up so that it gets inlined */
      o << s._name;
      o << ' ';
      o << s._age;
      return o;
   }
friend ostream& operator<<(ostream& o, Student* s)
   {
      o <<  s->_name;
      o << ' ';
      o << s->_age;
      return o;
   }
};

main()
{
Student * s1 = new Student("alpha", 10);
Student * s2 = new Student("beta", 20);
   cout << s1 << '\n' << s2 << endl;
   delete s1;
Student * s3 = new Student("nu", 13);
Student * s4 = new Student("mu", 23);
   cout << s3 << '\n' << s4 << endl;
   cout << Student::free() << endl;
   delete s2;
   delete s3;
   delete s4;
   cout << Student::free(1) << endl;
   cout << Student::free() << endl;
   exit(0);
}
