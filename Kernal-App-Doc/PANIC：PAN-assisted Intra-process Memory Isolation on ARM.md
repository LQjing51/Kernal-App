PANIC：PAN-assisted Intra-process Memory Isolation on ARM

LSU 指令（LDTR,STTR）在特权态并且 UAO unprivilege access overide 为 0 时， access memory like being executed at EL0，否则就是正常访存。

Overview：

修改寄存器（PSTATE的CurrentEL，执行特权指令的能力由CurrentEL决定）让它运行在内核态。用户执行时CurrentEL为特权态，但还是用的用户页表（地址空间还是低一半）。

1、它拥有了执行敏感指令和访问内核的数据的能力，怎么处理：

+ 敏感指令：指令扫描阻止。
  + 每个内核态应用的新增可执行页都需要扫描
  + 对于 jit compiler的情况，其 emit 到 u-page code cache 的 jited code，emit 的时候就进行扫描。它在emit之前就是（可写）可执行页了，故会涉及可执行页的修改问题，可以采用权限翻转方法，但是开销大，在emit处扫描就行（但若不用权限翻转，则不能管别的可写可执行的情况，若用，则这里的扫描是不必要的）。
+ 内核空间和用户空间的隔离：（1）用户可以通过特权指令获取内核页表基地址，并切换页表进行访问，这点通过指令扫描阻止；（2）exception的时候还是会切换页表，用户可以跳转到 exception 相关入口去访问内核空间，这点通过 shim 阻止。
  + shim 在 exception 进入和退出时修改 EPD0/EPD1以及 A1（原本EPD0和EPD1一直都是0（原本通过PAN和特权级别隔离阻止双向的空间访问），原本不改A1（kernel 代码是 global的，所有进程执行时TLB中均有，此时改 A1 是为了区分内核态应用的代码和kernel的代码），原本的16 个entry其余12个都是直接跳转到相应内核处理函数，没有这里的user_shim），并且通过 debug register 在shim中设置 TCR_EL1 的指令处设置 breakpoint以及上下文检查（SPSel）机制阻止用户跳转到shim代码中间。

2、利用 UAO+PAN+LSU 创建 IEE

用户空间中需要隔离数据设置为 U-page，其余是 P-page，p-page code 不可以通过 ld/st 命令访问 u-page。IEE 内的 p-page code 可以通过 lsu 访问  u-page，只有在IEE内 lsu 命令才 behave like unprivilege（进入 IEE 时关闭 UAO，退出时打开）。register 的 entry function 和 exit function 可以用来干嘛。

+ harden CFI defenses（shadow stack）：shadow stack 是 u-page，阻止应用代码有 lsu， CFI 代码可以有 lsu 指令。为什么不用 UAO 安全门？threat model 中表明攻击者不可以有任意代码执行能力，不能篡改数据流CFI，由此所有代码都可以有 LSU 指令。
+ harden JIT：两段VA均为 code cache（共享内存，一段 rx，一段rw），rw code cache 为 u-page，PANIC-vDSO 代码通过 LSU 向它注入代码，其余代码只可访问 rx 的 code cache。用 UAO 安全门了，PANIC-vDSO 和 rw code cache 放在 IEE 里面，rx code cache 和其余代码放在 IEE 外。 

Q:

+ 内核态是 EL1？：关闭了 VHE？

  + 是EL2，只是为了表述方便

+ 内核态强制 u-page 的DEP ：指 WXN吗？看 SCTLR_EL1/EL2 WXN 都是 0

  + 不是 WXN，ARM 强制特权态下可写页不可执行，不管有没有开 WXN，指令手册 Table D5-34 注释c，表明了高权级下可写的u page是不可执行的

+ 设置 VBAR_EL1 使得 exception 的入口变成 user_shim_vector，user_shim_exit 也是设置寄存器（kernel_shim 修改寄存器值，让 kernel 退出时调用到 user_shim_exit）。 user_shim_vector 和 user_shim_exit 都是在用户地址空间执行的，其中间会 jump to  kernel_shim，kernel_shim 在内核空间执行，其开关 EPD0，切换页表，并调用真正的异常处理函数。
  
+ 原本这四个 exception_vector 没有用到，此时填充成了 user_shim_vector，它和其它12个 exception vector 干的事情类似吗？不同，其它exception vector 直接跳转到内核相应代码，没有EPD1和ASID的切换等逻辑。  
  
+ 敏感指令处理流程图：每来一条敏感指令就将它存在 array map中，并把其在map中的index作为brk的立即数域对其进行替换。brk imm 只有 16bit，超出0xffff的指令以其PC值作为key存在hash map中，替换的brk imm 域都是 0xffff。brk 异常时，若imm 等于 0xffff，则到 hash map 中依据当前 PC 值查找原指令。

+ 实际上 shadow stack，code cache ，session key，都是放在 IEE 里面吗？fig6(b) 

  + shadow stack 不在 IEE 里面，后面两个在

    

typos：

+ page3：2.3 的第一段结束应该是句号
+ page9：第二段 they are only the entries to be called from the outside 是否应该是 they are the only entires that can be called from the outside
+ page9：6.4的第二段 it will inspect every issued instruction as well as the PANIC module 是否应该将 as well as 改为 like，表示 PANIC module 和这个 PANIC-vDSO 都会扫描代码，而不是 PANIC-vDSO 会扫描 module 的代码
+ page9：7的第三段 protecting the shadow stack，“On the function entry”，没用 IEE 的话，function entry 指的是什么？文中没有出现。
+ page10：8.3的第一段最后一句，是否应该是 whereas PANIC does not