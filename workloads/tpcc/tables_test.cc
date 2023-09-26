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
};

class TPCC_TABLE: public testing::Test {
  public:

  private:


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

}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
