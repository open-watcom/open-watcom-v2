//
// Class definitions provided by a library vendor.
//

#include <string>

class employee {
    std::string Name;
protected:
    int     hourRate;
public:
    employee( const char *n, int hr );
    virtual ~employee();

    std::string name() { return( Name ); }
    virtual int salary() = 0;
};

class manager : public employee {
    int     Experience;
public:
    manager( const char *n, int hr, int ex ) :
        employee( n, hr ), Experience( ex ) {}
    int salary();
};

//
// The following is a 'programmer' class defined by the user and based on
// the 'employee' class. Data members Overtime and OvertimeRate, and the
// member functions setOvertimeRate, addOvertime, resetOvertime and
// overtimePay were added.
//
// In a situation where library vendor didn't provide the source code for
// impl.cpp, users cannot recompile that module and hence cannot add any
// virtual member functions to the 'employee' base class. This means that
// in a heterogeneous collection of employees one needs to first check if
// an employee is a programmer before attempting to invoke one of the
// additional member functions.
//

class programmer : public employee {
    int     OvertimeRate;
    int     Overtime;
public:
    programmer( const char *n, int hr ) : employee( n, hr ) {}
    void setOvertimeRate( int otr ) { Overtime = 0; OvertimeRate = otr; }

    void addOvertime( int ov ) { Overtime += ov; }
    void resetOvertime() { Overtime = 0; }
    int  overtimePay() { return( Overtime * OvertimeRate ); }

    int  salary() ;
};
