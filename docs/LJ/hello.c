
#include <stdio.h>
#include <netinet/tcp.h>

int main(void)
{
    printf("Hello, Linux! TCP_NODELAY = %d\n", TCP_NODELAY);
    return 0;
}

