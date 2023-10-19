
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

void test_shared_memory(void) {
    
    // int fd = open("shm_file", O_CREAT|O_RDWR, 0777);
    int fd = open("mydir_file.txt", O_CREAT|O_RDONLY, 0777);
    if (fd < 0) {
        printf("shm_open failed\n");
        printf("%s\n", strerror(errno));
        return;
    }

    ftruncate(fd, 1024);

    char *mm = mmap(NULL, 1024, PROT_READ|PROT_EXEC, MAP_SHARED, fd, 0);
    if (!mm) {
        printf("mmap failed!\n");
        close(fd);
    }

    printf("mmap addr: %lx\n", (unsigned long)mm);
    
    while((*(mm+4)) != '1') { } 
    printf("shm_exec: begin br\n");

    printf("%x,%x,%x,%x\n", *(mm), *(mm+1), *(mm+2), *(mm+3));
    unsigned long long addr = (unsigned long long)(mm);
    asm volatile(
        "MOV x27, %0\n\t"
        "BLR x27\n\t"
        :
        :"r"(addr) 
        :"memory","cc"	
    );
    printf("shm_exec: success return\n");

    close(fd);
    munmap(mm, 1024);
    return;
}
int main() {
    printf("shm_exec: begin test!\n");
    test_shared_memory();
    printf("shm_exec: -----finish test 1-------\n");
    return 0;
}
