//
// tpcc.h
//
// Created by Zacharyliu-CS on 07/12/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include <gflags/gflags.h>
#include <gflags/gflags_declare.h>
#include "logging.h"

namespace TPCC {
//**************** TPCC table definitions (Schemas of key and value) end **************** //
DECLARE_bool(DEBUG);

DECLARE_int32(NUM_WAREHOUSE);
DECLARE_int32(FREQUENCY_NEW_ORDER);
DECLARE_int32(FREQUENCY_PAYMENT);
DECLARE_int32(FREQUENCY_ORDER_STATUS);
DECLARE_int32(FREQUENCY_DELIVERY);
DECLARE_int32(FREQUENCY_STOCK_LEVEL);
DECLARE_string(DB_PATH);

#define NUM_DISTRICT_PER_WAREHOUSE 10
#define NUM_CUSTOMER_PER_DISTRICT 3000
#define NUM_ITEM 100000
#define NUM_STOCK_PER_WAREHOUSE 100000

using itemkey_t = uint64_t;
using DataItem = void;

#define TPCC_TABLE_TYPES 12
#define TPCC_TX_TYPES 5

enum class TPCCTxType {
  kNewOrder = 0,
  kPayment,
  kDelivery,
  kOrderStatus,
  kStockLevel,
  kBottom = TPCC_TX_TYPES
};

// Table id
enum class TPCCTableType : uint8_t {
  kWarehouseTable = 0,
  kDistrictTable,
  kCustomerTable,
  kHistoryTable,
  kNewOrderTable,
  kOrderTable,
  kOrderLineTable,
  kItemTable,
  kStockTable,
  kCustomerIndexTable,
  kOrderIndexTable,
  kBottom = TPCC_TABLE_TYPES
};

// Magic numbers for debugging. These are unused in the spec.
const std::string tpcc_zip_magic("123456789");  // warehouse, district
const uint32_t tpcc_no_time_magic = 0;          // customer, history, order
const int64_t tpcc_add_magic =
    818;  // customer_index, order_index, new_order, order_line, item, stock

inline void TestConfig() {
  LOG("Test Configuration output:");
  LOG("DEBUG: ", FLAGS_DEBUG);
  LOG("NUM_WAREHOUSE: ", FLAGS_NUM_WAREHOUSE);
  LOG("FREQUENCY_NEW_ORDER: ", FLAGS_FREQUENCY_NEW_ORDER);
  LOG("FREQUENCY_PAYMENT: ", FLAGS_FREQUENCY_PAYMENT);
  LOG("FREQUENCY_ORDER_STATUS: ", FLAGS_FREQUENCY_ORDER_STATUS);
  LOG("FREQUENCY_DELIVERY: ", FLAGS_FREQUENCY_DELIVERY);
  LOG("FREQUENCY_STOCK_LEVEL: ", FLAGS_FREQUENCY_STOCK_LEVEL);
  LOG("DB_PATH: ", FLAGS_DB_PATH);
}

}  // end of namespace TPCC