
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
    const char str[6] = {0xc0, 0x03, 0x5f, 0xd6, '1', 0};

    // int fd = shm_open("shm_file", O_CREAT|O_RDWR, 0777);
    int fd = open("mydir_file.txt", O_CREAT|O_RDWR, 0777);
    if (fd < 0) {
        printf("shm_open failed\n");
        printf("%s\n", strerror(errno));
        return;
    }

    ftruncate(fd, 1024);

    // char *mm = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    // if (!mm) {
    //     printf("mmap failed!\n");
    //     close(fd);
    // }

    // printf("mmap addr: %lx\n", (unsigned long)mm);
    
    // memcpy(mm, str, strlen(str));

    write(fd, str, strlen(str));

    printf("shm_write: finish write ret\n");
    
    sleep(3);
    
    close(fd);
    // munmap(mm, 1024);
    // shm_unlink("shm_file");
    return;
}
int main() {
    printf("shm_write: begin test!\n");
    test_shared_memory();
    printf("shm_write: -----finish test 1-------\n");
    
    return 0;
}
