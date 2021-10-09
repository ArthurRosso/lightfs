#ifndef FYLESYSTEM_H
#define FYLESYSTEM_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "file.h"
#include "metadata.h"

#define NR_CLUSTERS     256
#define CLUSTER_SIZE    256 // 32768*8
#define BYTE 8

typedef struct Filesystem{
    // Descritor do arquivo
    int         fd;

    // Metadados
    Metadata_t* metadata;

    // Indice (tabela FAT)
    uint8_t     index[NR_CLUSTERS];
} Filesystem_t;


int make_filesystem(Filesystem_t* fs);
int mount_filesystem(Filesystem_t* fs);
int make_file(File_t* file, char* fname);
int make_dir(File_t* file, char* fname);


int open_disk(Filesystem_t* fs, char *path);
int close_disk(Filesystem_t* fs);
int cluster_write(Filesystem_t* fs, size_t block, const void* buf);
int cluster_read(Filesystem_t* fs, size_t block, void* buf);


#endif