#include<stdio.h>

int main(){
    // unsigned long long target = 0;
	// asm volatile(
	// 	"mrs x0,  ID_AA64MMFR2_EL1\n\t"
	// 	"mov %[result], x0 \n\t"
	// 	: [result]"=r"(target)
	// 	::	
	// 	);
	// printf(" ID_AA64MMFR2_EL1: %llx\n", target);
	
	// printf("test\n");
	asm volatile(
		"MSR XAFLAG,0\n\t"
		: 
		::	
		);

	// asm volatile(
	// 	"SYS #3, C7, C3, #4, x0\n\t"
	// 	:
	// 	::	
	// 	);
	printf("success\n");

	return 0;
}