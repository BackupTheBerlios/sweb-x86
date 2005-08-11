//----------------------------------------------------------------------
//  $Id: traps.c,v 1.2 2005/08/11 16:55:47 nightcreature Exp $
//----------------------------------------------------------------------
//
//  $Log: traps.c,v $
//  Revision 1.1  2005/08/01 08:22:38  nightcreature
//  code from mini-os needed for transition, maybe removedOB
//
//
//----------------------------------------------------------------------


#include "os.h"
#include "hypervisor.h"
#include "xen_memory.h"
#include "lib.h"

/*
 * These are assembler stubs in entry.S.
 * They are the actual entry points for virtual exceptions.
 */
void divide_error(void);
void debug(void);
void int3(void);
void overflow(void);
void bounds(void);
void invalid_op(void);
void device_not_available(void);
void double_fault(void);
void coprocessor_segment_overrun(void);
void invalid_TSS(void);
void segment_not_present(void);
void stack_segment(void);
void general_protection(void);
void page_fault(void);
void coprocessor_error(void);
void simd_coprocessor_error(void);
void alignment_check(void);
void spurious_interrupt_bug(void);
void machine_check(void);


extern void do_exit(void);

void dump_regs(struct pt_regs *regs)
{
	int in_kernel = 1;
	unsigned long esp;
	unsigned short ss;

	esp = (unsigned long) (&regs->esp);
	ss = __KERNEL_DS;
	if (regs->xcs & 2) {
		in_kernel = 0;
		esp = regs->esp;
		ss = regs->xss & 0xffff;
	}
	printf("EIP:    %04x:[<%08lx>]\n",
	       0xffff & regs->xcs, regs->eip);
	printf("EFLAGS: %08lx\n",regs->eflags);
	printf("eax: %08lx   ebx: %08lx   ecx: %08lx   edx: %08lx\n",
		regs->eax, regs->ebx, regs->ecx, regs->edx);
	printf("esi: %08lx   edi: %08lx   ebp: %08lx   esp: %08lx\n",
		regs->esi, regs->edi, regs->ebp, esp);
	printf("ds: %04x   es: %04x   ss: %04x\n",
		regs->xds & 0xffff, regs->xes & 0xffff, ss);
	printf("\n");
}	


static __inline__ void dump_code(unsigned eip)
{
  unsigned *ptr = (unsigned *)eip;
  int x;

  printf("Bytes at eip:\n");
  for (x = -4; x < 5; x++)
      printf("%x", ptr[x]);
}


/*
 * C handlers here have their parameter-list constructed by the
 * assembler stubs above. Each one gets a pointer to a list
 * of register values (to be restored at end of exception).
 * Some will also receive an error code -- this is the code that
 * was generated by the processor for the underlying real exception. 
 * 
 * Note that the page-fault exception is special. It also receives
 * the faulting linear address. Normally this would be found in
 * register CR2, but that is not accessible in a virtualised OS.
 */

static void __inline__ do_trap(int trapnr, char *str,
			   struct pt_regs * regs, long error_code)
{
  printf("FATAL:  Unhandled Trap (see mini-os:traps.c)");
  printf("%d %s", trapnr, str);
  dump_regs(regs);
  dump_code(regs->eip);

  do_exit();
}

#define DO_ERROR(trapnr, str, name) \
void do_##name(struct pt_regs * regs, long error_code) \
{ \
	do_trap(trapnr, str, regs, error_code); \
}

#define DO_ERROR_INFO(trapnr, str, name, sicode, siaddr) \
void do_##name(struct pt_regs * regs, long error_code) \
{ \
	do_trap(trapnr, str, regs, error_code); \
}

DO_ERROR_INFO( 0, "divide error", divide_error, FPE_INTDIV, regs->eip)
DO_ERROR( 3, "int3", int3)
DO_ERROR( 4, "overflow", overflow)
DO_ERROR( 5, "bounds", bounds)
DO_ERROR_INFO( 6, "invalid operand", invalid_op, ILL_ILLOPN, regs->eip)
DO_ERROR( 7, "device not available", device_not_available)
DO_ERROR( 8, "double fault", double_fault)
DO_ERROR( 9, "coprocessor segment overrun", coprocessor_segment_overrun)
DO_ERROR(10, "invalid TSS", invalid_TSS)
DO_ERROR(11, "segment not present", segment_not_present)
DO_ERROR(12, "stack segment", stack_segment)
DO_ERROR_INFO(17, "alignment check", alignment_check, BUS_ADRALN, 0)
DO_ERROR(18, "machine check", machine_check)

void do_page_fault(struct pt_regs *regs, long error_code,
                   unsigned long address)
{
    printf("Page fault\n");
    printf("Address: 0x%lx  ", address);
    printf("Error Code: 0x%lx  ", error_code);
    printf("eip: \t 0x%lx\n", regs->eip);
    do_exit();
}

void do_general_protection(struct pt_regs * regs, long error_code)
{
  HYPERVISOR_shared_info->vcpu_data[0].evtchn_upcall_mask = 0;
  printf("GPF\n");
  printf("Error Code: 0x%lx\n", error_code);
  dump_regs(regs);
  dump_code(regs->eip);
  do_exit();
}


void do_debug(struct pt_regs * regs, long error_code)
{
    printf("Debug exception\n");
#define TF_MASK 0x100
    regs->eflags &= ~TF_MASK;
    dump_regs(regs);
    do_exit();
}



void do_coprocessor_error(struct pt_regs * regs, long error_code)
{
    printf("Copro error\n");
    dump_regs(regs);
    dump_code(regs->eip);
    do_exit();
}

void simd_math_error(void *eip)
{
    printf("SIMD error\n");
}

void do_simd_coprocessor_error(struct pt_regs * regs,
                               long error_code)
{
    printf("SIMD copro error\n");
}

void do_spurious_interrupt_bug(struct pt_regs * regs,
                               long error_code)
{
}

/*
 * Submit a virtual IDT to teh hypervisor. This consists of tuples
 * (interrupt vector, privilege ring, CS:EIP of handler).
 * The 'privilege ring' field specifies the least-privileged ring that
 * can trap to that vector using a software-interrupt instruction (INT).
 */
static trap_info_t trap_table[] = {
    {  0, 0, __KERNEL_CS, (unsigned long)divide_error                },
    {  1, 0, __KERNEL_CS, (unsigned long)debug                       },
    {  3, 3, __KERNEL_CS, (unsigned long)int3                        },
    {  4, 3, __KERNEL_CS, (unsigned long)overflow                    },
    {  5, 3, __KERNEL_CS, (unsigned long)bounds                      },
    {  6, 0, __KERNEL_CS, (unsigned long)invalid_op                  },
    {  7, 0, __KERNEL_CS, (unsigned long)device_not_available        },
    {  8, 0, __KERNEL_CS, (unsigned long)double_fault                },
    {  9, 0, __KERNEL_CS, (unsigned long)coprocessor_segment_overrun },
    { 10, 0, __KERNEL_CS, (unsigned long)invalid_TSS                 },
    { 11, 0, __KERNEL_CS, (unsigned long)segment_not_present         },
    { 12, 0, __KERNEL_CS, (unsigned long)stack_segment               },
    { 13, 0, __KERNEL_CS, (unsigned long)general_protection          },
    { 14, 0, __KERNEL_CS, (unsigned long)page_fault                  },
    { 15, 0, __KERNEL_CS, (unsigned long)spurious_interrupt_bug      },
    { 16, 0, __KERNEL_CS, (unsigned long)coprocessor_error           },
    { 17, 0, __KERNEL_CS, (unsigned long)alignment_check             },
    { 18, 0, __KERNEL_CS, (unsigned long)machine_check               },
    { 19, 0, __KERNEL_CS, (unsigned long)simd_coprocessor_error      },
    {  0, 0,           0, 0                           }
};
    


void trap_init(void)
{
    HYPERVISOR_set_trap_table(trap_table);    
}
