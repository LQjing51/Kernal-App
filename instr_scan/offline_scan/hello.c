// #include<stdio.h>
int main(){
    // printf("hello\n");
    // asm volatile(
	// 	"MSR SPSel,0\n\t" //base filter
	// 	"WFI\n\t" //base trap
	// 	"MRS x0,NZCV\n\t" //good sys
	// 	"MRS x0,CurrentEL\n\t" //sys filter
	// 	"MRS x0,CTR_EL0\n\t"//sys trap
	// 	: 
	// 	::	
	// );
	int a = 0;
	a ++;
    return 0;
}