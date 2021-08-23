/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 *
 *	from: @(#)rpc.h 1.9 88/02/08 SMI
 *	from: @(#)rpc.h	2.4 89/07/11 4.0 RPCSRC
 *	$Id: rpc.h,v 1.5 1996/01/30 23:32:20 mpp Exp $
 */

/*
 * rpc.h, Just includes the billions of rpc header files necessary to
 * do remote procedure calling.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */
#ifndef __RPC_RPC_H
#define __RPC_RPC_H

#include <rpc/types.h>		/* some typedefs */

/* external data representation interfaces */
#include <rpc/xdr.h>		/* generic (de)serializer */

/* Client side only authentication */
#include <rpc/auth.h>		/* generic authenticator (client side) */

/* Client side (mostly) remote procedure call */
#include <rpc/clnt.h>		/* generic rpc stuff */

/* semi-private protocol headers */
#include <rpc/rpc_msg.h>	/* protocol for rpc messages */
#include <rpc/auth_uni.h>      /* protocol for unix style cred */

/*
 *  Uncomment-out the next line if you are building the rpc library with
 *  DES Authentication (see the README file in the secure_rpc/ directory).
 */
#if 1
#include <rpc/auth_des.h>	/* protocol for des style cred */
#endif

/* Server side only remote procedure callee */
#include <rpc/svc.h>		/* service manager and multiplexer */
#include <rpc/svc_auth.h>	/* service side authenticator */

/*
 * COMMENT OUT THE NEXT INCLUDE (or add to the #ifndef) IF RUNNING ON
 * A VERSION OF UNIX THAT USES SUN'S NFS SOURCE.  These systems will
 * already have the structures defined by <rpc/netdb.h> included in <netdb.h>.
 */
/* routines for parsing /etc/rpc */

struct rpcent {
      char    *r_name;        /* name of server for this rpc program */
      char    **r_aliases;    /* alias list */
      long    r_number;       /* rpc program number. */
};                            /* Must be 32-bit for small/large model */

__BEGIN_DECLS

extern struct rpcent *getrpcbyname  (char *);
extern struct rpcent *getrpcbynumber(long);
extern struct rpcent *getrpcent     (void);
extern void setrpcent (int);
extern void endrpcent (void);

#if defined(__MSDOS__) || defined(__TURBOC__) || defined(__HIGHC__) || defined(__DJGPP__)

  /* from RPC-DOS v0.1Alpha by Mike Durkin <mdurkin@tsoft.net> */
  /*                       updated by G.Vanem <giva@bgnett.no> */

  extern void   rpc_init     (void);
  extern void   set_user_id  (u_long uid);
  extern void   set_group_id (u_long gid);

  extern u_long __geteuid    (void);
  extern u_long __getegid    (void);
#endif

__END_DECLS

#endif
