#include<elf.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/time.h>

#define INST_LENTH_STR 32
#define INST_LENTH_INT 4
#define SYS_FIX_LENGTH 27
#define SYS_SHIFT_LENGTH 5
#define SYS_MAP_SIZE 1024
#define BASE_MAP_SIZE 256

const unsigned int SYS_Map[SYS_MAP_SIZE] = {
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3574300192, 0, 0, 0, 0, 0, 0, 0, 3574299936, 0,         0, 0, 0, 0, 0, 0, 3574299680, 0, 0, 0,  0, 0, 0, 0, 3574299424, 0, 0, 0, 0, 0, 
0, 0, 3574299168, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 3574297888, 0, 0, 0, 0, 0, 0, 0,  3574297632, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 3575333920, 3575333888, 0, 0, 0, 0, 0,         0, 0, 0, 0, 3575333568, 0, 3575333504, 0, 0, 0,         3575333376, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 3575369792, 0, 0, 3575373824, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 3577218880, 3577218848,         3577218816, 0, 0, 0, 0, 0, 3577218624, 3577218592, 3577218560, 0,        0, 3577218464, 3577218432, 0, 0, 3577218336, 3577218304, 0, 0, 0,       0, 0, 0, 3577218080, 3577218048, 0, 0, 0, 0, 0,         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    3577217216, 3577217184, 0, 0, 0, 0, 3577217024, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 3577431072, 3577431040, 0, 0, 0,         0, 0, 0, 0, 0, 0, 3577430720, 0, 3577430656, 0,         0, 0, 3577430528, 0, 0, 0, 0, 3577413856, 0, 0,         0, 0, 0, 3577413664, 0, 0, 0, 0, 0, 0,   0, 0, 3577471168, 0, 3577466976, 3577466944, 3577471040, 0, 3577470976, 0,      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0
};

const unsigned long long BASE_Map[BASE_MAP_SIZE] = {
    18437773157604524032ull, 0, 18437773157600329730ull, 18437773157600329731ull, 18437773157604524064ull, 0, 18437773157600329762ull, 18437773157600329763ull, 18437773157604524096ull, 0, 
    18437773157600329794ull, 18437773157600329795ull, 18437773157604524128ull, 0, 18437773157600329826ull, 18437773157600329827ull, 18437773157604524160ull, 0, 18437773157600329858ull, 18437773157600329859ull,
    18437773157604524192ull, 0, 18437773157600329890ull, 18437773157600329891ull, 18437773157604524224ull, 0, 18437773157600329922ull, 18437773157600329923ull, 18437773157604524256ull, 0, 
    18437773157600329954ull, 18437773157600329955ull, 18437773157604532224ull, 0, 18437773157600337922ull, 18437773157600337923ull, 18437773157604532256ull, 0, 18437773157600337954ull, 18437773157600337955ull, 
    18437773157604532288ull, 0, 18437773157600337986ull, 18437773157600337987ull, 18437773157604532320ull, 0, 18437773157600338018ull, 18437773157600338019ull, 18437773157604532352ull, 0,
    18437773157600338050ull, 18437773157600338051ull, 18437773157604532384ull, 0, 18437773157600338082ull, 18437773157600338083ull, 18437773157604532416ull, 0, 18437773157600338114ull, 18437773157600338115ull,
    18437773157604532448ull, 0, 18437773157600338146ull, 18437773157600338147ul, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 18446727580313731199ull, 
    0, 0, 0, 18446727580313731231ull, 0, 0, 0, 0, 0, 0, 
    0, 18446727580313927903ull, 0, 0, 0, 18446727580313927935ull, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 18446744072988336255ull, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 18446744073015329760ull, 0, 0, 18446739674968820735ull,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     
    0, 0, 0, 0, 0, 0
};

char* itoa(unsigned int inst) {
    char* str_inst = malloc(INST_LENTH_STR);
    for (int i = 0; i < INST_LENTH_STR; i++) {
        str_inst[INST_LENTH_STR-1-i] = (inst & 1) + '0';
        inst >>= 1; 
    }
    return str_inst;
}
unsigned int range_atoi(int begin, int end, char* str) {
    unsigned int value = 0;
    for (int i = begin; i <= end; i++) {
        value += str[i]-'0';
        if (i != end) value <<= 1;
    }
    return value;
}
const long LOW32 = (1ul << 32) - 1;
const int LOW10 = (1 << 10) - 1;
const int LOW7 = (1 << 7) - 1;
const int LOW5 = (1 << 5) - 1;
const int LOW3 = 7;
const int LOW2 = 3;
int sys_hash(unsigned int code) {
    //inst[21:12]
    int high = (code >> 12) & LOW10;
    //inst[11:5]
    int low = (code >> 5) & LOW7;
    return high - low;
}

int base_hash_int(unsigned int code) {

    int index = 0;
    // inst[25:24]
    index += (code >> 24) & LOW2;
    index <<= 1;

    //inst[13]
    index += (code >> 13) & 1;
    index <<= 3;


    // //inst[7:5]
    index += (code >> 5) & LOW3;
    index <<= 2;

    // //inst[1:0]
    index += code & LOW2;

    return index;
}


bool sys_hash_varify(unsigned int cur_inst, const unsigned int map_inst) {
    if ((cur_inst) >> SYS_SHIFT_LENGTH == 
            (map_inst) >> SYS_SHIFT_LENGTH)
        return true;
    return false;
}

bool base_hash_verify(unsigned int cur_inst, const unsigned long long map_inst) {

    // map_inst_high32: mask
    // map_inst_low32: bad inst
    if ((cur_inst & (map_inst >> INST_LENTH_STR)) == (map_inst & LOW32)){
        return true;
    }
    return false;
}

bool is_privilege(unsigned int inst){
    //1101010100x01
    //1101010100x1
    if (((inst >> 22) & LOW10) == 852 // 1101010100
            && ((((inst >> 19) & LOW2) == 1) //x01
            || ((inst >> 20) & 1))) //x1
            return true; 
    return false;
}
long long counter;
void text_scan(int *sh_data, long long size) {
    unsigned int *inst;
    register int inst_int;
    // int inst_int;
    long long scaned_size = 0;
    for (inst = sh_data; scaned_size < size; inst++, scaned_size += 4) {
        inst_int = *inst;
        // if ((inst_int >> 27) & 1){counter++; continue;}// 4 <= inst[27:25] <= 7
        // if (((inst_int >> 26) & LOW3) == 4) {counter++; continue;} // inst[28:26] = 100
        // if (((inst_int >> 26) & LOW3) == 5 && ((inst_int >> 29) & LOW3 != 6)) {counter ++; continue;} // inst[28:26] = 101, inst[31:29]!=110
        
        /* system, MSR/MRS instr*/
        if (is_privilege(inst_int)) {
            int index = sys_hash(inst_int);
            if (SYS_Map[index] && sys_hash_varify(inst_int, SYS_Map[index])){
            }
            else 
                printf("a bad sys inst!\n");

        } 
        /* base, SIMD&FP instr*/
        else {
            int index = base_hash_int(inst_int);
            if (BASE_Map[index] && base_hash_verify(inst_int, BASE_Map[index]))
                printf("index = %d, a bad base inst!\n", index);   

        }
    }
}   
struct timeval base_time;
void set_base_time() {
    gettimeofday(&base_time, NULL);
}
double get_cur_time() {
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    return cur_time.tv_sec - base_time.tv_sec + ((double) cur_time.tv_usec - (double) base_time.tv_usec) / 1000000.0;
}
int main(int argc, char **argv){
    
    const char* file = argv[1];
    FILE* fp = fopen(file,"rb");

    Elf64_Ehdr* elf_hdr = (Elf64_Ehdr*)malloc(sizeof(Elf64_Ehdr));
    memset(elf_hdr, 0, sizeof(Elf64_Ehdr));
    if (!fread(elf_hdr, sizeof(Elf64_Ehdr), 1, fp)) printf("read error\n");

    int shdrs_size = sizeof(Elf64_Shdr) * elf_hdr->e_shnum;
    Elf64_Shdr* shdrs = (Elf64_Shdr*)malloc(shdrs_size);
    memset(shdrs, 0, shdrs_size);
    fseek(fp, elf_hdr->e_shoff, SEEK_SET);
    if (!fread(shdrs, shdrs_size, 1, fp)) printf("read error\n");
    Elf64_Shdr* shdr = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr));

    int i;
   
    set_base_time();
    for (int round = 0; round < 100; round++) {
        for (i = 0, shdr = shdrs; i < elf_hdr->e_shnum; i++, shdr++) {
            if (!(shdr->sh_flags & SHF_EXECINSTR)) continue;
            int* sh_data = malloc(shdr->sh_size);
            fseek(fp, shdr->sh_offset, SEEK_SET);
            if (!fread(sh_data, shdr->sh_size, 1, fp)) printf("read error\n");
            text_scan(sh_data, shdr->sh_size);
            free(sh_data);
        }
        // printf("finish one round\n");
    }
    printf("cost time = %fs\n", get_cur_time());
    // printf("sys_inst = %llu\n", sys_inst);
    // printf("base_inst = %llu\n", base_inst);
    // printf("counter = %llu\n", counter);
    // printf("inst_num = %llu\n", inst_num);
}