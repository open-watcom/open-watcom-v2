
#include <iostream>
#include <typeinfo>
#include "impl.h"
#include "user.h"

company::~company()
{
    std::list< employee * >::iterator p;
    for( p = EmployeeList.begin(); p != EmployeeList.end(); ++p ) {
        delete *p;
    }
}

void company::addManager( const char *name, int hourRate, int experience )
{
    manager *m = new manager( name, hourRate, experience );
    EmployeeList.push_back( m );
}

//
// Since member functions aren't virtual, user must guarantee that that the
// newly added member functions (setOvertimeRate, addOvertime,
// resetOvertime and overtimePay) are only ever used if the 'employee'
// referred to is a 'programmer'. A dynamic_cast or a typeid can be used
// for this purpose.
//

void company::addProgrammer( const char *name, int hourRate, int ovRate )
{
    programmer  *p = new programmer( name, hourRate );
    p->setOvertimeRate( ovRate );
    EmployeeList.push_back( p );
}

void company::addOvertime( const char *name, int overtime )
{
    std::list< employee * >::iterator p;
    for( p = EmployeeList.begin(); p != EmployeeList.end(); ++p ) {
        if( (*p)->name() == name ) {
            programmer  *pe = dynamic_cast< programmer* >( *p );
            if( pe != NULL )
                pe->addOvertime( overtime );

            return;
        }
    }
}

int company::payRoll()
{
    int     expense = 0;
    std::list< employee * >::iterator p;

    for( p = EmployeeList.begin(); p != EmployeeList.end(); ++p ) {
        int pay = (*p)->salary();
        programmer* pe = dynamic_cast< programmer* >( *p );
        if( pe != NULL ) {
            pay += pe->overtimePay();
            pe->resetOvertime();
        }
        std::cout << (*p)->name().c_str() << " is a "
                  << typeid( **p ).name()
                  << " who was paid $" << pay << " this week." << std::endl;
        expense += pay;
    }
    std::cout << "The total expense in salary this week is "
              << expense << "." << std::endl;

    return( expense );
}
