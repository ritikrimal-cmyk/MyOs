#include "icmp.h"
#include "ip.h"
#include "vga.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static uint16_t checksum(uint8_t* data, uint32_t len) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < len - 1; i += 2)
        sum += (data[i] << 8) | data[i + 1];
    if (len & 1) sum += data[len - 1] << 8;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return ~sum;
}

void icmp_receive(uint8_t* ip_hdr, uint8_t* data, uint32_t len) {
    if (len < 8) return;

    uint8_t type = data[0];

    // Echo request — send reply
    if (type == 8) {
        uint32_t src_ip = (ip_hdr[12] << 24) | (ip_hdr[13] << 16) |
                          (ip_hdr[14] <<  8) |  ip_hdr[15];

        uint8_t reply[len];
        reply[0] = 0;    // type: echo reply
        reply[1] = 0;    // code
        reply[2] = 0; reply[3] = 0; // checksum placeholder
        for (uint32_t i = 4; i < len; i++) reply[i] = data[i];

        uint16_t csum = checksum(reply, len);
        reply[2] = (csum >> 8) & 0xFF;
        reply[3] = csum & 0xFF;

        ip_send(src_ip, IP_PROTO_ICMP, reply, len);
        vga_println("  ICMP: echo reply sent", COLOR_LIGHT_GREY);
    }

    // Echo reply
    if (type == 0) {
        vga_println("Ping reply received!", COLOR_LIGHT_GREEN);
    }
}

void icmp_ping(uint32_t dst_ip) {
    uint8_t pkt[16];
    pkt[0] = 8;    // type: echo request
    pkt[1] = 0;    // code
    pkt[2] = 0; pkt[3] = 0; // checksum
    pkt[4] = 0; pkt[5] = 1; // identifier
    pkt[6] = 0; pkt[7] = 1; // sequence
    for (int i = 8; i < 16; i++) pkt[i] = i; // payload

    uint16_t csum = checksum(pkt, 16);
    pkt[2] = (csum >> 8) & 0xFF;
    pkt[3] = csum & 0xFF;

    ip_send(dst_ip, IP_PROTO_ICMP, pkt, 16);
    vga_println("Ping sent...", COLOR_LIGHT_CYAN);
}
