//
// txn_orderstatus.cc
//
// Created by Zacharyliu-CS on 09/26/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include <memory>
#include <set>
#include "tpcc_txn.h"

namespace TPCC {

bool TPCCTxn::OrderStatus(TPCCTable* tpcc_client,
                          FastRandom& random_generator) {

  int y = tpcc_client->RandomNumber(random_generator, 1, 100);

  int warehouse_id_start_ = 1;
  int warehouse_id_end_ = tpcc_client->GetNumWarehouse();

  int district_id_start = 1;
  int district_id_end_ = tpcc_client->GetNumDistrictPerWareHouse();

  const uint32_t warehouse_id = tpcc_client->PickWarehouseId(
      random_generator, warehouse_id_start_, warehouse_id_end_);
  const uint32_t district_id = tpcc_client->RandomNumber(
      random_generator, district_id_start, district_id_end_);
  uint32_t customer_id = 0;

  if (y <= 60) {
    // FIXME:: Find customer by the last name
    customer_id = tpcc_client->GetCustomerId(random_generator);
  } else {
    customer_id = tpcc_client->GetCustomerId(random_generator);
  }

  tpcc_customer_key_t cust_key;
  tpcc_customer_val_t cust_val;
  cust_key.c_id =
      tpcc_client->MakeCustomerKey(warehouse_id, district_id, customer_id);
  tpcc_client->GetRecord(cust_key.item_key, &cust_val);
  //   auto cust_obj = std::make_shared<DataItem>((table_id_t)TPCCTableType::kCustomerTable, cust_key.item_key);
  //   dtx->AddToReadOnlySet(cust_obj);

  // FIXME: Currently, we use a random order_id to maintain the distributed transaction payload,
  // but need to search the largest o_id by o_w_id, o_d_id and o_c_id from the order table
  int32_t order_id = tpcc_client->RandomNumber(
      random_generator, 1, tpcc_client->GetNumCustomerPerDistrict());
  uint64_t o_key =
      tpcc_client->MakeOrderKey(warehouse_id, district_id, order_id);
  tpcc_order_key_t order_key;
  tpcc_order_val_t order_val;
  order_key.o_id = o_key;
  tpcc_client->GetRecord(order_key.item_key, &order_val);

  // o_entry_d never be 0

  for (int i = 1; i <= order_val.o_ol_cnt; i++) {
    int64_t ol_key =
        tpcc_client->MakeOrderLineKey(warehouse_id, district_id, order_id, i);
    tpcc_order_line_key_t order_line_key;
    tpcc_order_line_val_t order_line_val;
    order_line_key.ol_id = ol_key;
    tpcc_client->GetRecord(order_line_key.item_key, &order_line_val);
  }

  return true;
}

}  // end of namespace TPCC
