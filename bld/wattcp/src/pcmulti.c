/*
 * IP Multicasting extensions as per RFC 1112.
 * 
 * These extensions include routines to detect multicast addresses,
 * transform Multicast IP addresses to Multicast Ethernet addresses, as
 * well as a mechanism to join and leave multicast groups.
 *
 * Jim Martin
 * Rutgers University - RUCS-TD/NS
 * jim@noc.rutgers.edu
 * 6/6/93
 */

#include <stdio.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "misc.h"
#include "pcsed.h"
#include "pcconfig.h"
#include "pcigmp.h"
#include "pcmulti.h"

/*
 * Application must set this to non-zero before calling sock_init()
 * Also used by pcpkt.c.
 */
int _multicast_on = 0;

#if defined(USE_MULTICAST)

multicast _ipmulti [IPMULTI_SIZE];

/* 
 * multi_to_eth - calculates the proper ethernet address for a given
 *                IP Multicast address.
 *
 * int multi_to_eth (DWORD ip, BYTE *eth)
 * Where:
 *       ip:  IP address to be converted
 *       eth: pointer to ethernet MAC address
 */
void multi_to_eth (DWORD ip, BYTE *eth)
{
  DWORD top = ETH_MULTI;

  ip &= IPMULTI_MASK;
  eth[0] = (BYTE)(top >> 16);
  eth[1] = (BYTE)((top >> 8) & 0xFF);
  eth[2] = (BYTE)(top & 0xFF);
  eth[3] = (BYTE)(ip >> 16);
  eth[4] = (BYTE)((ip >> 8) & 0xFF);
  eth[5] = (BYTE)(ip & 0xFF);
}


/* 
 * join_mcast_group - joins a multicast group
 *
 * int join_mcast_group (DWORD ina)
 * Where:
 *       ina IP address of the group to be joined
 * Returns:
 *       1   if the group was joined successfully
 *       0   if attempt failed
 */
int join_mcast_group (DWORD ina)
{
  BYTE i;
  int  free = -1;

  /* first verify that it's a valid mcast address
   */
  if (!_multicast_on || !is_multicast(ina))
     return (0);

  /* Determine if the group has already been joined.
   * As well as what the first free slot is.
   */
  for (i = 0; i < IPMULTI_SIZE; i++)
  {
    if (_ipmulti[i].active && ina == _ipmulti[i].ina)
    {
      _ipmulti[i].processes++;
      return (1);
    }
    if (free == -1 && !_ipmulti[i].active)
       free = i;
  }

  /* alas, no...we need to join it
   */
  if (free == -1)        /* out of slots! */
     return (0);

  _ipmulti[free].ina = ina;
  if (!_eth_join_mcast_group(free))
     return (0);
  return (1);
}

/* 
 * leave_mcast_group - leaves a multicast group
 *
 * int leave_mcast_group( DWORD ina )
 * Where:
 *       ina IP address of the group to be joined
 * Returns:
 *       1   if the group was left successfully
 *       0   if attempt failed
 */
int leave_mcast_group (DWORD ina)
{
  BYTE i;
  int  groupnum = -1;

  /* first verify that it's a valid mcast address
   */
  if (!is_multicast(ina))
     return (0);

  /* determine if the group has more than one interested
   * process. if so, then just decrement .processes and
   * return
   */
  for (i = 0; i < IPMULTI_SIZE; i++)
  {
    if (_ipmulti[i].active && ina == _ipmulti[i].ina)
    {
      if (_ipmulti[i].processes > 1)
      {
        _ipmulti[i].processes--;
        return (1);
      }
      groupnum = i;
      break;
    }
  }

  /* did the ipaddr they gave match anything in _ipmulti ??
   */
  if (groupnum == -1)
     return (0);

  /* alas...we need to physically leave it
   */
  if (!_eth_leave_mcast_group(groupnum))
     return (0);

  return (1);
}

/*
 * Check to see if we owe any IGMP Reports
 * (moved from tcp_tick)
 */
void check_mcast_reports (void)
{
  int i;

  for (i = 0; i < IPMULTI_SIZE; i++)
  {
    if (_ipmulti[i].active &&
        _ipmulti[i].ina != ALL_SYSTEMS &&
        chk_timeout(_ipmulti[i].replytime))
    {
      igmp_report (_ipmulti[i].ina);
      _ipmulti[i].replytime = 0;
    }
  }
}

#endif /* USE_MULTICAST */

