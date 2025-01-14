//
// rocksdb_impl.cc
//
// Created by Zacharyliu-CS on 09/28/2023.
// Copyright (c) 2023 liuzhenm@mail.ustc.edu.cn.
//

#include "rocksdb_impl.h"
#include <rocksdb/status.h>
#include <string>
#include <valarray>
#include "rocksdb/cache.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/iterator.h"
#include "rocksdb/statistics.h"
#include "rocksdb/table.h"
#include "logging.h"

using namespace TPCC;

RocksDBImpl::RocksDBImpl(std::string dbpath) {

  options.create_if_missing = true;
  options.use_direct_reads = false;
  options.use_direct_io_for_flush_and_compaction = false;
  options.disable_auto_compactions = false;
  options.max_background_compactions = 8;
  options.max_background_jobs = options.max_background_compactions + 4;
  options.write_buffer_size = 64 << 20;
  options.target_file_size_base = 16 << 20;

  // setup the pmem configuration
  std::string pmem_rocksdb_path = dbpath;

  LOG("pm rocksdb path: ", pmem_rocksdb_path);
  // configure for pmem kv sepeartion
  options.wal_dir = pmem_rocksdb_path + "/wal";
  options.dcpmm_kvs_enable = true;
  options.dcpmm_kvs_mmapped_file_fullpath = pmem_rocksdb_path + "/kvs";
  options.dcpmm_kvs_mmapped_file_size = 16*uint64_t(1024*1024*1024);
  options.dcpmm_kvs_value_thres = 64;  // minimal size to do kv sep
  options.dcpmm_compress_value = false;
  options.allow_mmap_reads = true;
  options.allow_mmap_writes = true;
  options.allow_dcpmm_writes = true;

  rocksdb::BlockBasedTableOptions block_options;
  block_options.filter_policy.reset(
      rocksdb::NewBloomFilterPolicy(10));
  block_options.block_cache = rocksdb::NewLRUCache(500<<20);

  options.table_factory.reset(
      rocksdb::NewBlockBasedTableFactory(block_options));
  options.statistics = rocksdb::CreateDBStatistics();
  options.env = rocksdb::NewDCPMMEnv(rocksdb::DCPMMEnvOptions());


  rocksdb::Status s = rocksdb::DB::Open(options, pmem_rocksdb_path, &db_);
  if (!s.ok()) {
    LOG("init rocksdb failed!");
    abort();
  }
}
int RocksDBImpl::Put(uint64_t key, const std::string&value) {
  rocksdb::Status s;
  s = db_->Put(rocksdb::WriteOptions(), std::to_string(key), 
  value);
  if( !s.ok()){
    return -1;
  }
  return 1;
}
int RocksDBImpl::Get(uint64_t key, std::string& value) {
  rocksdb::Status s;
  s = db_->Get(rocksdb::ReadOptions(), std::to_string(key) ,
  &value);
  if( !s.ok()){
    return -1;
  }
  return 1;
}