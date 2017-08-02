// at one point gave wrong diagnose message
const int b = 3;

void bar( const int * & i, int * & j )
{
    i = &b;
    *j = 4; 	// we just changed a constant int (b), all because we allowed
    		// bar( const int * &) to be called with an int*&
}

void main(void)
{
    int a = 5;		// a is an int
    int * f1 = &a;	// f1 is a ptr to an int

    int * & b1 = f1;	// b1 and f1 now refer to the same ptr to int
    bar( b1, b1 );	// should give error (section 4.4)

}

