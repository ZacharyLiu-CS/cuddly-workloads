//
// tpcc.h
//
// Created by Zacharyliu-CS on 07/12/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include <gflags/gflags.h>
#include <gflags/gflags_declare.h>

DECLARE_int32(num_warehouse);
DECLARE_int32(percent_new_order);
DECLARE_int32(percent_payment);
DECLARE_int32(percent_order_status);
DECLARE_int32(percent_delivery);
DECLARE_int32(percent_stock_level);

enum TPCCTxnType : uint32_t {
  tNewOrder = 0,
  tPayment,
  tDelivery,
  tOrderStatus,
  tStockLevel,
};

#define TPCC_TXN_NUM 5
static const std::string TPCC_TXN_NAME[TPCC_TXN_NUM] = {
    "NewOrder", "Payment", "Delivery", "OrderStatus", "StockLevel"};

enum TPCCTableType : uint32_t {
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
};