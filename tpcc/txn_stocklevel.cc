//
// txn_stocklevel.cc
//
// Created by Zacharyliu-CS on 09/26/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include <cassert>
#include <iostream>
#include <memory>
#include <set>
#include <algorithm>
#include "schemas.h"
#include "tpcc_txn.h"

namespace TPCC {

bool TPCCTxn::StockLevel(TPCCTable* tpcc_client, FastRandom& random_generator) {

  int32_t threshold = tpcc_client->RandomNumber(
      random_generator, tpcc_stock_val_t::MIN_STOCK_LEVEL_THRESHOLD,
      tpcc_stock_val_t::MAX_STOCK_LEVEL_THRESHOLD);

  int warehouse_id_start_ = 1;
  int warehouse_id_end_ = tpcc_client->GetNumWarehouse();

  int district_id_start = 1;
  int district_id_end_ = tpcc_client->GetNumDistrictPerWareHouse();

  const uint32_t warehouse_id = tpcc_client->PickWarehouseId(
      random_generator, warehouse_id_start_, warehouse_id_end_);
  const uint32_t district_id = tpcc_client->RandomNumber(
      random_generator, district_id_start, district_id_end_);

  uint64_t d_key = tpcc_client->MakeDistrictKey(warehouse_id, district_id);
  tpcc_district_key_t dist_key;
  tpcc_district_val_t dist_val;
  dist_key.d_id = d_key;
  tpcc_client->GetRecord(dist_key.item_key, &dist_val);



  int32_t o_id = dist_val.d_next_o_id;

  std::vector<int32_t> s_i_ids;
  s_i_ids.reserve(300);

  // Iterate over [o_id-20, o_id)
  for (int order_id = o_id - tpcc_stock_val_t::STOCK_LEVEL_ORDERS;
       order_id < o_id; ++order_id) {
    // Populate line_numer is random: [Min_OL_CNT, MAX_OL_CNT)
    for (int line_number = 1; line_number <= tpcc_order_line_val_t::MAX_OL_CNT;
         ++line_number) {
      int64_t ol_key = tpcc_client->MakeOrderLineKey(warehouse_id, district_id,
                                                     order_id, line_number);
      tpcc_order_line_key_t order_line_key;
      tpcc_order_line_val_t order_line_val;
      order_line_key.ol_id = ol_key;
      auto status =
          tpcc_client->GetRecord(order_line_key.item_key, &order_line_val);
      //   auto ol_obj = std::make_shared<DataItem>(
      //       (table_id_t)TPCCTableType::kOrderLineTable, order_line_key.item_key);
      //   dtx->AddToReadOnlySet(ol_obj);

      if (status == -1) {
        break;
      }

      //   tpcc_order_line_val_t* ol_val = (tpcc_order_line_val_t*)ol_obj->value;
      //   if (ol_val->debug_magic != tpcc_add_magic) {
      //     RDMA_LOG(FATAL) << "[FATAL] Read order line unmatch, tid-cid-txid: "
      //                     << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
      //   }

      int64_t s_key = tpcc_client->MakeStockKey(warehouse_id, order_line_val.ol_i_id);
      tpcc_stock_key_t stock_key;
      tpcc_stock_val_t stock_val;
      stock_key.s_id = s_key;
      tpcc_client->GetRecord(stock_key.item_key, &stock_val);
      //   auto stock_obj = std::make_shared<DataItem>(
      //   (table_id_t)TPCCTableType::kStockTable, stock_key.item_key);
      //   dtx->AddToReadOnlySet(stock_obj);

      //   if (!dtx->TxExe(yield))
      //     return false;

    //   tpcc_stock_val_t* stock_val = (tpcc_stock_val_t*)stock_obj->value;
    //   if (stock_val->debug_magic != tpcc_add_magic) {
    //     RDMA_LOG(FATAL) << "[FATAL] Read stock unmatch, tid-cid-txid: "
    //                     << dtx->t_id << "-" << dtx->coro_id << "-" << tx_id;
    //   }

      if (stock_val.s_quantity < threshold) {
        s_i_ids.push_back(order_line_val.ol_i_id);
      }
    }
  }

  // Filter out duplicate s_i_id: multiple order lines can have the same item
  // In O3, this code may be optimized since num_distinct is not outputed.
  std::sort(s_i_ids.begin(), s_i_ids.end());
  int num_distinct = 0;  // The output of this transaction
  int32_t last = -1;     // -1 is an invalid s_i_id
  for (size_t i = 0; i < s_i_ids.size(); ++i) {
    if (s_i_ids[i] != last) {
      last = s_i_ids[i];
      num_distinct += 1;
    }
  }

  return true;
}
}  // end of namespace TPCC
