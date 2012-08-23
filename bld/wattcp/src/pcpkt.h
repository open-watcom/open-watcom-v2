#ifndef __PCPKT_H
#define __PCPKT_H

/* Basic PD Functions
 */
#define PD_DRIVER_INFO  0x1FF
#define PD_ACCESS       0x200
#define PD_RELEASE      0x300
#define PD_SEND         0x400
#define PD_TERMINATE    0x500
#define PD_GET_ADDRESS  0x600
#define PD_RESET        0x700

/* Extended PD Functions
 */
#define PD_GET_PARAM    0x0A00
#define PD_SET_RCV      0x1400
#define PD_GET_RCV      0x1500
#define PD_SET_MULTI    0x1600
#define PD_GET_MULTI    0x1700
#define PD_GET_STATS    0x1800
#define PD_SET_ADDR     0x1900

/* Packet-Driver Error Returns
 */
#define BAD_HANDLE      1
#define NO_CLASS        2
#define NO_TYPE         3
#define NO_NUMBER       4
#define BAD_TYPE        5
#define NO_MULTICAST    6
#define CANT_TERMINATE  7
#define BAD_MODE        8
#define NO_SPACE        9
#define TYPE_INUSE     10
#define BAD_COMMAND    11
#define CANT_SEND      12
#define CANT_SET       13
#define BAD_ADDRESS    14
#define CANT_RESET     15
#define BASIC_DVR      16

/* Packet-driver classes
 */
#define PD_ETHER        1    /* IEEE 802.2 */
#define PD_TOKEN        3    /* IEEE 802.5 */
#define PD_SLIP         6    /* Serial Line IP */
#define PD_AX25         9    /* Amateur X.25 (packet radio) */
#define PD_FDDI        12    /* FDDI w/802.2 headers */
#define PD_PPP         18

enum ReceiveModes {
     RM_OFF        = 1,  /* turn off receiver              */
     RM_DIRECT     = 2,  /* receive only to this interface */
     RM_BROADCAST  = 3,  /* DIRECT + broadcast packets     */
     RM_MULTICAST1 = 4,  /* BROADCAST + limited multicast  */
     RM_MULTICAST2 = 5,  /* BROADCAST + all multicast      */
     RM_PROMISCOUS = 6   /* receive all packets on network */
   };

#define PKT_FIRST_VEC  0x60   /* first and last vector to */
#define PKT_LAST_VEC   0x80   /* search for a driver */

extern WORD _pktdevclass;
extern WORD _pkt_ip_ofs;
extern BYTE _pktserial;
extern BYTE _pktdevlevel;
extern BYTE _pkterror;
extern BYTE _pktretries;

extern BOOL pkt_eth_init    (eth_address *eth);
extern int  pkt_release     (void);
extern int  pkt_send        (const void *tx, int length);
extern int  pkt_buf_wipe    (void);
extern void pkt_free_pkt    (const void *pkt, BOOL is_ip);
extern int  pkt_waiting     (void);
extern int  pkt_waiting_ip  (void);
extern int  pkt_waiting_arp (void);
extern int  pkt_set_addr    (eth_address *eth);
extern int  pkt_get_addr    (eth_address *eth);
extern int  pkt_get_mtu     (void);
extern int  pkt_get_mac_len (void);
extern int  pkt_get_drvr_ver(void);
extern int  pkt_dropped     (void);
extern int  pkt_dropped_ip  (void);
extern int  pkt_dropped_arp (void);

#if defined(USE_MULTICAST)
  extern int _pkt_set_rcv_mode       (int handle, int mode);
  extern int _pkt_get_rcv_mode       (int handle);
  extern int _pkt_get_multicast_list (int len, eth_address *listbuf);
  extern int _pkt_set_multicast_list (int len, eth_address *listbuf);
  extern int _pkt_get_ip_rcv_mode    (void);
  extern int _pkt_set_ip_rcv_mode    (int mode);
#endif

#if defined(DEFINE_IREGS)
/*
 * IREGS structures for pkt_api_entry()
 */
#if (DOSX & PHARLAP)
  #define IREGS      SWI_REGS
  #define r_flags    flags
  #define r_ax       eax
  #define r_bx       ebx
  #define r_dx       edx
  #define r_cx       ecx
  #define r_si       esi
  #define r_di       edi
  #define r_ds       ds
  #define r_es       es

#elif (DOSX & DJGPP)
  #define IREGS      __dpmi_regs
  #define r_flags    x.flags
  #define r_ax       d.eax
  #define r_bx       d.ebx
  #define r_dx       d.edx
  #define r_cx       d.ecx
  #define r_si       d.esi
  #define r_di       d.edi
  #define r_ds       x.ds
  #define r_es       x.es

#elif (DOSX & (DOS4GW|WDOSX))
  #define IREGS      struct DPMI_regs  /* in wdpmi.h */

#elif (DOSX & POWERPAK)     /* to-do !! */
  typedef struct IREGS {    /* just for now */
          WORD  r_ax, r_bx, r_cx, r_dx, r_bp;
          WORD  r_si, r_di, r_ds, r_es, r_flags;
        } IREGS;

#else  /* r-mode targets */

  /* IREGS must have same layout and size as Borland's 'struct REGPACK'
   * and Watcom's 'union REGPACK'. This is checked in check_reg_struct()
   * in pcpkt.c
   */
  #include <sys/packon.h>

  typedef struct IREGS {
          WORD  r_ax, r_bx, r_cx, r_dx, r_bp;
          WORD  r_si, r_di, r_ds, r_es, r_flags;
        } IREGS;

  #include <sys/packoff.h>
#endif
#endif /* DEFINE_IREGS */

/*
 * Define size and # of IP and ARP protocol buffers. We use
 * separate queues for IP and non-IP. Don' waste buffer space
 * enqueueing non-IP packets.
 */
#define PKT_MARGIN  10
#define IP_SIZE     (ETH_MAX + PKT_MARGIN)   /* 381 dwords */
#define ARP_SIZE    (ETH_MIN + PKT_MARGIN+2) /*  18 dwords */

#if (DOSX)
  #define IP_BUFS  20   /* # of IP/ARP buffers to use in queue */
  #define ARP_BUFS 4
#else
  #define IP_BUFS  5
  #define ARP_BUFS 2
#endif

/*
 * asmpkt4.asm depends on '_pkt_inf' beeing packed
 */
#if (DOSX & DOS4GW)
#include <sys/packon.h>
#endif

/*
 * Placeholder for vital data accessed on packet-driver upcall.
 * Keep it gathered to simplify locking memory at `_pkt_inf'.
 * This structure MUST match same structure in asmpkt4.asm
 */
struct pkt_info {
#if (DOSX & DJGPP)
       _go32_dpmi_seginfo rm_mem;
#endif
       WORD   ip_handle;               /* Packet-driver handles for   */
       WORD   arp_handle;              /*    IP, ARP and RARP packets */
       WORD   rarp_handle;
       WORD   pppoe_disc_handle;       /* handles for PPPoE protocols */
       WORD   pppoe_sess_handle;       /* (only set if USE_PPPOE)     */
       WORD   is_serial;               /* duplicated from _pktserial  */
       WORD   max_pkt_size;            /* max. size of receive frame  */
       struct pkt_ringbuf ip_queue;    /* Ring-structs for enqueueing */
       struct pkt_ringbuf arp_queue;   /* IP and ARP packets          */

       BYTE   ip_buf [IP_BUFS] [IP_SIZE];  /* Buffers used for IP+ARP */
       BYTE   arp_buf[ARP_BUFS][ARP_SIZE]; /*            ring-buffers */
     };

  /*
   * to-do!!: make ip_buf[x][14] align on dword (ip-head start on ether).
   */

extern struct pkt_info *_pkt_inf;

#if (DOSX & DOS4GW)
#include <sys/packoff.h>
#endif

/*
 * sizeof(*_pkt_inf), __LARGE__
 * 4*2         = 8
 * 2*16        = 32
 * 5*1524      = 7620
 * 2*(60+10+2) = 144
 *             = 7804
 *
 * sizeof(*_pkt_inf), DOS4GW
 * 4*2             = 8
 * 2*26            = 52
 * 20*(14+1500+10) = 30480
 * 4*(60+10+2)     = 288
 *                 = 30828
 */

#if defined(USE_DEBUG)
  #define ASSERT_PKT_INF(rc) do {                                \
                               if (!_pkt_inf) {                  \
                                  fprintf (stderr, "%s (%u): "   \
                                           "_pkt_inf == NULL\n", \
                                           __FILE__, __LINE__);  \
                                  return (rc);                   \
                               }                                 \
                             } while (0)
#else
  #define ASSERT_PKT_INF(rc) do {              \
                               if (!_pkt_inf)  \
                                  return (rc); \
                             } while (0)
#endif

#endif
