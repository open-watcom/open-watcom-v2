#ifndef __UDP_DOM_H
#define __UDP_DOM_H

#define MAX_NAMESERVERS  10   /* max # of nameservers supported (plenty!) */
#define DOMSIZE          512  /* maximum domain message size to mess with */
#define DOM_DST_PORT     53   /* destination port number for DNS protocol */
#define DOM_LOC_PORT     997  /* local port number for DNS protocol       */

/*
 *  Header for the DOMAIN queries
 *  ALL OF THESE ARE BYTE SWAPPED QUANTITIES!
 *  We are the poor slobs who are incompatible with the world's byte order
 */
struct dhead {
       WORD  ident;         /* unique identifier              */
       WORD  flags;
       WORD  qdcount;       /* question section, # of entries */
       WORD  ancount;       /* answers, how many              */
       WORD  nscount;       /* count of name server RRs       */
       WORD  arcount;       /* number of "additional" records */
     };

/*
 *  flag masks for the flags field of the DOMAIN header
 */
#define DQR       0x8000    /* query = 0, response = 1  */
#define DOPCODE   0x7100    /* opcode, see below        */
#define DAA       0x0400    /* Authoritative answer     */
#define DTC       0x0200    /* Truncation, response was cut off at 512 */
#define DRD       0x0100    /* Recursion desired        */
#define DRA       0x0080    /* Recursion available      */
#define DRCODE    0x000F    /* response code, see below */

/*
 * opcode possible values:
 */
#define DOPQUERY  0         /* a standard query */
#define DOPIQ     1         /* an inverse query */
#define DOPCQM    2         /* a completion query, multiple reply */
#define DOPCQU    3         /* a completion query, single reply   */
/* the rest reserved for future */

/*
 * legal response codes:
 */
#define DROK      0         /* okay response */
#define DRFORM    1         /* format error  */
#define DRFAIL    2         /* their problem, server failed           */
#define DRNAME    3         /* name error, we know name doesn't exist */
#define DRNOPE    4         /* no can do request                      */
#define DRNOWAY   5         /* name server refusing to do request     */

#define DTYPEA    1         /* host address resource record (A) */
#define DTYPEPTR 12         /* domain name ptr (PTR)            */

#define DIN       1         /* ARPA internet class                         */
#define DWILD   255         /* wildcard for several of the classifications */

#include <sys/packon.h>

/*
 *  a resource record is made up of a compressed domain name followed by
 *  this structure.  All of these words need to be byteswapped before use.
 */
struct rrpart {
       WORD   rtype;          /* resource record type = DTYPEA    */
       WORD   rclass;         /* RR class = DIN                   */
       DWORD  ttl;            /* time-to-live, changed to 32 bits */
       WORD   rdlength;       /* length of next field             */
       BYTE   rdata[DOMSIZE]; /* data field                       */
     };

/*
 *  data for domain name lookup
 */
struct useek {
       struct dhead h;
       BYTE   x[DOMSIZE];
     };

#include <sys/packoff.h>

#define defaultdomain    NAMESPACE (defaultdomain)
#define def_domain       NAMESPACE (def_domain)
#define loc_domain       NAMESPACE (loc_domain)
#define def_nameservers  NAMESPACE (def_nameservers)
#define dns_timeout      NAMESPACE (dns_timeout)
#define dns_recurse      NAMESPACE (dns_recurse)
#define last_nameserver  NAMESPACE (last_nameserver)


extern int (*_resolve_hook)(void);
extern int   _resolve_exit;
extern int   _resolve_timeout;

extern char  defaultdomain [MAX_HOSTLEN+1];
extern char *def_domain;
extern char *loc_domain;

extern DWORD def_nameservers [MAX_NAMESERVERS];
extern WORD  last_nameserver;
extern UINT  dns_timeout;
extern int   dns_recurse;
extern int   called_from_resolve;
extern int   called_from_ghbn;

extern DWORD resolve (const char *name);

#endif
