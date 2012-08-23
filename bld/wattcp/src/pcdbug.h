#ifndef __PCDBUG_H
#define __PCDBUG_H

extern const char *tcpState[];

extern BOOL      dbg_mode_all, dbg_print_stat, dbg_dns_details;

extern void      dbug_init   (void);
extern void      dbug_open   (void);
extern void      dbug_exit   (void);
extern const int dbug_handle (void);

extern void dbug_write_raw (const char*);

/*
 * Send Rx/Tx packet to ip-debugger if debugger active
 */
#if defined(USE_DEBUG)
  #define DEBUG_RX(sock, ip)                               \
          do {                                             \
            if (_dbugrecv)                                 \
              (*_dbugrecv) (sock, ip, __FILE__, __LINE__); \
          } while (0)

  #define DEBUG_TX(sock, ip)                               \
          do {                                             \
            if (_dbugxmit)                                 \
              (*_dbugxmit) (sock, ip, __FILE__, __LINE__); \
          } while (0)
#else
  #define DEBUG_RX(sock, ip) ((void)0)
  #define DEBUG_TX(sock, ip) ((void)0)
#endif

#endif

