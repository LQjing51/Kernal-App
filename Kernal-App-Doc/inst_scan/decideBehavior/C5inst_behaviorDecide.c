#include<stdio.h>
/*
x0 HCR_EL2:
    17:TID2
    20:TIDCP
    23:TPCP
    24:TPU
    28:TDZ
    34:E2H
    27:TGE
    42:NV
    52:TOCU
    53:EnSCXT
// x1 SCTLR_EL1:
    9:UMA
    10:EnRCTX
    14:DZE
    15:UCT
    20:TSCXT
X6 SCTLR_EL2
    10:EnRCTX
    14:DZE
    15:UCT
    20:TSCXT
    26:UCI
x18 HFGITR_EL2: undefine 全0
    2:ICIVAU
    7:DCCVAU
    8:DCCVAP
    9:DCCVADP
    10:DCCIVAC
    11:DCZVA
    48:CFPRCTX
    49:DVPRCTX
    50:CPPRCTX
    54:DCCVAC

*/
unsigned long long x0 = 0x488000000,x1 = 0x30500980,x2 = 0x300000,x3 =0x300000, \
x4 =0, x5 =0x1111111111111111, x6 =0x3474799d, x7 =0, x8 =0, x9 =0x1111111111111111, \
x10 =0, x11 = 0,x12 = 0xca6,x13 = 0,x14 = 0,x15 =0, x16 = 0x1000, x17 = 0,x18=0;
unsigned long long shift[64];
void decideBehavior(){
    printf("C5.3.2 DC CGDVAC, Clean of Data and Allocation Tags by VA to PoC\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[54])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[54])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.3 DC CGDVADP, Clean of Data and Allocation Tags by VA to PoDP\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[9])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[9])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.4 DC CGDVAP, Clean of Data and Allocation Tags by VA to PoP\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[8])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[8])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.6 DC CGVAC, Clean of Allocation Tags by VA to PoC\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[54])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[54])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.7 DC CGVADP, Clean of Allocation Tags by VA to PoDP\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[9])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[9])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.8 DC CGVAP, Clean of Allocation Tags by VA to PoP\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[8])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[8])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.10 DC CIGDVAC, Clean and Invalidate of Data and Allocation Tags by VA to PoC\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[10])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[10])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.12 DC CIGVAC, Clean and Invalidate of Allocation Tags by VA to PoC\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[10])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[10])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.14 DC CIVAC, Data or unified Cache line Clean and Invalidate by VA to PoC\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[10])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[10])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.16 DC CVAC, Data or unified Cache line Clean by VA to PoC\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[54])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[54])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.17 DC CVADP, Data or unified Cache line Clean by VA to PoDP\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[9])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[9])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.18 DC CVAP, Data or unified Cache line Clean by VA to PoP\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[23])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[8])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[23])
        printf("Undefine\n");
    else if (x18 & shift[8])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.19 DC CVAU, Data or unified Cache line Clean by VA to PoU\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[24])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[52])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[7])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[24])
        printf("Undefine\n");
    else if (x0 & shift[52])
        printf("Undefine\n");
    else if (x18 & shift[7])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.20 DC GVA, Data Cache set Allocation Tag by VA\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[14]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[28])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[11])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[14]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[28])
        printf("Undefine\n");
    else if (x18 & shift[11])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.21 DC GZVA, Data Cache set Allocation Tags and Zero by VA\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[14]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[28])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[11])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[14]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[28])
        printf("Undefine\n");
    else if (x18 & shift[11])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.28 DC ZVA, Data Cache Zero by VA\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[14]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[28])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[11])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[14]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[28])
        printf("Undefine\n");
    else if (x18 & shift[11])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.3.31 IC IVAU, Instruction Cache line Invalidate by VA to PoU\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[15]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[24])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x0 & shift[52])
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[2])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[26]))
        printf("Undefine\n");
    else
        printf("True\n");

    if (x0 & shift[24])
        printf("Undefine\n");
    else if (x0 & shift[52])
        printf("Undefine\n");
    else if (x18 & shift[2])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.6.1 CFP RCTX, Control Flow Prediction Restriction by Context\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[10]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[48])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[10]))
        printf("Undefine\n");
    else
        printf("True\n");
    
    if (x0 & shift[42])
        printf("Undefine\n");
    else if (x18 & shift[48])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.6.2 CPP RCTX, Cache Prefetch Prediction Restriction by Context\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[10]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[50])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[10]))
        printf("Undefine\n");
    else
        printf("True\n");
    
    if (x0 & shift[42])
        printf("Undefine\n");
    else if (x18 & shift[50])
        printf("Undefine\n");
    else
        printf("True\n");

    printf("True\n");

    printf("C5.6.3 DVP RCTX, Data Value Prediction Restriction by Context\n");

    if (!((x0 & shift[34] && x0 & shift[27])) && !(x1 & shift[10]))
        printf("Undefine\n");
    else if (!(x0 & shift[34] && x0 & shift[27]) && x18 & shift[49])
        printf("Undefine\n");
    else if ((x0 & shift[34] && x0 & shift[27]) && !(x6 & shift[10]))
        printf("Undefine\n");
    else
        printf("True\n");
    
    if (x0 & shift[42])
        printf("Undefine\n");
    else if (x18 & shift[49])
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
    freopen("C5_bahaviorRes.txt","w+",stdout);
    init_shift();
    decideBehavior();
    fclose(stdout);
}    