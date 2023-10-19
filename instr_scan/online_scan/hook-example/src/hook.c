#include <uapi/asm/mman.h>
#include <linux/mm.h>
#include <linux/rmap.h>
#include "hook.h"
#include "common.h"

#define CLEAR_EXEC 1
#define CLEAR_WRITE 2


#define HOOK(_name, _function, _original) {     \
                .name = (_name),                \
                .function = (_function),        \
                .original = (_original),        \
        }

struct ftrace_hook {
    const char *name;
    void *function;
    void *original;
    unsigned long address;
    struct ftrace_ops ops;
};

#define USE_FENTRY_OFFSET 0
long (*ORIG(execve))(const struct pt_regs *);
long (*ORIG(do_debug_exception))(unsigned long addr_if_watchpoint, unsigned int esr, struct pt_regs *regs);
long (*ORIG(set_pte))(struct vm_fault *vmf, struct page *page, unsigned long addr);
long (*ORIG(mprotect_fixup))(struct mmu_gather *tlb, struct vm_area_struct *vma, struct vm_area_struct **pprev,
	unsigned long start, unsigned long end, unsigned long newflags);


long (*ORIG(mm_fault))(struct vm_area_struct *vma, unsigned long address,
			   unsigned int flags, struct pt_regs *regs);

long (*ORIG(generic_perform_write))(struct kiocb *iocb, struct iov_iter *i);

void (*ORIG(rmap_walk))(struct folio *folio, struct rmap_walk_control *rwc);
struct anon_vma *(*ORIG(folio_lock_anon_vma_read))(struct folio *folio, struct rmap_walk_control *rwc);

extern void scan_page(unsigned long vaddr);
char pathbuf[PATH_MAX];
char pte_pathbuf[PATH_MAX];
int count = 0;
//char cur_spec_path[PATH_MAX];

int is_monitor_app(struct mm_struct *mm) {
    // char *pathbuf;
    char *path, *suffix;
    struct file *exe_file;
    int ret;

    exe_file = ORIG(get_mm_exe_file)(mm);   
    if (!exe_file)
        return 0;

    // pathbuf = kmalloc(PATH_MAX, GFP_KERNEL);
    // if (!pathbuf) 
    //     return -ENOMEM;
    
    path = file_path(exe_file, pathbuf, PATH_MAX);
    if (IS_ERR(path)) {
		ret = PTR_ERR(path);
		goto free_buf;
	}
  //  if (memcmp(pathbuf,cur_spec_path,PATH_MAX) != 0) {
    //    INFO("new test");
      //  INFO("path = %s\n", path);
        //memcpy(cur_spec_path,pathbuf,PATH_MAX);
    //}
    suffix = strrchr(path, '.');
    if (!suffix) {
        ret = 0;
        goto free_buf;
    }

    ret = (strncmp(suffix, ".mytest", 7) == 0);
    
free_buf:
   // kfree(pathbuf); 
    return ret; 
}


long __intercept_execve(const struct pt_regs *regs) {
    long ret; 

    ret = orig_execve(regs);	

    if (is_monitor_app(current->mm) != 1) { return ret; }

    INFO("Find target app!\n");
    return ret;
}

pte_t* usr_addr_to_pte (unsigned long addr, struct mm_struct *mm) {
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;
    // phys_addr_t pa;

    pgd = pgd_offset(mm, addr); 
    if (pgd_none(*pgd)) {
        // INFO("pgd none");
        return 0;
    }
    p4d = p4d_offset(pgd, addr);
    if (p4d_none(*p4d)) {
        // INFO("p4d none");
        return 0;
    }
    pud = pud_offset(p4d, addr);
    if (pud_none(*pud)) {
        // INFO("pud none");
        return 0;
    }
    pmd = pmd_offset(pud, addr);
    if (pmd_none(*pmd)) {
        // INFO("pmd none");
        return 0;
    }
    pte = pte_offset_kernel(pmd,addr);
    if (pte_none(*pte)) {
        // INFO("pte none");
        return 0;
    }
    if (!pte_present(*pte)) {
        // INFO("!pte present");
        return 0;
    }

    return pte;
    // pa = __pte_to_phys(*pte);
    // return __phys_to_virt(pa);

}

static int page_not_mapped(struct folio *folio){
	return !folio_mapped(folio);
}

bool rmap_one_change_pte(struct folio *folio, struct vm_area_struct *vma, unsigned long addr, void *arg) {
    int type = *(int*)arg;
    char * type_str = (type == CLEAR_EXEC) ? "CLEAR_EXEC" : "CLEAR_WRITE";
    pte_t* pte;
    
    // INFO("type: %s",type_str);
    // INFO("address: %lx", addr);
    pte = usr_addr_to_pte(addr, vma->vm_mm);
    // INFO("old pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));
    
    if (type == CLEAR_EXEC) {

        ((vma->vm_page_prot).pgprot) |= PTE_UXN;
        ((*pte).pte) |= PTE_UXN;
        
        flush_tlb_page(vma, addr);
        // INFO("new pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));

    }else if (type == CLEAR_WRITE){

        ((*pte).pte) &= ~PTE_WRITE;
        ((*pte).pte) |= PTE_RDONLY;
        
        flush_tlb_page(vma, addr);
        // INFO("new pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));

    }

    return 1;
}

void check_shared_pages(pte_t* pte, struct page *page, int fault_type) {
    phys_addr_t pa;
    struct page *final_page;

    struct rmap_walk_control rwc = {
        .rmap_one = rmap_one_change_pte,
        .arg = (void*)(&fault_type),
        .done = page_not_mapped,
        .anon_lock = orig_folio_lock_anon_vma_read,
    };
    if(!pte && !page) {
        INFO("check_shared_pages: pte=page=0");
        return;
    }
    if (!page) {
        pa = __pte_to_phys(*pte);
        final_page = __pfn_to_page(__phys_to_pfn(pa));
        orig_rmap_walk(page_folio(final_page), &rwc);
    }else {
        orig_rmap_walk(page_folio(page), &rwc);
    }

}

void __intercept_set_pte(struct vm_fault *vmf, struct page *page, unsigned long addr) {
	unsigned long flags = vmf->vma->vm_flags;
    phys_addr_t paddr;
    unsigned long vaddr;
    int is_shared_mm,is_dyn_lib;
    char* suffix;
    char *path;
    struct file *file = vmf->vma->vm_file; 

        	
	if (is_monitor_app(vmf->vma->vm_mm) != 1) {
        orig_set_pte(vmf, page, addr);
        return;
    }
    
    is_dyn_lib = 0;
    is_shared_mm = 0;
	if (flags & VM_EXEC) {
        
        if (file) {
            path = file_path(file, pte_pathbuf, PATH_MAX);
            
            suffix = strrchr(path, '.');
            if (suffix && !strncmp(suffix, ".so", 3)) is_dyn_lib = 1;

            suffix = strchr(path, '/');
            if (suffix && (!strncmp(suffix, "/usr", 4))) {
                orig_set_pte(vmf, page, addr);
                return;
            };
            if (suffix && (!strncmp(suffix, "/SYSV", 5) || !strncmp(suffix, "/dev/shm", 8) || !strncmp(suffix, "/memfd:", 7))) is_shared_mm = 1;
        }

        // INFO("---------------------SET_PTE EXEC-------------------\n");
        count++;
        if (count % 100 ==0) {
            INFO("count = %d",count);
            count = 0;
        }
        // if (file) INFO("path = %s\n", path);
        paddr = (phys_addr_t)((page_to_pfn(page)) << PAGE_SHIFT);
        vaddr = __phys_to_virt(paddr);

        // INFO("u_vaddr: %lx\n", addr);
		scan_page(vaddr);
        orig_set_pte(vmf, page, addr);
        // INFO("vma->vm_flags:%lx\n", vmf->vma->vm_flags); // 100077 RWX
        // INFO("vma->vm_page_prot:%llx\n", pgprot_val(vmf->vma->vm_page_prot));//20000000000fc3 PTE_PXN, PTE_NG, PTE_AF, PTE_SHARED, PTE_RDONLY, PTE_USER, PTE_TYPE_PAGE
        // if (is_shared_mm || (flags & VM_SHARED)) {
        //     INFO("***************in our SHARED EXEC page fault (do_set_pte) *************");
        //     check_shared_pages(0, page, CLEAR_WRITE);
        //     // INFO("check shared page finished(do_set_pte)\n");
        // }
        return;
	}
    
    /*exec do_set_pte */
	orig_set_pte(vmf, page, addr);

	return;
}

vm_fault_t __intercept_mm_fault(struct vm_area_struct *vma, unsigned long address, unsigned int flags, struct pt_regs *regs ) {
    
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *pte;

    unsigned long kernel_linear_addr;
    struct mm_struct *mm = vma->vm_mm;

    vm_fault_t ret;

    if (is_monitor_app(mm) != 1) {
        goto orig_mm;
    }
    
    pgd = pgd_offset(mm, address); 
    if (!pgd || pgd_none(*pgd)) {
        // INFO("mm: pgd none");
        goto orig_mm;
    }
    p4d = p4d_alloc(mm, pgd, address);
    if (!p4d || p4d_none(*p4d)) {
        // INFO("mm: p4d none");
        goto orig_mm;
    }
    pud = pud_alloc(mm, p4d, address);
    if (!pud || pud_none(*pud)) {
        // INFO("mm: pud none");
        goto orig_mm;
    }
    pmd = pmd_alloc(mm, pud, address);
    if (!pmd || pmd_none(*pmd)) {
        // INFO("mm: pmd none");
        goto orig_mm;
    }
    pte = pte_offset_map(pmd,address);


    //private:已有pte，之后执行时的错误
    if (((vma -> vm_flags & (VM_EXEC|VM_WRITE)) == (VM_EXEC|VM_WRITE)) && !(vma->vm_flags & VM_SHARED) && (pte_val(*pte) & PTE_UXN)) {
        // INFO("--------------in handle_mm_fault--------------\n");
        // INFO("u_vaddr: %lx\n", address);
        // INFO("mm_flags: %x\n", flags); //255(1001010101) W
        // INFO("vma->vm_flags:%lx\n", vma->vm_flags); // 100077 RWX
        // INFO("vma->vm_page_prot:%llx\n", pgprot_val(vma->vm_page_prot));//20000000000fc3 PTE_PXN, PTE_NG, PTE_AF, PTE_SHARED, PTE_RDONLY, PTE_USER, PTE_TYPE_PAGE
        INFO("********in our PRIVATE exec page fault********");
        // INFO("old pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));
        

        //scan page
        kernel_linear_addr = __phys_to_virt(__pte_to_phys(*pte));
        if(kernel_linear_addr != 0) {
            scan_page(kernel_linear_addr);
            // INFO("mm fault of WRITE & EXEC: finish scan one page before exec");
        }

        ((vma->vm_page_prot).pgprot) &= ~PTE_UXN;

        ((*pte).pte) &= ~PTE_UXN;
        ((*pte).pte) &= ~PTE_WRITE;
        ((*pte).pte) |= PTE_RDONLY;
 
        flush_tlb_page(vma, address);
        // INFO("new pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));

        return 0;
    }

    //private: 第一次访问该页的情况（!pte），和之后写该页的情况(已有pte，rdonly触发)
    if (((vma -> vm_flags & (VM_EXEC|VM_WRITE)) == (VM_EXEC|VM_WRITE)) && !(vma->vm_flags & VM_SHARED) && !(pte_val(*pte) & PTE_WRITE)) {
        // INFO("--------------in handle_mm_fault--------------\n");
        // INFO("u_vaddr: %lx\n", address);
        // INFO("mm_flags: %x\n", flags); //255(1001010101) W
        // INFO("vma->vm_flags:%lx\n", vma->vm_flags); // 100077 RWX
        // INFO("vma->vm_page_prot:%llx\n", pgprot_val(vma->vm_page_prot));//20000000000fc3 PTE_PXN, PTE_NG, PTE_AF, PTE_SHARED, PTE_RDONLY, PTE_USER, PTE_TYPE_PAGE
        INFO("********in our PRIVATE write page fault********");
        // INFO("old pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));
        
        ((vma->vm_page_prot).pgprot) |= PTE_UXN;

        ret = orig_mm_fault(vma,address,flags,regs);
        
        // INFO("new pte:%llx rdonly=%d write=%d uxn=%d\n", pte_val(*pte), !!(pte_val(*pte) & PTE_RDONLY), !!(pte_val(*pte) & PTE_WRITE), !!(pte_val(*pte) & PTE_UXN));

        return ret;
    }

    //shared: 已有pte，执行错误
    if(/*(vma->vm_flags & VM_SHARED) && */(vma->vm_flags & VM_EXEC) && ((pte_val(*pte) & PTE_UXN))) {
        INFO("********in our SHARED exec page fault********");
        
        //scan page
        kernel_linear_addr = __phys_to_virt(__pte_to_phys(*pte));
        if(kernel_linear_addr != 0) {
            scan_page(kernel_linear_addr);
            // INFO("mm fault of shared WRITE & EXEC: finish scan one page before exec");
        }

        ((vma->vm_page_prot).pgprot) &= ~PTE_UXN;
        ((*pte).pte) &= ~PTE_UXN;
        flush_tlb_page(vma, address);
        check_shared_pages(pte, 0, CLEAR_WRITE);
        // INFO("check shared pages finished(in exec fault)");
        return 0;
    }

    //shared：无pte和已有pte的写错误
    if((vma->vm_flags & VM_SHARED) && (vma->vm_flags & VM_WRITE) && (!(pte_val(*pte) & PTE_WRITE))) {
        INFO("********in our SHARED write page fault********");
        ((vma->vm_page_prot).pgprot) |= PTE_UXN;
        ret = orig_mm_fault(vma,address,flags,regs);
        check_shared_pages(pte, 0, CLEAR_EXEC);
        // INFO("check shared pages finished(in write fault)");
        return ret;
    }

    ret = orig_mm_fault(vma,address,flags,regs);
    return ret;

orig_mm: 
    ret = orig_mm_fault(vma,address,flags,regs);
    return ret;

}

ssize_t __intercept_generic_perform_write(struct kiocb *iocb, struct iov_iter *i)
{
	struct file *file = iocb->ki_filp;
	loff_t pos = iocb->ki_pos;
	struct address_space *mapping = file->f_mapping;
	const struct address_space_operations *a_ops = mapping->a_ops;
	long status = 0;
	ssize_t written = 0;
    
    if (is_monitor_app(current->mm) != 1) return orig_generic_perform_write(iocb, i);
    // INFO("************in sys_write: generic_perform_write**********");

	do {
		struct page *page;
		unsigned long offset;	/* Offset into pagecache page */
		unsigned long bytes;	/* Bytes to write to page */
		size_t copied;		/* Bytes copied from user */
		void *fsdata;

		offset = (pos & (PAGE_SIZE - 1));
		bytes = min_t(unsigned long, PAGE_SIZE - offset,
						iov_iter_count(i));

again:
		/*
		 * Bring in the user page that we will copy from _first_.
		 * Otherwise there's a nasty deadlock on copying from the
		 * same page as we're writing to, without it being marked
		 * up-to-date.
		 */
		if (unlikely(fault_in_iov_iter_readable(i, bytes) == bytes)) {
			status = -EFAULT;
			break;
		}

		if (fatal_signal_pending(current)) {
			status = -EINTR;
			break;
		}

		status = a_ops->write_begin(file, mapping, pos, bytes,
						&page, &fsdata);
		if (unlikely(status < 0))
			break;

		if (mapping_writably_mapped(mapping))
			flush_dcache_page(page);

		copied = copy_page_from_iter_atomic(page, offset, bytes, i);
		flush_dcache_page(page);

		status = a_ops->write_end(file, mapping, pos, bytes, copied,
						page, fsdata);

        if (page) {
            // INFO("---finsh write a page cache-------");
            check_shared_pages(0, page, CLEAR_EXEC);
            // INFO("check shared pages finished(in sys_write)");
        }

		if (unlikely(status != copied)) {
			iov_iter_revert(i, copied - max(status, 0L));
			if (unlikely(status < 0))
				break;
		}
		cond_resched();

		if (unlikely(status == 0)) {
			/*
			 * A short copy made ->write_end() reject the
			 * thing entirely.  Might be memory poisoning
			 * halfway through, might be a race with munmap,
			 * might be severe memory pressure.
			 */
			if (copied)
				bytes = copied;
			goto again;
		}
		pos += status;
		written += status;

		balance_dirty_pages_ratelimited(mapping);
	} while (iov_iter_count(i));

	return written ? written : status;
}


int __intercept_mprotect_fixup(struct mmu_gather *tlb, struct vm_area_struct *vma, struct vm_area_struct **pprev,
	unsigned long start, unsigned long end, unsigned long newflags) {
    unsigned long scan_addr;
    unsigned long kernel_linear_addr;
    char *suffix, *pathbuf, *path = NULL;
    struct file *file = vma->vm_file; 
    struct mm_struct *mm = vma->vm_mm;
    
    // char* app_path = "/home/ubuntu/lqj/online_scan/hook-example/tests/hello.mytest-64";
    // int length = strlen(app_path);

    if (is_monitor_app(current->mm) != 1) return orig_mprotect_fixup(tlb, vma, pprev, start, end, newflags);	

    if (newflags & VM_EXEC){
        INFO("----------------MPROTECT EXEC%lx---------------------\n",newflags);
        // INFO("start = %lx, end = %lx", start, end);
        if (file) {
            pathbuf = kmalloc(PATH_MAX, GFP_KERNEL);
            path = file_path(file, pathbuf, PATH_MAX);
            // INFO("path = %s\n", path);

            suffix = strrchr(path, '.');
            if ((newflags & VM_WRITE) && suffix && (!strncmp(suffix, ".mytest", 7)) && (vma->vm_flags & VM_EXEC)) {
                INFO("try to mprotect code pages as WRITE & EXEC, error!");
                // ret = orig_mprotect_fixup(vma, pprev, start, end, (newflags & ~VM_WRITE));
                kfree(pathbuf);
                return -EINVAL;
            }  

    	    kfree(pathbuf);
        }

        scan_addr = start;
        while (scan_addr <= end-PAGE_SIZE) {
            pte_t *pte = usr_addr_to_pte(scan_addr, mm);
            scan_addr += PAGE_SIZE;
            if (!pte) continue;
            kernel_linear_addr = __phys_to_virt(__pte_to_phys(*pte));
            if (kernel_linear_addr != 0){
                scan_page(kernel_linear_addr);
                // INFO("mprotect: finish scan one page");
            }
        }
        
    }
    return orig_mprotect_fixup(tlb, vma, pprev, start, end, newflags);;
}

void __intercept_do_debug_exception(unsigned long addr_if_watchpoint, unsigned int esr, struct pt_regs *regs){
    
    /*
    如果imm不是需要模拟的指令编码，则调用原函数，照常报错
    如果是，则模拟完后，修改pc，返回
    */
    unsigned int exception_type = (esr >> 26)&LOW6;
    unsigned int imm12 = esr&((1<<12)-1);
    unsigned index = imm12 >> 5;
    unsigned target = imm12 & LOW5;
    if (is_monitor_app(current->mm) != 1) {
        orig_do_debug_exception(addr_if_watchpoint,esr,regs);
        return;
    }
    
    if (exception_type != 0b111100) {
        /*not brk exception*/
        orig_do_debug_exception(addr_if_watchpoint,esr,regs);
        return;
    }

    INFO("---------------------in brk，index=%x, target=%d----------------------\n", index, target);
    switch(index){
        case 0b0000001:
            INFO("simulate CTR_EL0\n");
            regs->regs[target] = 0x94448004;
            regs->pc = regs->pc + 4;
            break;
        case 0b0101000:
            INFO("simultate ID_AA64DFR0_EL1\n");
            regs->regs[target] = 0x6;
            regs->pc = regs->pc + 4;
            break;
        case 0b0110000:
            INFO("simulate ID_AA64ISAR0_EL1\n");
            regs->regs[target] = 0x21100110212120;
            regs->pc = regs->pc + 4;
            break;
        case 0b0110001:
            INFO("simulate ID_AA64ISAR1_EL1 \n");
            regs->regs[target] = 0x1110211202;
            regs->pc = regs->pc + 4;
            break;
        case 0b0111000:
            INFO("simultate ID_AA64MMFR0_EL1\n");
            regs->regs[target] = 0x111ff000000;
            regs->pc = regs->pc + 4;
            break;
        case 0b0111001:
            INFO("simultate ID_AA64MMFR1_EL1\n");
            regs->regs[target] = 0;
            regs->pc = regs->pc + 4;
            break;
        case 0b0111010:
            INFO("simultate ID_AA64MMFR2_EL1\n");
            regs->regs[target] = 0x100000000;
            regs->pc = regs->pc + 4;
            break;
        case 0b0100000:
            INFO("simultate ID_AA64PFR0_EL1\n");
            regs->regs[target] = 0x1000000110011;
            regs->pc = regs->pc + 4;
            break;
        case 0b0000101:
            INFO("simultate MPIDR_EL1\n");
            regs->regs[target] = 0x80000000;
            regs->pc = regs->pc + 4;
            break;
        case 0b0101100:
            INFO("simultate ID_AA64AFR0_EL1\n");
            regs->regs[target] = 0;
            regs->pc = regs->pc + 4;
            break;
        case 0b0101101:
            INFO("simultate ID_AA64AFR1_EL1\n");
            regs->regs[target] = 0;
            regs->pc = regs->pc + 4;
            break;
        case 0b0101001:
            INFO("simultate ID_AA64DFR1_EL1\n");
            regs->regs[target] = 0;
            regs->pc = regs->pc + 4;
            break;
        case 0b0110010:
            INFO("simultate ID_AA64ISAR2_EL1\n");
            regs->regs[target] = 0;
            regs->pc = regs->pc + 4;
            break;
        case 0b0100001:
            INFO("simultate ID_AA64PFR1_EL1\n");
            regs->regs[target] = 0x20;
            regs->pc = regs->pc + 4;
            break;
        case 0b0000000:
            INFO("simultate MIDR_EL1\n");
            regs->regs[target] = 0x611f0231;
            regs->pc = regs->pc + 4;
            break;
        case 0b0000110:
            INFO("simultate REVIDR_EL1\n");
            regs->regs[target] = 0;
            regs->pc = regs->pc + 4;
            break;
        case 0b0000011:
            INFO("simulate WFI\n");
            regs->pc = regs->pc + 4;
            break;
        default:
            INFO("need filter\n");
            orig_do_debug_exception(addr_if_watchpoint,esr,regs);
    }
    // INFO("after brk\n");
    return;
}
static int fh_resolve_hook_address(struct ftrace_hook *hook) {
   	hook->address = kln(hook->name);
    if (!hook->address) {
        ERR("unresolved symbol: %s\n", hook->name);
        return -ENOENT;
    }

#if USE_FENTRY_OFFSET
    *((unsigned long*) hook->original) = hook->address + MCOUNT_INSN_SIZE;
#else
    *((unsigned long*) hook->original) = hook->address;
#endif

    return 0;
}

static void notrace fh_ftrace_thunk(unsigned long ip, unsigned long parent_ip,
        struct ftrace_ops *ops, struct ftrace_regs *regs) {
    struct ftrace_hook *hook = container_of(ops, struct ftrace_hook, ops);

#if USE_FENTRY_OFFSET
    regs->regs.pc = (unsigned long) hook->function;
#else
    if (!within_module(parent_ip, THIS_MODULE))
        regs->regs.pc = (unsigned long) hook->function;
#endif
}

/**
 * fh_install_hooks() - register and enable a single hook
 * @hook: a hook to install
 *
 * Returns: zero on success, negative error code otherwise.
 */
int fh_install_hook(struct ftrace_hook *hook) {
    int err, err1;

    err = fh_resolve_hook_address(hook);
    if (err)
        return err;

    /*
     * We're going to modify %rip register so we'll need IPMODIFY flag
     * and SAVE_REGS as its prerequisite. ftrace's anti-recursion guard
     * is useless if we change %rip so disable it with RECURSION_SAFE.
     * We'll perform our own checks for trace function reentry.
     */
    hook->ops.func = fh_ftrace_thunk;
    hook->ops.flags = FTRACE_OPS_FL_SAVE_REGS
        // | FTRACE_OPS_FL_RECURSION_SAFE
        | FTRACE_OPS_FL_IPMODIFY;

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 0, 0);
    if (err) {
        err1 = ftrace_set_filter(&hook->ops, (unsigned char *)hook->name, 0, 0);
        if (err1) {
            ERR("both ftrace_set_filter_ip() and ftrace_set_filter failed: %d, %d\n", err, err1);
            return err1;
        }
    }

    err = register_ftrace_function(&hook->ops);
    if (err) {
        ERR("register_ftrace_function() failed: %d\n", err);
        err1 = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
        if (err1)
            ftrace_set_filter(&hook->ops, (unsigned char *)hook->name, 1, 0);
        return err;
    }

    return 0;
}

/**
 * fh_remove_hooks() - disable and unregister a single hook
 * @hook: a hook to remove
 */
void fh_remove_hook(struct ftrace_hook *hook) {
    int err, err1;

    err = unregister_ftrace_function(&hook->ops);
    if (err) {
        ERR("unregister_ftrace_function() failed: %d\n", err);
    }

    err = ftrace_set_filter_ip(&hook->ops, hook->address, 1, 0);
    if (err) {
        err1 = ftrace_set_filter(&hook->ops, (unsigned char *)hook->name, 1, 0);
        if (err1) {
            ERR("both ftrace_set_filter_ip() and ftrace_set_filter failed: %d, %d\n", err, err1);
        }
    }
}

/**
 * fh_install_hooks() - register and enable multiple hooks
 * @hooks: array of hooks to install
 * @count: number of hooks to install
 *
 * If some hooks fail to install then all hooks will be removed.
 *
 * Returns: zero on success, negative error code otherwise.
 */
int fh_install_hooks(struct ftrace_hook *hooks, size_t count) {
    int err;
    size_t i;
    const char * name_rmap_walk = "rmap_walk";
    const char * name_folio_lock_anon_vma_read = "folio_lock_anon_vma_read";

    for (i = 0; i < count; i++) {
        err = fh_install_hook(&hooks[i]);
        if (err)
            goto error;
    }
    ORIG(rmap_walk) = (void (*)(struct folio *, struct rmap_walk_control *))kln(name_rmap_walk);
    ORIG(folio_lock_anon_vma_read) = (struct anon_vma * (*)(struct folio *, struct rmap_walk_control *))kln(name_folio_lock_anon_vma_read);
    return 0;

error:
    while (i != 0) {
        fh_remove_hook(&hooks[--i]);
    }

    return err;
}

/**
 * fh_remove_hooks() - disable and unregister multiple hooks
 * @hooks: array of hooks to remove
 * @count: number of hooks to remove
 */
void fh_remove_hooks(struct ftrace_hook *hooks, size_t count) {
    size_t i;

    for (i = 0; i < count; i++)
        fh_remove_hook(&hooks[i]);
}

static struct ftrace_hook hooks[] = {
    /* Functions hooked */
    // HOOK("do_debug_exception", __intercept_do_debug_exception, &orig_do_debug_exception),
    HOOK("do_set_pte", __intercept_set_pte, &orig_set_pte),
    // HOOK("__arm64_sys_execve", __intercept_execve, &orig_execve),
    // HOOK("mprotect_fixup", __intercept_mprotect_fixup, &orig_mprotect_fixup),
    // HOOK("handle_mm_fault", __intercept_mm_fault, &orig_mm_fault),
    // HOOK("generic_perform_write", __intercept_generic_perform_write, &orig_generic_perform_write)
     
};

int install_hooks(void) {
    int err;

    err = fh_install_hooks(hooks, ARRAY_SIZE(hooks));
    if (err)
        return err;

    INFO("install hooks\n");
    return 0;
}

void remove_hooks(void) {

    fh_remove_hooks(hooks, ARRAY_SIZE(hooks));

    INFO("uninstall hooks\n");
}
