EXCAREA Example
---------------

to run:         wmake test

output:         The program should display "Passed".

Description:    This example illustrates how to extend the static area used
                to contain unhandled exceptions.  The default sizes are 4k
                (32 bit) and 1k (16 bit).
                
                The main program throws and catches an 8k object. The source
                module excarea.cpp has been modified to have a 10k data area.
