// This file puts ztc into an endless loop.
// The type specifier on the constructor function is the source
// of this probem.
// Tcc diagnoses and reports the error.

class bug1
{
        int a;
    public:
        void bug1( void );      // the first void on this line is the problem
};

bug1::bug1( void )
{
    this->a = 0;        // it seems to get stuck on this line
}
