#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "misc.h"
#include "pctcp.h"


/*
 * sock_scanf - return number of fields returned
 */
int sock_scanf (sock_type *sk, const char *fmt, ...)
{
    char buffer [tcp_MaxBufSize];
    int  fields;
    int  status;

    fields = 0;
    while ((status = sock_dataready(sk)) == 0) {
        if (status == -1)
            return (-1);

        fields = sock_gets (sk, (BYTE*)&buffer, sizeof (buffer));
        if (fields) {
            va_list args;
            va_start (args, fmt);
            fields = vsscanf (buffer, fmt, args);
            va_end (args);
        }
    }
    return (fields);
}

