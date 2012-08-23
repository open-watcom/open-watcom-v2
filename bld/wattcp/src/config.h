#ifndef __WATERLOO_CONFIG_H
#define __WATERLOO_CONFIG_H

#ifndef __WATT_TARGET_H
#error CONFIG.H must be included after TARGET.H
#endif

/*
 * Define global library options here. Change '#undef' to '#define' to
 * include an option.
 *
 * NOTE: Unless you know exactly what you're doing, you MUST do
 *       "make clean" after adding/removing any of the following defines.
 */
#undef  USE_DEBUG       /* Include debug code */
#undef  USE_EXCHANDLER  /* Install exception handler for DOSX targets */
#undef  USE_MULTICAST   /* Include IP multicast code by Jim Martin */
#undef  USE_BIND        /* Include Bind resolver code */
#undef  USE_BSD_FUNC    /* Include BSD-sockets */
#undef  USE_BSD_FATAL   /* Exit application on BSD-socket fatal errors */
#undef  USE_BSD_FORTIFY /* Use Fortify malloc-lib for BSD-socket API */
#undef  USE_BOOTP       /* Include BOOTP client code */
#undef  USE_DHCP        /* Include DHCP boot client code */
#undef  USE_ETHERS      /* Include /etc/ethers file loading. Req. USE_BSD_FUNC */
#undef  USE_RARP        /* Include RARP boot client code. Contributed by Dan Kegel. */
#undef  USE_LANGUAGE    /* Include Language translation code */
#undef  USE_FRAGMENTS   /* Include IP-fragment handling */
#undef  USE_STATISTICS  /* Include protocol statistics count */
#undef  USE_FORTIFY     /* Use Fortify malloc-lib for other allocations */
#undef  USE_FSEXT       /* Use djgpp's File Extensions for file I/O functions */
#undef  USE_LOOPBACK    /* Use the simple loopback device */
#undef  USE_EMBEDDED    /* Make an embeddable (ROM-able) target. See note at end */
#undef  USE_BUFFERED_IO /* Use buffered I/O in pcconfig.c */
#undef  USE_LIBPCAP     /* Use pcap ibrary for SOCK_PACKET support */
#undef  USE_TFTP        /* Include TFTP protocol for simple file retrival */
#undef  USE_NEW_TIMERS  /* use new logic in set_timeout() */
#undef  USE_UDP_ONLY    /* Exclude all stuff related to the TCP protocol.
                         * For making small icmp/udp applications.
                         */
#undef  USE_BIGENDIAN   /* Hard to believe, but someone uses Watt-32 on
                         * Motorola/PowerPC embedded targets.
                         */

#undef  USE_MTU_DISC    /* Include support for atch MTU Discovery (not yet) */
#undef  USE_TCP_SACK    /* Include TCP Selective ACK feature (not yet) */
#undef  USE_ECHO_DISC   /* Include echo/discard servers */
#undef  USE_PPPOE       /* PPP-over-Ethernet encapsulation (!experimental) */

/*
 * Building small-model applications doesn't leave
 * much room for the fancy stuff :-(
 */
#if !defined(OPT_DEFINED) && defined(__SMALL__)
  #define USE_UDP_ONLY     /* test udp-only (cookie,ping) */
  #define OPT_DEFINED
#endif

/*
 * MS's cl 6.0 at least is severly limited in many ways..
 */
#if defined(_MSC_VER) && defined(__LARGE__)
  #define USE_EXCHANDLER
  #define USE_FRAGMENTS
  #define OPT_DEFINED
#endif

/*
 * For compact models define these options:
 */
#if !defined(OPT_DEFINED) && defined(__COMPACT__)
//  #define USE_DEBUG
//  #define USE_EXCHANDLER
//  #define USE_FRAGMENTS
  #define USE_BSD_FUNC
//  #define USE_BSD_FATAL
  #define OPT_DEFINED
#endif

/*
 * So is Turbo-C (2.01 at least)
 */
#if defined(OLD_TURBOC) && defined(__LARGE__)
  #define USE_EXCHANDLER
  #define USE_FRAGMENTS
  #define OPT_DEFINED
#endif

/*
 * For other large models define these options:
 */
#if !defined(OPT_DEFINED) && defined(__LARGE__)
//  #define USE_DEBUG
//  #define USE_EXCHANDLER
//  #define USE_FRAGMENTS
  #define USE_BSD_FUNC
//  #define USE_BSD_FATAL
  #define OPT_DEFINED
#endif

/*
 * Else, define options to everything except Multicast
 * RARP, non-BSD Fortify, Embedded, new-timers and UDP-only.
 */
#if !defined(OPT_DEFINED)
//  #define USE_DEBUG
  #define USE_EXCHANDLER
  #define USE_BOOTP
  #define USE_DHCP
  #define USE_LANGUAGE
  #define USE_FRAGMENTS
  #define USE_STATISTICS
  #define USE_BIND
  #define USE_BSD_FUNC
  #define USE_BSD_FATAL
  #define USE_BSD_FORTIFY
  #define USE_LOOPBACK
  #define USE_BUFFERED_IO
  #define USE_TFTP
  #define USE_MULTICAST
  #define OPT_DEFINED
#endif

#if defined(__DJGPP__)
  #define USE_ECHO_DISC
  #define USE_NEW_TIMERS
  #define USE_FSEXT
#endif

/*
 * Test some illegal combinations.
 */
#if defined(USE_ETHERS) && !defined(USE_BSD_FUNC)
#error USE_ETHERS requires USE_BSD_FUNC
#endif

#if defined(USE_UDP_ONLY) && defined(USE_BSD_FUNC)
#error USE_UDP_ONLY cannot be used with USE_BSD_FUNC
#endif

/*
 * NOTE:
 *  If including `#define USE_EMBEDDED', it's users responsibility to
 *  decide what code should be disabled for embedded targets. Sprinkle
 *  the code with `#if defined(USE_EMBEDDED)' to include special embedded
 *  features; like making `outch()' send to a RS-232 port.
 *  Insert `#if !defined(USE_EMBEDDED)' exclude things;
 *  such as preventing calls to file/disk related functions.
 */

#endif /* __WATERLOO_CONFIG_H */
