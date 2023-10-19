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
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   223393762, 0, 0, 0, 0, 0, 0, 0, 223393746, 0,   0, 0, 0, 0, 0, 0, 223393730, 0, 0, 0,   0, 0, 0, 0, 223393714, 0, 0, 0, 0, 0, 
0, 0, 223393698, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 223393618, 0, 0, 0, 0, 0, 0, 0,    223393602, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 223458338, 223458336, 0, 0, 0, 
0, 0, 0, 223458370, 223458368, 0, 0, 0, 0, 0,   0, 0, 0, 0, 223458348, 0, 223458344, 0, 0, 0,   223458336, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 223460612, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 223576181, 223576179,   223576177, 0, 0, 0, 0, 0, 223576165, 223576163, 223576161, 0,    0, 223576155, 223576153, 0, 0, 223576147, 223576145, 0, 0, 0,   0, 0, 0, 223576131, 223576129, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   223576077, 223576075, 0, 0, 0, 0, 223576065, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 223589410, 223589408, 0,   0, 0, 0, 0, 0, 223589442, 223589440, 0, 0, 0,   0, 0, 0, 0, 0, 0, 223589420, 0, 223589416, 0,   0, 0, 223589408, 0, 0, 0, 0, 223588366, 0, 0,    0, 0, 0, 223588355, 0, 0, 0, 0, 0, 0,   0, 0, 223591948, 0, 223591686, 223591684, 223591940, 0, 223591936, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0
};

const unsigned long long BASE_Map[BASE_MAP_SIZE] = {
72566299426816, 0, 72566291038212, 72566291038214, 72566299426880, 0, 72566291038276, 72566291038278, 72566299426944, 0, 
72566291038340, 72566291038342, 72566299427008, 0, 72566291038404, 72566291038406, 72566299427072, 0, 72566291038468, 72566291038470, 
72566299427136, 0, 72566291038532, 72566291038534, 72566299427200, 0, 72566291038596, 72566291038598, 72566299427264, 0, 
72566291038660, 72566291038662, 72566299443200, 0, 72566291054596, 72566291054598, 72566299443264, 0, 72566291054660, 72566291054662, 
72566299443328, 0, 72566291054724, 72566291054726, 72566299443392, 0, 72566291054788, 72566291054790, 72566299443456, 0, 
72566291054852, 72566291054854, 72566299443520, 0, 72566291054916, 72566291054918, 72566299443584, 0, 72566291054980, 72566291054982, 
72566299443648, 0, 72566291055044, 72566291055046, 0, 0, 0, 17981411717840958, 0, 0, 
0, 17981411717841022, 0, 0, 0, 17981411717841086, 0, 0, 0, 17981411717841150, 
0, 0, 0, 17981411717841214, 0, 0, 0, 17981411717841278, 0, 0, 
0, 17981411718234558, 0, 0, 0, 17981411718234622, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 18014397067051263, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
0, 0, 0, 0, 0, 0, 18014397121038272, 0, 0, 18014397121189886, 
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
    char* str_inst = malloc(INST_LENTH_STR+1);
    for (int i = 0; i < INST_LENTH_STR; i++) {
        str_inst[INST_LENTH_STR-1-i] = (inst & 1) + '0';
        inst >>= 1; 
    }
    str_inst[INST_LENTH_STR] = '\0';
    return str_inst;
}
char* itoa_ignore_low(unsigned int inst) {
    char* str_inst = malloc(SYS_FIX_LENGTH+1);
    int i;
    int ignore_low_inst;
    
    ignore_low_inst = (inst >>= (SYS_SHIFT_LENGTH));
    for (i = 0; i < SYS_FIX_LENGTH; i++) {
        str_inst[INST_LENTH_STR-6-i] = (ignore_low_inst & 1) + '0';
        ignore_low_inst >>= 1;
    }
    str_inst[SYS_FIX_LENGTH] = '\0';
    return str_inst;
}

unsigned long long range_atoi(int begin, int end, char* str) {
    unsigned long long value = 0;
    for (int i = begin; i <= end; i++) {
        value += str[i]-'0';
        if (i != end) value <<= 1;
    }
    return value;
}
const long LOW32 = (1ul << 32) - 1;
const int LOW16 = (1ul << 16) - 1;
const int LOW12 = (1ul << 12) - 1;
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


bool sys_hash_verify(unsigned int cur_inst, const unsigned int map_inst) {
    if ((cur_inst) >> SYS_SHIFT_LENGTH == 
            (map_inst) >> 1)
        return true;
    return false;
}
unsigned long long sys_inst = 0;
unsigned long long base_inst = 0;
unsigned long long counter = 0;
bool base_hash_verify(unsigned int cur_inst, const unsigned long long map_inst) {
    // counter++;

    if ((cur_inst & ((map_inst >> (INST_LENTH_STR+1)) | 0b11111111111000000000000000000000u)) == ((map_inst>>1) & LOW32)){
        // printf("%llx\n", map_inst >> INST_LENTH_STR);
        // printf("%llx\n", map_inst & LOW32);
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
void text_scan(int *sh_data, long long size) {
    unsigned int *inst;
    long long scaned_size = 0;
    unsigned int brk;
    for (inst = (unsigned int*)sh_data; scaned_size < size; inst++, scaned_size += 4) {
        /* system, MSR/MRS instr*/
        if (is_privilege(*inst)) {
            sys_inst ++;
            int index = sys_hash(*inst);
            if (SYS_Map[index] && sys_hash_verify(*inst, SYS_Map[index])){
                if (SYS_Map[index] & 1){
                    printf("sys:trap, %s\n", itoa_ignore_low(*inst));
                    brk = (0b11010100001u << 21) + ((((*inst)&LOW12) | 0b1000000000000u)<<5);
                    //*inst = brk;
                }else{
                    // INFO("sys:do nothing\n");
                }
            }else{
                printf("sys:filter, %s\n", itoa_ignore_low(*inst));
                brk = (0b11010100001u << 21) + (0b1111111111111u<<5);//imm16 = 111111111111: filter
                //*inst = brk;
            }
	    }
        /* base, SIMD&FP instr*/
        else {
            base_inst++;
            int index = base_hash_int(*inst);
            if (BASE_Map[index] && base_hash_verify(*inst, BASE_Map[index])){
                if (BASE_Map[index] & 1) {
                    printf("base:trap, %s\n", itoa_ignore_low(*inst));
                    brk = (0b11010100001u << 21) + ((((*inst)&LOW12) | 0b1000000000000u)<<5);////imm16 = 1:inst[12:0]
                    //*inst = brk;
                }else {
                    printf("base:filter, %s\n", itoa_ignore_low(*inst));
                    brk = (0b11010100001u << 21) + (0b1111111111111u<<5);//imm16 = 111111111111: filter
                    //*inst = brk;
                }
            }
        }
    }
    // for (inst = (unsigned int*)sh_data; scaned_size < size; inst++, scaned_size += 4) {
    //     base_inst++;
    //     if (((*inst >> 21) == 0b11010100001u) && (((*inst) & LOW5) == 0)) {
    //         printf("has a brk,imm: %x\n",((*inst)>>5)&LOW16);
    //     }
    // }

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
    fread(elf_hdr, sizeof(Elf64_Ehdr), 1, fp);
    
    // dump_basic_info(elf_hdr);
    // char* str = "1111111111111111111100001111111111010101000000000100000010111111";
    // printf("value = %llu\n", range_atoi(0, 63, str));


    int shdrs_size = sizeof(Elf64_Shdr) * elf_hdr->e_shnum;
    Elf64_Shdr* shdrs = (Elf64_Shdr*)malloc(shdrs_size);
    memset(shdrs, 0, shdrs_size);
    fseek(fp, elf_hdr->e_shoff, SEEK_SET);
    fread(shdrs, shdrs_size, 1, fp);
    Elf64_Shdr* shdr = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr));

    int i;
   
    set_base_time();
    for (int round = 0; round < 1000; round++) {
        for (i = 0, shdr = shdrs; i < elf_hdr->e_shnum; i++, shdr++) {
            if (!(shdr->sh_flags & SHF_EXECINSTR)) continue;
            // dump_phdr_info(phdr);
            
            int* sh_data = malloc(shdr->sh_size);
            fseek(fp, shdr->sh_offset, SEEK_SET);
            fread(sh_data, shdr->sh_size, 1, fp);
            text_scan(sh_data, shdr->sh_size);
            free(sh_data);
        }
        // printf("finish one round\n");
    }
    double time = get_cur_time();
    printf("cost time = %fs\n", time);
    printf("sys_inst = %llu\n", sys_inst/1000);
    printf("base_inst = %llu\n", base_inst/1000);
    printf("total inst = %llu\n", (sys_inst+base_inst)/1000);
    printf("perinst = %fs\n", (time/(double)(sys_inst+base_inst))*1000000000);
}