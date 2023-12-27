//
// Created by rjj on 12/5/23.
// 封装底层的缓冲区

#ifndef DAY01_BUFFER_H
#define DAY01_BUFFER_H
#include <iostream>
#include <string>
#include "common.h"

class Buffer {
 private:
  std::string buf_;

 public:
  DISALLOW_COPY_AND_MOVE(Buffer);
  Buffer() = default;
  ~Buffer() = default;

  void Append(const char *_str, int _size);
  void SetBuf(const char *c_str);
  size_t Size() const;
  const std::string &GetBuf() const;
  const char *ConstStr() const;
  void Clear();
};

#endif  // DAY01_BUFFER_H
