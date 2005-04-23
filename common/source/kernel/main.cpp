/**
 * $Id: main.cpp,v 1.11 2005/04/23 15:58:32 nomenquis Exp $
 *
 * $Log: main.cpp,v $
 * Revision 1.10  2005/04/23 12:52:26  nomenquis
 * fixes
 *
 * Revision 1.9  2005/04/23 12:43:09  nomenquis
 * working page manager
 *
 * Revision 1.8  2005/04/22 20:14:25  nomenquis
 * fix for crappy old gcc versions
 *
 * Revision 1.7  2005/04/22 19:43:04  nomenquis
 *  more poison added
 *
 * Revision 1.6  2005/04/22 18:23:16  nomenquis
 * massive cleanups
 *
 * Revision 1.5  2005/04/22 17:21:41  nomenquis
 * added TONS of stuff, changed ZILLIONS of things
 *
 * Revision 1.4  2005/04/21 21:31:24  nomenquis
 * added lfb support, also we now use a different grub version
 * we also now read in the grub multiboot version
 *
 * Revision 1.3  2005/04/20 08:06:18  nomenquis
 * the overloard (thats me) managed to get paging with 4m pages to work.
 * kernel is now at 2g +1 and writes something to the fb
 * w00t!
 *
 * Revision 1.2  2005/04/20 06:39:11  nomenquis
 * merged makefile, also removed install from default target since it does not work
 *
 * Revision 1.1  2005/04/12 18:42:51  nomenquis
 * changed a zillion of iles
 *
 * Revision 1.1  2005/04/12 17:46:44  nomenquis
 * added lots of files
 *
 *
 */
 
 
#include "types.h"
#include "multiboot.h"
#include "arch_panic.h"
#include "paging-definitions.h"
#include "console/ConsoleManager.h"
#include "mm/PageManager.h"

extern void* kernel_end_address;

extern "C"
{
	void startup()
	{
    /*
    pointer next_usable_address = (pointer)&kernel_end_address;
    next_usable_address = PageManager::createPageManager(next_usable_address);
    */
    ConsoleManager *manager = new ConsoleManager(1);
    
    Console *console = manager->getActiveConsole();
    /*uint32 i,k;
    

    for (i=0;i<16;++i)
    {
      for (k=0;k<16;++k)
      {
        console->setBackgroundColor((Console::BACKGROUNDCOLORS)i);
        console->setForegroundColor((Console::FOREGROUNDCOLORS)k);
        console->setCharacter(i,k,'A');
      }
    }
    */
    console->setBackgroundColor(Console::BG_BLACK);
    console->setForegroundColor(Console::FG_GREEN);

    uint32 i;
  
    console->writeString((uint8 const*)"Blabb\n");  

    console->writeString((uint8 const*)"Blubb sagte die Katze und frasz den Hund\n");
    console->writeString((uint8 const*)"Noch ne Zeile\n");
    console->writeString((uint8 const*)"Und jetzt ne leere\n\n");
    console->writeString((uint8 const*)"Gruen rackete autobus\n");


    for (;;);
	}
	  
}
