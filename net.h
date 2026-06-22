#ifndef NET_H
#define NET_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define ETH_ALEN      6    // MAC address length
#define ETH_MTU       1500 // Max packet size

struct EthFrame {
    uint8_t  dst_mac[ETH_ALEN];
    uint8_t  src_mac[ETH_ALEN];
    uint16_t ethertype;
    uint8_t  data[ETH_MTU];
    uint32_t data_len;
};

void     net_init();
int      net_send(uint8_t* dst_mac, uint16_t ethertype, uint8_t* data, uint32_t len);
void     net_poll();
uint8_t* net_get_mac();

#endif
