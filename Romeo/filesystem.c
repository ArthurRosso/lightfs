#include "filesystem.h"
#include "file.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int make_filesystem(Filesystem_t* fs){
    int i;
    Metadata_t* m = (Metadata_t*)malloc(sizeof(Metadata_t));
    m->index_size=0x08;     // somente o expoente do 2
    m->cluster_size=0x0F;   // somente o expoente do 2
    m->index_begin=0x08;    // inicio do index
    m->root_begin=0x108;    // inicio do root

    lseek(fs->fd, 0, SEEK_SET);
    write(fs->fd, m, sizeof(Metadata_t));
    fs->metadata=m;

    for(i=0; i<NR_CLUSTERS; i++){
        index_write(fs, i, 0x00);  // escreve no disco cada elemento da tabela fat
    }
    
    // criar root
    File_t root = {0};
    strcpy(root.name, "root");
    root.attr = 0x18;                                   // diretório e volume id
    root.createTime = time(0);
    cluster_write(fs, 0, &root);
    index_write(fs, 0, 0xFF);

    File_t cluster = {0};
    for(i=1; i<NR_CLUSTERS; i++)
        cluster_write(fs, i, &cluster);
    return 1;
}

int mount_filesystem(Filesystem_t* fs){
    // pegar metadados
    Metadata_t* m = (Metadata_t*)malloc(sizeof(Metadata_t));
    read(fs->fd, m, sizeof(Metadata_t)-3);
    fs->metadata = m;

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
    File_t* file_father = (File_t*)malloc(sizeof(File_t));
    int i;

    cluster_read(fs, father, file_father); 

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
    File_t* file = (File_t*)calloc(sizeof(File_t), 1);
        
    // Checagem de tamanho do nome, extensão....

    strcpy(file->name, fname);
    if(type)
        file->attr = 0x20; // arquivo
    else
        file->attr = 0x10; // diretório
    file->createTime = time(0);

    cluster_write(fs, index, file);
    
    return 0;
}

int write_file(Filesystem_t* fs, uint8_t index, void* data, int len){
    // Checagem se o cluster index é um arquivo
    File_t* file = (File_t*)malloc(sizeof(File_t));
    uint8_t next = index;
    printf("%d\n", len);
    cluster_read(fs, index, file); 
    if(file->attr && 0x20  == 0){
        return 1;
    }

    void *aux = calloc(1, sizeof(file->data));
    memcpy(file->data, aux, sizeof(file->data));

    while(len/sizeof(file->data)>0){
        memcpy(file->data, data, sizeof(file->data));

        cluster_write(fs, next, file);

        len -= sizeof(file->data);
        // procurar um cluster livre
        next = find_free_cluster(fs);

        index_write(fs, index, next);
        index=next;
    }

    index_write(fs, next, 0xFF);
    memcpy(file->data, data, len);
    cluster_write(fs, next, file);
    return 0;
}

    int delete_file(Filesystem_t* fs, uint8_t index, uint8_t father){
        File_t* file = (File_t*)malloc(sizeof(File_t));
        File_t aux = {0};
        uint8_t value;
        cluster_read(fs, index, file);
        // Tirar do pai
        cluster_read(fs, father, &aux);
        for(int i=0; i<sizeof(aux.data); i++){
            if(aux.data[i] == index){
                aux.data[i] = 0x00;
                break;
            }
        }
        cluster_write(fs, father, &aux);        
        do{
            index_read(fs, index, &value);
            index_write(fs, index, 0x00);
            index = value;
        }while(value != 0xFF);

        return 0;
    }

bool is_dir(Filesystem_t* fs, uint8_t index){
    File_t file;
    cluster_read(fs, index, &file);
    if(file.attr && ATTR_DIRECTORY != 0)
        return true;
    else
        return false;
} 

int show_dir(Filesystem_t* fs, uint8_t index, uint8_t** files){
    if(is_dir(fs, index) == false)
        return -1;

    File_t file;
    uint8_t num = child_num(fs, index);
    cluster_read(fs, index, &file);
    *files = (uint8_t*) malloc(num * sizeof(uint8_t));
    uint8_t count = 0;
    for(int i=0; i<sizeof(file.data) && count!=num; i++){
        if(file.data[i] != 0x00){
            *(*files+count) = file.data[i];
            count++;
        }
    }
    return num;
}

char* return_name(Filesystem_t* fs, uint8_t index){
    File_t file;
    cluster_read(fs, index, &file);
    char* name = (char*)malloc(sizeof(file.name));
    strcpy(name, file.name);

    return name;
}

int child_num(Filesystem_t* fs, uint8_t index){
    File_t file;
    int num = 0;
    cluster_read(fs, index, &file);
    for(int i=0; i<sizeof(file.data); i++){
        if(file.data[i] != 0x00){
            num++;
        }
    }

    return num;
}

int set_name(Filesystem_t* fs, uint8_t index, void* name){
    File_t file;
    char newname[23] = {0};
    cluster_read(fs, index, &file);
    memcpy(newname, name, sizeof(file.name));
    memcpy(&file.name, newname, sizeof(file.name));
    cluster_write(fs, index, &file);
    
    return 0;
}

time_t return_time(Filesystem_t* fs, uint8_t index){
    File_t file;
    cluster_read(fs, index, &file);

    return file.createTime;
}

