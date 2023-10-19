#include<elf.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<sys/time.h>

#define INST_LENTH_STR 32
#define INST_LENTH_INT 4
#define SYS_FIX_LENGTH 27
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

const char* BASE_Map[BASE_MAP_SIZE] = {
    "11010100010xxxxxxx0xxxxx00000000", "", "11010100000xxxxxxx0xxxxx00000010", "11010100000xxxxxxx0xxxxx00000011", "11010100010xxxxxxx0xxxxx00100000", "", "11010100000xxxxxxx0xxxxx00100010", "11010100000xxxxxxx0xxxxx00100011", "11010100010xxxxxxx0xxxxx01000000", "",
    "11010100000xxxxxxx0xxxxx01000010", "11010100000xxxxxxx0xxxxx01000011", "11010100010xxxxxxx0xxxxx01100000", "", "11010100000xxxxxxx0xxxxx01100010", "11010100000xxxxxxx0xxxxx01100011", "11010100010xxxxxxx0xxxxx10000000", "", "11010100000xxxxxxx0xxxxx10000010", "11010100000xxxxxxx0xxxxx10000011",
    "11010100010xxxxxxx0xxxxx10100000", "", "11010100000xxxxxxx0xxxxx10100010", "11010100000xxxxxxx0xxxxx10100011", "11010100010xxxxxxx0xxxxx11000000", "", "11010100000xxxxxxx0xxxxx11000010", "11010100000xxxxxxx0xxxxx11000011", "11010100010xxxxxxx0xxxxx11100000", "",     
    "11010100000xxxxxxx0xxxxx11100010", "11010100000xxxxxxx0xxxxx11100011", "11010100010xxxxxxx1xxxxx00000000", "", "11010100000xxxxxxx1xxxxx00000010", "11010100000xxxxxxx1xxxxx00000011", "11010100010xxxxxxx1xxxxx00100000", "", "11010100000xxxxxxx1xxxxx00100010", "11010100000xxxxxxx1xxxxx00100011",     
    "11010100010xxxxxxx1xxxxx01000000", "", "11010100000xxxxxxx1xxxxx01000010", "11010100000xxxxxxx1xxxxx01000011", "11010100010xxxxxxx1xxxxx01100000", "", "11010100000xxxxxxx1xxxxx01100010", "11010100000xxxxxxx1xxxxx01100011", "11010100010xxxxxxx1xxxxx10000000", "",     
    "11010100000xxxxxxx1xxxxx10000010", "11010100000xxxxxxx1xxxxx10000011", "11010100010xxxxxxx1xxxxx10100000", "", "11010100000xxxxxxx1xxxxx10100010", "11010100000xxxxxxx1xxxxx10100011", "11010100010xxxxxxx1xxxxx11000000", "", "11010100000xxxxxxx1xxxxx11000010", "11010100000xxxxxxx1xxxxx11000011",     
    "11010100010xxxxxxx1xxxxx11100000", "", "11010100000xxxxxxx1xxxxx11100010", "11010100000xxxxxxx1xxxxx11100011", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "11010101000000000100xxxx01111111",     
    "", "", "", "11010101000000000100xxxx10011111", "", "", "", "", "", "",     
    "", "11010101000000110100xxxx11011111", "", "", "", "11010101000000110100xxxx11111111", "", "", "", "",     

    "", "", "", "", "", "", "", "", "", "",     
    "", "11010101000000110010000001111111", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "11010110100111110000001111100000", "", "", "110101101001111100001x1111111111",     
    "", "", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "",     
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "",
    "", "", "", "", "", ""     
   
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

int base_hash_str(char* str) {

    int index = 0;
    index += str[6]-'0'; index <<= 1;      
    index += str[7]-'0'; index <<= 1;
    index += str[18]-'0'; index <<= 1;
    index += str[24]-'0'; index <<= 1;
    index += str[25]-'0'; index <<= 1;
    index += str[26]-'0'; index <<= 1;
    index += str[30]-'0'; index <<= 1;
    index += str[31]-'0'; 

    return index;
}
int base_hash_int(unsigned int code) {

    int index = 0;
    //inst[25:24]
    index += (code >> 24) & LOW2;
    index <<= 1;

    //inst[13]
    index += (code >> 13) & 1;
    index <<= 3;

    //inst[7:5]
    index += (code >> 5) & LOW3;
    index <<= 2;

    //inst[1:0]
    index += code & LOW2;

    return index;
}


void dump_basic_info(Elf64_Ehdr * elf_hdr) {
    printf("**********dump elf header info**********\n");
    printf("Magic:\t");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", elf_hdr->e_ident[i]);
    }
    printf("\n");

    printf("Type:\t");
    switch (elf_hdr->e_type)
    {
    case 0:
        printf("No file type\n");
        break;
    case 1:
        printf("Relocatable file\n");
        break;
    case 2:
        printf("Executable file\n");
        break;
    case 3:
        printf("Shared object file\n");
        break;
    case 4:
        printf("Core file\n");
        break;
    default:
        printf("error\n");
        break;
    }
    printf("\n");
}
void dump_phdr_info(Elf64_Phdr* phdr) {
    printf("**********dump program header info**********\n");
    printf("Type:\t");
    switch (phdr->p_type){
        case PT_NULL:	printf("Program header table entry unused\n"); break;
        case PT_LOAD:   printf("Loadable program segment\n"); break;
        case PT_DYNAMIC:	printf("Dynamic linking information\n"); break;
        case PT_INTERP:	printf("Program interpreter\n"); break;
        case PT_NOTE:	printf("Auxiliary information\n"); break;
        case PT_SHLIB:   printf("Reserved\n"); break;
        case PT_PHDR:	printf("Entry for header table itself\n"); break;
        case PT_TLS	:	printf("Thread-local storage segment\n"); break;
        default: printf("else type\n"); break;
    }
    printf("Flags:\t");
    if (phdr->p_flags & PF_X) printf("PF_X");
    if (phdr->p_flags & PF_W) printf(" PF_W");
    if (phdr->p_flags & PF_R) printf(" PF_R");
    printf("\n");
    printf("FileSize:\t%ld\n",phdr->p_filesz);
    printf("\n");
}

bool sys_hash_varify(unsigned int cur_inst, const unsigned int map_inst, int cmp_length) {
    if ((cur_inst) >> (INST_LENTH_STR-cmp_length) == 
            (map_inst) >> (INST_LENTH_STR-cmp_length))
        return true;

    return false;
}

bool base_hash_varify(unsigned int cur_inst, const char* map_inst) {
    char* cur_inst_str = itoa(cur_inst);
    for (int i = 0; i < INST_LENTH_STR; i++) {
        if (map_inst[i] == 'x') continue;
    
        if(cur_inst_str[i] != map_inst[i]) return false;
    }
    return true;
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
void text_scan(int *ph_data, long long size) {
    unsigned int *inst;
    long long scaned_size = 0;
    for (inst = ph_data; scaned_size < size; inst++, scaned_size += 4) {
        /* system, MSR/MRS instr*/
        if (is_privilege(*inst)) {
            if ((*inst >> 5) == 0b110101010000101101111110001) continue;
            if ((*inst >> 5) == 0b110101010000101101111010001) continue;
            if ((*inst >> 5) == 0b110101010000101101111101001) continue;
            if ((*inst >> 5) == 0b110101010000101101111100001) continue;
            if ((*inst >> 5) == 0b110101010000101101111011001) continue;
            if ((*inst >> 5) == 0b110101010000101101110100001) continue;
            if ((*inst >> 5) == 0b110101010000101101110101001) continue;
            if ((*inst >> 5) == 0b110101010011101101000100000) continue;
            if ((*inst >> 5) == 0b110101010011101101000100001) continue;
            if ((*inst >> 5) == 0b110101010011101100000000111) continue;
            if ((*inst >> 5) == 0b110101010011101111010000010) continue;
            if ((*inst >> 5) == 0b110101010011101111010000011) continue;
            if ((*inst >> 5) == 0b110101010011101101000010100) continue;
            if ((*inst >> 5) == 0b110101010011101101000010000) continue;
            if ((*inst >> 5) == 0b110101010011101101000010110) continue;
            if ((*inst >> 5) == 0b110101010011101111100000000) continue;
            if ((*inst >> 5) == 0b110101010011101111100000110) continue;
            if ((*inst >> 5) == 0b110101010011101111100000010) continue;
            if ((*inst >> 5) == 0b110101010011101100000000001) continue;
            if ((*inst >> 5) == 0b110101010011100000000101100) continue;
            if ((*inst >> 5) == 0b110101010011100000000101101) continue;
            if ((*inst >> 5) == 0b110101010011100000000101000) continue;
            if ((*inst >> 5) == 0b110101010011100000000101001) continue;
            if ((*inst >> 5) == 0b110101010011100000000110000) continue;
            if ((*inst >> 5) == 0b110101010011100000000110001) continue;
            if ((*inst >> 5) == 0b110101010011100000000110010) continue;
            if ((*inst >> 5) == 0b110101010011100000000111000) continue;
            if ((*inst >> 5) == 0b110101010011100000000111001) continue;
            if ((*inst >> 5) == 0b110101010011100000000111010) continue;
            if ((*inst >> 5) == 0b110101010011100000000100000) continue;
            if ((*inst >> 5) == 0b110101010011100000000100001) continue;
            if ((*inst >> 5) == 0b110101010011100000000000000) continue;
            if ((*inst >> 5) == 0b110101010011100000000000101) continue;
            if ((*inst >> 5) == 0b110101010011100000000000110) continue;
            if ((*inst >> 5) == 0b110101010001101101000100000) continue;
            if ((*inst >> 5) == 0b110101010001101101000100001) continue;
            if ((*inst >> 5) == 0b110101010001101111010000010) continue;
            if ((*inst >> 5) == 0b110101010001101101000010100) continue;
            if ((*inst >> 5) == 0b110101010001101101000010000) continue;
            if ((*inst >> 5) == 0b110101010001101101000010110) continue;
            if ((*inst >> 5) == 0b110101010001101111100000000) continue;
            printf("find a bad sys inst\n");
        } 
        /* base, SIMD&FP instr*/
        else {
            if ((*inst) == 0b11010110100111110000001111100000) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b1111111111 == 0b1111111111) && (((*inst) >> 11) == 0b110101101001111100001)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111 == 0) && (((*inst) >> 21) == 0b11010100010)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111 == 0b10) && (((*inst) >> 21) == 0b11010100000)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111 == 0b11) && (((*inst) >> 21) == 0b11010100000)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111111 == 0b1111111) && (((*inst) >> 12) == 0b11010101000000000100)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111111 == 0b10011111) && (((*inst) >> 12) == 0b11010101000000000100)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111111 == 0b11011111) && (((*inst) >> 12) == 0b11010101000000110100)) {
                printf("find a bad base inst\n");
                continue;
            }
            if (((*inst) & 0b11111111 == 0b11111111) && (((*inst) >> 12) == 0b11010101000000110100)) {
                printf("find a bad base inst\n");
                continue;
            }
            if ((*inst) == 0b11010101000000110010000001111111) {
                printf("find a bad base inst\n");
                continue;
            }

        }
    }
    return;
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
            text_scan(sh_data,shdr->sh_size);
        }
        // printf("finish one round\n");
    }
    printf("cost time = %fs\n", get_cur_time());

}