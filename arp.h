#ifndef ARP_H
#define ARP_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void     arp_init(uint32_t ip);
void     arp_receive(uint8_t* data, uint32_t len);
uint8_t* arp_lookup(uint32_t ip);
void     arp_request(uint32_t ip);

#endif
