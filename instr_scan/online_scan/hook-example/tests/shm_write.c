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

void test_shared_memory(void) {

    int shmid = shmget((key_t)1234, 4*1024, IPC_CREAT|0777);
    if (shmid == -1) {
        printf("shm_write: shmget failed\n"); 
        return;
    }

    char *shm = shmat(shmid, 0, 0);
    if (shm == (char*)-1) {
        printf("shm_write: shmat failed\n");
        return;
    }

    printf("shm_write: vaddr: %llx\n", (unsigned long long)shm);

    //ret
    *(shm) = 0xc0;
    *(shm+1) = 0x03;
    *(shm+2) = 0x5f;
    *(shm+3) = 0xd6;

    //write finish flag
    *(shm+4) = '1';

    printf("shm_write: finish write ret\n");

    // while(1);
    sleep(2);
    if (shmdt(shm) == -1) {
        printf("shm_write: shmdt failed\n");
        return;
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        printf("shm_write: shmctl failed\n");
        return;
    }
    return;
}
int main() {
    printf("shm_write: begin test!\n");
    test_shared_memory();
    printf("shm_write: -----finish test 3-------\n");
    // while(1){
    //     ;
    // }
        
    return 0;
}
