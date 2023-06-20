[TOC]

## 组内工作

攻击：漏洞挖掘（软件的安全漏洞分析：hyh，js引擎），漏洞利用（传怎样的输入给漏洞）

防御：数据保护（bac，利用硬件，保护指针防止篡改，密钥，分析linux的内核数据生命周期，不用的数据应该把其释放，读写权限关掉，有些数据只读）、代码保护（写同一个源程序，生成不同的二进制程序，防止别人进行源分析）、中关村实验室操作系统加固（代码持续随机化，模块隔离）

+ 代码混淆：
  二进制混淆，病毒可执行程序修改，识别不出来

  源代码混淆，每次编译出来都不一样，LLVM

+ 数据保护：
  + 变量的检查开关关掉，free，加完整性校验 ////
  + 内核态app，让进程使用特权的硬件，让进程自己使用页表，用户进程，JVM进行用户的GC，GC检查页表的访问情况，进行修改，用户运行在客户机的特权态，操作系统使用宿主机的特权态//M1 arm，pac /// LLVM+linux kernel，arm，不是x86，
  + BPF////
  + 体系结构安全，熔断幽灵，
  + 增强rust安全性////纯编译器
  + 代码随机，二进制代码变位置，操作系统解耦，返回地址的监控，

内核态app：

+ 保证功能性：arm异常（svc、IRQ、FIQ），arm异常处理，arm特权指令构成
+ 保证安全性：
  + 内存：app不能随意访问内核空间的数据和代码（os并不知道有用户运行在内核态，对os透明），通过隔离（isolation），arm 硬件特性：pac，mte
  + 特权指令：过滤filter安全门，通过安全门执行特权指令，有些指令（资源）可以，有些不行。
    + loader 扫描二进制文件，看有没有特权指令，（x86变长指令，修改指针，可以有原本没有的指令。arm固定长度指令，四个字节），之前组内做x86 内核态 app，利用虚拟化，虚拟化里面特权级，arm更加简单，直接跑在内核
    + elf：text段，rodata（程序用到的只读数据，理论上只可读，但是代码量比较少时，和text等放在一页中，就有了执行权限，pc跳到这里，就可以通过构造执行特权指令；编译器优化过程中，有的数据和代码段混合编在一起，就有了执行权限），plt。代码页数据的分离
+ 应用场景：JVM猜使用页面的情况，可以设计接口，直接告诉os，回收合并。app缓冲区溢出，使得攻击者可以读到key，将key存在内存特定区域，通过开关权限，syscall，改为直接通过安全接口。



四个SP，不用压栈，ELx可以用比等于小于的SP，实际上EL1还是用SP_EL1

内核态app移到os，还是使用SP_EL0，替换原本没用的Current exception level with SP_EL0的处理，跳到原本的用户异常处理

关闭内核空间，将临时向量表放到用户空间，先跳到临时向量表，然后开内核，跳到内核处理，再关内核，再eret，EL1-EL1。TLB命中时，不管后面内核size的检查，于是利用ASID机制，将内核空间和用户空间的ASID区分开来，阻止用户空间访问内核ASID的TLB项地址内容。需要访问内核时，临时向量区域会改为内核的ASID,注意不能不改，直观上只会造成多的miss，但是利用用户ASID加载页，返回之后用户态可以访问这些TLB项

用户空间通过异常到了临时向量表，会改SP，其它恶意挑战，SP不变，内核空间执行一段就检查一次

每当有可执行的页面出现：监控改页面的执行权限的系统调用，mmap，将其map到可执行的区域，execv执行完，即该进程的空间都分好，在调度之前，进行检查

硬件的DEP：WXn，若一个页有可执行可写权限，则屏蔽可执行



M1芯片，上不用mac os，改为linux（3月），双系统，加kenel moduler。不能调gdb，只能print，硬件串口调试，



MMU notifier：部分写PTE 的access flag未触发，；写保护页，权限由读变为读写，未触发

flush asid+1解决

KVM

LSB shared object/executable，代码无关



**毕设：**

静态二进制扫描，JIT编译器生成的代码扫描，快速。

二进制重写，根据筛查出来的指令，重改ELF。

segment section，每个函数之后emit constPool，数据和data编在一起



手册+决策树，先分为一样和不一样，再说可以根据环境的配置，将一些不一样的，变成了一样的，且没有需要模拟的，通过特权寄存器阻止了行为不一样，

代码加载的时候扫描，给定一个编码，快速识别，找到它是，二进制编码，看op是否在一个range里面，如果有，则不执行，

JIT 既有代码又有数据，怎么知道是代码，在os将其设置成可执行时，需要扫





背景：进程下放

线下扫描：替换成0。扫描代码段，（代码段中内嵌了数据，避免内嵌数据（二进制重写））

线上：verify 扫描可执行页中

load elf 和所有生成可执行页的操作前进行hook



spec中不会生成这些指令，报错的工作不可验证，自己加一个例子。



可执行权限数据（且满足特定编码）的消除——二进制方案

text section中的内嵌数据，rodatat和text编在一起，一些小section也会和text编在一起

内嵌数据的识别 NORAX

rodata拷贝出来，放在新的部分，然后重定位（PC+XX访问到这个数据，应该改数据引用指令的offset），只需要拷贝和重定位text同一页中后面的rodata，其它的被编为可执行的，将其权限修改。剩下的text改为只可执行。如果读原来的rodata段（将其改为0），会报page fault，处理它。静态，直接寻址，可以看出来，可能会有遗漏，采用动态重定位。



本来是只可读，和text编在一起后，会变得可执行。

磁盘上也有可能被攻击，load的时候：
./a.out，excve 创建进程，分配页表，设置映射等。load elf，

mmap没有映射，只有第一次访问该页的时候，触发page fault，才新建立映射。所有设置可执行页的时候要进行扫描。mmap写一个页，之后将其改为可执行。

do_fault_around，局部性原理，page fault一次，将其后的页的映射也给只上，扫描所有被置为可执行的页。

指令分类表格

kernel app背景



**扩展：**

指针保护/指针完全性

x86没有PAC机制，可以在内存中设置安全区（内存和安全区一比一的映射，源指针地址加一个offset就可以找到安全区的它，但是一比一很浪费，大部分地方是空洞，少数部分是指针，用hash算法压缩安全区，hash的输入是指针地址，通过hash后的值索引），备份敏感指针，保证其不会被修改，在解引用的时候，比较安全区中的内容和当前的指针值是否相同。

外面的指针产生，传播，使用，free，在安全区中都需要同步进行，

安全区的隔离，可以通过页表bit的设置，mpk

隔离为主，PAC为辅：利用PAC（输入context是指针地址，和key）替代hash，加快索引

PAC为主，隔离为辅：将context相同的指针放在安全区（实际上要放的是context相同，value不同的，context相同和value相同的，不需要保护）。这样子context的指针非常多，应该从指向的对象是否是一个来判断，看new的情况。代码指针不需要隔离，数据指针需要隔离，

函数指针，pac时context可以用function id，但是auth时只知道调用函数的类型，静态不可行

那些可以复用的指针，怎样设置好context，将相关的内容放在保护区中，可以动态计算，用的时候从保护区中获取数据进行计算。

## 存储器

半导体存储介质主要指 ROM 和 RAM。RAM通常用于内存，断电则数据丢失。ROM是不可擦写的，后续衍生出了EPROM 和 EEPROM，可以擦写（不符合 readonly memory性质了，但是沿用旧名字），后来又衍生出了 NAND FLASH 闪存，应用于U盘和手机存储。

磁盘分为固态硬盘（即磁盘，采用磁性存储，SSD）和机械硬盘（HDD），固态硬盘用的颗粒是基于 NAND FLASH 技术的，和u盘和手机存储类似，即和ROM有关

DRAM 用于内存， SRAM 用于 cache，速度快于 DRAM

## 编译器

Clang 和 gcc 都是c/c++的编译器， Clang 是编译器前端，采用底层虚拟机 LLVM 作为后端。相比GCC, Clang编译器速度快，内存占用小，诊断信息可读性强，兼容性好。Open64也是一个编译器

+   统编译器的工作原理，基本上都是三段式的，可以分为前端、优化器和后端。前端负责解析源代码，检查语法错误，并将其翻译为抽象的语法树；优化器对这一中间代码进行优化，试图使代码更高效；后端则负责将优化器优化后的中间代码转换为目标机器的代码，这一过程后端会最大化的利用目标机器的特殊指令，以提高代码的性能。 

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1662449605534.png" alt="1662449605534" style="zoom:67%;" />

SSA：静态单赋值，对某个变量的每次赋值用一个新变量名v1,v2……，每次引用采用最近的赋值的变量名。更新变量名后，每个变量只有一次静态赋值。有控制流的SSA，在if语句后，插入$\Phi$函数，$v3 = \Phi(v1,v2)$选择一个v作为后续使用。

RTT：Register Transfer Language，

交叉编译：

Build平台（X）：编译GCC的平台

Host平台（Y）：运行上述编译得到的GCC平台

Target平台（Z）：GCC生成的本地吗所属的平台

X=Y=Z：本地安装

X=Y!=Z：交叉编译

X!=Y!=Z：canadian 交叉编译

#### 栈中存储内容与格式：

活动记录也称为帧

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671356348021.png" alt="1671356348021" style="zoom:80%;" />



<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671356669725.png" alt="1671356669725" style="zoom:80%;" />

实际x86中有栈顶指针寄存器 rsp 和栈底指针寄存器 rbp





## 虚拟化

将一台计算机虚拟为多台逻辑计算机，实现资源的模拟，隔离和共享。整个虚拟机都保存在文件中，可以通过移动文件的方式迁移。

#### 虚拟化分类：

+ type1 virtualization，基于硬件的虚拟化：比如把x86平台的CPU，内存和外设作为资源，在同一个x86平台上可以虚拟多个x86平台，每个平台运行自己独立的操作系统。hypervisor架构，运行于x86裸机上。
+ type2 virtualization，基于操作系统的虚拟化：把操作系统及其提供的系统调用作为资源，例如Linux容器虚拟化，在同一个Linux操作系统上，虚拟出多个Linux操作系统。hosted架构（指在host os上建VMM），运行于host os上。例：lxc，docker

#### 虚拟化管理程序Hypervisor

以下均为type2虚拟化

（hypervisor上对应每一个虚拟机有一个**VMM，Virtual Machine Monitor**虚拟机监视器，VMM实现了虚拟机的硬件抽象并负责运行虚拟机系统）：

Hypervisor是一种运行在物理服务器和虚拟机客户端间的”元“操作系统，用于加载所有虚拟机客户端的操作系统，分配资源，并在不同虚拟机间施加防护，进行调度等。

 <img src="https://img-blog.csdn.net/20150418215204355?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQvZmx5Zm9yZnJlZWRvbTIwMDg=/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center" alt="img" style="zoom:80%;" /> 

特权指令：这些指令只有在最高特权级上能够正确运行，如果在非最高特权级上运行，可能会产生异常，然后陷入最高特权级进行执行/处理，也可能直接忽略。

敏感指令：操作特权资源的指令，包括修改虚拟机/物理机的运行状态；读写时钟、中断等寄存器；访问存储保护系统，地址重定位系统及所有的I/O指令等。

**后续所说的敏感指令：包括上面两种**

#### x86虚拟化

没有虚拟化时的特权架构为：

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666963513024.png" alt="1666963513024" style="zoom:50%;" />

虚拟化：GuestOS的特权被解除（privilege deprivileging，OS本应该运行在ring0），从ring0降到ring1/3，GuestOs中特权指令的执行将触发异常，并被VMM（host os中的一个kernel module）截获，VMM 为guest os模拟该特权指令。若是guest app的系统调用，同样先由host os中的VMM捕获，由VMM判断是由guest os处理还是自己处理，若是guest os处理，还会为其准备好环境，guest os处理完之后，先会返回VMM，再返回到应用。

<img src="C:\Users\24962\Desktop\1671967288705.png" alt="1671967288705" style="zoom:200%;" />

由此，虚拟化时敏感指令必须被VMM捕获完成。arm/x86架构均存在不是特权指令的敏感指令，且在ring0下和ring3下语义可能不同，即虚拟化漏洞。问题：如何在运行时陷入并翻译敏感指令和特权指令（即虚拟化这些指令），有以下三种解决方案：

+ 使用二进制翻译的全虚拟化：VMM对guest os中的二进制代码进行扫描，一旦发现guest os执行的代码中包含有特权指令二进制代码时，将这些二进制代码翻译成虚拟特权指令，或者翻译成运行在核心态中的特权指令，从而强制触发异常。异常陷入后，使用一系列作用于虚拟化硬件可达到效果的新指令序列替换那些不可虚拟化的指令。hypervisor将操作系统的指令翻译的结果缓存供之后使用。其余用户级指令无需修改直接运行在物理处理器上。模拟CPU让虚拟机使用，可以实现ARM平台模拟出x86平台这样，qemu，效率低。

  <img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666960968392.png" alt="1666960968392" style="zoom: 50%;" />

+ 操作系统辅助的虚拟化/半虚拟化：修改guest os kernel，将不可虚拟化的指令对应的调用（例如TLB_flush()）替换为直接与虚拟化层交互的hypercalls。例：Xen

  <img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666962210649.png" alt="1666962210649" style="zoom: 50%;" />

+ 硬件辅助的全虚拟化：Intel的VT-x和AMD的AMD-V，为CPU新增了一个执行模式root，用于运行VMM，VM 运行在 non-root模式下。部分特权和敏感调用自动陷入hypervisor（大部分指令可以直接执行，因为non-root模式下，用的都是虚拟化后的硬件），不需要二进制翻译或者半虚拟化，虚拟机的状态保存在虚拟机控制块中（VMCB,AMD-V；VMCS,VT-x）。只修改了CPU，还需要对网卡，存储使用虚拟化驱动程序。例：KVM， KVM通过在HostOS内核中加载**KVM Kernel Module**来将HostOS转换成为一个VMM。所以此时VMM可以看作是HostOS，反之亦然。 

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666962382660.png" alt="1666962382660" style="zoom:50%;" />

**内存虚拟化：**

VMM负责将VM的物理内存PA映射到硬件的机器内存MA，并使用影子表来加速映射过程（影子表指的是VPN-MFN的TLB）：红线表示VMM使用硬件中的TLB来直接映射虚拟内存到机器内存，当VA到PA的映射改变时，VMM更新影子表。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1666963201513.png" alt="1666963201513" style="zoom:67%;" />

VMM 截获 TLB 异常：

mips处理器 TLB miss会产生异常

![1671968164919](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671968164919.png)

相对应地，需要有**shadow page table：VPN-to-MFN（machine）**，guest os的表为VPN-to-PFN。下图右侧的Host page table部分（也由VMM管理）为硬件辅助虚拟化的功能

![1671968701602](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671968701602.png)

对于TLB miss 不产生异常（即miss之后由硬件自动查页表）的情况怎么处理：

![1671968484694](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671968484694.png)



**设备和I/O虚拟化：**



硬件辅助的虚拟化：虚拟化漏洞：部分操作系统资源的指令在用户权级也可以执行（但可能执行行为不同，比如用户态下被当作没有行为，实际上应该由VMM捕获，并实现其行为），即不会被捕获。以及跳转指令等，也不会被捕获，但是我们得限制跳转的范围等。

![1671972105961](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671972105961.png)

![1671972408946](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671972408946.png)







#### arm64虚拟化

硬件辅助虚拟化架构

VHE（virtualization host extensions）开启后，host os和hypervisor运行在同一级或者更高级。hypervisor运行在EL2，原本OS运行在EL1。支持VHE特性，将host os kernel移到EL2后，可以减少EL1和EL2直接模式切换的次数，否则KVM部分必须在EL2执行的代码（比如捕获guest os退出异常）在EL2执行，其它的KVM代码和host os都在EL1执行，这样hypervisor在进行资源管理和虚拟机调度时需要在EL1和EL2间进行切换。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1667027338356.png" alt="1667027338356" style="zoom:67%;" />

HCR_EL1的E2H位表明是否VHE使能，TGE位表明当VHE使能时，EL0运行的应用时guest还是host的。

```c
E2H
EL2 Host. Enables a configuration where a Host Operating System is running in EL2, and the Host
Operating System's applications are running in EL0.
TGE
Trap General Exceptions, from EL0. All exceptions that would be routed to EL1 are routed to EL2.
```



<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1667029006370.png" alt="1667029006370" style="zoom:50%;" />

  

**arm64 cpu 虚拟化：**新增EL2级，VMM运行在EL2，guest os运行在EL1，应用运行在EL0。EL2打开的模式下，EL1和EL0下的特权操作会陷入到EL2模式下（CPU自动检测），VMM模拟这些特权操作。ARM64为一部分寄存器提供虚拟寄存器，guest os读取虚拟寄存器，从而加快虚拟机的切换过程。增加HVC指令，让guest os必要时可以主动进入EL2执行，类似系统调用。

**arm64内存虚拟化：**开启虚拟机时，EL0/1的地址转换分为两个阶段：stage1：guest os将进程的虚地址转化为IPA；stage2：VMM把IPA转换为真正的物理地址PA。EL2的地址转发是直接由VA转变为PA。

在VHE引入之前，虚拟地址空间布局如下：EL2仅有一个区域，因为hypervisor不会host应用。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1667031524929.png" alt="1667031524929" style="zoom: 67%;" />

当E2H为1时，即将host OS放到EL2上时，布局改为下图所示：TGE位控制os运行在EL1（guest os）还是EL2（host os）

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1667031659742.png" alt="1667031659742" style="zoom:67%;" />

此外还需要重定向寄存器访问：E2H为1时，内核访问EL1寄存器时将重定向到相应的EL2寄存器，但是因为hypervisor仍然需要访问真实的EL1寄存器，以实现上下文切换，引入一组新寄存器EL12,EL02，用于在EL2下访问真正的EL1寄存器。



## ARM

CPU根据不同的指令集类型可以分为：

+ RISC：代表处理器（CPU）有Sun公司的SPARC系列、IBM的Power Architecture（Power PC）与ARM系列，mips
+ CISC：代表有AMD、Intel、VIA等x86指令集的CPU，这样的CPU大量用于个人计算机，个人计算机常被称为x86计算机，linux内核的底层就是x86架构的cpu。

### 异常

ARM架构根据 exception level分为EL0（app）,EL1（kernel）,EL2（hypervisor/virtual machine monitor）,EL3（secure monitor，在trustzone切换过程中的mode）。一般情况下EL0是执行在unprivilege mode下的，其他三种都是执行在privilege mode下的，需要配置自己的TCR寄存器。

arm提出trustzone之后，定义了两种physical address space，在理论上，secure和non-secure的physical address应该是完全分开的两个地址空间，甚至存储器，但是在实现中，多通过bus attribute来进行控制。表示的是同一块地址，在进行secure和non-secure切换的时候，需要自己保存上下文。

要想改变执行状态，AArch64与AArch32之间的转化，只能在陷入异常或者从异常返回时才可以改变。从低的异常级别陷入到高的异常级别时，可以保持执行状态不变也可以从AArch32变为AArch64；反之类似。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1667031323165.png" alt="1667031323165" style="zoom:80%;" />

+ 低level向高level切换通过exception的方式转换，有如下exceptions，expection entry时会写好ELR寄存器，用于异常的返回

+ 高level向低level则通过ERET指令；

异常分为同步异常和异步异常两种：

+ 同步异常：由刚刚执行的指令或者与之相关的操作引起，异常同步于执行流。例如：执行了无效的指令，内存访问地址未对其，MMU权限检查出错。异常生成指令：SVC,HVC,SMC用于帮助系统实现接口调用，允许权限较低的代码向权限较高的代码请求服务。

1. SVC for transition to EL1 (system calls)
2. HVC for transition to EL2 (hypervisor：超级监督者 calls)
3. SMC for transition to EL3 (secure monitor call)

+ 异步异常：外部产生的，ARMv8需要它在一个限定的时间内产生，异步异常可以被短暂屏蔽。比如物理中断（SError系统错误,IRQ,FIQ），虚拟中断（由执行在EL2的软件产生，vSError,vIRQ,vFIQ）。IRQ和FIQ均用于外设中断，用于安全和非安全中断。SError指由内存系统生成的响应去反应错误的内存访问，例如通过了MMU权限检查但在内存总线上遇到错误的内存访问，或对RAM的奇偶校验码或纠错码ECC检查出错。

异常处理：

先跳到向量表一个位置，该位置包含通用代码，负责将当前程序状态压栈退栈等，再跳转到进一步处理中断的代码，之后再回来。ELR （exception link register）：保存exception返回地址到ELR_ELx中，x是异常跳转到的EL的层级；SPSR （saved processor state register）： 执行exception前保存当前的PSTATE到SPSR_x中，执行exception完返回时，SPSR会被copied到PSTATE。

异常跳转到的EL的层级由更高的EL来决定：且SCR_EL3(Secure Configuration Register)可以屏蔽HCR_EL2(Hypervisor Configuration Register)

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1665469471067.png" alt="1665469471067" style="zoom:80%;" />

不同的level下，有不同向量表地址，向量表基址存在VBAR_ELx中，向量表是包含指令的普通内存区域：根据exception type（synchronous，IRQ，FIQ or systerm Error）或者exception origin（same or lower exception level）和register width来区分不同的异常的offset，syndrome寄存器提供了exception 信息。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1665469778164.png" alt="1665469778164" style="zoom:80%;" />

### MMU与虚拟内存布局

 ARMv8理论上最高可以提供提供了2^64个虚拟地址，但是超过16 Exabyte （2^4 * 2\^60）意义并不大，所以选择跟x86一样，可以使用最大支持2\^48虚拟地址的寻址范围就足够； **使用TTBR0存储user-space的page table地址，TTBR1存kernel-space页表地址**；根据TCR_EL1.T0/T1SIZE 来表示高bit的位数。VA的最高有效位（MSB）为0时，MMU使用TTBR0的转换表来翻译，VA最高位为1时，MMU使用TTBR1的转换表来翻译。**EL0用TTBR0_EL1, EL1用TTBR1_EL1，EL2和EL3有TTBR0_EL2和TTBR0_EL3，但是没有TTBR1，只能使用下面的虚拟地址范围。**

![1663601024761](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663601024761.png)



![1663589363718](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663589363718.png)

页表的每个entry都定义了 memory attributes 域：AF:acess flag, SH:shareable, AP: access permission, NS: no security, index: 指向MAIR_ELn

 <img src="https://pic2.zhimg.com/v2-47e31e79c1947c167114838d5d9d10b5_r.jpg" alt="img" style="zoom: 67%;" /> 





translation:启用了hypervisor的情况：

![1663589600576](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663589600576.png)



**内核虚拟空间布局：**

arm64 linux 在4KB页大小+4级页表（va为48位）时的内存布局如下：
<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1676529167757.png" alt="1676529167757" style="zoom:80%;" />

**内核空间低128TB是线性映射区，其虚拟地址VA和物理地址PA间可以通过线性关系转化**（这段映射在内核初始化时建立）。所有的物理内存都会被映射到这一段，即内核可以通过访问该段虚拟地址访问所有的物理内存。提供了一种访问用户空间地址的方式：内核不能直接访问用户虚拟地址，可以通过copy_from_user，或者访问映射到同一个物理页的内核虚拟地址来访问用户空间的内容。

```c
/* arch/arm64/include/asm/memory.h */
/*linux内核函数 __pa(x)和__va(x) 用于物理地址和内核虚拟地址的转化*/
#define __pa(x)   __virt_to_phys((unsigned long)(x))
/* 如果没有定义CONFIG_DEBUG_VIRTUAL宏时，__virt_to_phys定义如下 */
/*
__tag_reset(x)去掉了虚拟地址中高位的tag
__is_lm_address(x)判断虚拟地址是否落在内核线性映射区，如果是，采用__lm_to_phys(x)将其转化为物理地址，否则通过__kimg_to_phys(x)

*/
#define __virt_to_phys(x) __virt_to_phys_nodebug(x)
#define __virt_to_phys_nodebug(x)       \
 phys_addr_t __x = (phys_addr_t)(__tag_reset(x));  \
 __is_lm_address(__x) ? __lm_to_phys(__x) : __kimg_to_phys(__x); \

/*
 * Check whether an arbitrary address is within the linear map, which
 * lives in the [PAGE_OFFSET, PAGE_END) interval at the bottom of the
 * kernel's TTBR1 address range.
 */
/*
对于va为48bit的内存分布:
PAGE_OFFSET = (-(UL(1) << (48))) = 0xFFFF000000000000,
PAGE_END    = (-(UL(1) << (48-1)))  = 0xFFFF800000000000
即PAGE_OFFSET就是此时的内核空间的起始地址，PAGE_END是线性映射区的结束。
*/
#define __is_lm_address(addr)	(((u64)(addr) - PAGE_OFFSET) < (PAGE_END - PAGE_OFFSET))

/*对于线性区域的地址翻译，加上偏移即可*/
#define __lm_to_phys(addr)	(((addr) - PAGE_OFFSET) + PHYS_OFFSET)
#define __kimg_to_phys(addr)	((addr) - kimage_voffset)

```





### 栈溢出攻击与防御

栈溢出攻击：局部变量放在栈上，通过buffer的溢出，可以修改栈上其它的内容，然后再执行新写进去的内容，或者跳转到新写进去的地址执行。

通过在页表上加上执行权限的设置，可以防止某些页被执行：页表（页目录）上有**UXN**（User Execute-nerver，EL0）和**PXN**（Privileged Execute-nerver，EL1），XN（Execute Never）；SCTLR_ELx中还提供了控制位，使得**所有可写地址都不可执行**，这样，堆栈空间内容是不可执行的。

详情见 [D4.4.1 Memory access control · ARM Ar­chitec­ture R­eferen­ce Man­ual fo­r ARMv­8-­A (codingbelief.com)](https://armv8-ref.codingbelief.com/zh/chapter_d4/d44_1_memory_access_control.html#) 

![1671607970250](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671607970250.png)



<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663422202380.png" alt="1663422202380" style="zoom:67%;" />

对于可写地址不可执行的约束由SCTLR_ELx的WXN位控制，SCTLE_EL1的WXN位控制EL0和EL1的执行权限。

![1671609030533](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671609030533.png)

#### ROP，JOP

基于此防御机制，Return-oriented programming 和 Jump-oriented programming的改进，返回地址或者跳转的地址来源于相应的寄存器，攻击者可以通过修改栈中的内容，进而修改寄存器的内容（在A函数调用函数B时，会将返回地址压栈，B函数返回时，先从栈中找到之前存的地址，然后将其放到返回地址寄存器LR中，然后再return，由此在函数B的buffer溢出时，就可以修改对应的返回值）。跳转/返回到新的地址后，执行gadget，再跳转……，形成gadget串，就可以实现执行一个有效的由现有代码片段组成的新程序（之所以要现有的代码，是因为可写内存是不可执行的，不能够执行新的写的代码，找gadget流程（c库函数中有丰富的可用的gadget）可以自动化并且多次使用，地址空间随机化ASLR可以防止自动和多次攻击）。开始的几个 gadget 应该实现给rax，rdx等寄存器赋想要的值的功能，都准备好了之后，才可以继续跳转到想要的地方。CFI 控制流完整性，按照本意的控制流执行，不能走别的路，通过静态和动态检查，知道某个指针到底指向谁，跳过去的时候，检查是否是原来指向的东西。直接跳转改不了，主要针对间接跳转，可以篡改。

+ MTE：不可以越界/free后写；
+ PAN：跳转地址前后不能变，符合某pattern；
+ BTI：间接跳转后一定要到BTI指令；
+ PAN：kenel不可以访问（读/执行/写）user space的代码

#### MTE

**MTE（Memory Tagging Extension），阻止数组越界访问和释放之后再访问。**空间局部安全性：内存对象的访问超出了该内存对象所允许的最大有效范围，数组越界。时间局部安全性：当一个内存对象被释放之后，再次访问：

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663427470600.png" alt="1663427470600" style="zoom:80%;" />

**在分配内存的时候，就给地址的高位（4bits）打tag，指针访问时需要比较指针中的tag（key）与实际内存的tag（lock）是否匹配。**MTE使用Armv8-A体系结构的最高字节忽略（TBI）功能。启用TBI后，将虚拟地址的用作地址转换的输入时，将忽略该地址的高位字节。这允许高位字节存储元数据。图中一个tag能够管理的内存颗粒大小是16字节。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663429103724.png" alt="1663429103724" style="zoom:67%;" />



#### PAC

PAC：指针以64位存储，但是前12位都是000（kernel space地址）或者FFF（user space地址），利用这些**高位可以存储签名（pointer authentication code），存储返回地址时（不一定是存储时，后续觉得要用的时候，给它加上pac，真正用的时候auth），对返回地址进行签名，存在栈中（内存），若返回时的寄存器中的返回地址没有通过PAC认证，则报异常。（用于indirect branch，load）**签名用指令**PAC**,验证用指令**AUT**。pac = mac(pointer, key, modifier)，不同的pointer可以选用不同的key（总共5个，两个用于instruction pointers，两个用于data pointers，一个通用），modifier可以是一些context信息，例如此次function call的Stack pointer(SP，指向栈顶)信息。每个running application可以使用不同的key，相同application每次运行被分配不同的key。

放在高位：除了因为高位不用，还因为方便指针的拷贝/传播

指令指针：context存SP，指针（返回地址）的位置，a调用b，然后c，SP相同，漏洞 

数据指针：context指数据结构的类型，不存指针的位置，因为指针传播过程中，地址会变，那么每次传播都得先验证，再生成pac。若int*变为char\*，类型变化了，还是需要重新pac。问题在于：相同类型的指针，int a, int b, ptra  = &a, ptrb = &b，ptra，ptrb都加上了pac，本来希望得到b的地址，若将ptra的值赋值给ptrb之后，对ptrb的解引用，不会fail，得到的a的地址，访问a的内容。即相同context的指针可以替换，漏洞。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663423389981.png" alt="1663423389981" style="zoom:67%;" />

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663429491065.png" alt="1663429491065" style="zoom: 67%;" />

#### BTI

**BTIs（Branch target instruction）也称为landing pads，使得间接分支(BR和BLR）只能跳转到着陆台（BTI指令）**，着陆台有参数（比如BTI c，BTI j），指明了它可以接受哪些类型的间接跳转指令。原本利用的是库中已有的代码作为跳转目标，但是现在限制必须跳转到BTI指令，那么就只能使用库中有BTI指令的gadget，并且这里BTI还做了细化，必须要特定参数的BTI，找gadget难度增大。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663423684150.png" alt="1663423684150" style="zoom:67%;" />



Intel 的 SHSTK 和 IBT，编译时在函数开头插入ENDBR指令，每次间接跳转过来之后，要碰到ENDBR才可以继续走，不能限制跳到别的函数开头，但是可以限制跳到不是函数的地方（函数内部）。







#### PAN

**指不让特权态访问非特权态的内存，即不让EL1访问EL0的内存**。背景：用户只能看到和执行自己内存空间内的内容，但是内核往往可以看到和执行所有空间的内容，根据PXN位，可以设置不可执行，但针对某些情况，不仅仅要求不能执行user space的代码，还不能read。例如： 虽然没有办法直接执行userspace的代码，但是可以load userspace中的结构体，进而导致任意代码执行（和rop，jop有何不同：rop和jop是跳到自己可见空间的其它代码部分，此处是内核虽然被限制不能直接执行用户代码，但是它可以加载用户的代码到内核空间，再执行）

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663585936257.png" alt="1663585936257" style="zoom:80%;" />

arm的页表属性：表示用户态和内核的权限RWX，则需要6个bits，为了节省空间，ARM使用4个bits。BIT[54] UXN：为1表示用户态没有执行权限；BIT[53] PXN：为1表示内核态没有执行权限；AP[2] = 1表示内核态是readonly；AP[2] = 0表示内核态是RW；AP[1] = 0表示用户态没有任何权限；AP[1] = 1表示用户态跟内核态权限一样，即看AP[2]。由上述可以知道，只要用户态有权限，内核态的权限就和用户态的权限一样。于是，又在**pstate**中使用一个bit来存PAN，PAN=1时，若是在EL1中，则不允许访问EL0的内存。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671608532322.png" alt="1671608532322" style="zoom:80%;" />



<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671607612303.png" alt="1671607612303" style="zoom:67%;" />



开启pan之后，若想要访问用户态的数据，需要**copy_from_user,copy_to_user**。arm中还定义了指令**LDTR和STTR**，在EL1执行数据的加载和写入时，使用EL0的权限，在其他的except level中，LDTR和STTR指令与普通的LDR和STR相同。

### ARM指令   

PE：指的是 Process Element， 就是逻辑核心(logic core)，一个逻辑核心上可以跑一个线程。这个概念引出是由于现在有很多双线程的处理器(double-thread core)，可以一个核心运行两个完全不同的任务/线程, 一个当两个用，所以不能单单当成一个核了，就说一个核有两个PE。1个PE可以跑1个线程(thread)。PE指的是硬件，线程是跑在PE上的软件。比如说因特尔的i7-4790K，就是4核8线程处理器，每个核有两个线程，也就是有8个PE。

RISC 架构特点：

![1663145909794](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663145909794.png)

ARM AArch64执行模式：

![1663145849138](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663145849138.png)

ARMv8执行模式：AArch64（对应A64指令集）、AArch32

#### 寄存器：

unbaked register: 31个通用的64位寄存器（X0-X30用于整数运算或者寻址，如果只访问低32位用W0-W30，即访问相应32位寄存器，V0-V31用于浮点运算），注意没有W31或X31寄存器，31寄存器一般是栈指针寄存器或零寄存器（视指令而定），如果是栈指针，则指SP，如果是零寄存器，则指WZR（32位）或XZR（64位）

banked register（指一个寄存器不同模式下会对应不同的物理地址）：SPx、ELRx、SPSRx、PC、CPSR，其中，SPSRx是32位寄存器

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1663599801125.png" alt="1663599801125" style="zoom:67%;" />

Arm架构中寄存器分为两大类：

##### 系统寄存器

System Register: provide system control or status reporting

+ 13.2 general system control registers: 149
+ 13.3 debug register:30
+ 13.4 performance monitors register:19
+ 13.5 activity monitors registers:15
+ 13.6 statistical profiling extension registers:13
+ 13.7 RAS registers:16
+ 13.8 Generic Timer register:30

##### 通用寄存器，SIMD和浮点寄存器，特殊寄存器

Registers for instruction processing and handling exceptions

+ general-purpose 寄存器：X0-X30/W0-W30 31个。X30也是LR寄存器，用于存储子程序的返回地址。没有X31/W31，一些指令会将31号寄存器编译为zero寄存器，或SP。D1.6
+ SIMD and floating-point寄存器：V0-V31（128-bit）(还可以以下列方式访问这些寄存器：D0-D31（64-bit）,S0-S31（32-bit）,H0-H31（16-bit）,B0-B31（8-bit））。D1.6
+ 特殊寄存器：ELR （exception link register）：保存exception返回地址；SPSR （saved processor state register）： 执行exception前保存当前的processor state, 执行exception完返回时，SPSR会被copied到PSTATE。C5.2

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1664174789363.png" alt="1664174789363" style="zoom:80%;" />





<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1664170029466.png" alt="1664170029466" style="zoom:80%;" />



##### 抽象的PSTATE寄存器

D1.7。分为以下域：

+ condition flags: N,Z,C,V
+ execution state controls: SS,IL,nRW(current execution state,0是AArch64),EL,SP
+ exception mask bits: D,A,I,F
+ access control bits: PAN,UAO,TCO,BTYPE
+ time control bits: DIT
+ speculation control bits: SSBS

可以用以下寄存器访问对应的域，其它的域是不可读写的：

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1664179899340.png" alt="1664179899340" style="zoom:80%;" />

用MSR(register)可以将通用寄存器的值存到系统寄存器(包括特殊寄存器)，但MSR(immediate)只能操纵PSTATE的各个域。

## Kernel Module

dmesg 显示信息

dmesg -c 显示信息后清除信息

### Ftrance hook

ftrace hook (functionTracer ) 是linux内核的一个跟踪框架。

**原理：**使用ftrace需要目标Linux操作系统在编译时使用-pg选项和-mfentry选项（将对mcount的调用放到函数的第一条指令处）。此时会在每个非内联内核函数的入口插桩一个对\_mcount函数(或\__fentry__函数，若gcc>=4.6且为x86架构)的调用。mcount函数本身只是一个简单的返回指令，并没有什么实际意义，动态ftrace框架（开启CONFIG_DYNAMIC_FTRACE选项）会在启动时将所有对mcount的调用位置都填充为nop指令（通过scripts/recordmcount.pl脚本实现，所有函数都有个call指令，会严重影响性能），这样一来就在这些内核函数的开头产生了足以容纳一个call指令的空白区。

静态ftrace：frompc为当前函数的起始地址，selfpc为上级函数的起始地址，MCOUNT_INSN_SIZE为call mcount指令长度。

 ![1669172579132](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1669172579132.png)

动态ftrace： 在编译的时候调用recordmcount.pl搜集所有_mcount()函数的调用点，并且所有的调用点地址保存到section _\_mcount_loc， 在初始化init_ftrace时，遍历section __mcount_loc的调用点地址，默认给所有“bl _mcount”替换成“nop” 

![1669172813084](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1669172813084.png)

在需要hook时：

+ 被hook函数func()的入口出的nop被替换成了call frace\_caller()/frace\_regs\_caller()，称为一级hook点
+ frace\_caller()/frace\_regs\_caller()函数内的nop被替换为了 call ftrace_ops_no_ops()/ftrace_ops_list_ops() ，称为二级hook点。

+  在ftrace_ops_no_ops()/ftrace_ops_list_ops()函数中会逐个调用执行frace_ops_list链表中函数。 我们自定义的函数需要注册到这个链表中，称为三级链表调用点。

 <img src="https://img-blog.csdnimg.cn/20200718112001145.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3B3bDk5OQ==,size_16,color_FFFFFF,t_70" alt="img" style="zoom:67%;" /> 

ftrace对外接口函数：

+ ftrace\_set_filter_ip()：对需要hook函数的完成上述一、二级步骤，也就是完成了对自定义函数的调用执行
+ register\_ftrace\_function()：将我们自定义的函数加入到frace\_ops\_list中

**实例：**假如我们需要hook掉内核的cmdline\_proc\_show()函数

**1、定义结构体 frace_hook**

```c
struct ftrace_hook {
        const char *name;//被hook函数的函数名 execve()
        void *function;//指向我们新定义的实现监控和修改目的函数 myExecve()
        void *original;//指向该被hook函数地址的指针

        unsigned long address;//被hook函数的地址
        struct ftrace_ops ops;
}
```

**2、frace\_hook结构体的初始化**，需要将结构体的前三项设上：

```C
//新建一个结构体时original值为0（指向void类型的指针大小为8字节，即original占8字节，值为0，此时*original相当于取内存0地址的值，会报segment fault），初始化时，将其赋值为一个数，相当于将该地址由0改为某数，*original就是取内存该数对应地址的值。将其赋值为&a，即将其地址由0改为a的地址，*original的时候得到的是a。
#define HOOK(_name, _function, _original) \
        { \
            .name = (_name), \
            .function = (_function), \
            .original = (_original), \
        }

static struct ftrace_hook hooked_functions[] = {
        HOOK("cmdline_proc_show", fh_cmdline_proc_show, &real_cmdline_proc_show),
};

```

**3、install frace hook**，fh\_install\_hook(struct ftrace\_hook *hook)：

+ 查找被hook函数地址，并备份。resolve\_hook\_address(hook)
  + hook->address = kallsyms_lookup_name(hook->name)
  + *(hook->original) = hook->address，即将 real_cmdline_proc_show 的内容改为被hook函数的地址，后续直接调用 real_cmdline_proc_show 即可调用原函数

+ 初始化ops结构
  + hook->ops.func = fh_ftrace_thunk
  + hook->ops.flag =   FTRACE_OPS_FL_SAVE_REGS | FTRACE_OPS_FL_IPMODIFY; 
+ 使能被hook函数的第一、二级hook点
  + ftrace\_set\_filter_ip(&hook->ops,hook->address,0,0)，倒数第二个0表示使能hook点，若为1表示关闭hook点。
+ 把ops注册到第三级hook点的ftrace\_ops\_list链表中
  + register\_ftrace\_function(&hook->ops)

**4、三级hook调用函数fh\_frace\_thunk()**

使用该函数作为跳板，跳到真正想要执行的函数fh\_cmdline\_proc\_show()。 上述的技巧需要使能`CONFIG_DYNAMIC_FTRACE_WITH_REGS`。

为什么不能直接在三级hook处执行我们的函数：在三级hook处，我们只知道原函数运行的时机，但是拿不到原函数运行的数据。解决：定义一个和原函数参数一致的函数，插入到原函数的原有调用点，即hook层层返回后，用该函数代替原函数执行（函数的参数都在栈上）：

fh_frace_thunk()函数：notrace属性指no instrument function属性，不允许对该函数插入mcount， 可用于标记Linux内核跟踪中禁止使用ftrace的函数 。

```C
static void notrace fh_ftrace_thunk (unsigned long ip, unsigned long parent_ip, struct ftrace_ops *ops, struct pt_regs *regs)
{
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

    /* Skip the function calls from the current module. */
	/* (1) 防止递归 */
    /* (2) 通过修改`ftrace_caller()/ftrace_regs_caller()函数的返回函数来实现hook,原本执行完ftrace hook后返回原函数cmdline_proc_show(),将其替换成新函数fh_cmdline_proc_show()*/
    if (!within_module(parent_ip, THIS_MODULE))
            regs->ip = (unsigned long) hook->function;//regs->ip存的第一级hook函数的返回地址
}
```

fh\_cmdline\_proc_show()：该函数中可以`pre hook`、`调用原函数`、`post hook`。 

```C
/* 定义和原函数参数一致的fh_cmdline_proc_show()函数 */
static int fh_cmdline_proc_show(struct seq_file *m, void *v)
{
    int ret;  
	/* (1) pre hook 点 */
	seq_printf(m, "%s\n", "this has been ftrace hooked");
	/* (2) 调用原函数 */
    ret = real_cmdline_proc_show(m, v);
	/* (3) post hook点 */
    pr_debug("cmdline_proc_show() returns: %ld\n", ret);
	return ret;
}
```

**4.1、config_dynamic_ftrace_with_regs特性** 

若有该特性，则在ftrace_regs_caller()中会把寄存器情况保存到pt_regs中，并逐级传递给fh_ftrace_thunk，用于修改regs->ip，即ftrace_regs_caller的返回地址来插入hook。

```c
ftrace_regs_caller() {
	save pt_regs	
	ftrace_ops_list_func(regs)
	restore pt_regs
}
```

```C
static void ftrace_ops_list_func(unsigned long ip, unsigned long parent_ip,
				 struct ftrace_ops *op, struct pt_regs *regs)
{
	__ftrace_ops_list_func(ip, parent_ip, NULL, regs);//若不支持该特性，则后面两个参数都是NULL
}
```



**5、流程图**，防递归指，在fh_cmdline_proc_show中调用read_cmdline_proc_show时，里面不会再hook。通过判断调用三级hook函数的父亲ip来自kernel还是来自本kernel实现。

 ![img](https://img-blog.csdnimg.cn/20200718112026711.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3B3bDk5OQ==,size_16,color_FFFFFF,t_70) 



**-pg选项：**

内核编译时采用O2优化，O2优化使能-fipa-ra优化，即若函数a调用了函数b，未开优化时，A需要对caller-save register进行save和restore，B需要对callee-saved register进行save和restore。当开启优化时，a不会保存那些b没有写的寄存器。这样子存在问题：例如我们在函数a执行时修改了函数b的代码，让其代码第一条是一个jump指令，跳转到函数bnew。a跳到b之后，b马上跳到bnew，但是因为编译时，a所看到的她调用的函数b是没有跳转的，且没有使用某寄存器，那该寄存器就不会被保存。bnew中可能修改了该寄存器，返回A之后就会出现结果错误。开启-pg选项之后，会禁用-fipa-ra优化，且如果某函数A调用的函数B在另外一个文件中，那么在C调用A时会保存所有寄存器，因为编译器不知道被调用函数B的情况（调用mcount()函数满足了该情况，每个函数中都插入了mcount函数调用）。

a修改b第一条指令的代码如下：mprotect用于修改一段指定内存区域的保护属性。`` int mprotect(const void *start, size_t len, int prot); ``，start必须按页对齐，len大小是页大小的整数倍（15代表2^15次方）。b代码段第一个字节是0xe9，代表jump指令的opcode。后8字节存偏移地址： 目标地址-当前地址-5 = 偏移地址 。

```c
int main(void)
{
        int x;
        scanf("%d", &x);

        if (mprotect((void*)(((unsigned long)&b) & (~0xFFFF)), 15, 
                             PROT_WRITE | PROT_EXEC | PROT_READ)) {
                perror("mprotect");
                return 1;
        }

        /* 利用 jump 指令将函数 b 替换为 newb 函数 */
        ((char*)b)[0] = 0xe9;
        *(long*)((unsigned long)b + 1) = (unsigned long)&newb
                                         - (unsigned long)&b - 5;
        
        printf("%d", a(x));
        return 0;
}
```





### 编程语言与 linux 内核：

#pragma GCC diagnostic ignored 告诉编译器消除warning（将ignored 改为warning即为开启警告，改为error即为将警告升级为error），“-Wint-conversion”将整数未经强制类型转换就赋值给指针的waring。

struct mm_struct：每个进程/线程队都是task_struct（process descriptor进程描述符）的一个实例，记录该进程所有的context。task_struct中有一个mm_struct（内存描述符memory descriptor），该结构抽象地描述了进程地址空间的信息。

 <img src="https://images0.cnblogs.com/blog/516769/201304/13214813-ece9b1c2abbd4ea8b1de1dd266849b73.png" alt="img" style="zoom:67%;" /> 

struct file 每个打开的文件在内核空间都对应一个struct file，每个进程的PCB中存储了指向文件描述符表的指针，通过文件描述符fd索引该表，得到指向对应文件struct file的指针。

struct file\* get_mm_exe_file(struct mm_struct *mm) 索引到mm对应的可执行文件。

kallsymbols_lookup_name()：用于获取某未exported函数的指针位置，linux内核在5.7.0版本后，该kallsysmbols_lookup_name()函数也不export了（即不可以直接调用，不可以通过函数名索引到该函数），可以通过kprobe获得该函数的起始地址，然后调用它。

+ 调用register_kprobe获得 kallsyms_lookup_name函数的地址kallsyms_lookup_name_func (该变量是一个函数指针，初始化为 unsigned long (*kallsyms_lookup_name_fun)(const char *name) = NULL; ）
+ 调用kallsyms_lookup_name_func获取其它未export的函数（即我们想要知道地址的函数）的地址（直接kallsyms_xx_func("函数名")，函数指针与其它指针不一样，不需要加*）

current：define current get_current()，get_current()返回struct task_sturct*,指向当前进程task_struct的指针。

kmalloc(size_t size, int flags)：flags表示分配内存的类型。kmalloc/vmalloc用于在内核模块中动态开辟内存（利用kfree/vfree释放空间），malloc用于用户空间申请内存：

+  kmalloc申请的是较小的物理地址和虚拟地址都连续的空间。kmalloc和get_free_page最终调用实现是相同的，只不过在调用最终函数时所传的flag不同，且不对获得空间清零。 

+  kzalloc 先是用 kmalloc() 申请空间 , 然后用 memset() 清零来初始化 ,所有申请的元素都被初始化为 0。
+  vmalloc用于申请较大的内存空间，虚拟内存是连续，但是在物理上它们不要求连续。
+  malloc 用于用户空间申请内存，且不对获得空间清零 

file_path()：返回指向path字符串（即该可执行文件的源代码路径，在我的程序中是/home/lqj/Kernel-App/hook-example/tests/hello.spec，对于别的进程来说，可能是/usr/bin/cat等，每个进程都有自己的可执行文件，相应地在系统中有路径）的指针。

```c++
char *file_path(struct file *filp, char *buf, int buflen)
{
	return d_path(&filp->f_path, buf, buflen);
}
d_path: Convert a dentry into an ASCII path name. If the entry has been deleted the string " (deleted)" is appended
```

char \*strrchr(const char \*str, int c)搜索str所指向字符串中最后一次出现c的位置，未找到则返回空指针。

int strncmp(const char \*str1, const char \*str2, size_t n) 把 str1 和 str2 进行比较，最多比较前 n 个字节。 返回指小于/大于/等于0，则str1</>/=str2

### ELF

一个ELF文件主要由ELF头，程序头表（program header table），节头表（section header table）构成

elf提供两种视图：链接视图（以节section为单位）和执行视图（以段segment为单位）， 当多个可重定向文件最终要整合成一个可执行的文件的时候（链接过程linker），链接器把目标文件中相同的 section 整合成一个segment，在程序运行的时候，方便加载器loader的加载。 

 <img src="https://pic2.zhimg.com/v2-2c33f6155faccf24839b897ee9db10a1_r.jpg" alt="img" style="zoom: 80%;" /> 



<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1669464286503.png" alt="1669464286503" style="zoom:80%;" />

**ELF 头**：位于ELF的开始，通过``readelf -h``可以读ELF头部信息

```C
typedef struct elf64_hdr {
	 unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
	  Elf64_Half	e_type;			/* Object file type */
	  Elf64_Half	e_machine;		/* Architecture */
	  Elf64_Word	e_version;		/* Object file version */
	  Elf64_Addr	e_entry;		/* Entry point virtual address _start函数的地址 */
	  Elf64_Off	e_phoff;		/* Program header table file offset */
	  Elf64_Off	e_shoff;		/* Section header table file offset */
	  Elf64_Word	e_flags;		/* Processor-specific flags */
	  Elf64_Half	e_ehsize;		/* ELF header size in bytes */
	  Elf64_Half	e_phentsize;	/* Program header table entry size */
	  Elf64_Half	e_phnum;		/* Program header table entry count */
	  Elf64_Half	e_shentsize;	/* Section header table entry size */
	  Elf64_Half	e_shnum;		/* Section header table entry count */
	  Elf64_Half	e_shstrndx;		/* Section header string table index */
} Elf64_Ehdr;

```

**程序头表**：执行视图。列举了所有有效的段(segments)和他们的属性。 加载器根据程序头表将文件中的节加载到虚拟内存段中。每一项都是一个``elf64_phdr``结构，``readelf -l`` 命令可以查看一个链接后的elf可执行文件的段信息，以及每个segment对应哪些section的映射关系，即程序头相关信息。program header 中的 p_pvaddr 表示该segment对应的物理地址， 在带有 MMU 的平台上，这个字段不用考虑，输出等于虚拟地址，物理地址的概念只有在uboot或者裸机代码中存在。

**节头表**：链接视图。 一个ELF文件中到底有哪些具体的 sections，由包含在这个ELF文件中的 section head table(SHT)决定。表中每个``elf64_shdr``描述了这节的信息，比如每个节的节名、节的长度、在文件中的偏移、读写权限及节的其它属性。 ``readelf -S xx.o`` 可以读取节信息。

所有节区名都存在.shstrtab字符串表中，``elf64_shdr``中的``sh_name``存的是名字在字符串表中的偏移，段没有段名，只有p_type。

Linkable file格式：

 <img src="https://img-blog.csdnimg.cn/20210515101639285.png" alt="img" style="zoom:67%;" /> 

Executable file 格式：

 <img src="https://img-blog.csdnimg.cn/20210515184446614.png" alt="img" style="zoom:67%;" /> 

内存映像（memory snapshot）：IA32体系中代码段从0x08048000开始，IA64体系中从0x0000000000400000开始（在不开地址随机化的情况下）

 <img src="https://img-blog.csdnimg.cn/20210515215709945.png" alt="img" style="zoom:80%;" /> 

通常elf中有以下节：

+ .text 代码段，可以通过objdump -d反汇编，查看elf代码段的内容
+ .data 初始化了的数据，占用程序空间
+ .bss 未初始化的全局变量和静态局部变量，程序开始时，将这些数据初始化为0，不占用可执行文件空间，仅仅是占位符。
+ .init 进程初始化代码，程序在调用main函数前调用这些代码
+ .rodata 只读数据
+ .comment 版本控制信息
+ .eh_frame / .eh_frame_hdr 生成描述如何unwind堆栈的表
+ .debug 用于符号调试的信息
+ .dynsym 动态链接符号表，类似于elf头，存储了会链接啥动态库之类的信息。
+ .shstrtab sh字符串表，存放section名。用elf头的e_shstrndx*e_shentsize可以得到shstrtab header在section header中的位置，由此可以得到shstrtab的位置。
+ .strtab 字符串表，在ELF文件中，会用到很多字符串，比如节名，变量名等。ELF将所有的字符串集中放到一个表里，每一个字符串以’\0’分隔，然后使用字符串在表中的偏移来引用字符串。这样在ELF中引用字符串只需要给出一个数组下标即可 
+ .symtab 符号表， 在链接的过程中需要把多个不同的目标文件合并在一起，不同的目标文件相互之间会引用变量和函数。在链接过程中，我们将函数和变量统称为符号，函数名和变量名就是符号名，它们的符号值就是他们的地址。``readelf -s `` 可以看.symtab的内容
+ .got 全局偏移表
+ .plt 过程链接表
  + .relname 重定位信息，包含了链接器在处理各目标文件时进行重定位的信息，每个需要重定位的段都有自己的重定位表，例如.text节区的重定位区名字为.rel.text。``readelf -r``可以看可重定位信息。

objdump: -d 反汇编目标文件中需要执行指令的section

nm：-a列出所有符号，-g只显示外部符号，-u只显示没有定义的符号

![1676275393098](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1676275393098.png)



### 分段分页机制：

分段机制在老的intel的x86架构芯片上实现，后面intel为了向前兼容，采用了分段分页机制。后续新的处理器架构，比如ARM，只采用分页机制。linux操作系统最开始也是基于intel的芯片写的，保留了对于分段机制（segmentation）的支持。

 ![img](https://img-blog.csdn.net/20150612143923193) 

逻辑地址高位是段选择符（也可以用专门的段寄存器（CS:code segment，DS：data segment等）存段选择符，内容称为选择子。 代码段用cs寄存器来分段和引用；数据段用ds寄存器来分段和引用；栈段用ss寄存器来分段和引用。另外3个段寄存器es、fs和gs可以用来分段和引用额外的数据段。 在程序执行代码段里的代码、或访问数据段中的数据之前，需要事先将合法的16位段选择符的值加载到适当的段寄存器中，否则无法执行代码或访问数据。 ），通过**查段描述符表可以得到该段的虚拟地址基质，然后加上低32位的逻辑地址，得到线性地址，然后通过MMU，进行虚拟地址和物理地址的转换**。逻辑地址上同一段的内容，它们落在连续的虚拟地址区域。不同段之间不连续，即段号差别1的，在虚拟地址上不一定就相邻，取决于其段描述符中存的基址。通常说的虚拟地址既指逻辑地址，又指线性地址。

 **分段提供了隔绝各个代码、数据和堆栈区域的机制，它把处理器可寻址的线性地址空间划分成一些较小的称为段的受保护地址空间区域。**此时处理器就可以加强这些段之间的界限，并确保一个程序不会通过访问另一个程序的段而干扰程序的执行。 段描述符表的表项中定义了段的起始地址，界限，属性等内容。

页表和页目录大小均为4K，每级的索引值为10位，即页表中共有2^10项，每项4字节。三级页表中，第一级是页全局目录PGD，第二级是页中间目录PMD，第三级是页表项PTE。**PFN指page frame number，指物理内存区域编号**（若是4K大小页，则是32位物理地址的前20位值），**最后一级页表PTE中存了PFN**。

全局描述符表GDT：基址存在寄存器GDTR中，全局描述符表在系统中只能有一个，可以被每个进程共享，共享内存区的段基址就需要存在GDT中。

局部描述符表LDT：每个进程各有一个。LDT表放在GDT中，GDT的段选择符的bit3为1，LDT的段选择符的bit3为0。选择符的低三位都是0，因为一个描述符项占8字节，选择符得8字节对齐（与页表的情况不同，算偏移时是#GDTR+选择符值，页表是#CR3+页表索引值*页表项大小）。

中断描述符表IDT：系统中只能有一个，可以存放256个描述符，对应256个中断。

分段是intel为了向前兼容保留下来的机制，linux将段基址都设置为0，相当于直接用段内偏移访问内存空间，即逻辑地址的低32位就是线性地址，相当于不再分段。称为平坦模式。加上段间保护的称为保护模式（段选择符的bit0-2标识当前访问进程的特权级RPL，如果低于目标段的特权级(在段描述符的属性部分会定义DPL)，就会被拒绝访问），没有保护的称为实模式。

### linux 设备

linux设备分类：

+ 字符设备：能够像字节流一样被访问的设备，由字符设备驱动程序提供访问接口，至少要实现open，close，read，write接口。字符设备可以通过FS节点来访问，比如/dev/tty1等。例如：鼠标，键盘，串口，控制台，LED设备等
+ 块设备：也可以通过/dev目录下的FS节点来访问，使用驱动程序提供的接口操作块设备。块设上能够容纳filesystem，进行I/O操作时块设备每次只能传输一个或多个完整的块（512字节）。linux 可以让上层应用像字符设备一样读写块设别，允许一次传递任意多个字节的数据。故，块设备和字符设备的区别仅仅在于内核内部管理数据的方式。例如：磁盘/硬盘，U盘，SD卡等
+ 网络设备：网络接口用于与其它主机交换数据。在FS上不存在节点。

 <img src="https://img-blog.csdn.net/20160309214506200?watermark/2/text/aHR0cDovL2Jsb2cuY3Nkbi5uZXQv/font/5a6L5L2T/fontsize/400/fill/I0JBQkFCMA==/dissolve/70/gravity/Center" alt="img" style="zoom:80%;" /> 

### execve

 在linux中创建一个新进程，一般是 shell 进程先用fork()从父进程复制一个新的进程空间（复制了父进程的task_struct（tsk->stack不同，有自己的栈）， files，fs，mm，io等信息），fork的子进程返回后，调用execve() 加载新的 exe 文件，创建新的代码段、数据段、bss、heap、stack、mmap区域。子进程退出内核的时候，完成了所有的预备工作，新进程从main开始执行。

+ fork 出来的子进程复制了父进程的页表，共用物理内存，但设置了写保护。当需要修改的时候，采用copy on write机制，分配物理内存页，复制内容，修改内容，修改页表项映射。

+ Execve：
  + 根据文件名找到a.out，用它来取代当前进程的内容
  + 以ELF格式装载a.out，
  + 读取文件头，如果是静态链接程序，直接执行_start
  + 如果是动态链接程序
    + 读取.interp段，对应动态链接器的名称(ld-linx.so.2)
    + 将控制权交给ld-linux.so.2（在用户空间执行ld-linux.so.2，也是一个elf文件，也需要装载等操作，执行_start），检查并装载共享库，对外部引用进行重定位
      + 动态链接器在linux下是glibc的一部分，也就是属于系统库级别的，它的版本号往往跟系统的glibc库版本号是一样的。 当系统中的Glibc库更新或者安装其他版本的时候，/lib/ld-linux.so.2 这个软链接就会指向新的动态链接器，而可执行文件本身不需要修改 ".interp" 中的动态链接器路径来适应系统的升级  
    + 退出动态链接器，执行_start，进行初始化（流程为 _start -> _\_libc_start_main -> __libc_csu_init -> _init -> main -> _fini. ）



elf有四种格式ET_REL, ET_EXEC, ET_DYN, ET_CORE，一般为ET_EXEC，因为需要支持随机地址映射ASLR（ 通过随机放置进程关键数据区域的地址空间来防止攻击者能跳转到内存的特定位置来利用函数 ），可执行文件需要编译成位置无关码，格式为ET_DYN（等同于共享目标文件Shared Object File）。

ASLR开启之前进程的用户空间布局：mmap区域在新版本的linux中（2.6.7之后），是向下增长，之前向上。

 <img src="https://img-blog.csdnimg.cn/20201026144113472.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3B3bDk5OQ==,size_16,color_FFFFFF,t_70#pic_center" alt="在这里插入图片描述" style="zoom:80%;" /> 



ASLR开启之后，用户空间布局：text段，heap段的基地址有偏移，mmap段和stack的top有随机偏移

 <img src="https://img-blog.csdnimg.cn/20201026144152591.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3B3bDk5OQ==,size_16,color_FFFFFF,t_70#pic_center" alt="在这里插入图片描述" style="zoom: 80%;" /> 

load_elf时映射：

内核使用包括连续页的VMA（virtual memory area）来识别进程。在每个VMA中可能映射了一个或多个section，每个VMA代表一个ELF文件的segment。 

进程空间的映射，有些是和文件关联的。例如：code、data，以及mmap映射的动态库的code、data。还有些是存储临时数据的，是匿名映射。例如：bss、heap、stack，以及mmap映射动态库的bss。bss较小时，映射时和data放在同一个vma中。load_elf 时 elf一般分为两个 PT_LOAD segment来加载：

+ 第一个segment包含了.text,.init.rodata等段，加载成一个只读可执行的vma
+ 第二个segment包含了 .init_array .fini_array .dynamic .got .data .bss等，除去bss，加载成两个vma，data的vma可读写，剩下的vma只读，bss根据其大小要么和data在一起，要么是另外一个独立的vma。

下面是一个简单程序的readelf -l a.out的分段情况：可以知道第一个LOAD段除了.text, .init. .rodata外还有别的一些小段，.fini, .plt, .rela等。

![1669553724888](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1669553724888.png)

对于 LOAD 的 segment，因为是直接加载到内存中，所以其必须满足 (virtaddr-offset) % pagesize == 0，这里的 pagesize 也就是 align 的值：2M。

+ 段加载到内存中的位置为PAGESTART（VirtAddr），PAGESTART是指按页对其向下取整(内存映像的向上)，即因为需要按页对齐地加载，加载的起始位置不是vaddr，而是更加低的位置。
+ 加载的size是PAGEALIGN（p_filesz + PAGEOFFSET(vaddr)），PAGEALIGN指按页向上取整。
+ 该segment内容在虚存空间的起始位置仍然是vaddr。上面多出来PAGEOFFSET（VirtAddr）大小的内容和下面多出来的内容，对应elf的相邻的上下段。
+ ELF中读取内容的起始地址由p_offset，变为了p_offset - PAGEOFFSET(vaddr)。读取磁盘文件需要页对齐，即p_offset-PAGEOFFSET(vaddr)必须是页对齐的，即p_offset-p_vaddr必须是页对齐的。
+ elf 段加载进内存后，前后空余位置是否会清零：不清0。vma指向按页对齐的位置，mm中的start_code,  end_code,  start_data，end_data等指向实际位置。

type为PT_LOAD的段： 给出了一个可加载的段,段的大小由 p_filesz 和 p_memsz 描述。文件中的字节被映射到内存段开始处。如果 p_memsz 大于 p_filesz,“剩余”的字节要清零。p_filesz 不能大于 p_memsz。 

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1669286594847.png" alt="1669286594847" style="zoom:80%;" />

stack前面的内容也由execve构造好：

 <img src="https://img-blog.csdnimg.cn/20201027193443775.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3B3bDk5OQ==,size_16,color_FFFFFF,t_70#pic_center" alt="在这里插入图片描述" style="zoom: 67%;" /> 



内核中用struct linux_binprm来保存每个可执行文件的信息（可执行文件的路径，运行的参数，环境变量等，最开始的buf中存储了elf的头128字节）

```C
/*
* This structure is used to hold the arguments that are used when loading binaries.
*/
struct linux_binprm {
    char buf[BINPRM_BUF_SIZE];  // 保存可执行文件的头128字节
#ifdef CONFIG_MMU
    struct vm_area_struct *vma;
    unsigned long vma_pages;
#else
# define MAX_ARG_PAGES  32
    struct page *page[MAX_ARG_PAGES];
#endif
    struct mm_struct *mm;
    unsigned long p; /* current top of mem , 当前内存页最高地址*/
    unsigned int
            cred_prepared:1,/* true if creds already prepared (multiple
                             * preps happen for interpreters) */
            cap_effective:1;/* true if has elevated effective capabilities,
                             * false if not; except for init which inherits
                             * its parent's caps anyway */
#ifdef __alpha__
    unsigned int taso:1;
#endif
    unsigned int recursion_depth; /* only for search_binary_handler() */
    struct file * file;         /*  要执行的文件  */
    struct cred *cred;      /* new credentials */
    int unsafe;             /* how unsafe this exec is (mask of LSM_UNSAFE_*) */
    unsigned int per_clear; /* bits to clear in current->personality */
    int argc, envc;     /*  命令行参数和环境变量数目  */
    const char * filename;  /* Name of binary as seen by procps, 要执行的文件的名称  */
    const char * interp;    /* Name of the binary really executed. Most
                               of the time same as filename, but could be
                               different for binfmt_{misc,script} 要执行的文件的真实名称，通常和filename相同  */
    unsigned interp_flags;
    unsigned interp_data;
    unsigned long loader, exec;
};
```

每种可以被注册的可执行文件格式都用一个struct linux_binfmt存储该格式可执行文件的处理函数等，最常见的可执行文件格式就是elf。它对应的linux_binfmt为elf_format

```C
struct linux_binfmt {
    struct list_head lh;
    struct module *module;
    int (*load_binary)(struct linux_binprm *);//通过读存放在可执行文件中的信息为当前进程建立一个新的执行环境
    int (*load_shlib)(struct file *);//用于动态地把一个共享库捆绑到一个已经在运行的进程
    int (*core_dump)(struct coredump_params *cprm);
    unsigned long min_coredump;     /* minimal dump size */
 };

```

```C
static struct linux_binfmt elf_format = {
	.module		= THIS_MODULE,
	.load_binary	= load_elf_binary,
	.load_shlib	= load_elf_library,
	.core_dump	= elf_core_dump,
	.min_coredump	= ELF_EXEC_PAGESIZE,
};
```

调用流程为：do_execve -> do_execveat_common() → bprm_execve() → exec_binprm->search_binary_handler()(每个fmt都调用其load_binary函数尝试load一遍该可执行文件，返回错误代码：会比较文件头前四个字节，则试下一个) → fmt->load_elf_binary(struct linux_binprm *bprm) （先填充好binprm，再找到对应的format/handler，用其中的函数处理二进制文件）

load_elf_binary()负责解析elf文件格式，加载PT_LOAD segment到内存，并进行地址空间映射。


+ 读入elf的 program header table

+ 寻找解释器/动态链接器 segment，类型为PT_INTERP，段内容为字符串‘/lib/ld-linux.so.2’，通过open_exec()得到指向解释器可执行程序的file指针，读入解释器的程序头

+ flush_old_exec() / begin_new_exec() 清空从父进程继承下来的用户空间(包括信号处理表，文件，用户空间等,mm_struct下所有vma都释放)

+ setup_arg_pages() 设置好进程的栈空间位置，在初始化mm结构体__bprm_mm_init时，会设置进程栈虚拟地址对应vma的start和end，此时是一页大小，对应TASK_SIZE_MAX-PAGE SIZE 和TASK_SIZE_MAX。这一页用于存放二进制文件名，参数和环境变量等。在setup_arg_pages中，我们把前面这个页的栈空间移动到stack\_top（随机得到）位置，并扩展栈虚拟地址空间的大小（扩展128K），结果如下：

  <img src="https://pic2.zhimg.com/v2-2ec585eddf6051841d45997f6e3e1209_r.jpg" alt="img" style="zoom:67%;" /> 

+ 映射目标程序的PT_LOAD段，映射的地址是段指明的p_vaddr（如果是ET_DYN类型elf，每段load位置会加上一个相同的随机偏移 load_bias），通过elf_map建立用户空间与目标文件中某个连续区域的映射，返回值是实际映射的起始地址。设置好start/end_code, start/end_data,brk等，并对bss区清0。 load_addr = elf_ppnt->p_vaddr - elf_ppnt->p_offset （+load_bias）。vaddr-offset等于PAGESTART(vaddr)。

  + elf_map(bprm->file, load_bias + vaddr, elf_ppnt, elf_prot, elf_flags, total_size);
    + 对于ET_DYN类型的ELF，若是第一个map的段，先调用 map_addr = vm_mmap(filep, addr, total_size, prot, type, off);将所有load段映射过去，再调用 vm_munmap(map_addr+size, total_size-size);解映射后面的load段。这样的目的是，为了防止随机化映射地址导致后面的段没处映射。若不是第一次map，则直接映射该段即可。这个机制只适用于LOAD段连续的情况，否则LOAD间会有空洞（total_size为第一个load段的起始地址到最后一个load段的结束地址）。ET_DYN类型的ELF，load段连续，ET_EXEC不连续。？？？？

+ mmap上bss和break/heap段

+ 若存在动态链接器段，调用load_elf_interp映射到用户空间中，并将程序入口地址设置未interpret程序的入口地址，否则设置成该目标映像的入口地址（elf头的e_entry）

+ create_elf_table，将argc、envc，辅助向量（Auxiliary Vector）等复制到用户空间，使得在进入解释器或者目标映像的程序入口时出现在用户空间堆栈上。

pt_regs 用于描述用户态的cpu寄存器在内核栈中的保存情况，可以获取用户空间的信息。shell调用系统调用sys_execve时会将pt_regs压到栈上，当作系统调用的参数。

asmlinkage 是一个宏  define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0))) ，函数有这个关键字的，表示告诉编译器，该函数不需要通过寄存器传递参数，参数通过栈传递。asmlinkage多用于系统调用函数，寄存器值从用户空间传过来之后，SAVE_ALL压入堆栈，所以系统调用需要从堆栈上获取参数。



### mmap

 一个elf文件包含了text段，data段，bss段。Linux操作系统在加载elf文件运行进程后，还会生成stack段，heap段，每个segment用一个vm_area_struct结构体管理。 与前文的x86的硬件的segmentation机制不同，此处的segment管理是软件实现的，且默认不再使用segmentation（即采用平坦模式）。

**普通文件读写：**内存中分配一段空间（page cache）专门用于缓存该文件的内容，读写文件只涉及该区域。

+ 进程发起读文件请求。
+ 内核通过查找进程文件符表，定位到内核已打开文件集上的文件信息，从而找到此文件的inode。
+ inode依据address_space结构体查找要请求的文件页是否已经缓存在页缓存中。如果存在，则直接返回这个文件页的内容。
+ 如果不存在，则通过inode定位到文件磁盘地址，将数据从磁盘复制到页缓存。之后再次发起读页面过程，进而将页缓存中的数据发给用户进程。

 常规文件操作为了提高读写效率和保护磁盘，使用了页缓存机制。这样造成读文件时需要先将文件页从磁盘拷贝到页缓存中，由于**页缓存仅映射到内核空间，不能被用户进程直接寻址，**所以内核还需要将页缓存中数据页再次拷贝到用户空间的缓冲区buffer中。这样，通过了两次数据拷贝过程，才能完成进程对文件内容的获取任务。写操作也是一样，**待写入的buffer在内核空间不能直接访问，**必须要先拷贝至内核空间的page cache（如果不存在该文件页对应的页缓存，还会先从磁盘中读到页缓存中），操作系统会按某策略写回磁盘（延迟写回），也是需要两次数据拷贝。 x86通过SMAP控制内核是否可以访问用户页，ARM通过PAN机制。内核读写page cache前，会建立内核虚地址和page cache的映射，读写（拷贝）完成之后，会删去映射。

 <img src="https://pic3.zhimg.com/v2-532b29075e761c3c0ed63c80f9976b12_r.jpg" alt="img" style="zoom:67%;" /> 

**mmap读写文件：优化掉了 page cache 和用户空间 buffer 之间数据复制的过程。**

+ 缺页异常程序先根据要访问的偏移和大小从page cache中查询是否有该文件的缓存，如果找到就更新进程页表指向page cache那段物理内存（ page cache这段物理内存被同时映射到了内核空间和用户空间）。没找到就将文件从磁盘加载到内核page cache，然后再令进程的mmap虚拟地址的页表指向这段page cache中文件部分的物理内存

 <img src="https://pic1.zhimg.com/v2-83bb5c442a72be91e4abaf6f1189de00_r.jpg" alt="img" style="zoom: 67%;" /> 

操作接口：addr为指定的映射地址，一般设置为NULL，让os自己指定，offset为从被映射文件开头的几个字节开始映射。

```C
#include <sys/mman.h>

void* mmap(void *addr, size_t len,int prot, int flags, int fd, off_t offset);
返回：若成功则为被映射区的起始地址，若出错则为MAP_FAILED

int munmap(void *addr, size_t len);
```

+ prot有**PROT_EXEC, PROT_READ, PROT_WRITE, PROT_NONE**（映射区域不可访问）等，不能够和文件的打开方式相冲突。PROT_NONE 用于实现防范攻击的guard page，攻击者访问到时会触发SIGSEGV（segmentation fault）。
+ flags有**MAP_SHARED**（往映射区域的修改会同步到文件内，其它共享进程可见）, **MAP_PRIVATE**（需要写时，会将数据源拷贝副本，不会同步到文件内，其它共享进程不可见）, **MAP_FIXED**（指定映射到addr地址，且addr所指定的地址无法成功建立映射时，不尝试修改addr，返回错误）等
+ 如果open文件时设置的是O_RDONLY，则MAP_PRIVATE的映射是可以写的，因为不会同步到文件，但是MAP_SHARED的映射不可写相应区域（得设置成O_RDWR）。
+ MAP_SHARED 和 MAP_PRIVATE必须设置其中一个，且不能是两个都设置。如果是匿名映射，必须将flags设置为 (MAP_SHARED/MAP_PRIVATE)|MAP_ANNO。
+ 最终prot和flags会整合成vm_flags，传给底层函数，设置vma结构体中的vm_flags

使用：

```c
fd = open("/temp",  O_RDONLY);

mm = mmap( NULL, 1024, PROT_READ, MAP_SHARED, fd, 0);

printf("mm addr : %p\n", mm);

printf("read : %d\n", *(int *)mm);
printf("str : %s\n", mm+sizeof(int));
//mmap设置上写权限后，还可以直接memcpy等读内存，不mmap，则需要fread，fwrite等系统调用读磁盘，还需要进行用户空间和内核空间的内存拷贝。
close(fd);
munmap(mm, 1024);

```

### VMA

**vma结构内容：**

```c
struct vm_area_struct {
	unsigned long vm_start;	
	unsigned long vm_end;

	struct vm_area_struct *vm_next, *vm_prev;
	struct rb_node vm_rb;	
    ...
        
	struct mm_struct *vm_mm;	
	
	pgprot_t vm_page_prot;//arch-dependent，对应PTE的保护位
	unsigned long vm_flags;	//arch-independent
    ...

    const struct vm_operations_struct *vm_ops;

	unsigned long vm_pgoff;	
	struct file * vm_file;
    ...
} 

```

**32位x86的页表项**

 ![img](https://pic3.zhimg.com/v2-b297da76f281fbae787804c4b2d58556_r.jpg) 

+  P 标志位表示该页面是否已经映射到物理内存，如果该位为0，访问该页将触发缺页中断 
+  R/W 标志位表示该页的读写属性，0为只读； 
+  U/S 表示该页的访问权限，0表示只能被内核访问。 
+  D - Dirty，表示脏页面，如果一个页面被写过，它的 D 位被置为 1； 
+  A - Access，如果该页面被访问过（读或写），它的 A 位被置为 1。 

因为经典x86体系机上PTE没有执行权限的标记（允许栈上代码被执行，容易导致缓冲区溢出攻击），所以vma的权限和pte的权限不能完全对应。**64位x86种有可执行位**

**vm_ops结构内容：**

```C
struct vm_operations_struct {
	void (*open)(struct vm_area_struct * area);
	void (*close)(struct vm_area_struct * area);

	int (*mremap)(struct vm_area_struct *area);

	int (*mprotect)(struct vm_area_struct *vma, unsigned long start,
			unsigned long end, unsigned long newflags);
	
    vm_fault_t (*fault)(struct vm_fault *vmf);
	vm_fault_t (*map_pages)(struct vm_fault *vmf,
			pgoff_t start_pgoff, pgoff_t end_pgoff);
	...

	vm_fault_t (*page_mkwrite)(struct vm_fault *vmf);
	...
};
```

vm_area_struct是在mmap的时候创建的，vm_area_strcut代表了一段连续的虚拟地址/连续的页表项，这些虚拟地址相应地映射到一个文件或者一个匿名文件的若干页（仅虚拟页连续，但是物理页不连续）。每个vma记录着映射的起始地址和结束地址，访问权限，映射的文件（如果有的话，没有映射文件的VMA是匿名映射）。每一个memory segment对应一个vma。vma的访问权限和这些页在页表项PTE上的访问权限兼容（MMU由硬件实现，所以页表项的实现需要和对应架构规定页表项布局相同，页表权限可能没有那么多种，软件实现其它权限的检查）。

 <img src="https://pic1.zhimg.com/v2-12e764f96c6083fed333ed3ca73382ac_r.jpg" alt="img" style="zoom:67%;" /> 



mmap相当于设置好一段空间的vma，如果是文件映射则还会和文件的inode项关联（mmap的flags设置MAP POPULATE，则会为文件映射通过预读的方式准备好页表，随后对映射区的访问不会报错，文件映射还会设置vma->vm_ops即处理读写文件等handler函数，在page fault时调用）。

+ 文件映射：将文件区域映射到进程空间，文件存放在存储设备（磁盘）上
+ 匿名映射：没有普通文件对应的区域映射，内容存放在物理内存上。磁盘上存储的都是文件，实际上对应的是/dev/zero这个文件，匿名映射分配时会清0。

vm_ops的确定：

+ mmap 最终调到 mmap_region 时会调用 vm_area_alloc时，里面调用mm.h/vma_init，将ops初始化为空

  ```C
  static inline void vma_init(struct vm_area_struct *vma, struct mm_struct *mm)
  {
  	static const struct vm_operations_struct dummy_vm_ops = {};
  
  	memset(vma, 0, sizeof(*vma));
  	vma->vm_mm = mm;
  	vma->vm_ops = &dummy_vm_ops;
  	INIT_LIST_HEAD(&vma->anon_vma_chain);
  }
  ```

+ mmap_region中根据映射类型分为三类

  + if (file) :  call_mmap:  file->f_op->mmap(file, vma)

    + file 结构体中有 struct file_operations *f_op。file结构在文件open的时候确定下来：open 时找到其路径对应的 inode，然后根据inode->i_fop 填充 file->f_op，调用file->f_op->open 填充FILE\*结构体。（file 结构体对应打开的文件，存在进程的打开文件列表中，open返回的fd 就是这个打开的文件在列表的索引。inode对应某个特定的文件，一个文件只有一个inode，但可以有很多file结构体）

    + 驱动程序调用 cdev_init 将某个file_operations（驱动程序/kernel module中自己定义的）与和该cdev绑定，然后通过cdev_add 加到内核的 cdev_map 中。kernel module 可以通过封装好的 regitser_chrdev 函数注册设备，它整合了cdev_alloc, cdev_init, cdev_add等功能。

    + 目前常用的是ext4文件系统，file->f_op是 ext4_file_operations，其中对应的 mmap 是ext4_file_mmap()，将 vm_ops 设置为ext4_file_vm_ops：

      + ```C
        static const struct vm_operations_struct ext4_file_vm_ops = {
        	.fault		= filemap_fault,
        	.map_pages	= filemap_map_pages,
        	.page_mkwrite   = ext4_page_mkwrite,
        }
        ```

  + else if (vma_flags & VM_SHARED):  shmem_zero_setup(vma)，将其设置为shmem_vm_ops，匿名共享映射，也是此分支。

    + ```
      vma->vm_ops = &shmem_vm_ops;
      
      ifdef CONFIG_SHMEM，则是下面内容，否则是generic_file_vm_ops
      static const struct vm_operations_struct shmem_vm_ops = {
      	.fault		= shmem_fault,
      	.map_pages	= filemap_map_pages,
      #ifdef CONFIG_NUMA
      	.set_policy     = shmem_set_policy,
      	.get_policy     = shmem_get_policy,
      #endif
      };
      
      /mm/filemap.c
      const struct vm_operations_struct generic_file_vm_ops = {
      	.fault		= filemap_fault,
      	.map_pages	= filemap_map_pages,
      	.page_mkwrite	= filemap_page_mkwrite,
      };
      /mm/nommu.c 中vm_operations_struct为空 
      ```

  + else：vma_set_anonymous：vm_ops = NULL

### 共享内存：

**system V版本的共享内存：**

两个进程的各自有某段虚拟地址空间的页表项相同，映射到相同的物理地址上。系统调用shmget（），需要传入key值（可以利用ftok()函数生成），两个进程给相同的key，通过shmat建立虚实映射。shmget，shmat，shmdt，shmctl。文件映射，会隐式创建文件。do_shmat中会调用alloc_file和do_mmap进行文件页映射。

**POSIX 版本的共享内存：**

shm_open在/dev/shm下打开一个文件，然后调用mmap映射到自己的内存空间，shm_unlink， munmap，mysnc

**mmap建立的共享内存和system V版本的相同，两进程共享同样的物理空间（page cache）**

+ 在page fault 调用__do_fault/vm_ops.fault（shmem_fault）从文件中读取页时，会先看page cache中是否存在这页，如果存在，将地址填充到pte中，不存在，则读入后填充。
  + page cache的寻址通过每个文件唯一的address_space结构和该页在文件内的偏移找到。
+ 即两个进程通过mmap实现的共享内存，pte上对应的还是物理内存中的同一个page cache页，若某个进程修改这个page cache页，别的进程可见。可以通过msync及时将修改写回文件。
+ 匿名页的通信只能建立在有亲缘关系的进程间，此时父子进程的虚拟地址空间相同。

### malloc/remalloc

**malloc原理：**malloc是封装了相关系统调用的glibc库函数，而不是系统调用。若在堆缓冲区（chunk仓库）没有找到合适大小的内存（堆管理是由glibc进行的，free时不是直接释放给操作系统，而是还给glibc，glib有相应策略决定什么时候归还给系统，即可以不用进入内核态），底层会调用 brk 扩展和收缩堆大小：修改堆顶位置（分配小型内存，小于128K，glibc中设置的）或者mmap（分配大型内存）系统调用。延迟分配物理页，即malloc返回之后没有建立虚拟地址和物理地址的映射，会进page fault。

+ 注意：
  + brk/mmap分配的都是匿名页，匿名页在第一次访问的时候进入do_anonymous_page都分配的是清0的空间。但是glibc在管理堆缓冲区时，会把上一次free的页，给下一次malloc，这个过程中不会清0，所以导致了malloc的内存不清0的现象。同理，对于栈内区也是匿名页，只有在第一次访问的时候是清0的，因为栈的增长和减少，后续使用的栈可能之前已经用过这块内存了。

 <img src="https://img2018.cnblogs.com/blog/1771657/201912/1771657-20191213225043607-597522680.png" alt="img" style="zoom:80%;" /> 

**remalloc 原理：基于mremap**

### PAGE FAULT

思考：__do_page_fault 的时候权限检查是基于vma的权限检查(当前的访存失败的所需要的权限是否符合vma的vm_flags)，如果通过了这个检查：第一次访问，直接根据vma的flag情况，设置pte；非第一次访问的权限不对：如ROW（copy on write写时复制），有专门的处理流程。没有其它的情况，pte上的权限需要和vma上的权限对应，若MMU失败是因为权限检查失败，则在page fault的vma权限检查时也会失败。

基于ARM64处理器架构的page fault流程：

+ arch/arm64/kernel/entry.S:

 ![img](https://img-blog.csdnimg.cn/img_convert/b8b3845772b222ccfb0e536c3fc914d0.png) 

+ **do_mm_abort** 函数在 arch/arm64/mm/fault.c 中： 它根据传进来的 esr 获取 fault_info 信息，从而去调用处理函数。内核中定义了全局结构 fault_info 存放所有的错误处理方法：MMU查找页表时，如果没有成功，则进入do_page_fault 或 do_translation_fault(0/1/2/3级页表转换错误时进入，实际也会进入do_page_fault) 等函数处理：

   ```c
  static const struct fault_info fault_info[] = {
  	{ do_bad,		SIGBUS,  0,		"ttbr address size fault"	},
  	{ do_bad,		SIGBUS,  0,		"level 1 address size fault"	},
  	{ do_bad,		SIGBUS,  0,		"level 2 address size fault"	},
  	{ do_bad,		SIGBUS,  0,		"level 3 address size fault"	},
  	{ do_translation_fault,	SIGSEGV, SEGV_MAPERR,	"level 0 translation fault"	},
  	{ do_translation_fault,	SIGSEGV, SEGV_MAPERR,	"level 1 translation fault"	},
  	{ do_translation_fault,	SIGSEGV, SEGV_MAPERR,	"level 2 translation fault"	},
  	{ do_translation_fault,	SIGSEGV, SEGV_MAPERR,	"level 3 translation fault"	},
  	{ do_bad,		SIGBUS,  0,		"unknown 8"			},
  	{ do_page_fault,	SIGSEGV, SEGV_ACCERR,	"level 1 access flag fault"	},
  	{ do_page_fault,	SIGSEGV, SEGV_ACCERR,	"level 2 access flag fault"	},
  	{ do_page_fault,	SIGSEGV, SEGV_ACCERR,	"level 3 access flag fault"	},
  	{ do_bad,		SIGBUS,  0,		"unknown 12"			},
  	{ do_page_fault,	SIGSEGV, SEGV_ACCERR,	"level 1 permission fault"	},
  	{ do_page_fault,	SIGSEGV, SEGV_ACCERR,	"level 2 permission fault"	},
  	{ do_page_fault,	SIGSEGV, SEGV_ACCERR,	"level 3 permission fault"	},
       ...
  };
  
  ```



​		 <img src="https://img-blog.csdnimg.cn/img_convert/7bdd6e446b7cc11425b007b9aa51b855.png" alt="img" style="zoom:80%;" /> 

+ **do_page_fault**:  内核/用户模式下访问某个用户虚拟地址错误的处理函数。原子上下文指当前处于中断上下文中。写时复制的错误会通过此处的权限检查，意味着fork/私有文件映射的只读限制只在pte上体现。![img](https://img-blog.csdnimg.cn/img_convert/8b72f9b961988da6ce23f95d014efcf8.png) 

  do_page_fault: vm_flags和mm_flags设置

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1671525099223.png" alt="1671525099223" style="zoom: 67%;" />

+ **__do_page_fault**：(即下文主要关注用户访问某用户空间的page fault）：这里对传下来的vm_flags进行检查(检查用的是&，即有一项满足即可，针对do_page_fault的第三种情况，发生的是read错误，即vma->vm_flags满足上述一种(比如WRITE，write implies read)，就允许了read)，只给handle_mm_fault传mm_flags

```C

//找对应vma，找不到报错返回
struct vm_area_struct *vma = find_vma(mm, addr);
if (unlikely(!vma))
    return VM_FAULT_BADMAP;

//如果是栈区域，扩展栈大小
if (unlikely(vma->vm_start > addr)) {
    if (!(vma->vm_flags & VM_GROWSDOWN))
        return VM_FAULT_BADMAP;
    if (expand_stack(vma, addr))
        return VM_FAULT_BADMAP;
}

//检查vma是否允许该fault原因的操作，否则报错
if (!(vma->vm_flags & vm_flags))
    return VM_FAULT_BADACCESS;
//handle_mm_fault
return handle_mm_fault(vma, addr, mm_flags, regs);
```

+ **handle_mm_fault/__handle_mm_fault**：从此之后，函数与架构无关，page walk，没有则建立好页目录表项，
+ **handle_pte_fault**： mm_flags充当了vmf的flags。注意： vma_is_anonymous 逻辑为 !vma->vm_ops，根据前文所说vm_ops的生成，文件映射和匿名共享映射都是有vm_ops的，只有匿名私有映射才没有ops。

![img](https://img-blog.csdnimg.cn/img_convert/6cd819e0e9cfbec6877e8431b5dde1e3.png) 

```C
handle_pte_fault:
……
vmf->pte = pte_offset_map(vmf->pmd, vmf->address);
vmf->orig_pte = *vmf->pte;
……
//PTE不存在:NULL    
if (!vmf->pte) {
    if (vma_is_anonymous(vmf->vma))//匿名映射
        return do_anonymous_page(vmf);
    else//文件映射
        return do_fault(vmf);
}

//PTE 存在，但是 present = 0,页面被swap out了, PTE中存的不是物理页面的编号PPN，而是外部swap area中slot的编号swp_entry_t，从磁盘swap区找回该页，
if (!pte_present(vmf->orig_pte))
    return do_swap_page(vmf);

//NUMA(Non-uniform Memory Access,非统一内存访问)
if (pte_protnone(vmf->orig_pte) && vma_is_accessible(vmf->vma))
    return do_numa_page(vmf);

//COW，fork时父子进程的页表项都设置为了只读（vma的权限仍然是读写），写操作时进行写时复制
entry = vmf->orig_pte;
if (vmf->flags & FAULT_FLAG_WRITE) {
		if (!pte_write(entry))
			return do_wp_page(vmf);
	//可能因为pte上同时有write位和rdonly位，则清除rdonly位，置上dirty即可。
		entry = pte_mkdirty(entry);
	}
}
//标记pte项刚刚被访问过，以免页面被换出
entry = pte_mkyoung(entry);
//如果vmf->pte和entry内容不同（比如上面COW的else情况），该函数将会按需设置上access，dirty flags和write permisson，the PTE is never changed from no-exec to exec here.返回的是whether or not the PTE actually changed,
if (ptep_set_access_flags(vmf->vma, vmf->address, vmf->pte, entry,
                          vmf->flags & FAULT_FLAG_WRITE)) {
    update_mmu_cache(vmf->vma, vmf->address, vmf->pte);
} else {
    ……
}

```

 **do_fault: ** finish_fault 调用 do_set_pte，根据vma的prot生成新的entry（``mk_pte(page, vma->vm_page_prot)``），调用set_pte_at/__set_pte_at/set_pte将pte设置成这个entry（``set_pte_at(vma->vm_mm, addr, vmf->pte, entry)``）。

+ 对于写私有页的情况，pte的权限是只可读的，写页面时，才会进入do_cow_fault，新建vma和pte，设置它的可写权限。
+ 对于写共享页的情况，如果vma_wants_writenotify，那相应的pte会被设置为只读的，进do_shared_fault的时候才会置上写权限。

![img](https://img-blog.csdnimg.cn/img_convert/83021d2948f6b785ccce9d98d2fcbd0b.png) 

```C
//文件映射：
/*判断逻辑如下，有优先级，如果不是WRITE错误，
就直接进入read错误，即exec错误也会进入这里
write错误分为两种，一种是非shared的，一种是shared*/
if (!(vmf->flags & FAULT_FLAG_WRITE))
    ret = do_read_fault(vmf);
else if (!(vma->vm_flags & VM_SHARED))
    ret = do_cow_fault(vmf);
else
    ret = do_shared_fault(vmf);

```

上述调用到的 vm_ops -> map_pages 为 filemap_map_pages，vm_ops->fault 为 filemap_fault 或者 shmem_fault。

**do_anonymous_page:**  malloc/mmap 分配的堆区或者mmap区空间；用户栈不够时，栈区的扩大。分配的都是零页，zero-page指一个页，这样可以压缩使用的物理内存空间，所有的内容为0的虚拟页都可以映射到这个物理页上，要写的时候再用copy-on-write的机制，重新分配页进行写。

 ![img](https://img-blog.csdnimg.cn/img_convert/cd6fb641eba63694dc36fb83030b6db9.png) 

**do_wp_page：**写时复制 wp（write protect），有可能不需要复制，比如共享可写文件，则调用wp_page_shared，置上pte的写权限和dirty位即可，对于私有文件，调用wp_page_copy。

 <img src="https://img-blog.csdnimg.cn/img_convert/e8ce7c0f0133ba9a4258da4112038374.png" alt="img" style="zoom:80%;" /> 

 <img src="https://img-blog.csdnimg.cn/img_convert/2586d984f0d0cdc7e8eb69aadbcfada4.png" alt="img" style="zoom: 80%;" /> 

### SWAP 机制

对于文件页的回收：直接回收page cache（如果该物理页是clean的，即相对磁盘上的文件内容没有修改），或者将脏页写回磁盘后回收page cache。page cache 上的文件页被回收之后，下一次访问的时候会进的是page fault的do_fault分支（pte内容为空），从磁盘中的文件读回到内存page cache中来。

对于**匿名页的回收：swap机制**，将其写入磁盘后（匿名页没有对应的文件，必须写回，不然数据丢失），再回收

do_swap_page： 根据pte中指示的sawp entry在swap cache / swap area里面找到对应的页，后者会分配新的物理页，将该磁盘页内容加载到物理页上，前者直接采用swap cache 的物理页，不需要进行磁盘IO，最后填充pte，建立虚实地址映射。

+ swap cache：用于减少磁盘读写的次数。

+ swap cache中的页：most of the pages evicted that are either waiting to be written to the secondary swap space or got recently read back into memory.
  + 种类1：swap out 过程中修改完了pte，但是写磁盘操作未完成/物理页未被回收的页。例：参见mm/vmscan.c: shrink_page_list()，它调用的add_to_swap()会把swap cache页面标记成dirty，然后它调用try_to_unmap()将页面对应的page table mapping都删除，再调用pageout()回写dirty page，最后try_to_free_swap()会把该页从swap cache中删除。
  + 种类2：上一次 swap out 且 swap in 进来后，内容没有更改的页。例：当匿名页的内容发生变化时会删除对应的swap cache，代码参见mm/swapfile.c: reuse_swap_page()。
  + 功能：
    + 在 swap out 过程中，其它进程仍可以依据 page cache 读该页 （避免磁盘读），
    + swap out 某页时，先查是否在 swap cache 中，若在则不需要写磁盘（避免磁盘写，磁盘上已经有了一份相同的备份）
    +   需要 swap in 某页时，不重复分配物理页，利用 swap cache 中映射的物理页建立映射（避免磁盘读和内存中存有多份拷贝，多进程swapin）  

**do_swap_page流程：**

1） entry = pte_to_swp_entry(orig_pte)得到交换槽位信息；

2)    page = lookup_swap_cache(entry)查看交换槽对应的页面是否存在于交换缓存中，如果是则跳到第6步；

3)    调用**swapin_readahead**(entry, address, vma)从交换区中读取一组页面，函数中会对每个页面调用read_swap_cache_async()读取该页面（返回page，则表明已经读出来到了交换缓存中）；

4)    对于进程访问异常的页面再次调用read_swap_cache_async()读取该页面。因为swapin_readahead调用可能失败，在它成功的情况下read_swap_cache_async()发现该页面在交换缓存里，很快返回；

5)    如果页面还是没有在交换缓存中，可能存在其他内核控制路径已经把该页面换入。比较page_table对应的pte内容与orig_pte是否相同，如果不同，说明页面已经换入。函数跳出返回。

6)    如果页面在交换缓存中，调用mark_page_accessed并锁住该页面；

7)    pte_offset_map_lock(mm, pmd, address, &ptl)获取page_table对应的pte内容，与orig_pte比较，判断是否有其他内核控制路径进行换入操作；

8)    测试PG_uptodate标志，如果未设置，则出错返回；

9)    增加mm->anon_rss的计数；

10) mk_pte(page, vma->vm_page_prot)创建PTE并设置标志，插入到进程页表中；

11) page_add_anon_rmap()为该匿名页插入反向映射数据结构的内容；

12) swap_free(entry)释放页槽；如果磁盘上这个swap_entry对应的引用为0，则该swap_entry的页而已释放出来，供其它换出的页使用。

13) 如果write_access标志为1，说明是COW写时复制，调用do_wp_page()拷贝一份该页面；

  **读入的页：**

do_swap_page读入的页数量：先查fault页是否在swap cache中，如果在其中，则后续直接建立映射，并返回；如果不在，走swapin_readahead分支（内部调用__read_swap_cache_async），读入包括fault页面在内的若干页到swap cache中，但只返回fault页的page结构，对该页建立映射，返回。

待看： 

[内存管理特性分析（十一）:linux swap机制及优化技术分析 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/607295583) 

 [linux内存源码分析 - 内存回收(整体流程) - tolimit - 博客园 (cnblogs.com)](https://www.cnblogs.com/tolimit/p/5435068.html) 

### 内存脏页写回

脏页写回只对共享文件映射有意义。

+ 对于匿名映射，这个页本就不需要保留到磁盘上。除了页面回收的情况。
+ 对于私有文件映射，当第一次写的时候就会分配一个新的匿名映射页给该页，之后都读写新匿名映射页，修改不反应到磁盘上。除了页面回收的情况。
+ 共享文件映射，不论是一个进程还是多个进程，对于页面的修改都是直接反映到page cache上，并且要求文件也相应修改。

写页面的时候，MMU会自动将pte的脏位置上。

每个物理页在内核中都对应一个struct page，在进程退出、页面回收，底层执行ummap的时候，会将pte上的dirty位写到对应的struct page结构中（对于普通的系统调用write写文件的情况，在系统调用中会设置上页面的脏位）。操作系统对于页面的写回依据的就是struct page上的dirty位。

为了更加细粒度地感知页面的脏与否，我们期望实现的是第一次写页面时，page 的 dirty位置上，待写回磁盘后， dirty位清0，下一次写时，又置上。

+ 若vma上是可读可写，第一次访问某页时是读，则在do_read_fault中只给pte置上读权限，下一次写访问会进do_wp_page，置上写权限，并标记page dirty；若第一次访问是写，则进入do_shared_page，将文件页置上写权限，并标记dirty。
+ 写回磁盘后，会通过反向映射机制，将所有映射这个页的页表项的脏位清0，并设置写保护wprotect，struct page 脏位清0。后续写该页又会进do_wp_page。

实现：在mmap/mprotect的时候，vma上的vm_page_prot会置上只可读权限，mk_pte的时候依据的是vma上的vm_page_prot，在do_shared_page 和 do_wp_page 中额外置上写权限的 mkwrite。

do_mmap中调用mmap_region(或do_mprotect_pkey调用mprotect_fixup)，里面调用vma_set_page_prot，内容如下：基于 5.19.4 版本 Linux 内核。

```c
/* Update vma->vm_page_prot to reflect vma->vm_flags. */
void vma_set_page_prot(struct vm_area_struct *vma)
{
	unsigned long vm_flags = vma->vm_flags;
	pgprot_t vm_page_prot;

	vm_page_prot = vm_pgprot_modify(vma->vm_page_prot, vm_flags);
	if (vma_wants_writenotify(vma, vm_page_prot)) {
		vm_flags &= ~VM_SHARED;
		vm_page_prot = vm_pgprot_modify(vm_page_prot, vm_flags);
	}
	/* remove_protection_ptes reads vma->vm_page_prot without mmap_lock */
	WRITE_ONCE(vma->vm_page_prot, vm_page_prot);
}
```

vma_wants_writenotify的注释如下：

```
/*
 * Some shared mappings will want the pages marked read-only
 * to track write events. If so, we'll downgrade vm_page_prot
 * to the private version (using protection_map[] without the
 * VM_SHARED bit).
 */
```

vma_pgprot_modify内容如下：

```c
return pgprot_modify(oldprot, vm_get_page_prot(vm_flags))
```

vm_get_page_prot内容如下：

```c
pgprot_t vm_get_page_prot(unsigned long vm_flags)
{
    /*根据protection_map的映射，将vm_flags转化为对应的vm_page_prot*/
	pgprot_t ret = __pgprot(pgprot_val(protection_map[vm_flags &
				(VM_READ|VM_WRITE|VM_EXEC|VM_SHARED)]) |
			pgprot_val(arch_vm_get_page_prot(vm_flags)));

	return arch_filter_pgprot(ret);
}
```

![1678350819093](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1678350819093.png)

![1678350873850](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1678350873850.png)

ARM 的定义：

+ 由此，共享可写可执行/共享可写可读可执行对应的是S110/S111，即page_shared_exec，里面标识为只可读，可写，可执行(UXN标识用户不可执行，PXN标识特权态不可执行)。
+ 私有可写可执行/可写可读可执行S110/S111，即page_readonly_exec,只可读和执行权限。需要写的时候COW。
+ 共享可读可写/可写，page_shared，标识只可读，可写
+ 私有可读可写/可写，page_readonly，标识只可读，写时 COW。

![1678350794224](C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1678350794224.png)

+ 由此，未去掉shared位：第一次写进page fault的handle_pte_fault，pte有写权限，fault是因为readonly位置上了， 不进do_wp_fault，进后面的mk_dirty，在这个函数中会把dirty位置上，且如果有write位，则将rdonly位去掉。

+ 去掉shared位之后：pte上没有写权限，进handle_pte_fault的do_wp_page，针对vma上标识为私有可写的文件，则copy on write，标识为共享可写的文件（即此处因为writenotify而去掉shared位的情况，注意vma上没有去掉，只是在计算vm_page_prot时，传入的参数去掉了），进入wp_page_shared，在其中置上写权限和脏位。
+ 总结：未开writenotify的共享可写页，只有第一次会进page fault，去掉只可读，置上脏位。开了writenotify的共享可写页，定期（第一次写和写回磁盘后的第一次写）会进page fault的do_wp_page。私有可写页，只有第一次进page fault 的 do_wp_page, 写时复制。

验证：

（1) 针对私有的可读可写可执行的页(VMA上没有VM_SHARED标志)，对应的page_prot/pte为page_readonly_exec。mprotect将代码页改为可读可写可执行之后(包括pte)，该页第一次写时会进入handle_pte_fault。各权限值如下：

```c
mm_flags:255 记录错误信息1001010101 FAULT_FLAG_WRITE, FAULT_FLAG_ALLOW_RETRY, FAULT_FLAG_KILLABLE, FAULT_FLAG_USER, FAULT_FLAG_INTERRUPTIBLE
vma->vma_falgs: 100077 VM_READ, VM_WRITE, VM_EXEC, VM_MAY_READ, VM_MAY_WRITE, VM_MAY_EXEC, VM_ACCOUNT
vma->vm_page_prot: 20000000000fc3 PTE_PXN, PTE_NG, PTE_AF, PTE_SHARED(inner shareable), PTE_RDONLY, PTE_USER, PTE_TYPE_PAGE
old pte:20000129db8fc3 rdonly=1 write=0 uxn=0,dirty=0，中间部分是页框号，前后的属性值与vm_page_prot相同。
new pte:a80001250def43 rdonly=0 write=1 uxn=0，dirty=1 写时复制，物理页框号改变了
```

(2)可读可写的共享文件

+ mmap后先写后读。因为writenotify，vm_page_prot上没有写权限，第一次访问时进do_fault的do_shared_fault，置上写权限，清除rdonly，置脏位。

```c
第一次进page fault：
    mm_flags: 255 写错误
	vma->vm_flags:fb VM_READ, VM_WRITE, VM_SAHRED, VM_MAY_READ/WRITE/SHARED
	vma->vm_page_prot:60000000000fc3 PTE_UXN, PTE_PXN, PTE_RDONLY
	old pte:0 第一次访问，pte未分配
    new pte:e800011cb6bf43 rdonly=0 write=1 uxn=1 dirty=1，PTE_DIRTY, PTE_UXN, PTE_PXN, PTE_DBM(PTE_WRITE), 
第二次进page fault：?
    mm_flags: 275 1001110101 FAULT_FLAG_WRITE, FAULT_FLAG_TRIED
	vma->vm_flags:fb 
	vma->vm_page_prot:60000000000fc3 
	old pte:e800011dca4f43 rdonly=0 write=1 uxn=1
    old pte:e800011dca4f43 rdonly=0 write=1 uxn=1
```

+ mmap后先读后写。第一次读，依据vm_page_prot置pte权限，没有写权限。第二次写，进do_wp_page的wp_page_shared，置上写权限，清除rdonly，置脏位。

```c
第一次进page fault：
    mm_flags: 254 非写错误，非取指错误，即读错误
	vma->vm_flags:fb
	vma->vm_page_prot:60000000000fc3
	old pte:0
	new pte:6000011cb6bfc3 rdonly=1 write=0 uxn=1
第二次进page fault：
    mm_flags: 255
	vma->vm_flags:fb
	vma->vm_page_prot:60000000000fc3
	old pte:6000011cb6bfc3 rdonly=1 write=0 uxn=1
	new pte:e800011cb6bf43 rdonly=0 write=1 uxn=1
第三次进page fault:
	与mmap先写后读的第二次进page fault的情况相同。
```





### mremap：

**扩展或收缩现有的内存映射**

```
void *mremap(void *old_address, size_t old_size,
             size_t new_size, int flags, ... /* void *new_address */);
```

返回指向新虚拟映射内存区域的指针。 

flags可以为0或者有：

+ MREMAP_MAYMOVE：当现有空间不够展开时，可以将其重新映射到新的虚拟地址。

+ MREMAP_FIXED：和MREMAP_MAYMOVE一起用，指定移动到参数 new_address处
+ MREMAP_DONTUNMAP：和MREMAP_MAYMOVE一起用，原映射位置不 unmap，即在新地址建立映射之后，原映射默认unmap。

### mprotect：

**修改vma和对应页pte权限** 

``int mprotect(void *addr, size_t len, int prot)``，addr必须是内存页的起始地址（需要页对齐），len为修改属性区域的长度（若不是页大小整数倍，实际实现会向上取整），prot可以取 PROT_READ/WRITE/EXEC/NONE（内存段不可访问）/PROT_SEM（内存可原子操作）等。 

+ 系统调用将会调用到 do_mprotect_pkey：根据start = addr 和 end = addr+len 值找到区间内的各个vma。这里将传入的prot转为对应的vm_flags，原先vma flags上的读写执行位都会清空，设置为mprotect传入的。只有在原先的vma上MAYREAD位为1时，才可以将其设为READ，write和exec同理，否则会报错返回。
+ 调用 mprotect_fixup 修改各 vma 的 vm_flags 和相应的 vm_page_prot（若改的不是整个vma的权限，则会split vma）。
+ fixup 中调用 change_protection 逐级索引到pte，最后调用 change_pte_range 修改pte的权限。
  + 注意：若该页面还没有分配pmd，pte等，则在此处不会分配。比如mmap一个匿名页，没有访问，然后直接mprotect修改权限，则没有改到pte的权限，待后续第一次访问建立pte的时候，pte上的权限就是新的prot对应的权限了。



### 线下扫描内容确定

链接视角还是可执行视角？采用可执行视角，扫描有执行权限的LOAD段（内嵌数据进行分离之后再扫描）。

### 线上扫描内容确定

新增可执行页的情况：

+ 初始化elf时mmap的区域：hook do_set_pte

+ 用户进程通过mprotect修改的区域：hook mprotect？ 修改了vma和pte的权限

+ 共享内存shmget：同一个进程/不同进程可以有两段虚拟地址映射到同一段物理内存， 一段设置vma为可写，一段设置vma为可执行。页表上pte怎么设置的？ arm规定一个物理页不能同时可写可执行，怎么实现？两个进程有各自的页表标识可写/可执行权限，MMU怎么拦截？

+ ...

  

#### 一、第一次访问某页时的 page fault

在 handle_pte_fault 的 !vmf->pte 分支处理。

##### 1、对于文件页和匿名共享页，进入do_fault函数处理：

例子：execve 时 mmap 的动态库和测试文件的代码段，数据段等。

hook do_set_pte: 

+ 只会被 filemap_map_pages（调用vma->vm_ops->map_pages时，对应到该函数） 和 finish_fault 调用 

+ 正确性：对于有执行权限的区域，若要执行，第一次访问一定进 do_fault 的 do_read_fault，此处调用上面两个函数置上pte权限，若有执行权限，则scan；若要写，则该页一定没有执行权限，不scan。
+ 冗余性：只会在do_fault处调用到do_set_pte。
  + finish_fault 只会被 do_fault 下的三个函数调用
  + filemap_map_pages 经由vma->vm_ops->map_pages方式调用，而该方式只会被 do_fault_around 调用

**问题：**

privilege inst中的bad inst出现的有：

+ 110101010011100000000000000：MRS MIDR_EL1：？？EL0下不可执行
+ 110101010011101111100000010：MRS CNTVCT_EL0：CNTHCTL_EL2寄存器的EL0VCTEN域为1，可以执行。修正：MRS <Xt>,CNTFRQ_EL0，MRS <Xt>,CNTVCTSS_EL0，MRS <Xt>,CNTVCT_EL0，  MSR CNTFRQ_EL0,<Xt> 由conditional-undefine 改为 conditional-equal
+ 110101010011011000101011111：读debug寄存器（不是external debug，是D13.3章的debug寄存器）未定义，存在glibc的数据部分，但是和.text等在一个vma中。
+ 110101010001011111010000011：写debug寄存器，未定义，理由同上

gcc hello.c -o hello.spec -Wl,--rpath=/home/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/lqj/glibc-install/lib/ld-linux-aarch64.so.1

**解决：**

##### （1）重新编译glibc源码，将数据和代码分离

编译glibc流程：

+ 下载源码到glibc-2.31
+ 新建build目录和glibc-install目录
  + build 里面先``export LDFLAGS="-Wl,-z,separate-code"``
  + 然后执行``../glibc-2.31/configue --prefix = /home/ubuntu/lqj/glibc-insatll``
  + 然后make
  + 然后make install，至此可执行文件已经在glibc-install目录下，其中可执行动态库文件 libc-2.31.so在 glibc-insatll/lib/中，glibc中包含的动态链接器也在glibc-install/lib/，可以readelf -l 查看libc-2.31.so的分段情况，里面说明了 libc 执行时指定使用的动态链接器的路径，也即glibc自己的动态链接器。

注意：LDFLAGS="-Wl,-z,separate-code"在configue前export，不直接加在configue的参数中。

使用新的glibc：

```c
gcc hello.c -o hello.spec -Wl,--rpath=/home/ubuntu/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/ubuntu/lqj/glibc-install/lib/ld-linux-aarch64.so.1
```

注意，lib目录下还有一个ld-2.31.so，实际上ld-linux-aarch64.so.1是软链接到ld-2.31.so的，用上述命令编译出来的程序实际上用的是ld-2.31.so。查看软链接情况：ls -al。

rpath指 run-time search path, 它规定了可执行文件在寻找 .so 文件时的第一优先位置。搜索.so的优先级顺序：

- RPATH： 写在elf文件中
- LD_LIBRARY_PATH： 环境变量
- RUNPATH： 写在elf文件中
- `ldconfig`的缓存： 配置`/etc/ld.conf*`可改变
- 默认的`/lib`, `/usr/lib`

##### （2）判断mrs midr_el1执行情况

EL0下可以执行该指令，类似的读寄存器指令：

+ 类似指令：3074 AIDR_EL1, 3104 CCSIDR2_EL1, 3107 CCSIDR_EL1, 3109 CLIDR_EL1, 3300 GMID_EL1, 3435 ID_AA64AFR0_EL1, 3436 ID_AA64AFR1_EL1, 3440 ID_AA64DFR0_EL1, 3441  ID_AA64DFR1_EL1, 3445 ID_AA64ISAR0_EL1, 3451 ID_AA64ISAR1_EL1,  3453 ID_AA64ISAR2_EL1,...

出现文件：glibc-2.31/sysdeps/unix/sysv/linux/aarch64/cpu-features.c

解决：该类指令尽管走systemTrap，但是系统模拟了指令行为，返回了真实的寄存器值或者safe 寄存器值。目前认为该类指令可以执行，其中行为不一致的指令，待后续处理。

##### 2、对于匿名私有页，进入do_anonymous_page函数处理：

例子：malloc/mmap 分配的堆区或者mmap区空间；用户栈不够时，栈区的扩大。

可能的扫描原因：用户可以采用上一次存留的数据来执行，所以第一次进do_anonymous_page，分配一个物理页，设置成可执行的时候就需要扫描。

解决：匿名页的物理页分配是被清0了的，第一次访问时一定是全0。

hook do_anonymous_page（static函数不可hook，hook最上层的handle_mm_fault）：

+ 该函数调用static inline 函数 set_pte 设置页表项，不可hook。

+ 页表项 PTE 设置，见arch/arm64/include/asm/pgtable-hwdef.h，例如 PTE_UXN 表示用户不可执行位。如果!(vma->vm_page_prot & PTE_UXN), 则扫描。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1677481111770.png" alt="1677481111770" style="zoom:80%;" />

#### 二、swap out 导致的 page fault：

在 handle_pte_fault 的 !pte_present 分支处理。

tips：SWAP 或者 COW 的页面拷贝的过程中，因为是先拷贝再设置pte， 内核态应用没有机会在页面拷贝（有执行权限）时修改页面

hook do_swap_page()，swap out换进来之后需要再扫描：

+ do_swap_paeg内部会根据vma->vm_page_prot重新生成pte，然后再set pte。
  + vma的prot在swap out之后被修改？通过hook sys_mprotect，已经在改权限的时候将这些页面scan过（scan的时候肯定被swap in了）。
+ 页面内容可能在磁盘上被改动，比如其它进程修改了该文件的内容。
  + 共享文件的情况，shared mmap/fopen：文件页的换进换出，走的do_fault分支，``一``中扫描过了。
  + 共享匿名页的情况，shmget/shm_open：在页修改的时候scan。

预读：预读只完成了一半，将其他的页读到了swap cache中，但是没有建立映射，只需要hook do_swap_page对fault页进行扫描。

#### 三、NUMA自动平衡处理

条件：pte_protnone && vma_is_accessible。前者只有在 !PTE_VALID 且 PTE_PROT_NONE（在arch/arm64/include/asm/pgtable-prot.c中定义，软件定义的pte位，） 时成立，后者在 vma->vm_flags 有 （WRITE|READ|EXEC）至少有一个时成立。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1677481219832.png" alt="1677481219832" style="zoom:80%;" />

前文的 pte_present 条件为 PTE_VALID | PTE_PROT_NONE 一个成立即可，若两个都不成立则认为是swap out情况。走到 NUMA 说明，至少有一个成立，NUMA的条件表明，必须为 VALID 不成立，PROT_NONE成立（注释中的only when xxx，表示只有xxx的时候才看该位，该位才有意义）。

定义： [Concepts overview — The Linux Kernel documentation](https://docs.kernel.org/admin-guide/mm/concepts.html?highlight=numa) 

   UMA（ Uniform Memory Access）是SMP（对称多核处理器）计算机采用的存储器架构。 在UMA架构下，所有的内存访问都被传递到相同的共享内存总线上，不同的处理器访问存储器的延迟时间相同，任何一个进程或线程都可以被分配到任何一个处理器上运行。 现在有很多架构的物理内存是分布式的，由多个cell构成（每个核有自己的本地内存），访问本地内存时更加快， 访问其他CPU的内存或者全局内存的时候就比较慢，这称为非一致性内存访问架构NUMA 。
	根据访问延迟的长短，将这些 memory 划分成了各个 node，每个 node 有自己独立的内存管理系统，比如 free 和 used pages 的 list，set of zones（一个 node 由一些zones构成，一个zone由若干 page frames 构成），各类 counter。

**numa policy:**

 Linux memory policy用来指定在NUMA系统下kernel分配内存时具体从哪个节点获取, 系统的黙认policy为优选当前节点。 Linux memory policy 支持四种不同的模式——DEFAULT，BIND，PREFERRED，和INTERLEAVED，BIND强制内存分配必须在指定节点上完成，PREFERRED模式在内存分配时会优先指定的节点，失败时会从zonelist备选， INTERLEAVED会使内存分配依次在所选的节点上进行。 

**numa balancing：**

 Automatic NUMA balancing可以将task迁移到它大量访问内存的节点上，将task的内存页在晚些时候这个页面被访问到时按照policy的指示移动（当用户改变内存policy）。task_numa_work：在时钟中断处理时加到task work上，去掉此task的内存区的页表（数量由numa_balancing_scan_size决定）清除PRESENT位并且用一个预留位PROT_NONE来标示此页即将产生的Page Fault为NUMA Page Fault。do_numa_page：在Page Fault产生时用来处理NUMA Page Fault，来真正移动不符合内存policy的页面。

**鲲鹏芯片采用 NUMA 架构，M1芯片采用统一内存架构 UMA（现在开启了numa机制）。且认为其不改变执行权限**

#### 四、写时复制 COW 导致的 page fault

调用 wp_page_copy  新增页的页表权限根据vma->vm_page_prot决定。若其可执行，则之前就已经扫描过。

#### 五、mprotect修改权限

hook sys_mprotect，hook mprotect_fixup

page walk，然后通过__pte_to_phys， _phys_to_virt(pa) 函数获得该用户地址在内核线性空间的对应地址，然后访问。

#### 六、mmap 有 map_populate 参数

调用 mm_populate 函数处理该情况，底层调用get_user_pages 函数进行pin memory，将对应虚拟内存与物理页绑定，并且阻止该段内存被swap out到磁盘上。

+ 调用 __gup_longterm_locked->\_\_get_user_pages_locked->\_\_get_user_pages->faultin_page，调用handle_mm_fault。
+ 相当于模拟了一次页第一次访问的page fault，在``一``中已经处理

#### 七、brk内存分配

malloc 底层是利用 brk 或者 mmap，brk分配堆区内存，mmap分配堆区和栈区之间的部分。都是延迟分配内存，都会进 page fault。

#### 八、运行时装载库

dlopen用于运行时装载库：`` void * dlopen (const char *file, int mode) ``

mode：

+ 解析方式
  + RTLD_LAZY：在dlopen返回前，对于动态库中的没有定义的符号不运行解析（仅仅对函数引用有效。对于变量引用总是马上解析）。
  + RTLD_NOW： 须要在dlopen返回前。解析出全部没有定义符号，假设解析不出来。在dlopen会返回NULL，错误为：: undefined symbol: xxxx.......
+ 作用范围，可与解析方式通过“|”组合使用：
  + RTLD_GLOBAL：动态库中定义的符号可被其后打开的其他库解析。
  + RTLD_LOCAL： 与RTLD_GLOBAL作用相反，动态库中定义的符号不能被其后打开的其他库重定位。假设没有指明是RTLD_GLOBAL还是RTLD_LOCAL。则缺省为RTLD_LOCAL。
+ 作用方式：
  + RTLD_NODELETE： 在dlclose()期间不卸载库，而且在以后使用dlopen()又一次载入库时不初始化库中的静态变量。
  + RTLD_NOLOAD： 不载入库。可用于测试库是否已载入(dlopen()返回NULL说明未载入，否则说明已载入），也可用于改变已载入库的flag，如：先前载入库的flag为RTLD_LOCAL，用dlopen(RTLD_NOLOAD|RTLD_GLOBAL)后flag将变成RTLD_GLOBAL。
  + RTLD_DEEPBIND：在搜索全局符号前先搜索库内的符号。避免同名符号的冲突

dlsym 根据 dlopen 返回的句柄查找对应函数，dlclose 关闭动态链接库。例子：

````c
#define LIB_CACULATE_PATH "./libcaculator.so"
typedef int (*CAC_FUNC)(int, int);
int main{
    CAC_FUNC cac_func = NULL;

    handle = dlopen(LIB_CACULATE_PATH, RTLD_LAZY);

    *(void **) (&cac_func) = dlsym(handle, "add");
    printf("add: %d\n", (*cac_func)(2,7));
    cac_func = (CAC_FUNC)dlsym(handle, "sub");
    printf("sub: %d\n", cac_func(9,2));

    dlclose(handle);
}
````

dlopen 是 glibc 的函数，底层利用的是mmap系统调用，也会进 page fault。

#### 九、madvise

对于 MADV_WILLNEED, MADV_POPULATE_READ|WRITE 参数，在madvise_vma中调用 madvise_populate函数处理该情况，里面调用 faultin_vma_pages->__get_user_pages 函数进行pin memory，将对应虚拟内存与物理页绑定，并且阻止该段内存被swap out到磁盘上。模拟page fault，里面调用handle_mm_fault。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1677687987146.png" alt="1677687987146" style="zoom:80%;" />

#### 九、共享内存

shmat返回共享的虚拟地址，底层调用mmap，延迟分配。

#### 十、用户进程运行时写内存，执行内存数据：

开启ARM 的 WXN 不可同时有写和执行权限：将所有可写的页改为不可执行的。未开启 WXN 机制。

tips：只有在最后一级的AP位才有用，上级页表的AP不看。AP[2] Rdonly为1的时候是只可读的，为0的时候可读可写。

（1）mprotect修改可执行页为可写可执行：

mprotect 系统调用会调用vma_set_page_prot 设置vma的vm_page_prot 为 RDONLY 且无UXN，然后依据这个设置pte。

方案：第一次写正常进page fault，置上write，清除rdonly，置上uxn。后续执行的时候进page fault，扫描页，删去UXN, write，置上rdonly。

+ 写的页和执行的代码页是同一页，在write fault中将写权限置上，执行权限删去，返回后，重复执行该指令会失败。解决：禁止 mprotect 指令页为可写可执行（删去传入参数的VM_WRITE），其它的内存可以修改权限为可写可执行 

（2）mmap时将某页权限设置为可写可执行

mmap的页一定不是代码页，采用原方案。

例子（直接mmap设置权限和mmap后mprotect设置权限都可行）。

#### 十一、不同进程运行时通过共享内存，写然后执行

（1） SYSTEM V版本的共享内存：shmget，shmat。mmap文件映射。

+ `shmget()` . 检查权限，设置IPC related 数据结构，通过shmem_file_setup创建新的segment（file in 磁盘的shmfs区）
+ `shmat()` ，获得文件描述符，检查传入的参数后，调用do_mmap，建立映射，新建的vma的vm_ops是 shm_vm_ops，有针对shmfs区文件的shm_open,shm_close操作。

处理：因为写进程第一次写一定会进page fault，则在写进程进page fault时，置上写权限，并rmap，将所有共享虚拟页的执行权限删去（若本没有执行权限，则删去操作无影响），执行进程进page fault，则置上执行权限，并将所有共享虚拟页的写权限删去。还没有开始第一次写时，执行进程可以正常执行

+ 具体：写错误时，将该页的执行权限删去后，调用linux 自己的mm_fault函数置上写权限，然后check_shared，将所有共享页的执行权限删去；执行错误时，将该页的执行权限置上，然后check_shared，将有共享页的写权限删去。
+ 问题1：A进程写，B进程执行，若A页面shmat好，写page fault的时候，B页面还没有分配pte，则写page fault时，不能将B的UXN权限置上，后续A写B执行。反之，若B先执行，A后写，进page fault，则无问题。
+ 解决1：在do_fault（文件页，此处也是文件映射）分配pte的时候，如果是共享可执行页，判断是否有其它的共享可写页，若有，删去可写权限。
+ 问题2：若 shmat 的权限为 RDONLY|EXEC，调用do_mmap的时候发现 struct file 不可写，则会将VM_SHARED位删去，由此在 set pte 和 do_mm_fault 时对所有exec的页都需要 rmap，不能依据VM_SHARED 位。
+ 解决2：~~ARM64 有一个架构相关的 VM_FLAGS 位：VM_HIGH_ARCH_BIT_1 在 ARM 下为 VM_MTE_ALLOWED位（如果!defined(CONFIG_ARM64_MTE)，该值为 VM_NONE，即0，os中定义了该宏），只有在 mm/shmem.c 中会加上 VM_MTE_ALLOWED，arch_calc_vm_flags_bits 和 VM_DATA_DEFAULT_FLAGS 经过都没有加上。~~，在set pte的时候对所有exec的文件页都rmap。
+ 注意：对于write端mmap，unmap，exec端mmap，unmap的交替的情况也适用。对于shmat之后再mprotect也适用，因为我是在第一次写和第一次执行进page fault的地方，修改对方的权限，mprotect之后，仍然会进page fault。

（2）POSIX 版本的共享内存：先shm_open，再mmap。mmap文件映射

tips：实际上和先open一个本地的文件，再mmap设置为SHARED是类似的，只是这个shm_open系统调用帮我们创建了一个文件(/dev/shm下)，不需要我们在自己工作目录下指定/创建文件。

注意：以第一次open的权限为准，若执行端先open，则需要有O_RDWR位，相应地vma上有vm_shared位，若写端先open，执行端可以是O_RDONLY，相应的，没有VM_SHARED位。文件路径为/dev/shm/xxx，若采用open方式打开，则文件路径没有shm前缀。解决：在set pte的时候对所有exec的文件页都rmap。

+ 问题1：write端可以直接调用系统调用write来写文件，直接修改page cache的内容，不进行映射，由此exec端看不到有其它的共享页（且exec端VM_SHARED和VM_PRIVATE均可）。
+ 解决1：hook write系统调用的底层函数__intercept_generic_perform_write，其中调用begin_write后，获得了需要写的page cache，然后根据这个page rmap，将共享的页的执行权限去掉。

（3）父子进程共享内存：子进程copy父进程的页表，不可实现一边写一边执行，某一端若企图写共享的页，会COW分配新页，另一端读出来的仍然为原值。

（4）memfd_create：通过 memfd_create系统调用创建匿名文件（传入文件名xxx），得到fd，通过socket机制进行进程间的数据传输，将fd传给exec端。write端和exec端通过mmap建立映射，写/执行文件内容。匿名文件本质上是不写到磁盘上的文件，vma的file域不为空，文件名为/memfd: xxx，且create打开的文件默认为O_RDWR，写和执行端vma上均保留有VM_SHARED位。按照（1）（2）中的流程处理即可。匿名文件指该文件在有进程引用它期间，保存在磁盘上，无引用时，会从磁盘上删去，即在使用期间，其和普通文件的行为相同。

### SPEC CPU 2017

```c
CC                      = $(SPECLANG)gcc -Wl,-z,separate-code -Wl,--rpath=/home/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/lqj/glibc-install/lib/ld-linux-aarch64.so.1 -std=c++14   %{model}
   CXX                     = $(SPECLANG)g++ -Wl,-z,separate-code -Wl,--rpath=/home/lqj/glibc-install/lib -Wl,--dynamic-linker=/home/lqj/glibc-install/lib/ld-linux-aarch64.so.1 -fpermissive -std=c++03 %{model}
```







TO DO: 

+ 修改为先扫描再设置页表的执行权限/set pte
+ 修改指令为brk，并hook brk指令的异常？
+ 发现关键指令的终止程序，并返回报错 exit,sigkill
+ mremap 移动时会复制pte，即会有新的pte设置，但因为复制前后内容没有更改故不需要考虑。若还没有建立映射，就mremap，则会在新的地址page fault，也不需要考虑。（添加说明）
+ 进程A mmap文件页为共享可写，进程B mmap为私有可读，B可以看到A的修改吗？
+ mmap 私有页并修改文件后，进程结束，文件修改有反映到磁盘上吗?
+ 进程是否可以修改共享库的代码页为可写，获取printf地址？然后mprotect。
+ mprotect 修改为可写可执行（不可读），那还可以读取页面，并扫描吗
+ ARM的页大小不一定为4KB，共支持 4KB/16KB/64KB 三种。AArch64 共有 48-bit/52-bit 两种虚地址空间大小。4KB pages + 4 levels（4级页表） 对应 48-bit虚地址空间（内核和用户都是2^48B），64KB pages+3 levels 对应 52-bit 虚地址空间。



3.15 汇报：

+ hook anonymous page：static 不可hook（有的static的可以hook， 比如do_page_fault，函数有__kprobes关键字，因此可以找到函数？），hook 上级的handle_mm_fault，手动判断（省略部分复杂逻辑：认为我们的系统不支持hugepage）
  + 更新：分配的页是零页，解释堆区和栈区不为0的原因
+ swap 流程，swap around的情况，扫描重复问题(如果对于共享写进行了扫描，这里的扫描是多余的。因为扫描swap页的逻辑是，别的进程会改这个页)。
+ 可写可执行的情况： mprotect/mmap同时有写和可执行权限时，原本第一次写就会进一次page fault（要么是pte未分配，要么是rdonly导致进的page fault），在这里将写权限置上，执行关闭（uxn置上）。后续执行的时候进page fault，将执行打开（clear uxn），写关闭。
+ 共享可写可执行的情况：
  + 共享页：
    + mmap：父子进程间共享mmap的匿名共享页和mmap的文件共享页，无血缘关系进程只能共享mmap的共享文件页。
    + shmget/shmat：内部采用mmap的文件映射实现共享内存。
    + shm_open：基于tmpfs，相当于open tmpfs的文件，后续调用mmap建立文件映射。
  + 方案1：禁止 mmap 同时传入 MAP_SHARED 和 EXEC 参数（禁止 mprotect 对SHARED的vma传入EXEC参数），禁止 shmget 传入 EXEC 参数（禁止 shmctl 修改权限为  EXEC）。
  + 方案2：依据reverse map，将共享的所有虚拟页找出来，若其中有页是有写权限，有页是有执行权限的，则将页的执行权限去掉，执行时进page fault 扫描，执行权限置上 ，其它页的写权限关闭。其它页写时，page fault，将写权限置上，执行页的执行权限关闭。注意：动态增加的共享页





jit compiler 不进page fault，主动查找

shared mm:shmat 之后 mprotect，置上uxn/write。 mmap之后mremap

共享可执行页 和非共享的差别：除了VM_SHAREDS



隔离技术：

key 放在安全区，可信代码需要访问时，打开权限，访问完，关闭权限。

三个概念：空间 u-space/s-space(看高低地址)，页面：page （u-page/s-page), u-space 可以有 u-page，权级：根据寄存器

用户进程运行在特权态，在低地址空间，要exec等，必须得是x-page才可以，但是由此内核就就可以访问kernel app的内容（PAN阻止高权级代码访问EL0下可以访问的page，通常指u-page），将key 改为u-page，LDR不可访问，LDTR可以访问。问题：如果有两个key，特权代码fun1只可以访问key1，fun2只可以访问key2，怎么处理。

多域（>2）隔离：例如：链接多个库，认为不同库的可信程度不同

arm 现在都是2 domion的隔离，将它们组合起来，进行多域隔离。 x86 有 mpk ，硬件支持16个domion

进程内隔离：arm目前没有，kernel app可以采用PAN实现



ARM 反汇编出来仍然有对绝对地址的引用（原因：ARM 指令定长，x86 中可以 mov，x0， .Label，但是ARM中不能够直接存地址（.Label就是64位地址，太长了），ARM 中采用 ldr x1, 0x1234, mov x0, x1 其中 0x1234地址存储立即数内容），不好插桩指令（地址引用偏移都需要改），x86反汇编出来没有对绝对地址的引用。