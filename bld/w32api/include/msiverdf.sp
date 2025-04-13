#ifndef NTDDI_WS03SP1
#define NTDDI_WS03SP1   0x05020100
#endif

#ifndef NTDDI_WINXPSP2
#define NTDDI_WINXPSP2  0x05010200
#endif

#ifndef _WIN32_MSI
#if (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WS03SP1)
#define _WIN32_MSI 310
#elif (defined(NTDDI_VERSION) && NTDDI_VERSION >= NTDDI_WINXPSP2)
#define _WIN32_MSI 300
#else
#define _WIN32_MSI 200
#endif
#endif
