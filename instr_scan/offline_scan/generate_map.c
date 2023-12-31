#include<elf.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define INST_LENTH_STR 32
#define INST_LENTH_INT 4
#define SYS_FIX_LENGTH 27
#define SYS_MAP_SIZE 1024
#define BASE_MAP_SIZE 256

unsigned int SYS_Map[SYS_MAP_SIZE];
unsigned long long BASE_Map[BASE_MAP_SIZE];

unsigned long long range_atoi(int begin, int end, char* str) {

    unsigned long long value = 0;
    for (int i = begin; i <= end; i++) {
        value += str[i]-'0';
        if (i != end) value <<= 1;
    }
    return value;
}

const int LOW10 = (1 << 10) - 1;
const int LOW7 = (1 << 7) - 1;
const int LOW5 = (1 << 5) - 1;
const int LOW2 = 3;
const int LOW3 = 7;
int sys_hash(unsigned int code) {
    //inst[21:12]
    int high = (code >> 7) & LOW10;
    //inst[11:5]
    int low = code & LOW7;
    return high - low;
}
void insert_sys_map(char* code) {
    unsigned int value = range_atoi(0, 27, code);
    unsigned int inst_value = range_atoi(0,26,code); 
    int index = sys_hash(inst_value);
    SYS_Map[index] = value;
    // printf("inst:%s\n", code);
    // printf("index = %d\n", index);
    // printf("%u\n", SYS_Map[index]);
}
int base_hash_str(char* str) {

    int index = 0;
    index += str[27]-'0'; index <<= 1;      
    index += str[28]-'0'; index <<= 1;
    index += str[39]-'0'; index <<= 1;
    index += str[45]-'0'; index <<= 1;
    index += str[46]-'0'; index <<= 1;
    index += str[47]-'0'; index <<= 1;
    index += str[51]-'0'; index <<= 1;
    index += str[52]-'0'; 

    return index;
}
void insert_base_map(char* code) {
    int index = base_hash_str(code);
    unsigned long long value = range_atoi(0,53,code); 
    BASE_Map[index] = value;
    // printf("%d : %s\n", index, inst);
    
}
void init_map_nofile(void) {

    /*high 27 bits: inst[31:5]*/
    /*low 1 bit: needSimultate*/
    /*C5*/
    char* code = "1101010100001011011111100010"; insert_sys_map(code);
    code = "1101010100001011011110100010"; insert_sys_map(code);
    code = "1101010100001011011111010010"; insert_sys_map(code);
    code = "1101010100001011011111000010"; insert_sys_map(code);
    code = "1101010100001011011110110010"; insert_sys_map(code);
    code = "1101010100001011011101000010"; insert_sys_map(code);
    code = "1101010100001011011101010010"; insert_sys_map(code);
    /*MRS*/
    code = "1101010100111011010001000000"; insert_sys_map(code);
    code = "1101010100111011010001000010"; insert_sys_map(code);
    code = "1101010100111011000000001110"; insert_sys_map(code);
    code = "1101010100111011110100000100"; insert_sys_map(code);
    code = "1101010100111011110100000110"; insert_sys_map(code);
    code = "1101010100111011010000101000"; insert_sys_map(code);
    code = "1101010100111011010000100000"; insert_sys_map(code);
    code = "1101010100111011010000101100"; insert_sys_map(code);
    code = "1101010100111011111000000000"; insert_sys_map(code);
    code = "1101010100111011111000001100"; insert_sys_map(code);
    code = "1101010100111011111000000100"; insert_sys_map(code);

    /*!halted undefine*/
    code = "110101010011101101000101001"; insert_sys_map(code);		
    code = "110101010001101101000101001"; insert_sys_map(code);		
    code = "110101010011101101000101000"; insert_sys_map(code);		
    code = "110101010001101101000101000"; insert_sys_map(code);		

    /*simulate MRS*/
    code = "1101010100111011000000000011"; insert_sys_map(code);
    code = "1101010100111000000001011001"; insert_sys_map(code);
    code = "1101010100111000000001011011"; insert_sys_map(code);
    code = "1101010100111000000001010001"; insert_sys_map(code);
    code = "1101010100111000000001010011"; insert_sys_map(code);
    code = "1101010100111000000001100001"; insert_sys_map(code);
    code = "1101010100111000000001100011"; insert_sys_map(code);
    code = "1101010100111000000001100101"; insert_sys_map(code);
    code = "1101010100111000000001110001"; insert_sys_map(code);
    code = "1101010100111000000001110011"; insert_sys_map(code);
    code = "1101010100111000000001110101"; insert_sys_map(code);
    code = "1101010100111000000001000001"; insert_sys_map(code);
    code = "1101010100111000000001000011"; insert_sys_map(code);
    code = "1101010100111000000000000001"; insert_sys_map(code);
    code = "1101010100111000000000001011"; insert_sys_map(code);
    code = "1101010100111000000000001101"; insert_sys_map(code);
    /*MSR*/
    code = "1101010100011011010001000000"; insert_sys_map(code);
    code = "1101010100011011010001000010"; insert_sys_map(code);
    code = "1101010100011011110100000100"; insert_sys_map(code);
    code = "1101010100011011010000101000"; insert_sys_map(code);
    code = "1101010100011011010000100000"; insert_sys_map(code);
    code = "1101010100011011010000101100"; insert_sys_map(code);

    //base inst
    /*high 21 bit:mask, mask[31:21] = 11111111111*/
    /*mid 32 bit:inst*/
    /*low 1 bit:needSimulate*/
    code = "111111111111111111111110101101001111100000011111000000"; insert_base_map(code);//ERET
    code = "111111111111111111111110101101001111100001x11111111110"; insert_base_map(code);//ERETAA,ERETA

    code = "000000010000011111111110101000100000000000000000000000"; insert_base_map(code);//HLT
    code = "000000010000011111111110101000100000000100000000000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000001000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000001000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000010000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000010000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000011000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000011000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000100000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000100000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000101000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000101000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000110000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000110000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000000000111000000"; insert_base_map(code);
    code = "000000010000011111111110101000100000000100000111000000"; insert_base_map(code);

    code = "000000010000011111111110101000000000000000000000000100"; insert_base_map(code);//HVC
    code = "000000010000011111111110101000000000000100000000000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000001000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000001000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000010000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000010000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000011000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000011000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000100000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000100000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000101000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000101000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000110000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000110000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000111000100"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000111000100"; insert_base_map(code);

    code = "000000010000011111111110101000000000000000000000000110"; insert_base_map(code);//SMC
    code = "000000010000011111111110101000000000000100000000000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000001000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000001000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000010000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000010000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000011000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000011000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000100000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000100000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000101000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000101000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000110000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000110000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000000000111000110"; insert_base_map(code);
    code = "000000010000011111111110101000000000000100000111000110"; insert_base_map(code);

    code = "111111111000011111111110101010000000001000000011111110"; insert_base_map(code);//UAO
    code = "111111111000011111111110101010000000001000000100111110"; insert_base_map(code);//PAN
    code = "111111111000011111111110101010000000001000000101111110"; insert_base_map(code);//SPSel
    code = "111111111000011111111110101010000000001000000000111110"; insert_base_map(code);//CFINV 
    code = "111111111000011111111110101010000000001000000001111110"; insert_base_map(code);//XAFLAG
    code = "111111111000011111111110101010000000001000000010111110"; insert_base_map(code);//AXFLAG
    code = "111111111000011111111110101010000001101000000110111110"; insert_base_map(code);//DAIF SET 
    code = "111111111000011111111110101010000001101000000111111110"; insert_base_map(code);//DAIF CLEAR
    code = "111111111111111111111110101010000001100100000011111111"; insert_base_map(code);//WFI

}
void dump_whole_map(int is_sys) {
    char* type = is_sys ? "sys" : "base";
    printf("****** dump %s whole map ******\n", type);
    if (is_sys) {
        for (int i = 0; i < SYS_MAP_SIZE; i++) {
            if (!(i % 10)) printf("\t");
            if (!(i % 100)) printf("\n");

            if (SYS_Map[i]) {
                printf("%u, ", SYS_Map[i]);
            }else {
                printf("0, ");
            }
        }
    }else {
        for (int i = 0; i < BASE_MAP_SIZE; i++) {
            if (!(i % 10)) printf("\n");

            if (BASE_Map[i]) {
                printf("%llu, ", BASE_Map[i]);
            }else {
                printf("0, ");
            }
        }
    }
}
int main(){
    init_map_nofile();
    dump_whole_map(0);
}
//sys_map
/*
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
*/
//base map
/*
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
*/