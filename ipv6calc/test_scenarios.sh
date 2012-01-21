#!/bin/sh
#
# Project    : ipv6calc
# File       : test_scenarios.sh
# Version    : $Id: test_scenarios.sh,v 1.1 2012/01/10 20:50:16 peter Exp $
# Copyright  : 2001-2011 by Peter Bieringer <pb (at) bieringer.de>
#
# Test patterns for ipv6calc (functions only)

# Test Scenarios for autodetection "good case"
testscenarios_auto_good() {
cat <<END | grep -v '^#'
3ffe:831f:ce49:7601:8000:efff:af4a:86BF						ipv6addr
1.2.3.4										ipv4addr
1.2.3.4/0									ipv4addr
1.2.3.4/32									ipv4addr
01:23:45:67:89:01								mac
01-23-45-67-89-01								mac
012345678901									mac
012345-678901									mac
2002:102:304::1/0								ipv6addr
2002:102:304::1/128								ipv6addr
\\\\[x3FFEFFFF000000000000000000000001/64].IP6.ARPA.				bitstring
1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.int.	revnibbles.int
1.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.	revnibbles.arpa
0.0.0.0.0.0.0.0.0.0.0.0.0.1.0.1.f.0.0.1.0.f.f.f.f.e.f.f.3.ip6.arpa.		revnibbles.int
12345678									ipv4hex
4)+k&C#VzJ4br>0wv%Yp								base85
# RFC 5952 2.x
2001:db8:0:0:1:0:0:1								ipv6addr
2001:0db8:0:0:1:0:0:1								ipv6addr
2001:db8::1:0:0:1								ipv6addr
2001:db8::0:1:0:0:1								ipv6addr
2001:0db8::1:0:0:1								ipv6addr
2001:db8:0:0:1::1								ipv6addr
2001:db8:0000:0:1::1								ipv6addr
2001:DB8:0:0:1::1								ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:0001						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:001						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:01						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:1						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd::1							ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:0:1						ipv6addr
2001:db8:0:0:0::1								ipv6addr
2001:db8:0:0::1									ipv6addr
2001:db8:0::1									ipv6addr
2001:db8::1									ipv6addr
2001:db8::aaaa:0:0:1								ipv6addr
2001:db8:0:0:aaaa::1								ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:aaaa						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:AAAA						ipv6addr
2001:db8:aaaa:bbbb:cccc:dddd:eeee:AaAa						ipv6addr
64:ff9b::192.0.2.33								ipv6addr
fe80::1%eth0									ipv6addr
fe80--1.IPV6-LITERAL.NET							ipv6literal
2001-db8-0-0-0-0-0-1.ipv6-literal.net						ipv6literal
2001-0db8-0000-0000-0000-0000-0000-0001.ipv6-literal.net			ipv6literal
2001-db8--1.ipv6-literal.net							ipv6literal
fe80--218-8bff-fe17-a226s4.ipv6-literal.net					ipv6literal
END
}
