
#include <stdint.h>

typedef struct Metadata{
    // Metadados
    uint8_t     index_size;     // Tamanho do indice = 2^8
    uint8_t     cluster_size;   // Tamanho do cluster = 2^15
    uint8_t     index_begin;    // Byte inicio do indice = 0 até 3
    uint8_t     root_begin;     // Byte inicio do primeiro cluster(root) = 4
} Metadata_t;