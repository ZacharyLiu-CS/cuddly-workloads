//
// listdb_impl.cc
//
// Created by Zacharyliu-CS on 02/17/2024.
// Copyright (c) 2024 liuzhenm@mail.ustc.edu.cn.
//

#include "listdb_impl.h"
#include <sys/stat.h>
#include <string>

ListDBImpl::ListDBImpl(std::string dbpath) {
  auto file_exists = [](char const* file) {
    return access(file, F_OK);
  };
  if (file_exists(dbpath.c_str())) {
    mkdir(dbpath.c_str(), 0700);
  }
  db_ = new ListDB();
  db_->Init(dbpath, 16 * uint64_t(1024 * 1024 * 1024));
  client_ = new DBClient(db_, 0, 0);
}

int ListDBImpl::Put(uint64_t key, const std::string&value) {
  client_->PutStringKV(std::to_string(key), value);
  return 1;
}
int ListDBImpl::Get(uint64_t key, std::string& value) {
  uint64_t value_ptr;
  bool res = client_->GetStringKV(std::to_string(key), &value_ptr);
  if( res == false){
    return -1;
  }
  convert_valueptr_to_value(value, value_ptr);
  return 1;
}