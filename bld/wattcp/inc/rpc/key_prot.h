/*
 * This is a RPCGEN generated file. Do not edit.
 *
 * Generated: Sun Mar 14 19:22:40 1999
 */

#ifndef __KEY_PROT_H
#define __KEY_PROT_H


/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 */
#define PROOT                     3
#define HEXMODULUS                "d4a0ba0250b6fd2ec626e7efd637df76c716e22d0944b88b"
#define HEXKEYBYTES               48
#define KEYSIZE                   192
#define KEYBYTES                  24
#define KEYCHECKSUMSIZE           16

typedef enum keystatus {
          KEY_SUCCESS = 0,
          KEY_NOSECRET = 1,
          KEY_UNKNOWN = 2,
          KEY_SYSTEMERR = 3,
      } keystatus;
bool_t xdr_keystatus();

#ifndef KERNEL

typedef char keybuf[HEXKEYBYTES];
bool_t xdr_keybuf();

#endif

typedef char *netnamestr;
bool_t xdr_netnamestr();


typedef struct cryptkeyarg {
        netnamestr remotename;
        des_block deskey;
      } cryptkeyarg;
bool_t xdr_cryptkeyarg();


typedef struct cryptkeyres {
        keystatus status;
  union {
    des_block deskey;
    } cryptkeyres_u;
  } cryptkeyres;
bool_t xdr_cryptkeyres();

#define MAXGIDS                   16

typedef struct unixcred {
        long uid;
        long gid;
        struct {
          u_long gids_len;
          long *gids_val;
        } gids;
      } unixcred;
bool_t xdr_unixcred();


typedef struct getcredres {
        keystatus status;
  union {
    unixcred cred;
    } getcredres_u;
  } getcredres;
bool_t xdr_getcredres();


#define KEY_PROG                  100029UL
#define KEY_VERS                  1UL
#define KEY_SET                   1UL
extern long *key_set_1();

#define KEY_ENCRYPT               2UL
extern cryptkeyres *key_encrypt_1();

#define KEY_DECRYPT               3UL
extern cryptkeyres *key_decrypt_1();

#define KEY_GEN                   4UL
extern des_block *key_gen_1();

#define KEY_GETCRED               5UL
extern getcredres *key_getcred_1();


#endif /* !__KEY_PROT_H */
