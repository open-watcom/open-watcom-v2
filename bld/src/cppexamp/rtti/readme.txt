RTTI Example
------------

to run:         wmake test

output:         The program should display "Test Successful."

Description:    This example illustrates how one can use RTTI to process
                objects on a heterogeneous list. It may not always be
                feasible to use virtual functions, for example, if a base
                class is provided by a third party and can't be altered.
                A user defined derived class may add useful member
                functions that are unknown to the base class and can't be
                accessed without some sort of run time type identification.
