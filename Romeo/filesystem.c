#include "filesystem.h"
#include <stdint.h>
#include <sys/types.h>

int make_filesystem(Filesystem_t* fs){
    int i;

    Metadata_t* m = malloc(8); // definir metadados
    m->meta_size=0x08;
    m->index_size=0x08;        // somente o expoente do 2
    m->cluster_size=0x0F;      // somente o expoente do 2
    m->index_begin=0x08;       // inicio do index
    m->root_begin=0x108;        // inicio do root


    write(fs->cluster->fd, m, m->meta_size);
    fs->metadata=m;

    fat_t f;                            // aloca o ponteiro para a tabela FAT
    //f->index = malloc(NR_CLUSTERS);   // aloca a tabela FAT
    //f->index[0] = m->root_begin;      // ponteiro para o root
    
    //cluster_write(fs->cluster, 1, (char*)f);    // escreve no disco o root
    for(i=0; i<NR_CLUSTERS; i++){
        f.index[i] = 0x00;                     // definir tabela FAT como 0
        write(fs->cluster->fd, f.index, sizeof(uint8_t));  // escreve no disco cada elemento da tabela fat
    }
    //fs->fat=f;
    
    // criar root
    File_t root;
    strcpy(root.name, "root");
    root.attr = 0x18;                                   // diretório e volume id
    root.createTime = time(0);
    root.accessTime = time(0);
    root.fileSize = 0x00;
    root.deleted = 0;
    for(i=0; i<sizeof(root.data); i++)
        root.data[i] = 0x00;
    cluster_write(fs->cluster, 0, &root);


    return 1;
}

/*
int mount_filesystem(Filesystem_t* fs){
    int i;

    // pegar metadados
    Metadata_t* m = malloc(4);
    cluster_read(fs->cluster, 0, m);
    fs->metadata=m;


    fat_t* f = malloc(sizeof(fat_t*));
    f->index = malloc(NR_CLUSTERS*sizeof(uint8_t*));
    for(i=0; i<NR_CLUSTERS; i++){
        cluster_read(fs->cluster, i+1, (char*)f + CLUSTER_SIZE*i);
    }
    fs->fat=f;

    // pegar root
    File_t root;
    cluster_read(fs->cluster, m->root_begin, &root);

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
*/