/*
 * Dummy pcap-functions for Watt-32
 *
 *   Version
 *
 *   1.0 : Feb 02, 2000 : G. Vanem - created
 */

#include "socket.h"

#if defined(USE_BSD_FUNC) && defined(USE_LIBPCAP)

#include "w32pcap.h"

pcap_t             *_pcap_w32;
struct pcap_pkthdr  _pcap_w32_hdr;

pcap_t *pcap_open_live (char *device,
                        int   snaplen,
                        int   promisc_mode,
                        int   timeout_ms,
                        char *errbuf)
{
  fprintf (stderr, "\7Link pcap library before Watt-32 library\n");
  ARGSUSED (device);
  ARGSUSED (snaplen);
  ARGSUSED (promisc_mode);
  ARGSUSED (timeout_ms);
  ARGSUSED (errbuf);
  return (NULL);
}

BYTE *pcap_next (pcap_t *pcap, struct pcap_pkthdr *hdr)
{
  ARGSUSED (pcap);
  ARGSUSED (hdr);
  return (NULL);
}

char *pcap_peek (pcap_t *pcap)
{
  ARGSUSED (pcap);
  return (NULL);
}

char *pcap_geterr (pcap_t *pcap)
{
  ARGSUSED (pcap);
  return (NULL);
}

#endif
