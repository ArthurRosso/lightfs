
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cluster.h"
#include "file.h"
#include "metadata.h"
#include "fat.h"

#define NR_CLUSTERS 256
#define BYTE 8

typedef struct Filesystem{
    Metadata_t* metadata;

    // Indice (tabela FAT)
    fat_t*      fat;

    // Área de dados
    Cluster_t*  cluster;
} Filesystem_t;

int make_filesystem(Filesystem_t* fs);
int mount_filesystem(Filesystem_t* fs);

int make_file(File_t* file, char* fname);

int make_dir(File_t* file, char* fname);