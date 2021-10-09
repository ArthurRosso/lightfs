#include <stdio.h>
#include <stdlib.h>
#include "filesystem.h"

int main (){
    char* curDir="/root";
    Filesystem_t* filesys = malloc(sizeof(Filesystem_t));
    //Filesystem_t* filesys_teste = malloc(sizeof(Filesystem_t));

    //Cluster_t* cl = malloc(CLUSTER_SIZE);
    //Cluster_t* cl_teste = malloc(CLUSTER_SIZE);
    //filesys->cluster = cl;
    //filesys_teste->cluster = cl_teste;
    
    open_disk(filesys, "cdoispontos.bin");
    
    if(mount_filesystem(filesys)){
        // if(mount_filesystem(filesys_teste)){
            printf("ok: %d\n", filesys->metadata->root_begin);
            // printf("ok: %d\n", filesys_teste->metadata->root_begin);
            // printf("ok");
        // } else {
        //     printf("unable to mount_filesystem");
        // }
    } else {
        printf("unable to make_filesystem");
    }

    return 0;
}