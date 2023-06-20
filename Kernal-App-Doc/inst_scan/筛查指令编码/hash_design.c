#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
int strToInt(int begin, int end, char* code) {
    // for (int i = begin; i <= end; i++) {
    //     printf("%c",code[i]);
    // }
    // printf("\n");
    int value = 0;
    for (int i = begin; i <= end; i++) {
        value += code[i]-'0';
        value <<= 1;
    }
    value >>= 1;
    // char string[32] = {0};
    // itoa(value,string,2);
    // printf("value = %s\n",string);
    return value;
}
int main(){
    freopen("reduced-instcode.txt","r",stdin);
    char code[20];// 17 bit
    while(scanf("%s",code) != EOF){
        int value = strToInt(0,16,code);
        int high = strToInt(0,9,code);
        int low = strToInt(10,16,code);
        int result = high ^~ low;
        // printf("low = %d, high = %d\n", low, high);
        printf("%d\n",-result);

        // printf("%s\n",code);
    }

}