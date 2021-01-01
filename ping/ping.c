#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netinet/ip_icmp.h>

struct ICMPMessage {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequense;
};

int checksum(void* buf, size_t size) {
  // https://tools.ietf.org/html/rfc1071
  uint8_t* p = buf;
  uint32_t sum = 0;
  size_t start = 0;
  for (size_t i = start; i < size; i += 2) {
    sum += ((uint16_t)p[i + 0]) << 8 | p[i + 1];
  }
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  sum = ~sum;
  return ((sum >> 8) & 0xFF) | ((sum & 0xFF) << 8);

}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: ping IP_ADDR\n");
        return 1;
    }

    int target_addr = inet_addr(argv[1]);
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);

    if (sock < 0) {
        perror("socket() failed\n");
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = target_addr;

    // Send
    struct ICMPMessage icmp;
    memset(&icmp, 0, sizeof(icmp));
    icmp.type = ICMP_ECHO;
    icmp.checksum = checksum(&icmp, sizeof(icmp));

    int n = sendto(
            sock, &icmp, sizeof(icmp),
            0, (struct sockaddr*)&addr, sizeof(addr));

    if (n < 1) {
        perror("sendto () faild\n");
    }

    uint8_t recv_buf[256];
    socklen_t addr_size;
    int recv_len = recvfrom(sock, &recv_buf, sizeof(recv_buf), 0,
            (struct sockaddr*)&addr, &addr_size);

    if (recv_len < 1) {
        perror("recvfrom() faild\n");
    }

    printf("recvfrom returned: %i\n", recv_len);

    for (int i = 0; i < recv_len; i++) {
        printf("%02X", recv_buf[i]);
        printf((i & 0xF) == 0xF ? "\n" : " ");
    }
    printf("\n");

    struct ICMPMessage* recv_icmp = (struct ICMPMessage*)(recv_buf);

    printf("ICMP packet recieved from %s\n", inet_ntoa(addr.sin_addr));
    printf("ICMP Type: %i\n", recv_icmp->type);
    printf("ICMP Code: %i\n", recv_icmp->code);
    printf("ICMP Checksum %i\n", recv_icmp->checksum);

    close(sock);
    return 0;
}
