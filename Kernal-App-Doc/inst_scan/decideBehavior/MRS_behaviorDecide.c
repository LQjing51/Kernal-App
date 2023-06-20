#include<stdio.h>
/*
ID_AA64MMFR1_EL1：          10211122
    8-11 VH 1
ID_AA64DFR0_EL1: 10305f09
    8-11 PMUVer f
    32-35:PMSVer 0
    40-43:TraceFilt 0
ID_AA64PFR0_EL1: 1101000010110111
    28-31: RAS 1
    32-35:SVE 0
    36-39: SEL2 0
    44-47 AMU 0
    48-51 DIT 1
ID_AA64PFR1_EL1: 20
    0-3: BT 0
    4-7: SSBS 2
ID_AA64ISAR1_EL1: 11110211202
    0-3 DPB:2
    28-31: GPI
    24-27:GPA
    8-11:API
    4-7:APA
    40-43:SPECRES 1
    48-51:DGH 0
    56-59:XS 0
ID_AA64ISAR0_EL1: 221100110212120
    55-52:TS 2
    60-63:RNDR 0
// TCR_EL1(TCR_EL12):140008000 
    37:TBI0 0
    38:TBI1 0
    51:TBID0 0
    52:TBID1 0
TCR_EL2:10000217550b510
    37:TBI0 1
    38:TBI1 0
    TBID0 0
    TBID1 0
ID_AA64MMFR0_EL1: 12120f100001 111ff000000(el0)
ID_AA64MMFR1_EL1: 11212100  0(el0)
    32-35:TWED 0
ID_AA64MMFR2_EL1 : 1201111100001011 100000000(el0)
    24-27:NV 0
    36-39:IDS 0
DAIF: 0

x0 HCR_EL2:488000000,488000002
    5:AMO 0
    13:TWI 0
    14:TWE 0
    17:TID2 0
    20:TIDCP
    29:HCD 0
    34:E2H 1
    27:TGE 1
    53:EnSCXT
    45:NV2 0
    44:AT 0
    43:NV1 0 
    42:NV 0
    41：API 0
// x1 SCTLR_EL1(SCTLR_EL12): 30500980
    9:UMA 0
    15:UCT 0
    16:nTWI 0
    18:nTWE 1->0
    19:WXN 0
    20:TSCXT 1
    30：EnIB 0
    31：EnIA 0

SCTLR_EL2: fc74799d
    3:SA 1
    4:SA0 1
    19:WXN 0

// x2 CPACR_EL1(CPACR_EL12):300000
    20-21:FPEN 11
x3 CPTR_EL2:300000
    20-21:FPEN 11
    10:TFP
    30:TAM 0
    16-17:ZEN 0
X6 SCTLR_EL2: fc74799d
    0:M 1
    3:SA 1
    4:SA0 1
    15:UCT 0
    16:nTWI 0
    18:nTWE 1
    20:TSCXT
    24:E0E 0
    25:EE 0
    30：EnIB 1
    31：EnIA 1
X8 MDCR_EL2:0,c
    6:TPM
    5:TPMCR
    8:TDE
    9:TDA
    27:TDCC 0
// x11 CNTKCTL_EL1(CNTKCTL_EL12):0 ca6
    0:EL0PCTEN 0
    1:EL0VCTEN 1->0
    9:EL0PTEN 0
x12 CNTHCTL_EL2:ca6
    0:EL0PCTEN 0
    1:EL0VCTEN 1
    8:EL0VTEN 0
    9:EL0PTEN 0
    10:EL1PCTEN 0
    11:EL1PTEN 1
    13:EL1TVT 0
x16 MDSCR_EL1:1000
    12:TDCC 0

x4 HFGRTR_EL2: undefine 全0
    14:CTR_EL0
    15:DCZID_EL0
    31:SCXTNUM_EL0
    35:TPIDR_EL0
    34:TPIDRRO_EL0
x14 HFGWTR_EL2: undefine 全0
    31:SCXTNUM_EL0
    35:TPIDR_EL0
X7 HDFGRTR_EL2: undefine 全0
    14: PMCCFILTR_EL0
    15: PMCCNTR_EL0
    58: PMCEIDn_EL0
    16: PMCNTEN
    12: PMEVCNTRn_EL0
    13: PMEVTYPERn_EL0
    18: PMOVS
    19: PMSELR_EL0
    57: PMUSERENR_EL0
x15 HDFGWTR_EL2: undefine 全0
    14：PMCCFILTR_EL0
    15：PMCCNTR_EL0
    16：PMCNTEN
    21：PMCR_EL0
    12：PMEVCNTRn_EL0 
    13：PMEVTYPERn_EL0 
    18：PMOVS 
    19: PMSELR_EL0 
    20: PMSWINC_EL0 
x10 HAFGRTR_EL2:undefine 全0
    0:AMCNTEN0
    17:AMCNTEN1
    1:AMEVCNTR00_EL0//1+n
    18:AMEVCNTR1<x>_EL0[0]//18+2n
    19:AMEVTYPER1<x>_EL0[0]//19+2n

X5 PMUSERENR_EL0：undefine
    0:EN
    2:CR
    3:ER 
x9 AMUSERENR_EL0:undefine
    0:EN
x13 AMCR_EL0:undefine
    17:CG1RZ

*/
unsigned long long x0 = 0x488000000,x1 = 0x30500980,x2 = 0x300000,x3 =0x300000, \
x4 =0, x5 =0x1111111111111111, x6 =0x3474799d, x7 =0, x8 =0, x9 =0x1111111111111111, \
x10 =0, x11 = 0,x12 = 0xca6,x13 = 0,x14 = 0,x15 =0, x16 = 0x1000;
unsigned long long shift[64];
// MRS
void decideBehavior(){

    printf("411 C5.2.2   DAIF, Interrupt Mask Bits\n");
    if (!(x0 & shift[34] && x0 & shift[27]) && (x1 & shift[9])){
        printf("True\n");
    }else {
        printf("Undefine\n");
    }
    printf("True\n");
    printf("True\n");


    printf(" 426 C5.2.7   FPCR, Floating-point Control Register\n");
    if( !((x0 & shift[34] && x0 & shift[27])) && (!(x2 & shift[20] && x2 & shift[21]))){
        printf("Undefine\n");
    }else if((x0 & shift[34] && x0 & shift[27]) && (!(x3 & shift[20] && x3 & shift[21]))){
        printf("Undefine\n");
    }else if((x0 & shift[34]) && (!(x3 & shift[20]))) {
        printf("Undefine\n");
    }else if((!(x0 & shift[34])) && (x3 & shift[10])) {
        printf("Undefine\n");
    }else{
        printf("True\n");
    }

    if ((!(x2 & shift[20]))) {
        printf("Undefine\n");
    }else if((!(x0 & shift[34])) && (x3 & shift[10])) {
        printf("Undefine\n");
    }else if((x0 & shift[34]) && (!(x3 & shift[20]))) {
        printf("Undefine\n");
    }else {
        printf("True\n");
    }

    if ((!(x0 & shift[34])) && (x3 & shift[10]))
        printf("Undefine\n");
    else if ((x0 & shift[34]) && (!(x3 & shift[20])))
        printf("Undefine\n");
    else
        printf("True\n");

    printf(" 434 C5.2.8   FPSR, Floating-point Status Register\n");
    if (!((x0 & shift[34] && x0 & shift[27])) && (!(x2 & shift[20] && x2 & shift[21])))
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && (!(x3 & shift[20] && x3 & shift[21])))
        printf("Undefine\n");
    else if ((x0 & shift[34]) && (!(x3 & shift[20])))
        printf("Undefine\n");
    else if ((!(x0 & shift[34])) && (x3 & shift[10]))
        printf("Undefine\n");
    else
        printf("True\n");

    if ((!(x2 & shift[20])))
        printf("Undefine\n");
    else if ((!(x0 & shift[34])) && (x3 & shift[10]))
        printf("Undefine\n");
    else if ((x0 & shift[34]) && (!(x3 & shift[20])))
        printf("Undefine\n");
    else 
        printf("True\n");
    
    if ((!(x0 & shift[34])) && (x3 & shift[10]))
        printf("Undefine\n");
    else if ((x0 & shift[34]) && (!(x3 & shift[20])))
        printf("Undefine\n");
    else 
        printf("True\n");

    
    printf(" 3138 D13.2.34   CTR_EL0, Cache Type Register\n");
    if (!( (x0 & shift[34] && x0 & shift[27])) && (!(x1 & shift[15])))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x0 & shift[17])
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x4 & shift[14])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && (!(x6 & shift[15])))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[17])
        printf("Undefine\n");
    else if (x4 & shift[14])
        printf("Undefine\n");
    else
        printf("True\n");
    
    printf("True\n");

    printf(" 3143 D13.2.36   DCZID_EL0, Data Cache Zero ID register\n");

    if ((!(x0 & shift[34] && x0 & shift[27])) && x4 & shift[15]) {
        printf("Undefine\n");
    }else {
        printf("True\n");
    }

    if (x4 & shift[15]) {
        printf("Undefine\n");
    }else {
        printf("True\n");
    }

    printf("Ture\n");

    printf("3606 D13.2.114 S3_<op1>_<Cn>_<Cm>_<op2>, IMPLEMENTATION DEFINED registers\n");

    printf("True\n");
    
    if (x0 & shift[20])
        printf("Undefine\n");
    else 
        printf("True\n");

    printf("True\n");

    printf("3671 D13.2.119   SCXTNUM_EL0, EL0 Read/Write Software Context Number\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && x1 & shift[20]){
        printf("Undefine\n");
    }else if ((!(x0 & shift[34] && x0 & shift[27])) && (!(x0 & shift[53]))){
        printf("Undefine\n");
    }else if ((!(x0 & shift[34] && x0 & shift[27])) && x4 & shift[31]){
        printf("Undefine\n");
    }else if ((x0 & shift[34] && x0 & shift[27]) && x6 & shift[20]){
        printf("Undefine\n");
    }else{
        printf("True\n");
    }

    if ((!(x0 & shift[53]))){
        printf("Undefine\n");
    }else if (x4 & shift[31]){
        printf("Undefine\n");
    }else{
        printf("True\n");
    }

    printf("True\n");

    printf(" 3741 D13.2.130   TPIDR_EL0, EL0 Read/Write Software Thread ID Register\n");

    if ((!(x0 & shift[34] && x0 & shift[27])) && x4 & shift[35]){
        printf("Undefine\n");
    }else{
        printf("True\n");
    }

    if (x4 & shift[35])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3749 D13.2.134   TPIDRRO_EL0, EL0 Read-Only Software Thread ID Register\n");

    if ((!(x0 & shift[34] && x0 & shift[27])) && x4 & shift[34])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x4 & shift[34])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 3831 D13.3.6   DBGDTR_EL0, half-duplex\n");

    if ((x16 & shift[12]))
        printf("Undefine\n");
    else if ((x8 & shift[27]))
        printf("Undefine\n");
    else if ((x0 & shift[27]) || !(!(x8 & shift[8]) && !(x8 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    if ((x8 & shift[27]))
        printf("Undefine\n");
    else if (!(!(x8 & shift[8]) && !(x8 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("3834 D13.3.7 DBGDTRRX_EL0, Debug Data Transfer Register, Receive\n");

    if ((x16 & shift[12]))
        printf("Undefine\n");
    else if ((x8 & shift[27]))
        printf("Undefine\n");
    else if ((x0 & shift[27]) || !(!(x8 & shift[8]) && !(x8 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");
    
    if ((x8 & shift[27]))
        printf("Undefine\n");
    else if (!(!(x8 & shift[8]) && !(x8 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");
    
    printf("True\n");
    
    
    printf(" 3866 D13.3.16   MDCCSR_EL0, Monitor DCC Status Register\n");

    if ((x16 & shift[12]))
        printf("Undefine\n");
    else if ((x8 & shift[27]))
        printf("Undefine\n");
    else if ((x0 & shift[27]) || !(!(x8 & shift[8]) && !(x8 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    if ((x8 & shift[27]))
        printf("Undefine\n");
    else if (!(!(x8 & shift[8]) && !(x8 & shift[9])))
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");


    printf(" //3930 D13.4.1   PMCCFILTR_EL0, Performance M onitors Cycle Count Filter Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[14])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[14])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3935 D13.4.2   PMCCNTR_EL0, Performance Monitors Cycle Count Register\n");

    if (!(x5 & shift[0]) && !(x5 & shift[2]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[15])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[15])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3938 D13.4.3   PMCEID0_EL0, Performance Monitors Common Event Identification register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[58])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else 
        printf("True\n");

    if (x7 & shift[58])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3941 D13.4.4   PMCEID1_EL0, Performance Monitors Common Event Identification register 1\n");
    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[58])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else 
        printf("True\n");

    if (x7 & shift[58])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");
    
    printf("True\n");

    printf(" //3944 D13.4.5   PMCNTENCLR_EL0, Performance Monitors Count Enable Clear register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3947 D13.4.6   PMCNTENSET_EL0, Performance Monitors Count Enable Set register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[16])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");
    printf(" //3950 D13.4.7   PMCR_EL0, Performance Monitors Control Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else if (x8 & shift[5])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x8 & shift[6])
        printf("Undefine\n");
    else if (x8 & shift[5])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3958 D13.4.8   PMEVCNTR<n>_EL0, n = 0 - 30\n");

    if (!(x5 & shift[0]) && !(x5 & shift[3]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3962 D13.4.9   PMEVTYPER<n>_EL0, n = 0 - 30\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3976 D13.4.13   PMOVSCLR_EL0, Performance Monito rs Overflow Flag Status Clear Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3980 D13.4.14   PMOVSSET_EL0, Performance Monitors Overflow Flag Status Set register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[18])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[18])    
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3984 D13.4.15   PMSELR_EL0, Performance Monitors Event Counter Selection Register\n");

    if (!(x5 & shift[0]) && !(x5 & shift[3]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[19])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[19])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3989 D13.4.17   PMUSERENR_EL0, Performance Monitors User Enable Register\n");

    if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[57])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[57])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //3993 D13.4.18   PMXEVCNTR_EL0, Performance M onitors Selected Event Count Register\n");

    if (!(x5 & shift[0]) && !(x5 & shift[3]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[12])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("//3997 D13.4.19   PMXEVTYPER_EL0, Performance Mo nitors Selected Event Type Register\n");

    if (!(x5 & shift[0]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x7 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x7 & shift[13])
        printf("Undefine\n");
    else if (x8 & shift[6])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //4002 D13.5.1   AMCFGR_EL0, Activity Monitors Configuration Register\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //4005 D13.5.2   AMCG1IDR_EL0, Activity Monitors Counter Group 1 Identification Register\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //4007 D13.5.3   AMCGCR_EL0, Activity Monitors Counter Group Configuration Register\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");
    
    printf("True\n");

    printf(" //4009 D13.5.4   AMCNTENCLR0_EL0, Activity Moni tors Count Enable Clear Register 0\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[0])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else if (x10 & shift[0])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" //4012 D13.5.5   AMCNTENCLR1_EL0, Activity Moni tors Count Enable Clear Register 1\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[17])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else if (x10 & shift[17])
        printf("Undefine\n");
    else
        printf("True\n");
    
    printf("True\n");

    printf(" 4015 D13.5.6   AMCNTENSET0_EL0, Activity Monitors Count Enable Set Register 0\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[0])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else if (x10 & shift[0])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 4018 D13.5.7   AMCNTENSET1_EL0, Activity Monitors Count Enable Set Register 1\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[17])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else if (x10 & shift[17])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 4021 D13.5.8   AMCR_EL0, Activity Monitors Control Register\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    for (int i = 0; i < 4; i++) {
        printf(" 4024 D13.5.9   AMEVCNTR0<n>_EL0, n = %d\n",i);

        if (!(x9 & shift[0]))
            printf("Undefine\n");
        else if (x3 & shift[30])
            printf("Undefine\n");
        else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[1+i])
            printf("Undefine\n");
        else
            printf("True\n");

        if (x3 & shift[30])
            printf("Undefine\n");
        else if (x10 & shift[1+i])
            printf("Undefine\n");
        else
            printf("True\n");
        
        printf("True\n");
    }

    for (int i = 0; i < 16; i++) {
        printf(" 4027 D13.5.10   AMEVCNTR1<n>_EL0, n = %d\n",i);
     
        if (!(x9 & shift[0]))
            printf("Undefine\n");
        else if (x3 & shift[30])
            printf("Undefine\n");
        else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[18+2*i])
            printf("Undefine\n");
        else if (x13 & shift[17])
            printf("True A\n");
        else
            printf("True B\n");

        if (x3 & shift[30])
            printf("Undefine\n");
        else if (x10 & shift[18+2*i])
            printf("Undefine\n");
        else if (x13 & shift[17])
            printf("True A\n");
        else
            printf("True B\n");

        printf("True B\n");
    }
    
    printf(" 4034 D13.5.13   AMEVTYPER0<n>_EL0, n = 0 - 3\n");

    if (!(x9 & shift[0]))
        printf("Undefine\n");
    else if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");
    printf("True\n");

    for (int i = 0; i < 16; i++) {
        printf(" 4036 D13.5.14   AMEVTYPER1<n>_EL0, n = %d\n",i);

        if (!(x9 & shift[0]))
            printf("Undefine\n");
        else if (x3 & shift[30])
            printf("Undefine\n");
        else if ((!(x0 & shift[34] && x0 & shift[27])) && x10 & shift[19+2*i])
            printf("Undefine\n");
        else
            printf("True\n");
        
        if (x3 & shift[30])
            printf("Undefine\n");
        else if (x10 & shift[19+2*i])
            printf("Undefine\n");
        else
            printf("True\n");
        
        printf("True\n");
    }

    printf(" 4039 D13.5.15   AMUSERENR_EL0, Activity Monitors User Enable Register\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    if (x3 & shift[30])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf(" 4140 D13.8.1   CNTFRQ_EL0, Counter-timer Frequency register\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x11 & shift[0]) && !(x11 & shift[1]))
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x12 & shift[0]) && !(x12 & shift[1]))
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");
    printf("True\n");

    printf(" 4239 D13.8.28   CNTVCTSS_EL0, Counter-timer Self-Synchronized Virtual Count register\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x11 & shift[1]))
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x12 & shift[1]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x12 & shift[14])
        printf("Undefine\n");
    else
        if ((!(x0 & shift[34] && x0 & shift[27])))
            printf("True A\n");
        else
            printf("True B\n");

    if (x12 & shift[14])
        printf("Undefine\n");
    else
        printf("True A\n");

    if (!(x0 & shift[34]))
        printf("True A\n");
    else
        printf("True B\n");

    printf(" 4241 D13.8.29   CNTVCT_EL0, Counter-timer Virtual Count register\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x11 & shift[1]))
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x12 & shift[1]))
        printf("Undefine\n");
    else if ((!(x0 & shift[34] && x0 & shift[27])) && x12 & shift[14])
        printf("Undefine\n");
    else
        if ((!(x0 & shift[34] && x0 & shift[27])))
            printf("True A\n");
        else
            printf("True B\n");

    if (x12 & shift[14])
        printf("Undefine\n");
    else
        printf("True A\n");

    if (!(x0 & shift[34]))
        printf("True A\n");
    else
        printf("True B\n");


    // printf("4202 D13.8.16   CNTP_CTL_EL0, Counter-timer Physical Timer Control register\n");

    // if (!(EL2Enabled() && HCR_EL2.<E2H,TGE> == '11') && CNTKCTL_EL1.EL0PTEN == '0')
    //     if (EL2Enabled() && HCR_EL2.TGE == '1')
    //         AArch64.SystemAccessTrap(EL2, 0x18);
    //     else
    //         AArch64.SystemAccessTrap(EL1, 0x18);
    // else if (EL2Enabled() && HCR_EL2.E2H == '0' && CNTHCTL_EL2.EL1PCEN == '0')
    //     AArch64.SystemAccessTrap(EL2, 0x18);
    // else if (EL2Enabled() && HCR_EL2.<E2H,TGE> == '10' && CNTHCTL_EL2.EL1PTEN == '0')
    //     AArch64.SystemAccessTrap(EL2, 0x18);
    // else if (EL2Enabled() && HCR_EL2.<E2H,TGE> == '11' && CNTHCTL_EL2.EL0PTEN == '0')
    //     AArch64.SystemAccessTrap(EL2, 0x18);
    // else if (EL2Enabled() && HCR_EL2.<E2H,TGE> == '11' && SCR_EL3.NS == '0' && IsFeatureImplemented(FEAT_SEL2))
    //     printf("True A\n");
    // else if (EL2Enabled() && HCR_EL2.<E2H,TGE> == '11' && SCR_EL3.NS == '1')
    //     printf("True A\n");
    // else
    //     printf("True B\n");

    // if (EL2Enabled() && HCR_EL2.E2H == '0' && CNTHCTL_EL2.EL1PCEN == '0')
    //     AArch64.SystemAccessTrap(EL2, 0x18);
    // else if (EL2Enabled() && HCR_EL2.E2H == '1' && CNTHCTL_EL2.EL1PTEN == '0')
    //     AArch64.SystemAccessTrap(EL2, 0x18);
    // else if (EL2Enabled() && HCR_EL2.<NV2,NV1,NV> == '111')
    //     printf("True C\n");
    // else
    //     printf("True B\n");

    // if (HCR_EL2.E2H == '1' && SCR_EL3.NS == '0' && IsFeatureImplemented(FEAT_SEL2))
    //     printf("True A\n");
    // else if (HCR_EL2.E2H == '1' && SCR_EL3.NS == '1')
    //     printf("True A\n");
    // else
    //     printf("True B\n");

    // printf(" 4206 D13.8.17   CNTP_CVAL_EL0, Counter-timer Physical Timer CompareValue register\n");

    // printf(" 4210 D13.8.18   CNTP_TVAL_EL0, Counter-timer Physical Timer TimerValue register\n");

    // printf(" 4214 D13.8.19   CNTPCTSS_EL0, Counter-timer Self-Synchronized Physical Count register\n");

    // printf(" 4216 D13.8.20   CNTPCT_EL0, Counter-timer Physical Count register\n");

    // printf(" 4227 D13.8.25   CNTV_CTL_EL0, Counter-timer Virtual Timer Control register\n");

    // printf(" 4231 D13.8.26   CNTV_CVAL_EL0, Counter-timer Virtual Timer CompareValue register\n");

    // printf(" 4235 D13.8.27   CNTV_TVAL_EL0, Counter-timer Virtual Timer TimerValue register\n");




}
void init_shift(){
    for (int i = 0; i < 64; i++) {
        shift[i] = 1ull << i;
    }
}



void main(){
    freopen("MRS_behaviorRes.txt","w+",stdout);
    init_shift();
    decideBehavior();
    fclose(stdout);
}