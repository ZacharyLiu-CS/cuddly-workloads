//
// tpcc.h
//
// Created by Zacharyliu-CS on 07/12/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include "logging.h"
#include <gflags/gflags.h>
#include <gflags/gflags_declare.h>

namespace TPCC {
//**************** TPCC table definitions (Schemas of key and value) end **************** //
DECLARE_bool(DEBUG);

DECLARE_int32(NUM_WAREHOUSE);
DECLARE_int32(FREQUENCY_NEW_ORDER);
DECLARE_int32(FREQUENCY_PAYMENT);
DECLARE_int32(FREQUENCY_ORDER_STATUS);
DECLARE_int32(FREQUENCY_DELIVERY);
DECLARE_int32(FREQUENCY_STOCK_LEVEL);



#define NUM_DISTRICT_PER_WAREHOUSE 10
#define NUM_CUSTOMER_PER_DISTRICT 3000
#define NUM_ITEM 100000
#define NUM_STOCK_PER_WAREHOUSE 100000

using itemkey_t = uint64_t;
using DataItem = void;

#define TABLE_TPCC 0xBBC
#define TPCC_TX_TYPES 5

enum class TPCCTxType {
  kNewOrder = 0,
  kPayment,
  kDelivery,
  kOrderStatus,
  kStockLevel,
};

// Table id
enum class TPCCTableType : uint64_t {
  kWarehouseTable = TABLE_TPCC, // 48076
  kDistrictTable,
  kCustomerTable,
  kHistoryTable,
  kNewOrderTable, // 48080
  kOrderTable,
  kOrderLineTable,
  kItemTable,
  kStockTable, // 48084
  kCustomerIndexTable,
  kOrderIndexTable, // 48086
};

// Magic numbers for debugging. These are unused in the spec.
const std::string tpcc_zip_magic("123456789"); // warehouse, district
const uint32_t tpcc_no_time_magic = 0;         // customer, history, order
const int64_t tpcc_add_magic =
    818; // customer_index, order_index, new_order, order_line, item, stock

inline void TestConfig() {
  LOG("Test Configuration output:");
  LOG("DEBUG: ", FLAGS_DEBUG);
  LOG("NUM_WAREHOUSE: ", FLAGS_NUM_WAREHOUSE);
  LOG("FREQUENCY_NEW_ORDER: ", FLAGS_FREQUENCY_NEW_ORDER);
  LOG("FREQUENCY_PAYMENT: ", FLAGS_FREQUENCY_PAYMENT);
  LOG("FREQUENCY_ORDER_STATUS: ", FLAGS_FREQUENCY_ORDER_STATUS);
  LOG("FREQUENCY_DELIVERY: ", FLAGS_FREQUENCY_DELIVERY);
  LOG("FREQUENCY_STOCK_LEVEL: ", FLAGS_FREQUENCY_STOCK_LEVEL);
}

} // end of namespace TPCC