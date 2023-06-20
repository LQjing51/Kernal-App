

**1、1028  C6.2.89   ESB：Error Synchronization Barrier**

只有 FEAT_RAS 开启时才有定义，通过读 ID_AA64PFR0_EL1.RAS 知道M1机上开启该extension。EL0下调用vESBOperation，EL2下不会调用。

```c
SynchronizeErrors();//未实现
AArch64.ESBOperation();//EL相关函数
if PSTATE.EL IN {EL0, EL1} && EL2Enabled() then AArch64.vESBOperation();//EL相关函数
TakeUnmaskedSErrorInterrupts();//未实现
```

```c
HCR_EL2.<E2H,TGE> == '11'
PSTATE.A = 0
EDSCR.INTdis[0] ??
```

+ ELBOperation:

route_to_el2 = 1, target = EL2, mask_active = 1, mask_set = 0 , intdis 涉及External debug寄存器EDSCR.INTdis[0]，如果EL0和EL2下masked相同，则EL0/EL2下相同

+ vELBOperation：HCR_EL2.TGE != 0，vSEI_Pending为0，没有实际行为。

**2、1032  C6.2.92   HINT**

涉及各类hint，其中

+ Hint_Yield：Hint_Yield 函数没有给出实现
+ Hint_DGH：Feat_DGH未开启，该op对应指令无定义，且hint_dgh函数未实现
+ Hint_WFE：EL可能相关。
  + haveTWEDExt不成立（ID_AA64MMFR1_EL1.TWED = 0）, EL0下走AArch64.CheckForWFxTrap（EL1），其中SCTLR_EL2.nTWE = 1，不trap，check返回。EL0和EL2下最后都走WaitForEvent（即使haveTWEDExt成立，也是看SCTLR_EL2.nTWE，也是走WaitForEvent）。如果EventRegister没设置且设定时间没有timeout，则进入lowPowerState，等待一个WFE wakeup事件，否则什么也不干。EL0和EL2下行为一样。
+ Hint_WFI：EL可能相关 。EL0下不可执行，EL2下可以++
  + SCTLR_EL2.nTWI = 0， EL0时会在CheckForWFxTrap时调用WFxTrap被Trap到EL2。EL2下走最后的WaitForInterrupt。
+ Hint_SEV：SendEvent 函数未实现
+ Hint_SEVL：SendEventLocal EL无关
+ Hint_ESB：Feat_RAS开启，行为和ESB行为一样
+ Hint_PSB：Feat_SPE开启，函数未实现
+ Hint_TSB：Feat_TRF未开启，无定义，且函数未实现
+ Hint_CSDB：函数未实现
+ Hint_BTI：set Btype_Next EL无关

**5、WFE相关：**

1512  C6.2.348   WFE

operation为hint_WFE, conditional-equal

**6、WFI相关：**

1514  C6.2.350   WFI

operation为hint_WFI, EL0下conditional-undefine ++

**7、LDTR相关：内核可以有，不阻止**

1164   C6.2.156  LDTR  

1166 C6.2.157   LDTRB  

1168 C6.2.158   LDTRH  

1170 C6.2.159   LDTRSB  

1172 C6.2.160   LDTRSH  

1174 C6.2.161   LDTRSW  

1416 C6.2.293   STTR  

1418 C6.2.294   STTRB  

1420 C6.2.295   STTRH