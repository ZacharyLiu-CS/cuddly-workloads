//
// rocksdb_impl.h
//
// Created by Zacharyliu-CS on 09/28/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include "kv_interface.h"
#include "rocksdb/db.h"
#include "rocksdb/options.h"


class RocksDBImpl : public KVInterface {
 public:
  RocksDBImpl(std::string dbpath);
  int Put(uint64_t key, const std::string& value) override;
  int Get(uint64_t key, std::string& value) override;
  virtual ~RocksDBImpl() {}

 private:
  rocksdb::DB* db_;
  rocksdb::Options options;
};
