//
// txn_payment.cc
//
// Created by Zacharyliu-CS on 09/26/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <unistd.h>
#include "schemas.h"
#include "tpcc_txn.h"
#include <iostream>
#include <set>
#include <cassert>
#include <memory>

namespace TPCC {

bool TPCCTxn::Payment(TPCCTable *tpcc_client, FastRandom &random_generator) {


  // Generate parameters

  int x = tpcc_client->RandomNumber(random_generator, 1, 100);
  int y = tpcc_client->RandomNumber(random_generator, 1, 100);

  int warehouse_id_start_ = 1;
  int warehouse_id_end_ = tpcc_client->GetNumWarehouse();

  int district_id_start = 1;
  int district_id_end_ = tpcc_client->GetNumDistrictPerWareHouse();

  const uint32_t warehouse_id = tpcc_client->PickWarehouseId(random_generator, warehouse_id_start_, warehouse_id_end_);
  const uint32_t district_id = tpcc_client->RandomNumber(random_generator, district_id_start, district_id_end_);

  int32_t c_w_id;
  int32_t c_d_id;
  if (tpcc_client->GetNumWarehouse() == 1 || x <= 85) {
    // 85%: paying through own warehouse (or there is only 1 warehouse)
    c_w_id = warehouse_id;
    c_d_id = district_id;
  } else {
    // 15%: paying through another warehouse:
    // select in range [1, num_warehouses] excluding w_id
    do {
      c_w_id = tpcc_client->RandomNumber(random_generator, warehouse_id_start_, warehouse_id_end_);
    } while (c_w_id == warehouse_id);
    c_d_id = tpcc_client->RandomNumber(random_generator, district_id_start, district_id_end_);
  }
  uint32_t customer_id = 0;
  // The payment amount (H_AMOUNT) is randomly selected within [1.00 .. 5,000.00].
  float h_amount = (float)tpcc_client->RandomNumber(random_generator, 100, 500000) / 100.0;
  if (y <= 60) {
    // 60%: payment by last name
    char last_name[tpcc_customer_val_t::MAX_LAST + 1];
    size_t size = (tpcc_client->GetNonUniformCustomerLastNameLoad(random_generator)).size();
    assert(tpcc_customer_val_t::MAX_LAST - size >= 0);
    strcpy(last_name, tpcc_client->GetNonUniformCustomerLastNameLoad(random_generator).c_str());
    // FIXME:: Find customer by the last name
    // All rows in the CUSTOMER table with matching C_W_ID, C_D_ID and C_LAST are selected sorted by C_FIRST in ascending order.
    // Let n be the number of rows selected.
    // C_ID, C_FIRST, C_MIDDLE, C_STREET_1, C_STREET_2, C_CITY, C_STATE, C_ZIP, C_PHONE, C_SINCE, C_CREDIT, C_CREDIT_LIM, C_DISCOUNT,
    // and C_BALANCE are retrieved from the row at position (n/ 2 rounded up to the next integer) in the sorted set of selected rows from the CUSTOMER table.
    customer_id = tpcc_client->GetCustomerId(random_generator);
  } else {
    // 40%: payment by id
    assert(y > 60);
    customer_id = tpcc_client->GetCustomerId(random_generator);
  }

  // Run

  tpcc_warehouse_key_t ware_key;
  tpcc_warehouse_val_t ware_val;
  ware_key.w_id = warehouse_id;
  tpcc_client->GetRecord(ware_key.item_key, &ware_val);

  ware_val.w_ytd += h_amount;
  tpcc_client->PutRecord(ware_key.item_key, &ware_val);

//   auto ware_obj = std::make_shared<DataItem>((table_id_t)TPCCTableType::kWarehouseTable, ware_key.item_key);
//   dtx->AddToReadWriteSet(ware_obj);

  uint64_t d_key = tpcc_client->MakeDistrictKey(warehouse_id, district_id);
  tpcc_district_key_t dist_key;
  tpcc_district_val_t dist_val;
  dist_key.d_id = d_key;
  tpcc_client->GetRecord(dist_key.item_key, &dist_val);

  dist_val.d_ytd += h_amount;
  tpcc_client->PutRecord(dist_key.item_key, &dist_val);
//   auto dist_obj = std::make_shared<DataItem>((table_id_t)TPCCTableType::kDistrictTable, dist_key.item_key);
//   dtx->AddToReadWriteSet(dist_obj);

  tpcc_customer_key_t cust_key;
  tpcc_customer_val_t cust_val;
  cust_key.c_id = tpcc_client->MakeCustomerKey(c_w_id, c_d_id, customer_id);
  tpcc_client->GetRecord(cust_key.item_key, &cust_val);

// update customer data
  cust_val.c_balance -= h_amount;
  cust_val.c_ytd_payment += h_amount;
  cust_val.c_payment_cnt += 1;
  if (strcmp(cust_val.c_credit, BAD_CREDIT) == 0) {
    // Bad credit: insert history into c_data
    static const int HISTORY_SIZE = tpcc_customer_val_t::MAX_DATA + 1;
    char history[HISTORY_SIZE];
    int characters = snprintf(history, HISTORY_SIZE, "(%d, %d, %d, %d, %d, %.2f)\n",
                              customer_id, c_d_id, c_w_id, district_id, warehouse_id, h_amount);
    assert(characters < HISTORY_SIZE);

    // Perform the insert with a move and copy
    int current_keep = static_cast<int>(strlen(cust_val.c_data));
    if (current_keep + characters > tpcc_customer_val_t::MAX_DATA) {
      current_keep = tpcc_customer_val_t::MAX_DATA - characters;
    }
    assert(current_keep + characters <= tpcc_customer_val_t::MAX_DATA);
    memmove(cust_val.c_data + characters, cust_val.c_data, current_keep);
    memcpy(cust_val.c_data, history, characters);
    cust_val.c_data[characters + current_keep] = '\0';
    assert(strlen(cust_val.c_data) == characters + current_keep);
  }
  tpcc_client->PutRecord(cust_key.item_key, &cust_val);

// insert history data
  tpcc_history_key_t hist_key;
  tpcc_history_val_t hist_val;
  hist_key.h_id = tpcc_client->MakeHistoryKey(warehouse_id, district_id, c_w_id, c_d_id, customer_id);

  hist_val.h_date = tpcc_client->GetCurrentTimeMillis();  // different time at server and client cause errors?
  hist_val.h_amount = h_amount;
  strcpy(hist_val.h_data, ware_val.w_name);
  strcat(hist_val.h_data, "    ");
  strcat(hist_val.h_data, dist_val.d_name);
  tpcc_client->PutRecord(hist_key.item_key, &hist_val);
  return true;
}
} // end of namespace TPCC

