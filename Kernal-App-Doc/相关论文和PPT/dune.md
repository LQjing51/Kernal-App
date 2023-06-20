### Dune：Safe User-level Access to Privileged CPU Features 

64bit x86 Linux 平台

应用可以直接访问经过硬件虚拟化后的硬件资源：exceptions，页表，privilege modes，system call vectors等

+ kernel module：进行虚拟化配置，提供一个process abstraction

+ 用户层的库：帮助应用管理特权硬件资源

#### Introduction

之前的工作，应用直接使用特权资源的方式：

+ 修改内核，以保证hardware access in userspace是安全的
+ 将应用放在有特定内核的虚拟机中，以使用虚拟硬件资源，但是
  + 将应用移到虚拟机中，使得 inherit file descriptors from their parents, spawn other processes, share a file system with their parents 变得不可行，且为每个应用，制定一个特定的guest内核很困难。

本文提出一种让应用使用硬件特性的新途径： using virtualization hardware to provide a process, rather than a machine abstraction。

#### Virtualization and Hardware

**Intel 的硬件辅助的虚拟化VT-x**：CPU 分为 VMX root 和 VMX non-root 两种模式，前者用于跑VMM，不限制原本的指令执行，且允许配置 VT-x的指令执行。后者用于跑guest os，限制指令执行。

+ VMM 执行 VMLAUNCH/VMRESUME 指令的时候，cpu切换到VMX non-root模式；当guest os模式下触发相关异常时或执行VMCALL时，cpu执行 VM exit，跳转到 VMM 入口执行。
+ 通过VMCS（VM control structure）结构保存模式切换时的各类机器状态。VMCS 还保存有配置参数，VMM可以配置VMCS 来配置 guest 的哪些 event 会触发 VM exits，由此可以定义哪些硬件/指令能够暴露给 guest 直接访问/执行。但是有些硬件，non-root模式不会限制其指令执行，用户访问时不触发event/例外，由此，也没有机会导致VM exits，比如访问 interrupt descriptor table(IDT),privilege modes。
+ 内存虚拟化：VT-x通过 extended page table(EPT) 实现，AMD的SVM硬件辅助虚拟化通过类似的 nested page table(NPT) 实现内存虚拟化

**ARM 的硬件辅助虚拟化：**

与 VT-x类似的点：

+ 都可以expose direct access to privileged hardware features
+ Arm 的 System MMU 与 EPT 类似

不同：

+ ARM 提出新的 privilege mod Hyp，运行在 guest OS 下。而 VT-x 有 root 和 non-root 的并列关系。
+ ARM 在VMM 和 guest 之间切换的时候硬件不会自动保存恢复状态，而是软件保存。VT-x是硬件保存和恢复。

**dune 通过 VT-x 机制开放下列硬件给用户：**

+ exceptions：访问该硬件的特权指令：lidt（load interrupt descriptor table register），ltr（load task register），sti（set interrupt flag开中断），cli（clear interrupt 关中断）, iret等。
  + 通常内核向用户报一个异常，需要经过权级切换和 upcall 机制（例如 signals），dune 通过 VT-x deliver exceptions directly in hardware，可以省去异常处理过程中的软件开销。

+ virtual memory：相关指令：mov CRn，INVLPG，INVPCID（PCID：process-context identifier）。
  + dune 开放 page table 给用户，允许它们自己直接控制地址翻译，修改access permissions，global bits，modified/access bits等，不经过系统调用。不影响系统安全性，因为底层的EPT原本就开放了normal process address space 给用户。
  + 用于垃圾回收 GC：GC中获取dirty page的方式为：（1）利用 mprotect 和 signal handler 来维持自己的一套 dirty bit tracking。（2）使用 OS 提供的 dirty bit read api（比如Win32 API GetWriteWatch）。dune 可以直接操控页表、获取硬件异常，同时加快上述两个方案。
+ privilege modes：相关指令：sysret, sysexit, iret。
  + 用于沙箱 sandbox：VT-x 的 non-root 模式就有自己的一套权级（ring0（supervisor mode）- ring3（user mode）），dune通过 non-root 模式的权级限制实现 process 内部的 untrusted-code的隔离，可信代码运行在ring0，页表上标识supervisor bit，不可信代码运行在ring3，不可以访问可信代码区。ring0的代码可以访问我们开放的特权资源，但是 ring3 的不可信代码如果要执行 syscall 访问内核，或者 modify privileged state，会转到ring0 libdune中进行处理，按需放行。

dune框架图：

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1683387623515.png" alt="1683387623515" style="zoom: 50%;" />

dune module 可以截获每次 VM exits

**Comparing to a VMM**

Dune 构造了一个 process environment 而不是 machine environment，即它不支持普通的 guest OS。它 runs on top of an existing OS kernel，是 type-2 hypervisor。其与普通 VMM 的差别如下：

+ 半虚拟化方案中，modified guest os 通过 hypercalls 来请求 VMM 处理。在 Dune 中，hypercall 机制就是普通的 linux system calls。例如：普通的 VMM 会提供一个 hypercall 来 register interrupt handler for a virtual network device，但是在 Dune 中，它的 hypercall 就是 call a read on a TCP socket。
+ 全虚拟化方案中，VMM 会模拟 hardware interfaces 来让 guest os 以为自己像 host os 一样访问所有硬件资源。dune 只开放那些 guest os 可以直接访问（不触发异常）的硬件资源（exceptions，privilege modes），以及 virtual memory 相关的硬件（比如CR3寄存器）给guest os，以及部分 control register，其它的全部认为不可以访问，不对其进行模拟。
  + 因为开放的硬件资源很少，VMM 和虚拟机间切换时的 save and restore 开销减少
+ VMM 将不同 VM 放在 seperate 的虚地址空间，Dune 中通过配置 EPT 让其 reflect address space，这样   memory layout can be sparse and memory can be coherently shared when two processes map the same memory segment ？？？？

**dune process VS normal process**

+ dune process的代码运行在 ring0。尽管部分指令的行为有变化，但是不影响已有代码的兼容性（this does not typically result in any incompatibilities for exsiting code），不管。
+ dune process代码中 system call 都必须转化为 hypercall 形式。libdune中会截获ring0下代码的所有 system call，并redirect it to the kernel as a hypercall。而 dune module 中 hypercall 就是普通系统调用。
+ dune process 可以使用 libdune 提供的 api 来 access to privileged hardware，以避免 user code 中大量architecture-specific code。 

**memory management**

需要解决的问题在：让用户可以直接访问页表，但阻止其对物理地址的随意访问，并且需要提供给guest 用户与 host 用户一样模式的页表，其访问页表时感知不到其在虚拟机中。

<img src="C:\Users\24962\AppData\Roaming\Typora\typora-user-images\1683530824040.png" alt="1683530824040" style="zoom: 67%;" />

EPT 和 kernel page table 由 host os/kernel 管理，user page table 由 guest 管理。

为了给 dune process 提供与 normal process 相同的 address space，需要 match EPT to the kernel's page table（？）。存在两个问题：1、EPT的格式和 kernel page table 格式不同；2、Intel限制了 guest-physical 映射到 host-physical 时可以使用的物理内存大小， 小于 host-physical 大小。

+ 解决1：dune 人为更新 EPT。guest 执行时触发 EPT 缺页异常时，会触发 VM exit，dune module 新增 EPT 项。kernel 需要知道页面的 access 信息（辅助swapping）、dirty 信息（判断是否需要 write-back to disk），页面有的时候还会被 unmapped 掉，dune module 通过 hook into an MMU notifier chain（KVM的方案） 来让 EPT 针对上述情况进行同步（例如，页面被 unmap 的时候，EPT的相应项也得被 evited，并将相应linux page structures 的 dirty  bit 设上）。
  + dune 通过 hook mmu notifier 来保守地感知 EPT 被修改/访问（不一定mmu notifier的时候就是被修改/访问了），intel 有新的硬件支持来感知EPT的修改/访问（？），可以解决该问题。
+ 解决2：限制dune process使用的内存大小，只让其使用 EPT 对应 guest-physical 的前12GB，即不会超出虚拟机使用的物理内存限制。？？？？？？

**Implementation**

复用 KVM 的底层代码（manage low-level VT-x operations），高层代码和KVM不同，因为dune不是一个VMM。dune的实现比 full virtualization更加轻量级，且还通过 VPIDs（virtual processer identifiers）提升性能（VPIDs 给每个dune process 一个 unique TLB tag，由此，hypercalls and context switches 不需要 TLB invalidations）

进程可以通过 an ioctl on the /dev/dune device 不可逆地切换到 dune 模式，在此之前需要为其准备好 page table，切换之后，首先需要执行 libdune 的代码 configures privileged register，比如 load IDT。