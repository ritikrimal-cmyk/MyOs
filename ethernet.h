#ifndef ETHERNET_H
#define ETHERNET_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define ETHERTYPE_ARP  0x0806
#define ETHERTYPE_IP   0x0800

void eth_init();
void eth_receive(uint8_t* frame, uint32_t len);
int  eth_send(uint8_t* dst_mac, uint16_t ethertype, uint8_t* data, uint32_t len);

#endif
