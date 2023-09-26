//
// tables_test.cc 
//
// Created by Zacharyliu-CS on 07/13/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include <gtest/gtest.h>
#include "tables.h"

namespace TPCC{
DEFINE_bool(DEBUG, true, "Set if output log message.");
DEFINE_int32(NUM_WAREHOUSE, 10, "Set the num of warehouse.");
DEFINE_int32(FREQUENCY_NEW_ORDER, 45, "Default percentage of new-order txn.");
DEFINE_int32(FREQUENCY_PAYMENT, 43, "Default percentage of payment txn.");
DEFINE_int32(FREQUENCY_ORDER_STATUS, 4,
             "Default percentage of order-status txn.");
DEFINE_int32(FREQUENCY_DELIVERY, 4, "Default percentage of delivery txn.");
DEFINE_int32(FREQUENCY_STOCK_LEVEL, 4,
             "Default percentage of stock-level txn.");
}


class TPCC_TABLE: public testing::Test {
  public:
  uint64_t GenerateAllTables(){
    tpcc_table.LoadTables();
    return tpcc_table.GetLoadRecordCount();
  }
  private:
  TPCC::TPCCTable tpcc_table;

};

TEST_F(TPCC_TABLE, TABLE_DEFINITION_SIZE_TEST){
  EXPECT_EQ(sizeof(TPCC::tpcc_warehouse_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_district_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_customer_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_history_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_new_order_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_order_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_order_line_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_item_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::tpcc_stock_key_t), 8);
}

TEST_F(TPCC_TABLE, TABLE_CREATION){
  EXPECT_EQ(GenerateAllTables(), 6689729);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
