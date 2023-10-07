//
// tables.h
//
// Created by Zacharyliu-CS on 07/13/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//
#pragma once
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

#include "config.h"
#include "utils.h"

using Utils::FastRandom;

namespace TPCC {

// reference: https://github.com/evanj/tpccbench/
// Just a container for constants
static int g_uniform_item_dist = 0;

static int g_new_order_remote_item_pct = 1;

static int g_mico_dist_num = 20;

static const size_t CustomerLastNameMaxSize = 16;

static std::string NameTokens[10] = {
    std::string("BAR"),  std::string("OUGHT"), std::string("ABLE"),
    std::string("PRI"),  std::string("PRES"),  std::string("ESE"),
    std::string("ANTI"), std::string("CALLY"), std::string("ATION"),
    std::string("EING"),
};

const char GOOD_CREDIT[] = "GC";

const char BAD_CREDIT[] = "BC";

static const int DUMMY_SIZE = 12;

static const int DIST = 24;

// Constants
struct Address {
  static const int MIN_STREET = 10; // W_STREET_1 random a-string [10 .. 20]
                                    // W_STREET_2 random a-string [10 .. 20]
  static const int MAX_STREET = 20;
  static const int MIN_CITY = 10; // W_CITY random a-string [10 .. 20]
  static const int MAX_CITY = 20;
  static const int STATE = 2; // W_STATE random a-string of 2 letters
  static const int ZIP = 9;   // ZIP a-string of 9 letters
};

/******************** TPCC table definitions (Schemas of key and value) start
 * **********************/
/*
 * Warehouse table
 * Primary key: <int32_t w_id>
 */

union tpcc_warehouse_key_t {
  struct {
    int32_t w_id;
    uint8_t unused[4];
  };
  itemkey_t item_key;

  tpcc_warehouse_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_warehouse_key_t) == sizeof(itemkey_t), "");

struct tpcc_warehouse_val_t {
  static const int MIN_NAME = 6;
  static const int MAX_NAME = 10;

  float w_tax;
  float w_ytd;
  char w_name[MAX_NAME + 1];
  char w_street_1[Address::MAX_STREET + 1];
  char w_street_2[Address::MAX_STREET + 1];
  char w_city[Address::MAX_CITY + 1];
  char w_state[Address::STATE + 1];
  char w_zip[Address::ZIP + 1];
};

static_assert(sizeof(tpcc_warehouse_val_t) == 96, "");

/*
 * District table
 * Primary key: <int32_t d_id, int32_t d_w_id>
 */

union tpcc_district_key_t {
  struct {
    int64_t d_id;
  };
  itemkey_t item_key;

  tpcc_district_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_district_key_t) == sizeof(itemkey_t), "");

struct tpcc_district_val_t {
  static const int MIN_NAME = 6;
  static const int MAX_NAME = 10;

  float d_tax;
  float d_ytd;
  int32_t d_next_o_id;
  char d_name[MAX_NAME + 1];
  char d_street_1[Address::MAX_STREET + 1];
  char d_street_2[Address::MAX_STREET + 1];
  char d_city[Address::MAX_CITY + 1];
  char d_state[Address::STATE + 1];
  char d_zip[Address::ZIP + 1];
};

static_assert(sizeof(tpcc_district_val_t) == 100, "");

/*
 * Customer table
 * Primary key: <int32_t c_id, int32_t c_d_id, int32_t c_w_id>
 */

union tpcc_customer_key_t {
  int64_t c_id;
  itemkey_t item_key;

  tpcc_customer_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_customer_key_t) == sizeof(itemkey_t), "");

struct tpcc_customer_val_t {
  static const int MIN_FIRST = 8; // C_FIRST random a-string [8 .. 16]
  static const int MAX_FIRST = 16;
  static const int MIDDLE = 2;
  static const int MAX_LAST = 16;
  static const int PHONE = 16; // C_PHONE random n-string of 16 numbers
  static const int CREDIT = 2;
  static const int MIN_DATA = 300; // C_DATA random a-string [300 .. 500]
  static const int MAX_DATA = 500;

  float c_credit_lim;
  float c_discount;
  float c_balance;
  float c_ytd_payment;
  int32_t c_payment_cnt;
  int32_t c_delivery_cnt;
  char c_first[MAX_FIRST + 1];
  char c_middle[MIDDLE + 1];
  char c_last[MAX_LAST + 1];
  char c_street_1[Address::MAX_STREET + 1];
  char c_street_2[Address::MAX_STREET + 1];
  char c_city[Address::MAX_CITY + 1];
  char c_state[Address::STATE + 1];
  char c_zip[Address::ZIP + 1];
  char c_phone[PHONE + 1];
  uint32_t c_since;
  char c_credit[CREDIT + 1];
  char c_data[MAX_DATA + 1];
};

static_assert(sizeof(tpcc_customer_val_t) == 664, "");

union tpcc_customer_index_key_t {
  struct {
    uint64_t c_index_id;
  };
  itemkey_t item_key;

  tpcc_customer_index_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_customer_index_key_t) == sizeof(itemkey_t), "");

struct tpcc_customer_index_val_t {
  int64_t c_id;
  int64_t debug_magic;
};

static_assert(sizeof(tpcc_customer_index_val_t) == 16, ""); // add debug magic
// static_assert(sizeof(tpcc_customer_index_val_t) == 8, "");

/*
 * History table
 * Primary key: none
 */

union tpcc_history_key_t {
  int64_t h_id;
  itemkey_t item_key;

  tpcc_history_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_history_key_t) == sizeof(itemkey_t), "");

struct tpcc_history_val_t {
  static const int MIN_DATA =
      12; // H_DATA random a-string [12 .. 24] from TPCC documents 5.11
  static const int MAX_DATA = 24;

  float h_amount;
  uint32_t h_date;
  char h_data[MAX_DATA + 1];
};

static_assert(sizeof(tpcc_history_val_t) == 36, "");

/*
 * NewOrder table
 * Primary key: <int32_t no_w_id, int32_t no_d_id, int32_t no_o_id>
 */
union tpcc_new_order_key_t {
  int64_t no_id;
  itemkey_t item_key;

  tpcc_new_order_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_new_order_key_t) == sizeof(itemkey_t), "");

struct tpcc_new_order_val_t {
  static constexpr double SCALE_CONSTANT_BETWEEN_NEWORDER_ORDER = 0.7;

  char no_dummy[DUMMY_SIZE + 1];
  int64_t debug_magic;
};

static_assert(sizeof(tpcc_new_order_val_t) == 24, ""); // add debug magic
// static_assert(sizeof(tpcc_new_order_val_t) == 13, "");

/*
 * Order table
 * Primary key: <int32_t o_w_id, int32_t o_d_id, int32_t o_id>
 */
union tpcc_order_key_t {
  int64_t o_id;
  itemkey_t item_key;

  tpcc_order_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_order_key_t) == sizeof(itemkey_t), "");

struct tpcc_order_val_t {
  static const int MIN_CARRIER_ID = 1;
  static const int MAX_CARRIER_ID = 10; // number of distinct per warehouse

  int32_t o_c_id;
  int32_t o_carrier_id;
  int32_t o_ol_cnt;
  int32_t o_all_local;
  uint32_t o_entry_d;
};

static_assert(sizeof(tpcc_order_val_t) == 20, "");

union tpcc_order_index_key_t {
  int64_t o_index_id;
  itemkey_t item_key;

  tpcc_order_index_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_order_index_key_t) == sizeof(itemkey_t), "");

struct tpcc_order_index_val_t {
  uint64_t o_id;
  int64_t debug_magic;
};

static_assert(sizeof(tpcc_order_index_val_t) == 16, ""); // add debug magic
// static_assert(sizeof(tpcc_order_index_val_t) == 8, "");

/*
 * OrderLine table
 * Primary key: <int32_t ol_o_id, int32_t ol_d_id, int32_t ol_w_id, int32_t
 * ol_number>
 */

union tpcc_order_line_key_t {
  int64_t ol_id;
  itemkey_t item_key;

  tpcc_order_line_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_order_line_key_t) == sizeof(itemkey_t), "");

struct tpcc_order_line_val_t {
  static const int MIN_OL_CNT = 5;
  static const int MAX_OL_CNT = 15;

  int32_t ol_i_id;
  int32_t ol_supply_w_id;
  int32_t ol_quantity;
  float ol_amount;
  uint32_t ol_delivery_d;
  char ol_dist_info[DIST + 1];
  int64_t debug_magic;
};

static_assert(sizeof(tpcc_order_line_val_t) == 56, ""); // add debug magic
// static_assert(sizeof(tpcc_order_line_val_t) == 48, "");

/*
 * Item table
 * Primary key: <int32_t i_id>
 */

union tpcc_item_key_t {
  struct {
    int64_t i_id;
  };
  itemkey_t item_key;

  tpcc_item_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_item_key_t) == sizeof(itemkey_t), "");

struct tpcc_item_val_t {
  static const int MIN_NAME = 14; // I_NAME random a-string [14 .. 24]
  static const int MAX_NAME = 24;
  static const int MIN_DATA = 26;
  static const int MAX_DATA = 50; // I_DATA random a-string [26 .. 50]

  static const int MIN_IM = 1;
  static const int MAX_IM = 10000;

  int32_t i_im_id;
  float i_price;
  char i_name[MAX_NAME + 1];
  char i_data[MAX_DATA + 1];
  int64_t debug_magic;
};

static_assert(sizeof(tpcc_item_val_t) == 96, ""); // add debug magic
// static_assert(sizeof(tpcc_item_val_t) == 84, "");

/*
 * Stock table
 * Primary key: <int32_t s_i_id, int32_t s_w_id>
 */

union tpcc_stock_key_t {
  struct {
    int64_t s_id;
  };
  itemkey_t item_key;

  tpcc_stock_key_t() { item_key = 0; }
};

static_assert(sizeof(tpcc_stock_key_t) == sizeof(itemkey_t), "");

struct tpcc_stock_val_t {
  static const int MIN_DATA = 26;
  static const int MAX_DATA = 50;
  static const int32_t MIN_STOCK_LEVEL_THRESHOLD = 10;
  static const int32_t MAX_STOCK_LEVEL_THRESHOLD = 20;
  static const int STOCK_LEVEL_ORDERS = 20;

  int32_t s_quantity;
  int32_t s_ytd;
  int32_t s_order_cnt;
  int32_t s_remote_cnt;
  char s_dist[NUM_DISTRICT_PER_WAREHOUSE][DIST + 1];
  char s_data[MAX_DATA + 1];
  int64_t debug_magic;
};

static_assert(sizeof(tpcc_stock_val_t) == 328, ""); // add debug magic
// static_assert(sizeof(tpcc_stock_val_t) == 320, "");


} // end of namespace TPCC