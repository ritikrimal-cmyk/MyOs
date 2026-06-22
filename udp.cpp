#include "udp.h"
#include "ip.h"
#include "vga.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

void udp_receive(uint8_t* data, uint32_t len) {
    if (len < 8) return;

    uint16_t src_port = (data[0] << 8) | data[1];
    uint16_t dst_port = (data[2] << 8) | data[3];
    uint16_t udp_len  = (data[4] << 8) | data[5];
    uint8_t* payload  = data + 8;
    uint32_t plen     = udp_len - 8;

    vga_print("UDP packet from port ", COLOR_LIGHT_CYAN);
    char buf[8];
    buf[0] = '0' + (src_port / 10000) % 10;
    buf[1] = '0' + (src_port / 1000)  % 10;
    buf[2] = '0' + (src_port / 100)   % 10;
    buf[3] = '0' + (src_port / 10)    % 10;
    buf[4] = '0' + (src_port)         % 10;
    buf[5] = '\0';
    vga_println(buf, COLOR_WHITE);

    // Null terminate payload and print
    if (plen < 256) {
        char msg[256];
        for (uint32_t i = 0; i < plen; i++) msg[i] = payload[i];
        msg[plen] = '\0';
        vga_print("  Data: ", COLOR_LIGHT_GREY);
        vga_println(msg, COLOR_WHITE);
    }
}

int udp_send(uint32_t dst_ip, uint16_t src_port, uint16_t dst_port,
             uint8_t* data, uint32_t len) {
    uint32_t udp_len = 8 + len;
    uint8_t  pkt[1500];

    pkt[0] = (src_port >> 8) & 0xFF;
    pkt[1] = src_port & 0xFF;
    pkt[2] = (dst_port >> 8) & 0xFF;
    pkt[3] = dst_port & 0xFF;
    pkt[4] = (udp_len >> 8) & 0xFF;
    pkt[5] = udp_len & 0xFF;
    pkt[6] = 0; pkt[7] = 0; // checksum (optional for UDP)

    for (uint32_t i = 0; i < len; i++) pkt[8 + i] = data[i];

    return ip_send(dst_ip, IP_PROTO_UDP, pkt, udp_len);
}
