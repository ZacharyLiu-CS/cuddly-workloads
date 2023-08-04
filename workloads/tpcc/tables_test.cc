//
// tables_test.cc 
//
// Created by Zacharyliu-CS on 07/13/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include <gtest/gtest.h>
#include "tables.h"
class TPCC_TABLE: public testing::Test {
  public:


  private:


};

TEST_F(TPCC_TABLE, TABLE_DEFINITION_SIZE_TEST){
  EXPECT_EQ(sizeof(TPCC::warehouse_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::district_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::customer_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::history_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::new_order_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::order_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::order_line_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::item_key_t), 8);
  EXPECT_EQ(sizeof(TPCC::stock_key_t), 8);
}

TEST_F(TPCC_TABLE, TABLE_CREATION){

}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
