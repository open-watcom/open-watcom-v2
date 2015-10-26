#ifndef __PCCONFIG_H
#define __PCCONFIG_H

struct config_table {
       const char *keyword;
       int         type;
       void       *arg_func;
     };

#define ARG_ATOI    0   /* convert to int */
#define ARG_ATOB    1   /* convert to 8-bit byte */
#define ARG_ATOW    2   /* convert to 16-bit word */
#define ARG_ATON    3   /* convert to ip-address */
#define ARG_ATOX_B  4   /* convert to hex-byte */
#define ARG_ATOX_W  5   /* convert to hex-word */
#define ARG_STRDUP  6   /* duplicate string value */
#define ARG_RESOLVE 7   /* resolve host to ip-address */
#define ARG_FUNC    8   /* call convertion function */

#define sock_inactive      NAMESPACE (sock_inactive)
#define sock_data_timeout  NAMESPACE (sock_data_timeout)
#define sock_delay         NAMESPACE (sock_delay)
#define debug_on           NAMESPACE (debug_on)
#define multihomes         NAMESPACE (multihomes)
#define usr_init           NAMESPACE (usr_init)
#define cookies            NAMESPACE (cookies)
#define last_cookie        NAMESPACE (last_cookie)
#define ctrace_on          NAMESPACE (ctrace_on)
#define dynamic_host       NAMESPACE (dynamic_host)
#define print_hook         NAMESPACE (print_hook)

extern int    sock_inactive;
extern int    sock_data_timeout;
extern int    sock_delay;
extern int    multihomes;
extern void  (*print_hook) (const char*);
extern void  (*usr_init)   (const char*, const char*);

extern DWORD cookies [MAX_COOKIES];
extern WORD  last_cookie;
extern int   debug_on;
extern int   ctrace_on;
extern int   dynamic_host;

extern char  *_watt_config_name;
extern char  *_watt_environ_name, *_watt_environ_name1;

extern const char *inet_atoeth (const char *src, BYTE *eth);

extern void  _add_server   (WORD *counter, WORD max, DWORD *array, DWORD value);
extern int   tcp_config    (const char *path);
extern void  tcp_set_debug_state (WORD x);
extern BYTE  atox          (const char *value);

extern int   parse_config_table (const struct config_table *tab,
                                 const char *section,
                                 const char *name,
                                 const char *value);

extern int   netdb_init    (void);
extern int   is_local_addr (DWORD ip);
extern int   is_unique_addr(DWORD ip);
extern int   is_multicast  (DWORD ip);
extern int   is_ip_brdcast (const in_Header *ip);

#define is_it(x) if (!strcmp(name,x))

#endif

