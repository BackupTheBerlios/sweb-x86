/**
 * @file Bitmap.cpp
 */

#include "mm/Bitmap.h"
#include "console/kprintf.h"
#include "assert.h"

Bitmap::Bitmap (size_t number_of_bits)
{
  size_ = number_of_bits;
  num_bits_set_ = 0;
  size_t byte_count = number_of_bits / bits_per_bitmap_atom_
               + ((number_of_bits % bits_per_bitmap_atom_ > 0)?1:0);
  bit_count_ = byte_count * bits_per_bitmap_atom_;
  bitmap_=new uint8[byte_count];
  for (size_t byte = 0; byte < byte_count; ++byte)
    *(bitmap_+byte) = static_cast<uint8>(0);
}

Bitmap::~Bitmap ()
{
  delete bitmap_;
}

void Bitmap::setBit(size_t bit_number)
{
  assert(bit_number < size_);
  size_t byte_number = bit_number / bits_per_bitmap_atom_;
  size_t bit_offset = bit_number % bits_per_bitmap_atom_;
  *(bitmap_+byte_number) |= (1 << bit_offset);
  ++num_bits_set_;
}
bool Bitmap::getBit(size_t bit_number)
{
  assert(bit_number < size_);
  size_t byte_number = bit_number / bits_per_bitmap_atom_;
  size_t bit_offset = bit_number % bits_per_bitmap_atom_;
  return (*(bitmap_+byte_number) & (1 << bit_offset));
}
void Bitmap::unsetBit(size_t bit_number)
{
  assert(bit_number < size_);
  size_t byte_number = bit_number / bits_per_bitmap_atom_;
  size_t bit_offset = bit_number % bits_per_bitmap_atom_;
  *(bitmap_+byte_number) &= ~(1 << bit_offset);
  --num_bits_set_;
}

void Bitmap::setByte(size_t byte_number, uint8 byte)
{
  assert(byte_number*8 < size_);
  bitmap_[byte_number] = byte;
  for(uint32 i = 0; i < 8; i++)
  {
    if((byte >> i) & 0x01)
      ++num_bits_set_;
  }
}

uint8 Bitmap::getByte(size_t byte_number)
{
  assert(byte_number*8 < size_);
  return bitmap_[byte_number];
}

void Bitmap::bmprint()
{
  kprintfd("\n-----Bitmap: size=%d, num_bits_set=%d-----\n",size_,num_bits_set_);
  for(uint32 i = 0; i < size_; i++)
  {
    kprintfd( "%d", getBit( i));
  }
  kprintfd("\n-----Bitmap:end------\n");
}
