/*
 * Bootstrap Protocol (BOOTP)
 * Based on RFC 951.
 */

#ifndef __PCBOOTP_H
#define __PCBOOTP_H

extern DWORD _bootphost;
extern int   _bootptimeout;
extern int   _dobootp (void);

#define BOOTREQUEST   1       /* bootp.bp_op */
#define BOOTREPLY     2

#include <sys/packon.h>

/*
 * structure for send and receives
 */
struct bootp {
       BYTE   bp_op;         /* packet op code / message type.            */
       BYTE   bp_htype;      /* hardware address type, 1 = 10 mb ethernet */
       BYTE   bp_hlen;       /* hardware address len, eg '6' for 10mb eth */
       BYTE   bp_hops;       /* client sets to zero, optionally used by   */
                             /* gateways in cross-gateway booting.        */
       DWORD  bp_xid;        /* transaction ID, a random number           */
       WORD   bp_secs;       /* filled in by client, seconds elapsed      */
                             /* since client started trying to boot.      */
       WORD   bp_spare;
       DWORD  bp_ciaddr;     /* client IP address filled in by client if known*/
       DWORD  bp_yiaddr;     /* 'your' (client) IP address                */
                             /* filled by server if client doesn't know   */
       DWORD  bp_siaddr;     /* server IP address returned in bootreply   */
       DWORD  bp_giaddr;     /* gateway IP address,                       */
                             /* used in optional cross-gateway booting.   */
       BYTE   bp_chaddr[16]; /* client hardware address, filled by client */
       BYTE   bp_sname[64];  /* optional server host name, null terminated*/

       BYTE   bp_file[128];  /* boot file name, null terminated string    */
                             /* 'generic' name or null in bootrequest,    */
                             /* fully qualified directory-path            */
                             /* name in bootreply.                        */
       BYTE   bp_vend[64];   /* optional vendor-specific area             */
     };

/*
 * UDP port numbers, server and client.
 */
#define IPPORT_BOOTPS   (WORD)67
#define IPPORT_BOOTPC   (WORD)68


/******** the following is stolen from NCSA which came from CUTCP *********/
/* I have not implemented these, but someone may wish to in the future so */
/* I kept them around.                                                    */
/**************************************************************************/

/*
 * "vendor" data permitted for Stanford boot clients.
 */
struct vend {
       BYTE  v_magic[4];    /* magic number        */
       DWORD v_flags;       /* flags/opcodes, etc. */
       BYTE  v_unused[56];  /* currently unused    */
     };

#include <sys/packoff.h>


#define VM_STANFORD  "STAN"       /* v_magic for Stanford    */
#define VM_RFC1048   0x63825363L  /* I think this is correct */

/*
 * v_flags values
 */
#define VF_PCBOOT          1   /* an IBMPC or Mac wants environment info */
#define VF_HELP            2   /* help me, I'm not registered            */
#define TAG_BOOTFILE_SIZE  13  /* tag used by vend fields rfc 1048       */


#endif /* __PCBOOTP_H */


