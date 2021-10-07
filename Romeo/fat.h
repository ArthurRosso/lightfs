
#include <stdint.h>

typedef struct fat{
    uint8_t*    index;  // indice: vetor de 256 posições de ponteiros de 8 bits
} fat_t;