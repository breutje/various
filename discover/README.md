# discover
discover ethernet mac addresses on IPv4 network.

It also prints the manufacturers name for each interface.
Please note that discovery may take more than 4 minutes.
We assume eth0 is the interface connected to the network
to be discovered. If this is different, please change the
INTERFACE variable in the script. Or start with:

```
INTERFACE=eth2 ./discover.sh
```

The script should be run by 'root'. Also, a C-Class IPv4
network is assumed, but the script may be easily modified
for other networks.

# Background
The script gets the IP (IPv4) address from the interface
and pings each IP address in the same C-Class network.
This causes an entry in the arp cache (if the ping is succesful).
The MAC address is then retrieved and checked in the IEEE
database for the manufacturer. If oui.txt is present in the
same directory, the script will not try to retrieve the file
(if the file is too old and you have an internet connection,
just delete the file: the script will retrieve an fresh copy).

NOTE: this script fails if the host has more than one IP address.

Author: Bert Reuling

