/*
 * Project    : ipv6calc
 * File       : ipv6calc.c
 * Version    : $Id: ipv6calc.c,v 1.1 2002/03/18 20:00:20 peter Exp $
 * Copyright  : 2001-2002 by Peter Bieringer <pb (at) bieringer.de>
 * 
 * Information:
 *  Central program (main)
 *  This program print out different formats of an given IP(v4/v6) address
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <getopt.h> 

#include "ipv6calc.h"
#include "libipv6calc.h"
#include "ipv6calctypes.h"
#include "ipv6calcoptions.h"
#include "ipv6calchelp.h"

#include "libipv4addr.h"
#include "libipv6addr.h"
#include "libmac.h"

#include "showinfo.h"

#include "librfc1884.h"
#include "librfc1886.h"
#include "librfc1924.h"
#include "libifinet6.h"
#include "librfc2874.h"
#include "librfc3056.h"
#include "librfc3041.h"
#include "libeui64.h"
#include "version.h"

long int ipv6calc_debug = 0;

void printversion(void) {
	fprintf(stderr, "\n");
	fprintf(stderr, "%s: version %s\n", PROGRAM_NAME, PROGRAM_VERSION);
};

void printcopyright(void) {
	fprintf(stderr, "%s\n", PROGRAM_COPYRIGHT);
};

void printinfo(void)  {
	printversion();
	printcopyright();
	fprintf(stderr, "This program formats and calculates IPv6 addresses\n");
	fprintf(stderr, "See '%s -?' for more details\n\n", PROGRAM_NAME);
};


/**************************************************/
/* main */
#define DEBUG_function_name "ipv6calc/main"
int main(int argc,char *argv[]) {
	char resultstring[NI_MAXHOST] = "";
	char resultstring2[NI_MAXHOST] = "";
	char resultstring3[NI_MAXHOST] = "";
	int retval = 1, i, j, lop;
	unsigned long int command = 0;
	int bit_start = 0, bit_end = 0;

	/* new option style storage */	
	int inputtype = -1, outputtype = -1;
	
	/* convert storage */
	long int action = -1;

	/* format options storage */
	int formatoptions = 0;

	/* used structures */
	ipv6calc_ipv6addr ipv6addr, ipv6addr2, ipv6addr3, ipv6addr4;
	ipv6calc_ipv4addr ipv4addr;
	ipv6calc_macaddr  macaddr;

	if (argc <= 1) {
		printinfo();
		exit(EXIT_FAILURE);
	};

	/* Fetch the command-line arguments. */
	while ((i = getopt_long(argc, argv, ipv6calc_shortopts, ipv6calc_longopts, &lop)) != EOF) {
		switch (i) {
			case -1:
				break;
			case 'v':
				command |= CMD_printversion;
				break;
			case 'h':
			case '?':
				command |= CMD_printhelp;
				break;
				
			case 'd':
				ipv6calc_debug = atol(optarg);
				break;

			case CMD_printexamples:
				command = CMD_printexamples;
				break;


			/* backward compatibility/shortcut commands */
			case 'r':
			case CMD_addr_to_ip6int:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_revnibbles_int;
				break;

			case 'a':
			case CMD_addr_to_ip6arpa:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_revnibbles_arpa;
				break;

			case 'b':
			case CMD_addr_to_bitstring:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_bitstring;
				break;
				
			case CMD_addr_to_compressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_addr_to_uncompressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case CMD_addr_to_base85:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_base85;
				break;
				
			case CMD_base85_to_addr:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_base85;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printuncompressed;
				break;

			case CMD_mac_to_eui64:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_mac;
				outputtype = FORMAT_eui64;
				break;
				
			case CMD_addr_to_fulluncompressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case CMD_addr_to_ifinet6:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype  = FORMAT_ipv6addr;
				outputtype = FORMAT_ifinet6;
				break;

			case CMD_ifinet6_to_compressed:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_ifinet6;
				outputtype = FORMAT_ipv6addr;
				formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case CMD_ipv4_to_6to4addr:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_ipv4addr;
				outputtype = FORMAT_ipv6addr;
				action = ACTION_ipv4_to_6to4addr;
				break;
				
			case CMD_eui64_to_privacy:
				if (inputtype >= 0 || outputtype >= 0) { printhelp_doublecommands(); exit(EXIT_FAILURE); };
				inputtype = FORMAT_iid_token;
				outputtype = FORMAT_iid_token;
				break;

			case 'i':
			case CMD_showinfo:
				command = CMD_showinfo;
				break;

			case CMD_showinfotypes:
				command = CMD_showinfotypes;
				break;

			/* format options */
			case FORMATOPTION_printcompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printcompressed;
				break;
				
			case FORMATOPTION_printuncompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printuncompressed;
				break;
				
			case FORMATOPTION_printfulluncompressed + FORMATOPTION_HEAD:
	       			formatoptions |= FORMATOPTION_printfulluncompressed;
				break;
				
			case FORMATOPTION_printprefix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printprefix;
				break;
				
			case FORMATOPTION_printsuffix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printsuffix;
				break;
				
			case FORMATOPTION_maskprefix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_maskprefix;
				break;
				
			case FORMATOPTION_masksuffix + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_masksuffix;
				break;
				
			case 'l':	
			case FORMATOPTION_printlowercase + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printlowercase;
				break;
				
			case 'u':	
			case FORMATOPTION_printuppercase + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_printuppercase;
				break;
				
			case FORMATOPTION_printstart + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_start = atoi(optarg);
					formatoptions |= FORMATOPTION_printstart;
				} else {
					fprintf(stderr, " Argument of option 'printstart' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case FORMATOPTION_printend + FORMATOPTION_HEAD:
				if ((atoi(optarg) >= 1) && (atoi(optarg) <= 128)) {
					bit_end = atoi(optarg);
					formatoptions |= FORMATOPTION_printend;
				} else {
					fprintf(stderr, " Argument of option 'printend' is out or range (1-128): %d\n", atoi(optarg));
					exit(EXIT_FAILURE);
				};
				break;
				
			case 'm':	
			case FORMATOPTION_machinereadable + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_machinereadable;
				break;
				
			case 'q':	
			case FORMATOPTION_quiet + FORMATOPTION_HEAD:
				formatoptions |= FORMATOPTION_quiet;
				break;

			/* new options */
			case CMD_inputtype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got input string: %s\n", DEBUG_function_name, optarg);
				};

				if (strcmp(optarg, "-?") == 0) {
					inputtype = -2;
					command = CMD_printhelp;
					break;
				};
				
				inputtype = ipv6calctypes_checktype(optarg);
				
				if (inputtype < 0) {
					fprintf(stderr, " Input option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;	
				
			case CMD_outputtype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got output string: %s\n", DEBUG_function_name, optarg);
				};
				if (strcmp(optarg, "-?") == 0) {
					outputtype = -2;
					command = CMD_printhelp;
					break;
				};
				
				outputtype = ipv6calctypes_checktype(optarg);
				if (outputtype < 0) {
					fprintf(stderr, " Output option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;	

			case CMD_actiontype:
				if (ipv6calc_debug != 0) {
					fprintf(stderr, "%s: Got action string: %s\n", DEBUG_function_name, optarg);
				};
				if (strcmp(optarg, "-?") == 0) {
					action = -2;
					command = CMD_printhelp;
					break;
				};
				action = ipv6calctypes_checkaction(optarg);
				if (action < 0) {
					fprintf(stderr, " Action option is unknown: %s\n", optarg);
					exit(EXIT_FAILURE);
				};
				break;
				
			default:
				fprintf(stderr, "Usage: (see '%s --command -?' for more help)\n", PROGRAM_NAME);
				printhelp();
				break;
		};
	};
	argv += optind;
	argc -= optind;

	/* print help handling */
	if (command == CMD_printhelp) {
		if (outputtype == -2) {
			if (inputtype < 0) {
				inputtype = 0;
			};
			printhelp_outputtypes(inputtype);
			exit(EXIT_FAILURE);
		} else if (inputtype == -2) {
			printhelp_inputtypes();
			exit(EXIT_FAILURE);
		} else if (action == -2) {
			printhelp_actiontypes();
			exit(EXIT_FAILURE);
		};

	} else if (command == CMD_printexamples) {
		printhelp_output_dispatcher(outputtype);
		exit(EXIT_FAILURE);
	} else if (command == CMD_showinfotypes) {
		showinfo_availabletypes();
		exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "Debug value:%lx  command:%lx  inputtype:%lx   outputtype:%lx  action:%lx  formatoptions:%x\n", (unsigned long) ipv6calc_debug, command, (unsigned long) inputtype, (unsigned long) outputtype, (unsigned long) action, formatoptions); 
	};
	
	/* do work depending on selection */
	if (command == CMD_printversion) {
		printversion();
		exit(EXIT_FAILURE);
	};

	if (command == CMD_printhelp) {
		printhelp();
		exit(EXIT_FAILURE);
	};
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Got input %s\n", DEBUG_function_name, argv[0]);
	};

	/***** automatic action handling *****/
	if (inputtype == FORMAT_mac && outputtype ==FORMAT_eui64) {
		action = ACTION_mac_to_eui64;
	} else if (inputtype == FORMAT_iid_token && outputtype ==FORMAT_iid_token) {;
		action = ACTION_iid_token_to_privacy;
	};

	/***** input type handling *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of input type handling\n", DEBUG_function_name);
	};

	/* check length of input */
	if (argc > 0) {
		for (i = 0; i < argc; i++) {
			if ( argv[i] != NULL ) {
				if ( strlen(argv[i]) >= NI_MAXHOST ) {
					/* that's not good for size limited buffers...*/
					fprintf(stderr, "Argument %d is too long\n", i);
					exit(EXIT_FAILURE);
				};
			};
		};
	};
	
	/* autodetection */
	if ((inputtype == -1 || inputtype == FORMAT_auto) && argc > 0) {
		if (ipv6calc_debug != 0) {
			fprintf(stderr, "%s: Call input type autodetection\n", DEBUG_function_name);
		};
		/* no input type specified or automatic selected */
		if ((formatoptions & FORMATOPTION_quiet) == 0) {
			fprintf(stderr, "No input type specified, try autodetection...");
		};
		
		inputtype = libipv6calc_autodetectinput(argv[0]);

		if (inputtype >= 0) {
			for (i = 0; i < (int) (sizeof(ipv6calc_formatstrings) / sizeof(ipv6calc_formatstrings[0])); i++) {
				if (inputtype == ipv6calc_formatstrings[i].number) {
					if ((formatoptions & FORMATOPTION_quiet) == 0) {
						fprintf(stderr, "found type: %s\n", ipv6calc_formatstrings[i].token);
					};
					break;
				};
			};
		} else {
			fprintf(stderr, "no result!\n");
		};
	};

	/* check formatoptions whether valid */
	for (i = 0; i < (int) (sizeof(ipv6calc_outputformatoptionmap) / sizeof(ipv6calc_outputformatoptionmap[0])); i++) {
		if (outputtype != ipv6calc_outputformatoptionmap[i][0]) {
			continue;
		};
		
		if ((ipv6calc_outputformatoptionmap[i][1] & formatoptions) == formatoptions) {
			/* all options valid */
			break;
		};
		
		fprintf(stderr, " Unsupported format option(s):\n");

		/* run through format options */
		for (j = 0; j < (int) (sizeof(ipv6calc_formatoptionstrings) / sizeof (ipv6calc_formatoptionstrings[0])); j++) {
			if ((((~ ipv6calc_outputformatoptionmap[i][1]) & formatoptions) & ipv6calc_formatoptionstrings[j].number) != 0) {
				fprintf(stderr, "  %s: %s\n", ipv6calc_formatoptionstrings[j].token, ipv6calc_formatoptionstrings[j].explanation);
			};
		};
		exit(EXIT_FAILURE);
	};
	
	/* proceed input depending on type */	
	retval = -1; /* default */

	switch (inputtype) {
		case FORMAT_ipv6addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addr_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;

		case FORMAT_ipv4addr:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = addr_to_ipv4addrstruct(argv[0], resultstring, &ipv4addr);
			argc--;
			break;

		case FORMAT_base85:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = base85_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			break;
			
		case FORMAT_mac:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = mac_to_macaddrstruct(argv[0], resultstring, &macaddr);
			argc--;
			break;

		case FORMAT_ifinet6:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			if (argc < 2) {
				retval = libifinet6_ifinet6_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
				argc--;
			} else {
				retval = libifinet6_ifinet6_withprefixlength_to_ipv6addrstruct(argv[0], argv[1], resultstring, &ipv6addr);
				argc -= 2;
			};
			break;

		case FORMAT_iid_token:
			/* Get first interface identifier */
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = identifier_to_ipv6addrstruct(argv[0], resultstring, &ipv6addr);
			argc--;
			if (retval != 0) { break; };
			
			/* Get second token */
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = tokenlsb64_to_ipv6addrstruct(argv[1], resultstring, &ipv6addr2);
			argc--;
			break;

		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = librfc1886_nibblestring_to_ipv6addrstruct(argv[0], &ipv6addr, resultstring);
			argc--;
			break;
			
		case FORMAT_bitstring:
			if (argc < 1) { printhelp_missinginputdata(); exit(EXIT_FAILURE); };
			retval = librfc2874_bitstring_to_ipv6addrstruct(argv[0], &ipv6addr, resultstring);
			argc--;
			break;
			
		default:
			fprintf(stderr, " Input-type isn't implemented\n");
			exit(EXIT_FAILURE);
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit(EXIT_FAILURE);
	};
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of input type handling\n", DEBUG_function_name);
	};

	/***** postprocessing input *****/
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of postprocessing input\n", DEBUG_function_name);
	};

	if (ipv6addr.flag_valid == 1) {
		/* mask bits */
		if ((formatoptions & (FORMATOPTION_maskprefix | FORMATOPTION_masksuffix)) != 0) {
			if (ipv6addr.flag_prefixuse == 1) {
				if ((formatoptions & FORMATOPTION_maskprefix) != 0) {
					ipv6addrstruct_maskprefix(&ipv6addr);
				} else if ((formatoptions & FORMATOPTION_masksuffix) != 0) {
					ipv6addrstruct_masksuffix(&ipv6addr);
				};
			} else {
				fprintf(stderr, " Error: mask option used without specifying a prefix length\n");
				exit(EXIT_FAILURE);
			};
		};
		
		/* start bit */
		if ((formatoptions & FORMATOPTION_printstart) != 0) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Set bit start to: %d\n", DEBUG_function_name, bit_start);
			};
			ipv6addr.bit_start = (uint8_t) bit_start;
			ipv6addr.flag_startend_use = 1;
		} else {
			ipv6addr.bit_start = 1;
		};
		
		/* end bit */
		if ((formatoptions & FORMATOPTION_printend) != 0) {
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Set bit end to: %d\n", DEBUG_function_name, bit_end);
			};
			ipv6addr.bit_end = (uint8_t) bit_end;
			ipv6addr.flag_startend_use = 1;
		} else {
			/* default */
			ipv6addr.bit_end = 128;
		};
		
		/* prefix+suffix */
		if ((formatoptions & (FORMATOPTION_printprefix | FORMATOPTION_printsuffix)) != 0) {
			if ( ipv6addr.flag_prefixuse == 0 ) {
				fprintf(stderr, " Error: missing prefix length for printing prefix/suffix\n");
				exit(EXIT_FAILURE);
			} else {
				if ( ipv6addr.flag_startend_use == 0 ) {
					/* only print[prefix|suffix] */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						ipv6addr.bit_start = 1;
						ipv6addr.bit_end = ipv6addr.prefixlength;
						ipv6addr.flag_startend_use = 1;
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						ipv6addr.bit_end = 128;
						ipv6addr.flag_startend_use = 1;
					};
				} else {
					/* mixed */
					if ((formatoptions & FORMATOPTION_printprefix) != 0) {
						if ( ipv6addr.prefixlength < ipv6addr.bit_start ) {
							fprintf(stderr, " Error: prefix length '%u' lower than given start bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%u' higher than given end bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_end = ipv6addr.prefixlength;
						};
					} else if ((formatoptions & FORMATOPTION_printsuffix) != 0) {
						if ( ipv6addr.prefixlength >= ipv6addr.bit_end ) {
							fprintf(stderr, " Error: prefix length '%u' higher than or eqal to given end bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_end);
							exit(EXIT_FAILURE);
						} else if (ipv6addr.prefixlength >= ipv6addr.bit_start) {
							fprintf(stderr, " Error: prefix length '%u' higher than or equal to given start bit number '%u'\n", (unsigned int) ipv6addr.prefixlength, (unsigned int) ipv6addr.bit_start);
							exit(EXIT_FAILURE);
						} else {
							ipv6addr.bit_start = ipv6addr.prefixlength + 1;
						};
					};
				};
			};
		};

		/* check start/end */
		if ( ipv6addr.flag_startend_use == 1 ) {
			if ( ipv6addr.bit_start > ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit bigger than end bit\n");
				exit(EXIT_FAILURE);
			} else if ( ipv6addr.bit_start == ipv6addr.bit_end ) {
				fprintf(stderr, " Error: start bit equal to end bit\n");
				exit(EXIT_FAILURE);
			};
		};
	};

	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: result of 'inputtype': %d\n", DEBUG_function_name, retval);
	};

	if (retval != 0) {
		fprintf(stderr, "%s\n", resultstring);
		exit(EXIT_FAILURE);
	};
	
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of postprocessing input\n", DEBUG_function_name);
	};
	
	/***** action *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of action\n", DEBUG_function_name);
	};

	/* clear resultstring */
	sprintf(resultstring, "%s", "");
	
	switch (action) {
		case ACTION_mac_to_eui64:
			if (macaddr.flag_valid != 1) {
				fprintf(stderr, "No valid MAC address given!\n");
				exit(EXIT_FAILURE);
			};
			retval = create_eui64_from_mac(&ipv6addr, &macaddr);
			break;
			
		case ACTION_ipv4_to_6to4addr:
			if (inputtype == FORMAT_ipv4addr && outputtype == FORMAT_ipv6addr) {
				/* IPv4 -> IPv6 */
				if (ipv4addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv4 address given!\n");
					exit(EXIT_FAILURE);
				};
				retval = librfc3056_ipv4addr_to_ipv6to4addr(&ipv6addr, &ipv4addr);
			} else if (inputtype == FORMAT_ipv6addr && outputtype == FORMAT_ipv4addr) {
				/* IPv6 -> IPv4 */
				if (ipv6addr.flag_valid != 1) {
					fprintf(stderr, "No valid IPv6 address given!\n");
					exit(EXIT_FAILURE);
				};
				retval = librfc3056_ipv6addr_to_ipv4addr(&ipv4addr, &ipv6addr, resultstring);
			} else {
				fprintf(stderr, "Unsupported 6to4 conversion!\n");
				exit(EXIT_FAILURE);
			};
			break;
			
		case ACTION_iid_token_to_privacy:
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) {
				fprintf(stderr, "No valid interface identifiers given!\n");
				exit(EXIT_FAILURE);
			};
			retval = librfc3041_calc(&ipv6addr, &ipv6addr2, &ipv6addr3, &ipv6addr4);
			ipv6addr_copy(&ipv6addr, &ipv6addr3);
			ipv6addr_copy(&ipv6addr2, &ipv6addr4);
			break;

		default:
			/* no action */
			break;
	};

	if (retval != 0) {
		fprintf(stderr, "Problem occurs during action: %s\n", resultstring);
		exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of action\n", DEBUG_function_name);
	};

	/***** output type *****/
	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: Start of output handling\n", DEBUG_function_name);
	};
	
	/* catch showinfo */	
	if (command == CMD_showinfo) {
		if (ipv6addr.flag_valid == 1) {
			retval = showinfo_ipv6addr(&ipv6addr, formatoptions);
	       	} else if (ipv4addr.flag_valid == 1) {
			retval = showinfo_ipv4addr(&ipv4addr, formatoptions);
	       	} else if (macaddr.flag_valid == 1) {
		       	fprintf(stderr, "Showinfo of MAC address currently not implemented!\n");
			retval = 1;
		} else {
		       	fprintf(stderr, "No valid IPv6 address given!\n");
			retval = 1;
		};
		if (retval != 0) {
			fprintf(stderr, "Problem occurs during selection of showinfo\n");
			exit(EXIT_FAILURE);
		};
		goto RESULT_print;
	};


	switch (outputtype) {
		case -1:
			/* old implementation */
			break;

		case FORMAT_base85:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = ipv6addrstruct_to_base85(&ipv6addr, resultstring);
			break;
				
		case FORMAT_bitstring:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if (ipv6calc_debug != 0) {
				fprintf(stderr, "%s: Call 'librfc2874_addr_to_bitstring'\n", DEBUG_function_name);
			};
			retval = librfc2874_addr_to_bitstring(&ipv6addr, resultstring, formatoptions);
			break;
				
		case FORMAT_revnibbles_int:
		case FORMAT_revnibbles_arpa:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			switch (outputtype) {
				case FORMAT_revnibbles_int:
					retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.int.");
					break;
				case FORMAT_revnibbles_arpa:
					retval = librfc1886_addr_to_nibblestring(&ipv6addr, resultstring, formatoptions, "ip6.arpa.");
					break;
			};
			break;
			
		case FORMAT_ifinet6:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			retval = libifinet6_ipv6addrstruct_to_ifinet6(&ipv6addr, resultstring);
			break;

		case FORMAT_ipv6addr:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			if ((formatoptions & FORMATOPTION_printuncompressed) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			} else if ((formatoptions & FORMATOPTION_printfulluncompressed) != 0) {
				retval = libipv6addr_ipv6addrstruct_to_fulluncompaddr(&ipv6addr, resultstring, formatoptions);
			} else {
				retval = librfc1884_ipv6addrstruct_to_compaddr(&ipv6addr, resultstring, formatoptions);
			};
			break;
			
		case FORMAT_ipv4addr:
			if (ipv4addr.flag_valid != 1) { fprintf(stderr, "No valid IPv4 address given!\n"); exit(EXIT_FAILURE); };
			retval = libipv4addr_ipv4addrstruct_to_string(&ipv4addr, resultstring, formatoptions);
			break;
			
		case FORMAT_eui64:
			if (ipv6addr.flag_valid != 1) { fprintf(stderr, "No valid IPv6 address given!\n"); exit(EXIT_FAILURE); };
			formatoptions |= FORMATOPTION_printsuffix;
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring, formatoptions);
			break;
			
		case FORMAT_iid_token:
			if (ipv6addr.flag_valid != 1 || ipv6addr2.flag_valid != 1) { fprintf(stderr, "No valid IPv6 addresses given!\n"); exit(EXIT_FAILURE); };
			/* get interface identifier */
			retval = libipv6addr_ipv6addrstruct_to_uncompaddr(&ipv6addr, resultstring2, formatoptions | FORMATOPTION_printsuffix);
			if (retval != 0) { break; };
			
			/* get token */
			retval = libipv6addr_ipv6addrstruct_to_tokenlsb64(&ipv6addr2, resultstring3, formatoptions);
			
			/* cat together */
			sprintf(resultstring, "%s %s", resultstring2, resultstring3);
			break;


		default:
			fprintf(stderr, " Outputtype isn't implemented\n");
			exit(EXIT_FAILURE);
	};

	if (ipv6calc_debug != 0) {
		fprintf(stderr, "%s: End of output handling\n", DEBUG_function_name);
	};

RESULT_print:
	/* print result */
	if ( strlen(resultstring) > 0 ) {
		if ( retval == 0 ) {
			fprintf(stdout, "%s\n", resultstring);
		} else {
			fprintf(stderr, "%s\n", resultstring);
		};
	};
	exit(retval);
};
#undef DEBUG_function_name