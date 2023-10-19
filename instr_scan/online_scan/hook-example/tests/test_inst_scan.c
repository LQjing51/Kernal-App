#include<stdio.h>
int main(){
    printf("hello\n");
	unsigned long long target = 0;
    asm volatile(
		// "MSR SPSel,0\n\t" //base filter
		"WFI\n\t" //base trap
		"MRS x0,NZCV\n\t" //good sys
		"MRS x0,CurrentEL\n\t" //sys filter
		"MRS x1,CTR_EL0\n\t"//sys trap
		// "brk 1000\n\t"
		"mov %[result], x1 \n\t"
		: [result]"=r"(target)
		::	
	);
	printf("x1: %llx\n", target);
	printf("finish test\n");
    return 0;
}