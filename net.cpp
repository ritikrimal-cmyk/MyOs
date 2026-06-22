#include "net.h"
#include "ethernet.h"
#include "vga.h"
#include "memory.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

// RTL8139 I/O ports (base address from PCI)
#define RTL_BASE     0xC000
#define RTL_MAC0     (RTL_BASE + 0x00)
#define RTL_MAR0     (RTL_BASE + 0x08)
#define RTL_TXSTATUS (RTL_BASE + 0x10)
#define RTL_TXADDR   (RTL_BASE + 0x20)
#define RTL_RXBUF    (RTL_BASE + 0x30)
#define RTL_CMD      (RTL_BASE + 0x37)
#define RTL_CAPR     (RTL_BASE + 0x38)
#define RTL_IMR      (RTL_BASE + 0x3C)
#define RTL_ISR      (RTL_BASE + 0x3E)
#define RTL_TCR      (RTL_BASE + 0x40)
#define RTL_RCR      (RTL_BASE + 0x44)
#define RTL_CONFIG1  (RTL_BASE + 0x52)

static uint8_t  mac[ETH_ALEN];
static uint8_t  rx_buf[8192 + 16];
static uint8_t  tx_buf[ETH_MTU + 14];
static uint32_t tx_slot = 0;
static uint16_t rx_offset = 0;

// Port I/O helpers
static void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" :: "a"(val), "Nd"(port));
}
static void outw(uint16_t port, uint16_t val) {
    asm volatile("outw %0, %1" :: "a"(val), "Nd"(port));
}
static void outl(uint16_t port, uint32_t val) {
    asm volatile("outl %0, %1" :: "a"(val), "Nd"(port));
}
static uint8_t inb(uint16_t port) {
    uint8_t val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}
static uint16_t inw(uint16_t port) {
    uint16_t val;
    asm volatile("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}
static uint32_t inl(uint16_t port) {
    uint32_t val;
    asm volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

void net_init() {
    // Power on
    outb(RTL_CONFIG1, 0x00);

    // Software reset
    outb(RTL_CMD, 0x10);
    while (inb(RTL_CMD) & 0x10);

    // Read MAC address
    for (int i = 0; i < ETH_ALEN; i++)
        mac[i] = inb(RTL_MAC0 + i);

    // Set RX buffer
    outl(RTL_RXBUF, (uint32_t)rx_buf);

    // Enable TX and RX
    outb(RTL_CMD, 0x0C);

    // Configure RX — accept all packets
    outl(RTL_RCR, 0x0000000F);

    // Configure TX
    outl(RTL_TCR, 0x00000600);

    // Enable interrupts
    outw(RTL_IMR, 0x0005);

    vga_print("  MAC: ", COLOR_LIGHT_GREY);
    for (int i = 0; i < ETH_ALEN; i++) {
        char buf[3];
        buf[0] = "0123456789ABCDEF"[mac[i] >> 4];
        buf[1] = "0123456789ABCDEF"[mac[i] & 0xF];
        buf[2] = '\0';
        vga_print(buf, COLOR_WHITE);
        if (i < ETH_ALEN - 1) vga_print(":", COLOR_LIGHT_GREY);
    }
    vga_println("", COLOR_WHITE);
}

int net_send(uint8_t* dst_mac, uint16_t ethertype, uint8_t* data, uint32_t len) {
    // Build ethernet frame
    for (int i = 0; i < ETH_ALEN; i++) tx_buf[i]            = dst_mac[i];
    for (int i = 0; i < ETH_ALEN; i++) tx_buf[ETH_ALEN + i] = mac[i];
    tx_buf[12] = (ethertype >> 8) & 0xFF;
    tx_buf[13] = ethertype & 0xFF;
    for (uint32_t i = 0; i < len; i++) tx_buf[14 + i] = data[i];

    uint32_t frame_len = 14 + len;

    // Write to TX buffer
    outl(RTL_TXADDR + tx_slot * 4, (uint32_t)tx_buf);
    outl(RTL_TXSTATUS + tx_slot * 4, frame_len);

    tx_slot = (tx_slot + 1) % 4;
    return 0;
}

void net_poll() {
    uint16_t isr = inw(RTL_ISR);
    if (!(isr & 0x01)) return; // no packet received

    // Clear interrupt
    outw(RTL_ISR, 0x01);

    // Read packet header
    uint16_t* hdr = (uint16_t*)(rx_buf + rx_offset);
    uint16_t  pkt_len = hdr[1];

    if (pkt_len > 0 && pkt_len < ETH_MTU + 14) {
        uint8_t* pkt = (uint8_t*)(rx_buf + rx_offset + 4);
        // Pass to ethernet layer
        eth_receive(pkt, pkt_len);
    }

    // Advance RX offset
    rx_offset = (rx_offset + pkt_len + 4 + 3) & ~3;
    outw(RTL_CAPR, rx_offset - 16);
}

uint8_t* net_get_mac() {
    return mac;
}
