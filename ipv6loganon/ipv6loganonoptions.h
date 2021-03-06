/*
 * Project    : ipv6calc
 * File       : ipv6loganonoptions.h
 * Version    : $Id: ipv6loganonoptions.h,v 1.14 2014/07/31 17:22:51 ds6peter Exp $
 * Copyright  : 2007-2013 by Peter Bieringer <pb (at) bieringer.de>
 *
 * Information:
 *  Header file containing options for ipvloganon.c
 */ 

#include <getopt.h> 

#include "ipv6calctypes.h"


#ifndef _ipv6loganonoptions_h_

#define _ipv6loganonoptions_h_

/* Options */

/* define short options */
static char *ipv6loganon_shortopts = "vh?nc:w:a:f";

/* define long options */
static struct option ipv6loganon_longopts[] = {
	{"version", 0, NULL, (int) 'v' },

	/* help options */
	{"help"           , 0, NULL, (int) 'h'},

	/* special options */
	{"nocache"   , no_argument      , 0, (int) 'n'},
	{"flush"     , no_argument      , 0, (int) 'f'},
	{"cachelimit", required_argument, 0, (int) 'c'},
	{"write"     , required_argument, 0, (int) 'w'},
	{"append"    , required_argument, 0, (int) 'a'},
};                

#endif
