#include "cluster.h"

int open_disk(Cluster_t* cl, char *path){

    int fd;
	struct stat st;

    if ((fd = open(path, O_CREAT, 0644)) < 0) {
		perror("open");
		return -1;
	}

	if (fstat(fd, &st)) {
		perror("fstat");
		return -1;
	}

	if (st.st_size % CLUSTER_SIZE != 0) {
		perror("block");
		return -1;
	}

	cl->fd = fd;
	cl->cluster_index = st.st_size / CLUSTER_SIZE;

}

int close_disk(Cluster_t* cl){
    close(cl->fd);
    cl->fd = -1;

    return 1;
}

int cluster_read(Cluster_t* cl, size_t index, void* buf){

    // move o descritor até o indice do cluster passado como parametro
	if (lseek(cl->fd, index * CLUSTER_SIZE, SEEK_SET) < 0) {
		printf("deu ruim");
		return 0;
	}

	// lê o conteudo do cluster no buffer
	if (read(cl->fd, buf, CLUSTER_SIZE) < 0) {
		printf("deu ruim");
		return 0;
	}

    return 1;
}

int cluster_write(Cluster_t* cl, size_t index, const void* buf){
    
    // move o descritor até o indice do cluster passado como parametro
	if (lseek(cl->fd, index * CLUSTER_SIZE, SEEK_SET) < 0) {
		printf("deu ruim");
		return 0;
	}

	// escreve o conteudo de buffer no cluster
	if (write(cl->fd, buf, CLUSTER_SIZE) < 0) {
		printf("deu ruim");
		return 0;
	}

	return 1;

}