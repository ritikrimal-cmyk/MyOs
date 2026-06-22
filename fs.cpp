#include "fs.h"
#include "vga.h"

static File files[MAX_FILES];

static int str_equal(const char* a, const char* b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

static void str_copy(char* dst, const char* src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static int str_len(const char* s) {
    int i = 0;
    while (s[i]) i++;
    return i;
}

void fs_init() {
    for (int i = 0; i < MAX_FILES; i++)
        files[i].used = 0;

    // Create root directory
    files[0].used   = 1;
    files[0].is_dir = 1;
    files[0].size   = 0;
    str_copy(files[0].name,   "/", MAX_FILENAME);
    str_copy(files[0].parent, "",  MAX_FILENAME);
}

static File* find_file(const char* name) {
    for (int i = 0; i < MAX_FILES; i++)
        if (files[i].used && str_equal(files[i].name, name))
            return &files[i];
    return 0;
}

static File* find_free() {
    for (int i = 0; i < MAX_FILES; i++)
        if (!files[i].used)
            return &files[i];
    return 0;
}

int fs_exists(const char* name) {
    return find_file(name) != 0;
}

int fs_create(const char* name, const char* parent) {
    if (find_file(name)) return -1; // already exists
    File* f = find_free();
    if (!f) return -1; // no space
    f->used   = 1;
    f->is_dir = 0;
    f->size   = 0;
    str_copy(f->name,   name,   MAX_FILENAME);
    str_copy(f->parent, parent, MAX_FILENAME);
    return 0;
}

int fs_mkdir(const char* name, const char* parent) {
    if (find_file(name)) return -1;
    File* f = find_free();
    if (!f) return -1;
    f->used   = 1;
    f->is_dir = 1;
    f->size   = 0;
    str_copy(f->name,   name,   MAX_FILENAME);
    str_copy(f->parent, parent, MAX_FILENAME);
    return 0;
}

int fs_write(const char* name, const char* data) {
    File* f = find_file(name);
    if (!f || f->is_dir) return -1;
    int len = str_len(data);
    if (len >= MAX_FILESIZE) len = MAX_FILESIZE - 1;
    for (int i = 0; i < len; i++)
        f->data[i] = data[i];
    f->data[len] = '\0';
    f->size = len;
    return 0;
}

int fs_read(const char* name, char* buf, uint32_t size) {
    File* f = find_file(name);
    if (!f || f->is_dir) return -1;
    uint32_t len = f->size < size - 1 ? f->size : size - 1;
    for (uint32_t i = 0; i < len; i++)
        buf[i] = f->data[i];
    buf[len] = '\0';
    return len;
}

int fs_delete(const char* name) {
    if (str_equal(name, "/")) return -1; // can't delete root
    File* f = find_file(name);
    if (!f) return -1;
    f->used = 0;
    return 0;
}

void fs_list(const char* dir) {
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].used && str_equal(files[i].parent, dir)) {
            if (files[i].is_dir) {
                vga_print("  [DIR]  ", COLOR_LIGHT_CYAN);
            } else {
                vga_print("  [FILE] ", COLOR_LIGHT_GREY);
            }
            vga_println(files[i].name, COLOR_WHITE);
            found++;
        }
    }
    if (!found)
        vga_println("  (empty)", COLOR_DARK_GREY);
}
