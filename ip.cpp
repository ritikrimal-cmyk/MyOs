#include "ip.h"
#include "ethernet.h"
#include "arp.h"
#include "icmp.h"
#include "udp.h"
#include "vga.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static uint32_t my_ip = 0;
static uint16_t ip_id = 0;

static uint16_t checksum(uint8_t* data, uint32_t len) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < len - 1; i += 2)
        sum += (data[i] << 8) | data[i + 1];
    if (len & 1) sum += data[len - 1] << 8;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return ~sum;
}

void ip_init(uint32_t ip) {
    my_ip = ip;
}

void ip_receive(uint8_t* data, uint32_t len) {
    if (len < 20) return;

    uint8_t  protocol = data[9];
    uint8_t* payload  = data + 20;
    uint32_t plen     = len - 20;

    if      (protocol == IP_PROTO_ICMP) icmp_receive(data, payload, plen);
    else if (protocol == IP_PROTO_UDP)  udp_receive(payload, plen);
}

int ip_send(uint32_t dst_ip, uint8_t protocol, uint8_t* data, uint32_t len) {
    uint8_t pkt[1500];
    uint32_t total = 20 + len;

    // IP header
    pkt[0]  = 0x45;          // version + IHL
    pkt[1]  = 0;             // DSCP
    pkt[2]  = (total >> 8) & 0xFF;
    pkt[3]  = total & 0xFF;
    pkt[4]  = (ip_id >> 8) & 0xFF;
    pkt[5]  = ip_id & 0xFF;
    ip_id++;
    pkt[6]  = 0; pkt[7] = 0; // flags/fragment
    pkt[8]  = 64;             // TTL
    pkt[9]  = protocol;
    pkt[10] = 0; pkt[11] = 0; // checksum placeholder
    pkt[12] = (my_ip >> 24) & 0xFF;
    pkt[13] = (my_ip >> 16) & 0xFF;
    pkt[14] = (my_ip >>  8) & 0xFF;
    pkt[15] =  my_ip        & 0xFF;
    pkt[16] = (dst_ip >> 24) & 0xFF;
    pkt[17] = (dst_ip >> 16) & 0xFF;
    pkt[18] = (dst_ip >>  8) & 0xFF;
    pkt[19] =  dst_ip        & 0xFF;

    // Calculate checksum
    uint16_t csum = checksum(pkt, 20);
    pkt[10] = (csum >> 8) & 0xFF;
    pkt[11] = csum & 0xFF;

    // Copy payload
    for (uint32_t i = 0; i < len; i++) pkt[20 + i] = data[i];

    // Get destination MAC
    uint8_t* dst_mac = arp_lookup(dst_ip);
    if (!dst_mac) {
        arp_request(dst_ip);
        return -1;
    }

    return eth_send(dst_mac, 0x0800, pkt, total);
}
