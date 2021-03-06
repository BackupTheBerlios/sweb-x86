/**
 * @file arch_bd_ide_driver.h
 *
 */
 
#ifndef _IDE_BUS_DEVICE_DRIVER_
#define _IDE_BUS_DEVICE_DRIVER_

#include "arch_bd_ata_driver.h"
#include "arch_bd_io.h"

class IDEDriver : public bdio
{
public:

  /**
   * Constructor
   *
   */
  IDEDriver()
  {
    doDeviceDetection();
  };

  /**
   * Destructor
   *
   */
  ~IDEDriver()
  {
  };

  typedef struct fdisk_partition
  {
    uint8 bootid;   // bootable?  0=no, 128=yes
    uint8 beghead;
    uint8 begcyl;
    uint8 begsect;
    uint8 systid;   // Operating System type indicator code
    uint8 endhead;
    uint8 endcyl;
    uint8 endsect;
    uint32 relsect; // first sector relative to start of disk - We actually need only theese two params
    uint32 numsect; // number of sectors in partition
  } FP;

  typedef struct master_boot_record 
  {
    uint8 bootinst[446];           // GRUB space
    uint8 parts[ 4*sizeof(FP) ];
    uint16 signature;              // set to 0xAA55 for PC MBR
  } MBR;


  /**
   * Master Boot Record process
   *
   */
  int32 processMBR  ( ATADriver *, uint32, uint32, char* );

  /**
   * detects devices
   *
   */
  uint32 doDeviceDetection ( );

};

#endif
