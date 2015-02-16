#!/bin/bash
#
# Simple shell script to update ipv6calc (external) database files
#
# Project    : ipv6calc/databases
# File       : ipv6calc-db-update.sh
# Version    : $Id: ipv6calc-db-update.sh,v 1.3 2015/02/16 20:26:59 ds6peter Exp $
# Copyright  : 2015-2015 by Peter Bieringer <pb (at) bieringer.de>
# License    : GNU GPL version 2

IPV6CALC_DAT_DIR_DEFAULT="/usr/share/ipv6calc/db"

[ -z "$IPV6CALC_DAT_DIR" ] && IPV6CALC_DAT_DIR="$IPV6CALC_DAT_DIR_DEFAULT"

IPV6CALC_UPDATE_LIST="./ipv6calc-update-registries.sh /usr/share/ipv6calc/tools/ipv6calc-update-registries.sh"
IPV6CALC_CHECK_LIST="./ipv6calc-registries-check-run-create.sh /usr/share/ipv6calc/tools/ipv6calc-registries-check-run-create.sh"
IPV6CALC_CREATE_IPV4_LIST="./ipv6calc-create-registry-list-ipv4.pl /usr/share/ipv6calc/tools/ipv6calc-create-registry-list-ipv4.pl"
IPV6CALC_CREATE_IPV6_LIST="./ipv6calc-create-registry-list-ipv6.pl /usr/share/ipv6calc/tools/ipv6calc-create-registry-list-ipv6.pl"


help() {
	cat <<END
Usage: $(basename "$0") [-s] [-u]
	-s		skip download
	-u		unconditional update

	database directory: $IPV6CALC_DAT_DIR (default: $IPV6CALC_DAT_DIR_DEFAULT)
END
}

while getopts "ush\?" opt; do
	case $opt in
	    s)
		skip_download=1
		;;
	    u)
		update=1
		;;
	    *)
		help
		exit 1
		;;
	esac
done


## update registries
for entry in $IPV6CALC_UPDATE_LIST; do
	if [ -e "$entry" -a -x "$entry" ]; then
		update_registries="$entry"
		break
	fi
done

if [ -z "$update_registries" ]; then
	echo "ERROR : no ipv6calc registry updater found from list: $IPV6CALC_UPDATE_LIST"
	exit 1
else
	echo "INFO  : selected ipv6calc registries updater: $update_registries"
fi

## check registries
for entry in $IPV6CALC_CHECK_LIST; do
	if [ -e "$entry" -a -x "$entry" ]; then
		check_registries="$entry"
		break
	fi
done

if [ -z "$check_registries" ]; then
	echo "ERROR : no ipv6calc registry check found from list: $IPV6CALC_CHECK_LIST"
	exit 1
else
	echo "INFO  : selected ipv6calc registries check: $check_registries"
fi


## check create list for IPv4
for entry in $IPV6CALC_CREATE_IPV4_LIST; do
	if [ -e "$entry" -a -x "$entry" ]; then
		create_ipv4="$entry"
		break
	fi
done

if [ -z "$create_ipv4" ]; then
	echo "ERROR : no ipv6calc registries list creation for IPv4 found from list: $IPV6CALC_CREATE_IPV4_LIST"
	exit 1
else
	echo "INFO  : selected ipv6calc registries list creation: $create_ipv4"
fi


## check create list for IPv6
for entry in $IPV6CALC_CREATE_IPV6_LIST; do
	if [ -e "$entry" -a -x "$entry" ]; then
		create_ipv6="$entry"
		break
	fi
done

if [ -z "$create_ipv6" ]; then
	echo "ERROR : no ipv6calc registries list creation for IPv6 found from list: $IPV6CALC_CREATE_IPV6_LIST"
	exit 1
else
	echo "INFO  : selected ipv6calc registries list creation: $create_ipv6"
fi


## data directory
if [ ! -d "$IPV6CALC_DAT_DIR" ]; then
	echo "ERROR : missing directory: $IPV6CALC_DAT_DIR"
	exit 1
fi

if [ ! -w "$IPV6CALC_DAT_DIR" ]; then
	echo "ERROR : missing write permissions on directory: $IPV6CALC_DAT_DIR"
	exit 1
fi

# db subdirectory
if [ ! -e "$IPV6CALC_DAT_DIR" ]; then
	echo "NOTICE: missing db subdirectory (try to create now): $IPV6CALC_DAT_DIR"
	mkdir $IPV6CALC_DAT_DIR
	if [ $? -ne 0 ]; then
		echo "ERROR : can't create db subdirectory: $IPV6CALC_DAT_DIR ($!)"
		exit 1
	fi
elif [ ! -d "$IPV6CALC_DAT_DIR" ]; then
	echo "ERROR : existing, but not a directory: $IPV6CALC_DAT_DIR"
	exit 1
elif [ ! -w "$IPV6CALC_DAT_DIR" ]; then
	echo "ERROR : existing, but not a writable directory: $IPV6CALC_DAT_DIR"
	exit 1
fi

## update files from registries
if [ "$skip_download" != "1" ]; then
	echo "INFO  : updating files from registries now and store to: $IPV6CALC_DAT_DIR"
	$update_registries -D $IPV6CALC_DAT_DIR
	if [ $? -ne 0 ]; then
		echo "ERROR : update of files from registries resulted in a failure"
		exit 1
	fi
	echo "INFO  : update of files from registries finished in directory: $IPV6CALC_DAT_DIR"
else
	echo "NOTICE: download of files from registries skipped by option"
fi


## check files from registries
[ "$update" = "1" ] && options="$options -u"

for proto in 4 6; do
	echo "INFO  : call now check registries files for proto: IPv${proto} srcdir=$IPV6CALC_DAT_DIR dstdir=$IPV6CALC_DAT_DIR"

	create_var="create_ipv${proto}"
	$check_registries -B -${proto} -C "${!create_var}" -S "$IPV6CALC_DAT_DIR" -D "$IPV6CALC_DAT_DIR" $options
	rc=$?
	if [ $rc -ne 0 ]; then
		echo "ERROR : problem occurs"
		exit 1
	fi
done

exit 0
