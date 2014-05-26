/* Define default version numbers. */
#ifndef WINVER
    #define WINVER          0x0400
#endif
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT    0x0400
#endif
#ifndef _WIN32_IE
    #define _WIN32_IE       0x0400
#endif
:segment RESOURCE
#ifndef _WIN32_WINDOWS
    #define _WIN32_WINDOWS  0x0400
#endif
:elsesegment
#ifndef NTDDI_VERSION
    #define NTDDI_VERSION   0x05000000
#endif
:endsegment
