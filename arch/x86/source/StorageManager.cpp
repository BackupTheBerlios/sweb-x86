/**
 * @file StorageManager.cpp
 *
 */

#include "StorageManager.h"


StorageManager::StorageManager(uint16 num_inodes, uint16 num_zones)
{
  inode_bitmap_ = new Bitmap(num_inodes);
  zone_bitmap_ = new Bitmap(num_zones);
}

StorageManager::~StorageManager()
{
  delete inode_bitmap_;
  delete zone_bitmap_;
  inode_bitmap_ = 0;
  zone_bitmap_ = 0;
}

