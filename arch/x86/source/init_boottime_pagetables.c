/**
 * $Id: init_boottime_pagetables.c,v 1.6 2005/04/20 20:42:56 nomenquis Exp $
 *
 * $Log: init_boottime_pagetables.c,v $
 * Revision 1.5  2005/04/20 15:26:35  nomenquis
 * more and more stuff actually works
 *
 * Revision 1.4  2005/04/20 09:00:29  nomenquis
 * removed lots of trash symbols, and added "nice" symbol names
 * also added docs about these symbols into the wiki
 *
 * Revision 1.3  2005/04/20 07:09:59  nomenquis
 * added inital paging for the kernel, plus mapping to two gigs
 * hoever, the kernel now is at 1meg phys and 2gig + 1 meg virtual due to these 4m pages
 *
 * Revision 1.2  2005/04/12 18:42:50  nomenquis
 * changed a zillion of iles
 *
 * Revision 1.1  2005/04/12 17:46:44  nomenquis
 * added lots of files
 *
 *
 */

#include "types.h"
#include "boot-time.h"
#include "paging-definitions.h"
#include "offsets.h"

extern  page_directory_entry kernel_page_directory_start[];
extern  page_table_entry kernel_page_tables_start[];

void initialiseBootTimePaging()
{
  uint32 i,k;
 
  page_directory_entry *pde_start = (page_directory_entry*)VIRTUAL_TO_PHYSICAL_BOOT((pointer)&kernel_page_directory_start);
  uint8 *pde_start_bytes = (uint8 *)pde_start;
  page_table_entry *pte_start = (page_table_entry*)VIRTUAL_TO_PHYSICAL_BOOT((pointer)&kernel_page_tables_start);

  // we do not have to clear the pde since its in the bss  
  for (i=0;i<5;++i)
  {
    pde_start[i].pde4m.present = 1;
    pde_start[i].pde4m.writeable = 1;
    pde_start[i].pde4m.use_4_m_pages = 1;
    pde_start[i].pde4m.page_base_address = i;
  }

  for (i=0;i<4;++i)
  {
    pde_start[i+512].pde4k.present = 1;
    pde_start[i+512].pde4k.writeable = 1;
    pde_start[i+512].pde4k.page_table_base_address = ((pointer)&pte_start[1024*i])/PAGE_SIZE;
  }
 
  // ok, we currently only fill in mappings for the first 4 megs (aka one page table)
  // we do not have to zero out the other page tables since they're alreay empty
  // thanks to the bss clearance.
  for (i=0;i<1024;++i)
  {
    pte_start[i].present = 1;
    pte_start[i].writeable = 1;
    pte_start[i].page_base_address = i;
  }
  
  for (i=0;i<256;++i)
  {
    pde_start[i+768].pde4m.present = 1;
    pde_start[i+768].pde4m.writeable = 1;
    pde_start[i+768].pde4m.use_4_m_pages = 1;
    pde_start[i+768].pde4m.page_base_address = i;
  }
 
}

void removeBootTimeIdentMapping()
{
  uint32 i;

  for (i=0;i<5;++i)
  {
    kernel_page_directory_start[i].pde4m.present=0;
  }
}
