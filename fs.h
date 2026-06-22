#ifndef FS_H
#define FS_H

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#define MAX_FILES     64
#define MAX_FILENAME  32
#define MAX_FILESIZE  4096
#define MAX_DIRS      16

struct File {
    char     name[MAX_FILENAME];
    char     data[MAX_FILESIZE];
    uint32_t size;
    uint8_t  used;
    uint8_t  is_dir;
    char     parent[MAX_FILENAME];
};

void fs_init();
int  fs_create(const char* name, const char* parent = "/");
int  fs_mkdir(const char* name, const char* parent = "/");
int  fs_write(const char* name, const char* data);
int  fs_read(const char* name, char* buf, uint32_t size);
int  fs_delete(const char* name);
void fs_list(const char* dir);
int  fs_exists(const char* name);

#endif
