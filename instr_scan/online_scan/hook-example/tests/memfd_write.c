#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h> //sockaddr_un头文件
#include <sys/mman.h>
#include <time.h>
#include <sys/mman.h>
#include <linux/memfd.h>
#include <sys/syscall.h>
#include <errno.h>


//发送fd给接收函数
static int send_fd(int socket,int *fds,int n){
    struct msghdr msg={0};
    struct cmsghdr *cmsg;
    char buf[CMSG_SPACE(n*sizeof(int))],data;
    memset(buf,'\0',sizeof(buf));
    struct iovec io={.iov_base=&data,.iov_len=1};

    msg.msg_iov=&io;
    msg.msg_iovlen=1;
    msg.msg_control=buf;
    msg.msg_controllen=sizeof(buf);

    cmsg=CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level=SOL_SOCKET;
    cmsg->cmsg_type=SCM_RIGHTS;
    cmsg->cmsg_len=CMSG_LEN(n*sizeof(int));

    memcpy((int *)CMSG_DATA(cmsg),fds,n*sizeof(int));

    if(sendmsg(socket,&msg,0)==-1){
        perror("[failed to send message]");
        return -1;
    }
    return 0;
}


int main(int argc ,char** argv){
    srand((unsigned int)time(NULL));
    int sfd,fd;
    struct sockaddr_un addr;
    //获取套接字
    sfd=socket(AF_UNIX,SOCK_STREAM,0);
    if(sfd==-1){
        perror("[Failed to create socket]");
        return -1;
    }
    memset(&addr,'\0',sizeof(struct sockaddr_un));
    addr.sun_family=AF_UNIX;
    strncpy(addr.sun_path,"1.socket",sizeof(addr.sun_path)-1);

    //获取文件描述符
    fd = memfd_create("shm", 0);
    if(fd<0){
        perror("[syscall memfd_create error]");
    }
    printf("fd:%d\n",fd);
    //将文件扩大
    ftruncate(fd,0x1024);

    //初始化内存空间

    //连接接收数据端
    if(connect(sfd,(struct sockaddr *)&addr,sizeof(struct sockaddr_un))==-1){
        perror("[connect error]");
        return -1;
    }

    if(send_fd(sfd,&fd,1)==-1){
        exit(-1);
    }
    printf("OK\n");
    //映射fd
    char* shm = mmap(NULL,1024,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(shm == NULL){
        perror("[mmap error]");
        return -1;
    }
    *(shm) = 0xc0;
    *(shm+1) = 0x03;
    *(shm+2) = 0x5f;
    *(shm+3) = 0xd6;

    //write finish flag
    *(shm+4) = '1';

    sleep(3);
    printf("finish write\n");
    return 0;
}
