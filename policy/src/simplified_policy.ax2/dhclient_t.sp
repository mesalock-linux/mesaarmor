{
domain dhclient_t;
program /sbin/dhclient;
include common-relaxed.sp;
include daemon.sp;
include nameservice.sp;


#Writable files
allow /var/lib/dhcp/** r,w,s;
allow /etc/resolv.conf r,o,s;
allowtmp -dir /etc -name auto r,w,s;

#Executable
allow /etc/rc.d/init.d/** x,r,s;
allow /etc/sysconfig/network-scripts/** r,x,s;
allow /usr/bin/**   x,r,s;
allow /sbin/**   x,r,s;
allow /bin/** r,x,s;
allow /sbin/restorecon r,dx,s;

#Readable
allow /var/lock/subsys/** s,r;
allow /etc/sysconfig/** r,s;
allow /etc/selinux/config  s,r;
allow /etc/initlog.conf s,r;
allow etc_runtime_t s,r;

allownet -protocol udp,tcp -port 68 server;
allownet -protocol udp,tcp -port 67 client;

allowpriv getsecurity;
allowpriv netlink;
allowpriv cap_net_admin;
allowpriv cap_dac_read_search;
#Add by seedit-generator
allownet -protocol raw use;



}
