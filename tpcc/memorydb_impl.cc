//
// memorydb_impl.cc
//
// Created by Zacharyliu-CS on 09/28/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#include "memorydb_impl.h"
#include <string>

int MemoryDBImpl::Put(uint64_t key, const std::string& value) {
  memory_db.insert_or_assign(key, value);
  return 1;
}
int MemoryDBImpl::Get(uint64_t key, std::string& value) {
  auto res = memory_db.find(key);
  if (res == memory_db.end())
    return -1;
  value = res->second.data();
  return 1;
}
