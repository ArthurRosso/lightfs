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


//#define BYTE 8

typedef struct Filesystem{
    // Descritor do arquivo
    int         fd;

    // Metadados
    Metadata_t* metadata;

    // Indice (tabela FAT) TIRAR?
    uint8_t     index[NR_CLUSTERS];
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
int make_dir(File_t* file, char* fname);

int write_file(Filesystem_t* fs, uint8_t index, void* data, long len);


#endif