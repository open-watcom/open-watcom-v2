//
// This module contains member function definitions. It is provided by a
// library vendor and customers do not have access to its source code.
//

#include "impl.h"

employee::employee( const char *n, int hr ) : hourRate( hr ), Name( n )
{
}

employee::~employee()
{
    // Placeholder.
}

int manager::salary()
{
    return( hourRate * 40 + Experience * 10 );
}

int programmer::salary()
{
    return( hourRate * 40 );
}
