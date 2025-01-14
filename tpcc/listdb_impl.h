//
// listdb_impl.h
//
// Created by Zacharyliu-CS on 02/17/2024.
// Copyright (c) 2024 liuzhenm@mail.ustc.edu.cn.
//
#pragma once

#include "kv_interface.h"

#include <string>
#include "listdb/db_client.h"
#include "listdb/listdb.h"

class ListDBImpl : public KVInterface {
 public:
  ListDBImpl(std::string dbpath);
  int Put(uint64_t key, const std::string& value) override;
  int Get(uint64_t key, std::string& value) override;
  virtual ~ListDBImpl() {}

 private:
  inline void convert_valueptr_to_value(std::string & value, uint64_t& value_ptr) {
    size_t value_len = *((size_t*)value_ptr);
    value_ptr += sizeof(size_t);
    value.resize(value_len, 0);
    memcpy(value.data(), (char*)value_ptr, value_len);
  }
  ListDB* db_ = nullptr;
  DBClient* client_ = nullptr;
};
