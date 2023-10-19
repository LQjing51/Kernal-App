#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define LEN (8*1024)
typedef long long ll;
int test_swap() {
    FILE* fp;
    fp = freopen("test_out.txt", "w", stdout);
    sleep(1);                       //sleep 1 second
    ll unit = 1024ll * 1024 * 1024;
    //  ll unit = 1024ll;
    ll sz = unit * (1.1);
    char *buf = malloc(sz);
    ll i = 0;
    int flag = 0;
    int count = 0;
    while (1) {
        if (flag) {
            char c = buf[i];
            if (c != (char) i) {
                if (++count <= 1000)
                    printf("content error! %d %lld\n", (int) c, i);
            }
            if (!(i % (1024 * 1024))) printf("read success for i = %lld\n", i), fflush(stdout);
            // if (i + 16 >= sz) {
            //     // fflush(fp);
            //     // freopen("/dev/tty", "w", stdout);
            //     // fclose(fp);
            //     return 0;
            // }
        } else {
            buf[i] = (char) i;
            if (!(i % (1024 * 1024))) printf("write success for i = %lld\n", i), fflush(stdout);
        }
        i += 16;
        if (i >= sz) i = 0, flag = 1;
    }

}
void padding(void) {
    int a; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
}
void foo(void) {
    int j = 0;
    j++;
    printf("foo: j = %d\n", j);
}
void padding2(void) {
    //************padding for this page************
    int a;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1; 
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;
    a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;a = 1;

}
void test_change_code(void) {
    unsigned char *aligned_foo_addr;
    unsigned char *foo_addr = (unsigned char*)foo;
    printf("addr = %lx\n", (unsigned long)foo_addr);
    int page_size = getpagesize();
    aligned_foo_addr = foo_addr - (unsigned long)foo_addr % page_size;
    if (mprotect(aligned_foo_addr, page_size, PROT_READ | PROT_WRITE | PROT_EXEC) == -1){
        printf("mprotect failed\n");
        return;
    }

    foo(); // exec

    unsigned char *inst = (unsigned char*)foo_addr+17;
    printf("byte = %x%x\n", *(inst+1),*inst);//read

    *inst = 0x8; //write: clear rdonly, set write|uxn
    printf("byte = %x%x\n", *(inst+1),*inst);

    foo();//exec: scan, set rdonly, clear write|uxn
    return;
}

void test_shared_write(void){
    int fd;
    void* mm; 

    const char* str = "hi, this is a mmap file";
    fd = open("mmap_file.txt", O_RDWR|O_CREAT, S_IRWXU|S_IRWXG);
    printf("fd = %d\n", fd);
    if (fd == -1) {
        printf("open file failed\n");
        printf("%s\n", strerror(errno));
        return;
    }
    ftruncate(fd, strlen(str)); 

    mm = mmap(NULL, 1024, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if ((unsigned long)mm == -1) {
        printf("mmap failed!\n");
        printf("%s\n", strerror(errno));
        return;
    } 


    printf("mmap addr: %lx\n", (unsigned long)mm);
    
    printf("write in: %s, size = %ld\n", str, strlen(str));
    memcpy(mm, str, strlen(str));

    printf("read out: %s\n", (char*)mm);
    
    // printf("write in: %s, size = %ld\n", str, strlen(str));
    // memcpy(mm, str, strlen(str));

    close(fd);
    munmap(mm, 1024);

}

void test_mprotect(void) {
    char* addr;
    addr = mmap(NULL, LEN, PROT_READ, MAP_PRIVATE|MAP_ANON, -1, 0);
    if (addr == MAP_FAILED) {
        printf("mmap error\n");
        return;
    }

    if (mprotect(addr, LEN, PROT_READ|PROT_WRITE) == -1)
        printf("mprotect1 error\n");
    
    if (mprotect(addr, LEN, PROT_EXEC) == -1)
       printf("mprotect2 error\n");

    if (mprotect(addr, LEN, PROT_WRITE|PROT_EXEC) == -1)
       printf("mprotect3 error\n");
}


void test_mmap_WX(void){
    char *addr;
    unsigned long long br_addr;
    addr = mmap(NULL, 4*1024, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
    if (addr == MAP_FAILED) {
        printf("mmap error\n");
        return;
    }
    printf("finish mmap\n");
    br_addr = (unsigned long long)addr;
    
    if (mprotect((void*)br_addr, 4*1024, PROT_READ | PROT_WRITE | PROT_EXEC) == -1){
        printf("mprotect failed\n");
        return;
    }
    printf("finish mprotect\n");

    printf("mmap addr = %llx\n", br_addr);
    printf("begin write mmap page\n");

    //ret
    *(addr) = 0xc0;
    *(addr+1) = 0x03;
    *(addr+2) = 0x5f;
    *(addr+3) = 0xd6;

    printf("begin br\n");
    asm volatile(
        "MOV x29, %0\n\t"
		"BLR x29\n\t"
        :
		:"r"(addr) 
		:"memory","cc"	
	);

    printf("success return\n");
    return;
}

int main() {
    printf("begin test!\n");
    while(1);
    // test_change_code();
    // printf("-----finish test 1-------\n");
    test_mmap_WX();
    printf("-----finish test 2-------\n");
    
    printf("finish test!\n");
    
    return 0;
}
