//
// tpcc.cc
//
// Created by Zacharyliu-CS on 07/12/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include "tpcc/config.h"

namespace TPCC {

DEFINE_bool(DEBUG, true, "Set if output log message.");
DEFINE_int32(NUM_WAREHOUSE, 10, "Set the num of warehouse.");
DEFINE_int32(FREQUENCY_NEW_ORDER, 45, "Default percentage of new-order txn.");
DEFINE_int32(FREQUENCY_PAYMENT, 43, "Default percentage of payment txn.");
DEFINE_int32(FREQUENCY_ORDER_STATUS, 4,
             "Default percentage of order-status txn.");
DEFINE_int32(FREQUENCY_DELIVERY, 4, "Default percentage of delivery txn.");
DEFINE_int32(FREQUENCY_STOCK_LEVEL, 4,
             "Default percentage of stock-level txn.");
} // namespace TPCC

int main(int argc, char **argv) {
  google::SetUsageMessage("Usage message of pmem operation test:");
  google::ParseCommandLineFlags(&argc, &argv, true);
  TPCC::TestConfig();

  return 0;
}
