# Drop TCP SYN
iptables -A BLOCK -p tcp --tcp-flags SYN,ACK,FIN,RST SYN -j DROP

# Drop TCP FIN
iptables -A BLOCK -p tcp --tcp-flags SYN,ACK,FIN,RST FIN -j DROP

# Drop specified ICMP packets
iptables -A OUTPUT -p icmp -o eth0 -j ACCEPT
iptables -A INPUT -p icmp --icmp-type echo-reply -s 0/0 -i eth0 -j ACCEPT
iptables -A INPUT -p icmp --icmp-type destination-unreachable -s 0/0 -i eth0 -j ACCEPT
iptables -A INPUT -p icmp --icmp-type time-exceeded -s 0/0 -i eth0 -j ACCEPT
iptables -A INPUT -p icmp -i eth0 -j DROP

# Limit packet traffic on a TCP or UDP port:
iptables -A INPUT -p $proto --destination-port $port --syn -m state --state NEW -m limit --limit $lim/s --limit-burst $lb -j ACCEPT

# Limit established/related packet traffic:
iptables -A INPUT -m state --state RELATED,ESTABLISHED -m limit --limit $lim/s --limit-burst $lb -j ACCEPT




