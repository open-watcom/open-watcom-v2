// in general; a pointer cannot be cast to float/double or vice versa
double d;
float f;
long double l;
void *p;

void foo()
{
    f = (float) p;
    l = (long double) p;
    d = (double) p;     // error
    p = (void *) d;     // error
}
