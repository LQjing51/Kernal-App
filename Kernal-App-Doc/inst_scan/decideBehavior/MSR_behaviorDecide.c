#include<stdio.h>
/*
x0 HCR_EL2:
    17:TID2
    20:TIDCP
    34:E2H
    27:TGE
    53:EnSCXT
// x1 SCTLR_EL1:
    9:UMA
    15:UCT
    20:TSCXT

// x2 CPACR_EL1:
    20-21:FPEN

x3 CPTR_EL2:
    20-21:FPEN
    10:TFP
    30:TAM
x4 HFGRTR_EL2
    14:CTR_EL0
    15:DCZID_EL0
    31:SCXTNUM_EL0
    35:TPIDR_EL0
    34:TPIDRRO_EL0
X5 PMUSERENR_EL0
    0:EN
    1:SW
    2:CR
    3:ER 
X6 SCTLR_EL2
    15:UCT
    20:TSCXT
X7 HDFGRTR_EL2
    14: PMCCFILTR_EL0
    15: PMCCNTR_EL0
    58: PMCEIDn_EL0
    16: PMCNTEN
    12: PMEVCNTRn_EL0
    13: PMEVTYPERn_EL0
    18: PMOVS
    19: PMSELR_EL0
    57: PMUSERENR_EL0
X8 MDCR_EL2
    6:TPM
    5:TPMCR
x9 AMUSERENR_EL0
    0:EN
x10 HAFGRTR_EL2
    0:AMCNTEN0
    17:AMCNTEN1
    1:AMEVCNTR00_EL0//1+n
    18:AMEVCNTR1<x>_EL0[0]//18+2n
    19:AMEVTYPER1<x>_EL0[0]//19+2n
// x11 CNTKCTL_EL1
    0:EL0PCTEN
    1:EL0VCTEN
x12 CNTHCTL_EL2
    0:EL0PCTEN
    1:EL0VCTEN
x13 AMCR_EL0
    17:CG1RZ

x14 HFGWTR_EL2 全0
    31:SCXTNUM_EL0
    35:TPIDR_EL0
x15 HDFGWTR_EL2 全0
    14：PMCCFILTR_EL0
    15：PMCCNTR_EL0
    16：PMCNTEN
    21：PMCR_EL0
    12：PMEVCNTRn_EL0 
    13：PMEVTYPERn_EL0 
    18：PMOVS 
    19: PMSELR_EL0 
    20: PMSWINC_EL0 
x16 MDSCR_EL1
    12:TDCC
x17 MDCR_EL2
    8:TDE
    9:TDA
    27:TDCC
*/
unsigned long long x0 = 0x488000000,x1 = 0x30500980,x2 = 0x300000,x3 =0x300000, \
x4 =0, x5 =0x1111111111111111, x6 =0x3474599d, x7 =0, x8 =0, x9 =0x1111111111111111, \
x10 =0, x11 = 0,x12 = 0xca6,x13 = 0,x14 = 0,x15 =0, x16 = 0x1000, x17 = 0;
unsigned long long shift[64];
//MSR
void decideBehavior(){

    printf(" 411 C5.2.2   DAIF, Interrupt Mask Bits\n");

    if ((x0 & shift[34] && x0 & shift[27]) || !(x1 & shift[9]))
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");
    printf("True\n");

    printf(" 426 C5.2.7   FPCR, Floating-point Control Registern\n");    

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x2 & shift[20] && x2 & shift[21]))
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x3 & shift[20] && x3 & shift[21]))
        printf("Undefine\n");
    else if (x0 & shift[34] && !(x3 & shift[20]))
        printf("Undefine\n");
    else if (!(x0 & shift[34]) && x3 & shift[10])
        printf("Undefine\n");
    else
        printf("True\n");

    if (!(x2 & shift[20]))
        printf("Undefine\n");
    else if (!(x0 & shift[34]) && x3 & shift[10])
        printf("Undefine\n");
    else if (x0 & shift[34] && !(x3 & shift[20]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (!(x0 & shift[34]) && x3 & shift[10])
        printf("Undefine\n");
    else if (x0 & shift[34] && !(x3 & shift[20]))
        printf("Undefine\n");
    else
        printf("True\n");

    printf(" 434 C5.2.8   FPSR, Floating-point Status Register\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x2 & shift[20] && x2 & shift[21]))
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x3 & shift[20] && x3 & shift[21]))
        printf("Undefine\n");
    else if (x0 & shift[34] && !(x3 & shift[20]))
        printf("Undefine\n");
    else if (!(x0 & shift[34]) && x3 & shift[10])
        printf("Undefine\n");
    else
        printf("True\n");      

    if (!(x2 & shift[20]))
        printf("Undefine\n");
    else if (!(x0 & shift[34]) && x3 & shift[10])
        printf("Undefine\n");
    else if (x0 & shift[34] && !(x3 & shift[20]))
        printf("Undefine\n");
    else
       printf("True\n");

    if (!(x0 & shift[34]) && x3 & shift[10])
        printf("Undefine\n");
    else if (x0 & shift[34] && !(x3 & shift[20]))
        printf("Undefine\n");
    else
       printf("True\n");

    printf("3606 D13.2.114 S3_<op1>_<Cn>_<Cm>_<op2>, IMPLEMENTATION DEFINED registers\n");

    printf("True\n");
    
    if (x0 & shift[20])
        printf("Undefine\n");
    else 
        printf("True\n");

    printf("True\n");

    printf(" 3671 D13.2.119   SCXTNUM_EL0, EL0 Read/Write Software Context Number\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && x1 & shift[20])
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && !(x0 & shift[53]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x14 & shift[31])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && x6 & shift[20])
        printf("Undefine\n");
    else
        printf("True\n");

    if (!(x0 & shift[53]))
        printf("Undefine\n");
    else if (x14 & shift[31])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3741 D13.2.130   TPIDR_EL0, EL0 Read/Write Software Thread ID Register\n");

    if (!(x0 & shift[27] && x0 & shift[34]) && x14 & shift[35])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x14 & shift[35])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3831 D13.3.6   DBGDTR_EL0, half-duplex\n");

    if ((x16 & shift[12]))
        printf("Undefine\n");
    else if ((x17 & shift[27]))
        printf("Undefine\n");
    else if ((x0 & shift[27]) || !(!(x17 & shift[8]) && !(x17 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    if ((x17 & shift[27]))
        printf("Undefine\n");
    else if (!(!(x17 & shift[8]) && !(x17 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("3836 D13.3.8 DBGDTRTX_EL0, Debug Data Transfer Register, Transmit\n");

    if ((x16 & shift[12]))
        printf("Undefine\n");
    else if ((x17 & shift[27]))
        printf("Undefine\n");
    else if ((x0 & shift[27]) || !(!(x17 & shift[8]) && !(x17 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    if ((x17 & shift[27]))
        printf("Undefine\n");
    else if (!(!(x17 & shift[8]) && !(x17 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("  3930 D13.4.1   PMCCFILTR_EL0, Performance M onitors Cycle Count Filter Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[14])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[14])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3935 D13.4.2   PMCCNTR_EL0, Performance Monitors Cycle Count Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[15])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[15])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3944 D13.4.5   PMCNTENCLR_EL0, Performance Monitors Count Enable Clear register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3947 D13.4.6   PMCNTENSET_EL0, Performance Monitors Count Enable Set register\n");
    
    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3950 D13.4.7   PMCR_EL0, Performance Monitors Control Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[21])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else if (x8 & shift[5])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[21])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else if (x8 & shift[5])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3958 D13.4.8   PMEVCNTR<n>_EL0, n = 0 - 30\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3962 D13.4.9   PMEVTYPER<n>_EL0, n = 0 - 30\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3976 D13.4.13   PMOVSCLR_EL0, Performance Monito rs Overflow Flag Status Clear Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3980 D13.4.14   PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3984 D13.4.15   PMSELR_EL0, Performance Monitors Event Counter Selection Register\n");

    if ((!(x5 & shift[0]) && !(x5 & shift[3])) )
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[19])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[19])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3987 D13.4.16   PMSWINC_EL0, Performance Monitors Software Increment register\n");
    
    if ((!(x5 & shift[0]) && !(x5 & shift[1])) )
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[20])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[20])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3993 D13.4.18   PMXEVCNTR_EL0, Performance M onitors Selected Event Count Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("3997 D13.4.19   PMXEVTYPER_EL0, Performance Mo nitors Selected Event Type Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (!(x0 & shift[27] && x0 & shift[34]) && x15 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x15 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");


}

void init_shift(){
    for (int i = 0; i < 64; i++) {
        shift[i] = 1ull << i;
    }
}
void main(){
    freopen("MSR_bahaviorRes.txt","w+",stdout);
    init_shift();
    decideBehavior();
    fclose(stdout);
}