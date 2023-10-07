//
// kv_interface.h
//
// Created by Zacharyliu-CS on 09/27/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include <cstdint>

class KVInterface {

 public:
  virtual int Put(uint64_t key, uint8_t* value, uint32_t size) = 0;
  virtual int Get(uint64_t key, uint8_t* value) = 0;
  virtual ~KVInterface(){}
 private:
};
