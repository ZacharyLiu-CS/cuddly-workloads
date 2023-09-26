//
// tpcc_txn.cc
//
// Created by Zacharyliu-CS on 09/26/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include "tpcc_txn.h"
#include <iostream>
#include <set>
#include <memory>

namespace TPCC {

bool TPCCTxn::NewOrder(TPCCTable *tpcc_client, FastRandom &random_generator) {

  // Generate parameters

  int warehouse_id_start_ = 1;
  int warehouse_id_end_ = tpcc_client->GetNumWarehouse();

  int district_id_start = 1;
  int district_id_end_ = tpcc_client->GetNumDistrictPerWareHouse();

  const uint32_t warehouse_id = tpcc_client->PickWarehouseId(
      random_generator, warehouse_id_start_, warehouse_id_end_);
  const uint32_t district_id = tpcc_client->RandomNumber(
      random_generator, district_id_start, district_id_end_);
  const uint32_t customer_id =
      tpcc_client->GetCustomerId(random_generator);
  int64_t c_key =
      tpcc_client->MakeCustomerKey(warehouse_id, district_id, customer_id);

  int32_t all_local = 1;
  std::set<uint64_t> stock_set; // remove identity stock ids;

  // local buffer used store stocks
  int64_t remote_stocks[tpcc_order_line_val_t::MAX_OL_CNT],
      local_stocks[tpcc_order_line_val_t::MAX_OL_CNT];
  int64_t remote_item_ids[tpcc_order_line_val_t::MAX_OL_CNT],
      local_item_ids[tpcc_order_line_val_t::MAX_OL_CNT];
  uint32_t local_supplies[tpcc_order_line_val_t::MAX_OL_CNT],
      remote_supplies[tpcc_order_line_val_t::MAX_OL_CNT];

  int num_remote_stocks(0), num_local_stocks(0);

  const int num_items = tpcc_client->RandomNumber(
      random_generator, tpcc_order_line_val_t::MIN_OL_CNT,
      tpcc_order_line_val_t::MAX_OL_CNT);

  for (int i = 0; i < num_items; i++) {
    int64_t item_id = tpcc_client->GetItemId(random_generator);
    if (tpcc_client->GetNumWarehouse() == 1 ||
        tpcc_client->RandomNumber(random_generator, 1, 100) >
            g_new_order_remote_item_pct) {
      // local stock case
      uint32_t supplier_warehouse_id = warehouse_id;
      int64_t s_key = tpcc_client->MakeStockKey(supplier_warehouse_id, item_id);
      if (stock_set.find(s_key) != stock_set.end()) {
        i--;
        continue;
      } else {
        stock_set.insert(s_key);
      }
      local_supplies[num_local_stocks] = supplier_warehouse_id;
      local_item_ids[num_local_stocks] = item_id;
      local_stocks[num_local_stocks++] = s_key;
    } else {
      // remote stock case
      int64_t s_key;
      uint32_t supplier_warehouse_id;
      do {
        supplier_warehouse_id = tpcc_client->RandomNumber(
            random_generator, 1, tpcc_client->GetNumWarehouse());
      } while (supplier_warehouse_id == warehouse_id);

      all_local = 0;

      s_key = tpcc_client->MakeStockKey(supplier_warehouse_id, item_id);
      if (stock_set.find(s_key) != stock_set.end()) {
        i--;
        continue;
      } else {
        stock_set.insert(s_key);
      }
      remote_stocks[num_remote_stocks] = s_key;
      remote_supplies[num_remote_stocks] = supplier_warehouse_id;
      remote_item_ids[num_remote_stocks++] = item_id;
    }
  }

  // Run

  tpcc_warehouse_key_t ware_key;
  ware_key.w_id = warehouse_id;
  auto ware_obj = std::make_shared<DataItem>(
      (table_id_t)TPCCTableType::kWarehouseTable, ware_key.item_key);
  dtx->AddToReadOnlySet(ware_obj);

  tpcc_customer_key_t cust_key;
  cust_key.c_id = c_key;
  auto cust_obj = std::make_shared<DataItem>(
      (table_id_t)TPCCTableType::kCustomerTable, cust_key.item_key);
  dtx->AddToReadOnlySet(cust_obj);

  // read and update district value
  uint64_t d_key = tpcc_client->MakeDistrictKey(warehouse_id, district_id);
  tpcc_district_key_t dist_key;
  dist_key.d_id = d_key;
  auto dist_obj = std::make_shared<DataItem>(
      (table_id_t)TPCCTableType::kDistrictTable, dist_key.item_key);
  dtx->AddToReadWriteSet(dist_obj);

  if (!dtx->TxExe(yield))
    return false;

  auto *ware_val = (tpcc_warehouse_val_t *)ware_obj->value;
  std::string check(ware_val->w_zip);


  auto *cust_val = (tpcc_customer_val_t *)cust_obj->value;
  tpcc_district_val_t *dist_val = (tpcc_district_val_t *)dist_obj->value;
  check = std::string(dist_val->d_zip);
  const auto my_next_o_id = dist_val->d_next_o_id;

  dist_val->d_next_o_id++;

  // insert neworder record
  uint64_t no_key =
      tpcc_client->MakeNewOrderKey(warehouse_id, district_id, my_next_o_id);
  tpcc_new_order_key_t norder_key;
  norder_key.no_id = no_key;
  auto norder_obj = std::make_shared<DataItem>(
      (table_id_t)TPCCTableType::kNewOrderTable, sizeof(tpcc_new_order_val_t),
      norder_key.item_key, 1);
  // insert order record
  uint64_t o_key =
      tpcc_client->MakeOrderKey(warehouse_id, district_id, my_next_o_id);
  tpcc_order_key_t order_key;
  order_key.o_id = o_key;
  auto order_obj = std::make_shared<DataItem>(
      (table_id_t)TPCCTableType::kOrderTable, sizeof(tpcc_order_val_t),
      order_key.item_key, tx_id, 1);
  dtx->AddToReadWriteSet(order_obj);

  // insert order index record
  uint64_t o_index_key = tpcc_client->MakeOrderIndexKey(
      warehouse_id, district_id, customer_id, my_next_o_id);
  tpcc_order_index_key_t order_index_key;
  order_index_key.o_index_id = o_index_key;
  auto oidx_obj = std::make_shared<DataItem>(
      (table_id_t)TPCCTableType::kOrderIndexTable,
      sizeof(tpcc_order_index_val_t), order_index_key.item_key, tx_id, 1);
  dtx->AddToReadWriteSet(oidx_obj);

  if (!dtx->TxExe(yield))
    return false;

  // Respectively assign values
  tpcc_new_order_val_t *norder_val = (tpcc_new_order_val_t *)norder_obj->value;
  norder_val->debug_magic = tpcc_add_magic;

  tpcc_order_val_t *order_val = (tpcc_order_val_t *)order_obj->value;
  order_val->o_c_id = int32_t(customer_id);
  order_val->o_carrier_id = 0;
  order_val->o_ol_cnt = num_items;
  order_val->o_all_local = all_local;
  order_val->o_entry_d = tpcc_client->GetCurrentTimeMillis();

  tpcc_order_index_val_t *oidx_val = (tpcc_order_index_val_t *)oidx_obj->value;
  oidx_val->o_id = o_key;
  oidx_val->debug_magic = tpcc_add_magic;

  // -----------------------------------------------------------------------------
  for (int ol_number = 1; ol_number <= num_local_stocks; ol_number++) {
    const int64_t ol_i_id = local_item_ids[ol_number - 1];
    const uint32_t ol_quantity =
        tpcc_client->RandomNumber(random_generator, 1, 10);
    // read item info
    tpcc_item_key_t tpcc_item_key;
    tpcc_item_key.i_id = ol_i_id;

    auto item_obj = std::make_shared<DataItem>(
        (table_id_t)TPCCTableType::kItemTable, tpcc_item_key.item_key);
    dtx->AddToReadOnlySet(item_obj);

    int64_t s_key = local_stocks[ol_number - 1];
    // read and update stock info
    tpcc_stock_key_t stock_key;
    stock_key.s_id = s_key;

    auto stock_obj = std::make_shared<DataItem>(
        (table_id_t)TPCCTableType::kStockTable, stock_key.item_key);
    dtx->AddToReadWriteSet(stock_obj);

    if (!dtx->TxExe(yield))
      return false;

    tpcc_item_val_t *item_val = (tpcc_item_val_t *)item_obj->value;
    tpcc_stock_val_t *stock_val = (tpcc_stock_val_t *)stock_obj->value;

    if (item_val->debug_magic != tpcc_add_magic) {
      RDMA_LOG(FATAL) << "[FATAL] Read item unmatch, tid-cid-txid: "
                      << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    }
    if (stock_val->debug_magic != tpcc_add_magic) {
      RDMA_LOG(FATAL) << "[FATAL] Read stock unmatch, tid-cid-txid: "
                      << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    }

    if (stock_val->s_quantity - ol_quantity >= 10) {
      stock_val->s_quantity -= ol_quantity;
    } else {
      stock_val->s_quantity += (-int32_t(ol_quantity) + 91);
    }

    stock_val->s_ytd += ol_quantity;
    stock_val->s_remote_cnt +=
        (local_supplies[ol_number - 1] == warehouse_id) ? 0 : 1;

    // insert order line record
    int64_t ol_key = tpcc_client->MakeOrderLineKey(warehouse_id, district_id,
                                                   my_next_o_id, ol_number);
    tpcc_order_line_key_t order_line_key;
    order_line_key.ol_id = ol_key;
    // RDMA_LOG(DBG) << warehouse_id << " " << district_id << " " <<
    // my_next_o_id << " " <<  ol_number << ". ol_key: " << ol_key;
    auto ol_obj = std::make_shared<DataItem>(
        (table_id_t)TPCCTableType::kOrderLineTable,
        sizeof(tpcc_order_line_val_t), order_line_key.item_key, tx_id, 1);
    dtx->AddToReadWriteSet(ol_obj);

    if (!dtx->TxExe(yield))
      return false;

    tpcc_order_line_val_t *order_line_val =
        (tpcc_order_line_val_t *)ol_obj->value;

    order_line_val->ol_i_id = int32_t(ol_i_id);
    order_line_val->ol_delivery_d = 0; // not delivered yet
    order_line_val->ol_amount = float(ol_quantity) * item_val->i_price;
    order_line_val->ol_supply_w_id = int32_t(local_supplies[ol_number - 1]);
    order_line_val->ol_quantity = int8_t(ol_quantity);
    order_line_val->debug_magic = tpcc_add_magic;
  }

  for (int ol_number = 1; ol_number <= num_remote_stocks; ol_number++) {
    const int64_t ol_i_id = remote_item_ids[ol_number - 1];
    const uint32_t ol_quantity =
        tpcc_client->RandomNumber(random_generator, 1, 10);
    // read item info
    tpcc_item_key_t tpcc_item_key;
    tpcc_item_key.i_id = ol_i_id;

    auto item_obj = std::make_shared<DataItem>(
        (table_id_t)TPCCTableType::kItemTable, tpcc_item_key.item_key);
    dtx->AddToReadOnlySet(item_obj);

    int64_t s_key = remote_stocks[ol_number - 1];
    // read and update stock info
    tpcc_stock_key_t stock_key;
    stock_key.s_id = s_key;

    auto stock_obj = std::make_shared<DataItem>(
        (table_id_t)TPCCTableType::kStockTable, stock_key.item_key);
    dtx->AddToReadWriteSet(stock_obj);

    if (!dtx->TxExe(yield))
      return false;

    tpcc_item_val_t *item_val = (tpcc_item_val_t *)item_obj->value;
    tpcc_stock_val_t *stock_val = (tpcc_stock_val_t *)stock_obj->value;

    if (item_val->debug_magic != tpcc_add_magic) {
      RDMA_LOG(FATAL) << "[FATAL] Read item unmatch, tid-cid-txid: "
                      << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    }
    if (stock_val->debug_magic != tpcc_add_magic) {
      RDMA_LOG(FATAL) << "[FATAL] Read stock unmatch, tid-cid-txid: "
                      << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    }

    if (stock_val->s_quantity - ol_quantity >= 10) {
      stock_val->s_quantity -= ol_quantity;
    } else {
      stock_val->s_quantity += (-int32_t(ol_quantity) + 91);
    }

    stock_val->s_ytd += ol_quantity;
    stock_val->s_remote_cnt +=
        (remote_supplies[ol_number - 1] == warehouse_id) ? 0 : 1;

    // insert order line record
    int64_t ol_key = tpcc_client->MakeOrderLineKey(
        warehouse_id, district_id, my_next_o_id, num_local_stocks + ol_number);
    tpcc_order_line_key_t order_line_key;
    order_line_key.ol_id = ol_key;
    // RDMA_LOG(DBG) << warehouse_id << " " << district_id << " " <<
    // my_next_o_id << " " <<  num_local_stocks + ol_number << ". ol_key: " <<
    // ol_key;
    auto ol_obj = std::make_shared<DataItem>(
        (table_id_t)TPCCTableType::kOrderLineTable,
        sizeof(tpcc_order_line_val_t), order_line_key.item_key, tx_id, 1);
    dtx->AddToReadWriteSet(ol_obj);
    if (!dtx->TxExe(yield))
      return false;

    tpcc_order_line_val_t *order_line_val =
        (tpcc_order_line_val_t *)ol_obj->value;

    order_line_val->ol_i_id = int32_t(ol_i_id);
    order_line_val->ol_delivery_d = 0; // not delivered yet
    order_line_val->ol_amount = float(ol_quantity) * item_val->i_price;
    order_line_val->ol_supply_w_id = int32_t(remote_supplies[ol_number - 1]);
    order_line_val->ol_quantity = int8_t(ol_quantity);
    order_line_val->debug_magic = tpcc_add_magic;
  }

  return true;
}
bool TPCCTxn::Payment(TPCCTable *tpcc_client, FastRandom *random_generator) {}
bool TPCCTxn::Delivery(TPCCTable *tpcc_client, FastRandom *random_generator) {}
bool TPCCTxn::OrderStatus(TPCCTable *tpcc_client,
                          FastRandom *random_generator) {}
bool TPCCTxn::StockLevel(TPCCTable *tpcc_client, FastRandom *random_generator) {
}
} // end of namespace TPCC
