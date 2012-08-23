#ifndef __PPPOE_H
#define __PPPOE_H

#define PPPOE_CODE_PADI  0x09    /* Active Discovery Initiation */
#define PPPOE_CODE_PADO  0x07    /* Active Discovery Offer packet */
#define PPPOE_CODE_PADR  0x19    /* Active Discovery Request */
#define PPPOE_CODE_PADS  0x65    /* Active Discovery Session-confirmation */
#define PPPOE_CODE_PADT  0xA7    /* Active Discovery Terminate */
#define PPPOE_CODE_PADM  0xD3    /* Active Discovery Message packet */
#define PPPOE_CODE_PADN  0xD4    /* Active Discovery Network packet */
#define PPPOE_CODE_SESS  0x00    /* code when we have a session */

#define PPPOE_TAG_END_LIST       0x0000
#define PPPOE_TAG_SERVICE_NAME   0x0101
#define PPPOE_TAG_AC_NAME        0x0102
#define PPPOE_TAG_HOST_UNIQ      0x0103
#define PPPOE_TAG_AC_COOKIE      0x0104
#define PPPOE_TAG_VENDOR_SPES    0x0105
#define PPPOE_TAG_RELAY_SESS     0x0106
#define PPPOE_TAG_HOST_URL       0x0111
#define PPPOE_TAG_MOTM           0x0112
#define PPPOE_TAG_IP_ROUTE_ADD   0x0121
#define PPPOE_TAG_SERVICE_ERR    0x0201
#define PPPOE_TAG_AC_SYSTEM_ERR  0x0202
#define PPPOE_TAG_GENERIC_ERR    0x0203

#define PPPOE_HDR_SIZE     sizeof(pppoe_Header)
#define PPPOE_TAG_HDR_SIZE 4

extern void pppoe_init (void);
extern void pppoe_exit (void);
extern int  pppoe_discovery (void);
extern int  pppoe_handler (struct pppoe_Packet *pkt);
extern BOOL pppoe_is_up (void);

#endif
