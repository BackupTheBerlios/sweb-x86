#include "Buffer.h"
#include "assert.h"
#include "../../include/console/kprintf.h"

Buffer::Buffer(size_t size)
{
  size_ = size;
  buffer_ = new char[size_*sizeof(char)];
}

Buffer::Buffer(const Buffer &src)
{
  size_ = src.size_;
  buffer_ = new char[size_*sizeof(char)];
  for (size_t index = 0; index < size_; index++)
  {
    buffer_[index] = src.buffer_[index];
  }
}

Buffer::~Buffer()
{
  delete[] buffer_;
}
  
uint8 Buffer::getByte(size_t index)
{
  return (uint8)buffer_[index];
}

uint16 Buffer::get2Bytes(size_t index)
{
  uint16 dst = 0;
  dst |= buffer_[index + 1];
  dst = dst << 8;
  dst |= (buffer_[index] & 0xFF);
  return dst;
}

uint32 Buffer::get4Bytes(size_t index)
{
  uint32 dst = 0;
  dst |= get2Bytes(index + 2);
  dst = dst << 16;
  dst |= (get2Bytes(index) & 0xFFFF);
  return dst;
}

uint64 Buffer::get8Bytes(size_t index)
{
  uint64 dst = 0;
  dst |= get4Bytes(index + 4);
  dst = dst << 16;
  dst |= (get4Bytes(index) & 0xFFFFFFFF);
  return dst;
}

void Buffer::setByte(size_t index, uint8 byte)
{
  assert(index < size_);
  buffer_[index] = (char)byte;
}

void Buffer::set2Bytes(size_t index, uint16 byte)
{
  assert(index+1 < size_);
  char first_byte = byte >> 8;
  char second_byte = byte && 0xFF;
  buffer_[index] = second_byte;
  buffer_[index+1] = first_byte;
}

void Buffer::set4Bytes(size_t index, uint32 byte)
{
  assert(index+3 < size_);
  uint16 first_2_bytes = byte >> 16;
  uint16 second_2_bytes = byte && 0xFFFF;
  set2Bytes(index, second_2_bytes);
  set2Bytes(index+2, first_2_bytes);
}

void Buffer::set8Bytes(size_t index, uint64 byte)
{
  assert(index+7 < size_);
  uint32 first_4_bytes = byte >> 32;
  uint32 second_4_bytes = byte && 0xFFFFFFFF;
  set2Bytes(index, second_4_bytes);
  set2Bytes(index+4, first_4_bytes);
}

uint32 Buffer::getSize()
{
  return size_;
}

char* Buffer::getBuffer()
{
  return buffer_;
}

void Buffer::print()
{
  kprintfd( "\n----Buffer:size:%d------\n",size_);
  for(uint32 i = 0; i<size_;i++)
  {
    kprintfd( "%x",buffer_[i]);
  }
  kprintfd( "\n----Buffer:end------\n");
}
    
void Buffer::clear()
{
  for(uint32 i = 0; i < size_; i++)
  {
    buffer_[i] = 0;
  }
}

