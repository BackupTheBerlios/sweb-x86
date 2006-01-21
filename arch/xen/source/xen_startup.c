//----------------------------------------------------------------------
//  $Id: xen_startup.c,v 1.7 2006/01/21 11:10:03 nightcreature Exp $
//----------------------------------------------------------------------
//
//  $Log: xen_startup.c,v $
//  Revision 1.6  2006/01/20 07:20:04  nightcreature
//  updating to xen-3.0, modified sweb main to get the kernel end out of
//  ArchCommon
//
//  Revision 1.5  2005/11/29 15:14:16  rotho
//  did some cleaning up
//
//  Revision 1.4  2005/09/28 16:35:43  nightcreature
//  main.cpp: added XenConsole (partly implemented but works) to replace TextConsole
//  in xenbuild, first batch of fixes in xen part
//
//  Revision 1.3  2005/09/28 15:57:31  rotho
//  some work-progress (but qhacks too...)
//
//  Revision 1.2  2005/08/11 16:55:47  nightcreature
//  preview commit only for robert ;-)
//
//  Revision 1.1  2005/08/01 08:30:25  nightcreature
//  second boot stage based on kernel.c from mini-os
//
//
//----------------------------------------------------------------------

#include "types.h"
#include "hypervisor.h"
#include "os.h"
#include "xen_memory.h"
#include "lib.h"
#include "paging-definitions.h"
#include "xenprintf.h"

//#include "ArchCommon.h"

extern void startup();

extern void* kernel_start_address;
extern void* kernel_end_address;
extern void* bss_start_address;
extern void* bss_end_address;

extern char text_start_address;

extern char shared_info[PAGE_SIZE]; //defined in head.S
extern char console_page_dummy[PAGE_SIZE]; //defined in head.S

// extern void initialiseArchCommonMemoryMaps(uint32 nr_pages);
// extern void initialiseArchCommonModulesInformation(uint32 nr_modules, pointer start, pointer end);

pointer kernel_end_address_;

/*
 * This structure contains start-of-day info, such as pagetable base pointer,
 * address of the shared_info structure, and things like that.
 */
//placeholder to store startup information, also see hypervisor.h
union start_info_union start_info_union_;

/*
 * Shared page for communicating with the hypervisor.
 * Events flags go here, for example.
 */
shared_info_t *HYPERVISOR_shared_info_;

char console_ready;

/*
 * Just allocate the kernel stack here. SS:ESP is set up to point here
 * in head.S.
 */
char stack[8192]; //not anymore? use the stack the domain builder
//gives us?

/* default event handlers */
//static void exit_handler(int ev, struct pt_regs *regs);
//static void debug_handler(int ev, struct pt_regs *regs);

void print_boottime_paging(page_directory_entry *boot_page_directory);

static shared_info_t *map_shared_info(unsigned long pa)
{
  if ( HYPERVISOR_update_va_mapping(
      (unsigned long)shared_info, __pte(pa | 7), UVMF_INVLPG) )
  {
    //printk("Failed to map shared_info!!\n");
    *(int*)0=0;
  }
  return (shared_info_t *)shared_info;
}

/*
 * INITIAL C ENTRY POINT.
 */

char message_buffer_[1024];
static char *message = "First message...\n";

void start_kernel(start_info_t *si, pointer stack_pointer)
{  
    HYPERVISOR_console_io(CONSOLEIO_write,strlen(message),
                          message);
    //for ( ; ; ) HYPERVISOR_sched_op(SCHEDOP_yield, o);
    kernel_end_address_ = ((stack_pointer / PAGE_SIZE)+1) * PAGE_SIZE;

    /* Copy the start_info struct to a globally-accessible area. */
    // start_info is defined in hypervisor.h
    memcpy(&start_info_, si, sizeof(*si));

    // Grab the shared_info pointer and map it to our va space.
    //HYPERVISOR_shared_info defined as pointer in xen.h
    HYPERVISOR_shared_info_ = map_shared_info(start_info_.shared_info);

     if ( HYPERVISOR_update_va_mapping(
        (unsigned long)console_page_dummy,
        __pte((unsigned long)start_info_.console_mfn *PAGE_SIZE | 7),
        UVMF_INVLPG) )
     {
       *(int*)0=0;
     }

    
    /* Set up event and failsafe callback addresses. */
//     HYPERVISOR_set_callbacks(
//         __KERNEL_CS, (unsigned long)hypervisor_callback,
//         __KERNEL_CS, (unsigned long)failsafe_callback);


      
       
    //initalise phys to machine mapping
    initalisePhysToMachineMapping();
    
    //setup 1:1 mapping of "physical ram" after 3Gb
    //initalisePhysMapping3GB(si->nr_pages);  //FIXXXXXME
    
    //initalise memory maps in ArchCommon
    xenprintf("initalising arch common memory maps\n");
    
    //initialiseArchCommonMemoryMaps(si->nr_pages);
    //initialiseArchCommonModulesInformation(1,si->mod_start, si->mod_start+si->mod_len);
    xenprintf("arch common memory maps and modules information initalised\n");    

    /* init console driver */
    //ctrl_if_init();

    /* trap init*/
    //now done in ArchInterrupts.cpp
    //trap_init();

    /* ENABLE EVENT DELIVERY. This is disabled at start of day. */
    //__sti(); 


    /* print out some useful information  */

    print_short_info(si);
    print_extended_info();

    print_boottime_paging((page_directory_entry *)si->pt_base);
 
    //printmessage();
    xenprintf("\nxen initialisation done\n");

    /* do nothing */
    for ( ; ; ) HYPERVISOR_sched_op(SCHEDOP_yield,0);
    xenprintf("\nnow entering sweb startup\n");    
   /* entering sweb */
   startup();

}

/*
 * do_exit: This is called whenever an IRET fails in entry.S.
 * This will generally be because an application has got itself into
 * a really bad state (probably a bad CS or SS). It must be killed.
 * Of course, minimal OS doesn't have applications :-)
 */

void do_exit(void)
{
    xenprintf("do_exit called!\n"); 
    for ( ;; ) HYPERVISOR_sched_op(SCHEDOP_shutdown,SCHEDOP_shutdown);
}

static void exit_handler(int ev, struct pt_regs *regs) {
    do_exit();
}

/*
 * a debug handler to print out some state from the guest
 */
static void debug_handler(int ev, struct pt_regs *regs) {
  //dump_regs(regs);
}   

void print_short_info(start_info_t *si)
{
     xenprintf("Booting in Xen!\n"); 
     xenprintf("start_info:   %p\n",    si); 
     xenprintf("  nr_pages:   %lu",     si->nr_pages); 
     xenprintf("  nr_pages:   %lu",     start_info_.nr_pages); 
     xenprintf("  shared_inf: %08lx\n", si->shared_info); 
     xenprintf("  pt_base:    %p",      (void *)si->pt_base);  
     xenprintf("  mod_start:  0x%lx\n", si->mod_start); 
     xenprintf("  mod_len:    %lu\n",   si->mod_len);  
     xenprintf("  flags:      0x%x\n",  (unsigned int)si->flags); 
//     //xenprintf("  cmd_line:   %s\n",   
//     //si->cmd_line ? (const char *)si->cmd_line : "NULL");
     xenprintf("kernerl_start: %lx\n", &kernel_start_address);
     xenprintf("kernerl_end: %lx\n", &kernel_end_address);
     xenprintf("bss_start: %lx\n", &bss_start_address);
     xenprintf("bss_end: %lx\n", &bss_end_address);
}

void print_extended_info()
{
  
}

void print_boottime_paging(page_directory_entry *boot_page_directory)
{
  uint32 p = 512;
  uint32 n = 0;
  uint32 i = 0;
  
   for ( p = 0; p < 1024; ++p) //we're concerned with first two gig, rest stays as is
   {
    if(boot_page_directory[p].pde4k.present)
    {
      //uint32 pde_vpn = virtual_page / PAGE_TABLE_ENTRIES;
      //uint32 pte_vpn = virtual_page % PAGE_TABLE_ENTRIES;

      xenprintf("pd: nr: %04d entry: %08lx madr: %08lx v-range: %08lx - %08lx\n", p,
                boot_page_directory[p], boot_page_directory[p].pde4k.page_table_base_address, 
                p << 22, (p << 22)+PAGE_SIZE-1
        );
      for (i=0 ;i<200 ;i++ ) HYPERVISOR_sched_op(SCHEDOP_yield,0);
      
      
//         page_table_entry *pte_base = (page_table_entry *)
//           machine_to_phys(boot_page_directory[p].pde4k.page_table_base_address);

//      page_table_entry *pte_base = (page_table_entry *)(
//          (boot_page_directory[p].pde4k.page_table_base_address));

      page_table_entry *pte_base = (page_table_entry *)
                        (boot_page_directory[p].pde4k.page_table_base_address);
    
      for (n=0; n < PAGE_TABLE_ENTRIES; n++)
        if (pte_base[n].present > 0)
        {          
           xenprintf("  pt: nr %04d entry: %08lx madr: %08lx v-range: %08lx - %08lx\n", n,
                     pte_base[n], pte_base[n].page_base_address,
                     (p << 22) + (n << 12), (p << 22)+(n <<12) + +PAGE_SIZE-1

             );
           for (i=0 ;i<200 ;i++ ) HYPERVISOR_sched_op(SCHEDOP_yield,0);
        }      
    }
    else if(boot_page_directory[p].pde4m.present)
    {
      xenprintf("4mb page present...that is an unxepected sitution, \n");
      xenprintf("xen sweb is not aware of 4mb pages at the moment\n");
      for (i=0 ;i<500 ;i++ ) HYPERVISOR_sched_op(SCHEDOP_yield,0);
    }
  }
  
}
