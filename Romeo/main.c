#include <stdio.h>
#include <stdlib.h>

#include "filesystem.h"

int main (){
    char* curDir="/root";
    Filesystem_t* filesys = malloc(sizeof(Filesystem_t));
    
    if(make_filesystem(filesys)){
        printf("ok");
    }

    return 0;
}