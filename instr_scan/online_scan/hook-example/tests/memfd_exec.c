#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sys/types.h>          
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <sys/un.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>

static int recv_fd(int socket,int n){
    int fd;
    struct msghdr msg={0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(n*sizeof(int))],data;
    memset(buf,0,sizeof(buf));
    struct iovec io={.iov_base=&data,.iov_len=1};

    msg.msg_iov=&io;
    msg.msg_iovlen=1;
    msg.msg_control=buf;
    msg.msg_controllen=sizeof(buf);
    if(recvmsg(socket,&msg,0)<0){
        perror("[Failed to receive message]");
        return -1;
    }
    cmsg=CMSG_FIRSTHDR(&msg);
    memcpy(&fd,(int*)CMSG_DATA(cmsg),n*sizeof(int));
    return fd;
}
int main(int argc,char **argv){

    int rfd,cfd,fd;
    struct sockaddr_un addr;

    rfd=socket(AF_UNIX,SOCK_STREAM,0);
    if(rfd<0){
        perror("[socket error]");
        return -1;
    }
    printf("finish socket\n");
    if(unlink("1.socket")==-1 && errno !=ENOENT){
        perror("[Removing socket file failed]");
        return -1;
    }
    printf("finish unlink\n");

    memset(&addr,0,sizeof(struct sockaddr_un));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path,"1.socket",sizeof(addr.sun_path)-1);

    if(bind(rfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un))==-1){
        perror("[Failed to bind socket]");
        return -1;
    }
    printf("finish bind\n");
    if(listen(rfd,5)==-1){
        perror("[Failed to listen socket]");
        return -1;
    }
    printf("listening......\n");
    cfd=accept(rfd,NULL,NULL);
    if(cfd==-1){
        perror("[Failed to accept incoming connection]");
    }
    printf("finish accept\n");
    fd=recv_fd(cfd,1);
    if(fd<0){
        return -1;
    }

    printf("fd:%d\n",fd);
    
    char *shm=mmap(NULL,1024,PROT_EXEC|PROT_READ,MAP_SHARED,fd,0);
    if(shm==NULL){
        perror("[mmap fd error]");
        return -1;
    }
    
    while((*(shm+4)) != '1') { } 
    printf("shm_exec: begin br\n");

    printf("%x,%x,%x,%x\n", *(shm), *(shm+1), *(shm+2), *(shm+3));
    unsigned long long mm = (unsigned long long)(shm);
    asm volatile(
        "MOV x27, %0\n\t"
        "BLR x27\n\t"
        :
        :"r"(mm) 
        :"memory","cc"	
    );
    printf("shm_exec: success return\n");

    close(fd);
    close(cfd);
    return 0;
}
