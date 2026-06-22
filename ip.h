#ifndef IP_H
#define IP_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void ip_init(uint32_t ip);
void ip_receive(uint8_t* data, uint32_t len);
int  ip_send(uint32_t dst_ip, uint8_t protocol, uint8_t* data, uint32_t len);

#define IP_PROTO_ICMP 1
#define IP_PROTO_UDP  17

#endif
