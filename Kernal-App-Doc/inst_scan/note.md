[TOC]

M1芯片 10.3.0.198 -p 1234 密码 2001.5.1Lqj ARMv8p4

鲲鹏服务器host 10.208.130.12 -p 6543 密码 2001.5.1Lqj

virsh list --all 查看所有虚拟机

鲲鹏虚拟机 ssh ubuntu@kappvm-xmy 密码123 ARMv8p2

+ 要先开虚拟机：先virsh list —all，显示所有的虚拟机，然后“virsh start [虚拟机名称]” 把名字结尾是xmy的那个虚拟机启动
+ 关闭虚拟机：poweroff
+ 主机关闭虚拟机： virsh start（开机）/shutdown（关机）/destory（强制关机）/reboot(重启) [虚拟机名]



scp：前面原内容，后面是复制的目的地。远端地址需要指明端口等。

+ 更新虚拟机内容：scp -P 6543 -r lqj@10.208.130.12:/home/lqj/instr_scan/online_scan /home/ubuntu/lqj

+ 更新host内容：scp -P 6543 -r  /home/ubuntu/lqj/online_scan lqj@10.208.130.12:/home/lqj/instr_scan



funcCall.py：伪代码章节函数分类

alias.py：C6，7指令分类

fineSearch.py：C6 MSR/MRS涉及的寄存器分类

### 手册内容分布

​		**指令**：共1452条

+ C5.3-C5.6 System instruction

+ C6.2 base instruction

+ C7.2 SIMD instruction

  **寄存器**

+ C5.2 Special-purpose register

+ D13.2-D13.8 System Register

  **函数**

+ J1.2.3 Pseudocode

### **C5 指令分类**：

C5共有指令126条指令

### **MRS/MSR读写系统寄存器指令分类:**

部分指令没有规整的分支，单拎出来，人工分类。
+ MRS:3594（1），3596（1），3598（1），3603（1），3604（1），3605（1），3606（2），3834（2），3854（2），3862（2）
+ MSR:3594（1），3596（1），3598（1），3606（2），3836（2），3854（2），3862（2），4011（3），4014（3），4017（3），4020（3），4023（3），4026（3），4029（3），4038（3），4141（3）
+ IsHighestEL（4011-4141）（认为系统最高的EL为EL2），halted(3836,3854，3862，与debug state相关，默认为false)

注意事项：分开考虑读写；注意有些寄存器有两种访存/存储方式，EL0,EL02，当作两个寄存器(有的后面的寄存器与别的章节的同名寄存器行为一致，不单独考虑)；注意换页；一页有多个MSR/MRS的情况；有些寄存器只有读没有写（D13.2.111），有些只有写没有读（比如D13.3.8），有读的总共299个，有写的总共233个

怎么做：先将EL0,EL1,EL2下所有干活指令找出来（不以if，elsif，else开头，可能为return，AArch64.SystemAccessTrap，undefined，其它。注意碰到分号就要结束，避免扫到页尾字符）。若EL0的干活指令都是undefined或trap，将其归为第一类；若EL0，1，2，相同，归为第二类；EL0，1，2存在不一致将其归为第三类

#### 寄存器重定向

**注意：由此，对EL0/EL1下寄存器的值读取的时候需要考虑重定位，即在EL2下读部分寄存器的值时，应该采用EL02/EL12这种方式。**

见2787页 D5.6 Virtualization Host Extensions。我们的系统开了FEAT_VHE：

支持虚拟化后，一般软件栈为：hypervisor运行在EL2，虚拟机运行在EL0/1。 通常，内核运行在EL1，但虚拟控制在EL2。这意味着大多数host OS运行在EL1，通过EL2的stub代码来访问虚拟化控制。这种安排效率低下，因为它可能涉及增加的上下文切换。 于是我们需要在EL2上运行host os。

 <img src="https://img-blog.csdnimg.cn/3efc231a31044b21a1af07c06f671f21.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAZmx5aW5nbm9za3k=,size_15,color_FFFFFF,t_70,g_se,x_16" alt="img" style="zoom: 50%;" /> 

**HCR_EL2.{E2H,TGE}={1,1}时：**

**EL2下对于部分EL0/1 system register的访问实际上为访问相对应的EL2寄存器**：

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666942161276.png" alt="1666942161276" style="zoom:80%;" />

**为了实际上还是访问EL0/1的寄存器，EL2下可以通过一些别名寄存器来表示仍要访问EL0/1寄存器：**

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666942278566.png" alt="1666942278566" style="zoom: 67%;" />

2793页 D5.7 Nested virtualization。Feat_NV开启时，一个EL2下的host hypervisor可以在EL1下跑一个guest hypervisor。Feat_NV2开启时，PE将system register的访问直接转变为内存的访问。根据ID_AA64MMFR2_EL1.NV可以知道其未开启？（HCR_EL2.{NV,NV1,AT}均为0）。故在EL1下MRS/MSR部分寄存器会被改成load/store指令，直接访问内存某块区域的情况不存在。

即对于一个指令章节有多个寄存器读写的情况，需要额外考虑EL12/EL02的，不需要额外考虑EL1的。还发现有额外的EL2等，根据内容来排版，即xxEL2指令某些情况下返回的是EL1的值那该指令就会被放在EL1寄存器章节下，如417 ELR_EL1，内容在别的章节重复了，编码相同，不考虑。

C5.2寄存器 25个；D13.2 149个；D13.3 30个；D13.4 19个；D13.5 15个；D13.6 13个；D13.7 16个；D13.8 30个。总共寄存器 25+149+30+19+15+13+16+30=313个。

#### 第一类：Equal Inst

​	EL0,1,2分支内容完全相同，考虑页眉页脚的影响，先筛查EL0\==EL1\==EL2的，然后看其它的任意两个分支相等的，人为筛查。这里只包括EL0/1/2下有行为且相同的。

#### 第二类：EL0 Undefine Inst

​	EL0 undifined/（trap + undefined），不管跑在EL1还是EL2，都无条件改为报错指令；EL0下无定义可能是因为原本就是undefine，或者extension没有开，该指令undefine，或者根据配置情况，EL0下走trap分支。

#### 第三类：Conditional-Undefine Inst

##### 分类标准

​	EL0 有实际行为，且三个EL下的实际行为相同，根据系统配置情况对其进行处理

+ 有些配置下实际行为和EL1和EL2相同且有行为（即EL0==EL1\==EL2），此时不用对该指令进行处理（根据配置情况，直接走），归并到第一类
+ 有些配置下是trap/undifined，改为报错（根据配置情况，直接改），归并到第二类
+ 配置筛选：isHighestEL认为EL2是最高的，haveEL（EL3）认为false，EL2Enable认为true，HaveEL（EL2）认为True，其它的看寄存器配置，halt认为false。
+ 注意：寄存器（例如，CPTR_EL2）不同情况下，有不同的定义，假定部分情况都在前面的条件中判定了，例如426 C5.2.7，但extension没有判定；

##### 问题1：不同权级下，配置寄存器值可能不同

我们在EL2下读取某些寄存器的值来判断该指令它在不同权级下的执行情况，但是寄存器值在不同el下可能会变，甚至可能有不同实现？（切换权级的时候需要更改哪些寄存器值（除了PSTATE.EL）？解决：在EL0下读取这些配置寄存器的值，可能读不出来）

##### 问题2：某些种类寄存器因为Extension未开启，而undefine，解决：归入到第二类（undefine）

###### 1、PMU相关寄存器 结论：D13.4章的PMU相关寄存器均无定义（EL0/1/2下访问均失败）

PMUSERENR(Performance Monitors User Enable Register)

+ PMUSERENR_EL0 只有在有PMUv3的时候才可访问，访问 ID_AA64DFR0_EL1.PMUVer 值为f，指PMU由实现决定，PMUv3不支持。访问PMUSERENR_EL0，insmod时会segment fault。
  + 该寄存器的EN域为0时表示trap EL0下对某些PM寄存器的访问到EL1 或EL2（当EL2 enable 且 HCR_EL2.TGE=1）。
  + 该寄存器的CR域为0时表示trap EL0下对于cycle counter reads（PMCCNTR_EL0）到EL1或 EL2（为0时，还要求EN=0才可以trap，为1时，不管EN，允许其访问）。
  + 该寄存器的ER域为0表示trap EL0下对于event counter read的访问，限制 PMXEVCNTR_EL0，PMEVCNTR<n>_EL0的读，限制PMSELR_EL0的读写（和EN合作，为1时override EN，允许访问）
+ 所有条件中涉及这个寄存器的寄存器都只有在PMU开启时才有定义，EL0下访问时illegal instruction，EL2下访问insmod时报错 segment fault。

###### 2、AMU相关寄存器 结论：D13.5章的AMU相关寄存器均无定义

AMUSERENR(Activity Monitors User Enable Register)

+ AMUSERENR_EL0只有在有FEAT_AMUv1的时候才可访问，访问ID_AA64PFR0_EL1.AMU，值为0，AMU未开启。
+ 所有条件中涉及这个寄存器的寄存器都只有在AMU开启时才有定义，EL0下访问时illegal instruction，EL2下访问insmod时报错 segment fault。

AMCR_EL0(Activity Monitors Control Register)

+ AMCR_EL0只有在AMU开启时，才有定义。条件中涉及这个寄存器的寄存器也只有在AMU开启时才有定义。

###### 3、SPE相关寄存器 结论：D13.6章SPE相关寄存器均无定义

SPE(Statistical Profiling Extension)

##### 问题3：某些有定义寄存器的分支条件中存在undefine寄存器

###### 1、FGT相关寄存器 结论：认为其全0，行为符合未开启该扩展的行为

这些寄存器这些位表示EL0/1下访问某系统寄存器时，将其trap到EL2下。默认（3399页）HFGRTR_EL2为全0，不执行这些trap。

tips：在E2H TGE(trap general exception)均为1时，只有EL0和EL2，此时这些寄存器值影响不了EL0/2的行为。因为此时不需要细粒度的trap，EL0下所有应该trap到EL1的exception都被trap到了EL2。FGT存在的意义在于有EL1时，限制EL0/1下执行某些MSR/MRS/指令，并且让其trap到hypervisor，可以影响EL1的行为。

+ HFGRTR_EL2（Hypervisor Fine-Grained Read Trap Register，provide controls for traps of MRS/MRC(32位) reads of System register）：
  + 3138等MRS指令分支条件中需要访问HFGRTR_EL2寄存器，但是HFGRTR_EL2寄存器只有在实现了FEAT_FGT 时有定义，EL2下访问编译报错（编译器报错信息为：处理器不支持该系统寄存器名）。该MRS指令在鲲鹏服务器虚拟机（也没有实现FGT）中EL1下可以正常执行，认为走else分支，即HFGRTR_EL2.CTR_EL0为0。3143情况相同，EL1下正常执行，即HFGRTR_EL2.DCZID_EL0为0。3671，3741，3749情况相同。
+ HFGWTR_EL2（Hypervisor Fine-Grained Write Trap Register）：
+ HDFGRTR_EL2（Hypervisor Debug Fine-Grained Read Trap Register，MRS/MRC reads of debug, trace, PMU, and Statistical Profiling System registers.）：
+ HDFGWTR_EL2 （Hypervisor Debug Fine-Grained Write Trap Register）：
+ HFGITR_EL2 （Hypervisor Fine-Grained Instruction Trap Register）：
+ HAFGRTR_EL2 （Hypervisor Activity Monitors Fine-Grained Read Trap Register）

#### 第四类：BehaviorDiff Inst

EL0,1,2实际行为存在不一样，需要模拟。差别在于不同权级下可以返回不同权级的寄存器，例如：EL0和EL2下可以返回EL2/EL0，EL1下可以是返回NVM或者EL0。或者有extension，返回值不同，4214。

根据系统配置情况，对其进行细粒度划分，部分指令可以归为第1，2类：认为Feat_Sel2没有开启，

##### MRS：

1. 4202 D13.8.16   CNTP_CTL_EL0, Counter-timer Physical Timer Control register：归为Undefine Inst

   ```c
   EL0: 满足EL2Enabled() && HCR_EL2.<E2H,TGE> == '11' && CNTHCTL_EL2.EL0PTEN == '0' , 走trap分支
   EL1: CNTHCTL_EL2.EL1PTEN == '0'不满足, return CNTP_CTL_EL0
   EL2: SCR_EL3 Undefine，return CNTP_CTL_EL0
   ```

   CNTHCTL_EL2寄存器用于``Controls the generation of an event stream from the physical counter, and access from EL1 to the physical counter and the EL1 physical timer``，

   + EL0PTEN：当HCR_EL2.TGE为1的时候，该域为0，host app，EL0下对于相关CNTP寄存器的访问会被trap到EL2；HCR_EL2.TGE为0的时候，该域为1，guets app，不trap指令。
   + EL1PTEN：当TGE为0的时候，该域为0，trap EL0和EL1对于相关CNTP寄存器的访问到EL2，TGE为1的时候，该域为1，不trap。
     SCR_EL3.NS指no secure state

   归到第四类时因为EL1下有返回NVMem的情况，FEAT_NV，extension未开启，不会走该分支

2. 4206：与上相同

3.  4210：与上相同

4.  4214 D13.8.19   CNTPCTSS_EL0, Counter-timer Self-Synchronized Physical Count register：看EL0PCTEN（0）和EL1PCTEN（0），FEAT_VCE未开启，EL0,EL1均trap，EL2 return

5.  4216 与上结果相同

6.  4227 D13.8.25   CNTV_CTL_EL0, Counter-timer Virtual Timer Control register：看EL0VTEN（0）和EL1TVT（0）：EL0 trap，EL1/2下均返回CNTV_CTL_EL0

7.  4231：与上相同

8.  4235：与上相同

##### MSR:

1. 4202，4206，4210，4227，4231，4235六条指令与上情况相同，均为EL0 trap，EL1/2write。

### **C6，7指令筛查：**

+ C6共354条指令，C7共404条指令，共758条指令

+ 先找出所有的指令别名情况：指令的Operation（“Operation for all encodings”）：The description of xxxx gives the operational pseudocode for this instruction，记录该假指令与真指令的对应关系。指令A的某个域可变，在某些情况下，可以通过它的别名指令充当这个指令，例如C7.2.371 UMOV 的域 imm5可变，imm5 = 'xxx00'时，该指令等价于MOV（to general）指令。

+ 将所有真指令分类：
    + 对于指令operation/decode中有EL相关的，直接分类。
      
      + operation（“Operation for all encodings”，例1016 DSB）中读写系统寄存器
        + 注意除去那些已经筛查出来在EL0,1,2下行为完全一样的，
        + 分开读写，写：寄存后接“ = ”，否则为读
        + PSTATE各个域的访问情况，与寄存器不同，比如PSTATE.<A,I,F>；PSTATE还有域IL,nRW,BTYPE，认为它们读写也均与EL相关
        + DBGBCR<n>_EL1这种寄存器在指令中会以DBGBCR_EL1[n/xx]形式访问，改为DBGBCR_EL1[
      
    + 看指令的指令operation/decode中调用的函数是否被标记（EL相关）。
      + 先找所有原本就有EL相关的函数，将其标记上，看剩下的函数是否（多级）调用这些被标记的函数。共931（402（24+57+165+77+79）+529（69+4+409+5+42））个函数.
        
        ```c++
        tips：
        	注意章节之间的切换时的context终止位置，注意不要读入目录，注意匹配title的时候还有/和-。
            找函数时，注意前面不能是字母，后面匹配[]或()(有些只能匹配[]，有些只能匹配()，没有处理，有些函数是type结构，后面没有()/[]，不会匹配)；不匹配注释中的内容；不匹配".",因为AArch64.AddrTop和AddrTop不同，但是存在例外，比如AArch64.BranchTargetException和BranchTargetException相同，人工处理；
        ```
        
        + 删去V，VL，isSVEEnable函数，UsingAArch32函数（认为读nRW域和EL无关）
        
      + 再看指令调用上述函数的情况 568项：
      
        + CheckFPAdvSIMDEnabled64（312项），AArch64.CheckFPAdvSIMDEnabled（31项），CheckFPEnabled64（53项）：最终调用AArch64.CheckFPEnabled()。EL0,EL2的isInHost是true，EL1是false；EL0和EL2下都走check trap，EL1下正常返回（此时没有EL1），实操指令验证？
        + SP 232项（EL2下内核app使用的是SP_EL0，认为SP返回值不变）
        + Mem 185项 （因为BigEndian）
        + CheckSPAlignment 152项（认为check都成功，sp！=Align(sp，16)不满足？一定不满足吗，若可能满足需要看SCTLR相关寄存器）
        + MemAtomic 27项（因为BigEndian）
        + BigEndian 14项（需要看SCTLR相关寄存器）
        + AArch64.SetExclusiveMonitors 8项
        + AArch64.ExclusiveMonitorsPass 8项
        + AArch64.MemTag 8项
        + BranchTo 7项
        + AArch64 Abort 5项
        + MemAtomicCompareAndSwap 4项（因为BigEndian）
      
    + 对于剩下的仍然认为和EL相关的真指令，人工筛查。[见后文](#C6.C7 EL相关指令)
    
+ 对别名指令（假指令）进行分类，共92条指令是别的指令的别名。

+ 未实现的函数相关：总共有64条NonImplement的函数，删去其中RoundUp，RoundDown和physMemRead之后，相关的指令总共有594条（C6 187条，C7 404条）。指令调用的NonImple函数中最多的是V（383条），SetTagCheckedInstruction（165条），BracnTo（13条）

    + 调用的函数有：SetTagCheckedInstruction（159条），V（383条）
    + V不再考虑，通过查看 ID_AA64PFR0_EL1 的SVE域，认为FEAT_SVE未开启，采用默认的128，后面的ConstraintUnpredictableBool，无需关注，和EL无关。
    + SetTagCheckedInstruction 未实现。有的调用它的条件是HaveMTE2Ext()，通过查看 ID_AA64PFR1_EL1 的MTE域，认为MTE未开启，条件不满足；有的没条件，（1323，1351，1447），decode部分说明了MTE未开启的时候，指令没有定义。

**更新：**originEL相关，只看是否有PSTATE.EL字样。对于内容中出现的el，target_el等字样，一定是通过某个函数调用（在调用的函数中会出现PSTATE.EL，没有出现的情况，可能是HaveEL等，涉及的是系统配置，不是当前EL状态），或者传入的参数得到的（在原本的指令处，会将PSTATE.EL当作参数）。

C5 sysytem instruction 126条指令 

+ 106条 Undefine Inst
+ 13条 Conditional Undefine Inst
+ 7条 Conditional Equal Inst

MRS

+ 238条 Undefine Inst
+ 74条 Conditional Undefine Inst
+ 3条 Equal Inst
+ 5条 Conditional Equal Inst

MSR

+ 181条 Undefine Inst
+ 58条 Conditional Undefine Inst
+ 3条 Equal Inst
+ 3条 Conditional Equal Inst

C6 base instruction & C7  Advanced SIMD and Floating-point Instruction

+ Equal Inst 172条

+ EL相关的 585+1条

  + operation callELfunction的有585条

    operation 有 PSTATE.EL 的有8条（有一条不在callELfunction中）: 原来的note中都考虑了

    decode 部分没有callELfunction的

    decode 部分 PSTATE.EL 的有12条（都在callELfunction中）

  + 认为下列函数EL无关

    + SP EL无关：在EL2下和EL0下均返回SP_EL0，无条件EL无关
    
    + V，Vpart无关：读写 SIMD&FP register，因为CPTR_EL2.ZEN = x0，isSVEenabled 为false。即使不是SVEenable为true，EL0和EL2下得到的vl结果也相同。
    
    + BranchTo 无关：
    
      + ```C
        AddrTop的S1TranslationRegime（EL0返回EL2，EL1/2返回EL1/2）：EL0isInHost
        因为regime均为EL2，则EffectiveTBI和EL无关
        ```
    
    + CheckFPAdvSIMDEnabled64/CheckFPAdvSIMDEnabled->CheckFPEnabled64/CheckFPEnabled无关
    
      + ```C
        主函数只调用CheckFPEnabled()，里面调用CheckFPAdvSIMDTrap，EL0/2下行为相同，要么都trap，要么都不需要trap（此时是都不需要），check函数返回。
        ```
      
    + CheckSPAlignment 无关
    
      + ```C
        SCTLR_EL2.SA0 = 1, SCTLR_EL2.SA = 1
        ```
    
    + BigEndian，Mem，MemAtomic，MemAtomicCompareAndSwap 无关
    
      + ```C
        AccType_NV2REGISTER没有指令用这个acctype，故走后面两个分支
        SCTLR_EL2.E0E = 0,SCTLR_EL2.EE = 0
        ```
    
    + AArch64.SetExclusiveMonitors，AArch64.ExclusiveMonitorsPass无关？？
    
      + ```C
        首先调用CheckAlignment，因为abort而EL相关，而Abort中调用的各类Abort/Exception EL0/1/2下均AArch64.TakeException(EL2, exception, preferred_exception_return, vect_offset) 参数内容都相同。
        然后调用TranslateAddress，因为FullTranlate而EL相关，里面S1translate EL相关
        ```
    
    + AArch64.MemTag 无关
    
      + MTE未开启，所有调用这个memtag的指令在EL0/2下都undefine
    
    + AArch64.Abort 无关
    
      + ```C
        而Abort中调用的各类Abort/Exception EL0/1/2下均AArch64.TakeException(EL2, exception, preferred_exception_return, vect_offset) 参数内容都相同。
        ```
    
    + 剩下的指令筛查之后，四条EL0下 undefine，若干条待定





#### C6.C7 EL相关指令

4条 EL0下undefine（或部分条件下undefine，如MSR）

先暂时将未实现的指令当作EL无关的，之后另外考虑。

**887 ADDG：add with tag，将立即数加到源寄存器中地址的tag上，将结果放在目的寄存器中**

EL相关原因：访问了``GCR_EL1.Exclude``,调用了EL相关函数:``AArch64.AllocationTagAccessIsEnabled``,``AArch64.AddressWithAllocationTag``。

解决：FEAT_MTE开启时才有定义：MTE（Memory Tagging Extension）通过给内存地址设tag的方式实时检查内存访问错误。该指令无定义。

**913  C6.2.20   AUTDA, AUTDZA，Authenticate Data address using key A**

只调用AuthDA，认为EL无关

**914  C6.2.21   AUTDB, AUTDZB**

只调用AuthDB，认为EL无关

**915  C6.2.22   AUTIA, AUTIA1716, AUTIASP, AUTIAZ, AUTIZA，Authenticate Instruction address, using key A**

只调用AuthIA，认为EL无关

**917  C6.2.23   AUTIB, AUTIB1716, AUTIBSP, AUTIBZ, AUTIZB**

只调用AuthIB，认为EL无关

**920  C6.2.25   B.cond，直接有条件（cond）跳转到PC+imm的一个label处**

调用函数ConditionHolds（只访问PSTATE.NZCV，EL无关）和BranchTo

**921  C6.2.26   B，直接无条件跳转到PC+imm的一个label处**

调用BranchTo

**934  C6.2.33   BL，branch with link to a PC-relative offset，直接跳转到PC+imm处，同时将X30写为PC+4**

调用BranchTo

**935 BLR：Branch with Link to Register，间接跳转到register中的存的地址处，同时将X30写为PC+4**

EL相关原因：origin EL相关因为注释内容（PSTATE.BTYPE，与BTI相关），此外还调用了``BranchTo``,类别为BranchType_INDCALL(Indirect Branch with link, link指将PC+4写到寄存器中，指令中存的是寄存器地址或者内存地址，读出来相应的值再跳过去)。

解决：BranchTo EL无关

``S1TranslationRegime``：EL0：return EL2；EL1/2：return EL1/2; 

``HavePACExt``:等于HasArchVersion(Armv8p3)，组内机器上是v8p4，认为返回true

``EffectiveTBI``:TBI指Top Byte Ingore，虚地址的高8位用作标记。需要看寄存器和VA[55]，结果为1或者0。TCR_EL1.TBI1/TBI0和TCR_EL2.TBI1/TBI0，EL1和EL2的寄存器相同，和EL无关，va[55]为1则为返回0，为0则返回1。

``AddrTop``: return the MSB number(最高有效位) of virtual address in the stage 1 translation regime for el.若EffectiveTBI结果为1，则返回55，否则返回63

``BranchAddr``:按照AddrTop返回的最高有效位对va进行截取并补齐（符号位扩展，实际上都是补0，因为AddrTop返回55时，va[55]为0）

``BranchTo``:根据BranchAddr的结果跳转过去

**936 C6.2.35   BLRAA, BLRAAZ, BLRAB, BLRABZ：Branch with Link to Register,with point authentication，有PAC，用key检验将要跳转过去的地址**

EL相关原因：注释部分有PSTATE.BTYPE，还调用了target = ``AuthIA``或``AuthIB``，``BranchTo``

解决：PAC会先调用``ADDPACIA/B``将地址高位加上签名，然后需要使用这个地址的时候（寄存器内容作为branch地址），调用``AuthIA/B``对其进行验证，验证成功，将pointer authenticantion code field bits（若开启了TBI，则从次高字节开始）全部替换为va[55]（extension），并返回新的地址。若验证失败，将field的第二，三高位设置为一个unique的error码（表示不是extension），并返回，在后续地址翻译的时候报错。``AuthPACIA/B``和``AuthIA/B``中均会先对``SCTLR_EL1/2.EnIA/B``做判断（表示使用key A/B），enable均为0，不进行后续操作，直接返回源target，相当于未开启PAC，不设置pac域。

机器上暂时关闭了pac，认为pac EL无关

```C
AuthIA/B:EL0和EL2下的trapEL2和trapEL3都为false，最后都调用Auth
AuthDA/B相同
```



**938 C6.2.36   BR：Branch to Register**

EL相关原因：涉及BTYPENext赋值，BrachTo

**939  C6.2.37   BRAA, BRAAZ, BRAB, BRABZ：Branch to Register，with pointer authentication**

解决：BType，branchTo，AuthIA/B

**941  C6.2.38   BRK：Breakpoint instruction**

调用AArch64.SoftwareBreakpoint(imm16);

TGE 为1时EL0/EL1/EL2最后都调用 AArch64.TakeException(EL2, exception, preferred_exception_return, vect_offset) 参数内容都相同

**942  C6.2.39   BTI：Branch Target Identification，用于阻止那些不是intended target of a branch 的指令的执行，阻止它们跳转到guarded memory pages**

EL相关原因：op的别的case有PSTATE.EL字样

解决：FEAT_BTI未实现，v8p5才有BTI extension，不会有该条指令。BTI类似于intel 的 IBT。

间接跳转BR,BLR的指令末尾，会根据指令类型，跳转前的内存区域，间接跳转用的寄存器的类型，把PSTATE.BTYPE设置为一个非零值（其它指令，会将其设置为00）。当下一条指令处于guarded memory region 并且 PSTATE.BTYPE ！=0b00时，若这条指令不是兼容的BTI或者某些条件下的PACIASP,PACIBSP，Breakpoint Instruction exception，Halt Instruction debug event，则一定会报Branch Target exception错误。

即在每个BB块前要插桩插入BTI指令，BTI的target参数由跳转来的jump指令类型决定。

若这条BTI指令target type和当前的BType相兼容，则可以继续执行，否则报Branch Target Exception。BTI的操作数op2\<2:1\>表明了期望的间接跳转的type，有(omiited),c,j,jc四种，用于兼容性判断。SystemHintop会被设置为SystemHintOp_BTI，在此op下，将BtypeNext设为00。

**954  C6.2.44   CBNZ**

BranchTo

**955  C6.2.45   CBZ**

BranchTo

**1009  C6.2.76   DCPS1 Debug change PE state to EL1** 

External Debug相关，！Halted（），则undefine，平常执行时都是undefine的，在debug state情况下，调用DCPSInstruction。EL0 下 undefine

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1667915436790.png" alt="1667915436790" style="zoom:80%;" />

**1010  C6.2.77   DCPS2** 

External Debug相关，！Halted（），则undefine （认为一直不会处于debug state，是否合理，hlt指令EL0下不可执行，）

**1011  C6.2.78   DCPS3** 

External Debug相关，！Halted（），则undefine

**1015  C6.2.81   DRPS：Debug restore process state** 

decode部分说明 ！Halted（），则undefine，且EL0下无定义

**////1016  C6.2.82   DSB：Data Synchronization Barrier**

EL相关原因：涉及FEAT_XS的有PSTATE.EL字样，还调用了``DataSynchronizationBarrier``未实现函数或``SpeculativeStoreBypassBarrierToVA()``，``SpeculativeStoreBypassBarrierToPA()``

FEAT_XS，memory引入一个XS属性，指明a access could take a long time to complete，由此给DSB和TLB指令引入nXS变量，用于处理有XS属性的内存访问。v8p7开始支持，未实现。并且有Feat_XS的时候，nXS都是True，没有的时候都是false。行为没有差异

解决：Feat_XS未实现

**1026  C6.2.87   ERET** ++

decode部分说明 EL0下无定义

**1027  C6.2.88   ERETAA, ERETAB**++

decode部分说明 EL0下无定义

**1028  C6.2.89   ESB：Error Synchronization Barrier** ？？

只有 FEAT_RAS 开启时才有定义，平台开启该extension。RAS （Reliability, Availability, and Serviceability） RAS 框架允许平台为同步外部中止 (SEA)、异步外部中止（也记为SError）、fault handing 和 error recovery interrupts 等错误导致的异常路由到EL3下进行处理 。涉及RAS寄存器，例如 4092 DISR_EL1, Records that an SError interrupt has been consumed by an ESB instruction。

operation：

+ 无实现函数SynchronizeErrors，同步各个错误

+ ESBOperation：

  ``route_to_el2  = 1``，``target = EL2``，``当处于EL0/EL2时，mask_active为1``，``mask_set = PSTATE.A == 1``，``intdis = 0/1?`` ， ``masked = ?``

  PSTATE.A 异步异常mask：A：SError interrupt mask bit 

**1032  C6.2.92   HINT **？？

有包括 Hint_Yield()，Hint_DGH()，Hint_WFE/I，SendEvent，ESB，PSB在内的各种Hint。

Hint_Yield：未实现 Provides a hint that the task performed by a thread is of low importance so that it could yield to improve overall performance.

Hint_DGH： 未开启

Hint_WFE/WFI：

+ EL0下trap到EL1
+ EL1下走最后的waitForEvent/Interrupt
+ EL2下走最后的waitForEvent/Interrupt

Hint_SEV/SEVL：SendEvent未实现，SendEventLocal EL无关 

Hint_ESB：Feat_RAS，和ESB行为一样

Hint_PSB：Feat_SPE，ID_AA64DFR0_EL1.PMSVer

Hint_TSB：Feat_TRF，ID_AA64DFR0_EL1.TraceFilt

Hint_CSDB：未实现

Hint_BTI：set Btype Next EL无关

**1034  C6.2.93   HLT Halt Instruction** ++

EL0 下undefine 不可执行

产生一个 Halt Instruction debug event，导致进入 debug state

看能不能halt，可以的话调用Halt(DebugHalt_HaltInstruction);

**1035  C6.2.94   HVC ++**

描述：Hypervisor Call causes an exception to EL2. Software executing at EL1 can use this instruction to call the hypervisor to request a service

因为 HCR_EL2.HCD ==0(HVC instruction disable)，该指令为EL0下undefine，EL2下可执行

**1037  C6.2.96   IRG**

描述：Insert Random Tag, MTE未开启，未定义

**1040  C6.2.98   LD64B， Single-copy Atomic 64-byte**

FEAT_LS64开启才有定义，v8p7才有，该指令无定义

**1070  C6.2.114   LDAXP** ？？

调用AArch64.SetExclusiveMonitors（address，dbytes）调用 AArch64.CheckAlignment和AArch64.TranslateAddress EL相关，待检查

可能调用AArch64.Abort：其中调用的各类Abort/Exception EL0/1/2下均AArch64.TakeException(EL2, exception, preferred_exception_return, vect_offset) 参数内容都相同。

**1072  C6.2.115   LDAXR？**

调用AArch64.SetExclusiveMonitors（address，dbytes）

**1074  C6.2.116   LDAXRB**？

调用AArch64.SetExclusiveMonitors（address，1）

**1075  C6.2.117   LDAXRH**？

调用AArch64.SetExclusiveMonitors（address，2）

**1090  C6.2.124   LDG**

load an allocation tag from a memory address，MTE未开启，无定义

**1091  C6.2.125   LDGM**

load tag multiple，MTE未开启，无定义

**1113  C6.2.135   LDRAA, LDRAB，load register with pointer authentication**

LDR：根据base register value 和 offset register value 算出地址，从这个地址load数

AuthDA，AuthDB PAC相关操作，认为和EL无关

**1199  C6.2.174   LDXP** ？

调用 AArch64.Abort(EL无关) ，AArch64.SetExclusiveMonitors

**1201  C6.2.175   LDXR** ？

 AArch64.SetExclusiveMonitors

**1203  C6.2.176   LDXRB** ？

 AArch64.SetExclusiveMonitors(

**1204  C6.2.177   LDXRH** ？

 AArch64.SetExclusiveMonitors

**1237  C6.2.195   MSR (immediate)**  ++ 

MSR(immediate) 将imm数写到PSTATE中各个域，会在encoding部分检查权限和extension，没有extension的时候，不管EL，全部undifine；若权限下不可以写，trap 或者 undifine

EL0下，只有在op1=011，op2=001/010时才有定义，即访问DIT和SSBS时才有定义。

EL1下，若op1是100/101/110/111，则undefine，Feat_SEL2没有开(Secure EL2，看 ID_AA64PFR0_EL1.SEL2发现没有开 )

EL2下，若op1是110/111，则undefine

MSR(register) 将general-purpose 的寄存器内容写到system register

**1261  C6.2.208   PACDA, PACDZA，Pointer Authentication Code for Data address**

调用AddPACDA

```C
最后都调用 AddPAC，里面调用ComputePAC
```

**1262  C6.2.209   PACDB, PACDZB**

调用AddPACDB

**1263  C6.2.210   PACGA，using generic key**

调用AddPACGA

```C
最后都调用ComputePAC
```



**1264  C6.2.211   PACIA, PACIA1716, PACIASP, PACIAZ, PACIZA，for Instruction address**

调用AddPACIA

**1267  C6.2.212   PACIB, PACIB1716, PACIBSP, PACIBZ, PACIZB**

调用AddPACIB

**1282  C6.2.220   RET**

BranchTo 和 BtypeNext

**1283  C6.2.221   RETAA, RETAB**

BranchTo，BtypeNext，AuthIA/B

**1316  C6.2.241   SMC，secure Monitor Call causes an exception to EL3**++

EL0下undefine，EL1/2也下undefine？没有EL3，认为它在EL0/1/2下都会被拦截？

**1323  C6.2.247   ST2G**

MTE未实现，无定义

**1325  C6.2.248   ST64B**

FEAT_LS64开启才有定义，v8p7才有，该指令无定义

**1326  C6.2.249   ST64BV**

FEAT_LS64开启才有定义，v8p7才有，该指令无定义

**1328  C6.2.250   ST64BV0**

FEAT_LS64开启才有定义，v8p7才有，该指令无定义

**1348  C6.2.260   STG，store allocation tag**

MTE未开

**1350  C6.2.261   STGM，Store Tag Multiple**

MTE未开启，未定义

**1351  C6.2.262   STGP，Store Allocation Tag and Pair of registers**

MTE未开启，未定义

**1368  C6.2.272   STLXP，Store-Release Exclusive Pair of registers**？？

描述：stores two 32-bit words or two 64-bit doublewords to a memory location
if the PE has exclusive access to the memory address, from two registers, and returns a status value of 0 if the store was successful, or of 1 if no store was performed.

调用AArch64.ExclusiveMonitorsPass

**1371  C6.2.273   STLXR，Store-Release Exclusive Register**

调用AArch64.ExclusiveMonitorsPass

**1374  C6.2.274   STLXRB**

调用AArch64.ExclusiveMonitorsPass

**1376  C6.2.275   STLXRH**

调用AArch64.ExclusiveMonitorsPass

**1438  C6.2.305   STXP**

调用AArch64.ExclusiveMonitorsPass

**1441  C6.2.306   STXR**

调用AArch64.ExclusiveMonitorsPass

**1443  C6.2.307   STXRB**

调用AArch64.ExclusiveMonitorsPass

**1445  C6.2.308   STXRH**

调用AArch64.ExclusiveMonitorsPass

**1447  C6.2.309   STZ2G**

MTE未实现，无定义

**1449  C6.2.310   STZG**

MTE未实现，无定义

**1451  C6.2.311   STZGM**

store tag and zero ，MTE未开启，未定义

**1459  C6.2.315   SUBG**

subtract with tag，MTE未开启，未定义

**1470  C6.2.321   SVC，Supervisor Call causes an exception to be taken to EL1**  不需要处理

调用 AArch64.CheckForSVCTrap(imm16) 先看HaveFGTExt （Fine Grained Trap），未实现，直接返回，调用AArch64.CallSupervisor(imm16); 行为不一致：EL0/EL2下都调用AArch64.TakeException(EL2, exception, preferred_exception_return, vect_offset)。

**1485  C6.2.330   TBNZ**

BranchTo

**1486  C6.2.331   TBZ**

BranchTo

**1512  C6.2.348   WFE** ？？

Hint_WFE 0/2下行为不一致

**1513  C6.2.349   WFET**

Feat_WFxT未实现，无定义，即使实现了，和WFE的情况相同

**1514  C6.2.350   WFI** ？？

Hint_WFI 0/2下行为不一致

**1515  C6.2.351   WFIT**

Feat_WFxT未实现，无定义，即使实现和WFI的情况相同

**1517  C6.2.353   XPACD, XPACI, XPACLRI**

Strip Pointer Authentication Code. This instruction removes the pointer authentication code from an address

调用 Strip，认为和EL无关。

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

+ ESBOperation:

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
+ Hint_PSB：Feat_SPE未开启，函数未实现
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

#### 未实现函数相关

0级 59，1级45，2级58，3级33，4级7，5级1

默认EL无关：RoundUp，RoundDown，physMemRead，ThisInstr，ProcessorID

+ 调用未实现相关函数V[]

```C
V[]调用 ConstrainUnpredictableBool(),里面调用未实现函数ConstrainUnpredictable()??处理不可预测的case，认为EL无关？？
同理 ConstrainUnpredictableInteger 也认为和EL无关
```

+ AuthDA/AuthDB/AuthIA/AuthIB/AddPACDA/AddPACDB/AddPACGA/AddPACIA/AddPACIB

  ```c
   因为trapPACUse而和未实现函数相关，当前配置下不会走trapPACUse分支
  ```

+ BranchTo

```C
调用未实现函数Hint_Branch，记录branchtype？EL无关？
```

+ softwarebreakpoint？

```C
调用takeException，里面调用未实现函数synchronizeErrors 和 TakeUnmaskedPhysicalSErrorInterrupts。
```

+ setTagCheckedInstruction/MemTag

```c
MTE未开启，这个函数不会被实际调用到
```

+ MemAtomic/MemAtomicCompareAndSwap

```c
调用TranslateAddressForAtomicAccess，里面调用未实现函数ClearExclusiveByAddress？？
```

+ CheckSPAlignment 

```c
调用SPAlignmentFault，里面调用takeException??
```

+ clearExclusiveLocal认为EL无关？
+ DCPSInstruction/DRPSInstruction：``只会被和halt相关的指令调用，该类指令此处为undefine，不需要考虑。``
+  AArch64.ExceptionReturn/AArch64.CheckForERetTrap：``被eret类指令调用，EL0和kernel app中均禁止其有该类指令，不需要考虑``
+ Mem

```C
Mem调用MemSingle，里面调用ClearExclusiveByAddress？？
```

+ AArch64.SetExclusiveMonitors

```c
调用MarkExclusiveGlobal 和 MarkExclusiveLocal，EL无关？？
```

+ AArch64.Abort

```c
调用的excpetion/abort函数最终调用了takeException？
```

+ CheckFPAdvSIMDEnabled64/CheckFPAdvSIMDEnabled/CheckFPEnabled64/CheckFPEnabled

```C
调用AdvSIMDFPAccessTrap，里面调用takeException？
```

+ Hint_WFE

```c
调用WaitForEvent，里面调用EnterLowPowerState和LocalTimeoutEvent，EL无关？？
```

+ Hint_WFI

```C
WFI相关两个指令都为（conditional）undefine，不会执行
```

+ Strip

```c
调用CalculateBottomPACBit，里面调用未实现的VAMax，返回的是当前处理器支持的虚地址位数的最大值，认为EL无关。
```

+ Hint_DGH（data gathering hint）

```c
查看ID_AA64ISAR1_EL1的DGH域，DGH未开启，DGH指令condition-equal
```

+ ESBOperation

```c
调用 
 AArch64.PhysicalSErrorSyndrome 未实现，EL无关？
 ClearPendingPhysicalSError 未实现，set ISR_EL1.A to 0, EL无关？
 IsSynchronizablePhysicalSErrorPending，未实现，EL无关？
```

+ hint_yield

```c
未实现，EL无关？
```

+ sendEvent

```c
未实现，EL无关？
```

+ SysRegRead/Write

```c
未实现，EL无关？ 仅被MRS调用
```

+ exclusiveMonitorsStatus

```c
EL无关？
```

+ exclusiveMonitorsPass

```c
 调用未实现函数：EL无关？
 ClearExclusiveLocal
 IsExclusiveGlobal
 IsExclusiveLocal
```

+ sysInstrWithResult

```c
未实现，EL无关？
```



### **指令编码**

![1668491782930](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668491782930.png)

#### C7 指令:  

###### 1、 data processing--scalar floating-point and SIMD所有：inst[27:25] = 111

###### 2、loads and stores下的SIMD部分：下图op1=1, inst[27:25] = 110

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668749798625.png" alt="1668749798625" style="zoom:80%;" />

#### C6指令：

###### 1、Data processing--immidiate 所有: inst[28:26] = 100

###### 2、Branches，Exception Generating and System instructions的大部分：除了System instruction/register move部分: inst[28:26] = 101

**（1）从conditional branch(imm)到exception generation:** 

**（2）从System instructions with register argument到PSTATE：**

均满足 op0 == 0b00 （C5.1.3）：CRn区分种类

![1668511133597](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668511133597.png)

+ hint：YIELD,WFE,PACIA,BTI等

+ Barriers and CLREX

+ MSR（imm)，access the PSTATE fields

**（3）从 Unconditional branch（register）到Test and branch（imm）：**

###### 3、loads and stores除了SIMD的部分：下图op1=0, inst[27:25] = 100

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668749731000.png" alt="1668749731000" style="zoom:80%;" />

###### 4、data processing--register所有: inst[27:25] = 101

#### C5指令：

###### 1、Branches，Exception Generating and System instructions 中的System instructions

+ C5.1.4 op0==0b01：cache maintenance, TLB maintenance, and address translation instructions，prediction restriction instructions

  ![1668507675095](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668507675095.png)

  满足该格式的还有C6的指令 1482 sys，Operation为	

  ```c
  AArch64.SysInstr(1, sys_op1, sys_crn, sys_crm, sys_op2, X[t]);
  ```

  实际上就是类似于MSR/MRS，C6的sys指令给定编码之后就是C5的指令。

+ op0==0b01：且L位为1，此时为1484 SYSL指令（system instruction with result），Operation也是执行C5的指令，按照C5的情况对其进行trap。

  ````c
  X[t] = AArch64.SysInstrWithResult(1, sys_op1, sys_crn, sys_crm, sys_op2);
  ````



#### MSR/MRS指令：

###### 1、Branches，Exception Generating and System instructions 中的 System register move（MSR（register）/MRS）

L=1表示读，L=0表示写

+ D12.2 op0==0b10，用于访问debug system register（D13.3中部分）

![1668509181749](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668509181749.png)

+ D12.3 op0==0b11 MSR/MRS: 用于访问所有的Non-debug register（13.2，13.4，13.5，13.6，13.7，13.8），special-purpose register和部分D13.3debug寄存器。CRn区分种类，对于sepcial-register CRn=4

![1668504565684](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1668504565684.png)



![1670340506117](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1670340506117.png)

#### 编码分类总结：

**system Inst：1101010100x01**

**MRS/MSR：    1101010100x1**

**Base/SIMD&FP Inst：不满足上述编码的情况**

| C7     | 27-25：111，27-25：110                             |
| ------ | -------------------------------------------------- |
| **C6** | **28-25：100x**                                    |
|        | **28-25：101x，且 !=1101010100x1,!=1101010100x01** |
|        | **27-25：100**                                     |
|        | **27-25：101**                                     |

#### 哈希算法：

**systemInst 和 MRS/MSR 中有21条可以执行。**

基于range的筛查方法，若不区分两类指令，即使最优，对于每一条指令也至少需要比较6次：system inst（上range界+下range界），MRS/MSR（上+下），base inst（上+下）。

基于哈希的筛查方法，第一次比较可以得出落在第一类还是还是第二类，然后进行hash计算（位运算），判断是否在阴影部分，若在还需要进行hash varify（一次比较，没有hash冲突），若不在，则不需要varify。总共比较两次，hash计算一次。

针对第一类的21条待筛查指令，hash算法为：-（inst[21:12] ^~ inst[11:5]），21条指令的哈希结果均不同，hash表大小大约4K，但是仅访问其中不到100B的内容。

**base 和 SIMD&FP inst 中有7条不可以执行** 。

展开MSR(imm)之后有10条指令编码需要阻止，hash方法为：

```c
b: 2bit
c: 倒数第2位
d: 后3bit
f：2bit

a      b  c            d       e   f
110101 10 100111110000 0011111 000 00
110101 10 100111110000 1x11111 111 11
110101 00 010xxxxxxxxx xxxxxxx 000 00
110101 00 000xxxxxxxxx xxxxxxx 000 10
110101 00 000xxxxxxxxx xxxxxxx 000 11
110101 01 000000000100 xxxx011 111 11
110101 01 000000000100 xxxx100 111 11
110101 01 000000110100 xxxx110 111 11
110101 01 000000110100 xxxx111 111 11
110101 01 000000110010 0000011 111 11
```

Map的每一位存储一个32字节的str，即该指令的编码（由01x组成）

减少表的大小优化：

+ hash出来的结果限制在了1024/256位，且设为unsigned int，scan时不需要越界判断。
+ 不用char存每一位的编码，因为每一位只有0或者1或者x，每一位只需要2bit就可以表示，总共需要64bit，即8字节，而不是现在的32字节。将表缩减为之前的4分之1。

fast path: 提前判断是否在某个区间/符合某个pattern，若符合直接continue，不需要后续的访存

+ 原本100次 1.25s，加上三个if判断 1.9s，加上三个if判断和对应的continue 1.34s。

### **SystemTrap 相关行为不一致指令**

部分指令在EL0下尽管走systemtrap分支（原本认为应该被拦截），但是其并不是 invalid 的，在 trap 的处理函数中，会对该指令进行模拟，其结果可能和EL2下执行的结果相同，可能不同。

**解决：**

**step1、找出 C5+MRS+MSR 中EL0下走systemtrap分支的指令：**

+ 原本EL0下有实际行为，通过decideBehavior系列程序决定是否走trap分支
+ 原本EL0下只有trap+(undefine)，decideBehavior系列程序中没有包括它们，增加对于其是否走Trap的判断。
+ 原本EL0下只有undefine，仍然是unconditional undefine Inst

（1）C5指令和MSR指令只有第一类和第三类情况，查看decideBehavior系列程序

（2）MRS存在第二类情况，即EL0下是trap+undefine的组合，且都为下列形式：因为FEAT_IDST开启，其在EL0下都走Trap分支。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1677074623169.png" alt="1677074623169" style="zoom:80%;" />

总结：

C5中 走trap的指令：AArch64.SystemAccessTrap(EL2, 0x18)

| C5.6.1 CFP RCTX, Control Flow Prediction Restriction by  Context |
| ------------------------------------------------------------ |
| C5.6.2  CPP RCTX, Cache Prefetch Prediction Restriction by Context |
| C5.6.3  DVP RCTX, Data Value Prediction Restriction by Context |

MRS中走trap的指令：AArch64.SystemAccessTrap(EL2, 0x18)

| FEAT_IDST的那些指令，见敏感指令v31 MRS 黄色和橙色部分        |
| ------------------------------------------------------------ |
| 411 C5.2.2  DAIF,  Interrupt Mask Bits                       |
| 3138 D13.2.34   CTR_EL0, Cache Type Register                 |
| 3671 D13.2.119   SCXTNUM_EL0, EL0 Read/Write Software Context Number |
| 3831 D13.3.6  DBGDTR_EL0, half-duplex                        |
| 3834  D13.3.7 DBGDTRRX_EL0, Debug Data Transfer Register, Receive |
| 3866 D13.3.16  MDCCSR_EL0, Monitor DCC Status Register       |

| 4202 D13.8.16   CNTP_CTL_EL0, Counter-timer Physical Timer Control register |
| ------------------------------------------------------------ |
| 4206 D13.8.17  CNTP_CVAL_EL0, Counter-timer Physical  Timer CompareValue register |
| 4210 D13.8.18  CNTP_TVAL_EL0, Counter-timer Physical  Timer TimerValue register |
| 4214 D13.8.19  CNTPCTSS_EL0, Counter-timer  Self-Synchronized Physical Count register |
| 4216 D13.8.20  CNTPCT_EL0, Counter-timer Physical Count  register |
| 4227 D13.8.25  CNTV_CTL_EL0, Counter-timer Virtual Timer  Control register |
| 4231 D13.8.26  CNTV_CVAL_EL0, Counter-timer Virtual Timer  CompareValue register |
| 4235 D13.8.27  CNTV_TVAL_EL0, Counter-timer Virtual Timer  TimerValue register |

MSR中走trap的指令：AArch64.SystemAccessTrap(EL2, 0x18)

|                                                              |
| ------------------------------------------------------------ |
| 411 C5.2.2   DAIF, Interrupt Mask Bits                       |
| 3671 D13.2.119   SCXTNUM_EL0, EL0 Read/Write Software Context Number |
| 3831 D13.3.6  DBGDTR_EL0, half-duplex                        |
| 3836  D13.3.8 DBGDTRTX_EL0, Debug Data Transfer Register, Transmit |

| 4202  D13.8.16  CNTP_CTL_EL0, Counter-timer  Physical Timer Control register |
| ------------------------------------------------------------ |
| 4206 D13.8.17  CNTP_CVAL_EL0, Counter-timer Physical  Timer CompareValue register |
| 4210 D13.8.18  CNTP_TVAL_EL0, Counter-timer Physical  Timer TimerValue register |
| 4227 D13.8.25  CNTV_CTL_EL0, Counter-timer Virtual Timer  Control register |
| 4231 D13.8.26  CNTV_CVAL_EL0, Counter-timer Virtual Timer  CompareValue register |
| 4235 D13.8.27  CNTV_TVAL_EL0, Counter-timer Virtual Timer  TimerValue register |

**step2、对于确定走Trap分支的指令，实测其trap行为：invalid，有行为但不一致，行为一致**

见敏感指令v31表格SystemTrap的指令

源码：

/home/isec/Workspace/modified-linux-5.19-rc6/arch/arm64/kernel/traps.c sys64_hooks处理用户态执行以下指令的情况：

+ cache maintenance：EL0 undefine 或者 MTE未开导致undefine 或者可以执行

+ MRS CTR_EL0：Trap，EL0下和EL2下的值不同

+ MRS CNTVCT_EL0：可以执行

+ MRS CNTVCTSS_EL0：可以执行

+ MRS CNTFRQ_EL0：可以执行

+ MRS CPUID registers：进入emulate_sys_reg函数处理，若返回值非0，则不处理，认为其是 invalid instr（即不满足is_emulated的指令）。

  <img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1677318325111.png" alt="1677318325111" style="zoom:67%;" />

  (1) is_emulated： 只模拟 Op0 = 3，CRn = 0，Op1 = 0，CRm =  [0, 4~7] 的MRS register 指令。即以下寄存器：

  + CRm = 0: MIDR_EL1, MPIDR_EL1, REVIDR_EL1
  + CRm = 4: ID_AA64PFR0_EL1, ID_AA64PFR1_EL1, ID_AA64ZFR0_EL1
  + CRm = 5: ID_AA64DFR0_EL1, ID_AA64DFR1_EL1, ID_AA64AFR0_EL1, ID_AA64AFR1_EL1
  + CRm = 6: ID_AA64ISAR0_EL1, ID_AA64ISAR1_EL1, ID_AA64ISAR2_EL1
  + CRm = 7: ID_AA64MMFR0_EL1, ID_AA64MMFR1_EL1, ID_AA64MMFR2_EL1

  (2) emulate_id_reg: 对于CRm=0的三个寄存器，MIDR返回真实值，MPIDR返回模拟的safe值，REVIDR 是 implementation defined 的，返回0。

  (3) get_arm64_ftr_reg_nowarn: 在 arm64_ftr_regs 中查找该 feature register 的 entry 

  (4) arm64_ftr_reg_user_value:

  <img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1677319680418.png" alt="1677319680418" style="zoom:80%;" />

+ WFI

**step3、对于C6,C7需要阻拦的指令，判断是否有因为trap而阻拦的情况**





上下文切换变化的寄存器：

spsr：

pstate



内核态app和同时用户态app的寄存器值是一样的：对于内核和用户上下文切换时保存的寄存器，在运行内核态app的时候会恢复它们，然后内核态应用又不可以修改寄存器（除了NZCV等可以修改的，就是应该变化的寄存器），那么我们在EL2下运行内核态app时看到的寄存器值就会和EL0下app看到的寄存器值相同。

问题：有没有寄存器，内核会改，但是在内核和用户态进行切换上下文的时候不save restore，对于这类寄存器，内核改了之后，内核态app和用户态app看到的值都是改了之后的值。

如何保证在EL2下读出的寄存器的值和EL0下应用见到的值和内核态app见到的值相同：

+ 内核态app因为save restore和阻止恶意MSR的机制，保证和同时间的用户态app的寄存器环境相同
+ 大部分寄存器初始化后不会修改，当前读取的EL2下的值和EL0下的值相同；对于部分会save restore的寄存器，需要读出其EL0下的值当作condition，进行指令行为判断；对于那些不会save restore的，但是OS会修改的寄存器（用户态没有MSR能力修改），用户态app要么不可能用到，要么是自己想要改的，不存在问题。对于其它的情况，用户态app和内核态app看到的值仍然相同，但是当前EL2下的值是没有修改的，在check的时候会发现修改了。

base instrution 筛查指令的时候，确定为condition的寄存器只需要是那些在EL0/EL2下看的不同的寄存器，例如BigEndian。

注意：takeException和fulltranslate之类函数，在kernel app中处理了，此处认为其行为相同



扫spec，将所有代码段转储到文件中，扫文件



优化：hash前先比对一次，落在某个区间再hash；局部变量用寄存器存





非敏感指令：EL0下有行为且与EL2下行为一致

敏感指令：EL0下存在 undefine/systemtrap 或行为与EL2不一致的情况

+ 无条件敏感指令：EL0下一定是undefine（不管EL2，即EL0&EL2都是无条件undefine，比如EL3下才可以有的指令，也是无条件敏感指令）

+ 条件敏感指令：EL0在部分配置下才是undefine/systemtrap/与EL2不一样的行为



逻辑：

C5/D13（sys+register）：

+ 先看EL0&EL2下是否行为一致（有行为的一致和都是undefine），归为非敏感指令
+ 再看EL0下是否是undefine，EL2下有行为，归为无条件敏感指令
+ 剩下的均为有条件敏感指令：
  + 先判断configure 里面的 extension 是否支持，若不支持，则归为config
  + 若支持或没有extension要求：
    + 若EL0下有行为（不是trap/undefine）且和EL2下一致，归为config
    + 若EL0下走trap分支，且trap=invalid，归为config&filter
    + 若EL0下走trap分支，且trap后的结果和EL2下的结果相同（只有MRS指令），若EL0下走trap分支，且trap后的结果和EL2下的结果不同（只有MRS指令），归为config&trap

