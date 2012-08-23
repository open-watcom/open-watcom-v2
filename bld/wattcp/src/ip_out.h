#ifndef __IP_OUTPUT_H
#define __IP_OUTPUT_H

extern int _ip_id_increment;
extern int _default_ttl;
extern int _default_tos;

extern WORD _get_ip_id (void);
extern WORD _get_this_ip_id (void);

extern int _ip_output (in_Header *ip, DWORD src_ip, DWORD dst_ip,
                       WORD protocol, BYTE ttl, BYTE tos, WORD ip_id,
                       int data_len, const void *sock,
                       const char *file, unsigned line);

#define IP_OUTPUT(ip, src, dst, proto, ttl, tos, ip_id, data_len, sock) \
       _ip_output(ip, src, dst, proto, ttl, tos, ip_id, data_len, sock, \
                  __FILE__, __LINE__)

extern int _chk_ip_header (const in_Header *ip);


#if defined(USE_FRAGMENTS)
  int send_ip_fragments (sock_type *sk, WORD proto, DWORD dest,
                         const void *buf, int len, const char *file,
                         unsigned line);

  #define SEND_IP_FRAGMENTS(sk, proto, dest, buf, len) \
          send_ip_fragments(sk, proto, dest, buf, len, __FILE__, __LINE__)
#endif

#endif
