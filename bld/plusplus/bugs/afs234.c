struct foo{
    operator long();
    foo operator+(foo);
    foo operator>>(long);
    foo operator>>(int);
};

foo rsh( foo, long );
long rsh( long, int );

void main()
{
    foo a,b,c;

    b=(a+b)>>12;  // This generates E293
    //rsh( (a+b), 12 );
}

