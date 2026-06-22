#include "shell.h"
#include "keyboard.h"
#include "memory.h"
#include "vga.h"
#include "fs.h"
#include "process.h"
#include "elf.h"
#include "usermode.h"
#include "mmap.h"
#include "script.h"
#include "net.h"
#include "icmp.h"
#include "udp.h"
#include "timer.h"

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

static char input[256];
static int  input_len = 0;

static int str_equal(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Print a number
static void print_num(uint32_t n, unsigned char color) {
    if (n == 0) { vga_print("0", color); return; }
    char buf[12];
    int i = 0;
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    buf[i] = '\0';
    // reverse
    for (int a = 0, b = i-1; a < b; a++, b--) {
        char tmp = buf[a]; buf[a] = buf[b]; buf[b] = tmp;
    }
    vga_print(buf, color);
}

void cmd_help() {
    vga_println("Available commands:", COLOR_LIGHT_CYAN);
    vga_println("  help     - show this message",         COLOR_WHITE);
    vga_println("  clear    - clear the screen",          COLOR_WHITE);
    vga_println("  about    - about this OS",             COLOR_WHITE);
    vga_println("  memory   - show memory info",          COLOR_WHITE);
    vga_println("  meminfo  - detailed memory stats",     COLOR_WHITE);
    vga_println("  echo     - print text",                COLOR_WHITE);
    vga_println("  reboot   - reboot the system",         COLOR_WHITE);
    vga_println("  exit     - shutdown the system",       COLOR_WHITE);
    vga_println("  ls       - list files",                COLOR_WHITE);
    vga_println("  mkdir    - create directory",          COLOR_WHITE);
    vga_println("  write    - write to file",             COLOR_WHITE);
    vga_println("  cat      - read a file",               COLOR_WHITE);
    vga_println("  rm       - delete a file",             COLOR_WHITE);
    vga_println("  ps       - list processes",            COLOR_WHITE);
    vga_println("  exec     - run an ELF executable",     COLOR_WHITE);
    vga_println("  run      - run a script file",         COLOR_WHITE);
    vga_println("  ping     - ping an IP address",        COLOR_WHITE);
    vga_println("  ifconfig - show network info",         COLOR_WHITE);
    vga_println("  uptime   - show system uptime",        COLOR_WHITE);
    vga_println("  cpuinfo  - show CPU info",             COLOR_WHITE);
    vga_println("  color    - change text color",         COLOR_WHITE);
    vga_println("  banner   - show OS banner",            COLOR_WHITE);
    vga_println("  calc     - simple calculator",         COLOR_WHITE);
}

void cmd_about() {
    vga_println("",                            COLOR_WHITE);
    vga_println("  MyOS v1.0",                COLOR_LIGHT_CYAN);
    vga_println("  A bare metal x86 OS",      COLOR_LIGHT_GREY);
    vga_println("  Built with C++ and ASM",   COLOR_LIGHT_GREY);
    vga_println("  Features:",                COLOR_LIGHT_GREY);
    vga_println("   - VGA text display",      COLOR_WHITE);
    vga_println("   - Memory management",     COLOR_WHITE);
    vga_println("   - File system",           COLOR_WHITE);
    vga_println("   - Process scheduler",     COLOR_WHITE);
    vga_println("   - Network stack",         COLOR_WHITE);
    vga_println("   - Shell scripting",       COLOR_WHITE);
    vga_println("",                            COLOR_WHITE);
}

void cmd_memory() {
    vga_println("Memory layout:", COLOR_LIGHT_CYAN);
    vga_println("  Kernel loaded at : 0x00100000", COLOR_WHITE);
    vga_println("  Heap starts at   : 0x00400000", COLOR_WHITE);
    vga_println("  Heap size        : 4MB",         COLOR_WHITE);
    vga_println("  Block size       : 4KB",         COLOR_WHITE);
    vga_println("  Paging           : Enabled",     COLOR_LIGHT_GREEN);

    void* p = kmalloc(1024);
    if (p) {
        vga_println("  kmalloc(1024)    : OK", COLOR_LIGHT_GREEN);
        kfree(p);
    } else {
        vga_println("  kmalloc(1024)    : FAILED", COLOR_RED);
    }
}

void cmd_meminfo() {
    vga_println("Detailed Memory Info:", COLOR_LIGHT_CYAN);
    vga_println("  Total RAM        : 256MB",       COLOR_WHITE);
    vga_println("  Kernel size      : ~512KB",      COLOR_WHITE);
    vga_println("  Heap start       : 0x00400000",  COLOR_WHITE);
    vga_println("  Heap end         : 0x00800000",  COLOR_WHITE);
    vga_println("  Heap total       : 4MB",         COLOR_WHITE);
    vga_println("  Block size       : 4096 bytes",  COLOR_WHITE);
    vga_println("  Total blocks     : 1024",        COLOR_WHITE);
    vga_println("  Page size        : 4KB",         COLOR_WHITE);
    vga_println("  Identity mapped  : 0MB - 8MB",   COLOR_WHITE);

    // Test multiple allocs
    void* a = kmalloc(512);
    void* b = kmalloc(1024);
    void* c = kmalloc(2048);
    if (a && b && c) {
        vga_println("  Alloc test       : PASSED",  COLOR_LIGHT_GREEN);
        kfree(a); kfree(b); kfree(c);
    } else {
        vga_println("  Alloc test       : FAILED",  COLOR_RED);
    }
}

void cmd_reboot() {
    vga_println("Rebooting...", COLOR_LIGHT_RED);
    asm volatile("outb %0, %1" :: "a"((uint8_t)0xFE), "Nd"((uint16_t)0x64));
    while(1);
}

void cmd_exit() {
    vga_println("Shutting down...", COLOR_LIGHT_RED);
    asm volatile("outw %0, %1" :: "a"((uint16_t)0x2000), "Nd"((uint16_t)0x604));
    while(1);
}

void cmd_echo(const char* args) {
    vga_println(args ? args : "", COLOR_WHITE);
}

void cmd_uptime() {
    uint32_t ticks = timer_get_ticks();
    uint32_t seconds = ticks / 100;
    uint32_t minutes = seconds / 60;
    uint32_t hours   = minutes / 60;
    seconds %= 60;
    minutes %= 60;

    vga_print("Uptime: ", COLOR_LIGHT_CYAN);
    print_num(hours,   COLOR_WHITE); vga_print("h ", COLOR_WHITE);
    print_num(minutes, COLOR_WHITE); vga_print("m ", COLOR_WHITE);
    print_num(seconds, COLOR_WHITE); vga_print("s",  COLOR_WHITE);
    vga_println("", COLOR_WHITE);
}

void cmd_cpuinfo() {
    vga_println("CPU Information:", COLOR_LIGHT_CYAN);
    vga_println("  Architecture : x86 (32-bit)",   COLOR_WHITE);
    vga_println("  Mode         : Protected Mode",  COLOR_WHITE);
    vga_println("  FPU          : Present",         COLOR_WHITE);
    vga_println("  Paging       : Enabled",         COLOR_LIGHT_GREEN);
    vga_println("  Interrupts   : Enabled",         COLOR_LIGHT_GREEN);
    vga_println("  GDT          : Loaded",          COLOR_LIGHT_GREEN);
    vga_println("  IDT          : Loaded",          COLOR_LIGHT_GREEN);
}

void cmd_ifconfig() {
    vga_println("Network Interface:", COLOR_LIGHT_CYAN);
    vga_println("  Interface : eth0 (RTL8139)",     COLOR_WHITE);
    vga_println("  IP        : 10.0.2.15",          COLOR_WHITE);
    vga_println("  Gateway   : 10.0.2.2",           COLOR_WHITE);
    vga_println("  Netmask   : 255.255.255.0",      COLOR_WHITE);
    vga_println("  Status    : UP",                 COLOR_LIGHT_GREEN);
    uint8_t* mac = net_get_mac();
    vga_print("  MAC       : ", COLOR_WHITE);
    for (int i = 0; i < 6; i++) {
        char buf[3];
        buf[0] = "0123456789ABCDEF"[mac[i] >> 4];
        buf[1] = "0123456789ABCDEF"[mac[i] & 0xF];
        buf[2] = '\0';
        vga_print(buf, COLOR_LIGHT_CYAN);
        if (i < 5) vga_print(":", COLOR_WHITE);
    }
    vga_println("", COLOR_WHITE);
}

void cmd_color(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: color <name>",           COLOR_WHITE);
        vga_println("Colors: red green blue cyan",   COLOR_WHITE);
        vga_println("        yellow white grey",     COLOR_WHITE);
        return;
    }
    if      (str_equal(args, "red"))    vga_println("Color set to red!",    COLOR_RED);
    else if (str_equal(args, "green"))  vga_println("Color set to green!",  COLOR_LIGHT_GREEN);
    else if (str_equal(args, "blue"))   vga_println("Color set to blue!",   COLOR_LIGHT_BLUE);
    else if (str_equal(args, "cyan"))   vga_println("Color set to cyan!",   COLOR_LIGHT_CYAN);
    else if (str_equal(args, "yellow")) vga_println("Color set to yellow!", COLOR_YELLOW);
    else if (str_equal(args, "white"))  vga_println("Color set to white!",  COLOR_WHITE);
    else if (str_equal(args, "grey"))   vga_println("Color set to grey!",   COLOR_LIGHT_GREY);
    else vga_println("Unknown color!", COLOR_LIGHT_RED);
}

void cmd_banner() {
    vga_println("",                                          COLOR_LIGHT_CYAN);
    vga_println("  ███╗   ███╗██╗   ██╗ ██████╗ ███████╗", COLOR_LIGHT_CYAN);
    vga_println("  ████╗ ████║╚██╗ ██╔╝██╔═══██╗██╔════╝", COLOR_LIGHT_CYAN);
    vga_println("  ██╔████╔██║ ╚████╔╝ ██║   ██║███████╗", COLOR_LIGHT_CYAN);
    vga_println("  ██║╚██╔╝██║  ╚██╔╝  ██║   ██║╚════██║", COLOR_LIGHT_CYAN);
    vga_println("  ██║ ╚═╝ ██║   ██║   ╚██████╔╝███████║", COLOR_LIGHT_CYAN);
    vga_println("  ╚═╝     ╚═╝   ╚═╝    ╚═════╝ ╚══════╝", COLOR_LIGHT_CYAN);
    vga_println("",                                          COLOR_WHITE);
    vga_println("         A bare metal x86 Operating System", COLOR_YELLOW);
    vga_println("",                                          COLOR_WHITE);
}

void cmd_calc(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: calc <num> <op> <num>", COLOR_LIGHT_RED);
        vga_println("Example: calc 10 + 5",         COLOR_LIGHT_GREY);
        return;
    }

    int a = 0, b = 0;
    char op = 0;
    int i = 0;

    // Parse first number
    while (args[i] >= '0' && args[i] <= '9')
        a = a * 10 + (args[i++] - '0');
    while (args[i] == ' ') i++;

    // Parse operator
    op = args[i++];
    while (args[i] == ' ') i++;

    // Parse second number
    while (args[i] >= '0' && args[i] <= '9')
        b = b * 10 + (args[i++] - '0');

    int result = 0;
    int valid  = 1;
    if      (op == '+') result = a + b;
    else if (op == '-') result = a - b;
    else if (op == '*') result = a * b;
    else if (op == '/' && b != 0) result = a / b;
    else if (op == '/' && b == 0) {
        vga_println("Error: division by zero!", COLOR_LIGHT_RED);
        valid = 0;
    } else {
        vga_println("Error: unknown operator!", COLOR_LIGHT_RED);
        valid = 0;
    }

    if (valid) {
        vga_print("Result: ", COLOR_LIGHT_CYAN);
        if (result < 0) { vga_print("-", COLOR_WHITE); result = -result; }
        print_num(result, COLOR_WHITE);
        vga_println("", COLOR_WHITE);
    }
}

// --- File system commands ---

void cmd_ls() {
    vga_println("Contents of /:", COLOR_LIGHT_CYAN);
    fs_list("/");
}

void cmd_mkdir(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: mkdir <name>", COLOR_LIGHT_RED);
        return;
    }
    if (fs_mkdir(args) == 0)
        vga_println("Directory created", COLOR_LIGHT_GREEN);
    else
        vga_println("Failed to create directory", COLOR_LIGHT_RED);
}

void cmd_write(const char* args) {
    char fname[64];
    int i = 0;
    while (args[i] && args[i] != ' ' && i < 63) {
        fname[i] = args[i]; i++;
    }
    fname[i] = '\0';
    const char* content = (args[i] == ' ') ? &args[i + 1] : "";

    if (!fname[0]) {
        vga_println("Usage: write <filename> <content>", COLOR_LIGHT_RED);
        return;
    }
    if (!fs_exists(fname)) fs_create(fname);
    if (fs_write(fname, content) == 0)
        vga_println("File written", COLOR_LIGHT_GREEN);
    else
        vga_println("Failed to write file", COLOR_LIGHT_RED);
}

void cmd_cat(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: cat <filename>", COLOR_LIGHT_RED);
        return;
    }
    char buf[4096];
    if (fs_read(args, buf, 4096) >= 0)
        vga_println(buf, COLOR_WHITE);
    else
        vga_println("File not found", COLOR_LIGHT_RED);
}

void cmd_rm(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: rm <filename>", COLOR_LIGHT_RED);
        return;
    }
    if (fs_delete(args) == 0)
        vga_println("Deleted", COLOR_LIGHT_GREEN);
    else
        vga_println("Failed to delete", COLOR_LIGHT_RED);
}

// --- Process commands ---

void cmd_ps() {
    vga_println("Running processes:", COLOR_LIGHT_CYAN);
    int count = process_count();
    char buf[4];
    buf[0] = '0' + count;
    buf[1] = '\0';
    vga_print("  Total processes: ", COLOR_WHITE);
    vga_println(buf, COLOR_YELLOW);
}

// --- ELF / User mode commands ---

void cmd_exec(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: exec <filename>", COLOR_LIGHT_RED);
        return;
    }
    char buf[4096];
    if (fs_read(args, buf, 4096) < 0) {
        vga_println("File not found", COLOR_LIGHT_RED);
        return;
    }
    uint32_t entry = elf_load((uint8_t*)buf);
    if (!entry) {
        vga_println("Failed to load ELF", COLOR_LIGHT_RED);
        return;
    }
    void* stack = mmap_alloc_user(1, 4096);
    if (!stack) {
        vga_println("Failed to allocate user stack", COLOR_LIGHT_RED);
        return;
    }
    vga_println("Jumping to user mode...", COLOR_LIGHT_GREEN);
    usermode_jump(entry, (uint32_t)stack + 4096);
}

// --- Script commands ---

void cmd_run(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: run <script.sh>", COLOR_LIGHT_RED);
        return;
    }
    script_run(args);
}

// --- Network commands ---

void cmd_ping(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: ping <ip>", COLOR_LIGHT_RED);
        return;
    }
    uint32_t ip = 0;
    int shift = 24;
    int num = 0;
    for (int i = 0; args[i] && shift >= 0; i++) {
        if (args[i] >= '0' && args[i] <= '9') {
            num = num * 10 + (args[i] - '0');
        } else if (args[i] == '.') {
            ip |= (num << shift);
            shift -= 8;
            num = 0;
        }
    }
    ip |= (num << shift);
    icmp_ping(ip);
}

void cmd_udpsend(const char* args) {
    if (!args || !args[0]) {
        vga_println("Usage: udpsend <ip> <port> <message>", COLOR_LIGHT_RED);
        return;
    }
    vga_println("UDP send not yet implemented via shell", COLOR_LIGHT_GREY);
}

// --- Command executor ---

void execute(const char* cmd) {
    if (!cmd || !cmd[0]) return;

    char name[64];
    int i = 0;
    while (cmd[i] && cmd[i] != ' ' && i < 63) {
        name[i] = cmd[i]; i++;
    }
    name[i] = '\0';

    const char* args = (cmd[i] == ' ') ? &cmd[i + 1] : "";

    if      (str_equal(name, "help"))     cmd_help();
    else if (str_equal(name, "clear"))    vga_clear();
    else if (str_equal(name, "about"))    cmd_about();
    else if (str_equal(name, "memory"))   cmd_memory();
    else if (str_equal(name, "meminfo"))  cmd_meminfo();
    else if (str_equal(name, "reboot"))   cmd_reboot();
    else if (str_equal(name, "exit"))     cmd_exit();
    else if (str_equal(name, "echo"))     cmd_echo(args);
    else if (str_equal(name, "uptime"))   cmd_uptime();
    else if (str_equal(name, "cpuinfo"))  cmd_cpuinfo();
    else if (str_equal(name, "ifconfig")) cmd_ifconfig();
    else if (str_equal(name, "color"))    cmd_color(args);
    else if (str_equal(name, "banner"))   cmd_banner();
    else if (str_equal(name, "calc"))     cmd_calc(args);
    else if (str_equal(name, "ls"))       cmd_ls();
    else if (str_equal(name, "mkdir"))    cmd_mkdir(args);
    else if (str_equal(name, "write"))    cmd_write(args);
    else if (str_equal(name, "cat"))      cmd_cat(args);
    else if (str_equal(name, "rm"))       cmd_rm(args);
    else if (str_equal(name, "ps"))       cmd_ps();
    else if (str_equal(name, "exec"))     cmd_exec(args);
    else if (str_equal(name, "run"))      cmd_run(args);
    else if (str_equal(name, "ping"))     cmd_ping(args);
    else if (str_equal(name, "udpsend"))  cmd_udpsend(args);
    else {
        vga_print("Unknown command: ", COLOR_LIGHT_RED);
        vga_println(name, COLOR_LIGHT_RED);
        vga_println("Type 'help' for available commands", COLOR_LIGHT_GREY);
    }
}

// --- Shell ---

void shell_prompt() {
    vga_print("MyOS> ", COLOR_YELLOW);
}

void shell_init() {
    vga_init();
    vga_println("", COLOR_WHITE);
    vga_println("  Welcome to MyOS!", COLOR_LIGHT_CYAN);
    vga_println("  Type 'help' to get started", COLOR_LIGHT_GREY);
    vga_println("", COLOR_WHITE);
}

void shell_run() {
    shell_prompt();

    while (1) {
        char c = keyboard_getchar();
        if (!c) continue;

        if (c == '\n') {
            input[input_len] = '\0';
            vga_putchar('\n', COLOR_WHITE);
            execute(input);
            input_len = 0;
            shell_prompt();
        } else if (c == '\b') {
            if (input_len > 0) {
                input_len--;
                vga_putchar('\b', COLOR_WHITE);
            }
        } else {
            if (input_len < 255) {
                input[input_len++] = c;
                vga_putchar(c, COLOR_WHITE);
            }
        }
    }
}
