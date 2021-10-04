#include "filesystem.h"

int make_filesystem(Filesystem_t* fs){
    int i;

    // definir metadados
    fs->index_size=0x08; // somente o expoente do 2
    fs->cluster_size=0x0F; // somente o expoente do 2
    fs->index_begin=0x00;
    fs->root_begin=0x04;


    fs->index = malloc(NR_CLUSTERS*BYTE);
    // definir tabela FAT como 0
    for(i=0; i<NR_CLUSTERS; i++){
        fs->index[i] = 0x00;
    }
    fs->index[0]=0x02; // ponteiro para o cluster depois


    // criar root
    File_t root;
    root.name = "root";
    root.attr = 0x18; // diretório e volume id
    root.createTime = time(0);
    root.accessTime = time(0);
    root.fileSize = 0x00;
    root.deleted = 0;

    //fs->cluster = (Cluster_t*)&root;
    //printf("%x", fs->cluster->data);

    return 1;
}

int make_file(File_t* file, char* fname){

    strcpy(file->name, fname);
    file->attr = 0x20; // arquivo
    file->createTime = time(0);
    file->accessTime = time(0);
    file->fileSize = 0x00;
    file->deleted = 0;
}

int make_dir(File_t* dir, char* dname){

    strcpy(dir->name, dname);
    dir->attr = 0x10; // diretório
    dir->createTime = time(0);
    dir->accessTime = time(0);
    dir->fileSize = 0x00;
    dir->deleted = 0;
}