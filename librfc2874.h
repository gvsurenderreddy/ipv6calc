/*
 * Project    : ipv6calc
 * File       : librfc2874.h
 * Version    : $Id: librfc2874.h,v 1.1 2002/02/19 21:41:18 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb@bieringer.de>
 *
 * Information:
 *  Header file for librfc2874.c
 */ 

/* prototypes */
#include "ipv6calc.h"

extern int librfc2874_addr_to_bitstring(ipv6calc_ipv6addr *ipv6addrp, char *resultstring, long int command);
