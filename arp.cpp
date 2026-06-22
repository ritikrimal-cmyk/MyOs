#include "arp.h"
#include "ethernet.h"
#include "net.h"
#include "vga.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define ARP_TABLE_SIZE 16

struct ArpEntry {
    uint32_t ip;
    uint8_t  mac[6];
    uint8_t  valid;
};

static ArpEntry arp_table[ARP_TABLE_SIZE];
static uint32_t my_ip = 0;
static uint8_t  broadcast_mac[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

static uint16_t htons(uint16_t val) {
    return (val >> 8) | (val << 8);
}
static uint32_t htonl(uint32_t val) {
    return ((val & 0xFF) << 24) | (((val>>8) & 0xFF) << 16) |
           (((val>>16) & 0xFF) << 8) | ((val>>24) & 0xFF);
}

void arp_init(uint32_t ip) {
    my_ip = ip;
    for (int i = 0; i < ARP_TABLE_SIZE; i++)
        arp_table[i].valid = 0;
}

void arp_receive(uint8_t* data, uint32_t len) {
    if (len < 28) return;

    uint16_t opcode = (data[6] << 8) | data[7];
    uint32_t sender_ip;
    uint8_t  sender_mac[6];

    for (int i = 0; i < 6; i++) sender_mac[i] = data[8 + i];
    sender_ip = (data[14] << 24) | (data[15] << 16) | (data[16] << 8) | data[17];

    // Cache sender in ARP table
    for (int i = 0; i < ARP_TABLE_SIZE; i++) {
        if (!arp_table[i].valid || arp_table[i].ip == sender_ip) {
            arp_table[i].ip    = sender_ip;
            arp_table[i].valid = 1;
            for (int j = 0; j < 6; j++) arp_table[i].mac[j] = sender_mac[j];
            break;
        }
    }

    // ARP reply if someone is asking for our IP
    if (opcode == 1) {
        uint32_t target_ip = (data[24] << 24) | (data[25] << 16) | (data[26] << 8) | data[27];
        if (target_ip == my_ip) {
            uint8_t reply[28];
            // Hardware type: Ethernet
            reply[0] = 0x00; reply[1] = 0x01;
            // Protocol type: IP
            reply[2] = 0x08; reply[3] = 0x00;
            // HW size, proto size
            reply[4] = 6; reply[5] = 4;
            // Opcode: reply
            reply[6] = 0x00; reply[7] = 0x02;
            // Sender MAC and IP (us)
            uint8_t* our_mac = net_get_mac();
            for (int i = 0; i < 6; i++) reply[8 + i]  = our_mac[i];
            reply[14] = (my_ip >> 24) & 0xFF;
            reply[15] = (my_ip >> 16) & 0xFF;
            reply[16] = (my_ip >>  8) & 0xFF;
            reply[17] =  my_ip        & 0xFF;
            // Target MAC and IP
            for (int i = 0; i < 6; i++) reply[18 + i] = sender_mac[i];
            reply[24] = (sender_ip >> 24) & 0xFF;
            reply[25] = (sender_ip >> 16) & 0xFF;
            reply[26] = (sender_ip >>  8) & 0xFF;
            reply[27] =  sender_ip        & 0xFF;

            eth_send(sender_mac, 0x0806, reply, 28);
        }
    }
}

uint8_t* arp_lookup(uint32_t ip) {
    for (int i = 0; i < ARP_TABLE_SIZE; i++)
        if (arp_table[i].valid && arp_table[i].ip == ip)
            return arp_table[i].mac;
    return 0;
}

void arp_request(uint32_t ip) {
    uint8_t pkt[28];
    pkt[0] = 0x00; pkt[1] = 0x01;
    pkt[2] = 0x08; pkt[3] = 0x00;
    pkt[4] = 6;    pkt[5] = 4;
    pkt[6] = 0x00; pkt[7] = 0x01;
    uint8_t* our_mac = net_get_mac();
    for (int i = 0; i < 6; i++) pkt[8 + i]  = our_mac[i];
    pkt[14] = (my_ip >> 24) & 0xFF;
    pkt[15] = (my_ip >> 16) & 0xFF;
    pkt[16] = (my_ip >>  8) & 0xFF;
    pkt[17] =  my_ip        & 0xFF;
    for (int i = 0; i < 6; i++) pkt[18 + i] = 0x00;
    pkt[24] = (ip >> 24) & 0xFF;
    pkt[25] = (ip >> 16) & 0xFF;
    pkt[26] = (ip >>  8) & 0xFF;
    pkt[27] =  ip        & 0xFF;
    eth_send(broadcast_mac, 0x0806, pkt, 28);
}
