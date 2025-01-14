//
// kv_interface.h
//
// Created by Zacharyliu-CS on 09/27/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include <cstdint>
#include <string>

class KVInterface {

 public:
  virtual int Put(uint64_t key, const std::string& value) = 0;
  virtual int Get(uint64_t key, std::string& value) = 0;
  virtual ~KVInterface(){}
 private:
};
