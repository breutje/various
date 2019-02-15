//
// pmac - player main mac address ($ETH0)
//
#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  struct ifreq ifra;
  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024], ip[32];
  char *eth0 = NULL;
  unsigned char mac_address[6];
  struct sockaddr_in *ipv4;
  int success = 0;
  int sa;

  if (argc > 2) {
    printf("Usage: %s [ <interface> ]\n", argv[0]);
    return 1;
  }
  if (argc == 2)
    eth0 = argv[1];        // commandline overrules environment
  else
    eth0 = getenv("ETH0"); // remains NULL if not configured

  if (strcmp(eth0, "") == 0) // set to NULL if empty
    eth0 = NULL;
#if defined(DEBUG)
  if (eth0 == NULL)
    printf("eth0: NULL\n");
  else
    printf("eth0: '%s'\n", eth0);
#endif

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) {
    printf("%s: cannot create socket\n", argv[0]);
    return 2;
  }

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
    printf("%s: cannot set SIOCGIFCONF for socket\n", argv[0]);
    return 3;
  }

  struct ifreq* it = ifc.ifc_req;
  const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

  for (; it != end; ++it) {
    strcpy(ifr.ifr_name, it->ifr_name);
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
      if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {

          if (eth0 == NULL)
            printf("interface : %s\n", ifr.ifr_name);

          sa = socket(AF_INET, SOCK_DGRAM, 0);
          ifra.ifr_addr.sa_family = AF_INET;
          strncpy(ifra.ifr_name, ifr.ifr_name, IFNAMSIZ-1);
          ioctl(sa, SIOCGIFADDR, &ifra);
          close(sa);

          if (eth0 == NULL)
            printf("address   : %s\n", inet_ntoa(((struct sockaddr_in *) &ifra.ifr_addr)->sin_addr));

          //
          // print address if eth0 = NULL or eth0 = ifr.ifr_name
          //
          if (eth0 == NULL)
            printf("mac       : ");
          if (eth0 == NULL || strcmp(eth0, ifr.ifr_name) == 0) {
            memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
            for (int i = 0; i < 6; i++) {
              printf("%02x", mac_address[i]);
              if (i < 5)
                printf(":");
            }
            printf("\n");
          }

          if (eth0 == NULL)
            printf("\n");
        }
      }
    } else {
      printf("%s: cannot set SIOCGIFFLAGS for socket\n", argv[0]);
      return 4;
    }
  }
  return 0;
}
