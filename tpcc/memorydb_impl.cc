//
// memorydb_impl.cc
//
// Created by Zacharyliu-CS on 09/28/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#include "memorydb_impl.h"
#include <cstring>

int MemoryDBImpl::Put(uint64_t key, uint8_t* value, uint32_t size) {
  memory_db.insert_or_assign(key, std::string((char*)value, size));
  return 1;
}
int MemoryDBImpl::Get(uint64_t key, uint8_t* value) {
  auto res = memory_db.find(key);
  if (res == memory_db.end())
    return -1;
  memcpy((char*)value, res->second.data(), res->second.size());
  return 1;
}
