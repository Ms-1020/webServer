//
// Created by rjj on 12/5/23.
//

#include "include/Buffer.h"

void Buffer::Append(const char *_str, int _size) {
  for (int i = 0; i < _size; ++i) {
    if (_str[i] == '\0') {
      break;
    }
    buf_.push_back(_str[i]);
  }
}

void Buffer::SetBuf(const char *c_str) {
  std::string new_buf(c_str);
  buf_.swap(new_buf);
}

const std::string &Buffer::GetBuf() const { return buf_; }

size_t Buffer::Size() const { return buf_.size(); }

const char *Buffer::ConstStr() const { return buf_.c_str(); }

void Buffer::Clear() { buf_.clear(); }
