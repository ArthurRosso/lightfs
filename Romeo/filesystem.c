#include "filesystem.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int make_filesystem(Filesystem_t* fs){
    int i;
    Metadata_t* m = malloc(sizeof(Metadata_t));
    m->index_size=0x08;        // somente o expoente do 2
    m->cluster_size=0x0F;      // somente o expoente do 2
    m->index_begin=0x08;       // inicio do index
    m->root_begin=0x108;        // inicio do root


    write(fs->fd, m, sizeof(Metadata_t));
    fs->metadata=m;

    
    //cluster_write(fs->cluster, 1, (char*)f);    // escreve no disco o root
    for(i=0; i<NR_CLUSTERS; i++){
        fs->index[i] = 0x00;                     // definir tabela FAT como 0
        write(fs->fd, fs->index, sizeof(uint8_t));  // escreve no disco cada elemento da tabela fat
    }
    

    // criar root
    File_t root = {0};
    strcpy(root.name, "root");
    root.attr = 0x18;                                   // diretório e volume id
    root.createTime = time(0);
    //root.accessTime = time(0);
    root.fileSize = 0x00;
    root.cluster = 0x00;
    cluster_write(fs, 0, &root);
    index_write(fs, 0, 0xFF);

    File_t cluster = {0};
    for(i=1; i<NR_CLUSTERS; i++)
        cluster_write(fs, i, &cluster);
    return 1;
}
int mount_filesystem(Filesystem_t* fs){
    int i;

    // pegar metadados
    Metadata_t* m = malloc(sizeof(Metadata_t));
    read(fs->fd, m, sizeof(Metadata_t)-3);
    fs->metadata = m;

    read(fs->fd, fs->index, sizeof(fs->index));
    //pegar root
    File_t root;
    cluster_read(fs, 0, &root);

    printf("%ld\n", root.createTime);

    return 1;
}

int open_disk(Filesystem_t* fs, char *path){

    int fd;
	struct stat st;

	open(path, O_CREAT, 0644);

    if ((fd = open(path, O_RDWR, 0644)) < 0) {
		printf("problema no open");
		return -1;
	}

	if (fstat(fd, &st)) {
		printf("problema no fstat");
		return -1;
	}

	/*if (st.st_size % CLUSTER_SIZE != 0) {
		printf("problema no block");
		return -1;
	}*/

	fs->fd = fd;
	//cl->cluster_index = st.st_size / CLUSTER_SIZE; ??

}
int close_disk(Filesystem_t* fs){
    close(fs->fd);
    fs->fd = -1;

    return 1;
}

int cluster_read(Filesystem_t* fs, size_t index, void* buf){
    // move o descritor até o indice do cluster passado como parametro
	if (lseek(fs->fd, index * CLUSTER_SIZE + fs->metadata->root_begin, SEEK_SET) < 0) {
		printf("\nproblema no (r) lseek");
		return 0;
	}

	// lê o conteudo do cluster no buffer
	if (read(fs->fd, buf, CLUSTER_SIZE) < 0) {
		printf("\nproblema no read");
		return 0;
	}

    return 1;
}
int cluster_write(Filesystem_t* fs, size_t index, const void* buf){
    // move o descritor até o indice do cluster passado como parametro
	if (lseek(fs->fd, index * CLUSTER_SIZE + fs->metadata->root_begin, SEEK_SET) < 0) {
		printf("\nproblema no lseek (cw)");
		return 0;
	}

	// escreve o conteudo de buffer no cluster
	if (write(fs->fd, buf, CLUSTER_SIZE) < 0) {
		printf("\nproblema no write %x", buf);
		return 0;
	}

	return 1;
}

int index_write(Filesystem_t* fs, uint8_t index, uint8_t value){
    // move o descritor até o indice do cluster passado como parametro
	if (lseek(fs->fd, index + fs->metadata->index_begin, SEEK_SET) < 0) {
		printf("\nproblema no lseek (iw)");
		return 0;
	}

	// escreve o conteudo de buffer no cluster
	if (write(fs->fd, &value, sizeof(uint8_t)) < 0) {
		printf("\nproblema no write (i) %x", value);
		return 0;
	}

	return 1;
}
int index_read(Filesystem_t* fs, uint8_t index, uint8_t* value){
    // move o descritor até o indice do cluster passado como parametro
	if (lseek(fs->fd, index + fs->metadata->index_begin, SEEK_SET) < 0) {
		printf("\nproblema no lseek (ir)");
		return 0;
	}

	// lê o conteudo do cluster no buffer
	if (read(fs->fd, value, sizeof(uint8_t)) < 0) {
		printf("\nproblema no read (i)");
		return 0;
	}

    return 1;
}

uint8_t find_free_cluster(Filesystem_t* fs){
    uint8_t index, temp;
    int i;

    for(i=1; i<NR_CLUSTERS-1; i++){
        index_read(fs, i, &temp);
        if(temp == 0x00){
            index = i;
            break;  
        }
    }

    return index;
}

int make_file(Filesystem_t* fs, char* fname, uint8_t father, uint8_t type){
    // Checagem se o cluster pai é uma pasta
    File_t* file_father = malloc(sizeof(File_t));
    int i;

    cluster_read(fs, father, file_father); 

    //printf("%d\n", file_father->attr);
    if(file_father->attr && 0x10  == 0){
        return 1;
    }

    uint8_t index = find_free_cluster(fs); 
    index_write(fs, index, 0xFF);           // Atualização do index para EOF
    
    for(i=0; i<sizeof(file_father->data); i++){
        if(file_father->data[i] == 0x00){
            file_father->data[i] = index;
            break;
        }       
    }
    cluster_write(fs, father, file_father);
    free(file_father); // Livrai-nos dos ponteiros fantasmas amém

    // Criação do cluster
    File_t* file = calloc(sizeof(File_t), 1);
        
    // Checagem de tamanho do nome, extensão....

    strcpy(file->name, fname);
    if(type)
        file->attr = 0x20; // arquivo
    else
        file->attr = 0x10; // diretório
    file->createTime = time(0);
    file->fileSize = 0x00;
    file->cluster = index;

    cluster_write(fs, index, file);
    
    return 0;
}

int make_dir(File_t* dir, char* dname){
    //Checagem do tamanho do nome
    strcpy(dir->name, dname);
    dir->createTime = time(0);
    //dir->accessTime = time(0);
    dir->fileSize = 0x00;

    return 1;
}

int write_file(Filesystem_t* fs, uint8_t index, void* data, int len){
    // Checagem se o cluster index é um arquivo
    File_t* file = malloc(sizeof(File_t));
    uint8_t next = index;

    cluster_read(fs, index, file); 

    //printf("%d\n", file->attr);
    if(file->attr && 0x20  == 0){
        return 1;
    }
    printf("%d\n", len/CLUSTER_SIZE);

    while(len/CLUSTER_SIZE>0){
        printf("entrou\n");
        memcpy(file->data, data, CLUSTER_SIZE);

        cluster_write(fs, next, file);

        len -= CLUSTER_SIZE;

        // procurar um cluster livre
        next = find_free_cluster(fs);

        index_write(fs, index, next);
        index=next;
    }

    index_write(fs, next, 0xFF);
    
    memcpy(file->data, data, len);

    cluster_write(fs, next, file);
}