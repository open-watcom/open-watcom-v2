#ifndef __PCARP_H
#define __PCARP_H

#define MAX_GATE_DATA     5  /* max # of gateways */
#define MAX_ARP_DATA     40  /* max # of entries in ARP-cache */
#define MAX_ARP_GRACE  2000  /* additional grace upon expiration (2s) */

#define ARP_FLAG_FOUND    1  /* IP-addr has been resolved */
#define ARP_FLAG_FIXED  255  /* IP/hw address added in wattcp.cfg */

struct arp_table {
       DWORD       ip;
       eth_address hardware;
       BYTE        flags;
       BYTE        bits;      /* bits in network */
       DWORD       expiry;
     };

struct gate_table {
       DWORD  gate_ip;
       DWORD  subnet;
       DWORD  mask;
       int    metric;
     };

#define arp_cache_data    NAMESPACE (arp_cache_data)
#define arp_gate_list     NAMESPACE (arp_gate_list)
#define arp_last_gateway  NAMESPACE (arp_last_gateway)
#define arp_timeout       NAMESPACE (arp_timeout)
#define arp_alive         NAMESPACE (arp_alive)

extern struct arp_table  arp_cache_data[MAX_ARP_DATA];
extern struct gate_table arp_gate_list [MAX_GATE_DATA];

extern WORD arp_last_gateway;
extern int  arp_timeout;
extern int  arp_alive;

extern void _arp_add_gateway  (const char *data, DWORD ip);
extern void _arp_register     (DWORD use, DWORD instead_of, int nowait);
extern void _arp_add_cache    (DWORD ip, eth_address *eth, BOOL expire);
extern void _arp_tick         (DWORD ip);
extern int  _arp_reply        (const eth_address *dst, DWORD src_ip, DWORD dst_ip);
extern int  _arp_handler      (const arp_Header  *arp);
extern int  _arp_resolve      (DWORD ina, eth_address *eth, int nowait);
extern int  _arp_check_own_ip (void);

extern struct arp_table *_arp_search (DWORD ip, int create);

#endif
