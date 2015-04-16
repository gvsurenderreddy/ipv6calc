# Project    : ipv6calc
# File       : contrib/ipv6calc.spec
# Copyright  : 2001-2015 by Peter Bieringer <pb@bieringer.de>
# $Id: ipv6calc.spec,v 1.239 2015/04/16 06:23:20 ds6peter Exp $

Summary:	IPv6 address format change and calculation utility
Name:		ipv6calc
Version:	0.99.0
Release: 	9%{?dist}
Group:		Applications/Text
URL:		http://www.deepspace6.net/projects/%{name}.html
License:	GPLv2
Source:		ftp://ftp.bieringer.de/pub/linux/IPv6/ipv6calc/%{name}-%{version}.tar.gz
BuildRequires:	openssl-devel
BuildRequires:	perl(Digest::MD5), perl(Digest::SHA1), perl(URI::Escape)
BuildRequires:	perl(strict), perl(warnings)
Requires:	perl(:MODULE_COMPAT_%(eval "`%{__perl} -V:version`"; echo $version))

BuildRoot:	%{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)


# database support (deselectable)
%if "%{?_without_ip2location:0}%{?!_without_ip2location:1}" == "1"
%define enable_ip2location 1
%endif

%if "%{?_without_geoip:0}%{?!_without_geoip:1}" == "1"
%define enable_geoip 1
%endif

%if "%{?_without_dbip:0}%{?!_without_dbip:1}" == "1"
%define enable_dbip 1
%endif

%if "%{?_without_external:0}%{?!_without_external:1}" == "1"
%define enable_external 1
%endif

%if "%{?_without_shared:0}%{?!_without_shared:1}" == "1"
%define enable_shared 1
%endif


# database locations
%define ip2location_db %{_datadir}/IP2Location
%define geoip_db       %{_datadir}/GeoIP
%define dbip_db        %{_datadir}/DBIP
%define external_db    %{_datadir}/%{name}/db


# Berkeley DB selector
%define require_db4 %(echo "%{dist}" | egrep -q '^\.el(5|6)$' && echo 1 || echo 0)
%if %{require_db4}
BuildRequires:	db4-devel
%else
BuildRequires:	libdb-devel
%endif


%description
ipv6calc is a small utility which formats and calculates IPv4/IPv6 addresses
in different ways.

Install this package, if you want to retrieve information about a particular
IPv4/IPv6/MAC address (-i ADDRESS) or make life easier in adding entries to
reverse IPv6 DNS zones (e.g. -a 2001:db8:1234::1/48).

In addition many format and type conversions are supported, see online help
and/or given URL for more.

Also this package contains additional programs
 - ipv6loganon: anonymize Apache web server logs
 - ipv6logconv: special Apache web server log converter
    (examples included for use with analog)
 - ipv6logstats: create statistics from list of IPv4/IPv6 addresses
    (examples included for use with gnu-plot)

Support for following databases
 - IP2Location	%{?enable_ip2location:ENABLED}%{?!enable_ip2location:DISABLED}
		default directory for downloaded db files: %{ip2location_db}
		(requires also external library on system)

 - GeoIP	%{?enable_geoip:ENABLED}%{?!enable_geoip:DISABLED}
		default directory for downloaded db files: %{geoip_db}
		(requires also external library on system)

 - db-ip.com 	%{?enable_dbip:ENABLED}%{?!enable_dbip:DISABLED}
		(once generated database files are found on system)
		default directory for generated db files: %{dbip_db}

 - External	%{?enable_external:ENABLED}%{?!enable_external:DISABLED}
		default directory for generated db files: %{external_db}

Built %{?enable_shared:WITH}%{?!enable_shared:WITHOUT} shared-library

Available rpmbuild rebuild options:
  --without ip2location
  --without geoip
  --without dbip
  --without external
  --without shared


%package ipv6calcweb
Summary: 	IP address information web utility
Group:		Applications/Internet
Requires:	ipv6calc httpd
Requires:	perl(URI) perl(Digest::SHA1) perl(Digest::MD5) perl(HTML::Entities)
BuildRequires:	perl(URI) perl(Digest::SHA1) perl(Digest::MD5) perl(HTML::Entities)

%description ipv6calcweb
ipv6calcweb contains a CGI program and a configuration file for
displaying information of IP addresses on a web page using ipv6calc.

Check/addjust %{_sysconfdir}/httpd/conf.d/ipv6calcweb.conf
Default restricts access to localhost


%prep
%setup -q
%configure \
	%{?enable_ip2location:--enable-ip2location} \
	%{?enable_ip2location:--with-ip2location-dynamic --with-ip2location-headers-fallback} \
	--with-ip2location-db=%{ip2location_db} \
	%{?enable_geoip:--enable-geoip} \
	%{?enable_geoip:--with-geoip-dynamic --with-geoip-headers-fallback} \
	--with-geoip-db=%{geoip_db} \
	%{?enable_dbip:--enable-dbip} \
	--with-dbip-db=%{dbip_db} \
	%{?enable_external:--enable-external} \
	--with-external-db=%{external_db} \
	%{?enable_shared:--enable-shared}


%build
make clean
make %{?_smp_mflags} COPTS="$RPM_OPT_FLAGS"


%install
rm -rf %{buildroot}
make install DESTDIR=%{buildroot}

## Install examples and helper files
install -d -p %{buildroot}%{_docdir}/%{name}-%{version}/

# tools
install -d %{buildroot}%{_datadir}/%{name}/tools
cat <<END | while read tool; do install -m 755 tools/$tool %{buildroot}%{_datadir}/%{name}/tools; done
GeoIP-update.sh
IP2Location-update.sh
DBIP-update.sh
DBIP-generate-db.pl
ipv6calc-create-registry-list-ipv4.pl
ipv6calc-create-registry-list-ipv6.pl
ipv6calc-db-update.sh
ipv6calc-registries-check-run-create.sh
ipv6calc-update-registries.sh
END

## examples
install -d %{buildroot}%{_datadir}/%{name}/examples/

# ipv6logconv
install -d %{buildroot}%{_datadir}/%{name}/examples/ipv6logconv
for file in examples/analog/*.{cfg,txt,tab,sh}; do
	install $file %{buildroot}%{_datadir}/%{name}/examples/ipv6logconv/
done

# ipv6loganon
install -d %{buildroot}%{_datadir}/%{name}/examples/ipv6loganon
for file in ipv6loganon/README; do
	install $file %{buildroot}%{_datadir}/%{name}/examples/ipv6loganon/
done

# ipv6logstats
install -d %{buildroot}%{_datadir}/%{name}/examples/ipv6logstats
for file in ipv6logstats/README ipv6logstats/example_* ipv6logstats/collect_ipv6logstats.pl; do
	install $file %{buildroot}%{_datadir}/%{name}/examples/ipv6logstats/
done


# db directory
install -d %{buildroot}%{external_db}


# ipv6calcweb
install -d %{buildroot}%{_sysconfdir}/httpd/conf.d
install -d %{buildroot}%{_localstatedir}/www/cgi-bin

install ipv6calcweb/ipv6calcweb.conf %{buildroot}%{_sysconfdir}/httpd/conf.d
install -m 555 ipv6calcweb/ipv6calcweb.cgi  %{buildroot}%{_localstatedir}/www/cgi-bin


%clean
rm -rf %{buildroot}


%check
%ifnarch ppc64
	make test
%endif


%files
%doc ChangeLog README CREDITS TODO COPYING LICENSE USAGE doc/ipv6calc.{lyx,sgml,html,xml}

%defattr(644,root,root,755)

# binaries
%attr(755,-,-) %{_bindir}/*

# man pages
%{_mandir}/man8/*

# tools
%attr(755,-,-) %{_datadir}/%{name}/tools/*

# shared library
%{?enable_shared:%attr(755,-,-) %{_libdir}/*}

# database directory
%{external_db}

# examples
%{_datadir}/%{name}/examples/*


%files ipv6calcweb
%defattr(644,root,root,755)

%doc ipv6calcweb/README ipv6calcweb/USAGE

%attr(555,-,-) %{_localstatedir}/www/cgi-bin/ipv6calcweb.cgi
%config(noreplace) %{_sysconfdir}/httpd/conf.d/ipv6calcweb.conf


%changelog
* Sun Mar 01 2015 Peter Bieringer <pb@bieringer.de>
- remove not necessary x-bits for some files by proper definition in %files section

* Wed Feb 18 2015 Peter Bieringer <pb@bieringer.de>
- delete subpackage db, major spec file alignment with Fedora-SCM version

* Sun Feb 15 2015 Peter Bieringer <pb@bieringer.de>
- extend sub-package db

* Tue Dec 09 2014 Peter Bieringer <pb@bieringer.de>
- create new sub-package: db with all related tools
- configure: --enable-external

* Sat Oct 25 2014 Peter Bieringer <pb@bieringer.de>
- add /usr/share/ipv6calc/db directory

* Fri Oct 10 2014 Peter Bieringer <pb@bieringer.de>
- add DBIP-update.sh DBIP-generate-db.pl

* Mon Jul 14 2014 Peter Bieringer <pb@bieringer.de>
- review description

* Sun Feb 02 2014 Peter Bieringer <pb@bieringer.de>
- add support for optional shared-library build

* Tue Nov 12 2013 Peter Bieringer <pb@bieringer.de>
- add datadir with tools

* Thu Sep 26 2013 Peter Bieringer <pb@bieringer.de>
- add support for --with geoip-dyn and --with ip2location-dyn

* Wed Sep 12 2012 Peter Bieringer <pb@bieringer.de>
- change requirements for ip2location to Mandrake Linux compatible names
- change location of "ipv6calc" from bin to %{_bindir}
- install tools/GeoIP-update.sh tools/IP2Location-update.sh to doc dir

* Sun Jan 01 2012 Peter Bieringer <pb@bieringer.de>
- create subpackage for ipv6calcweb
- minor review

* Sat Oct 08 2011 Peter Bieringer <pb@bieringer.de>
- reduce "make test" to "make test-minimal"

* Wed Nov 19 2008 Peter Bieringer <pb@bieringer.de>
- switch from "make installonly" to "make install"

* Wed Feb 14 2007 Peter Bieringer <pb@bieringer.de>
- remove support for build option --with-geoip-system
- add support for 'ipv6loganon'

* Sat Oct 28 2006 Peter Bieringer <pb@bieringer.de>
- add support for build option --with-geoip-system

* Sun Aug 06 2006 Peter Bieringer <pb@bieringer.de>
- add support for build option --with-geoip

* Wed Jun 07 2006 Peter Bieringer <pb@bieringer.de>
- add support for build option --with-ip2location

* Wed Jul 20 2005 Peter Bieringer <pb@bieringer.de>
- adjust code because of use of "bindir" in Makefile of ivp6calc

* Sat Oct 30 2004 Peter Bieringer <pb@bieringer.de>
- remove openssl(-devel) from requirements, no longer needed

* Sat Nov 22 2003 Peter Bieringer <pb@bieringer.de>
- adjustments

* Fri Nov 21 2003 Peter Bieringer <pb@bieringer.de>
- add ipv6logstats
- add man pages
- add configure options

* Mon Nov 11 2002 Peter Bieringer <pb@bieringer.de>
- change IPv6 address in %description

* Sat Apr 20 2002 Peter Bieringer <pb@bieringer.de>
- Change URL

* Sun Apr 07 2002 Peter Bieringer <pb@bieringer.de>
- add more analog example files

* Sat Apr 06 2002 Peter Bieringer <pb@bieringer.de>
- remove BuildRequires extension, not needed for normal build

* Sun Mar 24 2002 Peter Bieringer <pb@bieringer.de>
- extend BuildRequires for perl /usr/bin/aggregate wget

* Mon Mar 18 2002 Peter Bieringer <pb@bieringer.de>
- add ipv6calcweb.cgi

* Sat Mar 16 2002 Peter Bieringer <pb@bieringer.de>
- add ipv6logconv, analog examples

* Mon Mar 11 2002 Peter Bieringer <pb@bieringer.de>
- Add perl to buildrequire and openssl to require

* Mon Jan 21 2002 Peter Bieringer <pb@bieringer.de>
- Add LICENSE + COPYING file

* Thu Dec 27 2001 Peter Bieringer <pb@bieringer.de>
- Add comment header
- Add call to configure on build

* Tue Dec 18 2001 Peter Bieringer <pb@bieringer.de>
- Replace hardwired version number with autoconf/configure variable

* Wed Apr 25 2001 Peter Bieringer <pb@bieringer.de>
- Fix permissions of doc files

* Thu Mar 15 2001 Peter Bieringer <pb@bieringer.de>
- Add doc directory also to %files to make sure the directory will be removed
   on update or deinstall
- change install permissions for entries in doc directory
- change "make install" to "make installonly" (make test should be only executed once)" 

* Wed Mar 14 2001 Peter Bieringer <pb@bieringer.de>
- Add "make clean" and "make test" on %build

* Tue Mar 13 2001 Peter Bieringer <pb@bieringer.de>
- add CREDITS and TODO for install

* Sat Mar 10 2001 Peter Bieringer <pb@bieringer.de>
- enable "URL"

* Sun Mar 04 2001 Peter Bieringer <pb@bieringer.de>
- change install location to /bin

* Tue Feb 27 2001 Peter Bieringer <pb@bieringer.de>
- review for new release, now named "ipv6calc"
- review install section for RedHat 7.0.91

* Sun Feb 25 2001 Peter Bieringer <pb@bieringer.de>
- initial build
