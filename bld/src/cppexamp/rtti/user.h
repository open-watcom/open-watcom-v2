
#include <list>

class company {
    std::list< employee * > EmployeeList;
public:
   ~company();
    void addManager( const char *name, int hourRate, int experience );
    void addProgrammer( const char *name, int hourRate, int overtimeRate );
    void addOvertime( const char *name, int overtime );
    int payRoll();
};
