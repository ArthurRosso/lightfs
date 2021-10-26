#ifndef FYLESYSTEM_H
#define FYLESYSTEM_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "file.h"
#include <stdbool.h>
#include "metadata.h"


//#define BYTE 8

typedef struct Filesystem{
    // Descritor do arquivo
    int         fd;

    // Metadados
    Metadata_t* metadata;
} Filesystem_t;


int make_filesystem(Filesystem_t* fs);
int mount_filesystem(Filesystem_t* fs);

int open_disk(Filesystem_t* fs, char *path);
int close_disk(Filesystem_t* fs);

int cluster_write(Filesystem_t* fs, size_t block, const void* buf);
int cluster_read(Filesystem_t* fs, size_t block, void* buf);

int index_write(Filesystem_t* fs, uint8_t index, uint8_t value);
int index_read(Filesystem_t* fs, uint8_t index, uint8_t* buf);

int make_file(Filesystem_t* fs, char* fname, uint8_t father, uint8_t type);
int delete_file(Filesystem_t* fs, uint8_t index, uint8_t father);

int write_file(Filesystem_t* fs, uint8_t index, void* data, int len);

uint8_t find_free_cluster(Filesystem_t* fs);

bool is_dir(Filesystem_t* fs, uint8_t index);
int show_dir(Filesystem_t* fs, uint8_t index, uint8_t** files);
int child_num(Filesystem_t* fs, uint8_t index); 
char* return_name(Filesystem_t* fs, uint8_t index);
int set_name(Filesystem_t* fs, uint8_t index, void* name);
time_t return_time(Filesystem_t* fs, uint8_t index);

#endif