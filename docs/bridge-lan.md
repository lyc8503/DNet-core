### Bridge DNet to LAN

1. make sure the IP addr of the bridge is in the same subnet as the LAN
2. create bridge and add two interfaces to it
   ```bash
   brctl addbr br0
   brctl addif br0 dnet0
   brctl addif br0 eth0
   ```
3. set ip and promiscuous mode
   ```bash
   ifconfig dnet0 0.0.0.0 promisc up
   ifconfig eth0 0.0.0.0 promisc up
   ifconfig br0 192.168.1.13 netmask 255.255.255.0 # your original config for eth0
   ```
4. more config
   ```bash
   echo 0 > /proc/sys/net/bridge/bridge-nf-call-iptables
   brctl setageing br0 0 # https://serverfault.com/questions/878353/bridge-doesnt-forward-all-traffic-ubuntu-xenial
   echo 1 > /proc/sys/net/ipv4/ip_forward # might not needed
   ```

some reference:
- https://openvpn.net/community-resources/ethernet-bridging/#linuxscript
- https://yuerblog.cc/2017/01/22/understand-bridge-and-dhcp/