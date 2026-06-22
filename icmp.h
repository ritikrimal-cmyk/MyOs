#ifndef ICMP_H
#define ICMP_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void icmp_receive(uint8_t* ip_hdr, uint8_t* data, uint32_t len);
void icmp_ping(uint32_t dst_ip);

#endif
