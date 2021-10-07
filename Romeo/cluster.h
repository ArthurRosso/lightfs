#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define CLUSTER_SIZE 512 // 32768*8

typedef struct Cluster{
    int fd; // file descriptor
    size_t cluster_index; // nr do cluster
    
} Cluster_t;

int open_disk(Cluster_t* cl, char *path);

int close_disk(Cluster_t* cl);

int cluster_write(Cluster_t* cl, size_t block, const void* buf);

int cluster_read(Cluster_t* cl, size_t block, void* buf);