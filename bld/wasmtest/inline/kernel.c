#include <sys\kernel.h>

main()
{
    __sendmx(0,0,0,0,0);
    __receivemx(0,0,0);
    __replymx(0,0,0);
    __creceivemx(0,0,0);
    __readmsgmx(0,0,0,0);
    __writemsgmx(0,0,0,0);
    __relay(0,0);
    __sret();
    __priority(0,0);
    __netdata(0,0);
    __yield();
    __sendfdmx(0,0,0,0,0);
    __trigger(0);
}
