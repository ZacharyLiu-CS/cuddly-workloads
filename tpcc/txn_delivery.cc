//
// txn_neworder.cc
//
// Created by Zacharyliu-CS on 09/26/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <iostream>
#include <memory>
#include <set>
#include "schemas.h"
#include "tpcc_txn.h"

namespace TPCC {

bool TPCCTxn::Delivery(TPCCTable* tpcc_client, FastRandom& random_generator) {

  // Generate parameters

  int warehouse_id_start_ = 1;
  int warehouse_id_end_ = tpcc_client->GetNumWarehouse();
  const uint32_t warehouse_id = tpcc_client->PickWarehouseId(
      random_generator, warehouse_id_start_, warehouse_id_end_);
  const int o_carrier_id = tpcc_client->RandomNumber(
      random_generator, tpcc_order_val_t::MIN_CARRIER_ID,
      tpcc_order_val_t::MAX_CARRIER_ID);
  const uint32_t current_ts = tpcc_client->GetCurrentTimeMillis();

  for (int d_id = 1; d_id <= tpcc_client->GetNumDistrictPerWareHouse();
       d_id++) {
    // FIXME: select the lowest NO_O_ID with matching NO_W_ID (equals W_ID) and NO_D_ID (equals D_ID) in the NEW-ORDER table
    int min_o_id =
        tpcc_client->GetNumCustomerPerDistrict() *
            tpcc_new_order_val_t::SCALE_CONSTANT_BETWEEN_NEWORDER_ORDER +
        1;
    int max_o_id = tpcc_client->GetNumCustomerPerDistrict();
    int o_id = tpcc_client->RandomNumber(random_generator, min_o_id, max_o_id);

    int64_t no_key = tpcc_client->MakeNewOrderKey(warehouse_id, d_id, o_id);
    tpcc_new_order_key_t norder_key;
    tpcc_new_order_val_t norder_val;
    norder_key.no_id = no_key;
    tpcc_client->GetRecord(norder_key.item_key, &norder_val);
    // auto norder_obj = std::make_shared<DataItem>((table_id_t)TPCCTableType::kNewOrderTable, norder_key.item_key);
    // dtx->AddToReadOnlySet(norder_obj);

    // Get the new order record with the o_id. Probe if the new order record exists
    // if (!dtx->TxExe(yield, false)) {
    //   dtx->RemoveLastROItem();
    //   continue;
    // }

    // The new order record exists. Remove the new order obj from read only set
    // dtx->RemoveLastROItem();

    // Add the new order obj to read write set to be deleted
    // dtx->AddToReadWriteSet(norder_obj);

    uint64_t o_key = tpcc_client->MakeOrderKey(warehouse_id, d_id, o_id);
    tpcc_order_key_t order_key;
    tpcc_order_val_t order_val;
    order_key.o_id = o_key;
    tpcc_client->GetRecord(order_key.item_key, &order_val);
    // auto order_obj = std::make_shared<DataItem>((table_id_t)TPCCTableType::kOrderTable, order_key.item_key);
    // dtx->AddToReadWriteSet(order_obj);

    // The row in the ORDER table with matching O_W_ID (equals W_ ID), O_D_ID (equals D_ID), and O_ID (equals NO_O_ID) is selected
    // if (!dtx->TxExe(yield)) return false;

    // auto* no_val = (tpcc_new_order_val_t*)norder_obj->value;
    // if (no_val->debug_magic != tpcc_add_magic) {
    //   RDMA_LOG(FATAL) << "[FATAL] Read new order unmatch, tid-cid-txid: " << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    // }

    // norder_obj->valid = 0;  // deleteNewOrder

    // o_entry_d never be 0
    // tpcc_order_val_t* order_val = (tpcc_order_val_t*)order_obj->value;
    // if (order_val->o_entry_d == 0) {
    //   RDMA_LOG(FATAL) << "[FATAL] Read order unmatch, tid-cid-txid: " << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    // }

    // O_C_ID, the customer number, is retrieved
    int32_t customer_id = order_val.o_c_id;

    // O_CARRIER_ID is updated
    order_val.o_carrier_id = o_carrier_id;
    tpcc_client->PutRecord(order_key.item_key, &order_val);

    // All rows in the ORDER-LINE table with matching OL_W_ID (equals O_W_ID), OL_D_ID (equals O_D_ID), and OL_O_ID (equals O_ID) are selected.
    // All OL_DELIVERY_D, the delivery dates, are updated to the current system time
    // The sum of all OL_AMOUNT is retrieved
    float sum_ol_amount = 0;
    for (int line_number = 1; line_number <= tpcc_order_line_val_t::MAX_OL_CNT;
         ++line_number) {
      int64_t ol_key =
          tpcc_client->MakeOrderLineKey(warehouse_id, d_id, o_id, line_number);
      tpcc_order_line_key_t order_line_key;
      tpcc_order_line_val_t order_line_val;
      order_line_key.ol_id = ol_key;
      tpcc_client->GetRecord(order_line_key.item_key, &order_line_val);
      order_line_val.ol_delivery_d = current_ts;
      tpcc_client->PutRecord(order_line_key.item_key, &order_line_val);
      sum_ol_amount += order_line_val.ol_amount;
    }

    // The row in the CUSTOMER table with matching C_W_ID (equals W_ID), C_D_ID (equals D_ID), and C_ID (equals O_C_ID) is selected
    tpcc_customer_key_t cust_key;
    tpcc_customer_val_t cust_val;
    cust_key.c_id =
        tpcc_client->MakeCustomerKey(warehouse_id, d_id, customer_id);
    tpcc_client->GetRecord(cust_key.item_key, &cust_val);
    // C_BALANCE is increased by the sum of all order-line amounts (OL_AMOUNT) previously retrieved
    cust_val.c_balance += sum_ol_amount;
    // C_DELIVERY_CNT is incremented by 1
    cust_val.c_delivery_cnt += 1;
    tpcc_client->PutRecord(cust_key.item_key, &cust_val);
  }
  return true;
}

}  // end of namespace TPCC
