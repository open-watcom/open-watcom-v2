#ifndef __WATT32_PCAP_H
#define __WATT32_PCAP_H

#define pcap_t void

struct pcap_pkthdr {             /* compatible <pcap.h> header */
       struct timeval ts;        /* time stamp */
       DWORD          caplen;    /* length of portion present */
       DWORD          len;       /* length this packet (off wire) */
     };

extern pcap_t             *_pcap_w32;
extern struct pcap_pkthdr  _pcap_w32_hdr;

extern pcap_t *pcap_open_live (char *device, int snaplen, int promisc_mode,
                               int timeout_ms, char *errbuf);

extern BYTE   *pcap_next   (pcap_t *pcap, struct pcap_pkthdr *hdr);
extern char   *pcap_peek   (pcap_t *pcap);
extern char   *pcap_geterr (pcap_t *pcap);

#endif /* __WATT32_PCAP_H */
