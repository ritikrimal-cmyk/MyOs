#ifndef UDP_H
#define UDP_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void udp_receive(uint8_t* data, uint32_t len);
int  udp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
              uint8_t* data, uint32_t len);

#endif
