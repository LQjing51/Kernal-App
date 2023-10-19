#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

void test_shared_memory(void) {

    int shmid = shmget((key_t)1234, 4*1024, IPC_CREAT|0777); //若找到该key对应的共享内存，则该处的flag必须和该共享内存之前设定的flag兼容。或者直接用0标识找已经存在的共享内存。
    if (shmid == -1) {
        printf("shm_exec: shmget failed\n"); 
        printf("%s\n", strerror(errno));
        return;
    }

    char *shm = shmat(shmid, 0, SHM_RDONLY|SHM_EXEC);
    if (shm == (char*)-1) {
        printf("shm_exec: shmat failed\n");
        printf("%s\n", strerror(errno));
        return;
    }

    printf("shm_exec: vaddr: %llx\n", (unsigned long long)shm);

    while(*(shm+4) != '1') { } 
    printf("shm_exec: begin br\n");

    printf("%x,%x,%x,%x\n", *(shm), *(shm+1), *(shm+2), *(shm+3));
    unsigned long long addr = (unsigned long long)(shm);
    asm volatile(
        "MOV x27, %0\n\t"
        "BLR x27\n\t"
        :
        :"r"(addr) 
        :"memory","cc"	
    );
    printf("shm_exec: success return\n");

    if (shmdt(shm) == -1) {
        printf("shm_exec: shmdt failed\n");
        return;
    }
    return;
}
int main() {
    printf("shm_exec: begin test!\n");
    test_shared_memory();
    printf("shm_exec: -----finish test 3-------\n");
    // while(1){
    //     ;
    // }
        
    return 0;
}
