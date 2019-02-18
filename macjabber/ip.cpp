#include <sys/ioctl.h>
#include <net/if.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int main()
{
  struct ifreq ifra;
  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024], ip[32];
  unsigned char mac_address[6];
  struct sockaddr_in *ipv4;
  int success = 0;
  int sa;

  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) { /* handle error*/ };

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) { /* handle error */ }

  struct ifreq* it = ifc.ifc_req;
  const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

  for (; it != end; ++it) {
    strcpy(ifr.ifr_name, it->ifr_name);
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
      if (! (ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {

          printf("%-10s", ifr.ifr_name);

          sa = socket(AF_INET, SOCK_DGRAM, 0);
          ifra.ifr_addr.sa_family = AF_INET;
          strncpy(ifra.ifr_name, ifr.ifr_name, IFNAMSIZ-1);
          ioctl(sa, SIOCGIFADDR, &ifra);
          close(sa);
          printf("%s  ", inet_ntoa(((struct sockaddr_in *)&ifra.ifr_addr)->sin_addr));

          memcpy(mac_address, ifr.ifr_hwaddr.sa_data, 6);
          for (int i = 0; i < 6; i++) {
            printf("%02x", mac_address[i]);
            if (i < 5)
              printf(":");
          }
          printf("\n");
        }
      }
    } else {
      /* handle error */
    }
  }
}
