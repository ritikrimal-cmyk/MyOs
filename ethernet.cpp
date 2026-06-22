#include "ethernet.h"
#include "net.h"
#include "arp.h"
#include "ip.h"
#include "vga.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void eth_init() {
    net_init();
}

void eth_receive(uint8_t* frame, uint32_t len) {
    if (len < 14) return;

    uint16_t ethertype = (frame[12] << 8) | frame[13];
    uint8_t* payload   = frame + 14;
    uint32_t plen      = len - 14;

    if      (ethertype == ETHERTYPE_ARP) arp_receive(payload, plen);
    else if (ethertype == ETHERTYPE_IP)  ip_receive(payload, plen);
}

int eth_send(uint8_t* dst_mac, uint16_t ethertype, uint8_t* data, uint32_t len) {
    return net_send(dst_mac, ethertype, data, len);
}
