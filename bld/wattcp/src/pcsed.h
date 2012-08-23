/*
 * Link-layer (DIX Ethernet, Token-Ring, FDDI and PPP/SLIP) Interface
 */

#ifndef __PCSED_H
#define __PCSED_H

/*
 * Hardware (MAC) address numbers (host order)
 */
#define HW_TYPE_ETHER     1
#define HW_TYPE_TOKEN     6
#define HW_TYPE_ARCNET    7
#define HW_TYPE_APPLETALK 8
#define HW_TYPE_FDDI      9   /* correct? */

#include <sys/packon.h>  /* align structs on byte boundaries */

/*
 * The Ethernet header
 */
typedef struct eth_Header {
        eth_address  destination;
        eth_address  source;
        WORD         type;
      } eth_Header;

typedef struct eth_Packet {
        eth_Header   head;
        BYTE         data [ETH_MAX_DATA];
      } eth_Packet;

/*
 * Token-Ring header, refer RFC-1042, pg. 8
 */
typedef struct tok_Header {
        /* MAC header
         */
        BYTE        accessCtrl;
        BYTE        frameCtrl;
        mac_address destination;
        mac_address source;

        /* 2-18 bytes of Routing Information (RIF) may be present here.
         * We discard it anyway (see _eth_fix_tok_head).
         */

        /* LLC header
         */
        BYTE        DSAP;
        BYTE        SSAP;
        BYTE        ctrl;

        /* SNAP extension
         */
        BYTE        org[3];
        WORD        type;
      } tok_Header;

typedef struct tok_Packet {
        tok_Header head;
        BYTE       data [TOK_MAX_DATA];
      } tok_Packet;

/*
 * From Xinu, tr.h
 */
#define TR_AC       0x10    /* Access Control; Frame bit (not token) */
#define TR_FC       0x40    /* Frame Control;  LLC header follows */
#define TR_DSAP     0xAA    /* DSAP field; SNAP follows LLC */
#define TR_SSAP     0xAA    /* SSAP field; SNAP follows LLC */
#define TR_CTRL     0x03    /* Unnumbered Information */
#define TR_ORG      0x00    /* Organisation Code or Protocol Id */


/*
 * The FDDI header
 */
typedef struct fddi_Header {
        BYTE        frameCtrl;
        eth_address destination;
        eth_address source;
        BYTE        DSAP;
        BYTE        SSAP;
        BYTE        ctrl;
        BYTE        org[3];
        WORD        type;
      } fddi_Header;

typedef struct fddi_Packet {
        fddi_Header head;
        BYTE        data [FDDI_MAX_DATA];
      } fddi_Packet;

#define FDDI_FC     0x40   /* See above */
#define FDDI_DSAP   0xAA
#define FDDI_SSAP   0xAA
#define FDDI_CTRL   0x03
#define FDDI_ORG    0x00

/*
 * The PPPOE header (RFC-2561)
 */
typedef struct pppoe_Header {
        eth_address  destination;
        eth_address  source;
        WORD         proto;
        BYTE         ver  : 4;
        BYTE         type : 4;
        BYTE         code;
        WORD         session;
        WORD         length;
      } pppoe_Header;

typedef struct pppoe_Packet {
        pppoe_Header head;
        BYTE         data [PPPOE_MAX_DATA];
      } pppoe_Packet;


/*
 * The union of all above MAC-headers
 */
typedef union link_Packet {
        struct eth_Packet   eth;    /* _pktdevclass = PD_ETHER */
        struct tok_Packet   tok;    /* _pktdevclass = PD_TOKEN */
        struct fddi_Packet  fddi;   /* _pktdevclass = PD_FDDI  */
        struct ip_Packet    ip;     /* _pktdevclass = PD_PPP/PP_SLIP */
        struct pppoe_Packet pppoe;  /* _pktdevclass = PD_ETHER */
      } link_Packet;

#include <sys/packoff.h>           /* restore default packing */


extern BOOL        _ip_recursion;
extern BOOL        _eth_is_init;
extern mac_address _eth_addr;
extern mac_address _eth_brdcast;
extern mac_address _eth_loop_addr;

extern void *(*_eth_recv_hook) (WORD *type);
extern int   (*_eth_xmit_hook) (void *buf, unsigned len);

extern int   _eth_init         (void);
extern void  _eth_release      (void);
extern int   _eth_send         (WORD len);
extern int   _eth_set_addr     (mac_address *addr);
extern BYTE *_eth_formatpacket (const void *mac_dest, WORD mac_type);
extern void  _eth_free         (const void *pkt, WORD mac_type);
extern void *_eth_arrived      (WORD *type, BOOL *brdcast);
extern void *_eth_hardware     (BYTE *p);
extern BYTE  _eth_get_hwtype   (BYTE *hwtype, BYTE *hwlen);

#if defined(USE_MULTICAST)
  extern int _eth_join_mcast_group  (int entry);
  extern int _eth_leave_mcast_group (int entry);
#endif

#if defined(USE_LOOPBACK)
  extern int _eth_send_loopback (union link_Packet);
#endif

/*
 * Return pointer to hardware source address of
 * an IP packet. For Ethernet:
 *
 *     struct eth_Packet {
 *      -14-> BYTE  dest [6];
 *       -8-> BYTE  src  [6];
 *            WORD  type;
 *       ip-> BYTE  data [1500];
 *          };
 *
 * For Token-Ring:
 *     struct tok_Packet {
 *      -22-> BYTE  AC, FC;
 *            BYTE  dest [6];
 *      -14-> BYTE  src  [6];
 *            BYTE  DSAP, SSAP, ctrl;
 *            BYTE  org [3];
 *            WORD  type;
 *       ip-> BYTE  data [1500];
 *
 * These macros and function should never be called for serial protocols
 * except that it doesn't hurt to use MAC_SRC() for all driver classes.
 */

#if defined(USE_DEBUG) && 0  /* slower, but safer method */
  extern void *_eth_mac_hdr (const in_Header *ip);
  extern void *_eth_mac_src (const in_Header *ip);
  extern WORD  _eth_mac_typ (const in_Header *ip);

  #define MAC_HDR(ip) _eth_mac_hdr(ip)
  #define MAC_SRC(ip) _eth_mac_src(ip)
  #define MAC_TYP(ip) _eth_mac_typ(ip)
#else
  #define MAC_HDR(ip) (void*) ((BYTE*)(ip) - _pkt_ip_ofs)

  #define MAC_SRC(ip) (void*) ((BYTE*)(ip) -             \
                        (_pktdevclass == PD_TOKEN ? 14 : \
                         _pktdevclass == PD_FDDI  ? 14 : 8))

  #define MAC_TYP(ip) (*(WORD*) ((BYTE*)(ip) - 2))
#endif

/*
 * Macros to insert or clear peer's source MAC-address stored in
 * the tcp_Socket.
 * Only used on passive (listening) udp/tcp sockets.
 * We don't use arp_resolve() because of reentrancy problems;
 * If inbound packets come from a different gateway than outbound
 * packets, then this macro fails.
 */
#define SET_PEER_MAC_ADDR(tcb,ip) do {                    \
          if (!_pktserial)                                \
             memcpy (&(tcb)->hisethaddr[0],MAC_SRC(ip),6);\
        } while (0)

#define CLR_PEER_MAC_ADDR(tcb) do {             \
          if (!_pktserial)                      \
             memset (&(tcb)->hisethaddr[0],0,6);\
        } while (0)

#endif
