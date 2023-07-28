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

namespace TPCC {

// reference: https://github.com/evanj/tpccbench/
// Just a container for constants
class Table {
  virtual std::string ConvertByRow() = 0;
  virtual std::vector<std::string> ConvertByColumn() = 0;
};

struct Address : Table {
  // TODO: Embed this structure in warehouse, district, customer? This would
  // reduce some duplication, but would also change the field names
  static const int MIN_STREET = 10;
  static const int MAX_STREET = 20;
  static const int MIN_CITY = 10;
  static const int MAX_CITY = 20;
  static const int STATE = 2;
  static const int ZIP = 9;

  static void copy(char *street1, char *street2, char *city, char *state,
                   char *zip, const char *src_street1, const char *src_street2,
                   const char *src_city, const char *src_state,
                   const char *src_zip);

private:
  Address();
};

namespace Warehouse {
static constexpr float MIN_TAX = 0;
static constexpr float MAX_TAX = 0.2000f;
static constexpr float INITIAL_YTD = 300000.00f;
static const int MIN_NAME = 6;
static const int MAX_NAME = 10;
// TPC-C 1.3.1 (page 11) requires 2*W. This permits testing up to 50
// warehouses. This is an arbitrary limit created to pack ids into integers.
static const int MAX_WAREHOUSE_ID = 100;

struct Key {
  int32_t w_id;
  int32_t unused;
  Key(int32_t w_id_ = 0) : w_id(w_id_), unused(0) {}
};

struct Value {
  float w_tax;
  float w_ytd;
  char w_name[MAX_NAME + 1];
  char w_street_1[Address::MAX_STREET + 1];
  char w_street_2[Address::MAX_STREET + 1];
  char w_city[Address::MAX_CITY + 1];
  char w_state[Address::STATE + 1];
  char w_zip[Address::ZIP + 1];
};
} // namespace Warehouse

namespace District {
static constexpr float MIN_TAX = 0;
static constexpr float MAX_TAX = 0.2000f;
static constexpr float INITIAL_YTD = 30000.00; // different from Warehouse
static const int INITIAL_NEXT_O_ID = 3001;
static const int MIN_NAME = 6;
static const int MAX_NAME = 10;
static const int NUM_PER_WAREHOUSE = 10;

struct Key {
  int32_t d_id;
  int32_t d_w_id;
  Key(int32_t d_id_ = 0, int32_t d_w_id_ = 0) : d_id(d_id_), d_w_id(d_w_id_) {}
};

struct Value {
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
} // namespace District

// YYYY-MM-DD HH:MM:SS This is supposed to be a date/time field from Jan 1st
// 1900 - Dec 31st 2100 with a resolution of 1 second. See TPC-C 1.3.1.
static const int DATETIME_SIZE = 14;

namespace Customer {
static constexpr float INITIAL_CREDIT_LIM = 50000.00;
static constexpr float MIN_DISCOUNT = 0.0000;
static constexpr float MAX_DISCOUNT = 0.5000;
static constexpr float INITIAL_BALANCE = -10.00;
static constexpr float INITIAL_YTD_PAYMENT = 10.00;
static const int INITIAL_PAYMENT_CNT = 1;
static const int INITIAL_DELIVERY_CNT = 0;
static const int MIN_FIRST = 6;
static const int MAX_FIRST = 10;
static const int MIDDLE = 2;
static const int MAX_LAST = 16;
static const int PHONE = 16;
static const int CREDIT = 2;
static const int MIN_DATA = 300;
static const int MAX_DATA = 500;
static const int NUM_PER_DISTRICT = 3000;
static const char GOOD_CREDIT[] = "GC";
static const char BAD_CREDIT[] = "BC";

struct Key {
  int32_t c_d_w_id; // c_d_w_id = customer district_id +
                    //            warehouse_id * NUM_DISTRICT_PER_WAREHOUSE
  int32_t c_id;
  Key(int32_t c_d_w_id_ = 0, int32_t c_id_ = 0)
      : c_d_w_id(c_d_w_id_), c_id(c_id_) {}
};

struct Value {
  int32_t c_d_id;
  int32_t c_w_id;
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
  char c_since[DATETIME_SIZE + 1];
  char c_credit[CREDIT + 1];
  char c_data[MAX_DATA + 1];
};
} // namespace Customer

namespace Stock {
static const int MIN_QUANTITY = 10;
static const int MAX_QUANTITY = 100;
static const int DIST = 24;
static const int MIN_DATA = 26;
static const int MAX_DATA = 50;
static const int NUM_STOCK_PER_WAREHOUSE = 100000;

struct Key {
  int32_t s_i_id;
  int32_t s_w_id;
  Key(int32_t s_i_id_ = 0, int32_t s_w_id_ = 0)
      : s_i_id(s_i_id_), s_w_id(s_w_id_) {}
};

struct Value {
  int32_t s_quantity;
  int32_t s_ytd;
  int32_t s_order_cnt;
  int32_t s_remote_cnt;
  char s_dist[District::NUM_PER_WAREHOUSE][DIST + 1];
  char s_data[MAX_DATA + 1];
};

} // namespace Stock

namespace Item {
static const int MIN_IM = 1;
static const int MAX_IM = 10000;
static constexpr float MIN_PRICE = 1.00;
static constexpr float MAX_PRICE = 100.00;
static const int MIN_NAME = 14;
static const int MAX_NAME = 24;
static const int MIN_DATA = 26;
static const int MAX_DATA = 50;
static const int NUM_ITEMS = 100000;

struct Key {
  int32_t i_id;
  int32_t unused;
  Key(int32_t i_id_ = 0) : i_id(i_id_), unused(0) {}
};

struct Value {
  int32_t i_im_id;
  float i_price;
  char i_name[MAX_NAME + 1];
  char i_data[MAX_DATA + 1];
};
} // namespace Item

namespace Order {
static const int MIN_CARRIER_ID = 1;
static const int MAX_CARRIER_ID = 10;
// HACK: This is not strictly correct, but it works
static const int NULL_CARRIER_ID = 0;
// Less than this value, carrier != null, >= -> carrier == null
static const int NULL_CARRIER_LOWER_BOUND = 2101;
static const int MIN_OL_CNT = 5;
static const int MAX_OL_CNT = 15;
static const int INITIAL_ALL_LOCAL = 1;
static const int INITIAL_ORDERS_PER_DISTRICT = 3000;
// See TPC-C 1.3.1 (page 15)
static const int MAX_ORDER_ID = 10000000;

struct Key {
  int32_t o_d_w_id; // o_d_w_id = customer district_id +
                    //            warehouse_id * NUM_DISTRICT_PER_WAREHOUSE
  int32_t o_id;
};
struct Value {
  int32_t o_c_id;
  int32_t o_carrier_id;
  int32_t o_ol_cnt;
  int32_t o_all_local;
  char o_entry_d[DATETIME_SIZE + 1];
};
} // namespace Order

namespace OrderLine {
static const int MIN_I_ID = 1;
static const int MAX_I_ID = 100000; // Item::NUM_ITEMS
static const int INITIAL_QUANTITY = 5;
static constexpr float MIN_AMOUNT = 0.01f;
static constexpr float MAX_AMOUNT = 9999.99f;
// new order has 10/1000 probability of selecting a remote warehouse for
// ol_supply_w_id
static const int REMOTE_PROBABILITY_MILLIS = 10;

struct Key {
  int32_t ol_o_id;
  int32_t ol_d_id;
  int32_t ol_w_id;
  int32_t ol_number;
};
struct Value {
  int32_t ol_i_id;
  int32_t ol_supply_w_id;
  int32_t ol_quantity;
  float ol_amount;
  char ol_delivery_d[DATETIME_SIZE + 1];
  char ol_dist_info[Stock::DIST + 1];
};
} // namespace OrderLine

namespace NewOrder {
static const int INITIAL_NUM_PER_DISTRICT = 900;

struct Key {
  int32_t no_o_id;
  int32_t unused;
  Key(int32_t no_o_id_ = 0) : no_o_id(no_o_id_) {}
};

struct Value {
  int32_t no_w_id;
  int32_t no_d_id;
};
} // namespace NewOrder

namespace History {

static const int MIN_DATA = 12;
static const int MAX_DATA = 24;
static constexpr float INITIAL_AMOUNT = 10.00f;

struct Key {
  int32_t h_c_id;
  int32_t unused;
  Key(int32_t h_c_id_ = 0) : h_c_id(h_c_id_), unused(0) {}
};
struct Value {
  int32_t h_c_d_id;
  int32_t h_c_w_id;
  int32_t h_d_id;
  int32_t h_w_id;
  float h_amount;
  char h_date[DATETIME_SIZE + 1];
  char h_data[MAX_DATA + 1];
};
} // namespace History

} // namespace TPCC
