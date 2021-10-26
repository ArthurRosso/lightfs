#ifndef FILE_H
#define FILE_H

//#include <cstdint>
#include <stdint.h>
#include <time.h>

#define NR_CLUSTERS     256
#define CLUSTER_SIZE    32768 // 32768

enum attribute { 
    ATTR_DIRECTORY = 0x10,
    ATTR_ARCHIVE = 0x20
};

typedef struct __attribute__((packed)) File {
    char        name[23];   // Nome (15 bytes) com extensão (3 byte)
    uint8_t     attr;       // Atributo
    time_t      createTime; // Data de criação.
    uint8_t     data[CLUSTER_SIZE-32];
} File_t;

#endif