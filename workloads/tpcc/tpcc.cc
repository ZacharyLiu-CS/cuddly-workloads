//
// tpcc.cc 
//
// Created by Zacharyliu-CS on 07/12/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include "config.h"
#include "logging.h"


DEFINE_bool(debug, false, "Set if output log message.");
DEFINE_int32(num_warehouse, 10 , "Set the num of warehouse.");
DEFINE_int32(percent_new_order, 45, "Default percentage of new-order txn.");
DEFINE_int32(percent_payment,43, "Default percentage of payment txn." );
DEFINE_int32(percent_order_status, 4,"Default percentage of order-status txn.");
DEFINE_int32(percent_delivery, 4, "Default percentage of delivery txn.");
DEFINE_int32(percent_stock_level, 4 ,"Default percentage of stock-level txn.");


int main(int argc, char ** argv)
{
  gflags::SetUsageMessage("Usage message of pmem operation test:");
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  LOG("Configuration output:");
  LOG("debug: ", FLAGS_debug);
  LOG("num_warehouse: ", FLAGS_num_warehouse);
  LOG("percent_new_order: ", FLAGS_percent_new_order);
  LOG("percent_payment: ", FLAGS_percent_payment);
  LOG("percent_order_status: ", FLAGS_percent_order_status);
  LOG("percent_delivery: ", FLAGS_percent_delivery);
  LOG("percent_stock_level: ", FLAGS_percent_stock_level);
  return 0;
}
