
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cluster.h"
#include "file.h"

#define NR_CLUSTERS 256
#define BYTE 8

typedef struct Filesystem{
    // Metadados
    uint8_t     index_size;     // Tamanho do indice = 2^8
    uint8_t     cluster_size;   // Tamanho do cluster = 2^15
    uint8_t     index_begin;    // Byte inicio do indice = 0 até 3
    uint8_t     root_begin;     // Byte inicio do primeiro cluster(root) = 4

    // Indice (tabela FAT)
    uint8_t*    index;          // indice: vetor de 256 posições de ponteiros de 8 bits

    // Área de dados
    Cluster_t*    cluster;
} Filesystem_t;

int make_filesystem(Filesystem_t* fs);

int make_file(File_t* file, char* fname);

int make_dir(File_t* file, char* fname);