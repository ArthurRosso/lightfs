//#include <cstdint>
#include <stdint.h>

typedef struct __attribute__((packed)) Metadata{
    // Metadados
    uint8_t     meta_size;
    uint8_t     index_size;     // Tamanho do indice = 2^8
    uint8_t     cluster_size;   // Tamanho do cluster = 2^15
    uint8_t     index_begin;    // Byte inicio do indice
    uint16_t    root_begin;     // Byte inicio do primeiro cluster(root) = 4
    uint8_t     padding[2];
} Metadata_t;