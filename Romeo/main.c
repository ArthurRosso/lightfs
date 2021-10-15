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
    
    if(make_filesystem(filesys)){
        // if(mount_filesystem(filesys_teste)){
            printf("ok: %d\n", filesys->metadata->root_begin);
            
            make_file(filesys, "teste.txt", 0, 1);
            char* texto1 = "Testando, colocando dados no cluster 1";
            write_file(filesys, 1, texto1, strlen(texto1));
            
            make_file(filesys, "Pasta", 0, 0);

            make_file(filesys, "Lorem Ipsum.txt", 2, 1);
            char* texto2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin vel orci arcu. In hac habitasse platea dictumst. Etiam erat odio, consectetur vitae dolor porta, laoreet varius diam. Curabitur blandit malesuada mi sed rutrum. Aliquam erat volutpat. Curabitur sit amet libero magna. Vivamus auctor convallis dui eu semper. Nullam ut fermentum metus. Fusce pellentesque sagittis gravida. Etiam vehicula leo vitae dui sodales, et mollis magna aliquet. Etiam porttitor accumsan orci, at tincidunt mauris consequat et. Phasellus vitae cursus arcu. Maecenas laoreet pellentesque tellus a lacinia.\n\nDonec consequat fringilla ipsum et faucibus. Nunc rhoncus felis sit amet eros molestie posuere. Donec ut pretium eros. Maecenas vehicula feugiat enim, quis blandit nunc blandit sit amet. Sed sed aliquam massa, ac vestibulum turpis. Fusce laoreet dui dapibus convallis placerat. Donec sem eros, interdum ut tempor at, suscipit sit amet ligula. Maecenas diam eros, eleifend id nulla in, viverra tincidunt turpis. Curabitur nec ante eu ante ullamcorper fermentum at ac nisi.";
            write_file(filesys, 3, texto2, strlen(texto2));

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