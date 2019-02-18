#!/bin/bash
#
# discover - discover MAC addresses and manufacturers
#
# Method:
# ping each IP address on the same interface as eth0, then
# get the mac address from the arp cache and compare with
# known manufacturer prefix. Report to stdout
#
# $Id: discover.sh 1284 2015-10-26 13:41:33Z bert $
#
# Version: 1.2
#
MANUFACTURERS="http://standards.ieee.org/develop/regauth/oui/oui.txt"

INTERFACE="enp4s0"

if [ "$INTERFACE" = "" ]
then
   INTERFACE="eth0"
fi

if [ "`whoami`" != "root" ]
then
   echo "Only root may use this script"
   exit 1
fi

#
# get local address
#
ADDRESS="`ifconfig $INTERFACE | grep inet | sed 's/^.*addr://' | sed 's/ .*$//'`"
NETWORK="`echo $ADDRESS | cut -d. -f1,2,3`"

echo "ADDRESS=[$ADDRESS]"
echo "NETWORK=[$NETWORK]"

NETWORK="10.10.0"

#
# exit if user hits control-c
#
crtl_c()
{
  echo -e "\nInterupted!\n"
  exit 0
}

#
# get the manufacturers list from IEEE (if we don't have them already)
#
if [ ! -f oui.txt ]
then
   echo "WARNING: oui.txt is not present, try to retrieve from IEEE"
   wget $MANUFACTURERS
fi
if [ ! -f oui.txt ]
then
   echo "Error retrieving \"${MANUFACTURERS}\""
   exit 2
fi

#
# probe the network and examine the arp cache
#
trap crtl_c SIGINT
PROBE=1
while test $PROBE -le 254
do
   IP="${NETWORK}.${PROBE}"
   if [ "$IP" != "$ADDRESS" ]
   then
      ping -c 1 -i 1 $IP >/dev/null
      if [ "$?" = "0" ]
      then
         MAC="`arp -a -i $INTERFACE | grep "($IP)" | sed 's/^.*at //' | sed 's/ .*$//'`"
         MAN="`echo $MAC | cut -d: -f1,2,3 | sed 's/:/-/g'`"
         CORP="`grep -i $MAN oui.txt | sed 's/$.*\t[A-Za-z]//' | sed 's/^.*(hex)//' | sed 's/\t//g'`"
         #HOSTNAME="`ssh $IP hostname`"
         #CHANNEL="`ssh $IP grep Output /var/content/output1.xml | sed 's/^.*Ouptut="//' | sed 's/".*$//'`"
         #echo -e "$IP\t$HOSTNAME\t$CHANNEL\t$MAC\t$CORP"
         echo -e "$IP\t$MAC\t$CORP"
      fi
   else
      #
      # myself (I'm not in the arp cache)
      #
      MAC="`ifconfig $INTERFACE | grep HWaddr | sed 's/^.*HWaddr //' | sed 's/ //g'`"
      MAN="`echo $MAC | cut -d: -f1,2,3 | sed 's/:/-/g'`"
      CORP="`grep -i $MAN oui.txt | sed 's/$.*\t[A-Za-z]//' | sed 's/^.*(hex)//' | sed 's/\t//g'`"
      echo -e "$IP\t$MAC\t$CORP"
   fi
   PROBE="`expr $PROBE + 1`"
done
