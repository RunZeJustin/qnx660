/*	$NetBSD: keydb.h,v 1.2 2005/12/10 23:44:08 elad Exp $	*/
/*	$FreeBSD: src/sys/netipsec/keydb.h,v 1.1.4.1 2003/01/24 05:11:36 sam Exp $	*/
/*	$KAME: keydb.h,v 1.14 2000/08/02 17:58:26 sakane Exp $	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _NETIPSEC_KEYDB_H_INCLUDED
#define _NETIPSEC_KEYDB_H_INCLUDED

#ifdef _KERNEL

#include <netipsec/key_var.h>
#include <net/route.h>
#include <netinet/in.h>

/*
 * The union of all possible address formats we handle.
 */
union sockaddr_union {
	struct sockaddr		sa;
	struct sockaddr_in	sin;
	struct sockaddr_in6	sin6;
};

/* Security Assocciation Index */
/* NOTE: Ensure to be same address family */
struct secasindex {
	union sockaddr_union src;	/* srouce address for SA */
	union sockaddr_union dst;	/* destination address for SA */
	u_int16_t proto;		/* IPPROTO_ESP or IPPROTO_AH */
	u_int8_t mode;			/* mode of protocol, see ipsec.h */
	u_int32_t reqid;		/* reqid id who owned this SA */
					/* see IPSEC_MANUAL_REQID_MAX. */
};

/* Security Association Data Base */
struct secashead {
	LIST_ENTRY(secashead) chain;

	struct secasindex saidx;

	struct sadb_ident *idents;	/* source identity */
	struct sadb_ident *identd;	/* destination identity */
					/* XXX I don't know how to use them. */

	u_int8_t state;			/* MATURE or DEAD. */
	LIST_HEAD(_satree, secasvar) savtree[SADB_SASTATE_MAX+1];
					/* SA chain */
					/* The first of this list is newer SA */

	struct route sa_route;		/* route cache */
};

struct xformsw;
struct enc_xform;
struct auth_hash;
struct comp_algo;

/* Security Association */
struct secasvar {
	LIST_ENTRY(secasvar) chain;

	u_int refcnt;			/* reference count */
	u_int8_t state;			/* Status of this Association */

	u_int8_t alg_auth;		/* Authentication Algorithm Identifier*/
	u_int8_t alg_enc;		/* Cipher Algorithm Identifier */
	u_int8_t alg_comp;		/* Compression Algorithm Identifier */
	u_int32_t spi;			/* SPI Value, network byte order */
	u_int32_t flags;		/* holder for SADB_KEY_FLAGS */

	struct sadb_key *key_auth;	/* Key for Authentication */
	struct sadb_key *key_enc;	/* Key for Encryption */
	caddr_t iv;			/* Initilization Vector */
	u_int ivlen;			/* length of IV */
	void *sched;			/* intermediate encryption key */
	size_t schedlen;

	struct secreplay *replay;	/* replay prevention */
	long created;			/* for lifetime */

	struct sadb_lifetime *lft_c;	/* CURRENT lifetime, it's constant. */
	struct sadb_lifetime *lft_h;	/* HARD lifetime */
	struct sadb_lifetime *lft_s;	/* SOFT lifetime */

	u_int32_t seq;			/* sequence number */
	pid_t pid;			/* message's pid */

	struct secashead *sah;		/* back pointer to the secashead */

	/*
	 * NB: Fields with a tdb_ prefix are part of the "glue" used
	 *     to interface to the OpenBSD crypto support.  This was done
	 *     to distinguish this code from the mainline KAME code.
	 */
	struct xformsw *tdb_xform;	/* transform */
	struct enc_xform *tdb_encalgxform;	/* encoding algorithm */
	struct auth_hash *tdb_authalgxform;	/* authentication algorithm */
	struct comp_algo *tdb_compalgxform;	/* compression algorithm */
	u_int64_t tdb_cryptoid;		/* crypto session id */

#ifdef IPSEC_NAT_T
	u_int16_t natt_type;
	u_int16_t esp_frag;
#endif	
#ifdef __QNXNTO__
	char if_inner[ IFNAMSIZ ];
	char if_outer[ IFNAMSIZ ];
	struct ifnet *ifn_inner;
	struct ifnet *ifn_outer;
#endif
};

/* replay prevention */
struct secreplay {
	u_int32_t count;
	u_int wsize;		/* window size, i.g. 4 bytes */
	u_int32_t seq;		/* used by sender */
	u_int32_t lastseq;	/* used by receiver */
	caddr_t bitmap;		/* used by receiver */
	int overflow;		/* overflow flag */
};

/* socket table due to send PF_KEY messages. */
struct secreg {
	LIST_ENTRY(secreg) chain;

	struct socket *so;
};

#ifndef IPSEC_NONBLOCK_ACQUIRE
/* acquiring list table. */
struct secacq {
	LIST_ENTRY(secacq) chain;

	struct secasindex saidx;

	u_int32_t seq;		/* sequence number */
	long created;		/* for lifetime */
	int count;		/* for lifetime */
};
#endif

/* Sensitivity Level Specification */
/* nothing */

#define SADB_KILL_INTERVAL	600	/* six seconds */

/* secpolicy */
extern struct secpolicy *keydb_newsecpolicy __P((void));
extern void keydb_delsecpolicy __P((struct secpolicy *));
/* secashead */
extern struct secashead *keydb_newsecashead __P((void));
extern void keydb_delsecashead __P((struct secashead *));
/* secasvar */
extern struct secasvar *keydb_newsecasvar __P((void));
extern void keydb_refsecasvar __P((struct secasvar *));
extern void keydb_freesecasvar __P((struct secasvar *));
/* secreplay */
extern struct secreplay *keydb_newsecreplay __P((size_t));
extern void keydb_delsecreplay __P((struct secreplay *));
/* secreg */
extern struct secreg *keydb_newsecreg __P((void));
extern void keydb_delsecreg __P((struct secreg *));

#endif /* _KERNEL */


#endif /* !_NETIPSEC_KEYDB_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/socket/public/netipsec/keydb.h $ $Rev: 680336 $")
#endif
