#include "filesystem.h"
#include <stdint.h>
#include <string.h>
#include <unistd.h>

int make_filesystem(Filesystem_t* fs){
    int i;

    //
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
    root.accessTime = time(0);
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


int make_file(Filesystem_t* fs, File_t* file, char* fname, uint8_t father){
    // Checagem se o cluster pai é uma pasta
    if(fs->index[father] && 0x10  != 0)
        return 1;

    // Posicionamento na tabela
    uint8_t index;
    int i;
    for(i=1; i<NR_CLUSTERS-1; i++){
        if(fs->index[i] == 0x00){
            index = i;
            break;  
        }
    }
    index_write(fs, index, 0xFF);   // Atualização do index para EOF
    
    // Atualização do cluster pai - HARDCODED - POR FAVOR ARRUMAR 
    int* buf = malloc(sizeof(file->data));
    lseek(fs->fd, father * CLUSTER_SIZE + fs->metadata->root_begin + 32 /*padding do data*/, SEEK_SET);
    read(fs->fd, buf, sizeof(uint8_t));

    for(i=0; i<sizeof(buf); i++){
        if(buf[i] == 0x00){
            lseek(fs->fd, father * CLUSTER_SIZE + fs->metadata->root_begin + 32 + i /*padding do data*/, SEEK_SET);
            write(fs->fd, &index, sizeof(uint8_t));
            break;
        }       
    }
    free(buf); // Livrai-nos dos ponteiros fantasmas amém

    // Checagem de tamanho do nome, extensão....
    strcpy(file->name, fname);
    file->attr = 0x20; // arquivo
    file->createTime = time(0);
    file->accessTime = time(0);
    file->fileSize = 0x00;
    file->cluster = index;

    strcpy(file->data, "Escrevendo dados em um arquivo.");


    cluster_write(fs, index, file);
    
    return 0;
}

int make_dir(File_t* dir, char* dname){

    //Checagem do tamanho do nome
    strcpy(dir->name, dname);
    dir->attr = 0x10; // diretório
    dir->createTime = time(0);
    dir->accessTime = time(0);
    dir->fileSize = 0x00;
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
	if (lseek(fs->fd, index + fs->metadata->root_begin, SEEK_SET) < 0) {
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