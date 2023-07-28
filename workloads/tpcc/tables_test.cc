//
// tables_test.cc 
//
// Created by Zacharyliu-CS on 07/13/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include <gtest/gtest.h>
#include "tables.h"

TEST(TPCC_TABLE, TABLE_DEFINITION_SIZE_TEST){
  EXPECT_EQ(sizeof(TPCC::Warehouse::Key), 8);
  EXPECT_EQ(sizeof(TPCC::District::Key), 8);
  EXPECT_EQ(sizeof(TPCC::Customer::Key), 8);
  EXPECT_EQ(sizeof(TPCC::History::Key), 8);
  EXPECT_EQ(sizeof(TPCC::NewOrder::Key), 8);
  EXPECT_EQ(sizeof(TPCC::Order::Key), 8);
  EXPECT_EQ(sizeof(TPCC::OrderLine::Key), 8);
  EXPECT_EQ(sizeof(TPCC::Item::Key), 8);
  EXPECT_EQ(sizeof(TPCC::Stock::Key), 8);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
