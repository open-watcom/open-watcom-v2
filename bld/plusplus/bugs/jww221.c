// better error message (low priority)
struct S {
    S & operator ++();
};

S a;

void foo( void )
{
    /* old C++ code couldn't declare a post ++ or post -- */
    /* print out a warning that a post version wasn't present */
    /* and post ++ isn't valid for the type */
    a++;
}
