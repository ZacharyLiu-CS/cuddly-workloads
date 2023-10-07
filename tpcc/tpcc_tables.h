//
// tpcc_tables.h
//
// Created by Zacharyliu-CS on 07/13/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once

#include <array>
#include <atomic>
#include "kv_interface.h"
#include "schemas.h"

namespace TPCC {
class TPCCTable {
 private:
  // Pre-defined constants, which will be modified for tests
  uint32_t num_warehouse_ = 0;
  uint32_t num_district_per_warehouse_ = 0;
  uint32_t num_customer_per_district_ = 0;
  uint32_t num_item_ = 0;
  uint32_t num_stock_per_warehouse_ = 0;

  KVInterface* kv_impl = nullptr;

  std::atomic_uint64_t write_record_count_ = 0;
  std::atomic_uint64_t read_record_count_ = 0;

 public:
  TPCCTable();
  virtual ~TPCCTable(){
    delete kv_impl;
  }
  uint32_t GetNumWarehouse() { return num_warehouse_; }
  uint32_t GetNumDistrictPerWareHouse() { return num_district_per_warehouse_; }
  uint32_t GetNumCustomerPerDistrict() { return num_customer_per_district_; }
  uint32_t GetNumItem() { return num_item_; }
  uint32_t GetNumStockPerWarehouse() { return num_stock_per_warehouse_; }

  // For server-side usage
  uint64_t GetLoadRecordCount() { return write_record_count_.load(); }
  uint64_t GetReadRecordCount() { return read_record_count_.load(); }

  void LoadTables();

  std::vector<TPCCTxType> CreateWorkgenArray();

  void PopulateWarehouseTable(unsigned long seed);

  void PopulateDistrictTable(unsigned long seed);

  void PopulateCustomerAndHistoryTable(unsigned long seed);

  void PopulateOrderNewOrderAndOrderLineTable(unsigned long seed);

  void PopulateItemTable(unsigned long seed);

  void PopulateStockTable(unsigned long seed);

  // -1 means fail, else means success
  template <typename T>
  int PutRecord(itemkey_t item_key, T* val_ptr) {
    write_record_count_ += 1;
    return kv_impl->Put(item_key, (uint8_t*)val_ptr, sizeof(T));
  }

  // -1 means fail, else means success
  template <typename T>
  int GetRecord(itemkey_t item_key, T* val_ptr) {
    read_record_count_ += 1;
    return kv_impl->Get(item_key, (uint8_t*)val_ptr);
  }

 public:
  /* Followng pieces of codes mainly comes from Silo */
  inline uint32_t GetCurrentTimeMillis() {
    // XXX(stephentu): implement a scalable GetCurrentTimeMillis()
    // for now, we just give each core an increasing number
    static __thread uint32_t tl_hack = 0;
    return ++tl_hack;
  }

  // utils for generating random #s and strings
  inline int CheckBetweenInclusive(int v, int lower, int upper) {
    assert(v >= lower);
    assert(v <= upper);
    return v;
  }

  inline int RandomNumber(FastRandom& r, int min, int max) {
    return CheckBetweenInclusive((int)(r.NextUniform() * (max - min + 1) + min),
                                 min, max);
  }

  inline int NonUniformRandom(FastRandom& r, int A, int C, int min, int max) {
    return (((RandomNumber(r, 0, A) | RandomNumber(r, min, max)) + C) %
            (max - min + 1)) +
           min;
  }

  inline int64_t GetItemId(FastRandom& r) {
    return CheckBetweenInclusive(
        g_uniform_item_dist ? RandomNumber(r, 1, num_item_)
                            : NonUniformRandom(r, 8191, 7911, 1, num_item_),
        1, num_item_);
  }

  inline int GetCustomerId(FastRandom& r) {
    return CheckBetweenInclusive(
        NonUniformRandom(r, 1023, 259, 1, num_customer_per_district_), 1,
        num_customer_per_district_);
  }

  // pick a number between [start, end)
  inline unsigned PickWarehouseId(FastRandom& r, unsigned start, unsigned end) {
    assert(start < end);
    const unsigned diff = end - start;
    if (diff == 1)
      return start;
    return (r.Next() % diff) + start;
  }

  inline size_t GetCustomerLastName(uint8_t* buf, FastRandom& r, int num) {
    const std::string& s0 = NameTokens[num / 100];
    const std::string& s1 = NameTokens[(num / 10) % 10];
    const std::string& s2 = NameTokens[num % 10];
    uint8_t* const begin = buf;
    const size_t s0_sz = s0.size();
    const size_t s1_sz = s1.size();
    const size_t s2_sz = s2.size();
    memcpy(buf, s0.data(), s0_sz);
    buf += s0_sz;
    memcpy(buf, s1.data(), s1_sz);
    buf += s1_sz;
    memcpy(buf, s2.data(), s2_sz);
    buf += s2_sz;
    return buf - begin;
  }

  inline size_t GetCustomerLastName(char* buf, FastRandom& r, int num) {
    return GetCustomerLastName((uint8_t*)buf, r, num);
  }

  inline std::string GetCustomerLastName(FastRandom& r, int num) {
    std::string ret;
    ret.resize(CustomerLastNameMaxSize);
    ret.resize(GetCustomerLastName((uint8_t*)&ret[0], r, num));
    return ret;
  }

  inline std::string GetNonUniformCustomerLastNameLoad(FastRandom& r) {
    return GetCustomerLastName(r, NonUniformRandom(r, 255, 157, 0, 999));
  }

  inline size_t GetNonUniformCustomerLastNameRun(uint8_t* buf, FastRandom& r) {
    return GetCustomerLastName(buf, r, NonUniformRandom(r, 255, 223, 0, 999));
  }

  inline size_t GetNonUniformCustomerLastNameRun(char* buf, FastRandom& r) {
    return GetNonUniformCustomerLastNameRun((uint8_t*)buf, r);
  }

  inline std::string GetNonUniformCustomerLastNameRun(FastRandom& r) {
    return GetCustomerLastName(r, NonUniformRandom(r, 255, 223, 0, 999));
  }

  inline std::string RandomStr(FastRandom& r, uint64_t len) {
    // this is a property of the oltpbench implementation...
    if (!len)
      return "";

    uint64_t i = 0;
    std::string buf(len, 0);
    while (i < (len)) {
      const char c = (char)r.NextChar();
      // XXX(stephentu): oltpbench uses java's Character.isLetter(), which
      // is a less restrictive filter than isalnum()
      if (!isalnum(c))
        continue;
      buf[i++] = c;
    }
    return buf;
  }

  // RandomNStr() actually produces a string of length len
  inline std::string RandomNStr(FastRandom& r, uint64_t len) {
    const char base = '0';
    std::string buf(len, 0);
    for (uint64_t i = 0; i < len; i++)
      buf[i] = (char)(base + (r.Next() % 10));
    return buf;
  }

  inline int64_t MakeDistrictKey(int32_t w_id, int32_t d_id) {
    int32_t did = d_id + (w_id * num_district_per_warehouse_);
    int64_t id = static_cast<int64_t>(did);
    // assert(districtKeyToWare(id) == w_id);
    return id;
  }

  inline int64_t MakeCustomerKey(int32_t w_id, int32_t d_id, int32_t c_id) {
    int32_t upper_id = w_id * num_district_per_warehouse_ + d_id;
    int64_t id =
        static_cast<int64_t>(upper_id) << 32 | static_cast<int64_t>(c_id);
    // assert(customerKeyToWare(id) == w_id);
    return id;
  }

  // only used for customer index, maybe some problems when used.
  inline void ConvertString(char* newstring, const char* oldstring, int size) {
    for (int i = 0; i < 8; i++)
      if (i < size)
        newstring[7 - i] = oldstring[i];
      else
        newstring[7 - i] = '\0';

    for (int i = 8; i < 16; i++)
      if (i < size)
        newstring[23 - i] = oldstring[i];
      else
        newstring[23 - i] = '\0';
  }

  inline uint64_t MakeCustomerIndexKey(int32_t w_id, int32_t d_id,
                                       std::string s_last,
                                       std::string s_first) {
    uint64_t* seckey = new uint64_t[5];
    int32_t did = d_id + (w_id * num_district_per_warehouse_);
    seckey[0] = did;
    ConvertString((char*)(&seckey[1]), s_last.data(), s_last.size());
    ConvertString((char*)(&seckey[3]), s_first.data(), s_first.size());
    return (uint64_t)seckey;
  }

  inline int64_t MakeHistoryKey(int32_t h_w_id, int32_t h_d_id,
                                int32_t h_c_w_id, int32_t h_c_d_id,
                                int32_t h_c_id) {
    int32_t cid = (h_c_w_id * num_district_per_warehouse_ + h_c_d_id) *
                      num_customer_per_district_ +
                  h_c_id;
    int32_t did = h_d_id + (h_w_id * num_district_per_warehouse_);
    int64_t id = static_cast<int64_t>(cid) << 20 | static_cast<int64_t>(did);
    return id;
  }

  inline int64_t MakeNewOrderKey(int32_t w_id, int32_t d_id, int32_t o_id) {
    int32_t upper_id = w_id * num_district_per_warehouse_ + d_id;
    int64_t id =
        static_cast<int64_t>(upper_id) << 32 | static_cast<int64_t>(o_id);
    return id;
  }

  inline int64_t MakeOrderKey(int32_t w_id, int32_t d_id, int32_t o_id) {
    int32_t upper_id = w_id * num_district_per_warehouse_ + d_id;
    int64_t id =
        static_cast<int64_t>(upper_id) << 32 | static_cast<int64_t>(o_id);
    // assert(orderKeyToWare(id) == w_id);
    return id;
  }

  inline int64_t MakeOrderIndexKey(int32_t w_id, int32_t d_id, int32_t c_id,
                                   int32_t o_id) {
    int32_t upper_id = (w_id * num_district_per_warehouse_ + d_id) *
                           num_customer_per_district_ +
                       c_id;
    int64_t id =
        static_cast<int64_t>(upper_id) << 32 | static_cast<int64_t>(o_id);
    return id;
  }

  inline int64_t MakeOrderLineKey(int32_t w_id, int32_t d_id, int32_t o_id,
                                  int32_t number) {
    int32_t upper_id = w_id * num_district_per_warehouse_ + d_id;
    // 10000000 is the MAX ORDER ID
    int64_t oid =
        static_cast<int64_t>(upper_id) * 10000000 + static_cast<int64_t>(o_id);
    int64_t olid = oid * 15 + number;
    int64_t id = static_cast<int64_t>(olid);
    // assert(orderLineKeyToWare(id) == w_id);
    return id;
  }

  inline int64_t MakeStockKey(int32_t w_id, int32_t i_id) {
    int32_t item_id = i_id + (w_id * num_stock_per_warehouse_);
    int64_t s_id = static_cast<int64_t>(item_id);
    // assert(stockKeyToWare(id) == w_id);
    return s_id;
  }
};
}  // end of namespace TPCC
