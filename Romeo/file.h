#ifndef FILE_H
#define FILE_H

//#include <cstdint>
#include <stdint.h>
#include <time.h>

enum attribute { 
    ATTR_READ_ONLY = 0x01,
    ATTR_HIDDEN = 0x02,
    ATTR_SYSTEM = 0x04,
    ATTR_VOLUME_ID = 0x08,
    ATTR_DIRECTORY = 0x10,
    ATTR_ARCHIVE = 0x20
};

typedef struct __attribute__((packed)) File {
    char        name[10];   // Nome (7 bytes) com extensão (3 byte)
    uint8_t     attr;       // Atributo
    time_t      createTime; // Data de criação.
    time_t      accessTime; // Ultimo acesso.
    uint8_t     cluster;    // Cluster de começo (0 for an empty file)
    uint32_t    fileSize;   // Tamanho do arquivo
    char        deleted;    // Se foi deleted
    uint8_t     data[256-33];
} File_t;

#endif