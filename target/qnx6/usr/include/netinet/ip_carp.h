/*	$NetBSD: ip_carp.h,v 1.2 2006/06/13 15:57:15 riz Exp $	*/
/*	$OpenBSD: ip_carp.h,v 1.18 2005/04/20 23:00:41 mpf Exp $	*/

/*
 * Copyright (c) 2002 Michael Shalayeff. All rights reserved.
 * Copyright (c) 2003 Ryan McBride. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR HIS RELATIVES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF MIND, USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NETINET_IP_CARP_H_INCLUDED
#define _NETINET_IP_CARP_H_INCLUDED

#ifndef _INTTYPES_H_INCLUDED
#include <inttypes.h>
#endif

/*
 * The CARP header layout is as follows:
 *
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |Version| Type  | VirtualHostID |    AdvSkew    |    Auth Len   |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Reserved    |     AdvBase   |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Counter (1)                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Counter (2)                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (1)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (2)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (3)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (4)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                        SHA-1 HMAC (5)                         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

struct carp_header {
#if BYTE_ORDER == LITTLE_ENDIAN
	uint8_t	carp_type:4,
			carp_version:4;
#endif
#if BYTE_ORDER == BIG_ENDIAN
	uint8_t	carp_version:4,
			carp_type:4;
#endif
	uint8_t	carp_vhid;	/* virtual host id */
	uint8_t	carp_advskew;	/* advertisement skew */
	uint8_t	carp_authlen;   /* size of counter+md, 32bit chunks */
	uint8_t	carp_pad1;	/* reserved */
	uint8_t	carp_advbase;	/* advertisement interval */
	uint16_t	carp_cksum;
	uint32_t	carp_counter[2];
	unsigned char	carp_md[20];	/* SHA1 HMAC */
} __attribute__((__packed__));

#define	CARP_DFLTTL		255

/* carp_version */
#define	CARP_VERSION		2

/* carp_type */
#define	CARP_ADVERTISEMENT	0x01

#define	CARP_KEY_LEN		20	/* a sha1 hash of a passphrase */

/* carp_advbase */
#define	CARP_DFLTINTV		1

/*
 * Statistics.
 */
struct carpstats {
	uint64_t	carps_ipackets;		/* total input packets, IPv4 */
	uint64_t	carps_ipackets6;	/* total input packets, IPv6 */
	uint64_t	carps_badif;		/* wrong interface */
	uint64_t	carps_badttl;		/* TTL is not CARP_DFLTTL */
	uint64_t	carps_hdrops;		/* packets shorter than hdr */
	uint64_t	carps_badsum;		/* bad checksum */
	uint64_t	carps_badver;		/* bad (incl unsupp) version */
	uint64_t	carps_badlen;		/* data length does not match */
	uint64_t	carps_badauth;		/* bad authentication */
	uint64_t	carps_badvhid;		/* bad VHID */
	uint64_t	carps_badaddrs;		/* bad address list */

	uint64_t	carps_opackets;		/* total output packets, IPv4 */
	uint64_t	carps_opackets6;	/* total output packets, IPv6 */
	uint64_t	carps_onomem;		/* no memory for an mbuf */
	uint64_t	carps_ostates;		/* total state updates sent */

	uint64_t	carps_preempt;		/* if enabled, preemptions */
};

#define CARPDEVNAMSIZ	16
#ifdef IFNAMSIZ
#if CARPDEVNAMSIZ != IFNAMSIZ
#error
#endif
#endif

/*
 * Configuration structure for SIOCSVH SIOCGVH
 */
struct carpreq {
	int		carpr_state;
#define	CARP_STATES	"INIT", "BACKUP", "MASTER"
#define	CARP_MAXSTATE	2

	char		carpr_carpdev[CARPDEVNAMSIZ];
	int		carpr_vhid;
	int		carpr_advskew;
	int		carpr_advbase;
	unsigned char	carpr_key[CARP_KEY_LEN];
};

/*
 * Names for CARP sysctl objects
 */
#define	CARPCTL_ALLOW		1	/* accept incoming CARP packets */
#define	CARPCTL_PREEMPT		2	/* high-pri backup preemption mode */
#define	CARPCTL_LOG		3	/* log bad packets */
#define	CARPCTL_ARPBALANCE	4	/* balance arp responses */
#define CARPCTL_STATS		5	/* carp statistics */
#define	CARPCTL_MAXID		6

#define	CARPCTL_NAMES { \
	{ 0, 0 }, \
	{ "allow", CTLTYPE_INT }, \
	{ "preempt", CTLTYPE_INT }, \
	{ "log", CTLTYPE_INT }, \
	{ "arpbalance", CTLTYPE_INT }, \
}

#ifdef _KERNEL
void		 carp_ifdetach (struct ifnet *);
void		 carp_proto_input (struct mbuf *, ...);
void		 carp_carpdev_state(void *);
int		 carp6_proto_input(struct mbuf **, int *, int);
int		 carp_iamatch(struct in_ifaddr *, u_char *,
		     uint32_t *, uint32_t);
struct ifaddr	*carp_iamatch6(void *, struct in6_addr *);
struct ifnet	*carp_ourether(void *, struct ether_header *, u_char, int);
int		 carp_input(struct mbuf *, uint8_t *, uint8_t *, uint16_t);
int		 carp_output(struct ifnet *, struct mbuf *, struct sockaddr *,
		     struct rtentry *);
#endif /* _KERNEL */
#endif /* !_NETINET_IP_CARP_H_INCLUDED */

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn/product/branches/6.6.0/trunk/lib/socket/public/netinet/ip_carp.h $ $Rev: 680336 $")
#endif
