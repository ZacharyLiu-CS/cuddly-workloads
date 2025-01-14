//
// memorydb_impl.h
//
// Created by Zacharyliu-CS on 09/28/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "kv_interface.h"

class MemoryDBImpl : public KVInterface {

 public:
  int Put(uint64_t key, const std::string& value) override;
  int Get(uint64_t key, std::string& value) override;
  virtual ~MemoryDBImpl(){}
 private:
  std::unordered_map<uint64_t, std::string> memory_db;
};
