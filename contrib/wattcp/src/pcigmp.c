
/*
 * Internet Group Management Protocol (IGMP) as per RFC 1112.
 * 
 * This protocol is to allow multicast routers to establish group
 * membership on attached networks. Thus allowing the router to not send 
 * data that is not needed.
 *
 * Jim Martin
 * Rutgers University - RUCS-TD/NS
 * jim@noc.rutgers.edu
 * 6/9/93
 */

#include <stdio.h>
#include <stdlib.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "misc.h"
#include "chksum.h"
#include "ip_out.h"
#include "pcmulti.h"
#include "pcdbug.h"
#include "pcstat.h"
#include "pcsed.h"
#include "pctcp.h"

#if defined(USE_MULTICAST)

#include <sys/packon.h>

struct IGMP_PKT {
       in_Header   in;
       IGMP_packet igmp;
     };

#include <sys/packoff.h>


/* 
 * igmp_handler - handles the incoming IGMP packets
 *
 * void igmp_handler (in_Header *ip)
 * Where:
 *      ip    is the IP packet in question
 *
 * Returns: None
 *
 */
void igmp_handler (const in_Header *ip, BOOL broadcast)
{
  BYTE         i;
  DWORD        host;
  BOOL         found = 0;
  WORD         len   = in_GetHdrLen (ip);
  IGMP_packet *igmp  = (IGMP_packet*) ((BYTE*)ip + len);

  DEBUG_RX (NULL, ip);

  if (len < sizeof(*igmp))
  {
    STAT (igmpstats.igps_rcv_tooshort++);
    return;
  }

  if (checksum(igmp,sizeof(*igmp)) != 0xFFFF)
  {
    STAT (igmpstats.igps_rcv_badsum++);
    return;
  }

  host = intel (igmp->address);

  /* Determine whether this is a report or a query
   */
  switch (igmp->type)
  {
    case IGMP_QUERY:
         STAT (igmpstats.igps_rcv_queries++);
         for (i = 0; i < IPMULTI_SIZE; i++)
             if (_ipmulti[i].active             &&
                 _ipmulti[i].ina != ALL_SYSTEMS &&
                 _ipmulti[i].replytime == 0)
             {
               _ipmulti[i].replytime = set_timeout (Random(500,1000));
               found = 1;
             }
         if (!found && !broadcast)
            STAT (igmpstats.igps_rcv_badqueries++);
         break;

    case IGMP_REPORT:
         STAT (igmpstats.igps_rcv_reports++);
         for (i = 0; i < IPMULTI_SIZE; i++)
             if (_ipmulti[i].active      &&
                 _ipmulti[i].ina == host &&
                 host != ALL_SYSTEMS)
             {
               _ipmulti[i].replytime = 0;
               found = 1;
               STAT (igmpstats.igps_rcv_ourreports++);
               break;
             }
         if (!found && !broadcast)
            STAT (igmpstats.igps_rcv_badreports++);
         break;
  }
}
         

/* 
 * igmp_report - send a IGMP Report packet
 *
 * int igmp_report (DWORD ip)
 * Where:
 *      ip is the IP address to report.
 *
 * Returns:
 *      0   if unable to send report
 *      1   report was sent successfully
 */
int igmp_report (DWORD ip)
{
  struct IGMP_PKT *pkt;
  IGMP_packet     *igmp;
  eth_address      ethaddr;

  /* get the ethernet addr of the destination
   */
  multi_to_eth ((DWORD)ALL_SYSTEMS, (BYTE*)&ethaddr);

  /* format the packet with the request's hardware address
   */
  pkt  = (struct IGMP_PKT*) _eth_formatpacket (ethaddr, IP_TYPE);
  igmp = &pkt->igmp;
  ip = intel (ip);

  /* fill in the igmp packet
   */
  igmp->type     = IGMP_REPORT;
  igmp->version  = IGMP_VERSION;
  igmp->mbz      = 0;
  igmp->address  = ip;
  igmp->checksum = 0;
  igmp->checksum = ~checksum (igmp,sizeof(*igmp));

  return IP_OUTPUT (&pkt->in, 0, ip, IGMP_PROTO,
                    0, 0, 0, (int)sizeof(*igmp), NULL);
}

#endif /* USE_MULTICAST */

